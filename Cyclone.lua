local t = { }
t.keys = { }

t.keys[ "A" ] = 0
t.keys[ "B" ] = 1
t.keys[ "C" ] = 2
t.keys[ "D" ] = 3
t.keys[ "E" ] = 4
t.keys[ "F" ] = 5
t.keys[ "G" ] = 6
t.keys[ "H" ] = 7
t.keys[ "I" ] = 8
t.keys[ "J" ] = 9
t.keys[ "K" ] = 10
t.keys[ "L" ] = 11
t.keys[ "M" ] = 12
t.keys[ "N" ] = 13
t.keys[ "O" ] = 14
t.keys[ "P" ] = 15
t.keys[ "Q" ] = 16
t.keys[ "R" ] = 17
t.keys[ "S" ] = 18
t.keys[ "T" ] = 19
t.keys[ "U" ] = 20
t.keys[ "V" ] = 21
t.keys[ "W" ] = 22
t.keys[ "X" ] = 23
t.keys[ "Y" ] = 24
t.keys[ "Z" ] = 25
t.keys[ "1" ] = 26
t.keys[ "2" ] = 27
t.keys[ "3" ] = 28
t.keys[ "4" ] = 29
t.keys[ "5" ] = 30
t.keys[ "6" ] = 31
t.keys[ "7" ] = 32
t.keys[ "8" ] = 33
t.keys[ "9" ] = 34
t.keys[ "SPACE" ] = 35
t.keys[ "ESC" ] = 36
t.keys[ "LALT" ] = 37
t.keys[ "LCTRL" ] = 38
t.keys[ "LSHIFT" ] = 39
t.keys[ "CAPSLOCK" ] = 40
t.keys[ "TAB" ] = 41
t.keys[ "BACKQUOTE" ] = 42
t.keys[ "COMMA" ] = 43
t.keys[ "PERIOD" ] = 44
t.keys[ "RALT" ] = 45
t.keys[ "RCTRL" ] = 46
t.keys[ "RSHIFT" ] = 47
t.keys[ "SLASH" ] = 48
t.keys[ "SEMICOLON" ] = 49
t.keys[ "QUOTE" ] = 50
t.keys[ "ENTER" ] = 51
t.keys[ "LBRACKET" ] = 52
t.keys[ "RBRACKET" ] = 53
t.keys[ "BACKSLASH" ] = 54
t.keys[ "BACKSPACE" ] = 55
t.keys[ "EQUALS" ] = 56
t.keys[ "DASH" ] = 57
t.keys[ "F1" ] = 58
t.keys[ "F2" ] = 59
t.keys[ "F3" ] = 60
t.keys[ "F4" ] = 61
t.keys[ "F5" ] = 62
t.keys[ "F6" ] = 63
t.keys[ "F7" ] = 64
t.keys[ "F8" ] = 65
t.keys[ "F9" ] = 66
t.keys[ "F10" ] = 67
t.keys[ "F11" ] = 68
t.keys[ "F12" ] = 69
t.keys[ "LEFT" ] = 70
t.keys[ "RIGHT" ] = 71
t.keys[ "UP" ] = 72
t.keys[ "DOWN" ] = 73
t.keys[ "INSERT" ] = 74
t.keys[ "HOME" ] = 75
t.keys[ "DELETE" ] = 76
t.keys[ "END" ] = 77
t.keys[ "PAGEUP" ] = 78
t.keys[ "PAGEDOWN" ] = 79
t.keys[ "F13" ] = 80
t.keys[ "F14" ] = 81
t.keys[ "F15" ] = 82

t.buttons = {}
t.buttons[ "M_LEFT" ] = 0
t.buttons[ "M_RIGHT" ] = 1
t.buttons[ "M_MIDDLE" ] = 2
t.buttons[ "M_WHEELUP" ] = 3
t.buttons[ "M_WHEELDOWN" ] = 4

function t.LoadMesh( path, name )
    return cpp_loadmesh( path, name )
end

t.Swapchain = { data = 0, create = function( self, int )
    self.data = int
    print ( self.data )
end }

t.Scene = { data = { meshes = {}, r = 0, g = 0, b = 0, a = 0},
Submit = function( self )

end,
DrawMesh = function( self, mesh )
    self.meshes[ #self.meshes+1 ] = mesh
end,
DrawClear = function( self, r, g, b, a )
    a = a or 255
    if type( r ) ~= "number" or type( g ) ~= "number" or type( b ) ~= "number" or type( a ) ~= "number" then
        print "ERROR: DrawClear->a param isn't a number. all params MUST ALWAYS be a number"
    end

    self.r = self.r * ( 1 - a  / 255 ) + r * ( a / 255 )
    self.g = self.g * ( 1 - a  / 255 ) + g * ( a / 255 )
    self.b = self.b * ( 1 - a  / 255 ) + b * ( a / 255 )
    self.a = self.a * ( 1 - a  / 255 ) + a * ( a / 255 )
end }

-- Gets the boolean value of the input key. Keys: Use the keys table. Modes: "pulse" and "hold", "pulse" by default
function t.GetKeyState( key, mode )
    mode = mode or "pulse"
    if type( key ) ~= "string" then
        print "ERROR: GetKeyState->key isn't a string. param key MUST ALWAYS be a string"
    end
    return cpp_GetKeyState( t.keys[ key ], mode )
end

function t.GetButtonState( btn, mode )
    mode = mode or "pulse"
    if type( btn ) ~= "string" then
        print "ERROR: GetMousebtnState->btn isn't a string. param btn MUST ALWAYS be a string"
    end
    return cpp_GetBtnState( t.buttons[ btn ], mode )
end

function t.CreatePipeline( dimtype, type, Vertshader, Fragshader )

end

function t.CreateBasicPipeline( dimtype )
    return t.CreatePipeline( dimtype, "graphics", "basicVert2d", "basicFrag2d" )
end

function t.DrawClear( r, g, b, a )
    a = a or 255
    if type( r ) ~= "number" or type( g ) ~= "number" or type( b ) ~= "number" or type( a ) ~= "number" then
        print "ERROR: DrawClear->a param isn't a number. all params MUST ALWAYS be a number"
    end
    cpp_DrawClear( r, g, b )
end

function t.Exit( code, message )
    message = message or ""
    if type( code ) ~= "number" then
        print "WARN: Exit->code isn't a number. param code MUST ALWAYS be a number"
        code = 0
    end
    if type( message ) ~= "string" then
        print "WARN: Exit->message ins't a string. Param 'message' MUST ALWAYS be a string"
        message = ""
    end
    cpp_Exit( code, message )
end

function t.SetWindowSize( w, h )
    cpp_SetWindowSize( w, h )
end

function t.SetWindowPos( x, y )
    cpp_SetWindowPos( x, y )
end

function t.SetWindowState( mode )
    cpp_SetWindowState( mode )
end

return t