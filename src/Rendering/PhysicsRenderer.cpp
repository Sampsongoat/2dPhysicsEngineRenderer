#include <glad/glad.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Rendering/PhysicsRenderer.h"
#include "Rendering/Renderer.h"

PhysicsEngine::PhysicsEngine(int width, int height, const char* title)
	: m_Width(width), m_Height(height), m_Window(nullptr), m_PhysicsLayer(nullptr)
{
	srand(static_cast<unsigned int>(time(nullptr)));
}

PhysicsEngine::~PhysicsEngine()
{
	if (m_PhysicsLayer)
	{
		delete m_PhysicsLayer;
		m_PhysicsLayer = nullptr;
	}

	if (m_Window)
	{
		glfwTerminate();
	}
}

bool PhysicsEngine::Init(const char* title)
{
	if (!glfwInit())
	{
		return false;
	}

	// Set GLFW Versions for other stuff
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(m_Width, m_Height, title, NULL, NULL);
	if (!m_Window)
	{
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(m_Window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return false;
	}

	glfwSwapInterval(1);
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	glfwSetWindowUserPointer(m_Window, this);
	glfwSetMouseButtonCallback(m_Window, MouseLeftClickCallBack);

	float scale = m_Height / 480.0f;
	float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	float gravity = 0.0005f;
	float groundPosition = -0.8;
	float groundHeight = 0.2;
	float bounceLevel = 0.5;
	float groundWidth = 1.6f / aspectRatio;

	m_PhysicsLayer = new Physics(gravity, groundPosition, groundHeight, groundWidth, bounceLevel);

	// Remove later, Just a rectangle at bottom of screen
	m_Shapes.push_back({ShapeType::Rectangle, 0.0f, groundPosition, groundHeight, groundWidth + 0.1f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, true });

	return true;
}

int PhysicsEngine::Run()
{
	if (!Init("PhysicsEngine"))
	{
		return -1;
	}

	Renderer renderer;

	float aspectRatio = static_cast<float>(m_Width) / static_cast<float>(m_Height);
	renderer.SetAspectRatio(aspectRatio);

	while (!glfwWindowShouldClose(m_Window))
	{
		m_PhysicsLayer->Update(m_Shapes);
		// Step one: clear screen
		renderer.Clear();

		// Step two: Add to batch
		renderer.BeginBatch();

		// Step three: Submit draw data
		for (const auto& shape : m_Shapes)
		{
			if (shape.shape == ShapeType::Square)
			{
				renderer.DrawSquare(shape.x, shape.y, shape.size, shape.width,
					shape.r, shape.g, shape.b, shape.a);
			}
			else if (shape.shape == ShapeType::Circle)
			{
				renderer.DrawCircle(shape.x, shape.y, shape.size,
					shape.r, shape.g, shape.b, shape.a);
			}
			else if (shape.shape == ShapeType::Rectangle)
			{
				renderer.DrawRectangle(shape.x, shape.y, shape.size, shape.width,
					shape.r, shape.g, shape.b, shape.a);
			}
		}

		// Step four: Draw everything all at once (Batch Rendering) 
		renderer.EndBatch();

		/* Swap front and back buffers */
		glfwSwapBuffers(m_Window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void PhysicsEngine::OnMouseLeftClick(double clickXPos, double clickYPos)
{
	// Conversion of screen to OpenGL coordinates
	/*
	The screen has a coordinate system where top left is (0, 0) while bottom right is (1, 1) on a window scale
	Opengl has a coordinate system where it's like a graph where (0, 0) is in the center and (-1 , -1) is bottom left
	*/

	// Example: (1, 1) Bottom right converted to (1, -1) Bottom right
	// 1 -> scale to window 1 -> 2 -> 1
	float x = (clickXPos / m_Width) * 2.0f - 1.0f;
	// 1 -> scale to window 1 -> 2 -> 1 -> -1
	// Flipped for Y but same as X overall
	float y = -((clickYPos / m_Height) * 2.0f - 1.0f);

	float scale = m_Height / 480.0f;

	float r = static_cast<float>(rand()) / RAND_MAX;
	float g = static_cast<float>(rand()) / RAND_MAX;
	float b = static_cast<float>(rand()) / RAND_MAX;

	bool isCircle = (rand() % 2) == 0;

	if (isCircle)
	{
		m_Shapes.push_back({ ShapeType::Circle, x, y, 0.1f * scale, 0.1f * scale, r, g, b, 1.0f, 0.0f, 0.0f, false });
	}
	else
	{
		m_Shapes.push_back({ ShapeType::Square, x, y, 0.05f * scale, 0.05f * scale, r, g, b, 1.0f, 0.0f, 0.0f, false });
	}
}

void PhysicsEngine::MouseLeftClickCallBack(GLFWwindow* window, int button, int action, int mods)
{
	PhysicsEngine* engine = static_cast<PhysicsEngine*>(glfwGetWindowUserPointer(window));
	if (engine && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double mouseXPos, mouseYPos;
		glfwGetCursorPos(window, &mouseXPos, &mouseYPos);
		engine->OnMouseLeftClick(mouseXPos, mouseYPos);
	}
}