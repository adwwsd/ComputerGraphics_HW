#pragma once
#include <glm/geometric.hpp>

void setNormalCamera();
void setBallCamera(glm::vec2 pos, float radius);
void renderScore(int score1, int score2);
void renderReady(int delayTime);
void renderGo();
void renderScoreText();
void renderCameraText(bool ballCameraMode);
void renderMenu(bool is2player);
void renderWinText(bool is2player, int score1, int score2);
void renderPikachu(MovableRectangleObject player, bool isLeft);
void renderNet(RectangleObject net);
void renderBall(BallObject ball);