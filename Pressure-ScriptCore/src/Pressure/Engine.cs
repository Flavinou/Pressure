namespace Pressure
{
	public static class Engine
	{
		public static float Speed
		{
			get => InternalCalls.Engine_GetSpeed();
			set => InternalCalls.Engine_SetSpeed(value);
		}

		public static Vector2 ViewportSize
		{
			get
			{
				InternalCalls.Engine_GetViewportSize(out Vector2 result);
				return result;
			}
		}
	}
}
