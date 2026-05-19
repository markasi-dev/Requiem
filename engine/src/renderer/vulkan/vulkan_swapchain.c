#include "vulkan_swapchain.h"

#include "core/logger.h"
#include "core/rq_memory.h"
#include "vulkan_device.h"

void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain);
void destroy(vulkan_context* context, vulkan_swapchain* swapchain);

void vulkan_swapchain_create(
    vulkan_context* context,
    u32 width, 
    u32 height,
    vulkan_swapchain* swapchain) {
    // Now we dont have to itterate over and over! Yay!!! :D
    create(context, width, height, swapchain);
}  

void vulkan_swapchain_recreate(
    vulkan_context* context,
    u32 width, 
    u32 height,
    vulkan_swapchain* swapchain) {
    // Now we dont have to itterate over and over! Yay!!! :D
    destroy(context, swapchain);
    create(context, width, height, swapchain);
}  

void vulkan_swapchain_destroy(
    vulkan_context* context,
    vulkan_swapchain* swapchain) {
    destroy(context, swapchain);
}

b8 vulkan_swapchain_acquire_next_image_index(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_index) {

    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device, 
        swapchain->handle, 
        timeout_ns, 
        image_available_semaphore, 
        fence, 
        out_image_index);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // Trigger swapchain recreation, then boot out of the render loop.
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
        return FALSE;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        RQ_FATAL("Failed to acquire swapchain image!");
        return FALSE;
    }

    return TRUE;
}

void vulkan_swapchain_present(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_image_index) {

    // Return the image to the swapchain for presentation.
    VkPresentInfoKHR present_info = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &render_complete_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchain->handle;
    present_info.pImageIndices = &present_image_index;
    present_info.pResults = 0;

    VkResult result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        vulkan_swapchain_recreate(context, context->framebuffer_width, context->framebuffer_height, swapchain);
    } else if (result != VK_SUCCESS) {
        RQ_FATAL("Failed to present swapchain images! Shutting down...");
    }
}

//-------CREATE-----------------------------------------------------------------------------
void create(vulkan_context* context, u32 width, u32 height, vulkan_swapchain* swapchain) {
    VkExtent2D swapchain_extent = {width, height}; // Kind of like a rectangle.
    swapchain->max_frames_in_flight = 2;

    // Choose a swap surface format.
    b8 found = FALSE;
    for (u32 i = 0; i < context->device.swapchain_support.format_count; ++i) {
        VkSurfaceFormatKHR format = context->device.swapchain_support.formats[i];
        // Preferred formats.
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain->image_format = format;
            found = TRUE;
            break;        
        }
    }

    if (!found) {
        RQ_WARN("SWAPCHAIN IMAGE FORMAT NOT FOUND. PICKING THE FIRST ONE AND HOPING FOR THE BEST...");
        swapchain->image_format = context->device.swapchain_support.formats[0];
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // Standard Vsync (I personally pronounce it FEEEEEFOOOOHHHHH)
    for (u32 i = 0; i < context->device.swapchain_support.present_mode_count; ++i) {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) { // WOAH YOU HAVE A COOLER Vsync? CAN I HAVE IT???!
            present_mode = mode;
            break;
        }
    }

    // Requery swapchain support.
    vulkan_device_query_swapchain_support(
        context->device.physical_device,
        context->surface,
        &context->device.swapchain_support);

    // Swapchain extent
    if (context->device.swapchain_support.capabilities.currentExtent.width != INVALID_ID) {
        swapchain_extent = context->device.swapchain_support.capabilities.currentExtent;
    }  

    // Clamp it to the value allowed by the GPU.
    VkExtent2D min = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_support.capabilities.maxImageExtent;
    swapchain_extent.width = RQ_CLAMP(swapchain_extent.width, min.width, max.width);
    swapchain_extent.height = RQ_CLAMP(swapchain_extent.height, min.height, max.height);
}
//-------CREATE END-----------------------------------------------------------------------------

void destroy(vulkan_context* context, vulkan_swapchain* swapchain) {

}