using System;
using System.Runtime.CompilerServices;

namespace Pressure
{
	public static class InternalCalls
	{
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void NativeLog(string message, int parameter);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern void NativeLog_Vector3(ref Vector3 parameter, out Vector3 result);

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal static extern float NativeLog_Vector3Dot(ref Vector3 parameter);
	}

	public struct Vector3
	{
		public float X;
		public float Y;
		public float Z;
		public Vector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}
	}

	public class Entity
	{
		public Entity()
		{
			Console.WriteLine("Hello World - Main constructor!");
			Log("Whatever native message you want to display!", 42);

			var pos = new Vector3(5, 2.5f, 1);
			var result = Log(pos);
			Console.WriteLine($"({result.X}, {result.Y}, {result.Z})");
			Console.WriteLine($"{InternalCalls.NativeLog_Vector3Dot(ref pos)}");
		}

		public void PrintMessage()
		{
			Console.WriteLine("Hello from PrintMessage C#!");
		}

		public void PrintInts(int value1, int value2)
		{
			Console.WriteLine($"Int values: {value1}, {value2}");
		}

		public void PrintCustomMessage(string message)
		{
			Console.WriteLine($"C# says: {message}");
		}

		private void Log(string text, int parameter)
		{
			InternalCalls.NativeLog(text, parameter);
		}

		private Vector3 Log(Vector3 vector)
		{
			InternalCalls.NativeLog_Vector3(ref vector, out Vector3 result);
			return result;
		}
	}
}
