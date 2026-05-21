#include "vulkan_device.h"

#include "core/logger.h"
#include "core/rq_string.h"
#include "core/rq_memory.h"

#include "containers/darray.h"

typedef struct vulkan_physical_device_requirements {
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    // Darray
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
} vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info {
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
} vulkan_physical_device_queue_family_info;

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_support_info* out_swapchain_support);

b8 select_physical_device(vulkan_context* context);

b8 vulkan_device_create(vulkan_context* context) {
    if (!select_physical_device(context)) {
        return FALSE;
    }


    RQ_INFO("Creating logical device...");
    //NOTE: Do not create additional queues for shared indices.
    b8 present_shares_graphics_queue = context->device.graphics_queue_index == context->device.present_queue_index;
    b8 transfer_shares_graphics_queue = context->device.graphics_queue_index == context->device.transfer_queue_index;
    u32 index_count = 1;
    if (!present_shares_graphics_queue) {
        index_count++;
    }
    if (!transfer_shares_graphics_queue) {
        index_count++;
    }
    u32 indices[index_count];
    u8 index = 0;
    indices[index++] = context->device.graphics_queue_index;
    if (!present_shares_graphics_queue) {
        indices[index++] = context->device.present_queue_index;
    }
    if (!transfer_shares_graphics_queue) {
        indices[index++] = context->device.transfer_queue_index;
    }

    VkDeviceQueueCreateInfo queue_create_infos[index_count];
    for (u32 i = 0; i < index_count; i++) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = indices[i];
        queue_create_infos[i].queueCount = 1;
        // TODO: For now, we will disable this as older graphics drivers dont support this.
        //if (indices[i] == context->device.graphics_queue_index) {
        //    queue_create_infos[i].queueCount = 2;
        //}
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = 0;
        static f32 queue_priority = 1.0f; // default
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    // Request device features.
    VkPhysicalDeviceFeatures device_features = {};
    device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    device_create_info.queueCreateInfoCount = index_count;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 1;
    const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
    device_create_info.ppEnabledExtensionNames = &extension_names;

    // Deprecated and now ignored, so pass nothing.
    device_create_info.enabledLayerCount = 0;
    device_create_info.ppEnabledLayerNames = 0;

    //--CREATE THE DEVICE------
    VK_CHECK(vkCreateDevice(
        context->device.physical_device,
        &device_create_info,
        context->allocator,
        &context->device.logical_device));
    RQ_INFO("Logical device created.");

    // Get queues
    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.graphics_queue_index,
        0,
        &context->device.graphics_queue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.present_queue_index,
        0,
        &context->device.present_queue);

    vkGetDeviceQueue(
        context->device.logical_device,
        context->device.transfer_queue_index,
        0,
        &context->device.transfer_queue);
    RQ_INFO("Queues obtained.");

    // Create command pool for graphics queue
    VkCommandPoolCreateInfo pool_create_info = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pool_create_info.queueFamilyIndex = context->device.graphics_queue_index;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    VK_CHECK(vkCreateCommandPool(
        context->device.logical_device,
        &pool_create_info,
        context->allocator,
        &context->device.graphics_command_pool));
    RQ_INFO("Graphics command pool created.");
 
    return TRUE;
}

void vulkan_device_destroy(vulkan_context* context) {

    context->device.graphics_queue = 0;
    context->device.present_queue = 0;
    context->device.transfer_queue = 0;
 
    RQ_DEBUG("Destroying command pools...")
    vkDestroyCommandPool(
        context->device.logical_device,
        context->device.graphics_command_pool,
        context->allocator);

    // Destroy the logical device
    RQ_DEBUG("Destroying logical device...");
    if (context->device.logical_device) {
        vkDestroyDevice(context->device.logical_device, context->allocator);
        context->device.logical_device = 0;
    }

    // Physical devices cannot be destroyed! We must release them...
    RQ_DEBUG("Releasing physical device resources...");
    context->device.physical_device = 0;

    if (context->device.swapchain_support.formats) {
        //TODO: change to rq_free
        rq_free(context->device.swapchain_support.formats,
        sizeof(VkSurfaceFormatKHR) * context->device.swapchain_support.format_count,
        MEMORY_TAG_RENDERER);
        context->device.swapchain_support.formats = 0;
        context->device.swapchain_support.format_count = 0;
    }

    if (context->device.swapchain_support.present_modes) {
        //TODO: change to rq_free
        rq_free(context->device.swapchain_support.present_modes,
        sizeof(VkPresentModeKHR) * context->device.swapchain_support.present_mode_count,
        MEMORY_TAG_RENDERER);
        context->device.swapchain_support.present_modes = 0;
        context->device.swapchain_support.present_mode_count = 0;
    }

    rq_zero_memory(
        &context->device.swapchain_support.capabilities,
        sizeof(context->device.swapchain_support.capabilities));

    context->device.graphics_queue_index = INVALID_ID;
    context->device.present_queue_index = INVALID_ID;
    context->device.transfer_queue_index = INVALID_ID;
}

b8 vulkan_device_detect_depth_format(vulkan_device* device) {
    // Format conditions
    const u64 candidate_count = 2;
    VkFormat candidates[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT};
    
    u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    for (u64 i = 0; i < candidate_count; ++i) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device->physical_device, candidates[i], &properties);

        if ((properties.linearTilingFeatures & flags) == flags) {
            device->depth_format = candidates[i];
            return TRUE;
        } else if ((properties.optimalTilingFeatures & flags) == flags) {
            device->depth_format = candidates[i];
            return TRUE;
        }
    }

    return FALSE;
}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_support_info) {

    // Surface capabilities
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physical_device,
        surface,
        &out_support_info->capabilities));
    
    // Surface formats
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
      physical_device,
      surface,
      &out_support_info->format_count,
      0)); 
    
    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) { // Almost always yes
            out_support_info->formats = rq_allocate(sizeof(VkSurfaceFormatKHR) * out_support_info->format_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));
    }

    // Present modes
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
      physical_device,
      surface,
      &out_support_info->present_mode_count,
      0)); 
    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) { // Almost always yes
            out_support_info->present_modes = rq_allocate(sizeof(VkPresentModeKHR) * out_support_info->present_mode_count, MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));
    }
}

b8 select_physical_device(vulkan_context* context) {

    RQ_DEBUG("select_physical_device called");
    RQ_DEBUG("instance handle: %p", (void*)context->instance);
    RQ_DEBUG("surface handle: %p", (void*)context->surface);
    
    u32 physical_device_count = 0;
     VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, 0));
    if (physical_device_count == 0) {
        RQ_FATAL("No devices which support Vulkan were found.");
        return FALSE;
    }

    VkPhysicalDevice* physical_devices =
    rq_allocate(sizeof(VkPhysicalDevice) * physical_device_count,
                MEMORY_TAG_RENDERER);
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices));
    for (u32 i = 0; i < physical_device_count; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        vulkan_physical_device_requirements requirements = {};
        requirements.graphics = TRUE;
        requirements.present = TRUE;
        requirements.transfer = TRUE;
        //NOTE: Enable this if compute will be required.
        //requirements.compute = TRUE;    
        requirements.sampler_anisotropy = TRUE;
        requirements.discrete_gpu = TRUE;
        requirements.device_extension_names = darray_create(const char*);
        darray_push(requirements.device_extension_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info = {};
        b8 result = physical_device_meets_requirements(
            physical_devices[i],
            context->surface,
            &properties,
            &features,
            &requirements,
            &queue_info,
            &context->device.swapchain_support);
        
        if (result) {
            RQ_INFO("Selected device: '%s'.", properties.deviceName);
            // GPU type, etc.
            switch (properties.deviceType) {
                default:
                case VK_PHYSICAL_DEVICE_TYPE_OTHER:
                    RQ_INFO("GPU type is Unknown.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    RQ_INFO("GPU type is Integrated.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    RQ_INFO("GPU type is Discrete.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    RQ_INFO("GPU type is Virtual.");
                    break;
                case VK_PHYSICAL_DEVICE_TYPE_CPU:
                    RQ_INFO("GPU type is CPU.");
                    break;
            }

            RQ_INFO(
            "GPU DRIVER VERSION: %d.%d.%d",
            VK_VERSION_MAJOR(properties.driverVersion),
            VK_VERSION_MINOR(properties.driverVersion),
            VK_VERSION_PATCH(properties.driverVersion));

            // Vulkan API version
            RQ_INFO(
                "VULKAN API VERSION: %d.%d.%d",
                VK_VERSION_MAJOR(properties.apiVersion),
                VK_VERSION_MINOR(properties.apiVersion),
                VK_VERSION_PATCH(properties.apiVersion));
            
            // Memory information
            for (u32 j = 0; j < memory.memoryHeapCount; ++j) {
                f32 memory_size_gib = (((f32)memory.memoryHeaps[j].size) / 1024.0f / 1024.0f / 1024.0f);
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    RQ_INFO("Local GPU memory: %.2f GiB", memory_size_gib);
                } else {
                    RQ_INFO("Shared system memory: %.2f GiB", memory_size_gib);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index = queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;
            
            // Keep a copy of properties, features and memory info for later use.
            context->device.properties = properties;
            context->device.features = features;
            context->device.memory = memory;
            break; // Holy fuck FINALLY.
        }
    }

    if (!context->device.physical_device) {
        RQ_FATAL("No physical devices were found which met the requirements.");
        return FALSE;
    }

    RQ_INFO("Physical device selected.");

    return TRUE;
}

b8 physical_device_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_info,
    vulkan_swapchain_support_info* out_swapchain_support) {
    
    // Evaluate device properties to determine if it meets the needs of our application.
    out_queue_info->graphics_family_index = INVALID_ID;
    out_queue_info->present_family_index = INVALID_ID;
    out_queue_info->compute_family_index = INVALID_ID;
    out_queue_info->transfer_family_index = INVALID_ID;
    
    // Discrete GPU?
    if (requirements->discrete_gpu) {
        if (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            RQ_INFO("Device is not a discrete GPU, and one is required. Skipping...");
            return FALSE;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);
    VkQueueFamilyProperties* queue_families =
    rq_allocate(sizeof(VkQueueFamilyProperties) * queue_family_count,
                MEMORY_TAG_RENDERER);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
    
    // Look at each queue and see what they support
    RQ_INFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255;
    for (u32 i = 0; i < queue_family_count; ++i) {
        u32 current_transfer_score = 0;
        ++current_transfer_score;

        // Graphics queue?
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_info->graphics_family_index = i;
            ++current_transfer_score;
        }

        // Compute queue?
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_info->compute_family_index = i;
            ++current_transfer_score;
        }

        // Transfer queue? 
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            if (current_transfer_score <= min_transfer_score) {
                min_transfer_score = current_transfer_score;
                out_queue_info->transfer_family_index = i;
            }
        }  

        // Present queue?
        VkBool32 supports_present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &supports_present)); 
        if (supports_present) {
            out_queue_info->present_family_index = i;
        }
    }

    // Print out some info about the device
    RQ_INFO("       %d |       %d |       %d |      %d | %s",
            out_queue_info->graphics_family_index != INVALID_ID,
            out_queue_info->present_family_index != INVALID_ID,
            out_queue_info->compute_family_index != INVALID_ID,
            out_queue_info->transfer_family_index != INVALID_ID,
            properties->deviceName);

    if (
        (!requirements->graphics || (requirements->graphics && out_queue_info->graphics_family_index != INVALID_ID)) && 
        (!requirements->present || (requirements->present && out_queue_info->present_family_index != INVALID_ID)) && 
        (!requirements->compute || (requirements->compute && out_queue_info->compute_family_index != INVALID_ID)) && 
        (!requirements->transfer || (requirements->transfer && out_queue_info->transfer_family_index != INVALID_ID))) {
            RQ_INFO("Device meets queue requirements!");
            RQ_TRACE("Graphics Family Index: %i", out_queue_info->graphics_family_index);
            RQ_TRACE("Present Family Index: %i", out_queue_info->present_family_index);
            RQ_TRACE("Compute Family Index: %i", out_queue_info->compute_family_index);
            RQ_TRACE("Transfer Family Index: %i", out_queue_info->transfer_family_index);
            
            // Query swapchain support
            vulkan_device_query_swapchain_support(
                device,
                surface,
                out_swapchain_support);

            if (out_swapchain_support->format_count < 1 || out_swapchain_support->present_mode_count < 1) {
                if (out_swapchain_support->formats) {
                    rq_free(out_swapchain_support->formats, sizeof(VkSurfaceFormatKHR) * out_swapchain_support->format_count, MEMORY_TAG_RENDERER);
                }
                if (out_swapchain_support->present_modes) {
                    rq_free(out_swapchain_support->present_modes, sizeof(VkPresentModeKHR) * out_swapchain_support->present_mode_count, MEMORY_TAG_RENDERER);
                }
                RQ_INFO("Required swapchain not present. Skipping device...");
                return FALSE;
            }

            // Device extensions
            if (requirements->device_extension_names) {
                u32 available_extension_count = 0;
                VkExtensionProperties* available_extensions = 0;
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                    device,
                    0,
                    &available_extension_count,
                    0));
                if (available_extension_count != 0) {
                    available_extensions = rq_allocate(sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                    VK_CHECK(vkEnumerateDeviceExtensionProperties(
                        device,
                        0,
                        &available_extension_count,
                        available_extensions));
                    
                    u32 required_extension_count = darray_length(requirements->device_extension_names);
                    for (u32 i = 0; i < required_extension_count; ++i) {
                        b8 found = FALSE;
                        for (u32 j = 0; j < available_extension_count; ++j) {
                            if (strings_equal(requirements->device_extension_names[i], available_extensions[j].extensionName)) {
                                found = TRUE;
                                break;
                            }
                        }
                        
                        if (!found) {
                            RQ_INFO("Required extension not found: '%s', skipping device...", requirements->device_extension_names[i]);
                            rq_free(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
                            return FALSE;
                        }
                    }
                }
                rq_free(available_extensions, sizeof(VkExtensionProperties) * available_extension_count, MEMORY_TAG_RENDERER);
            }

            // Sampler anisotrpoy
            if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
                RQ_INFO("Device does not support samplerAnisotropy. Skipping...");
                return FALSE;
            }

            // Device meets all requirements! Yay!!!
            return TRUE;
        }
    return FALSE;
}