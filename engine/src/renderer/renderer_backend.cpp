#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

// NOTE: declare backendContext before cpp includes so that they can use it
global_var vulkan_context backendContext;

#include <renderer/vulkan_renderer_backend_helpers.cpp>
#include <renderer/vulkan_device.cpp>


VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* user_data);

PXAPI b8
InitRendererBackend(char *appName, renderer_backend *backend, engine_state *engineState)
{
    // TODO: custom allocator
    backendContext.allocator = 0;

    VkApplicationInfo appInfo = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName    = appName;
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

    if (!VulkanCreateDevice(&backendContext, &engineState->frameArena))
    {
        PXERROR("Failed to create device!");
        return 0;
    }

    PXINFO("Vulkan renderer initialized sucessfully.");
    return 1;
}

PXAPI void
ShutdownRendererBackend(renderer_backend *backend)
{
    PXDEBUG("Shutting down renderer backend.");

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

}

PXAPI b8
RendererBackendBeginFrame(f32 deltaTime, renderer_backend *backend)
{
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
