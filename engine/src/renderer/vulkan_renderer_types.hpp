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

    VkFormat depthFormat;
};

struct vulkan_image
{
    u32 width;
    u32 height;

    VkImage         handle;
    VkDeviceMemory  memory;
    VkImageView     view;

};

struct vulkan_swapchain
{
    VkSurfaceFormatKHR imageFormat;
    u8                 maxFramesInFlight;
    VkSwapchainKHR     handle;

    u32                imageCount;
    VkImage            *images;
    VkImageView        *views;

    vulkan_image       depthAttachment;
};

enum vulkan_renderpass_state
{
    RENDERPASS_STATE_READY,
    RENDERPASS_STATE_RECORDING,
    RENDERPASS_STATE_IN_RENDER_PASS,
    RENDERPASS_STATE_IN_RECORDING_ENDED,
    RENDERPASS_STATE_IN_SUBMITTED,
    RENDERPASS_STATE_NOT_ALLOCATED
};

struct vulkan_renderpass
{
    VkRenderPass handle;
    f32 x, y, w, h;
    f32 r,  g,  b,  a; // Clear color

    f32 depth;
    u32 stencil;

    vulkan_renderpass_state state;
};

enum vulkan_command_buffer_state
{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDERPASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED
};

struct vulkan_command_buffer
{
    VkCommandBuffer handle;
    vulkan_command_buffer_state state;
};


struct vulkan_context
{
    VkInstance                  instance;
    VkAllocationCallbacks       *allocator = 0;
    vulkan_device               device;
    VkSurfaceKHR                surface;

    vulkan_swapchain            swapchain;
    u32                         imageIndex;
    u32                         currentFrame;
    b8                          recreatingSwapchain;

    u32 framebufferWidth;
    u32 framebufferHeight;

    vulkan_renderpass mainRenderpass;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT    debugMessenger;
#endif
};
