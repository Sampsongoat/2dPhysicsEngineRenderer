#include "Physics/PhysicsLayer.h"
#include <cmath>

Physics::Physics(float gravity, float groundPosition, float groundHeight, float groundWidth, float bounceLevel, float aspectRatio)
	: m_Gravity(gravity), m_GroundPosition(groundPosition),m_GroundHeight(groundHeight), m_GroundWidth(groundWidth), 
	m_BounceLevel(bounceLevel), m_AspectRatio(aspectRatio), m_Restitution(0.7f), m_VelocityThreshold(0.0001f)
{
}

Physics::~Physics()
{
}

void Physics::Update(std::vector<Shape>& shapes, float dt)
{
	for (auto& shape : shapes)
	{
		if (shape.noMovement)
		{
			continue;
		}
		ApplyGravity(shape);
		UpdatePosition(shape, dt);
		ApplyGroundCollision(shape);
	}

	UpdateObjectCollisions(shapes);

	for (auto& shape : shapes)
	{
		ApplyFriction(shape, shapes);
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

void Physics::ApplyFriction(Shape& shape, std::vector<Shape>& shapes)
{
	for (Shape& shape : shapes)
	{
		if (IsOnGround(shape))
		{
			shape.xVcty = shape.xVcty * 0.9999f;
		}

		if (IsOnGround(shape) && IsTouchingAnything(shape, shapes))
		{
			shape.xVcty = shape.xVcty * 0.9995f;
		}
		
		if (IsTouchingAnything(shape, shapes))
		{
			shape.xVcty = shape.xVcty * 0.9999f;
		}
	}
}

bool Physics::IsTouchingAnything(Shape& shape, std::vector<Shape>& shapes)
{
	for (Shape& otherShape : shapes)
	{
		if (&shape == &otherShape)
		{
			continue;
		}
		if (shape.shape == ShapeType::Circle && otherShape.shape == ShapeType::Circle)
		{
			if (CheckCircleCollision(shape, otherShape))
			{
				return true;
			}
		}
		else if (shape.shape == ShapeType::Square && otherShape.shape == ShapeType::Square)
		{
			// CheckSquareCollision(shape, otherShape);
			return true;
		}
		else
		{
			// CheckCircleSquareCollision(shape, otherShape);
			return true;
		}
	}

	return 0;
}

bool Physics::IsOnGround(Shape& shape)
{
	float topOfGround = m_GroundPosition + (m_GroundHeight / 2);
	float groundLeftBoundary = 0.0 - (m_GroundWidth / 2 / m_AspectRatio);
	float groundRightBoundary = 0.0 + (m_GroundWidth / 2 / m_AspectRatio);

	float bottomOfShape = shape.y - (shape.size / 2.0f);
	float leftOfShape = shape.x - (shape.size / 2.0f) / m_AspectRatio;
	float rightOfShape = shape.x + (shape.size / 2.0f) / m_AspectRatio;

	float distanceToGround = topOfGround - bottomOfShape;

	if (distanceToGround <= m_VelocityThreshold && rightOfShape > groundLeftBoundary && leftOfShape < groundRightBoundary)
	{
		return true;
	}

	return false;
}

bool Physics::CheckCircleCollision(Shape& circle1, Shape& circle2)
{
	// distance between two circles center points
	float dx = (circle2.x - circle1.x) * m_AspectRatio;
	float dy = circle2.y - circle1.y;
	float dist = sqrt(pow(dx, 2) + pow(dy, 2));

	// distance of radius of circles combined
	float radius1 = circle1.size / 3.5f;
	float radius2 = circle2.size / 3.5f;
	float distanceBetween = radius1 + radius2;

	if (dist >= distanceBetween)
	{
		return false;
	}

	return true;
}

void Physics::UpdatePosition(Shape& shape, float dt)
{
	shape.x = shape.x + (shape.xVcty * dt);
	shape.y = shape.y + (shape.yVcty * dt);
}

void Physics::UpdateObjectCollisions(std::vector<Shape>& shapes)
{
	for (int i = 0; i < shapes.size() - 1; i++)
	{
		for (int j = i + 1; j < shapes.size(); j++)
		{
			if (shapes[i].shape == ShapeType::Circle && shapes[j].shape == ShapeType::Circle)
			{
				ApplyCircleCollision(shapes[i], shapes[j]);
			}
		}
	}
}

void Physics::ApplyCircleCollision(Shape& circle1, Shape& circle2)
{
	// distance between two circles center points
	float dx = (circle2.x - circle1.x) * m_AspectRatio;
	float dy = circle2.y - circle1.y;
	float dist = sqrt(pow(dx, 2) + pow(dy, 2));

	// distance of radius of circles combined
	float radius1 = circle1.size / 3.5f;
	float radius2 = circle2.size / 3.5f;
	float distanceBetween = radius1 + radius2;

	if (CheckCircleCollision(circle1, circle2))
	{
		if (dist < 0.0001f)
		{
			dx = 0.01f;
			dy = 0.01f;
			dist = sqrt(pow(dx, 2) + pow(dy, 2));
		}

		float normalizedX = dx / dist;
		float normalizedY = dy / dist;

		float overlap = distanceBetween - dist;

		circle1.x -= normalizedX * overlap * 0.5;
		circle1.y -= normalizedY * overlap * 0.5;

		circle2.x += normalizedX * overlap * 0.5;
		circle2.y += normalizedY * overlap * 0.5;

		float relativeVelocityX = circle2.xVcty - circle1.xVcty;
		float relativeVelocityY = circle2.yVcty - circle1.xVcty;

		float velocityAlongNormal = (relativeVelocityX * normalizedX) + (relativeVelocityY * normalizedY);

		if (velocityAlongNormal > 0)
		{
			return;
		}

		float impulse = -(1 + m_Restitution) * velocityAlongNormal;

		impulse /= (1 / circle1.size) + (1 / circle2.size);

		float impulseX = impulse * normalizedX;
		float impulseY = impulse * normalizedY;

		circle1.xVcty -= impulseX;
		circle1.yVcty -= impulseY;

		circle2.xVcty += impulseX;
		circle2.yVcty += impulseY;
	}
}

void Physics::CheckSquareCollision(Shape& square1, Shape& square2)
{

}

void Physics::CheckCircleSquareCollision(Shape& circle, Shape& square)
{

}

void Physics::ApplyGroundCollision(Shape& shape)
{
	float topOfGround = m_GroundPosition + (m_GroundHeight / 2);
	float groundLeftBoundary = 0.0 - (m_GroundWidth / 2 / m_AspectRatio);
	float groundRightBoundary = 0.0 + (m_GroundWidth / 2 / m_AspectRatio);

	float halfHeight, halfWidth;
	if (shape.shape == ShapeType::Circle)
	{
		halfHeight = shape.size / 3.75f;
		halfWidth = (shape.size / 3.75f) / m_AspectRatio;
	}
	else if (shape.shape == ShapeType::Square)
	{
		halfHeight = shape.size / 2.0f;
		halfWidth = (shape.size / 2.0f) / m_AspectRatio;
	}
	else
	{
		halfHeight = shape.size / 2.0f;
		halfWidth = (shape.size / 2.0f) / m_AspectRatio;
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

void ApplyWallCollision(Shape& shape)
{

}