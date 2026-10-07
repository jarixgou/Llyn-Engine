#ifndef TEXTURE__H
#define TEXTURE__H

#include <vulkan/vulkan_raii.hpp>

#include "../LlynCore.h"

#include "Ressource.h"
#include "../Vector/FwdVec2.h"

#include "TextureConfig.h"

class Texture : public IRessource
{
private:
	TextureConfig m_config;

	VK_Texture* VK_texture = nullptr;
public:
	Texture() = default;
	~Texture() override;

	bool Load(const std::string& _filePath) override;
	void Unload() override;

	bool Add(const std::string& _filePath, IRessource* _ressource) override;

	VK_Texture* GetVKTexture();
private:
	void LoadConfig(const std::string& _filePath);
};

#endif
