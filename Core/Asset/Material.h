#ifndef MATERIAL__H
#define MATERIAL__H

#include <cstdint>
#include <memory>

#include "Ressource.h"
#include "../Vector/Vec3.h"
#include "../LlynCore.h"

/*enum class MaterialMode : uint8_t
{
	SPECULAR,
	METALLIC
};*/

class Material : public IRessource
{
private:
	RessourceID m_baseID = TOMBSTONE_RESOURCE;
	RessourceID m_specularID = TOMBSTONE_RESOURCE;
	RessourceID m_normalID = TOMBSTONE_RESOURCE;

	Vec3f m_baseColor = { 1.0f, 1.0f, 1.0f };
	Vec3f m_specularColor = { 1.0f, 1.0f, 1.0f };

	float m_smoothness = 0.5f;
	
	//bool m_alphaClipping = false;
public:
	Material() = default;
	~Material() override;

	Material(const Material&) = default;
	Material& operator=(const Material&) = default;
	Material(Material&&) = default;
	Material& operator=(Material&&) = default;

	bool Load(const std::string& _fileName) override;
	void Unload() override;

	bool Add(const std::string& _filePath, IRessource* _ressource) override;

	void SetBase(const std::string& _texturePath);
	void SetBase(const RessourceID& _textureID);
	RessourceID GetBase() const;

	void SetBaseColor(const Vec3f& _color);
	Vec3f GetBaseColor() const;

	void SetSpecular(const std::string& _texturePath);
	void SetSpecular(const RessourceID& _textureID);
	RessourceID GetSpecular() const;
	void SetSpecularColor(const Vec3f& _color);
	Vec3f GetSpecularColor() const;

	void SetNormal(const std::string& _texturePath);
	void SetNormal(const RessourceID& _textureID);
	RessourceID GetNormal() const;

	void SetSmoothness(float _smoothness);
	float GetSmoothness();
};

#endif