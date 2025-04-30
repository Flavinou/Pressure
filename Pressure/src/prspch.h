#pragma once

#include "Pressure/Core/PlatformDetection.h"

#ifdef PRS_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif


#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Pressure/Core/Base.h"

#include "Pressure/Core/Log.h"

#include "Pressure/Debug/Instrumentor.h"

#ifdef PRS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
