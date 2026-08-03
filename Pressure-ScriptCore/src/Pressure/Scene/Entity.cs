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

		public bool Enabled
		{
			get => InternalCalls.Entity_IsEnabled(Id);
			set => InternalCalls.Entity_SetEnabled(Id, value);
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

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(Id, out Vector3 result);
				return result;
			}
			set => InternalCalls.TransformComponent_SetScale(Id, ref value);
		}

		public string Name
		{
			get
			{
				InternalCalls.Entity_GetName(Id, out string result);
				return result;
			}
		}

		public static Entity Create(string tag)
		{
			ulong id = InternalCalls.Entity_Create(tag);
			return new Entity(id);
		}

		public static Entity Instantiate(string tag)
		{
			ulong id = InternalCalls.Entity_Duplicate(tag);
			return new Entity(id);
		}

		public static Entity Instantiate(Entity other)
		{
			ulong id = InternalCalls.Entity_DuplicateById(other.Id);
			return new Entity(id);
		}

		public void Destroy()
		{
			InternalCalls.Entity_Destroy(Id);
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

		public Entity FindEntityByName(string name)
		{
			ulong entityId = InternalCalls.Entity_FindEntityByName(name);
			if (entityId == 0)
				return null;

			return new Entity(entityId);
		}

		public T As<T>() where T : Entity, new()
		{
			object scriptInstance = InternalCalls.GetScriptInstance(Id);
			if (scriptInstance is T typedInstance)
				return typedInstance;

			return null;
		}
	}
}
