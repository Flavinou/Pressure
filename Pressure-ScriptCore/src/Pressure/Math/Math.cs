namespace Pressure
{
	public static class Math
	{
		public static float Lerp(float a, float b, float t)
		{
			return a * (1.0f - t) + b * t;
		}
	}
}
