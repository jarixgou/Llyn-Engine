#ifndef PREVIEW_TEXTURE__H
#define PREVIEW_TEXTURE__H
#include <unordered_map>

#include "../Asset/Texture.h"

struct Preview
{
	Texture texture;
	void* descriptor;
};

class PreviewTexture
{
private:
	std::unordered_map<StringHash, Preview> m_textures;
public:
	static PreviewTexture& Get();

	Preview* GetPreview(const std::string& _path);

	void Cleanup();
};

#endif