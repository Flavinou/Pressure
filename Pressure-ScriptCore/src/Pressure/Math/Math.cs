namespace Pressure
{
	public static class Math
	{
		public static float Lerp(float a, float b, float t)
		{
			return a * (1.0f - t) + b * t;
		}

		public static float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}
	}
}
