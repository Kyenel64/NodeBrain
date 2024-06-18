#pragma once

#include <entt/entt.hpp>

namespace NodeBrain
{
	class Entity
	{
	public:
		Entity() = default;
		explicit Entity(entt::entity handle) : m_Handle(handle) {}
		~Entity() = default;
		Entity(const Entity&) = default;

		// Overrides
		explicit operator bool() const { return m_Handle != entt::null; }
		explicit operator uint32_t() const { return (uint32_t)m_Handle; }
		explicit operator entt::entity() const { return m_Handle; }
		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle; }
		bool operator!=(const Entity& other) const { return m_Handle != other.m_Handle; }

	private:
		entt::entity m_Handle = entt::null;
	};
}

namespace std
{
	template<>
	struct hash<NodeBrain::Entity>
	{
		std::size_t operator()(const NodeBrain::Entity& handle) const
		{
			return hash<uint32_t>()((uint32_t)handle);
		}
	};
}
