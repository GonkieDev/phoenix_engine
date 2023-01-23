#pragma once

#include <defines.hpp>
#include <core/asserts.h>
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
    {                                   \
        PX_ASSERT(expr == VK_SUCCESS);  \
    }

struct vulkan_context
{
    VkInstance                  instance;
    VkAllocationCallbacks       *allocator;
#ifdef _DEBUG
    VkDebugUtilsMessengerEXT    debugMessenger;
#endif
};
