#ifdef _WIN64
#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <cstdint>
namespace TheLastOfSH {
	struct MyModel : public Model {

		MyModel(const char* path) {
			scene = importer.ReadFile(path, aiProcess_Triangulate);
			processNode(scene->mRootNode, scene);
		}

		std::vector<Vertex> GetVertex() const {
			return mVertex;
		}

		void processNode(aiNode* node, const aiScene* scene)
		{
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				processMesh(mesh, scene);
			}
			// 接下来对它的子节点重复这一过程
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				processNode(node->mChildren[i], scene);
			}
		}

		void processMesh(aiMesh* mesh, const aiScene* scene)
		{
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex vertex{};
				vertex.Position[0] = mesh->mVertices[i].x;
				vertex.Position[1] = mesh->mVertices[i].y;
				vertex.Position[2] = mesh->mVertices[i].z;
				vertex.Normal[0] = mesh->mNormals[i].x;
				vertex.Normal[1] = mesh->mNormals[i].y;
				vertex.Normal[2] = mesh->mNormals[i].z;
				vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
				vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;
				

				mVertex.push_back(vertex);
			}

			// 处理索引

			// 处理材质
			if (mesh->mMaterialIndex >= 0)
			{
			
			}
		}
		std::vector<Vertex> mVertex;
		std::vector<uint16_t> mIndex;

		Assimp::Importer importer;
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
