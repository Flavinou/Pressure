#pragma once

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

#include "Pressure/Core/Log.h"

#include "Pressure/Debug/Instrumentor.h"

#ifdef PRS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif