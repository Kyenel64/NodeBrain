#pragma once

#include <entt/entt.hpp>

namespace NodeBrain
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(uint32_t handle) : m_Handle((entt::entity)handle) {}
		~Entity() = default;

		// Overrides
		operator bool() const { return m_Handle != entt::null; }
		operator uint32_t() const { return (uint32_t)m_Handle; }
		operator entt::entity() const { return m_Handle; }
		bool operator==(const Entity& other) const { return m_Handle == other.m_Handle; }
		bool operator!=(const Entity& other) const { return m_Handle != other.m_Handle; }

	private:
		entt::entity m_Handle = entt::null;
	};
}
