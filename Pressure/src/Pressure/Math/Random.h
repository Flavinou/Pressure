#pragma once

#include <random>

namespace Pressure
{
	
	class Random
	{
	public:
		static void Init()
		{
			ms_RandomEngine.seed(std::random_device()());
		}

		static float Float()
		{
			return ms_UniformDistribution(ms_RandomEngine) / static_cast<float>(std::numeric_limits<uint32_t>::max());
		}

	private:
		static std::mt19937 ms_RandomEngine;
		static std::uniform_int_distribution<std::mt19937::result_type> ms_UniformDistribution;
	};

}
