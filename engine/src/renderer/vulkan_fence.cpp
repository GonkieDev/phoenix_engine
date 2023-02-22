#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

PXAPI void
VulkanFenceCreate(vulkan_context *context, b8 startSignal, vulkan_fence *fence)
{
    fence->signaled = startSignal;
    VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    /* createInfo.pNext = 0; */
    createInfo.flags = fence->signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VK_CHECK(vkCreateFence(
        context->device.logicalDevice,
        &createInfo,
        context->allocator,
        &fence->handle));
}

PXAPI void
VulkanFenceDestroy(vulkan_context *context, vulkan_fence *fence)
{
    if (fence->handle)
    {
        vkDestroyFence(
            context->device.logicalDevice,
            fence->handle,
            context->allocator);
        fence->handle = 0;
    }

    fence->signaled = 0;
}

// NOTE: returns 1 on success, 0 on failure
PXAPI b8 
VulkanFenceWait(vulkan_context *context, vulkan_fence *fence, u64 timeoutNanoseconds)
{
    if (fence->signaled)
    {
        return 1;
    }

    VkResult result = vkWaitForFences(
        context->device.logicalDevice,
        1,
        &fence->handle,
        1,
        timeoutNanoseconds);

    switch (result)
    {
        case VK_SUCCESS:
            fence->signaled = 1;
            return 1;
        case VK_TIMEOUT:
            PXWARN("VulkanFenceWait - Timed out!");
            break;
        case VK_ERROR_DEVICE_LOST:
            PXERROR("VulkanFenceWait - VK_ERROR_DEVICE_LOST.");
            break;
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            PXERROR("VulkanFenceWait - VK_ERROR_OUT_OF_HOST_MEMORY.");
            break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            PXERROR("VulkanFenceWait - VK_ERROR_OUT_OF_DEVICE_MEMORY.");
            break;
        default:
            PXERROR("VulkanFenceWait - An unknown error has ocurred.");
            break;
    }

    return 0;
}

PXAPI void
VulkanFenceReset(vulkan_context *context, vulkan_fence *fence)
{
    if (fence->signaled)
    {
        VK_CHECK(vkResetFences(context->device.logicalDevice, 1, &fence->handle));
        fence->signaled = 0;
    }
}
