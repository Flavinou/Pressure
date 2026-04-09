#pragma once

#include <xhash>

namespace Pressure
{

	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID; }

	private:
		uint64_t m_UUID;
	};

}

namespace std
{
	
	template<>
	struct hash<Pressure::UUID>
	{
		size_t operator()(const Pressure::UUID& uuid) const noexcept
		{
			return hash<uint64_t>()(uuid);
		}
	};

}
