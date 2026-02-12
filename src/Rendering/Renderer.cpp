#include "Rendering/Renderer.h"
#include "Rendering/VertexArray.h"   
#include "Rendering/IndexBuffer.h"  
#include "Rendering/Shader.h" 
#include "Rendering/VertexBuffer.h"
#include "Rendering/VertexBufferLayout.h"

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogError(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGl Error] (" << error << "): " << function << " "
            << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer()
    : QuadCount(0), m_AspectRatio(1.0f)
{
    VBO = new VertexBuffer(nullptr, MaxQuads * 4 * sizeof(Vertex), GL_DYNAMIC_DRAW);

    VertexBufferLayout layout;

    /*
        1. X, Y
        2. RGBA
        3. U, V
        4. IsCircle
    */

    layout.Push<float>(2);
    layout.Push<float>(4);
    layout.Push<float>(2);
    layout.Push<float>(1);

    VAO = new VertexArray();
    VAO->AddBuffer(*VBO, layout);

    // Indices for quads and cirlces (Quad with shaders);
    unsigned int* indices = new unsigned int[MaxQuads * 6];
    unsigned int offset = 0;
    for (unsigned int i = 0; i < MaxQuads * 6; i += 6)
    {
        indices[i + 0] = offset + 0;
        indices[i + 1] = offset + 1;
        indices[i + 2] = offset + 2;
        indices[i + 3] = offset + 2;
        indices[i + 4] = offset + 3;
        indices[i + 5] = offset + 0;
        offset += 4;
    }

    IBO = new IndexBuffer(indices, MaxQuads * 6);
    delete[] indices;

    m_Shader = new Shader("../../../res/shaders/Batch.shader");
}

Renderer::~Renderer()
{
    delete VAO;
    delete VBO;
    delete IBO;
    delete m_Shader;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::BeginBatch()
{
    Vertices.clear();
    QuadCount = 0;
}

void Renderer::DrawSquare(float x, float y, float size, float width, float r, float g, float b, float a)
{
    if (QuadCount >= MaxQuads)
    {
        // No more cause max Quads
        return;
    }

    float halfSize = size / 2.5f;
    float halfWidth = width / 2.5f;
    halfWidth = halfWidth / m_AspectRatio;

    // Bottom left
    Vertices.push_back({ x - halfWidth, y - halfSize, r, g, b, a, 0.0f, 0.0f, 0.0f });
    // Bottom right
    Vertices.push_back({ x + halfWidth, y - halfSize, r, g, b, a, 1.0f, 0.0f, 0.0f });
    // Top right
    Vertices.push_back({ x + halfWidth, y + halfSize, r, g, b, a, 1.0f, 1.0f, 0.0f });
    // Top left
    Vertices.push_back({ x - halfWidth, y + halfSize, r, g, b, a, 0.0f, 1.0f, 0.0f });


    QuadCount++;
}

void Renderer::SetAspectRatio(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
}   

void Renderer::DrawCircle(float x, float y, float radius, float r, float g, float b, float a)
{
    if (QuadCount >= MaxQuads)
    {
        // No more cause max Quads
        return;
    }

    float halfSize = radius / 3.5f;

    float halfWidth = halfSize / m_AspectRatio;

    // Bottom left
    Vertices.push_back({ x - halfWidth, y - halfSize, r, g, b, a, 0.0f, 0.0f, 1.0f });
    // Bottom right
    Vertices.push_back({ x + halfWidth, y - halfSize, r, g, b, a, 1.0f, 0.0f, 1.0f });
    // Top right
    Vertices.push_back({ x + halfWidth, y + halfSize, r, g, b, a, 1.0f, 1.0f, 1.0f });
    // Top left
    Vertices.push_back({ x - halfWidth, y + halfSize, r, g, b, a, 0.0f, 1.0f, 1.0f });

    QuadCount++;
}

void Renderer::EndBatch()
{
    if (QuadCount == 0)
    {
        return;
    }

    VBO->Bind();
    GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, Vertices.size() * sizeof(Vertex), Vertices.data()));

    m_Shader->Bind();
    VAO->Bind();
    IBO->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, QuadCount * 6, GL_UNSIGNED_INT, nullptr));
}