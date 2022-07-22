#pragma once
#ifdef _WIN64

namespace TheLastOfSH {
	struct Model {
	};

	Model* CreateModelFromFile(const char* path);
	void RemoveModel(Model* pModel);
}
#endif
