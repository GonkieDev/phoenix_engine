#include <core/engine.hpp>

#include "vulkan_renderer_types.hpp"

global_var vulkan_context backendContext;

PXAPI b8
InitRendererBackend(wchar_t *gameName, renderer_backend *backend, struct engine_state *engineState)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext               = 0;
    // TODO: convert gameName to char
    appInfo.pApplicationName    = "Phoenix Engine";
    appInfo.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName         = "Phoenix Engine";
    appInfo.engineVersion       = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo createInfo     = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // TODO
    createInfo.pNext                    = 0;
    createInfo.flags                    = 0;
    createInfo.pApplicationInfo         = &appInfo;
    createInfo.enabledLayerCount        = 0;
    createInfo.ppEnabledLayerNames      = 0;
    createInfo.enabledExtensionCount    = 0;
    createInfo.ppEnabledExtensionNames  = 0;

    if (!vkCreateInstance(&createInfo, 0, &backendContext.instance))
    {
        PXFATAL("Vulkan instance failed to create!");
        return 0;
    }

    return 1;
}
