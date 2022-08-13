#include "Lunarge.h"
#include <iostream>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#define VK_CHECK(result)                           \
	do                                             \
	{                                              \
		VkResult err = result;                     \
		if (err)                                   \
		{                                          \
            lunar::WaitMS(5000);                   \
			abort();                               \
		}                                          \
	} while (0)

using namespace lunar;

using std::stof;
#define stofp( x ) stof( lines[i + x].substr(0, lines[ i + x ].find_first_of('f')))
#define stofc( x ) stof( lines[i + x].substr(lines[ i + x ].find_first_of('f')+1))

lunar::Lresult< vector<lunar::Mesh> > lunar::LoadMeshes( vector< string > paths)
{
    vector<Mesh> res;
    for( Uint32 im = 0; im < paths.size(); ++im )
    {
        Mesh mesh;
        auto lines = GetLines( paths[ im ] ).result;
        for( Uint64 i = 0; i < lines.size(); i += 3 )
        {
            Vertex vert;
            vert.position = { stofp( 0 ), stofp( 1 ), stofp( 2 ) };
            vert.color = { stofc( 0 ), stofc( 1 ), stofc( 2 ) };

            mesh.vertices.push_back( vert );
        }
        mesh.name = paths[ im ];
        for ( auto o : mesh.vertices )
        {
            lunar_log( "Vertex pos %0.1fx %0.1fy %0.1fz\n", o.position.x, o.position.y, o.position.z )
            lunar_log( "Vertex col %0.1fr %0.1fg %0.1fb\n", o.color.x, o.color.y, o.color.z )
        }
        res.push_back( mesh );
    }

    Lresult< vector<Mesh> > rres;
    rres.result = res;
    return rres;
}

Lresult< vector<Mesh> > lunar::UploadMeshes( vector< Mesh > *meshes, VmaAllocator allocator, Lambda_vec< void > *deletionQueue )
{
    vector< Mesh > res;
    lunar_log( "Mesh upload size %lu\n", meshes->size() );
    for( Uint32 im = 0; im < meshes->size(); ++im )
    {
        Mesh *mesh = &meshes->at( im );
        
        #if 1
        VkDeviceSize meshSize = mesh->vertices.size() * sizeof(Vertex);
        VkBufferCreateInfo stagingBufInfo = {};
        stagingBufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        stagingBufInfo.pNext = nullptr;
        //we want a buffer girthy enough to hold our vertices, in bytes
        stagingBufInfo.size = meshSize;
        //specify that it is going to be a vertex buffer
        stagingBufInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        //stagingBufInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        //stagingBufInfo.flags = 0;

        //we want to share our juicy secrets with the gpu, so lets say that
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
        //allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        
        AllocationBuffer stagingBuffer;

        //time to create the buffer in the gpu
        VK_CHECK(vmaCreateBuffer(allocator, &stagingBufInfo, &allocInfo,
            &stagingBuffer._buffer,
            &stagingBuffer._allocation,
            nullptr));
        lunar_log( "Allocated buffer for mesh %s\n", meshes->at( im ).name.c_str() )
        lunar_log( "Staging buffer created with buffer %p\n", stagingBuffer._buffer )

        void *data;
        vmaMapMemory( allocator, stagingBuffer._allocation, &data );
        lunar_log( "Mapped mem for mesh %s\n", meshes->at( im ).name.c_str() )

        memcpy( data, mesh->vertices.data(), meshSize );

        vmaUnmapMemory( allocator, stagingBuffer._allocation );
        lunar_log( "Unmapped mem for mesh %s\n", meshes->at( im ).name.c_str() )

        //lets get ready to send a present to the gpu
        VkBufferCreateInfo vertBufInfo = {};
        vertBufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        vertBufInfo.pNext = nullptr;

        vertBufInfo.size = meshSize;
        //this buffer will be used as the actual vertex buffer, because vulkan is shitty
        vertBufInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VK_CHECK(vmaCreateBuffer(allocator, &vertBufInfo, &allocInfo,
		&mesh->_vertexBuffer._buffer,
		&mesh->_vertexBuffer._allocation,
		nullptr));
        lunar_log( "Mesh buffer created with buffer %p\n", mesh->_vertexBuffer._buffer )

        ImmediateSubmit([=](VkCommandBuffer cmd) {
            VkBufferCopy copy;
            copy.dstOffset = 0;
            copy.srcOffset = 0;
            copy.size = meshSize;
            vkCmdCopyBuffer(cmd, stagingBuffer._buffer, mesh->_vertexBuffer._buffer, 1, &copy);
	    });
        vmaDestroyBuffer( allocator, stagingBuffer._buffer, stagingBuffer._allocation );

        #else
        VkBufferCreateInfo BufInfo = {};
        BufInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufInfo.pNext = nullptr;
        //we want a buffer girthy enough to hold our vertices, in bytes
        BufInfo.size = mesh->vertices.size() * sizeof(Vertex);
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
        lunar_log( "Allocated buffer for mesh %s\n", meshes->at( im ).name.c_str() )
        lunar_log( "Staging buffer created with buffer %p\n", stagingBuffer._buffer )

        void *data;
        vmaMapMemory( allocator, stagingBuffer._allocation, &data );
        lunar_log( "Mapped mem for mesh %s\n", meshes->at( im ).name.c_str() )

        memcpy( data, mesh->vertices.data(), mesh->vertices.size() * sizeof( Vertex ));

        vmaUnmapMemory( allocator, stagingBuffer._allocation );
        lunar_log( "Unmapped mem for mesh %s\n", meshes->at( im ).name.c_str() )
        #endif
        
        //make sure to destroy the buffer
        deletionQueue->push_back([=]() {
            vmaDestroyBuffer( allocator, mesh->_vertexBuffer._buffer, mesh->_vertexBuffer._allocation );
            lunar_log( "Deleted buffer for %p\n", mesh->_vertexBuffer._buffer )
        });

        res.push_back( *mesh );
    }
    Lresult< vector< Mesh >> rres;
    rres.result = res;
    return rres;
}

VertexInputDesc lunar::Vertex::GetVertexDescription()
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