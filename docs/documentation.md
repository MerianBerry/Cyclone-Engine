# Cyclone Documentation

This file will include the entirety of the documentation for the engine because I don't want to make a website for this.

## *Contents*
1. [Lua](#lua-integrationextending)
    1. [Runtime manipulation](#runtime-manipulation)
    2. [Window inputs](#window-inputs)
    3. [Window manipulation](#window-manipulation)
    

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
