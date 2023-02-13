#pragma once

#include <defines.hpp>
#include <core/asserts.h>
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
    {                                   \
        PX_ASSERT(expr == VK_SUCCESS);  \
    }

// TODO: xxmemoryxx change most of these u32's to be u16's

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

    VkCommandPool graphicsCommandPool;

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

struct vulkan_framebuffer
{
    VkFramebuffer handle;
    u32 attachmentCount;
    VkImageView *attachments;
    vulkan_renderpass *renderpass;
};

enum vulkan_command_buffer_state
{
    COMMAND_BUFFER_STATE_NOT_ALLOCATED,
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDERPASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED
};

struct vulkan_command_buffer
{
    VkCommandBuffer handle;
    vulkan_command_buffer_state state;
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

    vulkan_framebuffer *framebuffers;
};

struct vulkan_fence 
{
    VkFence handle;
    b8 signaled;
};

struct vulkan_shader_stage
{
    VkShaderModuleCreateInfo createInfo;
    VkShaderModule handle;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo;
};

struct vulkan_pipeline
{
    VkPipeline handle;
    VkPipelineLayout pipelineLayout;
};

#define SHADER_OBJECT_STAGE_COUNT 2
struct vulkan_shader_object
{
    // vertex and fragment
    vulkan_shader_stage stages[SHADER_OBJECT_STAGE_COUNT];
    vulkan_pipeline pipeline;
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

    // Current width/height
    u64 framebufferWidth;
    u64 framebufferHeight;

    // Current generation of framebuffer size. If it does not match 
    // lastFramebufferGeneration, a new framebuffer should be
    // generated.
    u64 framebufferSizedGeneration;

    // The generation of the framebuffer when it was last created.
    // Set to framebufferSizedGeneration when updated.
    u64 lastFramebufferGeneration;

    vulkan_renderpass mainRenderpass;

    vulkan_command_buffer *graphicsCommandBuffers;

    VkSemaphore *imageAvailableSemaphores;
    VkSemaphore *queueCompleteSemaphores;

    u32 inFlightFenceCount;
    vulkan_fence *inFlightFences;
    vulkan_fence **imagesInFlight; // NOTE: these fences are not allocated by this list

    struct mem_arena *permArena;
    struct mem_arena *tempArena;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT    debugMessenger;
#endif
};
