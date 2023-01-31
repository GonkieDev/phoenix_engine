#include <core/engine.hpp>

#include <renderer/vulkan_renderer_types.hpp>

PXAPI void
VulkanImageViewCreate(
    vulkan_context      *context,
    VkFormat            format,
    vulkan_image        *image,
    VkImageAspectFlags  aspectFlags)
{
    VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
    /* createInfo.pNext                = 0; */
    /* createInfo.flags                = 0; */
    createInfo.image                = image->handle;
    createInfo.viewType             = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format               = format;
    /* createInfo.components           = ; */

    // TODO(gonk): configurable spport
    createInfo.subresourceRange.aspectMask      = aspectFlags;
    createInfo.subresourceRange.baseMipLevel    = 0;
    createInfo.subresourceRange.levelCount      = 1;
    createInfo.subresourceRange.baseArrayLayer  = 0;
    createInfo.subresourceRange.layerCount      = 1;

    VK_CHECK(vkCreateImageView(
        context->device.logicalDevice,
        &createInfo,
        context->allocator,
        &image->view));
}

struct vulkan_create_image_params
{
    vulkan_context          *context;
    u32                     width;
    u32                     height;
    VkFormat                format;
    VkImageTiling           tiling;
    VkImageUsageFlags       usageFlags;
    VkMemoryPropertyFlags   memoryFlags;
    b32                     createView;
    VkImageAspectFlags      viewAspectFlags;
    vulkan_image            *outImage;
};

PXAPI void
VulkanCreateImage(vulkan_create_image_params *params)
{
    params->outImage->width  = params->width;
    params->outImage->height = params->height;

    VkImageCreateInfo imgCreateInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
    /* imgCreateInfo.pNext                    = 0; */
    /* imgCreateInfo.flags                    = 0; */
    imgCreateInfo.imageType                = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format                   = params->format;
    imgCreateInfo.extent.width             = params->width;
    imgCreateInfo.extent.height            = params->height;
    imgCreateInfo.extent.depth             = 1; // TODO(gonk): support configurable depth
    imgCreateInfo.mipLevels                = 4; // TODO(gonk): support configurable mipmap level
    imgCreateInfo.arrayLayers              = 1; // TODO(gonk): support configurable number of layers
    imgCreateInfo.samples                  = VK_SAMPLE_COUNT_1_BIT; // TODO(gonk): configurable sample count
    imgCreateInfo.tiling                   = params->tiling;
    imgCreateInfo.usage                    = params->usageFlags;
    imgCreateInfo.sharingMode              = VK_SHARING_MODE_EXCLUSIVE; // TODO: support configurable sharingMode
    /* imgCreateInfo.queueFamilyIndexCount    = 0; */
    /* imgCreateInfo.pQueueFamilyIndices      = 0; */
    imgCreateInfo.initialLayout            = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_CHECK(vkCreateImage(
        params->context->device.logicalDevice,
        &imgCreateInfo,
        params->context->allocator,
        &params->outImage->handle));

    // Query image memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(params->context->device.logicalDevice,
            params->outImage->handle,
            &memoryRequirements);

    i32 memoryType = FindMemoryIndex(params->context, memoryRequirements.memoryTypeBits, params->memoryFlags);
    if (memoryType == -1)
    {
        PXERROR("VulkanCreateImage: Required memory type not found. Image not valid.");
    }

    // Allocate memory
    VkMemoryAllocateInfo allocationInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocationInfo.allocationSize   = memoryRequirements.size;
    allocationInfo.memoryTypeIndex  = memoryType;
    VK_CHECK(vkAllocateMemory(
        params->context->device.logicalDevice,
        &allocationInfo,
        params->context->allocator,
        &params->outImage->memory));

    // Bind memory
    VK_CHECK(vkBindImageMemory(
        params->context->device.logicalDevice,
        params->outImage->handle,
        params->outImage->memory,
        0)); // TODO(gonk): configurable memory offset (used for image pooling)

    // Create view
    if (params->createView)
    {
        params->outImage->view = 0;
        VulkanImageViewCreate(params->context, params->format, params->outImage, params->viewAspectFlags);
    }
}

PXAPI void
VulkanImageDestroy(vulkan_context *context, vulkan_image *image)
{
    if (image->view)
    {
        vkDestroyImageView(context->device.logicalDevice, image->view, context->allocator);
        image->view = 0;
    }

    if (image->memory)
    {
        vkFreeMemory(context->device.logicalDevice, image->memory, context->allocator);
        image->memory = 0;
    }

    if (image->handle)
    {
        vkDestroyImage(context->device.logicalDevice, image->handle, context->allocator);
        image->handle = 0;
    }
}
