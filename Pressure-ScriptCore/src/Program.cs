using System;

namespace Pressure
{
	public class Program
	{
		public float FloatVar { get; set; }

		public static void Main(string[] args)
		{
			Console.WriteLine("Hello World!");
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
	}
}
