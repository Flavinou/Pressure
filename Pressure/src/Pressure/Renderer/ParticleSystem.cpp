#include "prspch.h"
#include "ParticleSystem.h"

#include "Pressure/Core/Timestep.h"
#include "Pressure/Math/Random.h"
#include "Pressure/Renderer/Renderer2D.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Pressure
{

	ParticleSystem::ParticleSystem()
	{
		m_ParticlesPool.resize(MaxParticles);
	}

	void ParticleSystem::OnUpdate(Timestep ts)
	{
		for (auto& particle : m_ParticlesPool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			particle.LifeRemaining -= ts;
			particle.Position += particle.Velocity * static_cast<float>(ts);
			particle.Rotation += 0.01f * ts;
		}
	}

	void ParticleSystem::OnRender()
	{
		for (const auto& particle : m_ParticlesPool)
		{
			if (!particle.Active)
				continue;

			float life = particle.LifeRemaining / particle.LifeTime;
			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);

			// Batch rendered
			// TODO: Look into instancing maybe ?
			Renderer2D::DrawRotatedQuad(particle.Position, { size, size }, particle.Rotation, color);
		}
	}

	void ParticleSystem::Emit(const glm::mat4& transform, const ParticleProps& props)
	{
		Particle& particle = m_ParticlesPool[m_PoolIndex];
		particle.Active = true;
		particle.Position = { transform[3][0] + props.Position.x, transform[3][1] + props.Position.y };
		particle.Rotation = Random::Float() * 2.0f * glm::pi<float>(); // TODO: Pass from props

		// Velocity
		particle.Velocity = props.Velocity;
		particle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
		particle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);

		// Color
		particle.ColorBegin = props.ColorBegin;
		particle.ColorEnd = props.ColorEnd;

		particle.LifeTime = props.LifeTime;
		particle.LifeRemaining = props.LifeTime;
		particle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = props.SizeEnd;

		m_PoolIndex = --m_PoolIndex % m_ParticlesPool.size();
	}

}
