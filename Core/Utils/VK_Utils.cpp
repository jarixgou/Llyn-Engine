#include "VK_Utils.h"

#include <assert.h>
#include <iostream>

#include "../Vector/Vec2.h"
#include "../Render/Vulkan/VK_Device.h"

namespace Utils
{
	VkFormat FindSupportedFormat(const VK_Device* _device, const std::vector<VkFormat>& _candidates,
	                             VkImageTiling _tiling, VkFormatFeatureFlags _features)
	{
		if (_device == nullptr)
		{
			// TODO: print error message with logger
		}

		for (const auto& format : _candidates)
		{
			VkFormatProperties props{};
			vkGetPhysicalDeviceFormatProperties(*_device->GetPhycicalDevice(), format, &props);

			if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features)
			{
				return format;
			}

			if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features)
			{
				return format;
			}
		}
		std::cout << "Failed to find supported format !" << std::endl;

		std::exit(0);
	}

	std::pair<VkImage, VkDeviceMemory> CreateImage(const VK_Device* _device,
	                                               const Vec2<uint32_t>& _imageSize, VkFormat _format, VkImageTiling _tiling,
	                                               VkImageUsageFlags _usage, VkMemoryPropertyFlags _properties)
	{
		if (_device == nullptr)
		{
			// TODO: print error message with logger
		}

		VkImageCreateInfo imageCreateInfo{};
		imageCreateInfo.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType		= VK_IMAGE_TYPE_2D;
		imageCreateInfo.format			= _format;
		imageCreateInfo.extent			= { _imageSize.x, _imageSize.y };
		imageCreateInfo.mipLevels		= 1;
		imageCreateInfo.arrayLayers		= 1;
		imageCreateInfo.samples			= VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling			= _tiling;
		imageCreateInfo.usage			= _usage;
		imageCreateInfo.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.sharingMode		= VK_SHARING_MODE_EXCLUSIVE;

		VkImage image = VK_NULL_HANDLE;
		VK_CHECK(vkCreateImage(*_device->GetDevice(), &imageCreateInfo, nullptr, &image), "Failed to create Image");

		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(*_device->GetDevice(), image, &memRequirements);

		VkMemoryAllocateInfo allocCreateInfo;
		allocCreateInfo.allocationSize = memRequirements.size;
		allocCreateInfo.memoryTypeIndex = FindMemoryType(_device, memRequirements.memoryTypeBits, _properties);

		VkDeviceMemory imageMemory;
		VK_CHECK(vkAllocateMemory(*_device->GetDevice(), &allocCreateInfo, nullptr, &imageMemory), "Failed to Allocate Memory !");

		VK_CHECK(vkBindImageMemory(*_device->GetDevice(), image, imageMemory, 0), "Failed to Bind Image Memory !");

		return { image, imageMemory };

		/*vk::ImageCreateInfo imageInfo;
		imageInfo.setImageType(vk::ImageType::e2D);
		imageInfo.setFormat(_format);
		imageInfo.setExtent({ _imageSize.x, _imageSize.y });
		imageInfo.setMipLevels(1);
		imageInfo.setArrayLayers(1);
		imageInfo.setSamples(vk::SampleCountFlagBits::e1);
		imageInfo.setTiling(_tiling);
		imageInfo.setUsage(_usage);
		imageInfo.setSharingMode(vk::SharingMode::eExclusive);

		vk::raii::Image image = vk::raii::Image(VK_Device::Get().GetDevice(), imageInfo);

		const vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.setAllocationSize(memRequirements.size);
		allocInfo.setMemoryTypeIndex(FindMemoryType(memRequirements.memoryTypeBits, _properties));

		vk::raii::DeviceMemory imageMemory = vk::raii::DeviceMemory(VK_Device::Get().GetDevice(), allocInfo);
		image.bindMemory(imageMemory, 0);

		return { std::move(image), std::move(imageMemory) };*/
	}

	VkImageView CreateImageView(const VK_Device* _device, VkImage const& _image, VkFormat _format,
		VkImageAspectFlags _aspectFlags)
	{
		VkImageSubresourceRange imageSubResourceRange{};
		imageSubResourceRange.aspectMask		= _aspectFlags;
		imageSubResourceRange.baseMipLevel		= 0;
		imageSubResourceRange.levelCount		= 1;
		imageSubResourceRange.baseArrayLayer	= 1;
		imageSubResourceRange.layerCount		= 1;

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image				= _image;
		imageViewCreateInfo.viewType			= VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format				= _format;
		imageViewCreateInfo.subresourceRange	= imageSubResourceRange;

		VkImageView imageView{};
		VK_CHECK(vkCreateImageView(*_device->GetDevice(), &imageViewCreateInfo, nullptr, &imageView), "Failed to Create Image View !");

		return imageView;

		/*vk::ImageSubresourceRange imageSubresource;
		imageSubresource.setAspectMask(_aspectFlags);
		imageSubresource.setBaseMipLevel(0);
		imageSubresource.setLevelCount(1);
		imageSubresource.setBaseArrayLayer(0);
		imageSubresource.setLayerCount(1);

		vk::ImageViewCreateInfo viewInfo;
		viewInfo.setImage(_image);
		viewInfo.setViewType(vk::ImageViewType::e2D);
		viewInfo.setFormat(_format);
		viewInfo.setSubresourceRange(imageSubresource);

		return vk::raii::ImageView(_device->GetDevice(), viewInfo);*/
	}

	uint32_t FindMemoryType(const VK_Device* _device, uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
	{
		if (_device == nullptr)
		{
			// TODO: print error message with logger
		}

		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(*_device->GetPhycicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			if (_typeFilter & (1 << i) && 
				(memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
			{
				return i;
			}
		}

		std::cout << "Failed to find suitable memory type !" << std::endl;
	}

	VkDescriptorSetLayout CreateLayout(const VK_Device* _device, const std::vector<uint32_t>& _bindings,
	                                   const std::vector<uint32_t>& _counts, const std::vector<VkDescriptorType>& _types,
	                                   const std::vector<VkShaderStageFlags>& _stages)
	{
		if (_device == nullptr)
		{
			// TODO print error message with logger
		}

		if ((_bindings.empty() && _counts.empty() && _types.empty() && _stages.empty())
			|| _bindings.size() != _counts.size() || _bindings.size() != _types.size() || _bindings.size() != _stages.size())
		{
			// TODO: print error message with logger
		}

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings(_bindings.size());
		for (size_t i = 0; i < _bindings.size(); ++i)
		{
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = _bindings[i];
			layoutBinding.descriptorType = _types[i];
			layoutBinding.descriptorCount = _counts[i];
			layoutBinding.stageFlags = _stages[i];
		}

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutCreateInfo.pBindings = layoutBindings.data();
		layoutCreateInfo.pNext = VK_NULL_HANDLE;

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VK_CHECK(vkCreateDescriptorSetLayout(*_device->GetDevice(), &layoutCreateInfo, nullptr, &descriptorSetLayout), "Failed to create Descriptor Set Layout !");

		return descriptorSetLayout;
	}

	VkDescriptorSetLayout CreateBindlessLayout(const VK_Device* _device, const std::vector<uint32_t>& _bindings, const std::vector<uint32_t>& _counts,
	                                           const std::vector<VkDescriptorType>& _types, const std::vector<VkShaderStageFlags>& _stages)
	{
		if (_device == nullptr)
		{
			// TODO print error message with logger
		}

		if ((_bindings.empty() && _counts.empty() && _types.empty() && _stages.empty()) 
			|| _bindings.size() != _counts.size() || _bindings.size() != _types.size() || _bindings.size() != _stages.size())
		{
			// TODO: print error message with logger
		}

		std::vector<VkDescriptorSetLayoutBinding> layoutBindings(_bindings.size());
		std::vector<VkDescriptorBindingFlags> bindingFlags(_bindings.size());
		for (size_t i = 0; i < _bindings.size(); ++i)
		{
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = _bindings[i];
			layoutBinding.descriptorType = _types[i];
			layoutBinding.descriptorCount = _counts[i];
			layoutBinding.stageFlags = _stages[i];

			layoutBindings.emplace_back(layoutBinding);
			bindingFlags.emplace_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);
		}


		VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
		bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		bindingFlagsCreateInfo.pNext = VK_NULL_HANDLE;
		bindingFlagsCreateInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
		bindingFlagsCreateInfo.pBindingFlags = bindingFlags.data();

		VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutCreateInfo.pBindings = layoutBindings.data();
		layoutCreateInfo.pNext = &bindingFlagsCreateInfo;
		layoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
		VK_CHECK(vkCreateDescriptorSetLayout(*_device->GetDevice(), &layoutCreateInfo, nullptr, &descriptorSetLayout), "Failed to create Descriptor Set Layout !");

		return descriptorSetLayout;
	}
}
