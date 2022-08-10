#include "Lunar-runtime.h"
#include <iostream>

lunar::Lambda_vec<void> MasterDeletionQueue;

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
    lunar_log( "Lunarge initiated at %s\n", std::ctime( &sys_start_time ) );

    lunar::StopWatch stopwatch;
    lunar::StartStopwatch(&stopwatch);

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        lunar_log( "SDL error: SDL failed to initilise :(\n" )
        labort();
    }
    lunar_log( "SDL initiated in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds );
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
        lunar_log( "Destroyed vulkan (vkb) instance :)\n" )
    });
    
    lunar_log("Vulkan instance initiated in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
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
    lunar::cmdqueue_create_info cmdQueueInfo;
    cmdQueueInfo.amount = 1;
    Uint32 fams[] = { LUNAR_QUEUETYPE_GRAPHICS };
    cmdQueueInfo.families = fams;

    lunar::Lambda_func<vkb::Swapchain> swipychain([=, &vkbSwapchain](){
        vkb::SwapchainBuilder vkswapchain_builder{ phys_device, vkbDevice.device, mainwindow.surface};
    
        vkb::Swapchain vkb_swapchain = vkswapchain_builder
        .set_desired_format((VkSurfaceFormatKHR){ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_format_feature_flags(VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
        //vulkan present mode
        .set_desired_present_mode( VK_PRESENT_MODE_FIFO_KHR )
        .set_desired_extent(mainwindow.size.width, mainwindow.size.height)
        .build()
        .value();
        vkbSwapchain = vkb_swapchain;
        return vkb_swapchain;
    });
    (swipychain)();

    lunar_log("Command queue and swapchain created in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
    lunar::ResetStopwatch(&stopwatch);

    lunar::semaphore_create_info render_semaphore_info;
    lunar::semaphore_create_info present_semaphore_info;

    lunar::fence_create_info fence_info = { VK_FENCE_CREATE_SIGNALED_BIT };

    

    MasterDeletionQueue.push_back(
    [=]()
    {
        SDL_DestroyWindow(mainwindow.sdl_handle);
        vkb::destroy_swapchain(vkbSwapchain);
        vkDestroySurfaceKHR(vkb_inst.instance, mainwindow.surface, nullptr);
        vkb::destroy_device(vkbDevice);
        lunar_log( "Destroyed vulkan (vkb) device :)\n" )
    });
    
    lunar_log( "Beggining runtime\n\n" )
    //================================RUNTIME====================================
    SDL_Event e;
    Uint32 runtime_status = LUNAR_STATUS_IDLE;
    while (!lunar::CompareFlags(runtime_status, LUNAR_STATUS_QUIT))
    {
        lunar::StopWatch frame_delta;
        lunar::StartStopwatch(&frame_delta);
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE || e.type == SDL_QUIT)
            {
                runtime_status |= LUNAR_STATUS_QUIT;
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
    
    lunar::RqueueUse(MasterDeletionQueue);
    
    SDL_Quit();

    string ending;
    std::getline(std::cin, ending);
    return EXIT_SUCCESS;
}
