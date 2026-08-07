using System;
using System.Runtime.CompilerServices;

namespace Pressure
{
	public static class InternalCalls
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float Engine_GetSpeed();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Engine_SetSpeed(float value);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Engine_GetViewportSize(out Vector2 result);

		[MethodImpl(MethodImplOptions.InternalCall)]	
		internal static extern void Camera_ScreenToWorldPosition(Vector3 screenPosition, out Vector3 result);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_IsEnabled(ulong entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_SetEnabled(ulong entityId, bool enabled);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_GetName(ulong entityId, out string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent(ulong entityId, Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_FindEntityByName(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_Create(string tag);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void Entity_Destroy(ulong entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_Duplicate(string tag);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_DuplicateById(ulong entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object GetScriptInstance(ulong entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_GetTranslation(ulong entityId, out Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_SetTranslation(ulong entityId, ref Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_GetScale(ulong entityId, out Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_SetScale(ulong entityId, ref Vector3 scale);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_GetRotation(ulong entityId, out Vector3 rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_SetRotation(ulong entityId, ref Vector3 rotation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_GetPosition(ulong entityId, out Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_SetPosition(ulong entityId, ref Vector2 position);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_ApplyLinearImpulse(ulong entityId, ref Vector2 impulse, ref Vector2 point, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_ApplyLinearImpulseToCenter(ulong entityId, ref Vector2 impulse, bool wake);		
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_GetLinearVelocity(ulong entityId, out Vector2 linearVelocity);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float RigidBody2DComponent_GetGravityScale(ulong entityId);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_SetGravityScale(ulong entityId, float gravityScale);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern RigidBody2DComponent.BodyType RigidBody2DComponent_GetBodyType(ulong entityId);
		
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_SetBodyType(ulong entityId, RigidBody2DComponent.BodyType bodyType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextComponent_GetText(ulong entityId, out string text);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void TextComponent_SetText(ulong entityId, string text);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_GetColor(ulong entityId, out Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void SpriteRendererComponent_SetColor(ulong entityId, ref Vector4 color);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyDown(KeyCode keyCode);
	}
}
