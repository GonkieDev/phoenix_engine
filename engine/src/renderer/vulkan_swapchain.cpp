#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

// NOTE: the InternalVulkanCreateSwapchain and InternalVulkanDestroySwapchain exist as they're code
// which is shared by the VulkanSwapchainCreate and VulkanSwapchainDestroy and VulkanSwapchainDestroy

// NOTE: permArena is needed to allocate memory for the swapchain's VkImages and VkImageViews, if this function is used
// to recreate the swapchain, the memory for those images will be already allocated thus permArena can be null
PXAPI inline void
InternalVulkanCreateSwapchain(vulkan_context *context, u32 width, u32 height, vulkan_swapchain *outSwapchain, mem_arena *permArena = 0)
{
    VkExtent2D swapchainExtent = { width, height };
    outSwapchain->maxFramesInFlight = 2;

    // Choose swapchain format
    b8 formatFound = 0;
    for (u32 i = 0; i < context->device.swapchainSupportInfo.formatsCount; i++)
    {
        VkSurfaceFormatKHR format = context->device.swapchainSupportInfo.formats[i];
        // Preferred formats
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            outSwapchain->imageFormat = format;
            formatFound = 1;
            break;
        }
    }

    if (!formatFound)
    {
        outSwapchain->imageFormat = context->device.swapchainSupportInfo.formats[0];
    }

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->device.swapchainSupportInfo.presentModesCount; i++)
    {
        VkPresentModeKHR mode = context->device.swapchainSupportInfo.presentModes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = mode;
            break;
        }
    }

    // Requery surface capabilities as they may have changed due to a window resize, monitor change, etc.
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        context->device.physicalDevice,
        context->surface,
        &context->device.swapchainSupportInfo.capabilities));

    if (context->device.swapchainSupportInfo.capabilities.currentExtent.width != UINT32_MAX)
    {
        swapchainExtent = context->device.swapchainSupportInfo.capabilities.currentExtent;
    }

    {
        VkExtent2D min = context->device.swapchainSupportInfo.capabilities.minImageExtent;
        VkExtent2D max = context->device.swapchainSupportInfo.capabilities.maxImageExtent;
        swapchainExtent.width  = PXCLAMP(swapchainExtent.width, min.width, max.width);
        swapchainExtent.height = PXCLAMP(swapchainExtent.height, min.height, max.height);
    }

    u32 imageCount = context->device.swapchainSupportInfo.capabilities.minImageCount + 1;
    if (context->device.swapchainSupportInfo.capabilities.minImageCount > 0 &&
        imageCount > context->device.swapchainSupportInfo.capabilities.minImageCount)
    {
        imageCount = context->device.swapchainSupportInfo.capabilities.minImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface          = context->surface;
    swapchainCreateInfo.minImageCount    = imageCount;
    swapchainCreateInfo.imageFormat      = outSwapchain->imageFormat.format; 
    swapchainCreateInfo.imageColorSpace  = outSwapchain->imageFormat.colorSpace; 
    swapchainCreateInfo.imageExtent      = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (context->device.graphicsQueueIndex != context->device.presentQueueIndex)
    {
        u32 queueFamilyIndices[] = {
            (u32)context->device.graphicsQueueIndex,
            (u32)context->device.presentQueueIndex,
        };

        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;
        swapchainCreateInfo.pQueueFamilyIndices = 0;
    }

    swapchainCreateInfo.preTransform    = context->device.swapchainSupportInfo.capabilities.currentTransform;
    // NOTE: this is composition with windowing surface of the OS
    swapchainCreateInfo.compositeAlpha  = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode     = presentMode;
    swapchainCreateInfo.clipped         = VK_TRUE;
    swapchainCreateInfo.oldSwapchain    = 0;

    VK_CHECK(vkCreateSwapchainKHR(context->device.logicalDevice,
        &swapchainCreateInfo,
        context->allocator,
        &outSwapchain->handle));

    // Get swapchain image references
    // NOTE: we don't need to create (or destroy) these images as the swapchain does that for us

    context->currentFrame = 0;
    outSwapchain->imageCount = 0;

    // Images and views
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logicalDevice, outSwapchain->handle, &outSwapchain->imageCount, 0));

    if (permArena && !outSwapchain->images)
    {
        outSwapchain->images = (VkImage *)PXMemoryArenaAlloc(permArena, sizeof(VkImage) * outSwapchain->imageCount);
        outSwapchain->views  = (VkImageView *)PXMemoryArenaAlloc(permArena, sizeof(VkImageView) * outSwapchain->imageCount);
    }

    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logicalDevice, outSwapchain->handle, &outSwapchain->imageCount,
        outSwapchain->images));

    for (u32 viewIndex = 0; viewIndex < outSwapchain->imageCount; viewIndex++)
    {
        VkImageViewCreateInfo viewCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewCreateInfo.pNext            = 0;
        viewCreateInfo.flags            = 0;
        viewCreateInfo.image            = outSwapchain->images[viewIndex];
        viewCreateInfo.viewType         = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format           = outSwapchain->imageFormat.format;
        /* viewCreateInfo.components       = ; */
        viewCreateInfo.subresourceRange.aspectMask      = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseMipLevel    = 0;
        viewCreateInfo.subresourceRange.levelCount      = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer  = 0;
        viewCreateInfo.subresourceRange.layerCount      = 1;

        VK_CHECK(vkCreateImageView(
            context->device.logicalDevice,
            &viewCreateInfo,
            context->allocator,
            outSwapchain->views + viewIndex));

    }

    // Setup depth buffer
    if (!VulkanDeviceDetectDepthFormat(&context->device))
    {
        context->device.depthFormat = VK_FORMAT_UNDEFINED;
        PXFATAL("Failed to find a supported depth format!");
    }

    vulkan_create_image_params vulkanCreateImageParams;
    vulkanCreateImageParams.context         = context;
    vulkanCreateImageParams.width           = swapchainExtent.width;
    vulkanCreateImageParams.height          = swapchainExtent.height;
    vulkanCreateImageParams.format          = context->device.depthFormat;
    vulkanCreateImageParams.tiling          = VK_IMAGE_TILING_OPTIMAL;
    vulkanCreateImageParams.usageFlags      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    vulkanCreateImageParams.memoryFlags     = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    vulkanCreateImageParams.createView      = 1;
    vulkanCreateImageParams.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
    vulkanCreateImageParams.outImage        = &outSwapchain->depthAttachment; 

    VulkanCreateImage(&vulkanCreateImageParams);

    PXINFO("Swapchain created succesfully!");
}

PXAPI inline void
InternalVulkanDestroySwapchain(vulkan_context *context, vulkan_swapchain *swapchain)
{
    vkDeviceWaitIdle(context->device.logicalDevice);
    VulkanImageDestroy(context, &swapchain->depthAttachment);

    for (u32 imageIndex = 0; imageIndex < swapchain->imageCount; imageIndex++)
    {
        vkDestroyImageView(context->device.logicalDevice, swapchain->views[imageIndex], context->allocator);
    }

    vkDestroySwapchainKHR(context->device.logicalDevice, swapchain->handle, context->allocator);
}

PXAPI void
VulkanSwapchainCreate(vulkan_context *context, u32 width, u32 height, mem_arena *permArena, vulkan_swapchain *outSwapchain)
{
    InternalVulkanCreateSwapchain(context, width, height, outSwapchain, permArena);
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
        return;
    }
    else if (result != VK_SUCCESS)
    {
        PXFATAL("Failed to present swapchain image!");
    }

    // Increment (and loop) the index
    context->currentFrame = (context->currentFrame + 1) % context->swapchain.maxFramesInFlight;
}
