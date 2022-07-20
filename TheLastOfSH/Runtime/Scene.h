#pragma once
#ifdef _WIN64
#include "../Graphics/Renderer.h"
#include "Model.h"

#include <cstdint>
namespace TheLastOfSH {
	struct Scene {
		virtual void Draw() = 0;
		virtual void LoadModel(Model* pModel) = 0;
	};

	Scene* CreateScene(Renderer* pRenderer, uint32_t w, uint32_t h);
	void RemoveScene(Scene* pScene);
}
#endif
