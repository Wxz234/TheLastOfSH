#ifdef _WIN64
#include "Model.h"
#include "../ThirdParty/json/json.hpp"
#include <fstream>

namespace TheLastOfSH {

	struct GLTF_Model : public Model {

		GLTF_Model(const char* path) {
			std::ifstream f(path);
			gltf = nlohmann::json::parse(f);
		}

		uint32_t GetMaterialSize() const {
			return 0;
		}

		Material* GetMaterial(uint32_t i) const {
			return nullptr;
		}

		nlohmann::json gltf;

	};

	Model* CreateModelFromFile(const char* path) {
		return new GLTF_Model(path);
	}

	void RemoveModel(Model* pModel) {
		auto temp = dynamic_cast<GLTF_Model*>(pModel);
		delete temp;
	}
}

#endif
