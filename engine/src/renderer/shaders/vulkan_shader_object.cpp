#include <renderer/vulkan_renderer_types.hpp>

#define BUILTIN_SHADER_NAME_OBJECT "PHOENIX.ObjectShader"

PXAPI b8
VulkanShaderObjectCreate(vulkan_context *context, vulkan_shader_object *outShader)
{
    char stageTypeStrings[SHADER_OBJECT_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits stageTypes[SHADER_OBJECT_STAGE_COUNT] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    for (u32 stageIndex = 0; stageIndex < SHADER_OBJECT_STAGE_COUNT; stageIndex++)
    {
        if (!CreateShaderModule(
                context,
                BUILTIN_SHADER_NAME_OBJECT,
                stageTypeStrings[stageIndex],
                stageTypes[stageIndex],
                stageIndex,
                outShader->stages))
        {
            PXERROR("Unable to create %s shader module for '%s'", stageTypeStrings[stageIndex],
                BUILTIN_SHADER_NAME_OBJECT);
            return 0;
        }
    }

    return 1;
}

PXAPI b8
VulkanShaderObjectDestroy(vulkan_context *context, vulkan_shader_object *shader)
{
    return 1;
}

PXAPI void
VulkanShaderObjectUse(vulkan_context *context, vulkan_shader_object *shader)
{

}
