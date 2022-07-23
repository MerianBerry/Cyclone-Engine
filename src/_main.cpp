//#define LUNAR_NO_DEBUG //[disables vulkan debug messangers and lunar debug messangers]
#include "LunarGE.h"

void test_t(Lunar::types::instance *inst, bool *living, Lunar::types::timer *timer)
{
    bool not_spoken = false;
    while( !Lunar::flags::compare(inst->status, LUNAR_STATUS_QUIT) )
    {
        if (Lunar::time::CheckStopwatch(timer).result.milliseconds > 25.f && !not_spoken)
        {
            std::cout << "[LunarGE.subthread] Window for instance [ " << inst->name << " ] has blocked the event thread" << std::endl;
            Lunar::flags::add_flags(&inst->status, LUNAR_STATUS_EVENT_IDLE);
            not_spoken = true;
        }
        else if (Lunar::time::CheckStopwatch(timer).result.milliseconds < 25.f && not_spoken)
        {
            std::cout << "[LunarGE.subthread] Window for instance [ " << inst->name << " ] has freed the event thread" << std::endl;
            Lunar::flags::remove_flags(&inst->status, LUNAR_STATUS_EVENT_IDLE);
            not_spoken = false;
        }
        Lunar::time::WaitMCS(20);
    }
    std::cout << "[LunarGE.subthread] Main thread watcher is shutting down" << std::endl;
}
void tt(Lunar::types::instance *inst)
{
    //while ( Lunar::flags::compare(inst->status, LUNAR_STATUS_EVENT_IDLE ^ LUNAR_STATUS_AWAITING_RENDERING) )
    while ( !Lunar::flags::compare(inst->status, LUNAR_STATUS_QUIT) )
    { Lunar::time::WaitMCS(1); }
    std::cout << "[LunarGE.subthread] Event watcher is shutting down" << std::endl;
}

int main(int argc, char* argv[])
{
    bool living = true;
    Lunar::print_start();

    
    Lunar::types::instance m_inst = {};
    
    m_inst.win_title = "LunarGE (v0.0.3)";

    lunar_t::InitInstance(&m_inst);
    

    //=====================================INITIALIZING ALL THE LITTLE VULKAN SHITS======================================


    Lunar::types::cmdqueue_create_info queue_info = {};
    queue_info.amount = 1;
    Uint32 fams[] = { LUNAR_QUEUETYPE_GRAPHICS };
    queue_info.families = fams;
    
    Lunar::types::semaphore_create_info render_semaphore_info = {};
    Lunar::types::semaphore_create_info present_semaphore_info = {};

    Lunar::types::fence_create_info fence_info = {};
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    Lunar::types::frames_create_info frames_info = {};
    frames_info.frame_count = 2;
    frames_info.semaphore_count = 2;
    Lunar::types::semaphore_create_info semaphores[] = { render_semaphore_info, present_semaphore_info };
    frames_info.fence_count = 1;
    frames_info.fence_infos = &fence_info;

    Lunar::types::cmdobj_create_info cmdobj_info = {};
    cmdobj_info.obj_ammount = 1;
    cmdobj_info.pool_families = fams;
    cmdobj_info.buffer_ammount = 1;

    Lunar::types::attachment_info main_color_attachment = { LUNAR_COLOR_ATTATCHMENT_DEFAULT };
    /*
	//1 sample, we won't be doing MSAA
	main_color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// we Clear when this attachment is loaded
	main_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// we keep the attachment stored when the renderpass ends
	main_color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//we don't care about stencil
	main_color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	main_color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	//we don't know or care about the starting layout of the attachment
	main_color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	//after the renderpass ends, the image has to be on a layout ready for display
	main_color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    */
    
    Lunar::types::attachment_info attachment_infos[] = { main_color_attachment };

    Lunar::types::attachment_reference main_color_reference = {};
    main_color_reference.attachment = 0;
    main_color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Lunar::types::subpass_description subpass_info = {};
    subpass_info.colorAttachmentCount = 1;
    subpass_info.pipelineBindPoint = (VkPipelineBindPoint)LUNAR_PIPELINE_BIND_POINT_GRAPHICS;
    subpass_info.pColorAttachments = &main_color_reference;

    
    Lunar::types::subpass_description subpass_infos[] = { subpass_info };

    Lunar::types::renderpass_create_info renderpass_info = {};
    renderpass_info.attachmentCount = 1;

    renderpass_info.subpassCount = 1;

    Lunar::types::framebuffer_create_info framebuff_info = {};
    framebuff_info.layers = 1U;
    
    //HAS DEFAULT VALUES. IMAGE FORMAT = VK_FORMAT_B8G8R8A8_SRGB, COLOR SPACE = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    Lunar::types::swapchain_create_info swapchain_info = {};
    swapchain_info.present_mode = LUNAR_PRESENT_MODE_VSYNC;
    //========================================FINALLY ITS OVER===================================

    lunar_t::CreateSwapchain(&m_inst, &swapchain_info, &renderpass_info, attachment_infos, subpass_infos, &framebuff_info);

    //=============================== GET READY~~~~ ========================================

    Lunar::types::timer event_timer;
    Lunar::time::StartStopwatch(&event_timer);
    std::thread t1(test_t, &m_inst, &living, &event_timer);
    t1.detach();
    std::thread t2(tt, &m_inst);
    t2.detach();
    
    while( !Lunar::flags::compare(m_inst.status, LUNAR_STATUS_QUIT) )
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0) 
        {
            if(e.type == SDL_QUIT) {m_inst.status=LUNAR_STATUS_QUIT;break;}
            switch(e.type)
            {
                case SDL_KEYDOWN:
                    switch(e.LUNAR_KEYCODE)
                    {
                        case SDLK_ESCAPE:
                            m_inst.status=LUNAR_STATUS_QUIT;
                            break;
                        case SDLK_F11:
                            std::cout << "Borderlessing" << std::endl;
                            //SDL_SetWindowBordered(window.sdl_handle, (SDL_bool)Lunar::flags::compare(SDL_GetWindowFlags(window.sdl_handle), SDL_WINDOW_BORDERLESS));
                            if (Lunar::flags::compare(SDL_GetWindowFlags(m_inst.sdl_handle), SDL_WINDOW_FULLSCREEN_DESKTOP))
                            {
                                SDL_SetWindowFullscreen(m_inst.sdl_handle, 0);
                            }else
                            {
                                SDL_SetWindowFullscreen(m_inst.sdl_handle, SDL_WINDOW_FULLSCREEN_DESKTOP);
                            }
                        break;
                        case SDLK_F1:
                            
                        break;
                    }
                break;
                case SDL_KEYUP:
                    switch(e.LUNAR_KEYCODE)
                    {

                    }
                break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(e.LUNAR_BUTTONCODE)
                    {
                        case SDL_BUTTON_LEFT:
                            
                        break;
                        case SDL_BUTTON_RIGHT:

                        break;
                    }
                break;
                case SDL_WINDOWEVENT:
                    switch(e.window.event)
                    {
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            //std::cout << "[LunarGE] Window size has changed to " << e.window.data1 << "w | " << e.window.data2 << 'h' << std::endl;
                        break;
                    }
                break;
            }  
        }
        Lunar::time::ResetStopwatch(&event_timer);

        //SDL_SetWindowPosition(m_inst.sdl_handle, int(rand()/(float)RAND_MAX*900), int(rand()/(float)RAND_MAX*500));
        //std::cout << "hey im alive" << std::endl;
        Lunar::time::WaitMCS(16666);
    }

    lunar_t::TermainateInstance(&m_inst);
    Lunar::time::WaitMS(5000);
    char h[1];
    std::cin >> h;
}