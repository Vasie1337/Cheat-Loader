#pragma once 
#include "../../util/util.h"
#include "../sdk/sdk.h"

class Esp {
public:
	static void ActorLoop();
};

inline bool bCornerBox = true;
inline bool bSnapLines = true;
inline bool bCrossHair = true;

namespace Debug {
	static bool PrintPointers = false;
	static bool PrintLocations = false;
}