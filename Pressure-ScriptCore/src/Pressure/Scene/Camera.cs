namespace Pressure
{
	public static class Camera
	{
		public static Vector3 ScreenToWorldPosition(Vector3 screenPosition)
		{
			InternalCalls.Camera_ScreenToWorldPosition(screenPosition, out Vector3 result);
			return result;
		}
	}
}
