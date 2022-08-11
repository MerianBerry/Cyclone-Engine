#include "Lunar-runtime.h"
#include "Lunar-inits.h"
#include <iostream>

#define VK_CHECK(result)                                                 \
	do                                                              \
	{                                                               \
		VkResult err = result;                                           \
		if (err)                                                    \
		{                                                           \
            lunar::WaitMS(5000);                         \
			abort();                                                \
		}                                                           \
	} while (0)

lunar::Lambda_vec<void> MasterDeletionQueue;
lunar::Lambda_vec<void> SwapchainDeletionQueue;

constexpr Uint8 FRAME_OVERLAY = 2;

void labort()
{
    lunar::WaitMS(5000);
    abort();
}

int main()
{
    lunar::StopWatch starting_time;
    lunar::StartStopwatch( &starting_time );

    lunar::WriteFile("log.txt");

	auto sys_start_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    lunar_log( "Lunarge initiated at %s\n", std::ctime( &sys_start_time ) )

    lunar::StopWatch stopwatch;
    lunar::StartStopwatch(&stopwatch);

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        lunar_log( "SDL error: SDL failed to initilise :(\n" )
        labort();
    }
    lunar_log( "SDL initiated in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds )
    lunar::ResetStopwatch( &stopwatch );

    vkb::InstanceBuilder inst_builder;
    vkb::Instance vkb_inst;
    VkDebugUtilsMessengerEXT debugger;

    vkb_inst = inst_builder.set_app_name( "Lunarge" )
    .request_validation_layers()
    .require_api_version(1, 3, 0)
    .use_default_debug_messenger()
    .build().value();

    debugger = vkb_inst.debug_messenger;

    MasterDeletionQueue.push_back(
    [=]()
    {
        vkb::destroy_instance(vkb_inst);
        //lunar_log( "Destroyed vulkan (vkb) instance :)\n" )
    });
    
    lunar_log("Vulkan instance initiated in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds)
    lunar::ResetStopwatch(&stopwatch);

    vector<string> config = lunar::GetLines("config.txt").result;
    lunar::Window mainwindow;
    mainwindow.size.width = stoi(config[0]);
    mainwindow.size.height = stoi(config[1]);
    mainwindow.title = config[2].c_str();
    
    mainwindow.sdl_handle = SDL_CreateWindow(mainwindow.title,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    mainwindow.size.width,
    mainwindow.size.height,
    SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    SDL_SetWindowMinimumSize(mainwindow.sdl_handle, 960, 540);
    SDL_Vulkan_CreateSurface(mainwindow.sdl_handle, vkb_inst.instance, &mainwindow.surface);

    lunar_log("Window creation has been completed in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
    lunar::ResetStopwatch(&stopwatch);

    //==============================GPU SELECTION================================
    vkb::PhysicalDeviceSelector phys_selector{ vkb_inst };
    vkb::PhysicalDevice phys_device = phys_selector
        .set_minimum_version(1, 3)
        .set_surface(mainwindow.surface)
        .select()
        .value();
    lunar_log("Physical device selected in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
    lunar::ResetStopwatch(&stopwatch);

    vkb::DeviceBuilder deviceBuilder{ phys_device };

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
	shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	shader_draw_parameters_features.pNext = nullptr;
	shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

    vkb::Device vkbDevice = deviceBuilder.add_pNext(&shader_draw_parameters_features).build().value();
    
    lunar_log("Device has been selected in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
    lunar::ResetStopwatch(&stopwatch);

    

    //=============================MF SWAPCHAIN==================================

    vkb::Swapchain vkbSwapchain;
    lunar::swapchain swpchain;
    //auto renderPass = std::make_unique<VkRenderPass>( NULL );
    VkRenderPass renderPass;
    vector<VkFramebuffer> frameBufs;

    std::function<void(VkRenderPass, vector<VkFramebuffer>*, lunar::swapchain*, lunar::Window*, vkb::Swapchain*)> swipychain([=](VkRenderPass rdpass, vector<VkFramebuffer> *fmrbufs, lunar::swapchain *swapchain, lunar::Window *window, vkb::Swapchain* swph){
        
        vkb::SwapchainBuilder vkswapchain_builder{ phys_device, vkbDevice.device, window->surface };
        
        auto tmpswapchain = vkswapchain_builder
        .set_desired_format((VkSurfaceFormatKHR){ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_format_feature_flags(VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
        .set_old_swapchain(*swph)
        //vulkan present mode
        .set_desired_present_mode( (VkPresentModeKHR)LUNAR_PRESENT_MODE_VSYNC )
        .set_desired_extent(window->size.width, window->size.height)
        .build()
        .value();

       

        

        *swph = tmpswapchain;
        swapchain->swapchain = tmpswapchain.swapchain;
        swapchain->image_views = tmpswapchain.get_image_views().value();
        swapchain->image_format = tmpswapchain.image_format;
        swapchain->images = tmpswapchain.get_images().value();
        swapchain->presentmode = (VkPresentModeKHR)LUNAR_PRESENT_MODE_VSYNC;

        vector<VkImageView> h1 = swph->get_image_views().value();
        vector<VkImageView> h2 = tmpswapchain.get_image_views().value();

        for ( auto o : h2 )
        {
            //lunar_log( "tmpswapchain: %p\n", o )
            vkDestroyImageView( vkbDevice.device, o, nullptr );
        }

        for ( auto o : h1 )
        {
            //lunar_log( "swph: %p\n", o )
            vkDestroyImageView( vkbDevice.device, o, nullptr );
        }

        SwapchainDeletionQueue.push_back([=]()
        {
            for ( auto o : swapchain->image_views )
            {
                //lunar_log("help? %p\n", o)
                vkDestroyImageView( vkbDevice.device, o, nullptr );
            }
        });

        lunar::attachment_info mainColorInfo;
        //1 sample, we won't be doing MSAA
        mainColorInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        // we Clear when this attachment is loaded
        mainColorInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        // we keep the attachment stored when the renderpass ends
        mainColorInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //we don't care about stencil
        mainColorInfo.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        mainColorInfo.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //we don't know or care about the starting layout of the attachment
        mainColorInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //after the renderpass ends, the image has to be on a layout ready for display
        mainColorInfo.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        mainColorInfo.flags = 0;
        mainColorInfo.format = swapchain->image_format;
        

        lunar::attachment_reference mainColorReference;
        mainColorReference.attachment = 0;
        mainColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        lunar::subpass_description subpass;
        subpass.colorAttachmentCount = 1;
        subpass.pipelineBindPoint = (VkPipelineBindPoint)LUNAR_PIPELINE_BIND_POINT_GRAPHICS;

        lunar::attachment_reference attachmentrefs[] = { mainColorReference };
        subpass.pColorAttachments = attachmentrefs;
        subpass.inputAttachmentCount = NULL;
        subpass.pInputAttachments = NULL;
        subpass.pResolveAttachments = NULL;
        subpass.preserveAttachmentCount = NULL;
        subpass.pPreserveAttachments = NULL;
        subpass.pResolveAttachments = NULL;
        subpass.pDepthStencilAttachment = NULL;
        subpass.flags = 0;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        lunar::renderpass_create_info renderPassInfo;
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.pNext = nullptr;
        
        renderPassInfo.attachmentCount = 1;
        VkAttachmentDescription attachments[] = { mainColorInfo };
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        VkSubpassDescription subpasses[] { subpass };
        renderPassInfo.pSubpasses = subpasses;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        renderPassInfo.flags = 0;

        VK_CHECK(vkCreateRenderPass(vkbDevice.device, &renderPassInfo, nullptr, &rdpass));
        SwapchainDeletionQueue.push_back([=](){
            vkDestroyRenderPass(vkbDevice.device, rdpass, nullptr);
        });

        VkFramebufferCreateInfo frameBufInfo;
        frameBufInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufInfo.pNext = nullptr;

        frameBufInfo.layers = 1U;
        frameBufInfo.width = window->size.width;
        frameBufInfo.height = window->size.height;
        frameBufInfo.renderPass = rdpass;
        frameBufInfo.flags = 0;
        

        const uint32_t swapchain_imagecount = swapchain->images.size();
        //lunar_log( "swpchain image count %lu\n", swapchain->images.size() )
        fmrbufs->resize(swapchain_imagecount);
        
        for ( int i = 0; i < swapchain_imagecount; ++i )
        {
            frameBufInfo.pAttachments = &swapchain->image_views[i];
            frameBufInfo.attachmentCount = 1;

            VK_CHECK(vkCreateFramebuffer( vkbDevice.device, &frameBufInfo, nullptr, &fmrbufs->at(i) ));

            SwapchainDeletionQueue.push_back([=](){
                vkDestroyFramebuffer(vkbDevice.device, fmrbufs->at(i), nullptr);
            });
        }
    });
    (swipychain)(renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);

    lunar_log( "Swapchain created in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds )
    lunar::ResetStopwatch(&stopwatch);

    lunar::frameData frames;
    
    VkSemaphoreCreateInfo render_semaphore_info = vkinit::semaphore_create_info();
    VkSemaphoreCreateInfo present_semaphore_info = vkinit::semaphore_create_info();

    VkFenceCreateInfo fenceInfo = vkinit::fence_create_info( VK_FENCE_CREATE_SIGNALED_BIT );
    //vkinit::command

    VkCommandPoolCreateInfo cmdPoolInfo = vkinit::command_pool_create_info( (Uint32)LUNAR_QUEUETYPE_GRAPHICS, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

    MasterDeletionQueue.push_back(
    [=]()
    {
        SDL_DestroyWindow(mainwindow.sdl_handle);
        vkDestroySurfaceKHR(vkb_inst.instance, mainwindow.surface, nullptr);
        vkb::destroy_device(vkbDevice);
        //lunar_log( "Destroyed vulkan (vkb) device :)\n" )
    });
    
    lunar_log( "Beggining runtime, init sequence took %0.1fms\n\n", lunar::CheckStopwatch(starting_time).result.milliseconds )
    //================================RUNTIME====================================

    SDL_Event e;
    Uint32 runtime_status = LUNAR_STATUS_IDLE;
    while (!lunar::CompareFlags(runtime_status, LUNAR_STATUS_QUIT))
    {
        lunar::StopWatch frame_delta;
        lunar::StartStopwatch(&frame_delta);
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                runtime_status |= LUNAR_STATUS_QUIT;
                break;
            }
            switch( e.key.keysym.sym )
            {
                case SDLK_ESCAPE:
                    runtime_status |= LUNAR_STATUS_QUIT;
                break;
            }
            switch( e.window.event )
            {
                case SDL_WINDOWEVENT_RESIZED:
                    lunar::StopWatch swpchainrecreation;
                    lunar::StartStopwatch( &swpchainrecreation );
                    mainwindow.size.width = e.window.data1;
                    mainwindow.size.height = e.window.data2;
                    lunar::RqueueUse(SwapchainDeletionQueue);
                    SwapchainDeletionQueue.clear();
                    (swipychain)(renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);
                    //lunar_log("uhhh %lu\n", vkbSwapchain.get_image_views().value().size())
                    //lunar_log("uhhh %lu\n", swpchain.image_views.size())
                    //lunar_log("uhhh %lu\n", SwapchainDeletionQueue.size())
                    lunar_log("Swapchain re-created in %0.1fms! | w: %lu, h: %lu\n", lunar::CheckStopwatch( swpchainrecreation ).result.milliseconds, e.window.data1, e.window.data2)
                break;
            }
        }
        //lunar::CheckStopwatch(frame_delta).result.milliseconds;
        lunar::WaitMS(17);
    }
    

    //string hc[] = {"hello", "goodbye"};
    //string hg[] = {"screw you", "sdfoiajd"};
    //string hd[4];
    //memcpy(hd, hc, sizeof(hc));
    //memcpy(&hd[2], hg, sizeof(hg));
    
    //============================END OF RUNTIME=================================
    
    
    lunar_log( "Lunarge is shutting down... code = %lu\n", runtime_status )
    lunar_log( "Runtime duration was %0.2f minutes\n", lunar::CheckStopwatch(stopwatch).result.minutes )
    
    lunar::RqueueUse(SwapchainDeletionQueue);
    vkDestroySwapchainKHR(vkbDevice.device, swpchain.swapchain, nullptr);
    lunar::RqueueUse(MasterDeletionQueue);
    
    SDL_Quit();

    string ending;
    std::getline(std::cin, ending);
    return EXIT_SUCCESS;
}
