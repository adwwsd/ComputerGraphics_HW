#pragma once
#include "object.h"
#include "settings.h"

class BallObject : public Object {
private:
	float radius;
	glm::vec3 velocity;
public:
	BallObject() {}
	~BallObject() {}
	BallObject(glm::vec3 _position, float _radius, glm::vec3 _velocity) :
		Object(_position), radius(_radius), velocity(_velocity) {}
	float getRadius() { return radius; }
	glm::vec3 getVelocity() { return velocity; }
	void setRadius(float _radius) { radius = _radius; }
	void setVelocity(glm::vec3 _velocity) { velocity = _velocity; }
	void move(int delta) {
		glm::vec3 displacement = velocity * (float)delta;
		position += displacement;
		
		if (position.x < 0 || position.x + 2 * radius > MAP_SIZE.x) {
			velocity.x = -velocity.x;
			if (position.x < 0)
				position.x = 0;
			else
				position.x = MAP_SIZE.x - 2 * radius;
		}
		if (position.y < 0 || position.y + 2 * radius > MAP_SIZE.y) {
			velocity.y = -velocity.y;
			if (position.y < 0)
				position.y = 0;
			else
				position.y = MAP_SIZE.y - 2 * radius;
		}
	}
};