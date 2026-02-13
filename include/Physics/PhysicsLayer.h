#pragma once

#include "Rendering/Renderer.h"
#include <vector>

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

public:
	Physics(float gravity, float groundPosition, float groundHeight, float groundWidth, float bounceLevel, float aspectRatio);
	~Physics();

	void Update(std::vector<Shape>& shapes, float dt);
	void SetGravity(float gravity);
	void SetBounceLevel(float bounceLevel);

	// Collision functions
	void Physics::CheckCircleCollision(Shape& circle1, Shape& circle2);
	void Physics::CheckSquareCollision(Shape& square1, Shape& square2);
	void Physics::CheckCircleSquareCollision(Shape& circle, Shape& square);
	void Physics::UpdateObjectCollisions(std::vector<Shape>& shapes);

private:
	void ApplyGravity(Shape& shape);
	void UpdatePosition(Shape& shape, float dt);
	void CheckGroundCollision(Shape& shape);
};