#pragma once

#include "Rendering/Renderer.h"
#include <vector>

struct Wall
{
	float xPosition;
	float yPosition;
	float width;
	float height;
};

class Physics
{
private:
	float m_Gravity;
	float m_GroundPosition;
	float m_GroundHeight;
	float m_GroundWidth;
	float m_BounceLevel;
	float m_AspectRatio;
	float m_Restitution;
	float m_VelocityThreshold;

	std::vector<Wall> m_Walls;

public:
	Physics(float gravity, float groundPosition, float groundHeight, float groundWidth, float bounceLevel, float aspectRatio);
	~Physics();

	void Update(std::vector<Shape>& shapes, float dt);
	void SetGravity(float gravity);
	void SetBounceLevel(float bounceLevel);

	// Wall functions
	void AddWall(float xPosition, float yPosition, float width, float height);
	void ClearWalls();

private:
	void ApplyGravity(Shape& shape);
	void UpdatePosition(Shape& shape, float dt);
	void ApplyGroundCollision(Shape& shape);
	void ApplyWallCollision(Shape& shape);
	void ApplyFriction(Shape& shape, std::vector<Shape>& shapes);

	// Collision functions

	bool IsTouchingAnything(Shape& shape, std::vector<Shape>& shapes);
	bool IsOnGround(Shape& shape);

	bool CheckCircleCollision(Shape& circle1, Shape& circle2);
	bool Physics::CheckSquareCollision(Shape& square1, Shape& square2);
	bool Physics::CheckCircleSquareCollision(Shape& circle, Shape& square);

	void ApplyCircleCollision(Shape& circle1, Shape& circle2);
	void ApplySquareCollision(Shape& square1, Shape& square2);
	void ApplyCircleSquareCollision(Shape& circle, Shape& square);
	void UpdateObjectCollisions(std::vector<Shape>& shapes);

};