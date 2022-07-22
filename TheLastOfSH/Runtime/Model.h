#pragma once
#ifdef _WIN64
#include "Vertex.h"
#include <vector>
namespace TheLastOfSH {
	struct Model {
		virtual std::vector<Vertex> GetVertex() const = 0;
	};
	
	Model* CreateModel(const char* path);
	void RemoveModel(Model* pModel);
}
#endif
