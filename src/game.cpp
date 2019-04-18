#include "game.h"
#include "render.h"
#include "collision.h"
#include "sceneGraphNode.h"
#include <random>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>

void Game::exit() {
	exiting = true;
}

bool Game::isExiting() {
	return exiting;
}

SceneGraphNode *Game::constructSceneGraph() {
	glm::vec2 player1pos = player1.getPosition();
	glm::vec2 player2pos = player2.getPosition();
	glm::vec2 player1size = player1.getSize();
	glm::vec2 player2size = player2.getSize();
	glm::vec2 ballPos = ball.getPosition();
	float ballRadius = ball.getRadius();

	glm::mat4 backgroundToPikachu1 =
		glm::translate(glm::mat4(1), glm::vec3(player1pos.x, player1pos.y, 0.0f));

	/*
	glm::mat4 pikachuToEar1 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x / 5, player1size.y * 6 / 7, 0.0f)) * 
		glm::rotate(glm::mat4(1), (50.0f + player1.getEarAngle()) * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 pikachuToProximalTail1 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x * 0.05f, player1size.y * 0.2f, 0.0f)) *
		glm::rotate(glm::mat4(1), (60.0f + player1.getTailProximalAngle()) * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));
	
	glm::mat4 proximalToDistalTail1 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x * 0.1f, player1size.y * 0.2f, 0.0f)) *
		glm::rotate(glm::mat4(1), player1.getTailDistalAngle() * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));
		*/

	glm::mat4 backgroundToPikachu2 =
		glm::translate(glm::mat4(1), glm::vec3(player2pos.x +player2size.x, player2pos.y, 0.0f)) *
		glm::scale(glm::mat4(1), glm::vec3(-1.0f, 1.0f, 0.0f));
	/*
	glm::mat4 pikachuToEar2 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x / 5, player1size.y * 6 / 7, 0.0f)) *
		glm::rotate(glm::mat4(1), (50.0f + player2.getEarAngle()) * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 pikachuToProximalTail2 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x * 0.05f, player1size.y * 0.2f, 0.0f)) *
		glm::rotate(glm::mat4(1), (60.0f + player2.getTailProximalAngle()) * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 proximalToDistalTail2 =
		glm::translate(glm::mat4(1), glm::vec3(player1size.x * 0.1f, player1size.y * 0.2f, 0.0f)) *
		glm::rotate(glm::mat4(1), player2.getTailDistalAngle() * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));
	
	glm::mat4 backgroundToNet =
		glm::translate(glm::mat4(1), glm::vec3(netPos.x, netPos.y, 0.0f));
		*/
	glm::mat4 backgroundToBall =
		glm::translate(glm::mat4(1), glm::vec3(ballPos.x, ballPos.y, 0.0f));

	return
		new SceneGraphNode(glm::mat4(1), renderBackground,
			new SceneGraphNode(backgroundToPikachu1, renderPikachu,
				nullptr, new SceneGraphNode(backgroundToPikachu2, renderPikachu,
					nullptr, new SceneGraphNode(backgroundToBall, renderBall,
						nullptr, nullptr)
				)
			),
			nullptr
		);
}

void Game::render() {
	if (ballCameraMode)
		setBallCamera(ball.getPosition(), ball.getRadius());
	else
		setNormalCamera();

	if (gamestate == GAME_MENU) {
		renderMenu(is2player);
	}
	else {
		SceneGraphNode *root = constructSceneGraph();
		root->traverse();

		renderScore(score1, score2);

		if (gamestate == GAME_READY)
			renderReady(delayTime);
		else if (gamestate == GAME_PLAYING && delayTime > 0)
			renderGo();
		else if (gamestate == GAME_SCORE)
			renderScoreText();
		else if (gamestate == GAME_SET)
			renderWinText(is2player, score1, score2);
		else
			renderCameraText(ballCameraMode);

		delete root;
	}

	glutSwapBuffers();
}

float generateRandomFromZeroToOne() {
	std::mt19937 engine((unsigned int)time(NULL));
	std::uniform_real_distribution<float> distribution(0.0, 1.0);

	return distribution(engine);
}

glm::vec3 generateUnitVector_old() {
	float x = 1;
	float y = 0;
	while (x < 0.4 || y < 0.2) {
		x = generateRandomFromZeroToOne();
		y = sqrt(1 - x * x);
	}

	return glm::vec3(x, y, 0);
}

glm::vec3 generateUnitVector() {
	const float PI = 3.1415926535;
	float theta = 2 * PI * generateRandomFromZeroToOne();
	float phi = PI * generateRandomFromZeroToOne();
	return glm::vec3(cos(theta) * cos(phi), cos(theta) * sin(phi), sin(theta));
}

Game::Game() :
	player1(PLAYER_ONE_POSITION, PLAYER_ONE_SIZE, PLAYER_ONE_VELOCITY),
	player2(PLAYER_TWO_POSITION, PLAYER_TWO_SIZE, PLAYER_TWO_VELOCITY),
	ball(BALL_POSITION, BALL_RADIUS, BALL_SPEED * generateUnitVector_old()),
	gamestate(GAME_MENU),
	ballCameraMode(false),
	delayTime(3000),
	is2player(false),
	score1(0), score2(0), winningScore(5),
	player1Scored(false) {}

Game::~Game() {}

void Game::resetPosition() {
	delayTime = 3000;

	player1.setPosition(PLAYER_ONE_POSITION);
	player1.setsize(PLAYER_ONE_SIZE);
	player1.setVelocity(PLAYER_ONE_VELOCITY);

	player2.setPosition(PLAYER_TWO_POSITION);
	player2.setsize(PLAYER_TWO_SIZE);
	player2.setVelocity(PLAYER_TWO_VELOCITY);

	ball.setPosition(BALL_POSITION);
	ball.setRadius(BALL_RADIUS);
	glm::vec3 ballVelocity = BALL_SPEED * generateUnitVector_old();
	if ((player1Scored && ballVelocity.x < 0) || (!player1Scored && ballVelocity.x > 0))
		ballVelocity.x = -ballVelocity.x;
	ball.setVelocity(ballVelocity);
}

void Game::restartGame() {
	resetPosition();
	ballCameraMode = false;
	delayTime = 3000;
	score1 = 0;
	score2 = 0;
	player1Scored = false;
	gamestate = GAME_READY;
}

void Game::init(int argc, char* argv[], int width, int height, bool isFullScreen) {
	// Initialize window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pickachu Volleyball");
	if (isFullScreen)
		glutFullScreen();

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}

void Game::handleInput(unsigned char key) {
	if (gamestate == GAME_PLAYING || gamestate == GAME_SCORE) {
		glm::vec3 velocity(0.0f, 0.0f, 0.0f);

		switch (key) {
		case 'a':
			velocity.x = -0.1f;
			break;
		case 'd':
			velocity.x = 0.1f;
			break;
		case 'w':
			velocity.y = 0.1f;
			break;
		case 's':
			velocity.y = -0.1f;
			break;
		case 'r':
			restartGame();
			break;
		case ' ':
			ballCameraMode = !ballCameraMode;
			return;
		}

		player1.setVelocity(velocity);
	}
	else if (gamestate == GAME_MENU) {
		if (key == '\n' || key == 13)
			gamestate = GAME_READY;
	}
	else if (gamestate == GAME_SET) {
		if (key == '\n' || key == 13) {
			gamestate = GAME_MENU;
			score1 = 0;
			score2 = 0;
			resetPosition();
		}
	}

	if (key == ' ')
		ballCameraMode = !ballCameraMode;
}

void Game::handleInputUp(unsigned char key) {
	if (gamestate == GAME_PLAYING || gamestate == GAME_SCORE) {
		glm::vec3 velocity = player1.getVelocity();

		switch (key) {
		case 'a':
			if (velocity.x < 0)
				velocity.x = 0;
			break;
		case 'd':
			if (velocity.x > 0)
				velocity.x = 0;
			break;
		case 'w':
			if (velocity.y > 0)
				velocity.y = 0;
			break;
		case 's':
			if (velocity.y < 0)
				velocity.y = 0;
			break;
		}

		player1.setVelocity(velocity);
	}
}

void Game::handleSpecialInput(int key) {
	if (gamestate == GAME_PLAYING || gamestate == GAME_SCORE) {
		if (!is2player)
			return;
		glm::vec3 velocity(0.0f, 0.0f, 0.0f);

		switch (key) {
		case GLUT_KEY_LEFT:
			velocity.x = -0.1f;
			break;
		case GLUT_KEY_RIGHT:
			velocity.x = 0.1f;
			break;
		case GLUT_KEY_UP:
			velocity.y = 0.1f;
			break;
		case GLUT_KEY_DOWN:
			velocity.y = -0.1f;
			break;
		}

		player2.setVelocity(velocity);
	}
	else if (gamestate == GAME_MENU) {
		if (key == GLUT_KEY_UP)
			is2player = false;
		else if (key == GLUT_KEY_DOWN)
			is2player = true;
	}
}

void Game::handleSpecialInputUp(int key) {
	if (gamestate == GAME_PLAYING || gamestate == GAME_SCORE) {
		if (!is2player)
			return;

		glm::vec3 velocity = player2.getVelocity();

		switch (key) {
		case GLUT_KEY_LEFT:
			if (velocity.x < 0)
				velocity.x = 0;
			break;
		case GLUT_KEY_RIGHT:
			if (velocity.x > 0)
				velocity.x = 0;
			break;
		case GLUT_KEY_UP:
			if (velocity.y > 0)
				velocity.y = 0;
			break;
		case GLUT_KEY_DOWN:
			if (velocity.y < 0)
				velocity.y = 0;
			break;
		}

		player2.setVelocity(velocity);
	}
}

void Game::updateBall(int delta) {
	MovableCubeObject* objectsToCollideAgainstBall[2] = { &player1, &player2 };

	ball.move(delta);

	for (size_t i = 0; i < 2; i++) {
		Collision collision = CheckCollision(*objectsToCollideAgainstBall[i], ball);
		if (std::get<0>(collision)) {
			Direction dir = std::get<1>(collision);
			glm::vec2 diffVec = std::get<2>(collision);
			glm::vec3 ballVelocity = ball.getVelocity();
			glm::vec3 ballPosition = ball.getPosition();
			float ballRadius = ball.getRadius();

			if (dir == NEGATIVE_X || dir == POSITIVE_X) {
				ballVelocity.x = -ballVelocity.x;
				float penetration = ballRadius - std::abs(diffVec.x);
				if (dir == NEGATIVE_X)
					ballPosition.x += penetration;
				else
					ballPosition.x -= penetration;
			}
			else {
				ballVelocity.y = -ballVelocity.y;
				float penetration = ball.getRadius() - std::abs(diffVec.y);
				if (dir == NEGATIVE_Y)
					ballPosition.y += penetration;
				else
					ballPosition.y -= penetration;
			}
			ball.setVelocity(ballVelocity);
			ball.setPosition(ballPosition);
		}
	}

}

void Game::updatePlayer(int delta) {
	player1.move(delta);
	player2.move(delta);

	glm::vec3 player1Position = player1.getPosition();
	glm::vec3 player2Position = player2.getPosition();

	if (player1Position.x + player1.getSize().x > WINDOW_X_SIZE / 2)
		player1Position.x = WINDOW_X_SIZE / 2 - player1.getSize().x;
	if (player2Position.x < WINDOW_X_SIZE / 2)
		player2Position.x = WINDOW_X_SIZE / 2;
	
	player1.setPosition(player1Position);
	player2.setPosition(player2Position);
}


void Game::update(int delta) {

	if (gamestate == GAME_READY) {
		delayTime -= delta;
		if (delayTime < 0) {
			delayTime = 1000;
			gamestate = GAME_PLAYING;
		}
	}

	if (gamestate == GAME_PLAYING || gamestate == GAME_SCORE) {
		if (!is2player) {
			glm::vec3 velocity(0.0, 0.0, 0.0);

			if (player2.getPosition().y < ball.getPosition().y)
				velocity.y = ball.getVelocity().y * 0.83f;
			else
				velocity.y = -ball.getVelocity().y * 0.83f;

			player2.setVelocity(velocity);
		}
	}

	if (gamestate == GAME_PLAYING) {
		updateBall(delta);
		updatePlayer(delta);

		if (delayTime > 0)
			delayTime -= delta;

		int epsilon = 2;

		if ((ball.getPosition().x < epsilon) ||
			(ball.getPosition().x + 2 * ball.getRadius() + epsilon > WINDOW_X_SIZE)) {
			if (ball.getPosition().x < epsilon) {
				player1Scored = false;
				score2++;
			}				
			else {
				player1Scored = true;
				score1++;
			}
				

			if (score1 == winningScore || score2 == winningScore) {
				gamestate = GAME_SET;
				std::mt19937 engine((unsigned int)time(NULL));
				std::uniform_int_distribution<int> distribution(0, 1);
				player1Scored = distribution(engine);
			}
			else {
				gamestate = GAME_SCORE;
				delayTime = 3000;
			}
		}
	}
	else if (gamestate == GAME_SCORE) {
		updateBall(delta / 2);
		updatePlayer(delta / 2);
		delayTime -= delta;

		if (delayTime < 0) {
			delayTime = 3000;
			gamestate = GAME_READY;
			resetPosition();
		}
	}
	else if (gamestate == GAME_SET) {
		updateBall(delta / 2);
	}

	return;
}