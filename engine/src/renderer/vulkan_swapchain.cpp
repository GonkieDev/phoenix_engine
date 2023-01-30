#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

// NOTE: the InternalVulkanCreateSwapchain and InternalVulkanDestroySwapchain exist as they're code
// which is shared by the VulkanSwapchainCreate and VulkanSwapchainDestroy and VulkanSwapchainDestroy

PXAPI inline void
InternalVulkanCreateSwapchain(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *outSwapchain)
{
    VkExtent2D swapchainExtent = { width, height };
    swapchain->maxFramesInFlight = 2;
}

PXAPI inline void
InternalVulkanDestroySwapchain(vulkan_context *context, vulkan_swapchain *swapchain)
{

}

PXAPI void
VulkanSwapchainCreate(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *outSwapchain)
{
    InternalVulkanCreateSwapchain(context, width, height, outSwapchain);
}

PXAPI void
VulkanSwapchainRecreate(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *swapchain)
{
    InternalVulkanDestroySwapchain(context, swapchain);
    InternalVulkanCreateSwapchain(context, width, height, swapchain);
}

PXAPI void
VulkanSwapchainDestroy(vulkan_context *context, vulkan_swapchain *swapchain)
{
    InternalVulkanDestroySwapchain(context, swapchain);
}

PXAPI b8
VulkanSwapchainGetNextImageIndex(
    vulkan_context *context,
    vulkan_swapchain *swapchain,
    u64 timeoutNs,
    VkSemaphore imageAvailableSemaphore,
    VkFence fence,
    u32 *outImageIndex)
{
    VkResult result = vkAcquireNextImageKHR(
        context->device.logicalDevice,
        swapchain->handle,
        timeoutNs,
        imageAvailableSemaphore,
        fence,
        outImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // NOTE: can happen when window is resized
        VulkanSwapchainRecreate(context, context->framebufferWidth, context->framebufferHeight, swapchain);
        return 0;
    }

    if (result == VK_SUCCESS)
    {
        return 1;
    }

    if (result == VK_SUBOPTIMAL_KHR)
    {
        return 1;
    }

    PXFATAL("Failed to acquire swapchain image!");
    return 0;
}

PXAPI void
VulkanSwapchainPresent(
    vulkan_context *context,
    vulkan_swapchain *swapchain,
    VkQueue graphicsQueue,
    VkQueue presentQueue,
    VkSemaphore renderCompleteSemaphore,
    u32 presentImageIndex)
{
    
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType               = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount  = 1;
    presentInfo.pWaitSemaphores     = &renderCompleteSemaphore;
    presentInfo.swapchainCount      = 1;
    presentInfo.pSwapchains         = &swapchain->handle;
    presentInfo.pImageIndices       = &presentImageIndex;
    presentInfo.pResults            = 0;

    VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        // Swapchain is out of date, suboptimal or framebuffer was resized
        VulkanSwapchainRecreate(context, context->framebufferWidth, context->framebufferHeight, swapchain);
        return;
    }

    if (result != VK_SUCCESS)
    {
        PXFATAL("Failed to present swapchain image!");
    }
}
