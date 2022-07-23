#pragma once
#ifdef _WIN64
#include "../Graphics/Renderer.h"

namespace TheLastOfSH {
	struct Scene {
		virtual void Draw() = 0;
		virtual void LoadModelFromFile(const char *filePath) = 0;
	};

	Scene* CreateScene(Renderer* pRenderer);
	void RemoveScene(Scene* pScene);
}
#endif
