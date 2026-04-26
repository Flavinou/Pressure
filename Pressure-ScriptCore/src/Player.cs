using System;

using Pressure;

namespace Sandbox
{
	public class Player : Entity
	{
		private RigidBody2DComponent _rigidBody;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate : {Id}");

			_rigidBody = GetComponent<RigidBody2DComponent>();
		}

		void OnUpdate(float ts) 
		{
			Console.WriteLine($"Player.OnUpdate : {ts}");

			const float speed = 0.25f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.W))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;

			velocity *= speed;

			_rigidBody.ApplyLinearImpulse(velocity.XY, true);
		}
	}
}
