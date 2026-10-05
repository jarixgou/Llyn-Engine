#ifndef VK_STAGE_AND_ACCESS__H
#define VK_STAGE_AND_ACCESS__H
#include "VK_Pipeline.h"

struct VK_StageAndAccess
{
	VkPipelineStageFlags2 srcStage = VK_PIPELINE_STAGE_2_NONE;
	VkAccessFlags2 srcAccess = VK_ACCESS_2_NONE;

	VkPipelineStageFlags2 dstStage = VK_PIPELINE_STAGE_2_NONE;
	VkAccessFlags2 dstAccess = VK_ACCESS_2_NONE;
};

#endif