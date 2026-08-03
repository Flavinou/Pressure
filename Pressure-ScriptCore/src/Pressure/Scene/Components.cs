namespace Pressure
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }
	}

	public class TransformComponent : Component
	{
		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(Entity.Id, out Vector3 result);
				return result;
			}
			set => InternalCalls.TransformComponent_SetTranslation(Entity.Id, ref value);
		}
	}

	public class RigidBody2DComponent : Component
	{
		public enum BodyType
		{
			Static = 0,
			Dynamic,
			Kinematic,
		}

		public Vector2 Position
		{
			get
			{
				InternalCalls.RigidBody2DComponent_GetPosition(Entity.Id, out Vector2 result);
				return result;
			}
			set => InternalCalls.RigidBody2DComponent_SetPosition(Entity.Id, ref value);
		}

		public Vector2 LinearVelocity
		{
			get
			{
				InternalCalls.RigidBody2DComponent_GetLinearVelocity(Entity.Id, out Vector2 result);
				return result;
			}
		}

		public BodyType Type
		{
			get => InternalCalls.RigidBody2DComponent_GetBodyType(Entity.Id);
			set => InternalCalls.RigidBody2DComponent_SetBodyType(Entity.Id, value);
		}

		public float GravityScale
		{
			get => InternalCalls.RigidBody2DComponent_GetGravityScale(Entity.Id);
			set => InternalCalls.RigidBody2DComponent_SetGravityScale(Entity.Id, value);
		}

		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
		{
			InternalCalls.RigidBody2DComponent_ApplyLinearImpulse(Entity.Id, ref impulse, ref worldPosition, wake);
		}

		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
		{
			InternalCalls.RigidBody2DComponent_ApplyLinearImpulseToCenter(Entity.Id, ref impulse, wake);
		}
	}

	public class TextComponent : Component
	{
		public string Text
		{
			get
			{
				InternalCalls.TextComponent_GetText(Entity.Id, out string result);
				return result;
			}
			set => InternalCalls.TextComponent_SetText(Entity.Id, value);
		}
	}
}
