#include "LunarGE.h"
//#include "Lunar_inttypes.h"
#include "SELR_inits.h"

#ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wtautological-compare" // comparison of unsigned expression < 0 is always false
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wnullability-completeness"
#endif
bool SDL_INITED = false;
//==========================PRIVATE METHODS, EXCLUSIVE FOR LUNAR ENGINE USE ONLY=====================
/*
bool __cdecl Lunar_init_vkswapchain(Lunar::Lunar_window *lwindow, VkPresentModeKHR presentmode)
{
    std::cout <<  "Swapchain created" << std::endl;
    vkb::SwapchainBuilder vkswapchain_builder{ lg_vkphysdevice, lg_vkdevice, lwindow->surface};

    vkb::Swapchain vkb_swapchain = vkswapchain_builder
        .use_default_format_selection()

        //vulkan present mode
        .set_desired_present_mode(presentmode)
        .set_desired_extent(lwindow->size.width, lwindow->size.height)
        .build()
        .value();

    lg_vkswapchain.vk_swapchain = vkb_swapchain.swapchain;
    lg_vkswapchain.vk_swapchain_images = vkb_swapchain.get_images().value();
    lg_vkswapchain.vk_swapchain_image_views = vkb_swapchain.get_image_views().value();
    lg_vkswapchain.vk_swapchain_image_format = vkb_swapchain.image_format;
    lg_vkswapchain.vk_presentmode = presentmode;
    for (int i = 0; i < lg_vkswapchain.vk_swapchain_image_views.size(); ++i)
    {
        std::cout << "Swapchain image view index [" << i <<"] value :: " << lg_vkswapchain.vk_swapchain_image_views[i] << std::endl;
    }
    vkswapchainDeletionQueue.push_function([=](){
        //destroy swapchain resources
		for (int i = 0; i < lg_vkswapchain.vk_swapchain_image_views.size(); i++) {
            std::cout << "DELTION :: Swapchain image view index [" << i <<"] value :: " << lg_vkswapchain.vk_swapchain_image_views[i] << std::endl;
			vkDestroyImageView(lg_vkdevice, lg_vkswapchain.vk_swapchain_image_views[i], nullptr);
		}
        vkDestroySwapchainKHR(lg_vkdevice, lg_vkswapchain.vk_swapchain, nullptr);   
    });
    return true;
}
*/
//========================================LUNAR/Lunar::utils METHODS===============================
void __cdecl Lunar::print_start()
{
    auto Lunr_start = std::chrono::high_resolution_clock::now();
    auto _start = std::chrono::system_clock::now();
	std::time_t sys_start_time = std::chrono::system_clock::to_time_t(_start);
	std::cout << "LunarGE init time:: " << std::ctime(&sys_start_time);
}
/*
bool __cdecl Lunar::Lunar_init(Lunar::types::instance *inst)
{
    

    vkb::InstanceBuilder inst_builder;
    vkb::Instance vkb_inst;

    auto inst_ret = inst_builder.set_app_name("LunarGE")
    .request_validation_layers(true)
    .require_api_version(1, 3, 0)
    .use_default_debug_messenger()
    .build();
    vkb_inst = inst_ret.value();
    lg_vkinstance = vkb_inst.instance;
    lg_vkdebugmessenger = vkb_inst.debug_messenger;

    //===============CREATE THE WINDOW AND ASSIGN IT A SURFACE===================
    //Lunar::window::generate(lwindow);
    //SDL_Vulkan_CreateSurface(lwindow->sdl_handle, lg_vkinstance, &lwindow->surface);
    //==============================GPU SELECTION================================
    vkb::PhysicalDeviceSelector phys_selector{ vkb_inst };
    vkb::PhysicalDevice phys_device = phys_selector
        .set_desired_version(1, 3)
        .set_surface(inst->window.surface)
        .select()
        .value();

    vkb::DeviceBuilder deviceBuilder{ phys_device };

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
	shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	shader_draw_parameters_features.pNext = nullptr;
	shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

    vkb::Device vkbDevice = deviceBuilder.add_pNext(&shader_draw_parameters_features).build().value();
    lg_vkbdevice = vkbDevice;
    lg_vkdevice = vkbDevice.device;
    lg_vkphysdevice = phys_device.physical_device;

    
    lg_inited = true;
    return true;
}

bool __cdecl createInstance(Lunar::types::instance *inst, types::cmdqueue_create_info *queue_info, types::frames_create_info *frames_info, types::cmdobj_create_info *cmdobj_info)
{

    return true;
}
bool __cdecl Lunar::Lunar_terminate(Lunar::Lunar_window *lwindow, Lunar::FrameData *frame_data, int frame_overlap)
{
    if (lg_inited)
    {
        std::cout << "\n\nTerminate command issued..." << std::endl;
        VkFence allfences[frame_overlap];
        for (int i = 0; i < frame_overlap; ++i)
        {
            allfences[i] = frame_data[i].render_fence;
        }
        VK_CHECK(vkWaitForFences(lg_vkdevice, frame_overlap, allfences, true, std::numeric_limits<uint64_t>::max()), "terminate - wait for fences");

        vkswapchainDeletionQueue.flush();
        std::cout << "Swapchain flushed..." << std::endl;
        mainDeletionQueue.flush();
        std::cout << "Main deletion queue flushed..." << std::endl;
        for (int i = 0; i < window_deletion_queue.size(); i++)
        {
            SDL_DestroyWindow(window_deletion_queue[i]->sdl_handle);
            window_deletion_queue.erase(window_deletion_queue.begin()+ i);
        }
        
        vkb::destroy_debug_utils_messenger(lg_vkinstance, lg_vkdebugmessenger);
        vkDestroyDevice(lg_vkdevice, nullptr);
        vkDestroyInstance(lg_vkinstance, nullptr);
        SDL_Quit();

        auto _start = std::chrono::system_clock::now();
        std::time_t sys_start_time = std::chrono::system_clock::to_time_t(_start);
        std::cout << "\nLunarGE terminate time:: " << std::ctime(&sys_start_time);
        return true;
    }
    return false;
}
bool __cdecl Lunar::Lunar_createQueue(Lunar::Lunar_vkqueue *queue, vkb::QueueType queue_type)
{
    queue->queue = lg_vkbdevice.get_queue(queue_type).value();
    queue->family = lg_vkbdevice.get_queue_index(queue_type).value();
    
    return true;
}
//Creates a VkCommandPool and stores it in cmd_pool. Use command_queue_family to tell it which queue type to use
bool __cdecl Lunar::Lunar_createCommand_pool(Lunar::FrameData *frame_data, uint8_t frame_overlap, uint32_t command_queue_family, uint32_t cmdpool_flags)
{
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::command_pool_create_info(command_queue_family, cmdpool_flags);

    for (int i = 0; i < frame_overlap; ++i)
    {
        vkCreateCommandPool(lg_vkdevice, &commandPoolInfo, nullptr, &frame_data[i].cmd_Pool);
        
        mainDeletionQueue.push_function([=](){
            vkDestroyCommandPool(lg_vkbdevice, frame_data[i].cmd_Pool, nullptr);
        });
    }
    
    return true;
}
bool __cdecl Lunar::Lunar_createCommand_buffer(Lunar::FrameData *frame_data, uint8_t frame_overlap, uint8_t buffer_count, VkCommandBufferLevel buffer_level)
{
    for (int i = 0; i < frame_overlap; ++i)
    {
        VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(frame_data[i].cmd_Pool, buffer_count, buffer_level);

        VK_CHECK(vkAllocateCommandBuffers(lg_vkdevice, &cmdAllocInfo, &frame_data[i].cmd_Buffer), "create cmd buff - alloc");
    }
    
    return true;
}
bool __cdecl Lunar::Lunar_createColor_attachment(Lunar::Lunar_vkRender_pass_color_attachment &color_attachment)
{   
    color_attachment.format = lg_vkswapchain.vk_swapchain_image_format;
    color_attachment.color_attachment.format = color_attachment.format;
    //default 1, not doing MSAA
    color_attachment.color_attachment.samples = color_attachment.samples;
    //what to do when loading the attachment
    color_attachment.color_attachment.loadOp = color_attachment.loadOP;
    //what to do when storing the attachment
    color_attachment.color_attachment.storeOp = color_attachment.storeOP;

    color_attachment.color_attachment.stencilLoadOp = color_attachment.stencil_loadOP;
    color_attachment.color_attachment.stencilStoreOp = color_attachment.stencil_storeOP;

    color_attachment.color_attachment.initialLayout = color_attachment.initial_layout;
    color_attachment.color_attachment.finalLayout = color_attachment.final_layout;

    color_attachment.attachment_ref.attachment = color_attachment.ref_attachment_index;
    color_attachment.attachment_ref.layout = color_attachment.ref_image_layout;
    return true;
}
bool __cdecl Lunar::Lunar_createSubPass(Lunar::Lunar_vkSubpass &subpass, Lunar::Lunar_vkRender_pass_color_attachment &color_attachments, uint32_t color_attachment_count, uint32_t subpass_flags)
{
    subpass.vksubpass.pipelineBindPoint = subpass.pipeline_bind_point;
    subpass.vksubpass.colorAttachmentCount = color_attachment_count;
    subpass.vksubpass.pColorAttachments = &color_attachments.attachment_ref;
    subpass.vksubpass.flags = subpass_flags;
    return true;
}
bool __cdecl Lunar::Lunar_createRender_pass(VkRenderPass &render_pass, Lunar::Lunar_vkRender_pass_color_attachment &color_attachments, uint32_t color_attachment_count, Lunar::Lunar_vkSubpass *subpasses, uint32_t supbass_count)
{
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    render_pass_info.attachmentCount = color_attachment_count;
    render_pass_info.pAttachments = &color_attachments.color_attachment;

    render_pass_info.subpassCount = supbass_count;
    render_pass_info.pSubpasses = &subpasses->vksubpass;
    

    VK_CHECK(vkCreateRenderPass(lg_vkdevice, &render_pass_info, nullptr, &render_pass), "create render pass - create");
    vkswapchainDeletionQueue.push_function([=]()
    {
        vkDestroyRenderPass(lg_vkdevice, render_pass, nullptr);
    });
    return true;
}
bool __cdecl Lunar::Lunar_createFrameBuffer(std::vector<VkFramebuffer> &frame_buffers, VkRenderPass renderpass, Lunar::Lunar_window lwindow)
{
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = renderpass;
    fb_info.attachmentCount = 1;
    
    fb_info.width = lwindow.size.width;
    fb_info.height = lwindow.size.height;
    fb_info.layers = 1;
    
    const uint32_t swapchain_imagecount = lg_vkswapchain.vk_swapchain_images.size();
    frame_buffers.resize(swapchain_imagecount);

    for (int i = 0; i < swapchain_imagecount; ++i)
    {
        fb_info.pAttachments = &lg_vkswapchain.vk_swapchain_image_views[i];
        fb_info.attachmentCount = 1;
        VK_CHECK(vkCreateFramebuffer(lg_vkdevice, &fb_info, nullptr, &frame_buffers.at(i)), "create frame buff - create");
        vkswapchainDeletionQueue.push_function([=]()
        {
            //std::cout << "Frame buff " << i << "is being shot" << std::endl;
            vkDestroyFramebuffer(lg_vkdevice, frame_buffers.at(i), nullptr);
        });
    }
    return true;
}
bool __cdecl Lunar::Lunar_createSync_structures(Lunar::FrameData *frame_data, uint8_t frame_overlap)
{
    VkFenceCreateInfo fencecreateinfo = vkinit::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);

    VkSemaphoreCreateInfo semaphorecreateinfo = vkinit::semaphore_create_info();

    for (int i = 0; i < frame_overlap; ++i)
    {
        VK_CHECK(vkCreateFence(lg_vkdevice, &fencecreateinfo, nullptr, &frame_data[i].render_fence), "create sync structs - create fence");

        VK_CHECK(vkCreateSemaphore(lg_vkdevice, &semaphorecreateinfo, nullptr, &frame_data[i].present_semaphore), "create sync structs - create semaphore present");
        VK_CHECK(vkCreateSemaphore(lg_vkdevice, &semaphorecreateinfo, nullptr, &frame_data[i].render_semaphore), "create sync structs - create semaphore render");

        mainDeletionQueue.push_function([=](){
            vkDestroyFence(lg_vkdevice, frame_data[i].render_fence, nullptr);
            vkDestroySemaphore(lg_vkdevice, frame_data[i].present_semaphore, nullptr);
            vkDestroySemaphore(lg_vkdevice, frame_data[i].render_semaphore, nullptr);
        });
    }
    return true;
}
void __cdecl Lunar::wait_for_fences(Lunar::FrameData *frame_data, uint8_t frame_overlap)
{
    //std::cout << "why am i here :: " << frame_data[Lunar::get_current_frameindex(frame_overlap)].render_fence << " : " << Lunar::get_current_frameindex(frame_overlap) << std::endl;
    VK_CHECK(vkWaitForFences(lg_vkdevice, 1, &frame_data[Lunar::get_current_frameindex(frame_overlap)].render_fence, true, 1000000000), "wait fences - wait");
    VK_CHECK(vkResetFences(lg_vkdevice, 1, &frame_data[Lunar::get_current_frameindex(frame_overlap)].render_fence), "wait fences - reset");
    VK_CHECK(vkResetCommandBuffer(frame_data[Lunar::get_current_frameindex(frame_overlap)].cmd_Buffer, 0), "wait fences - reset cmd");
}
void __cdecl Lunar::wait_all_fences(Lunar::FrameData *frame_data, int frame_overlap)
{
    VkFence allfences[frame_overlap];
    for (int i = 0; i < frame_overlap; ++i)
    {
        allfences[i] = frame_data[i].render_fence;
    }
    VK_CHECK(vkWaitForFences(lg_vkdevice, frame_overlap, allfences, true, std::numeric_limits<uint64_t>::max()), "wait all fences");
}
VkResult __cdecl Lunar::request_swapchain_image(Lunar::FrameData *frame_data, uint32_t *swapchain_image_index, uint8_t frame_overlap)
{
    VkResult result;
    //VK_CHECK(result = vkAcquireNextImageKHR(lg_vkdevice, lg_vkswapchain.vk_swapchain, 1000000000, frame_data[Lunar::get_current_frameindex(frame_overlap)].present_semaphore, nullptr, swapchain_image_index), "Request swapchain img");
    result = vkAcquireNextImageKHR(lg_vkdevice, lg_vkswapchain.vk_swapchain, 1000000000, frame_data[Lunar::get_current_frameindex(frame_overlap)].present_semaphore, nullptr, swapchain_image_index);
    VK_CHECK(result, "whhh");
    return result;
}
void __cdecl Lunar::advance_frame()
{
    ++frame_number;
}
VkSwapchainKHR* __cdecl Lunar::get_swapchain()
{
    return &lg_vkswapchain.vk_swapchain;
}
// A function that returns the time since the call of Lunar::init(), defaults to LUNAR_TIME_MILLISECONDS. Use the LUNAR_TIME enums to specify format.
float __cdecl Lunar::time::get_time_since_start(uint32_t lformat)
{
    switch(lformat)
    {
        case LUNAR_TIME_MILLISECONDS:
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Lunar_start).count();
        case LUNAR_TIME_SECONDS:
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Lunar_start).count()/1000.f;
        case LUNAR_TIME_MINUTES:
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Lunar_start).count()/60000.f;
    }
    return 0.f;
}*/

void __cdecl Lunar::time::WaitMS(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds (milliseconds) );
}
void __cdecl Lunar::time::WaitMCS(Uint32 microseconds)
{
    std::this_thread::sleep_for((std::chrono::microseconds)microseconds);
}
Lunar::Lresult<std::chrono::steady_clock::time_point> __cdecl Lunar::time::StartStopwatch(types::timer *timer)
{
    Lresult<std::chrono::steady_clock::time_point> result;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return result;
}
Lunar::Lresult<Lunar::types::times> __cdecl Lunar::time::CheckStopwatch(types::timer *timer)
{
    Lresult<Lunar::types::times> result;

    result.result.milliseconds =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/1.f;
    result.result.seconds =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/1000.f;
    result.result.minutes =  std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/60000.f;
    result.result.hours = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timer->start_time).count()/3600000.f;
    return result;
}
Lunar::Lresult<void*> __cdecl Lunar::time::ResetStopwatch(types::timer *timer)
{
    Lresult<void*> result;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return result;
}

/* Depricated for explicit use, automatically is called with Lunar::init()
bool __cdecl Lunar::window::change_title(Lunar::Lunar_window *lwindow, const char* ltitle)
{
    if(lwindow == nullptr)
    {
        std::cout << "LunarGE warning: " << "window::change_name - param - lwindow: is nullptr" << std::endl;
        return false;
    }
    else
        SDL_SetWindowTitle(lwindow->sdl_handle, ltitle);
    return true;
}
bool __cdecl Lunar::window::destroy_window(Lunar::Lunar_window *lwindow)
{
    if (lwindow->sdl_handle == nullptr)
    {
        std::cout << "LunarGE warning: Lunar::window::destroy_window - param - lwindow->sdl_handle is nullptr" << std::endl;
        return false;
    }
    for (int i = 0; i < window_deletion_queue.size(); i++)
    {
        if (window_deletion_queue[i]->sdl_handle == lwindow->sdl_handle)
        {
            //std::cout << "Window handle found in deletion queue for window title: " << lwindow->title << std::endl;
            SDL_DestroyWindow(lwindow->sdl_handle);
            window_deletion_queue.erase(window_deletion_queue.begin()+ i);
            //std::cout << "Window deletion queue size is now: " << window_deletion_queue.size() << std::endl;
            Lunar::setQuitStatus();

            return true;
        }
    }
    return false;
}
bool __cdecl Lunar::window::change_size(Lunar::Lunar_window *lwindow, int nw, int nh, std::vector<std::function<void()>> swapchain_recreation)
{
    std::cout << "New window size: " << nw << "w : " << nh << "h" << std::endl;
    lwindow->size.width = nw;
    lwindow->size.height = nh;
    //Lunar::recreate_swapchain(lwindow, swapchain_recreation);
    return true;
}
*/

bool __cdecl Lunar::flags::add_flags(uint32_t *linput, uint32_t lflags)
{
    if(linput == nullptr)
    {
        std::cout << "LunarGE warning: " << "window::change_name - param - linput: is nullptr" << std::endl;
        return false;
    }
    *linput |= lflags;
    return true;
}
bool __cdecl Lunar::flags::remove_flags(uint32_t *linput, uint32_t lflags)
{
    if(linput == nullptr)
    {
        std::cout << "LunarGE warning: " << "window::remove_flags - param - linput: is nullptr" << std::endl;
        return false;
    }
    *linput &= ~lflags;
    return true;
}
bool __cdecl Lunar::flags::compare(uint32_t lflag, uint32_t lcompare)
{
    return (lflag & lcompare) == lcompare;
}
/*
bool __cdecl Lunar::events::poll_window_resize(Lunar::Lunar_window *lwindow, SDL_Event e, std::vector<std::function<void()>> swapchain_recreation, Lunar::FrameData *frame_data, int frame_overlap)
{
    if (e.type == SDL_WINDOWEVENT_SIZE_CHANGED)
    {
        std::cout <<"WHYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\nYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\nYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY"<<std::endl;
        std::cout << "New window width: " << e.window.data1 << " || New window height: " << e.window.data2 << std::endl;
        lwindow->size.width = e.window.data1;
        lwindow->size.height = e.window.data2;

        Lunar::wait_all_fences(frame_data, frame_overlap);
        vkswapchainDeletionQueue.flush();
        std::cout << "ehlp" << std::endl;
        Lunar_init_vkswapchain(lwindow, lg_vkswapchain.vk_presentmode);
        Lunar::queue_use(swapchain_recreation);
        return true;
    }
    return false;
}
bool __cdecl Lunar::recreate_swapchain(Lunar::Lunar_window *lwindow, std::vector<std::function<void()>> swapchain_recreation, Lunar::FrameData *frame_data, int frame_overlap)
{
    int w,h = 0;
    SDL_GetWindowSize(lwindow->sdl_handle, &w, &h);
    std::cout << "New window size: " << w << "w : " << h << "h" << std::endl;
    lwindow->size.width = w;
    lwindow->size.height = h;

    Lunar::wait_all_fences(frame_data, frame_overlap);
    vkswapchainDeletionQueue.flush();
    std::cout << "ehlp" << std::endl;
    Lunar_init_vkswapchain(lwindow, lg_vkswapchain.vk_presentmode);
    Lunar::queue_use(swapchain_recreation);
    return true;
}

void Lunar::add_to_deletion_queue(std::function<void()>&& function)
{
    //mainDeletionQueue.push_function([=]() {
	//	function;
    //});
}*/

void __cdecl Lunar::queue_pushback(Lunar::types::Lambda_vec *functionqueue, types::Lambda_func functions)
{
    functionqueue->push_back(functions);
}
void __cdecl Lunar::Rqueue_use(types::Lambda_vec functionqueue)
{
    for (auto i = functionqueue.rbegin(); i != functionqueue.rend(); ++i)
    {
        (*i)();
    }
}
void __cdecl Lunar::queue_use(types::Lambda_vec functionqueue)
{
    for (auto i = functionqueue.begin(); i != functionqueue.end(); ++i)
    {
        (*i)();
    }
}
/*
int Lunar::get_current_frameindex(int frame_overlap)
{
    return frame_number % frame_overlap;
}

bool __cdecl Lunar::getQuitStatus()
{
    return programquit;
}
void __cdecl Lunar::setQuitStatus(bool status)
{
    programquit = status;
}
*/


Lunar::Lresult<Lunar::types::instance> __cdecl lunar_t::InitInstance(Lunar::types::instance *inst)
{
    Lunar::Lresult<Lunar::types::instance> result;

    vkb::InstanceBuilder inst_builder;
    vkb::Instance vkb_inst;
    #ifndef int_lunar_no_debug
        auto inst_ret = inst_builder.set_app_name(inst->name)
        .request_validation_layers(true)
        .require_api_version(1, 3, 0)
        .use_default_debug_messenger()
        .build();
    #else
        auto inst_ret = inst_builder.set_app_name(inst->name)
        .request_validation_layers(false)
        .require_api_version(1, 3, 0)
        .build();
    #endif
    inst->vkb_instance = inst_ret.value();
    vkb_inst = inst->vkb_instance;
    inst->vk_instance = inst->vkb_instance.instance;
    #ifndef int_lunar_no_debug
        inst->debug_messenger = vkb_inst.debug_messenger;
        //std::cout << "LunarGE status update: InitInstance for instance name [" << inst->name << ']' << std::endl;
    #endif
    if (!SDL_INITED)
    {
        if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
        {
            std::cout << "[LunarGE] LunarGE error: SDL failed to initiate its sub systems for instance [ " << inst->name << " ]" << std::endl;
            result.error_code = LUNAR_ERROR_SDL_INIT_ERROR;
            return result;
        }
        std::cout << "[LunarGE] SDL initiated its sub systems successfully for instance [ " << inst->name << " ]" << std::endl;
    }

    //===============CREATE THE WINDOW AND ASSIGN IT A SURFACE===================
    inst->sdl_handle = SDL_CreateWindow(
        inst->win_title,
        inst->win_pos.x,
        inst->win_pos.y,
        inst->win_size.width,
        inst->win_size.height,
        SDL_WINDOW_VULKAN | inst->win_flags
    );
    
    if (inst->sdl_handle == NULL)
    {
        std::cout << "[LunarGE] LunarGE error while generating window: inst->window.sdl_handle for instance [ " << inst->name << " ] is NULL." << std::endl;
        std::cout << "SDL error when making window: " << SDL_GetError() << std::endl;
        result.error_code = 0;
        return result;
    }
    //inst->win_flags = inst->win_flags | SDL_WINDOW_VULKAN;
    SDL_UpdateWindowSurface(inst->sdl_handle);
    SDL_SetWindowMinimumSize(inst->sdl_handle, 960, 540);
    SDL_Vulkan_CreateSurface(inst->sdl_handle, inst->vk_instance, &inst->vk_surface);
    std::cout << "[LunarGE] Window creation has been completed for instance [ " << inst->name << " ]" << std::endl;
    //==============================GPU SELECTION================================
    vkb::PhysicalDeviceSelector phys_selector{ vkb_inst };
    vkb::PhysicalDevice phys_device = phys_selector
        .set_minimum_version(1, 3)
        .set_surface(inst->vk_surface)
        .select()
        .value();
    std::cout << "[LunarGE] Physical device selected for instance [ " << inst->name << " ]" << std::endl;
    vkb::DeviceBuilder deviceBuilder{ phys_device };

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
	shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	shader_draw_parameters_features.pNext = nullptr;
	shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

    vkb::Device vkbDevice = deviceBuilder.add_pNext(&shader_draw_parameters_features).build().value();
    inst->vkb_device = vkbDevice;
    inst->vk_device = vkbDevice.device;
    inst->vk_physdevice = phys_device.physical_device;
    std::cout << "[LunarGE] Device has been selected for instance [ " << inst->name << " ]" << std::endl;

    Lunar::queue_pushback(&inst->main_deletion_queue, [=](){
        SDL_DestroyWindow(inst->sdl_handle);
        SDL_Quit();
        vkb::destroy_debug_utils_messenger(inst->vk_instance, inst->debug_messenger);
        vkDestroySurfaceKHR(inst->vk_instance, inst->vk_surface, nullptr);
        vkDestroyDevice(inst->vk_device, nullptr);
        vkDestroyInstance(inst->vk_instance, nullptr);
    });

    return result;
}

Lunar::Lresult<void*> lunar_t::CreateSwapchain(Lunar::types::instance *inst, Lunar::types::swapchain_create_info *swapchain_info, Lunar::types::renderpass_create_info *renderpass_info, Lunar::types::attachment_info *attachments, Lunar::types::subpass_description *subpass_descriptions, Lunar::types::framebuffer_create_info *framebuffer_info)
{
    Lunar::Lresult<void*> result;
    
    vkb::SwapchainBuilder vkswapchain_builder{ inst->vk_physdevice, inst->vk_device, inst->vk_surface};

    vkb::Swapchain vkb_swapchain = vkswapchain_builder
        .set_desired_format((VkSurfaceFormatKHR){ (VkFormat)swapchain_info->image_format, (VkColorSpaceKHR)swapchain_info->color_space })
        .set_format_feature_flags(VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
        //vulkan present mode
        .set_desired_present_mode((VkPresentModeKHR)swapchain_info->present_mode)
        .set_desired_extent(inst->win_size.width, inst->win_size.height)
        .build()
        .value();
    
    inst->swapchain.vk_swapchain = vkb_swapchain.swapchain;
    inst->swapchain.vk_swapchain_images = vkb_swapchain.get_images().value();
    inst->swapchain.vk_swapchain_image_views = vkb_swapchain.get_image_views().value();
    inst->swapchain.vk_swapchain_image_format = vkb_swapchain.image_format;
    inst->swapchain.vk_presentmode = (VkPresentModeKHR)swapchain_info->present_mode;

    Lunar::queue_pushback(&inst->swapchain_deletion, [=](){
        //destroy swapchain resources
		for (int i = 0; i < inst->swapchain.vk_swapchain_image_views.size(); i++) {
            std::cout << "[LunarGE] Swapchain image view index [" << i <<"] value :: " << inst->swapchain.vk_swapchain_image_views[i] << " is about to be destroyed" << std::endl;
			vkDestroyImageView(inst->vk_device, inst->swapchain.vk_swapchain_image_views[i], nullptr);
		}
        vkDestroySwapchainKHR(inst->vk_device, inst->swapchain.vk_swapchain, nullptr);
    });
    //======================================SWAPCHAIN HAS BEEN CREATED=====================================
    std::cout <<  "[LunarGE] Swapchain created for instance [ " << inst->name << " ]" << std::endl;

    //====================================ATTACHMENT CREATION==================================
    for ( Uint8 i = 0U; i < renderpass_info->attachmentCount; ++i )
    {
        //renderpass_info->pAttachments[i].format = inst->swapchain.vk_swapchain_image_format;
        attachments[i].format = inst->swapchain.vk_swapchain_image_format;
    }
    //=====================================SUBPASS CREATION=========================================
    
    renderpass_info->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

    renderpass_info->pSubpasses = subpass_descriptions;
    renderpass_info->pAttachments = attachments;

    //render_pass_info.dependencyCount = renderpass_info->dependency_count;
    //render_pass_info.pDependencies = renderpass_info->dependencies;

    std::cout << "[LunarGE] Attachment(s), subpass(s) and renderpass have been filled out for instance [ " << inst->name << " ]" << std::endl;

    VK_CHECK(vkCreateRenderPass(inst->vk_device, renderpass_info, nullptr, &inst->render_pass), "create render pass - create");
    std::cout << "[LunarGE] Renderpass has been created for instance [ " << inst->name << " ]" << std::endl;
    Lunar::queue_pushback(&inst->main_deletion_queue, [=](){
        vkDestroyRenderPass(inst->vk_device, inst->render_pass, nullptr);
    });
    
    VkFramebufferCreateInfo fb_info = {};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.pNext = nullptr;

    fb_info.renderPass = inst->render_pass;
    fb_info.attachmentCount = 1;
    
    fb_info.width = inst->win_size.width;
    fb_info.height = inst->win_size.height;
    fb_info.layers = framebuffer_info->layers;
    fb_info.flags = framebuffer_info->flags;
    
    const uint32_t swapchain_imagecount = inst->swapchain.vk_swapchain_images.size();
    inst->frame_buffs.resize(swapchain_imagecount);

    for (int i = 0; i < swapchain_imagecount; ++i)
    {
        fb_info.pAttachments = &inst->swapchain.vk_swapchain_image_views[i];
        fb_info.attachmentCount = 1;
        VK_CHECK(vkCreateFramebuffer(inst->vk_device, &fb_info, nullptr, &inst->frame_buffs[i]), "create frame buff - create");

        Lunar::queue_pushback(&inst->swapchain_deletion, [=](){
            vkDestroyFramebuffer(inst->vk_device, inst->frame_buffs[i], nullptr);
        });
    }
    std::cout << "[LunarGE] Framebuffer has been created for instance [ " << inst->name << " ]" << std::endl;

    return result;
}

Lunar::Lresult<void*> __cdecl lunar_t::TermainateInstance(Lunar::types::instance *inst)
{
    Lunar::Lresult<void*> result;

    std::cout << "\n\nTerminate command issued..." << std::endl;
    
    /*for (int i = 0; i < frame_overlap; ++i)
    {
        allfences[i] = frame_data[i].render_fence;
    }
    VK_CHECK(vkWaitForFences(lg_vkdevice, frame_overlap, allfences, true, std::numeric_limits<uint64_t>::max()), "terminate - wait for fences");
    */
    Lunar::Rqueue_use(inst->swapchain_deletion);
    std::cout << "Swapchain flushed..." << std::endl;

    Lunar::Rqueue_use(inst->main_deletion_queue);
    std::cout << "Main deletion queue flushed..." << std::endl;

    auto _start = std::chrono::system_clock::now();
    std::time_t sys_start_time = std::chrono::system_clock::to_time_t(_start);
    std::cout << "\nLunarGE terminate time:: " << std::ctime(&sys_start_time);

    return result; 
}

Lunar::Lresult<void*> __cdecl lunar_t::StartRuntime(Lunar::types::instance *inst, Lunar::types::Lambda_vec *runtime_lambda)
{
    Lunar::Lresult<void*> result;
    

    return result;
}
