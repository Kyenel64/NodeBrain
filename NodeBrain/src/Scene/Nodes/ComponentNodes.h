#pragma once

#include "Scene/Nodes/Node.h"

namespace NodeBrain
{
	class TagComponentNode : public Node
	{
	public:
		explicit TagComponentNode(TagComponent& tagComp)
				: m_TagComponent(tagComp), Node(NodeType::TagComponent)
		{
			NB_PROFILE_FN();

			m_InputPorts.emplace_back(*this, PortDataType::String, std::string());
		}

		void Evaluate() override
		{
			NB_PROFILE_FN();

			m_TagComponent.Tag = std::get<std::string>(m_InputPorts[0].GetValue());
		}

	private:
		TagComponent& m_TagComponent;
	};



	class TransformComponentNode : public Node
	{
	public:
		explicit TransformComponentNode(TransformComponent& transformComp)
				: m_TransformComponent(transformComp), Node(NodeType::TransformComponent)
		{
			NB_PROFILE_FN();

			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(0.0f));
			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(0.0f));
			m_InputPorts.emplace_back(*this, PortDataType::Vec3, glm::vec3(1.0f));
		}

		void Evaluate() override
		{
			NB_PROFILE_FN();

			m_TransformComponent.Position = std::get<glm::vec3>(m_InputPorts[0].GetValue());
			m_TransformComponent.SetEulerRotation(std::get<glm::vec3>(m_InputPorts[1].GetValue()));
			m_TransformComponent.Scale = std::get<glm::vec3>(m_InputPorts[2].GetValue());
		}

	private:
		TransformComponent& m_TransformComponent;
	};



	class MeshComponentNode : public Node
	{
	public:
		explicit MeshComponentNode(MeshComponent& meshComp)
				: m_MeshComponent(meshComp), Node(NodeType::MeshComponent)
		{
			NB_PROFILE_FN();

			m_InputPorts.emplace_back(*this, PortDataType::Int, 0);
		}

		void Evaluate() override
		{
			NB_PROFILE_FN();

			m_MeshComponent.Type = MeshType(std::get<int>(m_InputPorts[0].GetValue()));
		}

	private:
		MeshComponent& m_MeshComponent;
	};



	class MaterialComponentNode : public Node
	{
	public:
		explicit MaterialComponentNode(MaterialComponent& matComp)
				: m_MaterialComponent(matComp), Node(NodeType::MaterialComponent)
		{
			NB_PROFILE_FN();

		}

		void Evaluate() override
		{
			NB_PROFILE_FN();

		}

	private:
		MaterialComponent& m_MaterialComponent;
	};
}
