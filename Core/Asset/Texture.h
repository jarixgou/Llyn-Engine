#ifndef TEXTURE__H
#define TEXTURE__H

#include "BaseAssetVisitor.h"
#include "../LlynCore.h"

#include "Ressource.h"
#include "../Vector/FwdVec2.h"

#include "TextureConfig.h"
#include "../Vector/Vec2.h"

class Texture : public IRessource, public BaseAssetVisitor
{
private:
	TextureConfig m_config;
	Vec2u m_texSize = { 0,0 };
	VK_Texture* m_VK_texture = nullptr;
public:
	Texture() = default;
	~Texture() override;

	bool Load(const std::string& _filePath) override;
	void Unload() override;

	bool Add(const std::string& _filePath, IRessource* _ressource) override;

	VK_Texture* GetVKTexture();
	void SetVKTexture(VK_Texture* _VK_Texture);

	void Accept(AssetVisitor* _visitor) override;
private:
	void LoadConfig(const std::string& _filePath);
};

#endif
