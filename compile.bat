clang++ -std=c++17 -o Cyclone.exe -Ithings/glm -Ithings/VkBootstrap -IC:/VulkanSDK/1.3.211.0/Include/vulkan -IC:/VulkanSDK/1.3.211.0/Include/SDL2 -Ithings/VulkanMemoryAllocator-master/include -Ithings/tinyobjloader -LC:/VulkanSDK/1.3.211.0/Lib -lvulkan-1 -lSDL2 things/VkBootstrap/Vkbootstrap.cpp things/tinyobjloader/tiny_obj_loader.cc src/Cyclone-inits.cpp src/Cyclone.cpp src/Cyclone-mesh.cpp src/Cyclone-runtime.cpp
pause