#pragma once
#ifdef _WIN64
#include <cstdint>
namespace TheLastOfSH {
	struct	Material {
		virtual uint32_t GetID() const = 0;
	};


}
#endif
