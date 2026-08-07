namespace Pressure
{
	public static class Math
	{
		public static float Lerp(float a, float b, float t)
		{
			return a * (1.0f - t) + b * t;
		}

		public static Vector2 Lerp(Vector2 a, Vector2 b, float t)
		{
			return new Vector2(
				a.X * (1.0f - t) + b.X * t,
				a.Y * (1.0f - t) + b.Y * t
			);
		}

		public static Vector3 Lerp(Vector3 a, Vector3 b, float t)
		{
			return new Vector3(
				a.X * (1.0f - t) + b.X * t,
				a.Y * (1.0f - t) + b.Y * t,
				a.Z * (1.0f - t) + b.Z * t
			);
		}

		public static float Clamp(float value, float min, float max)
		{
			if (value < min) return min;
			if (value > max) return max;
			return value;
		}

		public static float DegreesToRadians(float degrees)
		{
			return (float)(System.Math.PI * degrees / 180.0f);
		}
	}
}
