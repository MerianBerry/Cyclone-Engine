clang++ -std=c++17 -o Cyclone.exe -Ithings/glm -Ithings/VkBootstrap -IC:/VulkanSDK/1.3.211.0/Include/vulkan -IC:/VulkanSDK/1.3.211.0/Include/SDL2 -Ithings/VulkanMemoryAllocator-master/include -Ithings/tinyobjloader -LC:/VulkanSDK/1.3.211.0/Lib -lvulkan-1 -lSDL2 things/VkBootstrap/Vkbootstrap.cpp things/tinyobjloader/tiny_obj_loader.cc src/Cyclone-inits.cpp src/Cyclone.cpp src/Cyclone-mesh.cpp src/Cyclone-runtime.cpp
rmdir CycloneDistro /Q /S
mkdir CycloneDistro
mkdir CycloneDistro\\Lua
mkdir CycloneDistro\\scripts
mkdir CycloneDistro\\shaders
mkdir CycloneDistro\\cfg
xcopy Cyclone.exe CycloneDistro
xcopy SDL2.dll CycloneDistro /Q
xcopy lua54.dll CycloneDistro /Q
xcopy shaders CycloneDistro\\shaders /Q /H /E
xcopy cfg CycloneDistro\\cfg /Q /H /E
xcopy Lua CycloneDistro\\Lua /E /H /Q
rmdir CycloneDistro\\Lua\\include /Q /S
xcopy scripts CycloneDistro\\scripts /E /H /Q
xcopy Cyclone.lua CycloneDistro /Q
pause