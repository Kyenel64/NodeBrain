#include "TestEditor.h"

namespace NodeBrain
{
    TestEditor::TestEditor(Renderer& renderer)
        : m_Renderer(renderer), m_RendererAPI(renderer.GetAPI()), m_Context(m_Renderer.GetContext()), m_Window(m_Renderer.GetContext().GetWindow())
    {
        m_EditorCamera = std::make_shared<EditorCamera>(45.0f, (float)m_Window.GetWidth() / (float)m_Window.GetHeight(), 0.01f, 1000.0f);
        m_EditorScene = std::make_shared<Scene>(m_Renderer);
        m_ViewportSize = { m_Window.GetWidth(), m_Window.GetHeight() };


        // --- Test Entities ---
        m_TestTexture = Texture2D::Create(m_Context, "Assets/Textures/TestTexture.png");
        Entity entity = m_EditorScene->CreateEntity("Test Cube");
        MeshComponent& mesh = m_EditorScene->AddComponent<MeshComponent>(entity);
        MaterialComponent& material = m_EditorScene->AddComponent<MaterialComponent>(entity);
        material.Texture = m_TestTexture;
        mesh.Type = MeshType::Cube;

        m_BrickTexture = Texture2D::Create(m_Context, "Assets/Textures/BrickTexture.jpg");
        Entity entity2 = m_EditorScene->CreateEntity("Test Plane");
        MeshComponent& mesh2 = m_EditorScene->AddComponent<MeshComponent>(entity2);
        m_EditorScene->GetComponent<TransformComponent>(entity2).Position = { 1.5f, 0.0f, 0.0f };
        MaterialComponent& material2 = m_EditorScene->AddComponent<MaterialComponent>(entity2);
        material2.Texture = m_BrickTexture;
        material2.Color = { 0.0f, 1.0f, 0.0f, 1.0f };
        mesh2.Type = MeshType::Plane;

        m_TestMesh = std::make_shared<Mesh>(m_Context, "Assets/Models/teapot.obj");
        Entity entity3 = m_EditorScene->CreateEntity("Test Mesh");
        MeshComponent& mesh3 = m_EditorScene->AddComponent<MeshComponent>(entity3);
        MaterialComponent& material3 = m_EditorScene->AddComponent<MaterialComponent>(entity3);
        mesh3.Type = MeshType::Custom;
        mesh3.Mesh = m_TestMesh;
    }

    TestEditor::~TestEditor()
    {

    }

    void TestEditor::OnUpdate(float deltaTime)
    {
        m_EditorCamera->OnUpdate(deltaTime);
        m_EditorScene->OnEditorUpdate(m_EditorCamera);
    }

    void TestEditor::OnEvent(Event& event)
    {
        event.AttachEventFunction<WindowResizedEvent>([this](WindowResizedEvent& event) { OnWindowResize(event); });
    }

    void TestEditor::OnWindowResize(WindowResizedEvent& event)
    {
        m_ViewportSize = { m_Window.GetWidth(), m_Window.GetHeight() };
        m_EditorCamera->Resize(m_ViewportSize.x, m_ViewportSize.y);
    }
}
