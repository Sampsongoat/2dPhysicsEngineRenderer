#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

enum class ShapeType { Square, Circle, Rectangle };

// Forward Declarations to avoid circular dependencies issues
class VertexArray;
class IndexBuffer;
class Shader;
class VertexBuffer;

// Debugging macros
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogError(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogError(const char* function, const char* file, int line);

/*
	Example of UV coordinates for a square:

     0,1 -------- 1,1
       |          |
       |          |
     0,0 -------- 1,0

*/

struct Vertex {
	float x, y;
	float r, g, b, a;
	float u, v;
	float isCircle;
};

struct Shape {
	ShapeType shape;
	float x, y;
	float size;
	float width;
	float r, g, b, a;
	float xVcty, yVcty;
	bool noMovement;
};

class Renderer
{
private:
	VertexArray* VAO;
	VertexBuffer* VBO;
	IndexBuffer* IBO;
	Shader* m_Shader;

	// Struct Holding: xPos, yPos, r, g, b, a, u, v, isCircle;
	std::vector<Vertex> Vertices;
	unsigned int QuadCount;
	float m_AspectRatio;

	// Just a number to max out number of quads so there's no performance issues
	static const unsigned int MaxQuads = 1000;
public:
	Renderer();
	~Renderer();

	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void SetAspectRatio(float aspectRatio);

	void BeginBatch();
	void DrawSquare(float x, float y, float size, float width, float r, float g, float b, float a);
	void DrawRectangle(float x, float y, float size, float width, float r, float g, float b, float a);
	void DrawCircle(float x, float y, float radius, float r, float g, float b, float a);
	void EndBatch();
};