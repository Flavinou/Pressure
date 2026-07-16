#include "prspch.h"
#include "UUID.h"

#include <random>

namespace Pressure
{

	namespace
	{
		std::random_device s_RandomDevice;
		std::mt19937_64 s_RandomGenerator(s_RandomDevice());
		std::uniform_int_distribution<uint64_t> s_UniformDistribution;
	}

	UUID::UUID()
		: m_UUID(s_UniformDistribution(s_RandomGenerator))
	{
	}

	UUID::UUID(uint64_t uuid)
		: m_UUID(uuid)
	{
	}
}
