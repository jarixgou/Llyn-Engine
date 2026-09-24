#ifndef UNIFROM_MANAGER__H
#define UNIFORM_MANAGER__H

#include <string>
#include <cstdint>
#include <vector>

#include "../LlynCore.h"

class IUniformManager
{
public:
	IUniformManager() = default;
	~IUniformManager() = default;

	virtual void Init() = 0;

	virtual void StoreData(std::string _name, void* _data, size_t _size, uint32_t _frameIndex) = 0;
	virtual void StoreTexture(Texture* _texture, uint32_t _dstArray, uint32_t _frameIndex) = 0;
	virtual void StoreTextures(std::vector<Texture>& _textures, uint32_t _frameIndex) = 0;

	virtual void PushUniform() = 0;
};

#endif