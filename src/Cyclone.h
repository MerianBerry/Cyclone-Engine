#pragma once

#include <iostream>

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wtautological-compare" // comparison of unsigned expression < 0 is always false
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wnullability-completeness"
    #pragma clang diagnostic ignored "-Wstring-plus-int"
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    //#pragma clang diagnostic ignored "-W#pragma-messages"
#endif


#define cyc_log(fmt, args...) {                                                                       \
    string newFmt="[%s:%s:%i] "; newFmt+=fmt;                                                           \
    string file = __FILE__;                                                                             \
    if ( file.find_first_of('/') != string::npos ) file = file.substr( file.find_last_of('/') + 1 );    \
    newFmt=string_format (newFmt , file.c_str(), __func__, __LINE__, args );                            \
    std::cout << newFmt;                                                                                \
    cyc::AppendFile("log.txt", newFmt);                                                               \
}
#include "Cyclone-defs.h"
using std::string; using std::vector; using std::function; using std::array;

inline Uint64 FRAME_NUMBER = 0;

inline constexpr Uint8 FRAME_OVERLAP = 3;
inline Uint8 GET_FRAME()
{
    return FRAME_NUMBER % FRAME_OVERLAP;
}

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

namespace cyc
{
    template<typename T>
    struct Lresult
    {
        string message;
        T result;
        Uint32 error_code = CYC_ERROR_SUCCESS;

        operator T()
        {
            return result;
        }
    };

    template<class T>
    using Lambda_vec = std::vector<std::function<T()>>;
    template< typename T, typename ..._Types >
    using Lambda = std::function< T(_Types ...) >;

    template< typename T >
    struct Dtype
    {
        T x;
        Dtype( T _t, Lambda< void > _func )
        {
            x = _t;
            deleter = _func;
        }
        const void callFunc()
        {
            (deleter)();
        }
        ~Dtype()
        {
            (deleter)();
        }
        private:
        Lambda< void > deleter;
    };

    typedef std::chrono::steady_clock::time_point SteadyTimePoint;
    struct times
    {
        float microseconds;
        float milliseconds;
        float seconds;
        float minutes;
        float hours;
    };
    struct StopWatch
    {
        SteadyTimePoint start_time;
        SteadyTimePoint pause_time;
    };

    typedef VkAttachmentDescription attachment_info;
    typedef VkAttachmentReference attachment_reference;
    
    typedef VkSubpassDescription subpass_description;
    
    typedef VkRenderPassCreateInfo renderpass_create_info;

    struct cmdqueue_create_info
    {
        Uint8 amount;
        Uint32 *families;
    };
    
    struct semaphore_create_info
    {
        Uint32 flags;
    };
    
    struct fence_create_info
    {
        Uint32 flags;
    };

    struct frames_create_info
    {
        Uint8 frame_count;

        Uint8 semaphore_count;
        semaphore_create_info *semaphore_infos;

        Uint8 fence_count;
        fence_create_info *fence_infos;
    };

    struct cmdobj_create_info
    {
        Uint8 obj_ammount;
        Uint32 *pool_families;

        Uint8 buffer_ammount;
    };
    
    struct framebuffer_create_info
    {
        Uint32 layers;

        Uint32 flags;
    };
    struct swapchain_create_info
    {
        Uint32 image_format = VK_FORMAT_B8G8R8A8_SRGB;
        Uint32 color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

        Uint32 present_mode;
    };

    typedef vkb::Swapchain* Swapchain;
    struct T_Swapchain
    {
        VkPresentModeKHR presentmode;

        VkSwapchainKHR swapchain;

        VkFormat image_format;

        vector<VkImage> images;

        vector<VkImageView> image_views;
    };

    class PipelineBuilder {
    public:
        vector<VkPipelineShaderStageCreateInfo> _shaderStages;
        VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
        VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
        VkViewport _viewport;
        VkRect2D _scissor;
        VkPipelineRasterizationStateCreateInfo _rasterizer;
        VkPipelineColorBlendAttachmentState _colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo _multisampling;
        VkPipelineLayout _pipelineLayout;

        VkPipelineDepthStencilStateCreateInfo _depthStencil;

        VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
    };

    struct Window
    {
        SDL_Window *sdl_handle;
        VkExtent2D size = CYC_WINDOW_SIZE_DEFAULT;
        glm::vec2 pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR surface;
        const char* title = "Babys first LunarGE game";
        uint32_t flags = 0;
    };
    class Window
    {
        public:
        SDL_Window*             Handle;
        SDL_WindowFlags         SDL_flags;

        vkb::Device             Device;
        vkb::PhysicalDevice     PhysicalDevice;
        VkSurfaceKHR            Surface;
        Swapchain               Swapchain;


        VkExtent2D              Size = CYC_WINDOW_SIZE_DEFAULT;
        glm::vec2               Pos  = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        const char*             title = "Babys first LunarGE game";

    };

    struct cmd_queue
    {
        VkQueue                 queue;
        Uint32                  family;
    };

    struct cmd_obj
    {
        VkCommandPool           pool;
        VkCommandBuffer         buffer;
    };

    struct subpass
    {
        VkSubpassDescription    vksubpass = {};
        VkPipelineBindPoint     pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    };

    struct frameData {
        VkSemaphore             present_semaphore, render_semaphore;
        VkFence                 render_fence;

        VkCommandPool           cmdPool;
        VkCommandBuffer         cmdBuf;
    };

    struct Shader
    {
        VkPipeline                  Pipeline;
        VkPipelineLayout            PipelineLayout;
        vector< VkShaderModule >    ShaderModules; 

        string                      Name;
        Uint32                      Type;
        uint32_t                    FillMode;
        string                      VertShader;
        string                      FragShader;
        string                      CompShader;

        Lambda_vec< void >          Deletion;
    };

    struct AllocationBuffer
    {
        VkBuffer            _buffer;
        VmaAllocation       _allocation;
    };

    struct VertexInputDesc
    {
        vector< VkVertexInputBindingDescription > bindings;
        vector< VkVertexInputAttributeDescription > attribs;

        VkPipelineVertexInputStateCreateFlags flags = 0;
    };

    struct Vertex
    {
        glm::vec3           position;
        glm::vec3           normal;
        glm::vec3           color;

        static VertexInputDesc GetVertexDescription();
    };

    struct Mesh
    {
        glm::vec3           position = { 0.f, 0.f, 0.f };
        glm::vec3           scale = { 1.f, 1.f, 1.f };
        glm::vec3           rotation = { 0.f, 0.f, 0.f };
        vector< Vertex >    vertices;
        string              name = "";
        bool                render = false;

        AllocationBuffer    _vertexBuffer;
    };
    struct Dmesh
    {
        Mesh                x;
        //VmaAllocator        a;
        /*Lambda< void, std::unordered_map< string, Dmesh >*, vector< string >* > d =
        [=]( std::unordered_map< string, Dmesh > *meshes,
        vector< string >* keys )
        -> void
        {
            printf( "HELP \"%s\"\n", x.name.c_str() );
            x.render = false;
            if ( meshes->find( x.name ) == meshes->end() )
                printf( "hey it exits\n" );
            else
                printf( "it doestnt\n" );

            auto o = (*meshes->find( x.name )).second;

            printf( "hhhhhhhhhh\n" );
            vmaDestroyBuffer( a, o.x._vertexBuffer._buffer, o.x._vertexBuffer._allocation );
            meshes->erase( x.name );
            printf( "dddddddddd\n" );
            for ( int i = 0; i < keys->size(); ++i )
            {
                if ( keys->at( i ) == x.name )
                    keys->erase( keys->begin() + i );
            }
        };
        void Destroy()
        {
            vmaDestroyBuffer( a, x._vertexBuffer._buffer, x._vertexBuffer._allocation );
        }*/
    };

    struct MeshPushConstant
    {
        glm::vec4           data;
        glm::mat4           renderMatrix;
    };

    struct Camera
    {
        glm::vec3           pos = { 0.f, 0.f, 0.f };
        glm::vec3           rotation = { 0.f, 0.f, 0.f };
        float               FOV = 70.f;

        glm::vec2           cuttof = { .1f, 200.f };
        float               orbit = 0.f;
    };
    struct Keyboard
    {
        array< bool, 83 >    hold = { false };
        array< bool, 83 >    holdCpy = { false };
        array< bool, 83 >    pulse = { false };
    };
    struct Mouse
    {
        array< bool, 5 >    hold = { false };
        array< bool, 5 >    holdCpy = { false };
        array< bool, 5 >    pulse = { false };

        glm::vec2           velocity = { 0.f, 0.f };
    };

    struct LuaScript
    {
        lua_State          *L;
        string              name;
        bool                active;
    };

    /*struct instance
    {
        VkInstance vk_instance;
        vkb::Instance vkb_instance;
        VkDevice vk_device;
        vkb::Device vkb_device;
        VkPhysicalDevice vk_physdevice;

        Lambda_vec main_deletion_queue;
        Lambda_vec swapchain_deletion;

        const char* name = "LunarGE application";

        swapchain swapchain;

        VkRenderPass render_pass;

        cmd_queue *cmd_queue;
        int queue_ammount = 1;
        
        FrameData *frames;

        std::vector<VkFramebuffer> frame_buffs;


        SDL_Window *sdl_handle;
        VkExtent2D win_size = CYC_WINDOW_SIZE_DEFAULT;
        glm::vec2 win_pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR vk_surface;
        const char* win_title = "Babys first LunarGE game";
        uint32_t win_flags = SDL_WINDOW_RESIZABLE;

        uint32_t status = CYC_STATUS_IDLE;

        #ifndef CYC_NO_DEBUG
        VkDebugUtilsMessengerEXT debug_messenger;
        #endif
    };*/

    float get_time_since_start(uint32_t lformat = CYC_TIME_MILLISECONDS);
    void WaitMS(uint32_t milliseconds);
    void WaitUS(Uint32 microseconds);
    inline void WaitUntilTrue( Lambda< bool > _condition )
    {
        while( !_condition() )
        {
            WaitUS( 2 );
        }
    }
    Lresult<SteadyTimePoint> __cdecl StartStopwatch(StopWatch *timer);
    Lresult<times> __cdecl CheckStopwatch(StopWatch timer, SteadyTimePoint comparitor = std::chrono::high_resolution_clock::now());
    Lresult<times> __cdecl PauseStopwatch(StopWatch *timer);
    Lresult<void*> __cdecl ResetStopwatch(StopWatch *timer);

    bool __cdecl CompareFlags(Uint32 lflag_first, Uint32 lflag_second);

    template<class T>
    void __cdecl QueuePushback(Lambda_vec<T> *functionqueue, Lambda<T> functions)
    {
        functionqueue->push_back(functions);
    }
    template<class T>
    void __cdecl RqueueUse(Lambda_vec<T> functionqueue)
    {
        for (auto i = functionqueue.rbegin(); i != functionqueue.rend(); ++i)
        {
            (*i)();
        }
    }
    template<class T>
    inline void __cdecl RCqueueUse( Lambda_vec<T> *_func )
    {
        for (auto i = _func->rbegin(); i != _func->rend(); ++i)
        {
            (*i)();
        }
        _func->clear();
    }
    template<class T>
    void __cdecl QueueUse(Lambda_vec<T> functionqueue)
    {
        for (auto i = functionqueue.begin(); i != functionqueue.end(); ++i)
        {
            (*i)();
        }
    }

    Lresult<string> __cdecl ReadFile(string path);

    Lresult< void* > __cdecl WriteFile(string path, string contents = "");

    Lresult< void* > __cdecl AppendFile(string path, string addition);

    Lresult<vector<string>> __cdecl GetFiles(string path, string extention_filter);

    Lresult<vector<string>> __cdecl GetLines(string path);

    Lresult<string> __cdecl GetLine(string path, int line);

    string __cdecl GetCurrentDir();

    Lresult<void*> __cdecl CopyFile(string from, string to);

    Lresult<void*> __cdecl CopyAllFiles(string from, string to);

    bool __cdecl DoesFileExist(string path);

    Lresult< vector< Mesh >> LoadMeshes( vector< string > paths);

    Lresult< Mesh > LoadMesh( Mesh* mesh, string path );

    void UploadMesh( Dmesh *mesh, VmaAllocator allocator );

    template< typename T >
    inline T __cdecl UnorderedGet( std::unordered_map< string, T > *map, string ind )
    {
        //search for the object, and return nullptr if not found
        T r;
        auto it = map->find( ind );
        if (it == map->end()) {
            return r;
        }
        else
        {
            return (*it).second;
        }
    }

    template< typename T >
    inline bool __cdecl UnorderedExists( std::unordered_map< string, T > *map, string ind )
    {
        //search for the object, and return nullptr if not found
        auto it = map->find( ind );
        if (it == map->end()) {
            return false;
        }
        else
        {
            return true;
        }
    }

    inline bool __cdecl CheckLua( lua_State *L, int r )
    {
        if ( r != LUA_OK )
        {
            string errmsg = lua_tostring( L, -1 );
            cyc_log( "Lua err: %s\n", errmsg.c_str() )
            return false;
        }
        return true;
    }
}

using cyc::Lambda;

inline Lambda< void, Lambda< void, VkCommandBuffer >> ImmediateSubmit;
