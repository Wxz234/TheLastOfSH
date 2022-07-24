#pragma once
#ifdef _WIN64
#include <cstdint>
namespace TheLastOfSH {
	struct Vertex {
		float Position[3];
		float Normal[3];
		float TexCoords[2];
		int32_t MaterialID;
	};

}
#endif
