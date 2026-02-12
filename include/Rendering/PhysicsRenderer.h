#pragma once 

#include <GLFW/glfw3.h>
#include <vector>

#include "Rendering/Renderer.h"
#include "Physics/PhysicsLayer.h"

class PhysicsEngine
{
private:
	int m_Width;
	int m_Height;
	GLFWwindow* m_Window;

	// Struct Holding: shape, xPos, yPos, size/radius, r, g, b, a)
	std::vector<Shape> m_Shapes;
	Physics* m_PhysicsLayer;

public:
	PhysicsEngine(int width, int height, const char* title);
	~PhysicsEngine();

	bool Init(const char* title);
	int Run();

	void OnMouseLeftClick(double clickXPos, double clickYPos);

	static void MouseLeftClickCallBack(GLFWwindow* window, int button, int action, int mods);
};