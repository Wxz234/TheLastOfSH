#pragma once
#ifdef _WIN64
#include <cstdint>

namespace TheLastOfSH {
	struct Vertex {
		float pos[3];
		float normal[3];
		float uv[2];
		uint32_t materialID;
	};
}
#endif

