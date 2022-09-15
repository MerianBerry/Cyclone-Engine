clang++ -std=c++17 -o Cyclone.exe -Ithings/glm -Ithings/VkBootstrap -IC:/VulkanSDK/1.3.211.0/Include -IC:/VulkanSDK/1.3.211.0/Include/vulkan -IC:/VulkanSDK/1.3.211.0/Include/SDL2 -Ithings/VulkanMemoryAllocator-master/include -Ithings/tinyobjloader -Ithings/imgui_docking -Ithings/implot-master -LC:/VulkanSDK/1.3.211.0/Lib -lvulkan-1 -lSDL2 things/imgui-docking/imgui_impl_vulkan.cpp things/imgui-docking/imgui_impl_sdl.cpp things/imgui-docking/imgui_tables.cpp things/imgui-docking/imgui_widgets.cpp things/imgui-docking/imgui_draw.cpp things/imgui-docking/imgui_demo.cpp things/imgui-docking/imgui.cpp things/implot-master/implot_items.cpp things/implot-master/implot.cpp things/VkBootstrap/Vkbootstrap.cpp things/tinyobjloader/tiny_obj_loader.cc src/Cyclone-inits.cpp src/Cyclone.cpp src/Cyclone-mesh.cpp src/Cyclone-runtime.cpp
rmdir CycloneDistro /Q /S
mkdir CycloneDistro
mkdir CycloneDistro\\Lua
mkdir CycloneDistro\\scripts
mkdir CycloneDistro\\shaders
mkdir CycloneDistro\\fonts
xcopy Cyclone.exe CycloneDistro
xcopy SDL2.dll CycloneDistro /Q
xcopy lua54.dll CycloneDistro /Q
xcopy shaders CycloneDistro\\shaders /Q /H /E
xcopy Lua CycloneDistro\\Lua /Q /H /E
xcopy fonts CycloneDistro\\fonts /Q /H /E
rmdir CycloneDistro\\Lua\\include /Q /S
xcopy scripts CycloneDistro\\scripts /Q /H /E
xcopy Cyclone.lua CycloneDistro /Q
xcopy imgui.ini CycloneDistro /Q
xcopy shaderload.txt CycloneDistro /Q
pause