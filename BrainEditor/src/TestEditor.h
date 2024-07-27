#pragma once

#include <NodeBrain/NodeBrain.h>

namespace NodeBrain
{
    class TestEditor : public Layer
    {
    public:
        TestEditor(Renderer& renderer);
        ~TestEditor() override;

        void OnUpdate(float deltaTime) override;
        void OnEvent(Event& e) override;
        void OnUpdateGUI() override {}

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
