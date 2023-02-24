#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

PXAPI void
VulkanFramebufferCreate(
    vulkan_context *context,
    mem_arena *permArena,
    vulkan_renderpass *renderpass,
    u32 width, u32 height,
    u32 attachmentCount,
    VkImageView *attachments,
    vulkan_framebuffer *outFramebuffer)
{
    if (!outFramebuffer->attachments)
    {
        outFramebuffer->attachments = (VkImageView *)PXMemoryArenaAlloc(
            permArena, sizeof(VkImageView) * attachmentCount);
    }

    for (u32 attachmentIndex = 0; attachmentIndex < attachmentCount; attachmentIndex++)
    {
        /* outFramebuffer->attachments[attachmentIndex] = attachments[attachmentIndex]; */
        *(outFramebuffer->attachments + attachmentIndex) = attachments[attachmentIndex];
    }

    outFramebuffer->renderpass = renderpass;
    outFramebuffer->attachmentCount = attachmentCount;

    VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
    createInfo.renderPass = renderpass->handle;
    createInfo.attachmentCount = attachmentCount;
    createInfo.pAttachments = outFramebuffer->attachments;
    createInfo.width = width;
    createInfo.height = height;
    createInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(
        context->device.logicalDevice,
        &createInfo,
        context->allocator,
        &outFramebuffer->handle));
}

PXAPI void
VulkanFramebufferDestroy(vulkan_context *context, vulkan_framebuffer *framebuffer)
{
    vkDestroyFramebuffer(context->device.logicalDevice, framebuffer->handle, context->allocator);

    // NOTE: we don't need to clear the memory of the attachments as they'll occupy the same
    // memory space during the whole program, so even if the renderpass is recreated, it'll use
    // the same memory space
    // Since this memory space is a permanent arena, it'll be cleared at the end by either
    // a memory free for the whole arena or by the OS

    framebuffer->handle = 0;
    framebuffer->attachmentCount = 0;
    framebuffer->renderpass = 0;
}
