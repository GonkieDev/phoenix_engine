#pragma once

#include <defines.hpp>
#include <core/asserts.h>
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
    {                                   \
        PX_ASSERT(expr == VK_SUCCESS);  \
    }

struct vulkan_swapchain_support_info
{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 formatsCount;
    VkSurfaceFormatKHR *formats;
    u32 presentModesCount;
    VkPresentModeKHR *presentModes;
};

struct vulkan_device
{
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vulkan_swapchain_support_info swapchainSupportInfo;

    u32 graphicsQueueIndex;
    u32 presentQueueIndex;
    u32 transferQueueIndex;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;

    VkPhysicalDeviceProperties       properties;
    VkPhysicalDeviceFeatures         features;
    VkPhysicalDeviceMemoryProperties memoryProperties;
};

struct vulkan_swapchain
{
    VkSurfaceFormatKHR imageFormat;
    u8                 maxFramesInFlight;
    VkSwapchainKHR     handle;

    u32                imageCount;
    VkImage            *images;
    VkImageView        *views;
};

struct vulkan_context
{
    VkInstance                  instance;
    VkAllocationCallbacks       *allocator = 0;
    vulkan_device               device;
    VkSurfaceKHR                surface;

    u32 framebufferWidth;
    u32 framebufferHeight;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT    debugMessenger;
#endif
};
