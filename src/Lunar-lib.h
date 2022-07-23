#include "Lunar_defs.h"
using std::string; using std::vector;

namespace lunar
{
    template<typename T>
    struct Lresult
    {
        string message;
        T result;
        Uint32 error_code = LUNAR_ERROR_SUCCESS;
    };

    struct times
    {
        float milliseconds;
        float seconds;
        float minutes;
        float hours;
    };

    struct timer
    {
        std::chrono::steady_clock::time_point start_time;
        std::chrono::steady_clock::time_point pause_time;
    };
    
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
    
    typedef VkAttachmentDescription attachment_info;
    typedef VkAttachmentReference attachment_reference;
    /*struct attachment_info
    {
        VkFormat format;
        VkAttachmentDescription attachment = {};
        VkAttachmentReference attachment_ref= {};
        VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentLoadOp loadOP = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp storeOP = VK_ATTACHMENT_STORE_OP_STORE;
        VkAttachmentLoadOp stencil_loadOP = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        VkAttachmentStoreOp stencil_storeOP = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        VkImageLayout initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        uint32_t ref_attachment_index = 0;
        VkImageLayout ref_image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    };*/
    
    typedef VkSubpassDescription subpass_description;
    /*struct subpass_create_info
    {
        Lunar::types::attachment_reference *color_attatchments;
        Uint8 color_attachment_count;

        Lunar::types::attachment_reference *depthStincil_attatchment;

        Uint32 bind_point = LUNAR_PIPELINE_BIND_POINT_GRAPHICS;

        Uint32 flags;
    };*/
    
    typedef VkRenderPassCreateInfo renderpass_create_info;
    
    /*struct renderpass_create_info
    {
        Uint8 subpass_count;
        subpass_create_info *subpass_infos;

        Uint8 attachment_count;
        Lunar::types::attachment_info *attachment_infos;

        Uint32 flags;
    };*/
    
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

    typedef std::vector<std::function<void()>> Lambda_vec;
    typedef std::function<void()> Lambda_func;

    struct swapchain
    {
        VkPresentModeKHR vk_presentmode;

        VkSwapchainKHR vk_swapchain;

        VkFormat vk_swapchain_image_format;

        std::vector<VkImage> vk_swapchain_images;

        std::vector<VkImageView> vk_swapchain_image_views;
    };

    struct window
    {
        SDL_Window *sdl_handle;
        VkExtent2D size = LUNAR_WINDOW_SIZE_DEFAULT;
        glm::vec2 pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR surface;
        const char* title = "Babys first LunarGE game";
        uint32_t flags = SDL_WINDOW_RESIZABLE;
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

    struct instance
    {
        VkInstance vk_instance;
        vkb::Instance vkb_instance;
        VkDevice vk_device;
        vkb::Device vkb_device;
        VkPhysicalDevice vk_physdevice;

        Lambda_vec main_deletion_queue;
        Lambda_vec swapchain_deletion;

        const char* name = "LunarGE application";

        //Lunar::types::window window;

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
    };


    float get_time_since_start(uint32_t lformat = LUNAR_TIME_MILLISECONDS);
    void WaitMS(uint32_t milliseconds);
    void WaitMCS(Uint32 microseconds);
    Lresult<std::chrono::steady_clock::time_point> __cdecl StartStopwatch(timer *timer);
    Lresult<times> __cdecl CheckStopwatch(timer *timer, std::chrono::steady_clock::time_point comparitor = std::chrono::high_resolution_clock::now());
    Lresult<times> __cdecl PauseStopwatch(timer *timer);
    Lresult<void> __cdecl ResetStopwatch(timer *timer);

    bool __cdecl CompareFlags(uint32_t lflag_first, uint32_t lflag_second);

    void __cdecl QueuePushback(Lambda_vec *functionqueue, Lambda_func functions);
    void __cdecl RqueueUse(Lambda_vec functionqueue);
    void __cdecl QueueUse(Lambda_vec functionqueue);


    Lresult<string> __cdecl ReadFile(string path);

    template<typename T>
    Lresult<void> __cdecl WriteFile(string path, T contents = "");

    Lresult<vector<string>> __cdecl GetFiles(string path, string extention_filter);

    Lresult<vector<string>> __cdecl GetLines(string path);

    Lresult<string> __cdecl GetLine(string path, int line);

    string __cdecl CurrentDir();
}