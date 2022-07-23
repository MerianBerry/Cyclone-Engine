# whatam i doing

*The structure of LunarGE has possibly reached its final form, and it goes like this:*

**LunarGE Instance**
- Vulkan instance
- Device
- Physical device, and debug 
- And other global contexts

**-> Scenes**
- Scenes represent physical window contexts
- These run independently from each other
- These Operate their own Event polling and Scene scripts

**--> Scene scripts**
- These operate in the context of their parent scene, and can be accessed by anything lower than them, this also includes shader scripts
- This can be considered the runtime scripts, that operate the child Scene objects/Scene object scripts

**---> Scene object**
- These are anything that gets represented via the parent scene, be it a UI element, pictures, 3D model, or even audio

**----> Scene object scripts**
- These do not parent the object, but are parented by the object they belong to
- These scripts can access parent scene scripts, the parent scene, and the instance


```c++
    

```