#include <renderer/vulkan_renderer_types.hpp>

PXAPI b8
VulkanBufferCreate(
    vulkan_context *context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memoryPropertyFlags,
    vulkan_buffer *outBuffer,
    b8 bindOnCreate = 0);

PXAPI void
VulkanBufferDestroy(vulkan_context *context, vulkan_buffer *buffer);

// NOTE: This creates a new buffer
PXAPI b8
VulkanBufferResize(
    vulkan_context *context,
    u64 newSize,
    vulkan_buffer *buffer,
    VkQueue queue,
    VkCommandPool pool);

PXAPI inline void
VulkanBufferBind(vulkan_context *context, vulkan_buffer *buffer, u64 offset);

PXAPI inline void *
VulkanBufferLockMemory(vulkan_context *context, vulkan_buffer *buffer, u64 offset, u64 size, u32 flags);

PXAPI inline void
VulkanBufferUnlockMemory(vulkan_context *context, vulkan_buffer *buffer);

PXAPI inline void
VulkanBufferLoadData(
    vulkan_context *context,
    vulkan_buffer *buffer,
    u64 offset,
    u64 size,
    u32 flags,
    void *data);

PXAPI void
VulkanBufferCopyTo(
    vulkan_context *context,
    VkCommandPool pool,
    VkFence fence,
    VkQueue queue,
    VkBuffer source,
    u64 sourceOffset,
    VkBuffer dest,
    u64 destOffset,
    u64 size);



// ====================================================
// ====================================================
// ====================================================

PXAPI b8
VulkanBufferCreate(
    vulkan_context *context,
    u64 size,
    VkBufferUsageFlagBits usage,
    u32 memoryPropertyFlags,
    vulkan_buffer *outBuffer,
    b8 bindOnCreate)
{
    *outBuffer = {};
    outBuffer->totalSize = size;
    outBuffer->usage = usage;
    outBuffer->memoryPropertyFlags = memoryPropertyFlags;

    VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferCreateInfo.size = size;
    bufferCreateInfo.usage = usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(
        context->device.logicalDevice,
        &bufferCreateInfo,
        context->allocator,
        &outBuffer->handle));

    // Gather memory requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logicalDevice, outBuffer->handle, &requirements);
    FindMemoryIndex(context, requirements.memoryTypeBits, outBuffer->memoryPropertyFlags);

    if (outBuffer->memoryIndex == -1)
    {
        PXERROR("Unable to create vulkan buffer because the required memory type index was not found.");
        return 0;
    }

    // Allocate the memory
    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)outBuffer->memoryIndex;

    VkResult allocationResult = vkAllocateMemory(
        context->device.logicalDevice,
        &allocateInfo,
        context->allocator,
        &outBuffer->memory);

    if (allocationResult != VK_SUCCESS)
    {
        PXERROR("Unable to create vulkan buffer because the required memory allocation failed. Error: %i",
            allocationResult);
        return 0;
    }

    if (bindOnCreate)
    {
        VulkanBufferBind(context, outBuffer, 0);
    }

    return 1;
}


PXAPI void
VulkanBufferDestroy(vulkan_context *context, vulkan_buffer *buffer)
{
    if (buffer->memory)
    {
        vkFreeMemory(context->device.logicalDevice, buffer->memory, context->allocator);
        buffer->memory = 0;
    }

    if (buffer->handle)
    {
        vkDestroyBuffer(context->device.logicalDevice, buffer->handle, context->allocator);
        buffer->handle = 0;
    }

    buffer->totalSize = 0;
    buffer->usage = 0;
    buffer->isLocked = 0;
}

PXAPI b8
VulkanBufferResize(
    vulkan_context *context,
    u64 newSize,
    vulkan_buffer *buffer,
    VkQueue queue,
    VkCommandPool pool)
{
    VkBufferCreateInfo bufferCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    bufferCreateInfo.size = newSize;
    bufferCreateInfo.usage = buffer->usage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer newBuffer;
    VK_CHECK(vkCreateBuffer(
        context->device.logicalDevice,
        &bufferCreateInfo,
        context->allocator,
        newBuffer));

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(context->device.logicalDevice, newBuffer, &requirements);

    VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = (u32)buffer->memoryIndex;

    VkDeviceMemory newMemory;
    VkResult allocationResult = vkAllocateMemory(
        context->device.logicalDevice,
        &allocateInfo,
        context->allocator,
        &newMemory);

    if (allocationResult != VK_SUCCESS)
    {
        PXERROR("Unable to resize vulkan buffer because the required memory allocation failed. Error: %i",
            allocationResult);
        return 0;
    }

    VK_CHECK(vkBindBufferMemory(context->device.logicalDevice, newBuffer, newMemory, 0));

    // Copy data over
    VulkanBufferCopyTo(context, pool, 0, queue, buffer->handle, 0, newBuffer, 0, buffer->totalSize);

    vkDeviceWaitIdle(context->device.logicalDevice);

    if (buffer->memory)
    {
        vkFreeMemory(context->device.logicalDevice, buffer->memory, context->allocator);
        buffer->memory = 0;
    }

    if (buffer->handle)
    {
        vkDestroyBuffer(context->device.logicalDevice, buffer->handle, context->allocator);
        buffer->handle = 0;
    }

    buffer->handle = newBuffer;
    buffer->totalSize = newSize;
    buffer->memory = newMemory;

    return true;
}

PXAPI inline void
VulkanBufferBind(vulkan_context *context, vulkan_buffer *buffer, u64 offset)
{
    VK_CHECK(vkBindBufferMemory(context->device.logicalDevice, buffer->handle, buffer->memory, offset));
}

PXAPI inline void *
VulkanBufferLockMemory(vulkan_context *context, vulkan_buffer *buffer, u64 offset, u64 size, u32 flags)
{
    void *data;
    VK_CHECK(vkMapMemory(context->device.logicalDevice, buffer->memory, offset, size, flags, &data));
    return data;
}

PXAPI inline void
VulkanBufferUnlockMemory(vulkan_context *context, vulkan_buffer *buffer)
{
    vkUnmapMemory(context->device.logicalDevice, buffer->memory);
}

PXAPI inline void
VulkanBufferLoadData(
    vulkan_context *context,
    vulkan_buffer *buffer,
    u64 offset,
    u64 size,
    u32 flags,
    void *data)
{
    void *dataPtr;
    VK_CHECK(vkMapMemory(context->device.logicalDevice, buffer->memory, offset, size, flags, &dataPtr));
    // memcopy
    vkUnmapMemory(context->device.logicalDevice, buffer->memory);
}

