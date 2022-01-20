#include <Pressure.h>

class Sandbox : public Pressure::Application 
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Pressure::Application* Pressure::CreateApplication()
{
	return new Sandbox();
}