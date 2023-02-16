#include <renderer/vulkan_renderer_types.hpp>
#include <math/math_types.hpp>

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

    // Descriptors TODO

    // Pipeline creation
    VkViewport viewport;
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (f32)context->framebufferWidth;
    viewport.height = (f32)context->framebufferHeight;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0;
    scissor.extent.width = context->framebufferWidth;
    scissor.extent.height = context->framebufferHeight;

    // Attributes
    u32 offset = 0;
    const i32 attributeCount = 1;
    VkVertexInputAttributeDescription attributeDescriptions[attributeCount];

    // Position
    VkFormat formats[attributeCount] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };

    u64 sizes[attributeCount] = {
        sizeof(v3)
    };

    for (u32 i = 0; i < attributeCount; i++)
    {
        attributeDescriptions[i].binding = 0; // binding index - should match binding desc
        attributeDescriptions[i].location = i; // attrib location
        attributeDescriptions[i].format = formats[i];
        attributeDescriptions[i].offset = offset;
        offset += sizes[i];
    }

    // TODO: Descriptors and set layouts

    // Stages
    VkPipelineShaderStageCreateInfo stagesCreateInfos[SHADER_OBJECT_STAGE_COUNT];
    for (u32 i = 0; i < SHADER_OBJECT_STAGE_COUNT; i++)
    {
        stagesCreateInfos[i] = {};
        stagesCreateInfos[i].sType = outShader->stages[i].shaderStageCreateInfo.sType;
        stagesCreateInfos[i] = outShader->stages[i].shaderStageCreateInfo;
    }

    vulkan_graphics_pipeline_create_params createPipelineParams{};
    createPipelineParams.renderpass = &context->mainRenderpass;
    createPipelineParams.attributeCount = attributeCount;
    createPipelineParams.attributes = attributeDescriptions;
    createPipelineParams.descriptorSetLayoutCount = 0;
    createPipelineParams.descriptorSetLayouts = 0;
    createPipelineParams.stageCount = SHADER_OBJECT_STAGE_COUNT;
    createPipelineParams.stages = stagesCreateInfos;
    createPipelineParams.viewport = viewport;
    createPipelineParams.scissor = scissor;
    createPipelineParams.isWireframe = 0;

    if(!VulkanGraphicsPipelineCreate( context, &createPipelineParams, &outShader->pipeline))
    {
        PXERROR("Failed to load graphics pipeline for PHOENIX.ObjectShader.");
        return 0;
    }

    return 1;
}

PXAPI b8
VulkanShaderObjectDestroy(vulkan_context *context, vulkan_shader_object *shader)
{
    VulkanGraphicsPipelineDestroy(context, &shader->pipeline);

    for (u32 i = 0; i < SHADER_OBJECT_STAGE_COUNT; i++)
    {
        vkDestroyShaderModule(context->device.logicalDevice, shader->stages[i].handle, context->allocator);
        shader->stages[i].handle = 0;
    }

    return 1;
}

PXAPI void
VulkanShaderObjectUse(vulkan_context *context, vulkan_shader_object *shader)
{

}
