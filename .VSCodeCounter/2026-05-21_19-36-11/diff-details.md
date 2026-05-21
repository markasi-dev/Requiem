# Diff Details

Date : 2026-05-21 19:36:11

Directory c:\\Dev\\Engine\\Requiem\\code\\engine\\src\\renderer

Total : 40 files,  -1090 codes, -161 comments, -271 blanks, all -1522 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [engine/build.bat](/engine/build.bat) | Batch | -14 | -4 | -4 | -22 |
| [engine/build.sh](/engine/build.sh) | Shell Script | -9 | -2 | -1 | -12 |
| [engine/src/containers/darray.c](/engine/src/containers/darray.c) | C | -97 | -3 | -19 | -119 |
| [engine/src/containers/darray.h](/engine/src/containers/darray.h) | C++ | -48 | -7 | -21 | -76 |
| [engine/src/core/application.c](/engine/src/core/application.c) | C | -149 | -12 | -41 | -202 |
| [engine/src/core/application.h](/engine/src/core/application.h) | C++ | -12 | -6 | -9 | -27 |
| [engine/src/core/asserts.h](/engine/src/core/asserts.h) | C++ | -44 | -4 | -11 | -59 |
| [engine/src/core/clock.c](/engine/src/core/clock.c) | C | -14 | 0 | -4 | -18 |
| [engine/src/core/clock.h](/engine/src/core/clock.h) | C++ | -9 | -3 | -5 | -17 |
| [engine/src/core/event.c](/engine/src/core/event.c) | C | -100 | -14 | -26 | -140 |
| [engine/src/core/event.h](/engine/src/core/event.h) | C++ | -34 | -11 | -20 | -65 |
| [engine/src/core/input.c](/engine/src/core/input.c) | C | -157 | -19 | -33 | -209 |
| [engine/src/core/input.h](/engine/src/core/input.h) | C++ | -147 | -3 | -22 | -172 |
| [engine/src/core/logger.c](/engine/src/core/logger.c) | C | -33 | -7 | -14 | -54 |
| [engine/src/core/logger.h](/engine/src/core/logger.h) | C | -45 | -11 | -17 | -73 |
| [engine/src/core/rq\_memory.c](/engine/src/core/rq_memory.c) | C | -91 | -3 | -19 | -113 |
| [engine/src/core/rq\_memory.h](/engine/src/core/rq_memory.h) | C++ | -30 | -1 | -10 | -41 |
| [engine/src/core/rq\_string.c](/engine/src/core/rq_string.c) | C | -15 | 0 | -4 | -19 |
| [engine/src/core/rq\_string.h](/engine/src/core/rq_string.h) | C++ | -5 | 0 | -5 | -10 |
| [engine/src/defines.h](/engine/src/defines.h) | C++ | -75 | -11 | -14 | -100 |
| [engine/src/entry.h](/engine/src/entry.h) | C++ | -28 | -8 | -10 | -46 |
| [engine/src/game\_types.h](/engine/src/game_types.h) | C++ | -10 | -10 | -7 | -27 |
| [engine/src/platform/platform.h](/engine/src/platform/platform.h) | C++ | -23 | 0 | -8 | -31 |
| [engine/src/platform/platform\_linux\_x11.c](/engine/src/platform/platform_linux_x11.c) | C | -498 | -43 | -79 | -620 |
| [engine/src/platform/platform\_win32.c](/engine/src/platform/platform_win32.c) | C | -224 | -27 | -53 | -304 |
| [engine/src/renderer/vulkan/vulkan\_backend.c](/engine/src/renderer/vulkan/vulkan_backend.c) | C | 192 | 14 | 39 | 245 |
| [engine/src/renderer/vulkan/vulkan\_command\_buffer.c](/engine/src/renderer/vulkan/vulkan_command_buffer.c) | C | 82 | 3 | 19 | 104 |
| [engine/src/renderer/vulkan/vulkan\_command\_buffer.h](/engine/src/renderer/vulkan/vulkan_command_buffer.h) | C++ | 28 | 0 | 8 | 36 |
| [engine/src/renderer/vulkan/vulkan\_device.c](/engine/src/renderer/vulkan/vulkan_device.c) | C | 34 | 2 | 5 | 41 |
| [engine/src/renderer/vulkan/vulkan\_device.h](/engine/src/renderer/vulkan/vulkan_device.h) | C++ | 1 | 0 | 1 | 2 |
| [engine/src/renderer/vulkan/vulkan\_fence.c](/engine/src/renderer/vulkan/vulkan_fence.c) | C | 66 | 1 | 8 | 75 |
| [engine/src/renderer/vulkan/vulkan\_fence.h](/engine/src/renderer/vulkan/vulkan_fence.h) | C++ | 9 | 0 | 5 | 14 |
| [engine/src/renderer/vulkan/vulkan\_framebuffer.c](/engine/src/renderer/vulkan/vulkan_framebuffer.c) | C | 40 | 2 | 7 | 49 |
| [engine/src/renderer/vulkan/vulkan\_framebuffer.h](/engine/src/renderer/vulkan/vulkan_framebuffer.h) | C++ | 11 | 0 | 3 | 14 |
| [engine/src/renderer/vulkan/vulkan\_image.c](/engine/src/renderer/vulkan/vulkan_image.c) | C | 78 | 5 | 16 | 99 |
| [engine/src/renderer/vulkan/vulkan\_image.h](/engine/src/renderer/vulkan/vulkan_image.h) | C++ | 20 | 0 | 4 | 24 |
| [engine/src/renderer/vulkan/vulkan\_renderpass.c](/engine/src/renderer/vulkan/vulkan_renderpass.c) | C | 106 | 11 | 26 | 143 |
| [engine/src/renderer/vulkan/vulkan\_renderpass.h](/engine/src/renderer/vulkan/vulkan_renderpass.h) | C++ | 15 | 0 | 5 | 20 |
| [engine/src/renderer/vulkan/vulkan\_swapchain.c](/engine/src/renderer/vulkan/vulkan_swapchain.c) | C | 76 | 6 | 14 | 96 |
| [engine/src/renderer/vulkan/vulkan\_types.inl](/engine/src/renderer/vulkan/vulkan_types.inl) | C++ | 63 | 4 | 25 | 92 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details