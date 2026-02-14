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
		ApplyWallCollision(shape);
	}

	UpdateObjectCollisions(shapes);

	for (auto& shape : shapes)
	{
		ApplyFriction(shape, shapes);
	}
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

			if (shapes[i].shape == ShapeType::Square && shapes[j].shape == ShapeType::Square)
			{
				ApplySquareCollision(shapes[i], shapes[j]);
			}
		}
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
			if (CheckSquareCollision(shape, otherShape))
			{
				return true;
			}
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


bool Physics::CheckSquareCollision(Shape& square1, Shape& square2)
{
	float halfWidth1 = (square1.size / 2.0f) / m_AspectRatio;
	float halfHeight1 = square1.size / 2.0f;
	float halfWidth2 = (square2.size / 2.0f) / m_AspectRatio;
	float halfHeight2 = square2.size / 2.0f;

	return ((abs(square1.x - square2.x) < (halfWidth1 + halfWidth2)) && (abs(square1.y - square2.y) < (halfHeight1 + halfHeight2)));
}


bool Physics::CheckCircleSquareCollision(Shape& circle, Shape& square)
{
	return false;
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

void Physics::ApplySquareCollision(Shape& square1, Shape& square2)
{
	if (CheckSquareCollision(square1, square2))
	{
		float dx = square2.x - square1.x;
		float dy = square2.y - square1.y;

		float overlapX = (square1.size) / m_AspectRatio - abs(dx);
		float overlapY = square1.size - abs(dy);

		bool directionX = (dx > 0);
		bool directionY = (dy > 0);

		if (overlapX < overlapY)
		{
			if (directionX)
			{
				square1.x -= 1.0 * overlapX * 0.5f;
				square2.x += 1.0 * overlapX * 0.5f;
			}
			else
			{
				square1.x -= -1.0 * overlapX * 0.5f;
				square2.x += -1.0 * overlapX * 0.5f;
			}

			float relativeVelocityX = square2.xVcty - square1.xVcty;
			float impulse = -(1 + m_Restitution) * relativeVelocityX;
			impulse /= (1 / square1.size) + (1 / square2.size);

			square1.xVcty -= impulse;
			square2.xVcty += impulse;
		}
		else
		{
			if (directionY)
			{
				square1.y -= 1.0 * overlapY * 0.5f;
				square2.y += 1.0 * overlapY * 0.5f;
			}
			else
			{
				square1.y -= -1.0 * overlapY * 0.5f;
				square2.y += -1.0 * overlapY * 0.5f;
			}

			float relativeVelocityY = square2.yVcty - square1.yVcty;
			float impulse = -(1 + m_Restitution) * relativeVelocityY;
			impulse /= (1 / square1.size) + (1 / square2.size);

			square1.yVcty -= impulse;
			square2.yVcty += impulse;
		}
	}
}

void Physics::ApplyCircleSquareCollision(Shape& circle, Shape& square)
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

void Physics::AddWall(float xPosition, float yPosition, float width, float height)
{
	m_Walls.push_back({xPosition, yPosition, width, height});
}

void Physics::ClearWalls()
{
	m_Walls.clear();
}

void Physics::ApplyWallCollision(Shape& shape)
{
	float shapeHalfWidth;
	float shapeHalfHeight;

	if (shape.shape == ShapeType::Circle)
	{
		shapeHalfWidth = (shape.size / 3.75f) / m_AspectRatio;
		shapeHalfHeight = (shape.size / 3.75f);
	}
	else if (shape.shape == ShapeType::Square)
	{
		shapeHalfWidth = (shape.size / 2.0f) / m_AspectRatio;
		shapeHalfHeight = (shape.size / 2.0f);
	}
	else
	{
		shapeHalfWidth = (shape.size / 2.0f) / m_AspectRatio;
		shapeHalfHeight = (shape.size / 2.0f);
	}

	float leftOfShape = shape.x - shapeHalfWidth;
	float rightOfShape = shape.x + shapeHalfWidth;
	float topOfShape = shape.y + shapeHalfHeight;
	float bottomOfShape = shape.y - shapeHalfHeight;

	for (const auto& wall : m_Walls)
	{
		float wallHalfWidth = (wall.width / 2.0f) / m_AspectRatio;
		float wallHalfHeight = (wall.height / 2.0f);
		float wallLeftEdge = wall.xPosition - wallHalfWidth;
		float wallRightEdge = wall.xPosition + wallHalfWidth;
		float wallTopEdge = wall.yPosition + wallHalfHeight;
		float wallBottomEdge = wall.yPosition - wallHalfHeight;

		bool horizontalOverlap = (topOfShape > wallBottomEdge && bottomOfShape < wallTopEdge);
		bool verticalOverlap = (rightOfShape > wallLeftEdge && leftOfShape < wallRightEdge);

		// sideCollision is number from 0 - 3. 0 being bottom wall and going clockwise for rest
		int sideCollision = 0;

		if (horizontalOverlap && verticalOverlap)
		{
			float overlapLeft = rightOfShape - wallLeftEdge;
			float overlapRight = wallRightEdge - leftOfShape;
			float overlapTop = wallTopEdge - bottomOfShape;
			float overlapBottom = topOfShape - wallBottomEdge;

			float minOverlap = overlapLeft;

			if (overlapRight < minOverlap)
			{
				minOverlap = overlapRight;
				sideCollision = 1;
			}
			if (overlapTop < minOverlap)
			{
				minOverlap = overlapTop;
				sideCollision = 2;
			}
			if (overlapBottom < minOverlap)
			{
				minOverlap = overlapBottom;
				sideCollision = 3;
			}

			switch (sideCollision) {
			case 0:
				shape.x = wallLeftEdge - shapeHalfWidth;
				if (shape.xVcty > 0.0f)
				{
					shape.xVcty = -shape.xVcty * m_BounceLevel;
					if (abs(shape.xVcty) < m_VelocityThreshold)
					{
						shape.xVcty = 0.0f;
					}
				}
				break;

			case 1:
				shape.x = wallRightEdge + shapeHalfWidth;
				if (shape.xVcty < 0.0f)
				{
					shape.xVcty = -shape.xVcty * m_BounceLevel;
					if (abs(shape.xVcty) < m_VelocityThreshold)
					{
						shape.xVcty = 0.0f;
					}
				}
				break;

			case 2:
				shape.y = wallTopEdge + shapeHalfHeight;
				if (shape.yVcty < 0.0f)
				{
					shape.yVcty = -shape.yVcty * m_BounceLevel;
					if (abs(shape.yVcty) < m_VelocityThreshold)
					{
						shape.yVcty = 0.0f;
					}
				}
				break;

			case 3:
				shape.y = wallBottomEdge - shapeHalfHeight;
				if (shape.yVcty > 0.0f)
				{
					shape.yVcty = -shape.yVcty * m_BounceLevel;
					if (abs(shape.yVcty) < m_VelocityThreshold)
					{
						shape.yVcty = 0.0f;
					}
				}
				break;

			default:
				break;
			}
		}	
	}
}