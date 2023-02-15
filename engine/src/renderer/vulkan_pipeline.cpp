#include <core/engine.hpp>
#include <renderer/vulkan_renderer_types.hpp>
#include <renderer/graphics_math_types.hpp>

struct vulkan_graphics_pipeline_create_params
{
    vulkan_renderpass                   *renderpass;
    u32                                 attributeCount;
    VkVertexInputAttributeDescription   *attributes;
    u32                                 descriptorSetLayoutCount;
    VkDescriptorSetLayout               *descriptorSetLayouts;
    u32                                 stageCount;
    VkPipelineShaderStageCreateInfo     *stages;
    VkViewport                          viewport;
    VkRect2D                            scissor;
    b8                                  isWireframe;
};

// NOTE: renderpass does not need to be used by this pipeline, 
// it simply needs to be compatible with the note that will be used
PXAPI b8
VulkanGraphicsPipelineCreate(
    vulkan_context *context,
    vulkan_graphics_pipeline_create_params *params,
    vulkan_pipeline *outPipeline)
{
    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.viewportCount = 1;
    viewportState.pViewports    = &params->viewport;
    viewportState.scissorCount  = 1;
    viewportState.pScissors     = &params->scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
    rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerCreateInfo.depthClampEnable        = VK_FALSE;
    rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerCreateInfo.polygonMode = params->isWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizerCreateInfo.lineWidth               = 1.0f;
    rasterizerCreateInfo.cullMode                = VK_CULL_MODE_BACK_BIT;
    rasterizerCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizerCreateInfo.depthBiasEnable         = VK_FALSE;
    rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizerCreateInfo.depthBiasClamp          = 0.0f;
    rasterizerCreateInfo.depthBiasSlopeFactor    = 0.0f;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
    multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleCreateInfo.sampleShadingEnable   = VK_FALSE;
    multisampleCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
    multisampleCreateInfo.minSampleShading      = 1.0f;
    multisampleCreateInfo.pSampleMask           = 0;
    multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleCreateInfo.alphaToOneEnable      = VK_FALSE;

    // Depth and stencil testing
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable       = VK_TRUE;
    depthStencil.depthWriteEnable      = VK_TRUE;
    depthStencil.depthCompareOp        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable     = VK_FALSE;

    // Blending
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
    colorBlendAttachmentState.blendEnable = VK_TRUE;

    // Color blending
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.colorBlendOp        = VK_BLEND_OP_ADD;

    // Alpha blending
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;

    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{};
    colorBlendStateCreateInfo.sType =  VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable   = VK_FALSE;
    colorBlendStateCreateInfo.logicOp         = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments    = &colorBlendAttachmentState;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = ArrayLen(dynamicStates);
    dynamicStateCreateInfo.pDynamicStates    = dynamicStates;

    // Vertex Input
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0; // Binding index
    bindingDescription.stride = sizeof(vertex_3d_P);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex

    // Vertex attributes
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

    return 1;
}

PXAPI void
VulkanGraphicsPipelineDestroy(vulkan_context *context, vulkan_pipeline *pipeline)
{

}

PXAPI void
VulkanPipelineBind(vulkan_command_buffer *cmdbuf, VkPipelineBindPoint bindPoint, vulkan_pipeline *pipeline)
{

}
