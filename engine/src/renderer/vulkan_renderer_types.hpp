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
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    vulkan_swapchain_support_info swapchainSupportInfo;

    u32 graphicsQueueIndex;
    u32 presentQueueIndex;
    u32 transferQueueIndex;

    VkPhysicalDeviceProperties       properties;
    VkPhysicalDeviceFeatures         features;
    VkPhysicalDeviceMemoryProperties memoryProperties;
};

struct vulkan_context
{
    VkInstance                  instance;
    VkAllocationCallbacks       *allocator;
    vulkan_device               device;
    VkSurfaceKHR                surface;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT    debugMessenger;
#endif
};
