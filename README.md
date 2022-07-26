# What is Cyclone?
Cyclone (Game engine / Graphics Engine) is a heavy in-development light-ish weight engine written in c++, and uses the Vulkan graphics api. *Planned* supported scripting languages are Lua and python. But Java and C# support is being looked into.


### *NOTE: Cyclone is currently going to be dropped while lower level abstractions are being made. The engine will still be C/C++, but all low level processes are going to be handed off to other services, which are being produced right now.*

[Heres](docs/documentation.md) the documentation

### *Contents*

1. [Structure](#structure)
2. [Code Examples](#code-examples)
    1. [Lua](#lua)
3. [Current focus/progress](#current-focusprogress)
4. [Libraries, Binaries and API's](#libraries-binaries-and-apis)

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

### Lua

```lua
function Update()
    print( "I am updating!" )
end

function Render()
    print( "I am rendering!" )
end
```
This lua script, when used by the engine, will print ``"I am updating!"`` during logic updating, and ``"I am rendering!"`` during the renderpass.

```lua
cyc = require "Cyclone"

function Update()
    if cyc.GetButtonState "M_LEFT" then
        print "You killed jim!"
    end
    if cyc.GetButtonState( "M_WHEELUP", "hold" ) or cyc.getButtonState( "M_WHEELDOWN", "hold" ) then
        print "And ill do it again"
    end
end
```
When used by the engine, this script will print ``"You killed jim!"`` when the left mouse button is pressed, and will print ``"And ill do it again"`` when the mouse wheel gets moved up OR down.


## Current focus/progress
Lua scripting is currently being created, there are still a LOT of functions to be extended into lua. Once that is done, python scripting will be pretty close up to being investigated.

As of the moment, the engine doesnt have enough functionality/structure to expand the language support. Lua is essentially the test subject for scripting. Along side that, the engines functionality is being expanded. Mesh loading and drawing is the current focus.


## Libraries, Binaries and API's
![Vulkan](docs/Vulkan_100px_Dec16.png)
- [Vulkan 1.3](https://www.vulkan.org/)
- [SDL2](https://www.libsdl.org/)
- [VMA *Vulkan Memory Allocator*](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [VkBootstrap](https://github.com/charles-lunarg/vk-bootstrap)
- [glm](https://github.com/g-truc/glm)
- [Lua binaries](http://luabinaries.sourceforge.net)
