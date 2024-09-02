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

		std::shared_ptr<Material> mat = std::make_shared<Material>(m_Context, m_Renderer.GetPipelineByName("UnlitColor"));
		mat->SetData("Color", { 1.0f, 0.0f, 0.0f, 1.0f });

		std::shared_ptr<Material> mat2 = std::make_shared<Material>(m_Context, m_Renderer.GetPipelineByName("UnlitTexture"));
		mat2->SetData("albedoTex",m_TestTexture);


#if 1
		Entity entity1 = m_EditorScene->CreateEntity("Custom 1");
		MeshComponent& mesh1 = m_EditorScene->AddComponent<MeshComponent>(entity1);
		MaterialComponent& material1 = m_EditorScene->AddComponent<MaterialComponent>(entity1);
		mesh1.Type = MeshType::Custom;
		mesh1.Mesh = m_TestMesh;
		material1.Material = mat;

		Entity entity2 = m_EditorScene->CreateEntity("Custom 2");
		m_EditorScene->GetComponent<TransformComponent>(entity2).Position = { 0.0f, 3.0f, 0.0f };
		MeshComponent& mesh2 = m_EditorScene->AddComponent<MeshComponent>(entity2);
		MaterialComponent& material2 = m_EditorScene->AddComponent<MaterialComponent>(entity2);
		mesh2.Type = MeshType::Custom;
		mesh2.Mesh = m_TestMesh;
		material2.Material = mat2;

		for (size_t i = 0; i < 10; i++)
		{
			Entity e = m_EditorScene->CreateEntity("Custom");
			m_EditorScene->GetComponent<TransformComponent>(e).Position = { 0.0f, 3 * i, 0.0f };
			MeshComponent& mesh = m_EditorScene->AddComponent<MeshComponent>(e);
			MaterialComponent& material = m_EditorScene->AddComponent<MaterialComponent>(e);
			mesh.Type = MeshType::Custom;
			mesh.Mesh = m_TestMesh;
			material.Material = mat2;
		}


		Entity entity3 = m_EditorScene->CreateEntity("Quad 1");
		m_EditorScene->GetComponent<TransformComponent>(entity3).Position = { 3.0f, 0.0f, 0.0f };
		MeshComponent& mesh3 = m_EditorScene->AddComponent<MeshComponent>(entity3);
		MaterialComponent& material3 = m_EditorScene->AddComponent<MaterialComponent>(entity3);
		mesh3.Type = MeshType::Quad;
		material3.Material = mat;

		Entity entity4 = m_EditorScene->CreateEntity("Cube 1");
		m_EditorScene->GetComponent<TransformComponent>(entity4).Position = { -3.0f, 0.0f, 0.0f };
		MeshComponent& mesh4 = m_EditorScene->AddComponent<MeshComponent>(entity4);
		MaterialComponent& material4 = m_EditorScene->AddComponent<MaterialComponent>(entity4);
		mesh4.Type = MeshType::Cube;
		material4.Material = mat2;

		Entity entity5 = m_EditorScene->CreateEntity("Cube 2");
		m_EditorScene->GetComponent<TransformComponent>(entity5).Position = { -5.0f, 0.0f, 0.0f };
		MeshComponent& mesh5 = m_EditorScene->AddComponent<MeshComponent>(entity5);
		MaterialComponent& material5 = m_EditorScene->AddComponent<MaterialComponent>(entity5);
		mesh5.Type = MeshType::Cube;
		material5.Material = mat;

		Entity entity6 = m_EditorScene->CreateEntity("Quad 2");
		m_EditorScene->GetComponent<TransformComponent>(entity6).Position = { 5.0f, 0.0f, 0.0f };
		MeshComponent& mesh6 = m_EditorScene->AddComponent<MeshComponent>(entity6);
		MaterialComponent& material6 = m_EditorScene->AddComponent<MaterialComponent>(entity6);
		mesh6.Type = MeshType::Quad;
		material6.Material = mat2;

#endif
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
