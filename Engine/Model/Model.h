#ifndef MODEL__H
#define MODEL__H

#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
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
		std::vector<Texture> m_loadedTextures;
	public:
		Model(const char* _filePath);
		~Model();

		void Draw(Shader& _shader, Camera& _camera);
	private:
		std::vector<unsigned char> LoadBinaryFile();
		std::vector<float> GetFloatsData(json& _accessor);
		std::vector<GLuint> GetIndices(json& _accessor);
		std::vector<Texture> GetTextures(json& _accessor);

		std::vector<Vertex> AssembleVertices(std::vector<glm::vec3>& _positions, std::vector<glm::vec3>& _normals, std::vector<glm::vec2>& _texUVs);

		std::vector<glm::vec2> GroupFloatsVec2(const std::vector<float>& _data);
		std::vector<glm::vec3> GroupFloatsVec3(const std::vector<float>& _data);
		std::vector<glm::vec4> GroupFloatsVec4(const std::vector<float>& _data);
	};
}

#endif