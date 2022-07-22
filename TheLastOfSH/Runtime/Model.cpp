#ifdef _WIN64
#include "Model.h"
#include "Vertex.h"
#include "../ThirdParty/json/json.hpp"
#include <fstream>
#include <vector>
namespace TheLastOfSH {

	struct GLTF_Model : public Model {

		GLTF_Model(const char* path) {
			std::ifstream f(path);
			gltf = nlohmann::json::parse(f);
		}


		nlohmann::json gltf;
		std::vector<Vertex> vertex;

	};

	Model* CreateModelFromFile(const char* path) {
		return new GLTF_Model(path);
	}

	void RemoveModel(Model* pModel) {
		//auto temp = dynamic_cast<GLTF_Model*>(pModel);
		//delete temp;
	}
}

#endif
