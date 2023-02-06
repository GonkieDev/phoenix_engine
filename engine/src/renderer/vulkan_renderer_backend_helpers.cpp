#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

PXAPI b8
VulkanResultIsSuccess(VkResult result)
{
    // Codes from: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkResult.html
    switch (result)
    {
        // Success codes
        case VK_SUCCESS:
        case VK_NOT_READY:
        case VK_TIMEOUT:
        case VK_EVENT_SET:
        case VK_EVENT_RESET:
        case VK_INCOMPLETE:
        case VK_SUBOPTIMAL_KHR:
        case VK_THREAD_IDLE_KHR:
        case VK_THREAD_DONE_KHR:
        case VK_OPERATION_DEFERRED_KHR:
        case VK_OPERATION_NOT_DEFERRED_KHR:
        case VK_PIPELINE_COMPILE_REQUIRED:
            return 1;

        default:
        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        case VK_ERROR_INITIALIZATION_FAILED:
        case VK_ERROR_DEVICE_LOST:
        case VK_ERROR_MEMORY_MAP_FAILED:
        case VK_ERROR_LAYER_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_INCOMPATIBLE_DRIVER:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
        case VK_ERROR_FRAGMENTED_POOL:
        case VK_ERROR_SURFACE_LOST_KHR:
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        case VK_ERROR_INVALID_SHADER_NV:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        case VK_ERROR_FRAGMENTATION:
        case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        /* case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: */
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        /* case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR: */
        /* case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR: */
        /* case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR: */
        /* case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR: */
        /* case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR: */
        /* case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR: */
        case VK_ERROR_UNKNOWN:
            return 0;
    }
}

PXAPI b8 
CheckIfRequiredLayersAreAvailable(u32 requiredValidationLayersCount, 
        char **requiredValidationLayers, mem_arena *arena)
{
    // Get availableLayersCount
    u32 availableLayersCount;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayersCount, 0));

    VkLayerProperties *availableLayers = (VkLayerProperties*)
        PXMemoryArenaAlloc(arena, sizeof(VkLayerProperties) * availableLayersCount);

    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers));

    PXINFO("Validation layers enabled. Enumerating...");

    // Compare to required layers
    for (u32 reqLayerIndex = 0;
         reqLayerIndex < requiredValidationLayersCount;
         reqLayerIndex++)
    {
        PXINFO("Searching for layer %s...", requiredValidationLayers[reqLayerIndex]);

        b32 layerFound = 0;
        char *reqLayerName = requiredValidationLayers[reqLayerIndex];

        for (u32 avLayerIndex = 0;
             avLayerIndex < availableLayersCount;
             avLayerIndex++)
        {
            if (StringEquals(reqLayerName, availableLayers[avLayerIndex].layerName))
            {
                layerFound = 1;
                PXINFO("Found.");
                break;
            }
        }
        
        if (!layerFound)
        {
            PXFATAL("Required validation layer is missing: %s", requiredValidationLayers[reqLayerIndex]);
            return 0;
        }
    }

    PXINFO("All required validation layers are present.");

    return 1;
}

PXAPI i32
FindMemoryIndex(vulkan_context *context, u32 typeFilter, u32 propertyFlags)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(context->device.physicalDevice, &memoryProperties);

    for_u32(i, memoryProperties.memoryTypeCount)
    {
        if (typeFilter & (1 << i) &&
            (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
        {
            return i;
        }
    }

    PXWARN("Unabled to find suitable memory type!");
    return -1;
}
