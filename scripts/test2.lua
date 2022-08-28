cyc = require "Cyclone"

function Update()
    if cyc.GetKeyState( "O", "hold" ) and cyc.GetKeyState( "LCTRL", "hold" ) then
        print "Open up"
    end
end