using System;

using Pressure;

namespace Sandbox
{
	public class Player : Entity
	{
		private RigidBody2DComponent _rigidBody;

		// Exposed to the editor
		public float Speed = 0.25f;
		public float Time = 0.0f;

		void OnCreate()
		{
			Console.WriteLine($"Player.OnCreate : {Id}");

			_rigidBody = GetComponent<RigidBody2DComponent>();
		}

		void OnUpdate(float ts)
		{
			Time += ts;

			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.W))
				velocity.Y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.S))
				velocity.Y = -1.0f;

			if (Input.IsKeyDown(KeyCode.A))
				velocity.X = -1.0f;
			else if (Input.IsKeyDown(KeyCode.D))
				velocity.X = 1.0f;

			velocity *= Speed * ts;

			_rigidBody.ApplyLinearImpulse(velocity.XY, true);
		}
	}
}
