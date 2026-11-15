#include "PreviewTexture.h"

#include "../Asset/RessourceManager.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_vulkan.h"
#include "Vulkan/VK_Buffer.h"
#include "Vulkan/VK_Texture.h"
#include "Vulkan/VK_TextureInfo.h"

#define PREVIEW_SIZE 128

PreviewTexture& PreviewTexture::Get()
{
	static PreviewTexture instance;
	return instance;
}

Preview* PreviewTexture::GetPreview(const std::string& _path)
{
	StringHash hash = StringHasher(_path);
	if (!m_textures.contains(hash))
	{
		VK_TextureInfo textureInfo{};
		textureInfo.config.format = IMAGE_FORMAT_R8G8B8A8;
		textureInfo.config.mipmap = false;
		textureInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		textureInfo.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		textureInfo.textureSize = { PREVIEW_SIZE, PREVIEW_SIZE };

		VK_Texture* texture = new VK_Texture;

		texture->Init(textureInfo);

		Texture* baseTexture = RessourceManager::Get().GetRessource<Texture>(_path);

		VkCommandBuffer cmd = VK_Buffer::BeginSingleTimeCommand();

		texture->TransitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		baseTexture->GetVKTexture()->BlitTo(cmd, *texture, { 128, 128 }, 0);

		LOGGER_ERROR("Transition texture preview");
		texture->TransitionLayout(cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		LOGGER_ERROR("After Transition texture preview");

		VK_Buffer::EndSingleTimeCommand(cmd);

		Preview preview{};
		preview.texture.SetVKTexture(texture);
		preview.descriptor = ImGui_ImplVulkan_AddTexture(texture->m_imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_textures[hash] = preview;

	}

	return &m_textures[hash];
}

void PreviewTexture::Cleanup()
{

}