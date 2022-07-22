#ifdef _WIN64
#include "Model.h"

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
		
	}
}

#endif
