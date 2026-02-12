#pragma once
#include <vector>

#include "Rendering/Renderer.h"

class Physics
{
private:
	float m_Gravity;
	float m_GroundPosition;
	float m_GroundHeight;
	float m_GroundWidth;
	float m_BounceLevel;
public:
	Physics(float gravity, float groundPosition, float groundHeight, float groundWidth, float bounceLevel);
	~Physics();

	void Update(std::vector<Shape>& shapes);
	void SetGravity(float gravity);
	void SetBounceLevel(float bounceLevel);
private:
	void ApplyGravity(Shape& shape);
	void UpdatePosition(Shape& shape);
	void CheckGroundCollision(Shape& shape);
};