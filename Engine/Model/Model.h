#ifndef MODEL__H
#define MODEL__H

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <json/json.h>
#include "../Macro.h"

using json = nlohmann::json;

namespace ENGINE_NAME
{
	class Mesh;
	class Texture;
	class Shader;
	class Camera;
	struct Vertex;

	class Model
	{
	private:
		const char* m_filePath;
		std::vector<unsigned char> m_data;
		json m_json;

		std::vector<Mesh> m_meshes;
		std::vector<glm::vec3> m_translationsMeshes;
		std::vector<glm::quat> m_rotationsMeshes;
		std::vector<glm::vec3> m_scalesMeshes;
		std::vector<glm::mat4> m_matricesMeshes;

		std::vector<Texture> m_loadedTextures;
	public:
		Model(const char* _filePath);
		~Model();

		void Draw(Shader& _shader, Camera& _camera);
	private:
		void LoadMesh(unsigned int _indMesh);

		void TraverseNode(unsigned int _nextNode, glm::mat4 _matrix = glm::mat4(1.f));

		std::vector<unsigned char> LoadBinaryFile();
		std::vector<float> GetFloats(json& _accessor);
		std::vector<GLuint> GetIndices(json& _accessor);
		std::vector<Texture> GetTextures();

		std::vector<Vertex> AssembleVertices(std::vector<glm::vec3>& _positions, std::vector<glm::vec3>& _normals, std::vector<glm::vec2>& _texUVs);

		std::vector<glm::vec2> GroupFloatsVec2(const std::vector<float>& _data);
		std::vector<glm::vec3> GroupFloatsVec3(const std::vector<float>& _data);
		std::vector<glm::vec4> GroupFloatsVec4(const std::vector<float>& _data);
	};
}

#endif