#ifndef MODEL__H
#define MODEL__H
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/fwd.hpp>
#include <glm/mat4x4.hpp>

namespace Llyn
{
	class Shader;
	class Mesh;
	class Material;
	class Camera;

	class Model
	{
	private:
		std::vector<Mesh*> m_meshes;
		std::string m_directory;
		std::vector<glm::mat4> m_models;
	public:
		Model(const char* _path);
		~Model();

		void Draw(Shader& _shader, Camera& _camera, glm::mat4 _model = glm::mat4(1.f));
	private:
		void LoadModel(std::string _path);
		void ProcessNode(aiNode* _node, const aiScene* _scene, glm::mat4& _parentModel);
		Mesh* ProcessMesh(aiMesh* _mesh, const aiScene* _scene) const;
		Material* LoadMaterial(aiMaterial* _mat) const;
	};
}

#endif