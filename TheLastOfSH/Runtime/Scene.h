#pragma once
#ifdef _WIN64
#include "../Graphics/Renderer.h"
#include "Model.h"

namespace TheLastOfSH {
	struct Scene {
		virtual void Draw() = 0;
		virtual void LoadModel(Model* pModel) = 0;
	};

	Scene* CreateScene(Renderer* pRenderer);
	void RemoveScene(Scene* pScene);
}
#endif
