#include "vulkan/vulkan_core.h"
#include <renderer/vulkan_renderer_types.hpp>

PXAPI b8
CreateShaderModule(
                vulkan_context *context,
                const char *name,
                const char *typeStr,
                VkShaderStageFlagBits shaderStageFlag,
                u32 stageIndex,
                vulkan_shader_stage *shaderStages)
{
    char filename[512];
    
    StringFormat(filename, ArrayLen(filename), "assets/shaders/%s.%s.spv", name, typeStr);

    read_file_result readResult = PlatformReadEntireFile(filename);
    if (!readResult.contents)
    {
        PXERROR("Unable to read shader module: %s", filename);
        return 0;
    }

    shaderStages[stageIndex].createInfo = {};
    shaderStages[stageIndex].createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderStages[stageIndex].createInfo.codeSize = readResult.contentSize;
    shaderStages[stageIndex].createInfo.pCode = (u32*)readResult.content;

    VK_CHECK(vkCreateShaderModule(
        context->device.logicalDevice,
        &shaderStages[stageIndex].createInfo,
        context->allocator,
        &shaderStages[stageIndex].handle));

    return 1;
}
