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
        m_BrickTexture = Texture2D::Create(m_Context, "Assets/Textures/BrickTexture.jpg");
        m_TestMesh = std::make_shared<Mesh>(m_Context, "Assets/Models/DefaultCube.obj");

        Entity entity1 = m_EditorScene->CreateEntity("Test Mesh");
        MeshComponent& mesh1 = m_EditorScene->AddComponent<MeshComponent>(entity1);
        MaterialComponent& material1 = m_EditorScene->AddComponent<MaterialComponent>(entity1);
        mesh1.Type = MeshType::Custom;
        mesh1.Mesh = m_TestMesh;
        material1.Material = std::make_shared<Material>(m_Context, m_Renderer.GetPipelineByName("UnlitColor"));
        material1.Material->SetData("Color", { 1.0f, 0.0f, 0.0f, 1.0f });

        Entity entity2 = m_EditorScene->CreateEntity("Test Mesh 2");
        m_EditorScene->GetComponent<TransformComponent>(entity2).Position = { 0.0f, 0.0f, 0.0f };
        MeshComponent& mesh2 = m_EditorScene->AddComponent<MeshComponent>(entity2);
        MaterialComponent& material2 = m_EditorScene->AddComponent<MaterialComponent>(entity2);
        mesh2.Type = MeshType::Quad;
        //mesh2.Mesh = m_TestMesh;
        material2.Material = std::make_shared<Material>(m_Context, m_Renderer.GetPipelineByName("UnlitTexture"));
        material2.Material->SetData("Albedo", m_TestTexture);

        Entity entity3 = m_EditorScene->CreateEntity("Test Mesh 3");
        m_EditorScene->GetComponent<TransformComponent>(entity3).Position = { 2.0f, 0.0f, 0.0f };
        MeshComponent& mesh3 = m_EditorScene->AddComponent<MeshComponent>(entity3);
        MaterialComponent& material3 = m_EditorScene->AddComponent<MaterialComponent>(entity3);
        mesh3.Type = MeshType::Quad;
        material3.Material = material2.Material;
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
