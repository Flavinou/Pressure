#pragma once

#include <glm/glm.hpp>

namespace Pressure
{
	struct ParticleEmitterComponent;
	class Timestep;

	struct ParticleProps
	{
		glm::vec2 Position{ 0.0f };
		glm::vec2 Velocity{ 0.0f }, VelocityVariation{ 0.0f };
		glm::vec4 ColorBegin{ 1.0f }, ColorEnd{ 1.0f };
		float SizeBegin = 0.5f, SizeEnd = 0.5f, SizeVariation = 0.0f;
		float LifeTime = 1.0f;
	};

	class ParticleSystem
	{
	public:
		static constexpr unsigned int MaxParticles = 10000;

		ParticleSystem();

		void OnUpdate(Timestep ts);
		void OnRender();

		void Emit(const glm::mat4& transform, const ParticleProps& props);

	private:
		struct Particle
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
			glm::vec4 ColorBegin, ColorEnd;
			float Rotation = 0.0f;
			float SizeBegin, SizeEnd;

			float LifeTime = 1.0f;
			float LifeRemaining = 0.0f;

			bool Active = false;
		};

		std::vector<Particle> m_ParticlesPool;
		uint32_t m_PoolIndex = MaxParticles - 1;
	};

}

