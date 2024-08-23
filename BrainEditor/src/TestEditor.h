#pragma once

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
    class TestEditor
    {
    public:
        TestEditor(Renderer& renderer);
        ~TestEditor();

        void OnUpdate(float deltaTime);
        void OnEvent(Event& e);
        void OnUpdateGUI() {}

    private:
        void OnWindowResize(WindowResizedEvent& event);

    private:
        Renderer& m_Renderer;
        RendererAPI& m_RendererAPI;
        RenderContext& m_Context;
        Window& m_Window;

        std::shared_ptr<EditorCamera> m_EditorCamera;
        std::shared_ptr<Scene> m_EditorScene;

        glm::vec2 m_ViewportSize;

        // Temp
        std::shared_ptr<Texture2D> m_TestTexture;
        std::shared_ptr<Texture2D> m_BrickTexture;

        std::shared_ptr<Mesh> m_TestMesh;

    };
}
