#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#include "SDL.h"
#include "SDL_vulkan.h"
#include "glm.hpp" 
#include "vulkan.h"
#include "Lunar_defs.h"

#include <iostream>
#include "math.h"
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#undef main

//constexpr float operator"" ft(long double val);

#define VK_CHECK(x, y)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = x;                                           \
		if (err)                                                    \
		{                                                           \
			std::cout <<"Detected Vulkan error: " << err << " : " << y <<  std::endl; \
            Lunar::time::WaitMS(5000);                         \
			abort();                                                \
		}                                                           \
	} while (0)

#ifdef LUNAR_NO_DEBUG
    #define int_lunar_no_debug
#endif

namespace Lunar
{
    template<typename T>
    struct Lresult
    {
        const char* message;
        T result;
        Uint32 error_code = LUNAR_ERROR_SUCCESS;
    };
    namespace types
    {
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

            Lunar::types::swapchain swapchain;

            VkRenderPass render_pass;

            Lunar::types::cmd_queue *cmd_queue;
            int queue_ammount = 1;
            
            Lunar::types::FrameData *frames;

            std::vector<VkFramebuffer> frame_buffs;

            struct h
            {

            } h;

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
    }
    
    /*
    typedef struct local_vkspwapchain
    {
        VkPresentModeKHR vk_presentmode;

        VkSwapchainKHR vk_swapchain;

        VkFormat vk_swapchain_image_format;

        std::vector<VkImage> vk_swapchain_images;

        std::vector<VkImageView> vk_swapchain_image_views;
    }Lunarl_vkswapchain;

    typedef struct Lunar_window
    {
        SDL_Window *sdl_handle;
        VkExtent2D size = LUNAR_WINDOW_SIZE_DEFAULT;
        glm::vec2 pos = { SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED };
        VkSurfaceKHR surface;
        const char* title = "Babys first LunarGE game";
        int* frame_number;
        uint32_t flags;
    }Lunar_window;
    typedef struct Lunar_vkqueue
    {
        VkQueue queue;
        uint32_t family;
    }Lunar_vkqueue;

    typedef struct Lunar_vkRender_pass_color_attachment
    {
        VkFormat format;
        VkAttachmentDescription color_attachment = {};
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
    }Lunar_vkRender_pass_color_attachment;

    typedef struct Lunar_vkSubpass
    {
        VkSubpassDescription vksubpass = {};
        VkPipelineBindPoint pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    }Lunar_vkSubpass;

    struct FrameData {
        VkSemaphore present_semaphore, render_semaphore;
        VkFence render_fence;

        VkCommandPool cmd_Pool;
        VkCommandBuffer cmd_Buffer;
    };

    struct Lunar_instance
    {
        Lunar::Lunar_window window;

        Lunar::Lunarl_vkswapchain swapchain;

        Lunar::Lunar_vkqueue cmd_queue;

        Lunar::FrameData *frame_data;
        int frame_overlap = 1;

        std::vector<VkFramebuffer> frame_buffs;


    };
    */

    uint64_t get_current_frame();
    void __cdecl print_start();
    /*
    bool __cdecl Lunar_init(Lunar::types::instance *inst, types::cmdqueue_create_info *queue_info, types::frames_create_info *frames_info, types::cmdobj_create_info *cmdobj_info);
    bool __cdecl createInstance(Lunar::types::instance *inst, types::cmdqueue_create_info *queue_info, types::frames_create_info *frames_info, types::cmdobj_create_info *cmdobj_info);
    bool __cdecl Lunar_terminate(Lunar::Lunar_window *lwindow, Lunar::FrameData *frame_data, int frame_overlap);
    bool __cdecl Lunar_createQueue(Lunar::Lunar_vkqueue *queue, vkb::QueueType queue_type = (vkb::QueueType)LUNAR_QUEUETYPE_GRAPHICS);
    bool __cdecl Lunar_createCommand_pool(Lunar::FrameData *frame_data, uint8_t frame_overlap, uint32_t command_queue_family, uint32_t cmdpool_flag = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    bool __cdecl Lunar_createCommand_buffer(Lunar::FrameData *frame_data, uint8_t frame_overlap, uint8_t buffer_count = 1, VkCommandBufferLevel buffer_level = (VkCommandBufferLevel)LUNAR_CMDBUFFER_LEVEL_PRIMARY);
    bool __cdecl Lunar_createColor_attachment(Lunar::Lunar_vkRender_pass_color_attachment &color_attachment);
    bool __cdecl Lunar_createSubPass(Lunar::Lunar_vkSubpass &subpass, Lunar::Lunar_vkRender_pass_color_attachment &color_attachments, uint32_t color_attachment_count, uint32_t subpass_flags = NULL);
    bool __cdecl Lunar_createRender_pass(VkRenderPass &render_pass, Lunar::Lunar_vkRender_pass_color_attachment &color_attachments, uint32_t color_attachment_count, Lunar::Lunar_vkSubpass *subpasses, uint32_t supbass_count);
    bool __cdecl Lunar_createFrameBuffer(std::vector<VkFramebuffer> &frame_buffers, VkRenderPass renderpass, Lunar::Lunar_window lwindow);
    bool __cdecl Lunar_createSync_structures(Lunar::FrameData *frame_data, uint8_t frame_overlap);
    void __cdecl wait_for_fences(Lunar::FrameData *frame_data, uint8_t frame_overlap);
    void __cdecl wait_all_fences(Lunar::FrameData *frame_data, int frame_overlap);
    VkResult __cdecl request_swapchain_image(Lunar::FrameData *frame_data, uint32_t *swapchain_image_index, uint8_t frame_overlap);
    VkSwapchainKHR* __cdecl get_swapchain();
    */

    void __cdecl advance_frame();

    namespace time
    {
        float get_time_since_start(uint32_t lformat = LUNAR_TIME_MILLISECONDS);
        void WaitMS(uint32_t milliseconds);
        void WaitMCS(Uint32 microseconds);
        Lresult<std::chrono::steady_clock::time_point> __cdecl StartStopwatch(types::timer *timer);
        Lresult<types::times> __cdecl CheckStopwatch(types::timer *timer);
        Lresult<void*> __cdecl ResetStopwatch(types::timer *timer);
    }
    /*
    namespace window
    {
        bool __cdecl change_title(Lunar::Lunar_window *lwindow, const char* ltitle);
        bool __cdecl destroy_window(Lunar::Lunar_window *lwindow);
        bool __cdecl change_size(Lunar::Lunar_window *lwindow, int nw, int nh, types::Lambda_vec swapchain_recreation);
    }
    
    
    namespace events
    {
        bool __cdecl poll_window_resize(Lunar::Lunar_window *lwindow, SDL_Event e, types::Lambda_vec swapchain_recreation, Lunar::FrameData *frame_data, int frame_overlap);
    }
    bool __cdecl recreate_swapchain(Lunar::Lunar_window *lwindow, types::Lambda_vec swapchain_recreation, Lunar::FrameData *frame_data, int frame_overlap);

    
    //void add_to_deletion_queue(std::function<void()>&& function);
    int get_current_frameindex(int frame_overlap);

    bool __cdecl getQuitStatus();
    void __cdecl setQuitStatus(bool status = true);
    */
    namespace flags
    {
        bool __cdecl add_flags(uint32_t *linput, uint32_t lflags);
        bool __cdecl remove_flags(uint32_t *linput, uint32_t lflags);
        bool __cdecl compare(uint32_t lflag_first, uint32_t lflag_second);
    }
    void __cdecl queue_pushback(Lunar::types::Lambda_vec *functionqueue, types::Lambda_func functions);
    void __cdecl Rqueue_use(types::Lambda_vec functionqueue);
    void __cdecl queue_use(types::Lambda_vec functionqueue);
}

namespace lunar_t
{
    Lunar::Lresult<Lunar::types::instance> __cdecl InitInstance(Lunar::types::instance *inst);
    Lunar::Lresult<void*> __cdecl CreateSwapchain(Lunar::types::instance *inst, Lunar::types::swapchain_create_info *swapchain_info, Lunar::types::renderpass_create_info *renderpass_info, Lunar::types::attachment_info *attachments, Lunar::types::subpass_description *subpass_descriptions,Lunar::types::framebuffer_create_info *frambuffer_info);
    Lunar::Lresult<void*> __cdecl TermainateInstance(Lunar::types::instance *inst);
    Lunar::Lresult<void*> __cdecl StartRuntime(Lunar::types::instance *inst, Lunar::types::Lambda_vec *runtime_lambda);
}

