#ifdef _WIN64
#include "Scene.h"

namespace TheLastOfSH {
	struct MyScene : public Scene {
		MyScene(Renderer* pRenderer):pRenderer(pRenderer) {
		
		
		}

		void Draw() {
			pRenderer->Present();
		}
		void LoadModel(Model* pModel) {}
		Renderer* pRenderer = nullptr;
	};

	Scene* CreateScene(Renderer* pRenderer) {
		MyScene* scene = new MyScene(pRenderer);
		return scene;
	}

	void RemoveScene(Scene* pScene) {
		auto temp = dynamic_cast<MyScene*>(pScene);
		delete temp;
	}
}

#endif
