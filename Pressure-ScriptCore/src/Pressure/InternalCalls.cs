using System;
using System.Runtime.CompilerServices;

namespace Pressure
{
	public static class InternalCalls
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Entity_HasComponent(ulong entityId, Type componentType);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern ulong Entity_FindEntityByName(string name);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern object GetScriptInstance(ulong entityId);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_GetTranslation(ulong entityId, out Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool TransformComponent_SetTranslation(ulong entityId, ref Vector3 translation);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_ApplyLinearImpulse(ulong entityId, ref Vector2 impulse, ref Vector2 point, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void RigidBody2DComponent_ApplyLinearImpulseToCenter(ulong entityId, ref Vector2 impulse, bool wake);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern bool Input_IsKeyDown(KeyCode keyCode);
	}
}
