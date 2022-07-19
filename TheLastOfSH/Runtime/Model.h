#pragma once
#ifdef _WIN64
#include "Material.h"
#include <cstdint>

namespace TheLastOfSH {
	struct Model {
		virtual uint32_t GetMaterialSize() const = 0;
		virtual Material* GetMaterial(uint32_t i) const = 0;
	};

	Model* CreateModelFromFile(const char* path);
	void RemoveModel(Model* pModel);
}
#endif
