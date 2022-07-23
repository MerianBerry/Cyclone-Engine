# What is Lunar?
LunarGE (Game engine / Graphics Engine) is an in-development light-ish weight engine written in c++, and uses the Vulkan graphics api. *Planned* supported scripting languages are c++, lua, and python.

***Contents***

[Structure](#structure)

[Code Examples](#code-examples)

[Libraries and API's](#libraries-and-apis)

## Structure

*The structure of LunarGE has possibly reached its final form, and it goes like this:*

**Lunar Instance**
- Vulkan instance
- Vulkan Device
- Vulkan Physical device
- Vulkan Debug messanger
- Vulkan Command pool
- And other global contexts

**-> Lunar Scene**
- Scenes represent physical window contexts
- These run independently from each other
- These Operate their own Event polling and Scene scripts
- Scenes will also have their own command queues

**--> Scene scripts**
- These operate in the context of their parent scene, and can be accessed by anything lower than them, this also includes shader scripts
- This can be considered the runtime scripts, that operate the child Scene objects/Scene object scripts

**---> Scene object**
- These are anything that gets represented via the parent scene, be it a UI element, pictures, 3D model, or even audio

**----> Scene object scripts**
- These do not parent the object, but are parented by the object they belong to
- These scripts can access parent scene scripts, the parent scene, and the instance


## Code Examples
```c++
    std::cout << "your mother" << std::endl;

```

## Libraries and API's
- [Vulkan 1.3](https://www.vulkan.org/)
- [SDL2](https://www.libsdl.org/)
- [VMA *Vulkan Memory Allocator*](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [VkBootstrap](https://github.com/charles-lunarg/vk-bootstrap)
- [glm](https://github.com/g-truc/glm)