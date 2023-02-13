#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

#include <renderer/vulkan_renderer_backend_helpers.cpp>
#include <renderer/vulkan_device.cpp>
#include <renderer/vulkan_image.cpp>
#include <renderer/vulkan_swapchain.cpp>
#include <renderer/vulkan_renderpass.cpp>
#include <renderer/vulkan_command_buffer.cpp>
#include <renderer/vulkan_framebuffer.cpp>
#include <renderer/vulkan_fence.cpp>

// NOTE: declare backendContext before cpp includes so that they can use it
global_var vulkan_context backendContext;
global_var u32 cachedFramebufferWidth;
global_var u32 cachedFramebufferHeight;

VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* user_data);

PXAPI void
BackendCreateCommandBuffers(renderer_backend *backend, mem_arena *permArena);

PXAPI void
BackendRegenerateBuffers(
    renderer_backend *backend,
    vulkan_swapchain *swapchain,
    vulkan_renderpass *renderpass,
    mem_arena *permArena);

PXAPI b8
SwapchainRecreate(renderer_backend *backend);

// TODO: get rid of allocations in these functions that can be used with variable lengths stack arrays
PXAPI b8
InitRendererBackend(char *appName, renderer_backend *backend, engine_state *engineState)
{
    backendContext.permArena = &engineState->permArena;
    backendContext.tempArena = &engineState->frameArena;

    // TODO: custom allocator
    backendContext.allocator = 0;

    backendContext.framebufferWidth = (cachedFramebufferWidth == 0 ) ? 800 : engineState->width;
    backendContext.framebufferHeight = (cachedFramebufferHeight == 0 ) ? 800 : engineState->height;


    // TODO: change this to cached stuff
    backendContext.framebufferWidth = engineState->width;
    backendContext.framebufferHeight = engineState->height;

    VkApplicationInfo appInfo = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    /* appInfo.pApplicationName    = appName; */
    appInfo.pApplicationName    = "test";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "Phoenix Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo     = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo         = &appInfo;

    // Get array of required extensions
    const char *appRequiredExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _DEBUG
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
    };

    const char *requiredExtensions[ArrayLen(appRequiredExtensions) +
                                   ArrayLen(VULKAN_PLATFORM_REQUIRED_EXTENSIONS)];
    for (u32 i = 0; i < ArrayLen(requiredExtensions); i++)
    {
        requiredExtensions[i] = (i < ArrayLen(appRequiredExtensions))
                                ? appRequiredExtensions[i]
                                : VULKAN_PLATFORM_REQUIRED_EXTENSIONS[i - ArrayLen(appRequiredExtensions)];
    }

#ifdef _DEBUG
    PXDEBUG("Vulkan required extensions:");
    for (u32 i = 0; i < ArrayLen(requiredExtensions); i++)
    {
        PXDEBUG("%s", requiredExtensions[i]);
    }
#endif

    // Validations layers TODO
#ifndef _DEBUG
    char *requiredValidationLayers[] = {};
#else

    char *requiredValidationLayers[] = {
        "VK_LAYER_KHRONOS_validation",
    };

    if (!CheckIfRequiredLayersAreAvailable(ArrayLen(requiredValidationLayers), requiredValidationLayers,
            &engineState->frameArena))
    {
        return 0; // NOTE: func above logs error for us
    }

#endif // !_DEBUG

    createInfo.enabledExtensionCount    = ArrayLen(requiredExtensions);
    createInfo.ppEnabledExtensionNames  = requiredExtensions;
    createInfo.enabledLayerCount        = ArrayLen(requiredValidationLayers);
    createInfo.ppEnabledLayerNames      = requiredValidationLayers;

    VK_CHECK(vkCreateInstance(&createInfo, backendContext.allocator, &backendContext.instance));
    PXINFO("Vulkan instance created.");

#ifdef _DEBUG
    PXDEBUG("Creating Vulkan debugger...");
    u32 logSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | 
                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType               = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity     = logSeverity;
    debugCreateInfo.messageType         =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback     = VKDebugCallback;
    debugCreateInfo.pUserData           = 0;

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(backendContext.instance, "vkCreateDebugUtilsMessengerEXT");
    PX_ASSERT_MSG(func, "Failed to create vulkan debug messenger!");
    VK_CHECK(func(backendContext.instance, &debugCreateInfo, backendContext.allocator, &backendContext.debugMessenger));

    PXDEBUG("Vulkan debugger created.");

#endif

    PXDEBUG("Creating a Vulkan suface...");
    if (!PlatformVulkanCreateSurface(engineState->platformState, &backendContext))
    {
        return 0;
    }
    PXINFO("Vulkan surface created succesfully.");

    if (!VulkanCreateDevice(&backendContext, &engineState->permArena, &engineState->frameArena))
    {
        PXERROR("Failed to create device!");
        return 0;
    }

    VulkanSwapchainCreate(
        &backendContext,
        engineState->width,
        engineState->height,
        &engineState->permArena,
        &backendContext.swapchain);

    VulkanRenderpassCreate(
        &backendContext,
        0, 0, backendContext.framebufferWidth, backendContext.framebufferHeight,
        0.5f, 0.0f, 0.5f, 1.0f,
        1.0f,
        0,
        &backendContext.mainRenderpass);

    // Swapchain framebuffers
    backendContext.swapchain.framebuffers = (vulkan_framebuffer *)PXMemoryArenaAlloc(
        &engineState->permArena,
        sizeof(vulkan_framebuffer) * backendContext.swapchain.imageCount);
    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        backendContext.swapchain.framebuffers[imageIndex] = {};
    }
    BackendRegenerateBuffers(backend, &backendContext.swapchain, &backendContext.mainRenderpass,
        &engineState->permArena);

    BackendCreateCommandBuffers(backend, &engineState->permArena);

    // Create sync objects
    // Allocate memory
    backendContext.imageAvailableSemaphores = (VkSemaphore *)PXMemoryArenaAlloc(
        &engineState->permArena,
        sizeof(VkSemaphore) * backendContext.swapchain.maxFramesInFlight);
    backendContext.queueCompleteSemaphores = (VkSemaphore *)PXMemoryArenaAlloc(
        &engineState->permArena,
        sizeof(VkSemaphore) * backendContext.swapchain.maxFramesInFlight);
    backendContext.inFlightFences = (vulkan_fence *)PXMemoryArenaAlloc(
        &engineState->permArena,
        sizeof(vulkan_fence) * backendContext.swapchain.maxFramesInFlight);

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.maxFramesInFlight; imageIndex++)
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

        vkCreateSemaphore(
            backendContext.device.logicalDevice,
            &semaphoreCreateInfo,
            backendContext.allocator,
            backendContext.imageAvailableSemaphores + imageIndex);

        vkCreateSemaphore(
            backendContext.device.logicalDevice,
            &semaphoreCreateInfo,
            backendContext.allocator,
            backendContext.queueCompleteSemaphores + imageIndex);

        // NOTE: We want to create the fences for the first frame on a signaled state
        // so that the application does not wait indefinetly
        VulkanFenceCreate(&backendContext, 1, backendContext.inFlightFences + imageIndex);
    }

    backendContext.imagesInFlight = (vulkan_fence **)PXMemoryArenaAlloc(
        &engineState->permArena,
        sizeof(vulkan_fence *) * backendContext.swapchain.imageCount);
    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        backendContext.imagesInFlight[imageIndex] = 0;
    }

    PXINFO("Vulkan renderer initialized sucessfully.");
    return 1;
}

PXAPI void
ShutdownRendererBackend(renderer_backend *backend)
{
    PXDEBUG("Shutting down renderer backend.");

    vkDeviceWaitIdle(backendContext.device.logicalDevice);

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.maxFramesInFlight; imageIndex++)
    {
        if (backendContext.imageAvailableSemaphores[imageIndex])
        {
            vkDestroySemaphore(
                backendContext.device.logicalDevice,
                backendContext.imageAvailableSemaphores[imageIndex],
                backendContext.allocator);

            backendContext.imageAvailableSemaphores[imageIndex] = 0;
        }

        if (backendContext.queueCompleteSemaphores[imageIndex])
        {
            vkDestroySemaphore(
                backendContext.device.logicalDevice,
                backendContext.queueCompleteSemaphores[imageIndex],
                backendContext.allocator);

            backendContext.queueCompleteSemaphores[imageIndex] = 0;
        }

        VulkanFenceDestroy(&backendContext, backendContext.inFlightFences + imageIndex);
    }
    backendContext.imagesInFlight = 0; // NOTE: no need to deallocate since it's allocated on arena
    backendContext.inFlightFences = 0;
    backendContext.queueCompleteSemaphores = 0;
    backendContext.imageAvailableSemaphores = 0;

    // Command buffers
    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        if (backendContext.graphicsCommandBuffers[imageIndex].handle)
        {
            VulkanCommandBufferFree(
                &backendContext,
                backendContext.device.graphicsCommandPool,
                backendContext.graphicsCommandBuffers + imageIndex);
            backendContext.graphicsCommandBuffers[imageIndex].handle = 0;
        }
    }
    backendContext.graphicsCommandBuffers = 0;

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        VulkanFramebufferDestroy(&backendContext, backendContext.swapchain.framebuffers + imageIndex);
    }

    VulkanRenderPassDestroy(&backendContext, &backendContext.mainRenderpass);

    VulkanSwapchainDestroy(&backendContext, &backendContext.swapchain);

    vkDestroySurfaceKHR(backendContext.instance, backendContext.surface, backendContext.allocator);

    VulkanDestroyDevice(&backendContext);

#if defined(_DEBUG)
    if (backendContext.debugMessenger)
    {
        PXDEBUG("Destroying Vulkan debugger...");
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(backendContext.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(backendContext.instance, backendContext.debugMessenger, backendContext.allocator);
    }
#endif

    PXDEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(backendContext.instance, backendContext.allocator);
}

PXAPI void
RendererBackendOnResized(u32 width, u32 height, renderer_backend *backend)
{
    /* backendContext.framebufferWidth  = width; */
    /* backendContext.framebufferHeight = height; */
    cachedFramebufferWidth  = width;
    cachedFramebufferHeight = height;
    backendContext.framebufferSizedGeneration++;

    PXINFO("Vulkan renderer backend resized: w/h/gen: %i/%i/%llu", width, height,
            backendContext.framebufferSizedGeneration);

    // NOTE: do we want to cache the framebuffer w/h?

    // TODO: finish this
}

PXAPI b8
RendererBackendBeginFrame(f32 deltaTime, renderer_backend *backend)
{
    if (backendContext.recreatingSwapchain)
    {
        VkResult result = vkDeviceWaitIdle(backendContext.device.logicalDevice);
        if (!VulkanResultIsSuccess(result))
        {
            PXERROR("RendererBackendBeginFrame vkDeviceWaitIdle() (1) failed");
            return 0;
        }
        PXINFO("Recreating swapchain, booting.");
        return 0;
    }

    if (backendContext.framebufferSizedGeneration != backendContext.lastFramebufferGeneration)
    {
        VkResult result = vkDeviceWaitIdle(backendContext.device.logicalDevice);
        if (!VulkanResultIsSuccess(result))
        {
            PXERROR("RendererBackendBeginFrame vkDeviceWaitIdle() (2) failed");
            return 0;
        }

        // If swapchain recreation failed (because, for i.e. the window was minimised)
        // bootout before setting the flag
        if (!SwapchainRecreate(backend))
        {
            return 0;
        }

        PXINFO("Resized, booting.");
        return 0;
    }

    // Wait for the execution of the current frame to complete
    if (!VulkanFenceWait(
            &backendContext,
            backendContext.inFlightFences + backendContext.currentFrame,
            UINT64_MAX))
    {
        PXWARN("In flight fence wait faliure!");
        return 0;
    }

    // Acquire the next image from the swapchain. Pass the semaphore that should be signaled
    // when this complete. This semaphore will be waited upon later by the queue
    // submission to ensure this image is available
    if (!VulkanSwapchainGetNextImageIndex(
            &backendContext,
            &backendContext.swapchain,
            UINT64_MAX,
            backendContext.imageAvailableSemaphores[backendContext.currentFrame],
            0,
            &backendContext.imageIndex))
    {
        return 0;
    }

    vulkan_command_buffer *cmdbuf = backendContext.graphicsCommandBuffers + backendContext.imageIndex;
    VulkanCommandBufferReset(cmdbuf);
    VulkanCommandBufferBegin(cmdbuf, 0, 0, 0);

    // Dynamic state
    VkViewport viewport;
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (f32)backendContext.framebufferWidth;
    viewport.height = (f32)backendContext.framebufferHeight;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    VkRect2D scissor;
    scissor.offset.x = scissor.offset.y = 0; 
    scissor.extent.width = backendContext.framebufferWidth;
    scissor.extent.height = backendContext.framebufferHeight;

    vkCmdSetViewport(cmdbuf->handle, 0, 1, &viewport);
    vkCmdSetScissor(cmdbuf->handle, 0, 1, &scissor);

    backendContext.mainRenderpass.w = backendContext.framebufferWidth;
    backendContext.mainRenderpass.h = backendContext.framebufferHeight;

    VulkanRenderpassBegin(
        cmdbuf,
        &backendContext.mainRenderpass,
        backendContext.swapchain.framebuffers[backendContext.imageIndex].handle);

    return 1;
}

// NOTE: !!! RendererBackendEndFrame !!! should NOT get called
// if RendererBackendBeginFrame fails
PXAPI b8
RendererBackendEndFrame(f32 deltaTime, renderer_backend *backend)
{
    vulkan_command_buffer *cmdbuf = &backendContext.graphicsCommandBuffers[backendContext.imageIndex];

    VulkanRenderpassEnd(cmdbuf, &backendContext.mainRenderpass);

    VulkanCommandBufferEnd(cmdbuf);

    // Make sure previous frame is not using current image
    if (backendContext.imagesInFlight[backendContext.imageIndex] != VK_NULL_HANDLE)
    {
        VulkanFenceWait(
            &backendContext,
            backendContext.imagesInFlight[backendContext.imageIndex],
            UINT64_MAX);
    }

    // Mark image fence as in-use by this frame
    backendContext.imagesInFlight[backendContext.imageIndex] =  
            backendContext.inFlightFences + backendContext.imageIndex;

    // Reset fence so it can be used in next frame
    VulkanFenceReset(&backendContext, backendContext.imagesInFlight[backendContext.currentFrame]);

    VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdbuf->handle;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = backendContext.queueCompleteSemaphores + backendContext.currentFrame;

    // Wait semaphores ensure that operation cannot begin until image is available
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = backendContext.imageAvailableSemaphores + backendContext.currentFrame;

    VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.pWaitDstStageMask = flags;

    VkResult result = vkQueueSubmit(
        backendContext.device.graphicsQueue,
        1,
        &submitInfo,
        backendContext.inFlightFences[backendContext.currentFrame].handle);

    if (result != VK_SUCCESS)
    {
        PXERROR("vkQueueSubmit failed with result");
        return 0;
    }

    VulkanCommandBufferUpdateSubmitted(cmdbuf);

    // Present to screen
    VulkanSwapchainPresent(
        &backendContext,
        &backendContext.swapchain,
        backendContext.device.graphicsQueue,
        backendContext.device.presentQueue,
        backendContext.queueCompleteSemaphores[backendContext.currentFrame],
        backendContext.imageIndex);

    return 1;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT /* msgType */,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* /* user_data */)
{
    switch (msgSeverity)
    {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            PXERROR("%s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            PXWARN("%s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            PXINFO("%s", callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            PXTRACE("%s", callbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

PXAPI void
BackendCreateCommandBuffers(renderer_backend *backend, mem_arena *permArena)
{
    if (!backendContext.graphicsCommandBuffers)
    {
        backendContext.graphicsCommandBuffers = (vulkan_command_buffer *)PXMemoryArenaAlloc(
            permArena, sizeof(vulkan_command_buffer) * backendContext.swapchain.imageCount);

        for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
        {
            backendContext.graphicsCommandBuffers[imageIndex] = {};
        }
    }

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        if (backendContext.graphicsCommandBuffers[imageIndex].handle)
        {
            VulkanCommandBufferFree(
                &backendContext,
                backendContext.device.graphicsCommandPool,
                &backendContext.graphicsCommandBuffers[imageIndex]);
        }

        backendContext.graphicsCommandBuffers[imageIndex] = {};
        VulkanCommandBufferAllocate(
            &backendContext,
            backendContext.device.graphicsCommandPool,
            1,
            &backendContext.graphicsCommandBuffers[imageIndex]);
    }

    PXINFO("Vulkan command buffers created.");
}


PXAPI void
BackendRegenerateBuffers(
    renderer_backend *backend,
    vulkan_swapchain *swapchain,
    vulkan_renderpass *renderpass,
    mem_arena *permArena)
{
    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        u32 attachmentCount = 2;
        VkImageView attachments[] = {
            swapchain->views[imageIndex],
            swapchain->depthAttachment.view
        };

        VulkanFramebufferCreate(
            &backendContext,
            permArena,
            renderpass,
            backendContext.framebufferWidth, 
            backendContext.framebufferHeight,
            attachmentCount,
            attachments,
            backendContext.swapchain.framebuffers + imageIndex);
    }
}

PXAPI b8
SwapchainRecreate(renderer_backend *backend)
{
    if (backendContext.recreatingSwapchain)
    {
        PXDEBUG("SwapchainRecreate called when already recreating. Booting.");
        return 0;
    }

    if (backendContext.framebufferWidth == 0 || backendContext.framebufferHeight == 0)
    {
        PXDEBUG("SwapchainRecreate called when window is <1 in a dimension. Booting");
        return 0;
    }

    backendContext.recreatingSwapchain = 1;

    vkDeviceWaitIdle(backendContext.device.logicalDevice);

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        backendContext.imagesInFlight[imageIndex] = 0;
    }

    VulkanDeviceQuerySwapchainSupport(
        backendContext.device.physicalDevice,
        backendContext.surface,
        backendContext.permArena,
        &backendContext.device.swapchainSupportInfo);
    VulkanDeviceDetectDepthFormat(&backendContext.device);

    VulkanSwapchainRecreate(
        &backendContext,
        cachedFramebufferWidth,
        cachedFramebufferHeight,
        &backendContext.swapchain);

    // Sync the framebuffer size with the cached size
    backendContext.framebufferWidth  = cachedFramebufferWidth;
    backendContext.framebufferHeight = cachedFramebufferHeight;
    backendContext.mainRenderpass.w = backendContext.framebufferWidth;
    backendContext.mainRenderpass.h = backendContext.framebufferHeight;
    cachedFramebufferWidth  = 0;
    cachedFramebufferHeight = 0;

    backendContext.lastFramebufferGeneration = backendContext.framebufferSizedGeneration;

    // Cleanup swapchain
    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        VulkanCommandBufferFree(
            &backendContext,
            backendContext.device.graphicsCommandPool,
            backendContext.graphicsCommandBuffers + imageIndex);
    }

    for (u32 imageIndex = 0; imageIndex < backendContext.swapchain.imageCount; imageIndex++)
    {
        VulkanFramebufferDestroy(&backendContext, backendContext.swapchain.framebuffers + imageIndex);
    }

    // NOTE: this is temporary, i know it's a repeat of above
    backendContext.mainRenderpass.x = 0;
    backendContext.mainRenderpass.y = 0;
    backendContext.mainRenderpass.w = backendContext.framebufferWidth;
    backendContext.mainRenderpass.h = backendContext.framebufferHeight;

    BackendRegenerateBuffers(
        backend,
        &backendContext.swapchain,
        &backendContext.mainRenderpass,
        backendContext.permArena);

    BackendCreateCommandBuffers(backend, backendContext.permArena);

    backendContext.recreatingSwapchain = 0;

    return 1;
}
