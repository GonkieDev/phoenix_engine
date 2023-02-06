#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

PXAPI void
VulkanRenderpassCreate(
    vulkan_context *context,
    f32 x, f32 y, f32 w, f32 h,
    f32 r, f32 g, f32 b, f32 a, // Clear color
    f32 depth,
    u32 stencil,
    vulkan_renderpass *outRenderpass)
{
    outRenderpass->x = x;
    outRenderpass->y = y;
    outRenderpass->w = w;
    outRenderpass->h = h;

    outRenderpass->r = r;
    outRenderpass->g = g;
    outRenderpass->b = b;
    outRenderpass->a = a;

    outRenderpass->depth = depth;
    outRenderpass->stencil = stencil;

    // Main subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable
    enum
    {
        ATTACHMENT_COLOR,
        ATTACHMENT_DEPTH,

        ATTACHMENT_COUNT
    };
    VkAttachmentDescription attachmentDescriptions[ATTACHMENT_COUNT];

    // Color attachment
    VkAttachmentDescription colorAttachment;
    colorAttachment.flags           = 0;
    colorAttachment.format          = context->swapchain.imageFormat.format; // TODO: configurable
    colorAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentDescriptions[ATTACHMENT_COLOR] = colorAttachment;

    VkAttachmentReference colorAttachmentReference;
    colorAttachmentReference.attachment = ATTACHMENT_COLOR; // Attachment description array index
    colorAttachmentReference.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentReference;

    // Depth attachment
    VkAttachmentDescription depthAttachment;
    depthAttachment.flags           = 0;
    depthAttachment.format          = context->device.depthFormat;
    depthAttachment.samples         = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attachmentDescriptions[ATTACHMENT_DEPTH] = depthAttachment;

    VkAttachmentReference depthAttachmentReference;
    depthAttachmentReference.attachment = ATTACHMENT_DEPTH; // Attachment description array index
    depthAttachmentReference.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentReference;

    // TODO: other attachment types (input, resolve, preserve)

    // Depth stencil data
    subpass.pDepthStencilAttachment = &depthAttachmentReference;

    // Input from a shader
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments    = 0;

    // Attachments used for multisampling colour attachments
    subpass.pResolveAttachments = 0;

    // Not used in this subpass, but preserved for next subpass
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments    = 0;

    // Renderpass dependecies TODO: configurable
    VkSubpassDependency depency;
    depency.srcSubpass      = VK_SUBPASS_EXTERNAL;
    depency.dstSubpass      = 0;
    depency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depency.srcAccessMask   = 0;
    depency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    depency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT ;
    depency.dependencyFlags = 0;

    // Renderpass create info
    VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    /* createInfo.pNext        = 0; */
    /* createInfo.flags        = 0; */
    createInfo.attachmentCount = ATTACHMENT_COUNT;
    createInfo.pAttachments    = attachmentDescriptions;
    createInfo.subpassCount    = 1;
    createInfo.pSubpasses      = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies   = &depency;

    VK_CHECK(vkCreateRenderPass(
        context->device.logicalDevice,
        &createInfo,
        context->allocator,
        &outRenderpass->handle));
}

PXAPI void
VulkanRenderPassDestroy(vulkan_context *context, vulkan_renderpass *renderpass)
{
    if (renderpass && renderpass->handle)
    {
        vkDestroyRenderPass(context->device.logicalDevice, renderpass->handle, context->allocator);
        renderpass->handle = 0;
    }
}

PXAPI void
VulkanRenderpassBegin(
    vulkan_command_buffer *commandBuffer,
    vulkan_renderpass *renderpass,
    VkFramebuffer framebuffer)
{
    VkRenderPassBeginInfo beginInfo =  { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    beginInfo.renderPass    = renderpass->handle;
    beginInfo.framebuffer   = framebuffer;
    beginInfo.renderArea.offset.x = renderpass->x;
    beginInfo.renderArea.offset.y = renderpass->y;
    beginInfo.renderArea.extent.width  = renderpass->w;
    beginInfo.renderArea.extent.height = renderpass->h;

    VkClearValue clearValues[2];
    clearValues[0] = {};
    clearValues[0].color.float32[0] = renderpass->r;
    clearValues[0].color.float32[1] = renderpass->g;
    clearValues[0].color.float32[2] = renderpass->b;
    clearValues[0].color.float32[3] = renderpass->a;
    clearValues[1].depthStencil.depth   = renderpass->depth;
    clearValues[1].depthStencil.stencil = renderpass->stencil;

    beginInfo.clearValueCount = ArrayLen(clearValues);
    beginInfo.pClearValues    = clearValues;

    vkCmdBeginRenderPass(commandBuffer->handle, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    commandBuffer->state = COMMAND_BUFFER_STATE_IN_RENDERPASS;
}

PXAPI void
VulkanRenderpassEnd(vulkan_command_buffer *commandBuffer, vulkan_renderpass *renderpass)
{
    vkCmdEndRenderPass(commandBuffer->handle);
    commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}
