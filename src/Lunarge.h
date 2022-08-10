#pragma once

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wtautological-compare" // comparison of unsigned expression < 0 is always false
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wnullability-completeness"
    #pragma clang diagnostic ignored "-Wstring-plus-int"
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #pragma clang diagnostic ignored "-Wunused-result"
#endif

#define lunar_log(fmt, args...) {                                 \
    string newFmt="[%s:%i] "; newFmt+=fmt;                        \
    newFmt=string_format (newFmt , __FILE__, __LINE__, args );    \
    std::cout << newFmt;                             \
    lunar::AppendFile("log.txt", newFmt);                         \
}
#include "Lunar-defs.h"
using std::string; using std::vector; using std::function; using std::array;

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

namespace lunar
{
    template<typename T>
    using Lambda = std::function<void(T)>;

    template<typename T, typename D>
    struct generictype
    {
        T data;
        std::function<void()> deleter;
        generictype (T t, D d)
        {
            data = t;
            deleter = d;
        }
        operator T() const noexcept
        {
            return data;
        }
        T operator +(T t)
        {
            return data + t;
        }
        T operator +=(T t)
        {
            return (data+=t, data);
        }
        T operator -(T t)
        {
            return data - t;
        }
        T operator -=(T t)
        {
            return (data-=t, data);
        }
        T operator *(T t)
        {
            return data * t;
        }
        T operator *=(T t)
        {
            return (data*=t, data);
        }
        T operator /(T t)
        {
            return data / t;
        }
        T operator /=(T t)
        {
            return (data/=t, data);
        }
        void operator =(T t)
        {
            data = t;
        }
        ~generictype() noexcept
        {
            (deleter)();
        }
    };
    template<typename T, typename D>
    using Dtype = generictype<T, std::function<void()>>;

    template<typename T>
    struct Lresult
    {
        string message;
        T result;
        Uint32 error_code = LUNAR_ERROR_SUCCESS;
    };

    template<class T>
    using Lambda_vec = std::vector<std::function<T()>>;
    template<class T>
    using Lambda_func = std::function<T()>;

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

    struct swapchain
    {
        VkPresentModeKHR vk_presentmode;

        VkSwapchainKHR vk_swapchain;

        VkFormat vk_swapchain_image_format;

        std::vector<VkImage> vk_swapchain_images;

        std::vector<VkImageView> vk_swapchain_image_views;
    };

    struct Window
    {
        SDL_Window *sdl_handle;
        VkExtent2D size = LUNAR_WINDOW_SIZE_DEFAULT;
        glm::vec2 pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR surface;
        const char* title = "Babys first LunarGE game";
        uint32_t flags = 0;
    };

    struct cmd_queue
    {
        VkQueue queue;
        Uint32 family;
    };

    struct cmd_obj
    {
        VkCommandPool pool;
        VkCommandBuffer buffer;
    };

    struct subpass
    {
        VkSubpassDescription vksubpass = {};
        VkPipelineBindPoint pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    };

    struct FrameData {
        VkSemaphore present_semaphore, render_semaphore;
        VkFence render_fence;

        cmd_obj *cmd;
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
        VkExtent2D win_size = LUNAR_WINDOW_SIZE_DEFAULT;
        glm::vec2 win_pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR vk_surface;
        const char* win_title = "Babys first LunarGE game";
        uint32_t win_flags = SDL_WINDOW_RESIZABLE;

        uint32_t status = LUNAR_STATUS_IDLE;

        #ifndef LUNAR_NO_DEBUG
        VkDebugUtilsMessengerEXT debug_messenger;
        #endif
    };*/

    float get_time_since_start(uint32_t lformat = LUNAR_TIME_MILLISECONDS);
    void WaitMS(uint32_t milliseconds);
    void WaitMCS(Uint32 microseconds);
    Lresult<SteadyTimePoint> __cdecl StartStopwatch(StopWatch *timer);
    Lresult<times> __cdecl CheckStopwatch(StopWatch timer, SteadyTimePoint comparitor = std::chrono::high_resolution_clock::now());
    Lresult<times> __cdecl PauseStopwatch(StopWatch *timer);
    Lresult<void*> __cdecl ResetStopwatch(StopWatch *timer);

    bool __cdecl CompareFlags(Uint32 lflag_first, Uint32 lflag_second);

    template<class T>
    void __cdecl QueuePushback(Lambda_vec<T> *functionqueue, Lambda_func<T> functions)
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
    void __cdecl QueueUse(Lambda_vec<T> functionqueue)
    {
        for (auto i = functionqueue.begin(); i != functionqueue.end(); ++i)
        {
            (*i)();
        }
    }

    Lresult<string> __cdecl ReadFile(string path);

    Lresult<void*> __cdecl WriteFile(string path, string contents = "");

    Lresult<void*> __cdecl AppendFile(string path, string addition);

    Lresult<vector<string>> __cdecl GetFiles(string path, string extention_filter);

    Lresult<vector<string>> __cdecl GetLines(string path);

    Lresult<string> __cdecl GetLine(string path, int line);

    string __cdecl GetCurrentDir();

    Lresult<void*> __cdecl CopyFile(string from, string to);

    Lresult<void*> __cdecl CopyAllFiles(string from, string to);

    bool __cdecl DoesFileExist(string path);
}