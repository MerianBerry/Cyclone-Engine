# What is Lunar?
LunarGE (Game engine / Graphics Engine) is a heavy in-development light-ish weight engine written in c++, and uses the Vulkan graphics api. *Planned* supported scripting languages are Lua and python. But I am looking into supporting Java and C#.

Right now (8-22-2022) only Lua is 100% confirmed, python will be tested sometime soon. C++ was removed from the scripting language support due to troubles with structuring the engine and the requirement of clang, a C/C++ compiler. Also, this engine is being renamed to Cyclone engine/Cyclone game engine. The next push to this repo will namely have the renamed changes, Lua scripting and (hopefully) python scripting, although with very limited functions while all the new methods are being made.

### *NOTE: this readme is in progress, as things arent set in stone yet*

[Heres](docs/documentation.md) the documentation (not close to being worked on)

### *Contents*

1. [Structure](#structure)
2. [Code Examples](#code-examples)
3. [Libraries and API's](#libraries-and-apis)

## Structure

*(IN PROGRESS)* The structure of Cyclone has *not* reached its final form, I lied:

**Cyclone instance:**
This will include *everything* that is needed to run, the engine will handle pretty much everything, including but not limited to:
- Window
- Render pipeline
- Vulkan globals
- Vma globals
- Meshes/Ui's/Textures

**Global scripts:**
These will be what the user creates, the scripts will be called on a ``Update`` and ``Render`` function basis, where ``Update`` is logic processing, and ``Render`` is during the engines render pass. 

**Objects:**
Because the engine doesn't have an object handling system, and probably never will, if thats what the user/dev wants/needs, they will either need to make it or use other code.


## Code Examples
```lua
-- LUA

function Update()
    print( "I am updating!" )
end

function Render()
    print( "I am rendering!" )
end
```
This lua script, when used by the engine, will print "I am updating!" during logic updating, and "I am rendering!" during the renderpass.

## Libraries and API's
![Vulkan](docs/Vulkan_100px_Dec16.png)
- [Vulkan 1.3](https://www.vulkan.org/)
- [SDL2](https://www.libsdl.org/)
- [VMA *Vulkan Memory Allocator*](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [VkBootstrap](https://github.com/charles-lunarg/vk-bootstrap)
- [glm](https://github.com/g-truc/glm)
