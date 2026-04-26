using System;

namespace Pressure
{
	public class Entity
	{
		public readonly ulong Id;

		protected Entity() { Id = 0; }

		internal Entity(ulong id)
		{
			Id = id;
		}

		public Vector3 Translation
		{
			get
			{
				InternalCalls.TransformComponent_GetTranslation(Id, out Vector3 result);
				return result;
			}
			set => InternalCalls.TransformComponent_SetTranslation(Id, ref value);
		}

		public bool HasComponent<T>() where T : Component, new()
		{
			Type componentType = typeof(T);
			return InternalCalls.Entity_HasComponent(Id, componentType);
		}
		
		public T GetComponent<T>() where T : Component, new()
		{
			if (!HasComponent<T>())
				return null;
		
			T component = new T { Entity = this };
			return component;
		}
	}
}
