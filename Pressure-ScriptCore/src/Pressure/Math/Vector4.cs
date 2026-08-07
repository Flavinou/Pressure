namespace Pressure
{
	public struct Vector4
	{
		public float X;
		public float Y;
		public float Z;
		public float W;
			
		public float R
		{
			get => X;
			set => X = value;
		}

		public float G
		{
			get => Y;
			set => Y = value;
		}

		public float B
		{
			get => Z;
			set => Z = value;
		}

		public float A
		{
			get => W;
			set => W = value;
		}

		public static Vector4 Zero => new Vector4(0.0f);
		public static Vector4 One => new Vector4(1.0f);

		public float Length => (float)System.Math.Sqrt(LengthSquared);

		public float LengthSquared => X * X + Y * Y + Z * Z + W * W;

		public Vector2 XY
		{
			get => new Vector2(X, Y);
			set
			{
				X = value.X;
				Y = value.Y;
			}
		}

		public Vector3 XYZ
		{
			get => new Vector3(X, Y, Z);
			set
			{
				X = value.X;
				Y = value.Y;
				Z = value.Z;
			}
		}

		public Vector3 RGB
		{
			get => new Vector3(R, G, B);
			set
			{
				R = value.X;
				G = value.Y;
				B = value.Z;
			}
		}

		private static readonly float TOLERANCE = 0.0001f;

		public static bool operator ==(Vector4 a, Vector4 b)
		{
			return System.Math.Abs(a.X - b.X) < TOLERANCE
			       && System.Math.Abs(a.Y - b.Y) < TOLERANCE
			       && System.Math.Abs(a.Z - b.Z) < TOLERANCE
			       && System.Math.Abs(a.W - b.W) < TOLERANCE;
		}

		public static bool operator !=(Vector4 a, Vector4 b)
		{
			return !(a == b);
		}

		public static Vector4 operator +(Vector4 a, Vector4 b)
		{
			return new Vector4(a.X + b.X, a.Y + b.Y, a.Z + b.Z, a.W + b.W);
		}

		public static Vector4 operator -(Vector4 a, Vector4 b)
		{
			return new Vector4(a.X - b.X, a.Y - b.Y, a.Z - b.Z, a.W - b.W);
		}

		public static Vector4 operator *(Vector4 vector, float scalar)
		{
			return new Vector4(vector.X * scalar, vector.Y * scalar, vector.Z * scalar, vector.W * scalar);
		}

		public Vector4(float scalar)
		{
			X = scalar;
			Y = scalar;
			Z = scalar;
			W = scalar;
		}

		public Vector4(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		public Vector4(Vector2 xy, float z, float w)
		{
			X = xy.X;
			Y = xy.Y;
			Z = z;
			W = w;
		}

		public Vector4(Vector3 xyz, float w)
		{
			X = xyz.X;
			Y = xyz.Y;
			Z = xyz.Z;
			W = w;
		}

		public Vector4 Normalized()
		{
			var length = Length;
			return length == 0
				? Zero
				: new Vector4(X / length, Y / length, Z / length, W / length);
		}
	}
}
