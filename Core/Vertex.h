#ifndef VERTEX__H
#define VERTEX__H

#include <vulkan/vulkan.hpp>

#include "Vector/Vec2.h"
#include "Vector/Vec3.h"

struct Vertex
{
	Vec3f position = { 0.f, 0.f, 0.f };
	Vec3f normal = { 0.f, 0.f, 0.f };
	Vec3f color = { 1.f, 1.f, 1.f };
	Vec2f uv = { 0.f, 0.f };

	static vk::VertexInputBindingDescription GetBindingDescrition()
	{
		return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
	}

	static std::array<vk::VertexInputAttributeDescription, 4> GetAttributeDescriptions()
	{
		return {
			{
				{0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)},
				{1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal)},
				{2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color)},
				{3, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, uv)},
			} };
	}
};

#endif