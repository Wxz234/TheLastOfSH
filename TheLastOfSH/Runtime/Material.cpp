#ifdef _WIN64
#include "Material.h"

namespace TheLastOfSH {
	struct MyPBRMaterial : public Material {

		MyPBRMaterial() {
			static uint32_t i = 0;
			id = i++;
		}

		uint32_t GetID() const {
			return id;
		}

		uint32_t id;
	};
}

#endif
