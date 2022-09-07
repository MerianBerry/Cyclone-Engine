# Cyclone Documentation

This file will include the entirety of the documentation for the engine because I don't want to make a website for this.

## *Contents*
1. [Lua](#lua-integrationextending)
    1. [Runtime manipulation](#runtime-manipulation)
    2. [Window inputs](#window-inputs)
    3. [Window manipulation](#window-manipulation)
    4. [Scene management (upcoming)](#scene-management)
    5. [Mesh handling/accessing (upcoming)](#mesh-handlingaccessing)
    

## Lua integration/extending
You can look at the engines lua file at [Cyclone.lua](../Cyclone.lua).


### Runtime manipulation
```lua
Exit( code -> int, message -> string ) -> nothing
```
``code`` is just an integer that you want to output, it can be any number. Gets printed to console as an exit message. **NOTE:** code is converted to integer in the engine.

``message`` is just a string input that you want to print along with the exit message. **NOTE:** message input *CAN* be left empty.

<br/><br/>

### Window Inputs
```lua
GetKeyState( key -> string, mode -> string ) -> bool
```
``key`` is a string input from the keys table, though the table value are actually numbers. but thats for how the engine is structured

``mode`` is a string input thats either "hold" or "pulse" PULSE BY DEFAULT. Setting it as hold, it will return true *AS LONG AS* the key is pressed. Pulse is the first tick its pressed.

This returns true if the `key` requested was pressed during the *last frame*.

###
```lua
GetButtonState( btn -> string, mode -> string ) -> bool
```
``btn`` is a string input from the buttons table, it acks the same as the keys table.

``mode`` is the exact same as the ``GetKeyState`` function. **NOTE:** When attempting to get the mouse wheel boolean, it is strongly recommend to use mode = "hold".

This returns true if the `btn` requested was pressed during the *last frame*.

<br/><br/>

### Window manipulation
```lua
SetWindowSize( w -> number, h -> number ) -> nothing
```
``w`` is the desired width for the window to be set to. **NOTE:** Minimum window width in the engine is 960px, can be set lower. Its also converted to an integer in the engine.

``h`` acts the same as w, but it sets the desired height for the window to be set to. The minimum window height is 560px. It's also converted to an integer in the engine.

###

```lua
SetWindowPos( x -> number, y -> number ) -> nothing
```
``x`` is the desired x position *IN THE ENTIRE WINDOW SPACE* in pixels. Converted to an integer in the engine.

``y``, acting similar to x, is the desired y position. Also converted to an integer in the engine.

###

```lua
SetWindowState( mode -> string ) -> nothing
```
``mode`` is a string input that dictates what will happen to the window.

These are the accepted modes:
- `"min"`. Minimizes the window.
- `"max"`. Maximizes the window, this mode retains the window decorations (*borders*)
- `"restore"`. Restores the window from a minimized state to the previous size.

If the mode isn't valid, nothing will happen.

<br/><br/>

### Scene management

```lua
Scene.SetColor( self, r2 -> number, g2 -> number, b2 -> number, a2 -> number ) -> nothing
```
This is a method of the Scene table. Know that giving the `self` input or using `Scene:` will be required.

`r2, g2, b2, a2` are all very similar, as they are number inputs (typical range is 0 - 255 ). They are different in what they represent. They represent the new color of red, green, blue, alpha (respective) to set to the local `Scene` draw color.
This method automatically alpha blends the new color with the old color. **NOTE:** The defualt color of the Scene table is `r = 0, g = 0, b = 0, a = 255`.

###

```lua
Scene.DrawMesh( self, name -> string ) -> nothing
```
This is a method of the Scene table. Know that giving the `self` input or using `Scene:` will be required.

`name` is a string input of the name of the mesh that you want to draw. **NOTE:** You *MUST* use `LoadMesh` with the desired name *BEFORE* using `Scene.DrawMesh`.

###

```lua
Scene.DrawClear( self ) -> nothing
```
This is a method of the Scene table. Know that giving the `self` input or using `Scene:` will be required.

This method takes no inputs, though it uses Scenes `r, g, b` local values to draw to the scene. When called, it will set the background color of the screen to the current color of the `Scene` local color values. As it is setting the background to this color. It does not draw over other drawn items except other `Scene.DrawClear`s.

<br/><br/>

### Mesh handling/accessing

```lua
LoadMesh( path -> string, name -> string ) -> string
```
**HUGE NOTE:** This method uses a No Wait method of loading. So the mesh might not be immediately accessable right after the mothod call.

`path` is a string input representing the path to the mesh you want to load. Know that the path root is the directory that the Excecutable is in. **NOTE:** The only *currently* supported file format is *.obj.

`name` is a string input representing the name you want to give the mesh. This name will be used for handing/accessing the mesh inside the engine structure, so *ONLY **ONE*** mesh with that name can exist.

`return value` is the name of the mesh so you can store it into a variable. It is the same as the `name` input.

###

```lua
SetMeshState( name -> string, mode -> string ) -> nothing
```
`name` is a string input representing the name of the mesh that you want to modify.

`mode` is a string input representing the desired mode that you want the mesh to be in. Accepted inputs are listed:
- `"hide"`. Makes the mesh invisible.
- `"show"`. Makes the mesh visible. On by default.
- `"wire"`. Renders the mesh in wireframe mode.
- `"fill"`. Renders the mesh in fill mode. On my default.

If the `mode` input isnt one of these listed modes, it will do nothing.
