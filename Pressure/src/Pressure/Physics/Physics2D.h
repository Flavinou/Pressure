#pragma once

#include <box2d/id.h>

namespace Pressure
{

	struct PhysicsWorldImpl
	{
		b2WorldId WorldId{};
	};

	struct RuntimeBodyImpl
	{
		b2BodyId BodyId{};
	};

}
