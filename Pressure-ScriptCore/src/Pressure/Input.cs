namespace Pressure
{
	public class Input
	{
		public static bool IsKeyDown(KeyCode keyCode)
		{
			return InternalCalls.Input_IsKeyDown(keyCode);
		}
	}
}
