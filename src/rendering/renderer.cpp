#include "renderer.h"
#include "settings.h"
#include "game.h"
#include "sceneGraphNode.h"

void Renderer::init(int width, int height) {
	gouraudShader = Shader("./src/shaders/gouraud.vert", "./src/shaders/gouraud.frag");
	phongShader = Shader("./src/shaders/phong.vert", "./src/shaders/phong.frag");
	pikachu = Model("./resources/models/pikachu/Pikachu.obj", true);
	pokeball = Model("./resources/models/ball/ball.obj", true);
	map = Model("./resources/models/box/box.obj", false);
	isPhong = false;
	useTexture = false;
	textRenderer = TextRenderer(width, height);
	textRenderer.Load("./resources/fonts/OCRAEXT.TTF", 48);

	gouraudShader.use();

	gouraudShader.setVec3("pointLight.ambient", glm::vec3(0.2f));
	gouraudShader.setVec3("pointLight.diffuse", glm::vec3(0.5f));
	gouraudShader.setVec3("pointLight.specular", glm::vec3(1.0f));
	gouraudShader.setFloat("pointLight.constant", 1.0f);
	gouraudShader.setFloat("pointLight.linear", 0.027f);
	gouraudShader.setFloat("pointLight.quadratic", 0.0028f);

	gouraudShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
	gouraudShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
	gouraudShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	gouraudShader.setFloat("material.shininess", 32.0f);

	phongShader.use();

	phongShader.setVec3("pointLight.ambient", glm::vec3(0.2f));
	phongShader.setVec3("pointLight.diffuse", glm::vec3(0.5f));
	phongShader.setVec3("pointLight.specular", glm::vec3(1.0f));
	phongShader.setFloat("pointLight.constant", 1.0f);
	phongShader.setFloat("pointLight.linear", 0.045f);
	phongShader.setFloat("pointLight.quadratic", 0.0075f);

	phongShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
	phongShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
	phongShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
	phongShader.setFloat("material.shininess", 32.0f);
}

void Renderer::setScreenSize(int _width, int _height) {
	width = _width;
	height = _height;
	glViewport(0, 0, _width, _height);
	textRenderer.setScreenSize(_width, _height);
}

glm::vec3 Renderer::getCameraPosition(MovableCubeObject& player1, ViewMode viewmode, CameraForViewThree& cameraForViewThree) {
	glm::vec3 position;
	if (viewmode == VIEW_CHARACTER_EYE)
		position = player1.getPosition() + (player1.getSize() / 2.0f) + (player1.getSize().x / 2.0f) * player1.getDirectionVector();
	else if (viewmode == VIEW_CHARACTER_BACK)
		position = player1.getPosition() + (player1.getSize() / 2.0f) - player1.getSize().x * 2 * player1.getDirectionVector() + glm::vec3(0.0f, 0.0f, 10.0f);
	else if (viewmode == VIEW_CEILING)
		position = glm::vec3(cameraForViewThree.getPosition().x, cameraForViewThree.getPosition().y, MAP_SIZE.z * 3.0f);

	return position;
}

glm::mat4 Renderer::getViewMatrix(MovableCubeObject& player1, ViewMode viewmode, CameraForViewThree& cameraForViewThree) {
	glm::vec3 eye, center, up;

	eye = getCameraPosition(player1, viewmode, cameraForViewThree);

	if (viewmode == VIEW_CHARACTER_EYE) {
		center = eye + player1.getDirectionVector();
		up = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else if (viewmode == VIEW_CHARACTER_BACK) {
		center = eye + player1.getDirectionVector() + glm::vec3(0.0f, 0.0f, -0.1f);
		up = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	else if (viewmode == VIEW_CEILING) {
		center = glm::vec3(MAP_SIZE.x / 2.0f, MAP_SIZE.y / 2.0f, 0.0f);
		up = glm::vec3(1.0f, 0.0f, 0.0f);
	}

	return glm::lookAt(eye, center, up);
}

glm::mat4 Renderer::makePikachuModelMatrix(MovableCubeObject& player, bool isPlayer1) {
	float playerDirectionAngle = player.getDirectionAngle().x;
	if (!isPlayer1)
		playerDirectionAngle = glm::radians(180.0f) - playerDirectionAngle;

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 model = glm::mat4(1.0f);
	// finally, get pikachu to its position
	model = glm::translate(model, player.getPosition());
	// fourth, rotate pikachu to look at the right direction
	model = glm::translate(model, +player.getSize() / 2.0f);
	model = glm::rotate(model, playerDirectionAngle, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, -player.getSize() / 2.0f);
	// third, apply scaling to match model's size to bounding box (should be done after rotating model)
	model = glm::scale(model, player.getSize() / glm::vec3(rotation * glm::vec4(pikachu.getSize(), 1.0f)));
	// second, rotate pikachu so that the head towards to the positive z-axis
	model = model * rotation;
	// first, align pikachu's left-bottom-back point to origin
	model = glm::translate(model, -pikachu.getMin());

	return model;
}

glm::mat4 Renderer::makePokeballModelMatrix(BallObject& ball) {
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = glm::rotate(rotation, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::vec3 normalized_velocity = ball.getVelocity() / sqrt(glm::dot(ball.getVelocity(), ball.getVelocity()));
	float theta = acos(glm::dot(normalized_velocity, glm::vec3(1.0f, 0.0f, 0.0f)));
	theta = ball.getVelocity().y > 0 ? theta : -theta;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, ball.getPosition());
	model = glm::translate(model, +glm::vec3(ball.getRadius()));
	model = glm::rotate(model, theta, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, -glm::vec3(ball.getRadius()));
	model = glm::scale(model, glm::vec3(ball.getRadius() * 2) / pokeball.getSize());
	model = model * rotation;
	model = glm::translate(model, -pokeball.getMin());

	return model;
}

glm::mat4 Renderer::makeMapModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, MAP_SIZE / map.getSize());
	model = glm::translate(model, -map.getMin());

	return model;
}

void Renderer::renderScene(MovableCubeObject& player1, MovableCubeObject& player2, BallObject& ball, ViewMode viewmode, CameraForViewThree& cameraForViewThree, int degree) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader& shader = isPhong ? phongShader : gouraudShader;

	shader.use();
	shader.setBool("useTexture", useTexture);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
	glm::mat4 view = getViewMatrix(player1, viewmode, cameraForViewThree);
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);

	shader.setVec3("pointLight.position", ball.getPosition() + glm::vec3(ball.getRadius()) + glm::vec3(0.0f, 0.0f, 10.0f));
	shader.setVec3("viewPos", getCameraPosition(player1, viewmode, cameraForViewThree));

	float radian = glm::radians((float)(degree % 180));
	float dayOrNight = degree % 360 < 180 ? 1.0f : 0.2f;

	shader.setVec3("dirLight.direction", glm::vec3(0.0f, -glm::cos(radian), -glm::sin(radian)));
	shader.setVec3("dirLight.ambient", glm::vec3(0.2f));
	shader.setVec3("dirLight.diffuse", glm::vec3(0.5f * dayOrNight * glm::sin(radian)));
	shader.setVec3("dirLight.specular", glm::vec3(1.0f * dayOrNight * glm::sin(radian)));

	SceneGraphNode* sceneGraph =
		new SceneGraphNode(makeMapModelMatrix() , &map, glm::vec3(1.0f), useNormalMap,
			nullptr,
			new SceneGraphNode(makePikachuModelMatrix(player1, true), &pikachu, glm::vec3(1.0f, 1.0f, 0.0f), false,
				nullptr,
				new SceneGraphNode(makePikachuModelMatrix(player2, false), &pikachu, glm::vec3(1.0f, 1.0f, 0.0f), false,
					nullptr,
					new SceneGraphNode(makePokeballModelMatrix(ball), &pokeball, glm::vec3(1.0f, 0.0f, 0.0f), false,
						nullptr,
						nullptr
					)
				)
			)
		);

	sceneGraph->traverse(shader, glm::mat4(1.0f));

	delete sceneGraph;
}

void Renderer::renderText(ViewMode viewmode, GameState gamestate, int score1, int score2, int delayTime) {
	glDisable(GL_DEPTH_TEST);

	textRenderer.RenderText(std::to_string(score1), 30.0f, height - 10.0f - 48.0f, 1.0f);
	GLfloat text_width = textRenderer.TextWidth(std::to_string(score1), 1.0f);
	textRenderer.RenderText(std::to_string(score2), width - 30.0f - text_width, height - 10.0f - 48.0f, 1.0f);

	string view = "???";
	string shading = isPhong ? " | Phong shading" : " | Gouraud shading";
	string texture = useTexture ? " | Texture(O)" : " | Texture(X)";
	string normal = useNormalMap ? " | Normal Mapping(O)" : " | Normal Mapping(X)";

	if (viewmode == VIEW_CHARACTER_EYE)
		view = "Eye View";
	else if (viewmode == VIEW_CHARACTER_BACK)
		view = "Back View";
	else if (viewmode == VIEW_CEILING)
		view = "Ceiling View";

	text_width = textRenderer.TextWidth(view + shading + texture + normal, 0.3f);
	textRenderer.RenderText(view + shading + texture + normal, width / 2.0f - text_width / 2.0f, 35.0f, 0.3f, glm::vec3(1.0f));

	text_width = textRenderer.TextWidth("Press Spacebar | F | T | N to change", 0.3f);
	textRenderer.RenderText("Press Spacebar | F | T | N to change", width / 2.0f - text_width / 2.0f, 15.0f, 0.3f, glm::vec3(1.0f));

	if (gamestate == GAME_READY) {
		text_width = textRenderer.TextWidth(std::to_string(delayTime / 1000 + 1), 1.0f);
		textRenderer.RenderText(std::to_string(delayTime / 1000 + 1), width / 2.0f - text_width / 2.0f, height - 10.0f - 48.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (gamestate == GAME_PLAYING && delayTime > 0) {
		text_width = textRenderer.TextWidth("GO!", 1.0f);
		textRenderer.RenderText("GO!", width / 2.0f - text_width / 2.0f, height - 10.0f - 48.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (gamestate == GAME_SCORE) {
		text_width = textRenderer.TextWidth("SCORE!", 1.0f);
		textRenderer.RenderText("SCORE!", width / 2.0f - text_width / 2.0f, height - 10.0f - 48.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (gamestate == GAME_SET) {
		string s;

		if (score1 > score2)
			s = "YOU WIN!";
		else
			s = "YOU LOSE ;(";

		text_width = textRenderer.TextWidth(s, 1.0f);
		textRenderer.RenderText(s, width / 2.0f - text_width / 2.0f, height - 10.0f - 48.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

		text_width = textRenderer.TextWidth("(Press Enter)", 0.8f);
		textRenderer.RenderText("(Press Enter)", width / 2.0f - text_width / 2.0f, height - 10.0f - 96.0f, 0.8f, glm::vec3(1.0f, 0.0f, 0.0f));
	}
}

bool Renderer::getIsPhong() {
	return isPhong;
}

void Renderer::setIsPhong(bool value) {
	isPhong = value;
}

bool Renderer::getUseTexture() {
	return useTexture;
}

void Renderer::setUseTexture(bool value) {
	useTexture = value;
}

bool Renderer::getUseNormalMap() {
	return useNormalMap;
}

void Renderer::setUseNormalMap(bool value) {
	useNormalMap = value;
}
