#include "NBpch.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace NodeBrain
{
    static SubMesh ProcessSubMesh(const aiMesh* mesh, const aiScene* scene)
    {
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        std::vector<std::shared_ptr<Texture2D>> textures;

        // Iterate through each mesh vertex
        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            VertexData vertex = {};

            // Position
            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            // Normals
            if (mesh->HasNormals())
            {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
            }

            // TexCoords
            if (mesh->mTextureCoords[0])
            {
                vertex.UVX = mesh->mTextureCoords[0][i].x;
                vertex.UVY = mesh->mTextureCoords[0][i].y;
            }
            else
            {
                vertex.UVX = 0.0f;
                vertex.UVY = 0.0f;
            }

            vertex.Color = { 1.0f, 1.0f, 1.0f, 1.0f };

            vertices.push_back(vertex);
        }

        // Iterate through indices
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        return { vertices, indices, textures };
    }

    SubMesh::SubMesh(std::vector<VertexData> vertices, std::vector<uint32_t> indices, std::vector<std::shared_ptr<Texture2D>> textures)
        : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Textures(std::move(textures))
    {

    }


    Mesh::Mesh(RenderContext& context, const std::filesystem::path& path)
        : m_Context(context), m_Path(path)
    {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path.string().c_str(),
            aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

        NB_ASSERT(scene, importer.GetErrorString());
        NB_ASSERT(scene->mRootNode, importer.GetErrorString());

        ProcessNode(scene->mRootNode, scene);

        m_VertexBuffer = VertexBuffer::Create(m_Context, m_SubMeshes[0].m_Vertices.data(), sizeof(VertexData) * m_SubMeshes[0].m_Vertices.size());
        m_IndexBuffer = IndexBuffer::Create(m_Context, m_SubMeshes[0].m_Indices.data(), sizeof(uint32_t) * m_SubMeshes[0].m_Indices.size());
    }

    void Mesh::ProcessNode(const aiNode* node, const aiScene* scene)
    {
        // Process each mesh in node
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_SubMeshes.push_back(ProcessSubMesh(mesh, scene));
        }

        // Process children nodes
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }
    }

}
