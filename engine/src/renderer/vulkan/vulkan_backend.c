#include "vulkan_backend.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_fence.h"
#include "vulkan_framebuffer.h"
#include "vulkan_platform.h"
#include "vulkan_renderpass.h"
#include "vulkan_swapchain.h"
#include "vulkan_types.inl"
#include "vulkan_utils.h"

#include "core/application.h"
#include "core/logger.h"
#include "core/rq_string.h"
#include "core/rq_memory.h"

#include "containers/darray.h"

// static Vulkan context
static vulkan_context context;
static u32 cached_framebuffer_width;
static u32 cached_framebuffer_height;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

i32 find_memory_index(u32 type_filter, u32 property_flags);

void create_command_buffers(renderer_backend* backend);
void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass);
b8 recreate_swapchain(renderer_backend* backend);

b8 vulkan_renderer_backend_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state) {

    // Func pointers
    context.find_memory_index = find_memory_index;

    //TODO: Custom allocator
    context.allocator = 0;

    application_get_framebuffer_size(&cached_framebuffer_width, &cached_framebuffer_height);
    context.framebuffer_width = (cached_framebuffer_width != 0) ? cached_framebuffer_width : 800;
    context.framebuffer_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : 600;
    cached_framebuffer_width = 0;
    cached_framebuffer_height = 0;

    // Setup vulkan instance
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = application_name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Requiem Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO}; // This just wipes it to zero. (VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO)
    create_info.pApplicationInfo = &app_info;

    // Obtain a list of required extensions
    const char** required_extensions = darray_create(const char*);
    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension.
    platform_get_required_extension_names(&required_extensions);      // Platform-specific extension(s)
#if defined(_DEBUG)
    darray_push(required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    RQ_DEBUG("Required Extensions");
    u32 length = darray_length(required_extensions);
    for (u32 i = 0; i < length; i++) {
        RQ_DEBUG(required_extensions[i]);
    }
#endif

    create_info.enabledExtensionCount = darray_length(required_extensions);
    create_info.ppEnabledExtensionNames = required_extensions;
    
    const char** required_validation_layer_names = 0;
    u32 required_validation_layer_count = 0;


#if defined(_DEBUG) 
    RQ_INFO("Validation layers enabled. Enumerating...");

    // The list of validation layers required.
    required_validation_layer_names = darray_create(const char*);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = darray_length(required_validation_layer_names);

    // Obtain a list of available validation layers.
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties* available_layers = darray_reserve(VkLayerProperties, available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

    // Verify all required layers are available
    for (u32 i = 0; i < required_validation_layer_count; ++i) {
        RQ_INFO("Searching for layer: %s...", required_validation_layer_names[i]);
        b8 found = FALSE;
        for (u32 j = 0; j < available_layer_count; ++j) {
            if (strings_equal(required_validation_layer_names[i], available_layers[j].layerName)) {
                found = TRUE;
                RQ_INFO("Found the validation layer!");
                break;
            }
        }

        if (!found) {
            RQ_FATAL("Required validation layer is missing: %s", required_validation_layer_names[i]);
            return FALSE;
        }

        RQ_INFO("All validation layers are present.");
    }
#endif


    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;

    u32 ext_count = darray_length(required_extensions);
    RQ_DEBUG("Extension count: %d", ext_count);
    for (u32 i = 0; i < ext_count; i++) {
        RQ_DEBUG("Extension %d: %s", i, required_extensions[i]);
    }

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance));
    RQ_INFO("Vulkan instance created.");


    // Debugger
#if defined(_DEBUG)
    RQ_DEBUG("Creating Vulkan debugger...");
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;
    debug_create_info.pUserData = 0;

    PFN_vkCreateDebugUtilsMessengerEXT func = 
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    RQ_ASSERT_MSG(func, "Failed to create debug messenger!");
    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    RQ_DEBUG("Vulkan debugger created.");
#endif
    
    // Surface creation.
    RQ_DEBUG("Creating Vulkan surface...");
    if (!platform_create_vulkan_surface(plat_state, &context)) {
        RQ_ERROR("Failed to create Vulkan platform surface.");
        return FALSE;
    }
    RQ_DEBUG("Surface creation was successful.");

    // Device creation.
    if (!vulkan_device_create(&context)) {
        RQ_ERROR("Vulkan device failed to be created.");
        return FALSE;
    }

    // Swapchain creation.
    vulkan_swapchain_create(
        &context,
        context.framebuffer_width,
        context.framebuffer_height,
        &context.swapchain);

    vulkan_renderpass_create(
        &context,
        &context.main_renderpass,
        0, 0, context.framebuffer_width, context.framebuffer_height,
        0.0f, 0.0f, 0.2f, 1.0f,
        1.0f,
        0);

    context.swapchain.framebuffers = darray_reserve(vulkan_framebuffer, context.swapchain.image_count);
    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);

    // Create command buffers.
    create_command_buffers(backend);

    // Create sync objects

// Per-frame sync objects
context.image_available_semaphores =
    darray_reserve(VkSemaphore, context.swapchain.max_frames_in_flight);

context.in_flight_fences =
    darray_reserve(vulkan_fence, context.swapchain.max_frames_in_flight);

// Per-image sync objects
context.queue_complete_semaphores =
    darray_reserve(VkSemaphore, context.swapchain.image_count);

// Create per-frame sync objects
for (u32 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {

    VkSemaphoreCreateInfo semaphore_create_info = {
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    vkCreateSemaphore(
        context.device.logical_device,
        &semaphore_create_info,
        context.allocator,
        &context.image_available_semaphores[i]);

    vulkan_fence_create(
        &context,
        TRUE,
        &context.in_flight_fences[i]);
}

    // Create per-image render-finished semaphores
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {

        VkSemaphoreCreateInfo semaphore_create_info = {
            VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
        };

        vkCreateSemaphore(
            context.device.logical_device,
            &semaphore_create_info,
            context.allocator,
            &context.queue_complete_semaphores[i]);
    }

    // Track which fence owns each swapchain image.
    context.images_in_flight =
        darray_reserve(vulkan_fence*, context.swapchain.image_count);

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        context.images_in_flight[i] = 0;
    }
    RQ_INFO("Vulkan renderer initialized successfully.");
    return TRUE;
}

void vulkan_renderer_backend_shutdown(renderer_backend* backend) {
    vkDeviceWaitIdle(context.device.logical_device);

    // Sync objects
    for (u8 i = 0; i < context.swapchain.max_frames_in_flight; ++i) {
        if (context.image_available_semaphores[i]) {
            vkDestroySemaphore(context.device.logical_device,
                context.image_available_semaphores[i], context.allocator);
            context.image_available_semaphores[i] = 0;
        }
        vulkan_fence_destroy(&context, &context.in_flight_fences[i]);
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.queue_complete_semaphores[i]) {
            vkDestroySemaphore(context.device.logical_device,
                context.queue_complete_semaphores[i], context.allocator);
            context.queue_complete_semaphores[i] = 0;
        }
    }
    darray_destroy(context.image_available_semaphores);
    context.image_available_semaphores = 0;

    darray_destroy(context.queue_complete_semaphores);
    context.queue_complete_semaphores = 0;

    darray_destroy(context.in_flight_fences);
    context.in_flight_fences = 0;

    darray_destroy(context.images_in_flight);
    context.images_in_flight = 0;

    // Command buffers
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.graphics_command_buffers[i].handle) {
            vulkan_command_buffer_free(
                &context,
                context.device.graphics_command_pool,
                &context.graphics_command_buffers[i]);
            context.graphics_command_buffers[i].handle = 0;
        }
    }
    darray_destroy(context.graphics_command_buffers);
    context.graphics_command_buffers = 0;

    RQ_DEBUG("Destroying Vulkan framebuffers")    
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    darray_destroy(context.swapchain.framebuffers);
    context.swapchain.framebuffers = 0;

    RQ_DEBUG("Destroying Vulkan Renderpass...");
    vulkan_renderpass_destroy(&context, &context.main_renderpass);

    RQ_DEBUG("Destroying Vulkan swapchain...");
    vulkan_swapchain_destroy(&context, &context.swapchain);

    RQ_DEBUG("Destroying Vulkan device...");
    vulkan_device_destroy(&context);

    RQ_DEBUG("Destroying Vulkan surface...");
    if (context.surface) {
        vkDestroySurfaceKHR(context.instance, context.surface, context.allocator);
        context.surface = 0;
    }

    RQ_DEBUG("Destroying Vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = 
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }

    RQ_DEBUG("Destroying Vulkan instance...");
    vkDestroyInstance(context.instance, context.allocator);
}

void vulkan_renderer_backend_on_resize(renderer_backend* backend, u16 width, u16 height) {
    cached_framebuffer_width = width;
    cached_framebuffer_height = height;
    context.framebuffer_size_generation++;

    RQ_INFO("Vulkan renderer backend->resized: w/h/gen = %i/%i/%llu", width, height, context.framebuffer_size_generation);
}
b8 vulkan_renderer_backend_begin_frame(renderer_backend* backend, f32 delta_time) {



    vulkan_device* device = &context.device;

    // If the swapchain is currently being recreated, wait for the device
    // to become idle before continuing.
    if (context.recreating_swapchain) {
        VkResult result = vkDeviceWaitIdle(device->logical_device);
        if (!vulkan_result_is_success(result)) {
            RQ_ERROR(
                "vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed: '%s'",
                vulkan_result_string(result, TRUE));
            return FALSE;
        }

        RQ_INFO("Recreating swapchain. booting");
        return FALSE;
    }

    // Handle framebuffer resize.
    if (context.framebuffer_size_generation != context.framebuffer_size_last_generation) {
        VkResult result = vkDeviceWaitIdle(device->logical_device);
        if (!vulkan_result_is_success(result)) {
            RQ_ERROR(
                "vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '%s'",
                vulkan_result_string(result, TRUE));
            return FALSE;
        }

        if (!recreate_swapchain(backend)) {
            return FALSE;
        }

        RQ_INFO("Resized, booting");
        return FALSE;
    }

    // Wait for the current frame fence before using it again.
    if (!vulkan_fence_wait(
            &context,
            &context.in_flight_fences[context.current_frame],
            INVALID_ID)) {
        RQ_WARN("In-flight fence wait failure!");
        return FALSE;
    }

    // Acquire the next image from the swapchain.
    if (!vulkan_swapchain_acquire_next_image_index(
            &context,
            &context.swapchain,
            INVALID_ID,
            context.image_available_semaphores[context.current_frame],
            0,
            &context.image_index)) {
        return FALSE;
    }

    // If the image is already in flight, wait for it.
    if (context.images_in_flight[context.image_index] != 0) {
        vulkan_fence_wait(
            &context,
            context.images_in_flight[context.image_index],
            INVALID_ID);
    }

    // Mark the image as now being used by this frame.
    context.images_in_flight[context.image_index] =
        &context.in_flight_fences[context.current_frame];

    // Reset the fence for use during this frame submission.
    vulkan_fence_reset(
        &context,
        &context.in_flight_fences[context.current_frame]);

    // Obtain the command buffer for this image.
    vulkan_command_buffer* command_buffer =
        &context.graphics_command_buffers[context.image_index];

    // Reset and begin recording.
    vulkan_command_buffer_reset(command_buffer);

    vulkan_command_buffer_begin(
        command_buffer,
        FALSE,
        FALSE,
        FALSE);

    // Configure viewport.
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = (f32)context.framebuffer_height;
    viewport.width = (f32)context.framebuffer_width;
    viewport.height = -(f32)context.framebuffer_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(
        command_buffer->handle,
        0,
        1,
        &viewport);

    // Configure scissor.
    VkRect2D scissor;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = context.framebuffer_width;
    scissor.extent.height = context.framebuffer_height;

    vkCmdSetScissor(
        command_buffer->handle,
        0,
        1,
        &scissor);

    // Update renderpass dimensions.
    context.main_renderpass.w = context.framebuffer_width;
    context.main_renderpass.h = context.framebuffer_height;

    // Begin the renderpass.
    vulkan_renderpass_begin(
        command_buffer,
        &context.main_renderpass,
        context.swapchain.framebuffers[context.image_index].handle);

    return TRUE;
}

b8 vulkan_renderer_backend_end_frame(renderer_backend* backend, f32 delta_time) {

    vulkan_command_buffer* command_buffer =
        &context.graphics_command_buffers[context.image_index];

    // End the active renderpass.
    vulkan_renderpass_end(
        command_buffer,
        &context.main_renderpass);

    // Finish recording commands.
    vulkan_command_buffer_end(command_buffer);

    // Submit info.
    VkSubmitInfo submit_info = {VK_STRUCTURE_TYPE_SUBMIT_INFO};

    // Command buffers to submit.
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer->handle;

    // Wait semaphore (image acquired from swapchain).
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores =
        &context.image_available_semaphores[context.current_frame];

    // Wait stages.
    VkPipelineStageFlags wait_stages[1] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    submit_info.pWaitDstStageMask = wait_stages;

    // Signal semaphore (render complete).
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores =
        &context.queue_complete_semaphores[context.image_index];

    // Submit to graphics queue.
    VkResult result = vkQueueSubmit(
        context.device.graphics_queue,
        1,
        &submit_info,
        context.in_flight_fences[context.current_frame].handle);

    if (result != VK_SUCCESS) {
        RQ_ERROR(
            "vkQueueSubmit failed with result: %s",
            vulkan_result_string(result, TRUE));
        return FALSE;
    }

    // Mark command buffer as submitted.
    vulkan_command_buffer_update_submitted(command_buffer);

    // Present the image to the swapchain.
        // Present the image to the swapchain.
    vulkan_swapchain_present(
        &context,
        &context.swapchain,
        context.device.graphics_queue,
        context.device.present_queue,
        context.queue_complete_semaphores[context.image_index],
        context.image_index);

    // Advance the current frame index.
    context.current_frame =
        (context.current_frame + 1) %
        context.swapchain.max_frames_in_flight;

    return TRUE;
}


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    switch(message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            RQ_ERROR(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            RQ_WARN(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            RQ_INFO(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            RQ_TRACE(callback_data->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
            RQ_INFO("Max enums");
            break;
    }
    return VK_FALSE;
}

i32 find_memory_index(u32 type_filter, u32 property_flags) {
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(context.device.physical_device, &memory_properties);

    for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
        // Check each memory type to see if its bit is set to 1.
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
            return i;
        }
    }

    RQ_WARN("Unable to find suitable memory type");
    return -1;
}

void create_command_buffers(renderer_backend* backend) {
    if (!context.graphics_command_buffers) {
        context.graphics_command_buffers = darray_reserve(vulkan_command_buffer, context.swapchain.image_count);
        for (u32 i = 0; i < context.swapchain.image_count; ++i) {
            rq_zero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        }
    }

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        if (context.graphics_command_buffers[i].handle) {
            vulkan_command_buffer_free(
                &context,
                context.device.graphics_command_pool,
                &context.graphics_command_buffers[i]);
        }
        rq_zero_memory(&context.graphics_command_buffers[i], sizeof(vulkan_command_buffer));
        vulkan_command_buffer_allocate(
            &context,
            context.device.graphics_command_pool,
            TRUE,
            &context.graphics_command_buffers[i]);
    }
}

void regenerate_framebuffers(renderer_backend* backend, vulkan_swapchain* swapchain, vulkan_renderpass* renderpass) {
    for (u32 i = 0; i < swapchain->image_count; ++i) {
        u32 attachment_count = 2;
        VkImageView attachments[] = {
            swapchain->views[i],
            swapchain->depth_attachment.view};

        vulkan_framebuffer_create(
            &context,
            renderpass,
            context.framebuffer_width,
            context.framebuffer_height,
            attachment_count,
            attachments,
            &context.swapchain.framebuffers[i]);
    }
}
b8 recreate_swapchain(renderer_backend* backend) {
    if (context.recreating_swapchain) {
        RQ_DEBUG("recreate_swapchain called when already recreating. Booting");
        return FALSE;
    }

    // Use cached size if available, otherwise keep current size.
    u32 new_width  = (cached_framebuffer_width  != 0) ? cached_framebuffer_width  : context.framebuffer_width;
    u32 new_height = (cached_framebuffer_height != 0) ? cached_framebuffer_height : context.framebuffer_height;

    if (new_width == 0 || new_height == 0) {
        RQ_DEBUG("recreate_swapchain called when window is < 1 in a dimension. Booting");
        return FALSE;
    }

    context.recreating_swapchain = TRUE;
    vkDeviceWaitIdle(context.device.logical_device);

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        context.images_in_flight[i] = 0;
    }

    vulkan_device_query_swapchain_support(
        context.device.physical_device,
        context.surface,
        &context.device.swapchain_support);
    vulkan_device_detect_depth_format(&context.device);

    vulkan_swapchain_recreate(&context, new_width, new_height, &context.swapchain);

    context.framebuffer_width  = new_width;
    context.framebuffer_height = new_height;
    cached_framebuffer_width   = 0;
    cached_framebuffer_height  = 0;

    context.framebuffer_size_last_generation = context.framebuffer_size_generation;

    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_command_buffer_free(&context, context.device.graphics_command_pool, &context.graphics_command_buffers[i]);
    }
    for (u32 i = 0; i < context.swapchain.image_count; ++i) {
        vulkan_framebuffer_destroy(&context, &context.swapchain.framebuffers[i]);
    }

    context.main_renderpass.x = 0;
    context.main_renderpass.y = 0;
    context.main_renderpass.w = new_width;
    context.main_renderpass.h = new_height;

    regenerate_framebuffers(backend, &context.swapchain, &context.main_renderpass);
    create_command_buffers(backend);

    context.recreating_swapchain = FALSE;
    return TRUE;
}