#ifdef _WIN64
#include "Model.h"
#include "../ThirdParty/json/json.hpp"
namespace TheLastOfSH {

	struct GLTF_Model : public Model {
		uint32_t GetMaterialSize() const {
			return 0;
		}

		Material* GetMaterial(uint32_t i) const {
			return nullptr;
		}
	};

	Model* CreateModelFromFile(const char* path) {
		return new GLTF_Model;
	}

	void RemoveModel(Model* pModel) {
		auto temp = dynamic_cast<GLTF_Model*>(pModel);
		delete temp;
	}
}

#endif
