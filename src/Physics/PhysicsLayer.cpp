#include "Physics/PhysicsLayer.h"

Physics::Physics(float gravity, float groundPosition, float groundHeight, float groundWidth, float bounceLevel)
	: m_Gravity(gravity), m_GroundPosition(groundPosition),
	m_GroundHeight(groundHeight), m_GroundWidth(groundWidth), m_BounceLevel(bounceLevel)
{
}

Physics::~Physics()
{
}

void Physics::Update(std::vector<Shape>& shapes)
{
	for (auto& shape : shapes)
	{
		if (shape.noMovement)
		{
			continue;
		}
		ApplyGravity(shape);
		UpdatePosition(shape);
		CheckGroundCollision(shape);
	}
}

void Physics::SetGravity(float gravity)
{
	m_Gravity = gravity;
}

void Physics::SetBounceLevel(float bounceLevel)
{
	m_BounceLevel = bounceLevel;
}

void Physics::ApplyGravity(Shape& shape)
{
	shape.yVcty = shape.yVcty - m_Gravity;
}

void Physics::UpdatePosition(Shape& shape) 
{
	shape.x = shape.x + shape.xVcty;
	shape.y = shape.y + shape.yVcty;
}

void Physics::CheckGroundCollision(Shape& shape)
{
	float topOfGround = m_GroundPosition + (m_GroundHeight / 2);
	float groundLeftBoundary = 0.0 - (m_GroundWidth / 2);
	float groundRightBoundary = 0.0 + (m_GroundWidth / 2);

	float halfHeight, halfWidth;
	if (shape.shape == ShapeType::Circle)
	{
		halfHeight = shape.size / 3.5f;
		halfWidth = shape.size / 3.5f;
	}
	else if (shape.shape == ShapeType::Square)
	{
		halfHeight = shape.size / 2.0f;
		halfWidth = shape.size / 2.0f;
	}
	else
	{
		halfHeight = shape.size / 2.0f;
		halfWidth = shape.size / 2.0f;
	}

	float bottomOfShape = shape.y - halfHeight;
	float leftOfShape = shape.x - halfWidth;
	float rightOfShape = shape.x + halfWidth;

	bool isAboveGround = (bottomOfShape <= topOfGround);
	bool isWithinGroundWidth = (rightOfShape > groundLeftBoundary && leftOfShape < groundRightBoundary);

	if (isAboveGround && isWithinGroundWidth)
	{
		shape.y = topOfGround + halfHeight;

		if (shape.yVcty < 0.0f)
		{
			shape.yVcty = -shape.yVcty * m_BounceLevel;

			if (abs(shape.yVcty) < 0.0001f)
			{
				shape.yVcty = 0.0f;
			}
		}
	}
}