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

// TODO: get rid of allocations in these functions that can be used with variable lengths stack arrays
PXAPI b8
InitRendererBackend(char *appName, renderer_backend *backend, engine_state *engineState)
{
    // TODO: custom allocator
    backendContext.allocator = 0;

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

    for_u32(imageIndex, backendContext.swapchain.maxFramesInFlight)
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
    for_u32(imageIndex, backendContext.swapchain.imageCount)
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

    for_u32(imageIndex, backendContext.swapchain.imageCount)
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
    for_u32(imageIndex, backendContext.swapchain.imageCount)
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

    for_u32(imageIndex, backendContext.swapchain.imageCount)
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
RendererBackendOnResize(u16 width, u16 height, renderer_backend *backend)
{
    backendContext.framebufferWidth  = width;
    backendContext.framebufferHeight = height;
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
        if (result != VK_SUCCESS)
        {
            // TODO: get error string
            PXERROR("RendererBackendBeginFrame vkDeviceWaitIdle() (1) failed");
            return 0;
        }
        PXINFO("Recreating swapchain, booting.");
        return 0;
    }

    if (backendContext.framebufferSizedGeneration != backendContext.lastFramebufferGeneration)
    {
        VkResult result = vkDeviceWaitIdle(backendContext.device.logicalDevice);
        if (result != VK_SUCCESS)
        {
            // TODO: get error string
            PXERROR("RendererBackendBeginFrame vkDeviceWaitIdle() (2) failed");
            return 0;
        }

        // If swapchain recreation failed (because, for i.e. the window was minimised)
        // bootout before setting the flag
        if (!VulkanSwapchainRecreate(&backendContext, , , &backendContext.swapchain))
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
            backendContext.imageAvailableSemaphores + backendContext.currentFrame,
            0,
            &backendContext.imageIndex))
    {
        return 0;
    }

    vulkan_command_buffer *commandBuffer = backendContext.graphicsCommandBuffers + backendContext.currentFrame;
    VulkanCommandBufferReset(commandBuffer);
    VulkanCommandBufferBegin(commandBuffer, 0, 0, 0);

    // Dynamic state
    VkViewport viewport;
    viewport.x = 0.f;
    viewport.y = (f32)backendContext.framebufferHeight;
    viewport.width = (f32)backendContext.framebufferWidth;
    viewport.height = -((f32)backendContext.framebufferHeight);
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    return 1;
}

PXAPI b8
RendererBackendEndFrame(f32 deltaTime, renderer_backend *backend)
{
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

        for_u32(imageIndex, backendContext.swapchain.imageCount)
        {
            backendContext.graphicsCommandBuffers[imageIndex] = {};
        }
    }

    for_u32(imageIndex, backendContext.swapchain.imageCount)
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
    for_u32(imageIndex, swapchain->imageCount)
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
