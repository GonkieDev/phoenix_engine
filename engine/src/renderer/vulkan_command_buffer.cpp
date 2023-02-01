#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>


// NOTE: there are gonna be various times where we need a command buffer
// once, this function is for those cases
PXAPI void
VulkanCommandBufferAllocateAndBeginSingleUse(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *commandBuffer);

// Counter part to function above
// NOTE: also submits command buffer to queue
// NOTE: only use this function and its counter part for light usage as it uses
// a vkQueueWaitIdle call
PXAPI void
VulkanCommandBufferEndSingleUse(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *commandBuffer,
    VkQueue queue);


PXAPI inline void
VulkanCommandBufferAllocate(
    vulkan_context *context,
    VkCommandPool pool,
    b8 isPrimary,
    vulkan_command_buffer *outCommandBuffer)
{
    *outCommandBuffer = {};

    VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocateInfo.pNext = 0;
    allocateInfo.commandPool = pool;
    allocateInfo.level = isPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    allocateInfo.commandBufferCount = 1;

    outCommandBuffer->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
    VK_CHECK(vkAllocateCommandBuffers(
        context->device.logicalDevice,
        &allocateInfo,
        &outCommandBuffer->handle));
    outCommandBuffer->state = COMMAND_BUFFER_STATE_READY;
}

PXAPI inline void
VulkanCommandBufferFree(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *commandBuffer)
{
    vkFreeCommandBuffers(
        context->device.logicalDevice,
        pool,
        1,
        &commandBuffer->handle);

    commandBuffer->handle = 0;
    commandBuffer->state  = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

PXAPI inline void
VulkanCommandBufferBegin(
    vulkan_command_buffer *commandBuffer,
    b8 isSingleUse = 0,
    b8 isRenderpassContinue = 0,
    b8 isSimultaneousUse = 0)
{
    VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    beginInfo.pNext = 0;
    beginInfo.flags = 0;

    // Set flags
    if (isSingleUse) { beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; }
    if (isRenderpassContinue) { beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; }
    if (isSimultaneousUse) { beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; }

    VK_CHECK(vkBeginCommandBuffer(commandBuffer->handle, &beginInfo));
    commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING;
}

PXAPI inline void
VulkanCommandBufferEnd( vulkan_command_buffer *commandBuffer)
{
    vkEndCommandBuffer(commandBuffer->handle);
    // @stability : check if state is correct
    commandBuffer->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
}

PXAPI inline void
VulkanCommandBufferUpdateSubmitted(vulkan_command_buffer *commandBuffer)
{
    commandBuffer->state = COMMAND_BUFFER_STATE_SUBMITTED;
}

PXAPI void
VulkanCommandBufferReset(vulkan_command_buffer *commandBuffer)
{
    commandBuffer->state = COMMAND_BUFFER_STATE_READY;
}


PXAPI void
VulkanCommandBufferAllocateAndBeginSingleUse(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *commandBuffer)
{
    VulkanCommandBufferAllocate(context, pool, 1, commandBuffer);
    VulkanCommandBufferBegin(commandBuffer, 1, 0, 0);
}

PXAPI void
VulkanCommandBufferEndSingleUse(
    vulkan_context *context,
    VkCommandPool pool,
    vulkan_command_buffer *commandBuffer,
    VkQueue queue)
{
    VulkanCommandBufferEnd(commandBuffer);

    // Submit to queue
    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer->handle;
    VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, /* fence */ 0));

    VK_CHECK(vkQueueWaitIdle(queue));

    VulkanCommandBufferFree(context, pool, commandBuffer);
}
