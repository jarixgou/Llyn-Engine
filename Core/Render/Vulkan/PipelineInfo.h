#ifndef PIPELINE_KEY__H
#define PIPELINE_KEY__H

#include <cstdint>
#include <string>

enum class SurfaceType : uint8_t
{
	OPAQUE,
	TRANSPARENT
};

enum class BlendMode : uint8_t
{
	ALPHA,
	PRE_MULTIPLY,
	ADDITIVE,
	MULTIPLY
};

enum class RenderFace : uint8_t
{
	BACK,
	FRONT,
	BOTH
};

struct PipelineInfo
{
	SurfaceType surfaceType = SurfaceType::OPAQUE;
	BlendMode blendMode = BlendMode::ALPHA;
	RenderFace renderFace = RenderFace::BACK;

	std::string shader = "Core/Shaders/slang.spv";

	bool operator==(const PipelineInfo& _info) const
	{
		return surfaceType == _info.surfaceType &&
			blendMode == _info.blendMode &&
			renderFace == _info.renderFace &&
			shader == _info.shader;
	}
};

struct PipelineInfoHash
{
	size_t operator()(const PipelineInfo& _info) const
	{
		const size_t h1 = std::hash<uint8_t>()(static_cast<uint8_t>(_info.surfaceType));
		const size_t h2 = std::hash<uint8_t>()(static_cast<uint8_t>(_info.blendMode));
		const size_t h3 = std::hash<uint8_t>()(static_cast<uint8_t>(_info.renderFace));
		const size_t h4 = std::hash<std::string>()(_info.shader);

		return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
	}
};

#endif