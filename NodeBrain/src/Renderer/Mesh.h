#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "Renderer/RenderContext.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"

namespace NodeBrain
{
    struct MeshVertexData
    {
        glm::vec3 Position;
        float UVX;
        glm::vec3 Normal;
        float UVY;
    };

    class SubMesh
    {
    public:
        SubMesh(std::vector<MeshVertexData> vertices, std::vector<uint32_t> indices, std::vector<std::shared_ptr<Texture2D>> textures);

    private:
        std::vector<MeshVertexData> m_Vertices;
        std::vector<uint32_t> m_Indices;
        std::vector<std::shared_ptr<Texture2D>> m_Textures;

    public:
        friend class Mesh;
    };



    class Mesh
    {
    public:
        Mesh(RenderContext& context, const std::filesystem::path& path);

        [[nodiscard]] std::shared_ptr<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
        [[nodiscard]] std::shared_ptr<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

    private:
        void ProcessNode(const aiNode* node, const aiScene* scene);

    private:
        RenderContext& m_Context;
        std::filesystem::path m_Path;
        std::vector<SubMesh> m_SubMeshes;

        std::shared_ptr<VertexBuffer> m_VertexBuffer;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;

        // TODO: texture
    };
}
