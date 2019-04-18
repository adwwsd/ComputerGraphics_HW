#pragma once
#include <tuple>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ballObject.h"
#include "movableCubeObject.h"
#include "sceneGraphNode.h"

enum GameState {
	GAME_MENU,
	GAME_READY,
	GAME_PLAYING,
	GAME_SCORE,
	GAME_SET,
};

enum Direction {
	POSITIVE_X,
	POSITIVE_Y,
	NEGATIVE_X,
	NEGATIVE_Y
};
typedef std::tuple<bool, Direction, glm::vec2> Collision;

const float WINDOW_X_SIZE = 192.0;
const float WINDOW_Y_SIZE = 108.0;


const float PLAYER_MAX_VELOCITY = 0.1;

const glm::vec3 PLAYER_ONE_POSITION = glm::vec3(24.0f, 54.0f, 0.0f);
const glm::vec3 PLAYER_ONE_SIZE = glm::vec3(20.0f, 35.0f, 0.1f);
const glm::vec3 PLAYER_ONE_VELOCITY = glm::vec3(0.0f, 0.0f, 0.0f);

const glm::vec3 PLAYER_TWO_POSITION = glm::vec3(148.0f, 54.0f, 0.0f);
const glm::vec3 PLAYER_TWO_SIZE = glm::vec3(20.0f, 35.0f, 0.1f);
const glm::vec3 PLAYER_TWO_VELOCITY = glm::vec3(0.0f, 0.0f, 0.0f);

const glm::vec3 BALL_POSITION = glm::vec3(88.5f, 70.0f, 0.0f);
const float BALL_RADIUS = 7.5f;
const float BALL_SPEED = 0.15f;


class Game {
private:
	MovableCubeObject player1;
	MovableCubeObject player2;
	BallObject ball;
	GameState gamestate;
	bool ballCameraMode;
	bool is2player;
	int delayTime;
	int score1;
	int score2;
	int winningScore;
	bool player1Scored;
	bool exiting;

	void resetPosition();
	void restartGame();
	void updateBall(int delta);
	void updatePlayer(int delta);
	SceneGraphNode* constructSceneGraph();
public:
	Game();
	~Game();
	void init(int argc, char* argv[], int width, int height, bool isFullScreen);
	void exit();
	bool isExiting();
	void handleInput(unsigned char key);
	void handleInputUp(unsigned char key);
	void handleSpecialInput(int key);
	void handleSpecialInputUp(int key);
	void update(int delta);
	void render();
};
