clang++ -std=c++17 -o LunarGE_t.exe -Ithings/glm -Ithings/VkBootstrap -IC:/VulkanSDK/1.3.211.0/Include/vulkan -IC:/VulkanSDK/1.3.211.0/Include/SDL2 -LC:/VulkanSDK/1.3.211.0/Lib -lvulkan-1 -lSDL2 things/VkBootstrap/Vkbootstrap.cpp src/Lunar-inits.cpp src/Lunarge.cpp src/Lunar-runtime.cpp
pause