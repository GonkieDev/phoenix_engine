#include <core/engine.hpp>

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
