/* #include "vulkan/vulkan_core.h" */
#include <core/engine.hpp>

struct vulkan_physical_device_requirements
{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    b8 samplerAnisotropy;
    b8 discreteGPU;

    char **deviceExtensionNames;
    u32 deviceExtensionNamesCount;
};

struct vulkan_physical_device_queue_family_info
{
    u32 graphicsFamilyIndex;
    u32 presentFamilyIndex;
    u32 computeFamilyIndex;
    u32 transferFamilyIndex;
};

PXAPI b8
SelectPhysicalDevice(vulkan_context *context, mem_arena *permArena, mem_arena *tempArena);

PXAPI b8
VulkanDeviceDetectDepthFormat(vulkan_device *device);

PXAPI b8
VulkanCreateDevice(vulkan_context *context, mem_arena *permArena, mem_arena *tempArena)
{
    if (!SelectPhysicalDevice(context, permArena, tempArena))
    {
        return 0;
    }

    PXINFO("Creating vulkan logical device...");

    // NOTE: do not create additional queues for shared indices
    b8 presentSharesGraphicsQueue  = context->device.presentQueueIndex  == context->device.graphicsQueueIndex;
    b8 transferSharesGraphicsQueue = context->device.transferQueueIndex == context->device.graphicsQueueIndex;

    u32 qIndexCount = 1 + (!presentSharesGraphicsQueue) + (!transferSharesGraphicsQueue);
    u32 queueIndices[qIndexCount];
    
    {
        u32 qIndex = 0;
        queueIndices[qIndex++] = context->device.graphicsQueueIndex;
        if (!presentSharesGraphicsQueue)  { queueIndices[qIndex++] = context->device.presentQueueIndex;  }
        if (!transferSharesGraphicsQueue) { queueIndices[qIndex++] = context->device.transferQueueIndex; }
    }

    VkDeviceQueueCreateInfo queueCreateInfos[qIndexCount];
    // NOTE: use this to replace VLAs?
    // VkDeviceQueueCreateInfo queueCreateInfos[32];
    for (u32 i = 0; i < qIndexCount; i++)
    {
        f32 queuePriority = 1.0f;
        queueCreateInfos[i] = {};
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = queueIndices[i];
        queueCreateInfos[i].queueCount = 1;
        // TODO: enabled this for future enhancement
        /* if (queueIndices[i] == context->device.graphicsQueueIndex) */
        /* { */
        /*     queueCreateInfos[i].queueCount = 2; */
        /* } */
        queueCreateInfos[i].flags = 0;
        queueCreateInfos[i].pNext = 0;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    // TODO: should be configuration driven
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType                = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext                = 0;
    deviceCreateInfo.queueCreateInfoCount = qIndexCount;
    deviceCreateInfo.pQueueCreateInfos    = queueCreateInfos;
    deviceCreateInfo.pEnabledFeatures     = &deviceFeatures;

    // Extensions
    char *extensionNames[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    deviceCreateInfo.enabledExtensionCount   = ArrayLen(extensionNames);
    deviceCreateInfo.ppEnabledExtensionNames = extensionNames;

    // Deprecated and ignored, so pass nothing
    deviceCreateInfo.enabledLayerCount   = 0;
    deviceCreateInfo.ppEnabledLayerNames = 0;

    VK_CHECK(vkCreateDevice(
        context->device.physicalDevice,
        &deviceCreateInfo,
        context->allocator,
        &context->device.logicalDevice));

    PXINFO("Logical device created!");

    // Get queues
    vkGetDeviceQueue(
        context->device.logicalDevice,
        context->device.graphicsQueueIndex,
        0,
        &context->device.graphicsQueue);

    vkGetDeviceQueue(
        context->device.logicalDevice,
        context->device.presentQueueIndex,
        0,
        &context->device.presentQueue);

    vkGetDeviceQueue(
        context->device.logicalDevice,
        context->device.transferQueueIndex,
        0,
        &context->device.transferQueue);

    PXINFO("Queues obtained.");

    // Create command pool
    VkCommandPoolCreateInfo commandPoolCreateInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
    commandPoolCreateInfo.queueFamilyIndex = context->device.graphicsQueueIndex;
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(
        context->device.logicalDevice,
        &commandPoolCreateInfo,
        context->allocator,
        &context->device.graphicsCommandPool));
    PXINFO("Graphics command pool created!");

    return 1;
}

PXAPI void
VulkanDestroyDevice(vulkan_context *context)
{
    // Unset queues
    context->device.graphicsQueue = 0;
    context->device.presentQueue  = 0;
    context->device.transferQueue = 0;

    vkDestroyCommandPool(
        context->device.logicalDevice,
        context->device.graphicsCommandPool,
        context->allocator);

    PXINFO("Destroying vulkan logical device...");
    if (context->device.logicalDevice)
    {
        vkDestroyDevice(context->device.logicalDevice, context->allocator);
        context->device.logicalDevice = 0;
    }

    context->device.physicalDevice = 0;

    context->device.graphicsQueueIndex = -1;
    context->device.presentQueueIndex  = -1;
    context->device.transferQueueIndex = -1;
}

PXAPI void
VulkanDeviceQuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, mem_arena *permArena,
        vulkan_swapchain_support_info *outSupportInfo)
{
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &outSupportInfo->capabilities));

    // NOTE: we only allocate if it hasn't been allocated yet so we don't get multiple allocations,
    // this works because the number of formats and present modes will always be the same
    // however their information might change

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &outSupportInfo->formatsCount, 0));
    if (outSupportInfo->formatsCount)
    {
        if (!outSupportInfo->formats)
        {
            outSupportInfo->formats = (VkSurfaceFormatKHR *)
                PXMemoryArenaAlloc(permArena, sizeof(VkSurfaceFormatKHR) * outSupportInfo->formatsCount);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &outSupportInfo->formatsCount,
                    outSupportInfo->formats));
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &outSupportInfo->presentModesCount, 0));
    if (outSupportInfo->presentModesCount)
    {
        if (!outSupportInfo->presentModes )
        {
            outSupportInfo->presentModes = (VkPresentModeKHR  *)
                PXMemoryArenaAlloc(permArena, sizeof(VkPresentModeKHR) * outSupportInfo->presentModesCount);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &outSupportInfo->formatsCount,
                    outSupportInfo->formats));
    }
}

PXAPI b8
PhysicalDeviceMeetsRequirements(
    VkPhysicalDevice                            device,
    VkSurfaceKHR                                surface,
    VkPhysicalDeviceProperties                  *properties,
    VkPhysicalDeviceFeatures                    *features,
    vulkan_physical_device_requirements         *requirements,
    mem_arena                                   *permArena, // NOTE: allocations required for program's lifetime
    mem_arena                                   *tempArena, // NOTE: stuff allocated on this can be freed after
    vulkan_physical_device_queue_family_info    *outQueueFamilyInfo,
    vulkan_swapchain_support_info               *outSwapchainSupport)
{
    outQueueFamilyInfo->graphicsFamilyIndex = -1;
    outQueueFamilyInfo->presentFamilyIndex  = -1;
    outQueueFamilyInfo->computeFamilyIndex  = -1;
    outQueueFamilyInfo->transferFamilyIndex = -1;

    if (requirements->discreteGPU)
    {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            PXINFO("Device is not a discrete GPU and one is required. Skipping.");
            return 0;
        }
    }

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, 0);
    /* VkQueueFamilyProperties *queueFamilies = (VkQueueFamilyProperties *) */
    /*     PXMemoryArenaAlloc(tempArena, sizeof(VkQueueFamilyProperties) * queueFamilyCount); */
    VkQueueFamilyProperties queueFamilies[32];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);


    PXINFO("Graphics | Present | Compute | Transfer | Name");
    u8 minTransferScore = 255;
    for (u32 i = 0; i < queueFamilyCount; i++)
    {
        u8 currentTransferScore = 0;

        // Graphics queue
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            outQueueFamilyInfo->graphicsFamilyIndex = i;
            currentTransferScore++;
        }

        // Compute queue
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            outQueueFamilyInfo->computeFamilyIndex = i;
            currentTransferScore++;
        }

        // Transfer queue
        if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            if (currentTransferScore <= minTransferScore)
            {
                outQueueFamilyInfo->transferFamilyIndex = i;
                minTransferScore = currentTransferScore;
            }
        }

        VkBool32 supportsPresent = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supportsPresent));
        if (supportsPresent)
        {
            outQueueFamilyInfo->presentFamilyIndex =  i;
        }
    }

    PXINFO("       %d |       %d |       %d |        %d | %s",
            outQueueFamilyInfo->graphicsFamilyIndex,
            outQueueFamilyInfo->presentFamilyIndex,
            outQueueFamilyInfo->computeFamilyIndex,
            outQueueFamilyInfo->transferFamilyIndex,
            properties->deviceName);

    // Check if device meets requirements
    if (
            (!requirements->graphics || outQueueFamilyInfo->graphicsFamilyIndex != -1) &&
            (!requirements->present  || outQueueFamilyInfo->presentFamilyIndex  != -1) &&
            (!requirements->compute  || outQueueFamilyInfo->computeFamilyIndex  != -1) &&
            (!requirements->transfer || outQueueFamilyInfo->transferFamilyIndex != -1)
       )
    {
        PXINFO("Device meets queue requirements.");

        // TODO: check if we need the outswapchain support later on, if so don't pass it the frame arena
        VulkanDeviceQuerySwapchainSupport(device, surface, permArena, outSwapchainSupport);

        if (outSwapchainSupport->formatsCount < 1 || outSwapchainSupport->presentModesCount < 1)
        {
            PXINFO("Required swapchain support not present. Skipping device.");
        }

        // Device extensions
        if (requirements->deviceExtensionNames)
        {
            u32 availableExtensionsCount = 0;
            VkExtensionProperties *availableExtensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionsCount, 0));
            
            if (availableExtensionsCount)
            {
                availableExtensions = (VkExtensionProperties *)
                    PXMemoryArenaAlloc(tempArena, sizeof(VkExtensionProperties) * availableExtensionsCount);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(device, 0, &availableExtensionsCount,
                            availableExtensions));
                for (u32 i = 0; i < requirements->deviceExtensionNamesCount; i++)
                {
                    b8 found = 0;
                    for (u32 j = 0; j < availableExtensionsCount; j++)
                    {
                        if (StringEquals(
                                requirements->deviceExtensionNames[i], availableExtensions[j].extensionName))
                        {
                            found = 1;
                            break;
                        }
                    } // !for (u32 j = 0; j < availableExtensionsCount; j++)

                    if (!found)
                    {
                        PXINFO("Required extension not found: '%s'. Skipping device.", 
                                requirements->deviceExtensionNames[i]);
                        return 0;
                    }

                } // !for (u32 i = 0; i < requirements->deviceExtensionNamesCount; i++)
            }
        } // ! if (requirements->deviceExtensionNames)

        if (requirements->samplerAnisotropy && !features->samplerAnisotropy)
        {
            PXINFO("Device does not support samplerAnisotropy. Skipping.");
            return 0;
        }

        // All requirements have been met
        return 1;

    } // if device meets requirements

    return 0;
}

PXAPI b8
SelectPhysicalDevice(vulkan_context *context, mem_arena *permArena, mem_arena *tempArena)
{
    u32 physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, 0));

    if (physicalDeviceCount == 0)
    {
        PXFATAL("No devices that support Vulkan were found.");
        return 0;
    }

    /* VkPhysicalDevice *physicalDevices = (VkPhysicalDevice *) */
    /*     PXMemoryArenaAlloc(tempArena, sizeof(VkPhysicalDevice) * physicalDeviceCount); */
    VkPhysicalDevice physicalDevices[32];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physicalDeviceCount, physicalDevices));

    for (u32 i = 0; i < physicalDeviceCount; i++)
    {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevices[i], &memoryProperties);

        vulkan_physical_device_requirements requirements = {};
        requirements.graphics           = 1;
        requirements.present            = 1;
        requirements.compute            = 0;
        requirements.transfer           = 1;
        requirements.samplerAnisotropy  = 1;
        requirements.discreteGPU        = 1;

        char *deviceExtensionNames[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };

        requirements.deviceExtensionNames = deviceExtensionNames;
        requirements.deviceExtensionNamesCount = ArrayLen(deviceExtensionNames);

        vulkan_physical_device_queue_family_info queueInfo = {};

        b8 result = PhysicalDeviceMeetsRequirements(
            physicalDevices[i],
            context->surface,
            &properties,
            &features,
            &requirements,
            permArena,
            tempArena,
            &queueInfo,
            &context->device.swapchainSupportInfo);

        if (result)
        {
            PXINFO("Selected GPU: '%s'.", properties.deviceName);

            switch(properties.deviceType)
            {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    PXINFO("GPU type is unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    PXINFO("GPU type is discrete (dedicated GPU).");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    PXINFO("GPU type is integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    PXINFO("GPU type is CPU.");
                    break;
            }

            for (u32 memoryHeapIndex = 0;
                 memoryHeapIndex < memoryProperties.memoryHeapCount;
                 memoryHeapIndex++)
            {
                f32 memorySizeGiB = (((f32)memoryProperties.memoryHeaps[memoryHeapIndex].size) / 1024.f / 1024.f / 1024.f);
                if (memoryProperties.memoryHeaps[memoryHeapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                {
                    PXINFO("Local GPU memory: %.2f GiB", memorySizeGiB);
                }
                else
                {
                    PXINFO("Shared system memory: %.2f GiB", memorySizeGiB);
                }
            }

            context->device.physicalDevice = physicalDevices[i];

            context->device.graphicsQueueIndex = queueInfo.graphicsFamilyIndex;
            context->device.presentQueueIndex  = queueInfo.presentFamilyIndex;
            context->device.transferQueueIndex = queueInfo.transferFamilyIndex;
            
            context->device.properties = properties;
            context->device.features = features;
            context->device.memoryProperties = memoryProperties;

            break;
        } // !if (result)
    }

    if (!context->device.physicalDevice)
    {
        PXERROR("No physical devices were found which met the requirements.");
        return 0;
    }

    PXINFO("Physical device selected!");
    return 1;
}

PXAPI b8
VulkanDeviceDetectDepthFormat(vulkan_device *device)
{
    VkFormat desiredFormats[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    u32 flags = VK_FORMAT_FEATURE_2_DEPTH_STENCIL_ATTACHMENT_BIT;

    for (u32 formatIndex = 0; formatIndex < ArrayLen(desiredFormats); formatIndex++)
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(
            device->physicalDevice,
            desiredFormats[formatIndex],
            &properties);

        if ((properties.linearTilingFeatures & flags) == flags)
        {
            device->depthFormat = desiredFormats[formatIndex];
            return 1;
        }

        if ((properties.optimalTilingFeatures & flags) == flags)
        {
            device->depthFormat = desiredFormats[formatIndex];
            return 1;
        }
    }

    return 0;
}
