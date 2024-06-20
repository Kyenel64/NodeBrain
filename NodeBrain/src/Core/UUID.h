#pragma once

namespace NodeBrain
{
	class UUID
	{
	public:
		UUID();
		UUID(const UUID&) = default;
		~UUID() = default;

		explicit operator uint64_t() const { return m_UUID; }

		bool operator==(const UUID& other) const
		{
			return m_UUID == other.m_UUID;
		}

		bool operator!=(const UUID& other) const
		{
			return m_UUID != other.m_UUID;
		}
	private:
		uint64_t m_UUID;
	};
}

namespace std
{
	template<>
	struct hash<NodeBrain::UUID>
	{
		std::size_t operator()(const NodeBrain::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}