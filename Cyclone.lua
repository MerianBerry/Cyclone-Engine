local t = {}

function t.LoadMesh( path, name )
    return cpp_loadmesh( path, name )
end

return t