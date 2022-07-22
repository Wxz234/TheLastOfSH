#ifdef _WIN64
#include "Model.h"
#include "assimp/Importer.hpp"
namespace TheLastOfSH {
	struct MyModel : public Model {
		virtual std::vector<Vertex> GetVertex() const {
			return vertex;
		}
		std::vector<Vertex> vertex;
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
