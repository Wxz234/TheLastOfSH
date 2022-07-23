#ifdef _WIN64
#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
namespace TheLastOfSH {
	struct MyModel : public Model {

		MyModel(const char* path) {
			Assimp::Importer importer;
			scene = importer.ReadFile(path, aiProcess_Triangulate);
		}

		~MyModel() {

		}
		virtual std::vector<Vertex> GetVertex() const {
			return vertex;
		}
		std::vector<Vertex> vertex;
		const aiScene* scene;
	};

	Model* CreateModel(const char* path) {
		return nullptr;
	}
	void RemoveModel(Model* pModel) {
		auto temp = dynamic_cast<MyModel*>(pModel);
		delete temp;
	}
}

#endif
