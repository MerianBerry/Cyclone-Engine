#include "Cyclone.h"
#include <iostream>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "tiny_obj_loader.h"

#define MESH_VERBOSE

#define VK_CHECK(result)                           \
	do                                             \
	{                                              \
		VkResult err = result;                     \
		if (err)                                   \
		{                                          \
            cyc::WaitMS(5000);                     \
			abort();                               \
		}                                          \
	} while (0)

using namespace cyc;

Mesh LoadObj( const char* filename )
{
    Mesh mesh;
    //attrib will contain the vertex arrays of the file
    tinyobj::attrib_t attrib;
    //shapes contains the info for each separate object in the file
    vector< tinyobj::shape_t > shapes;
    //materials contains the information about the material of each shape, but we ownt use it
    vector< tinyobj::material_t > materials;

    //error and warning output from the load function
    string warn;
    string err;

    //load the OBJ file
    tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, filename, "assets/", false );
    //output the warning if there is one
    if( !warn.empty() )
    {
        cyc_log( "tinyobj WARN: %s\n", warn.c_str() )
    }
    //if we have any error, print it to the console, and break the mesh loading.
    //This happens if the file can't be found or is malformed
    if( !err.empty() )
    {
        cyc_log( "tinyobj ERR: %s\n", err.c_str() )
        return mesh;
    }

    // Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            //hardcode loading to triangles
			int fv = 3;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
            	tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];

                tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                //copy it into our vertex
				Vertex new_vert;
				new_vert.position.x = vx;
				new_vert.position.y = vy;
				new_vert.position.z = vz;

				new_vert.normal.x = nx;
				new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                //we are setting the vertex color as the vertex normal. This is just for display purposes
                new_vert.color = { nx, ny, nz };

                
				mesh.vertices.push_back(new_vert);
			}
			index_offset += fv;
		}
	}

    return mesh;
}

using std::stof;
#define meshVread( x ) { stof( x.substr(0, x.find_first_of('|')) ), \
    stof( x.substr(x.find_first_of('|')+1, x.find_last_of('|')) ),  \
    stof( x.substr(x.find_last_of('|')+1) ) }

cyc::Lresult< vector<cyc::Mesh> > cyc::LoadMeshes( vector< string > paths )
{
    vector< Mesh > res;
    for( Uint32 im = 0; im < paths.size(); ++im )
    {
        Mesh mesh;
        string ext = paths[ im ];
        ext = ext.substr( ext.find_last_of('.')+1 );
        string file = paths[ im ].substr( 0, paths[ im ].find_last_of( '.' ) );
        cyc_log( "Found loading file type %s\n", ext.c_str() );
        if( ext == "obj" )
        {
            #ifdef MESH_VERBOSE
            cyc_log( "loading %s\n", paths[ im ].c_str() )
            #endif

            //lets load the obj using the location from the .obj
            mesh = LoadObj( paths[ im ].c_str() );

            #ifdef MESH_VERBOSE
            cyc_log( "loaded %s\n", paths[ im ].c_str() )
            #endif

            res.push_back( mesh );
        }
        else
        {
            cyc_log( "Mesh %s isnt a supported format, supported formats is .obj\n", paths[ im ].c_str() )
        }
    }

    Lresult< vector< Mesh > > rres;
    rres.result = res;
    return rres;
}

Lresult< Mesh > cyc::LoadMesh( Mesh* mesh, string path )
{
    Lresult< Mesh > res;

    cyc_log( "Request to load file %s is being fufilled\n", path.c_str() );
    string ext = path;
    ext = ext.substr( ext.find_last_of('.')+1 );
    string file = path.substr( 0, path.find_last_of( '.' ) );
    
    if( ext == "obj" )
    {
        #ifdef MESH_VERBOSE
        cyc_log( "loading %s\n", path.c_str() )
        #endif
        auto tmesh = LoadObj( path.c_str() );
        //lets load the obj using the location from the .obj
        //mesh = &tmesh;
        memcpy( mesh, &tmesh, sizeof( tmesh ) ); 

        #ifdef MESH_VERBOSE
        cyc_log( "loaded %s, %lu verticies\n", path.c_str(), mesh->vertices.size() )
        #endif
    }
    else
    {
        cyc_log( "Mesh %s isnt a supported format, supported formats is .obj\n", path.c_str() )
    }

    return res;
}

void cyc::UploadMesh( Dmesh *mesh, VmaAllocator allocator )
{
    //{
        //Dmesh m;
        //memcpy( &m, mesh, sizeof( *mesh ) );
        
        #ifdef MESH_VERBOSE
        cyc_log( "Mesh upload size %lu\n", mesh->x.vertices.size(), mesh );
        #endif

        {
            VkDeviceSize meshSize = mesh->x.vertices.size() * sizeof(Vertex);
            AllocationBuffer stagingBuffer = {};

            //we want to share our juicy secrets with the gpu, so lets say that
            VmaAllocationCreateInfo allocInfo = {};
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
            //allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

            VkBufferCreateInfo stagingBufInfo = {};
            stagingBufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            stagingBufInfo.pNext = nullptr;
            //we want a buffer girthy enough to hold our vertices, in bytes
            stagingBufInfo.size = meshSize;
            //specify that it is going to be a vertex buffer
            stagingBufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            //stagingBufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            //stagingBufInfo.flags = 0;
            
            //time to create the buffer in the gpu
            VK_CHECK(vmaCreateBuffer(allocator, &stagingBufInfo, &allocInfo,
                &stagingBuffer._buffer,
                &stagingBuffer._allocation,
                nullptr));
            #ifdef MESH_VERBOSE
            cyc_log( "Allocated buffer for mesh %s\n", mesh->x.name.c_str() )
            cyc_log( "Staging buffer created with buffer %p\n", stagingBuffer._buffer )
            #endif

            void *data;
            vmaMapMemory( allocator, stagingBuffer._allocation, &data );
            #ifdef MESH_VERBOSE
            cyc_log( "Mapped mem for mesh %s\n", mesh->x.name.c_str() )
            #endif

            memcpy_s( data, sizeof( mesh->x.vertices ), mesh->x.vertices.data(), sizeof( mesh->x.vertices ) );

            vmaUnmapMemory( allocator, stagingBuffer._allocation );
            #ifdef MESH_VERBOSE
            cyc_log( "Unmapped mem for mesh %s\n", mesh->x.name.c_str() )
            #endif

            //lets get ready to send a present to the gpu
            VkBufferCreateInfo vertBufInfo = {};
            vertBufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vertBufInfo.pNext = nullptr;

            vertBufInfo.size = meshSize;
            //this buffer will be used as the actual vertex buffer, because vulkan is shitty
            vertBufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

            VK_CHECK(vmaCreateBuffer(allocator, &vertBufInfo, &allocInfo,
            &mesh->x._vertexBuffer._buffer,
            &mesh->x._vertexBuffer._allocation,
            nullptr));
            #ifdef MESH_VERBOSE
            cyc_log( "Mesh buffer created with buffer %p\n", mesh->x._vertexBuffer._buffer )
            #endif

            ImmediateSubmit( [=]( VkCommandBuffer cmd ) {
                VkBufferCopy copy;
                copy.dstOffset = 0;
                copy.srcOffset = 0;
                copy.size = meshSize;
                vkCmdCopyBuffer( cmd, stagingBuffer._buffer, mesh->x._vertexBuffer._buffer, 1, &copy );
            });
            #ifdef MESH_VERBOSE
            cyc_log( "Submitted buffer %p copy\n", &mesh->x._vertexBuffer._buffer );
            #endif
            vmaDestroyBuffer( allocator, stagingBuffer._buffer, stagingBuffer._allocation );
            #ifdef MESH_VERBOSE
            cyc_log( "Destroyed buffer %p\n", stagingBuffer._buffer );
            #endif
            
        }

        #if 0
        VkBufferCreateInfo BufInfo = {};
        BufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufInfo.pNext = nullptr;
        //we want a buffer girthy enough to hold our vertices, in bytes
        BufInfo.size = mesh->x.vertices.size() * sizeof(Vertex);
        //specify that it is going to be a vertex buffer
        BufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        //BufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //BufInfo.flags = 0;

        //we want to share our juicy secrets with the gpu, so lets say that
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        //allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        
        AllocationBuffer stagingBuffer;

        //time to create the buffer in the gpu
        VK_CHECK(vmaCreateBuffer(allocator, &BufInfo, &allocInfo,
            &stagingBuffer._buffer,
            &stagingBuffer._allocation,
            nullptr));
        cyc_log( "Allocated buffer for mesh %s\n", meshes->at( im ).name.c_str() )
        cyc_log( "Staging buffer created with buffer %p\n", stagingBuffer._buffer )

        void *data;
        vmaMapMemory( allocator, stagingBuffer._allocation, &data );
        cyc_log( "Mapped mem for mesh %s\n", meshes->at( im ).name.c_str() )

        memcpy( data, mesh->x.vertices.data(), mesh->x.vertices.size() * sizeof( Vertex ));

        vmaUnmapMemory( allocator, stagingBuffer._allocation );
        cyc_log( "Unmapped mem for mesh %s\n", meshes->at( im ).name.c_str() )
        #endif
        
        //mesh->a = allocator;
        bool t = true;
        memcpy_s( &mesh->x.render, sizeof( mesh->x.render ), &t, sizeof( t ) );
        //memcpy_s( mesh, sizeof( *mesh ), &m, sizeof( m ) );
        //memcpy_s( mesh->a, sizeof( mesh-> a ), allocator, sizeof( allocator ) );
        //m = {};

        //vmaDestroyBuffer( allocator, mesh->x._vertexBuffer._buffer, mesh->x._vertexBuffer._allocation );
        #ifdef MESH_VERBOSE
        cyc_log( "Moved mesh memory :flushed emoji:\n" );
        #endif
    //}
    //cyc_log( "Your dumb ass finally made it\n" );
    
    
}

VertexInputDesc cyc::Vertex::GetVertexDescription()
{
    VertexInputDesc description;

    //theres only a need for 1 vertex buffer binding, with a per-vertex rate
    VkVertexInputBindingDescription mainBinding;
    mainBinding.binding = 0;
    mainBinding.stride = sizeof( Vertex );
    mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    description.bindings.push_back( mainBinding );

    //Position will be stored at location 0
    VkVertexInputAttributeDescription posAttrib;
    posAttrib.binding = 0;
    posAttrib.location = 0;
    posAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
    posAttrib.offset = offsetof( Vertex, position );

    //Normal will be stored at location 1
    VkVertexInputAttributeDescription normAttrib;
    normAttrib.binding = 0;
    normAttrib.location = 1;
    normAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
    normAttrib.offset = offsetof( Vertex, normal );

    //Color will be stored at location 2
    VkVertexInputAttributeDescription colorAttrib;
    colorAttrib.binding = 0;
    colorAttrib.location = 2;
    colorAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorAttrib.offset = offsetof( Vertex, color );

    description.attribs.push_back( posAttrib );
    description.attribs.push_back( normAttrib );
    description.attribs.push_back( colorAttrib );

    return description;
}