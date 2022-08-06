#include "Lunar-runtime.h"
#include <iostream>

lunar::Lambda_vec MasterDeletionQueue;

void labort()
{
    lunar::WaitMS(5000);
    abort();
}

int main()
{
    lunar::WriteFile("log.txt");

    auto Lunr_start = std::chrono::high_resolution_clock::now();
    auto _start = std::chrono::system_clock::now();
	std::time_t sys_start_time = std::chrono::system_clock::to_time_t(_start);
	std::cout << "LunarGE init time:: " << std::ctime(&sys_start_time) << std::endl;
    lunar::AppendFile("log.txt", string("Lunarge init time:: ") + std::ctime(&sys_start_time));

    lunar::StopWatch stopwatch;
    lunar::StartStopwatch(&stopwatch);

    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        std::cout << "[Lunarge] SDL error: SDL failed to initilise\n";
        labort();
    }
    printf("[Lunarge.log] SDL init time : %fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds);
    lunar::AppendFile("log.txt", string("\nSDL init time: ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.milliseconds) + "ms");
    lunar::ResetStopwatch(&stopwatch);

    vkb::InstanceBuilder inst_builder;
    vkb::Instance vkb_inst;
    VkDebugUtilsMessengerEXT debugger;

    vkb_inst = inst_builder.set_app_name("Lunarge")
    .request_validation_layers()
    .require_api_version(1, 3, 0)
    .use_default_debug_messenger()
    .build().value();

    debugger = vkb_inst.debug_messenger;

    MasterDeletionQueue.push_back(
    [=]()
    {
        vkb::destroy_instance(vkb_inst);
        printf("Destroyed vulkan instace\n");
    });
    
    std::cout << "[Lunarge.log] Instance init time : " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms\n";
    lunar::AppendFile("log.txt", string("\nInstance init time: ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.milliseconds) + "ms");
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

    MasterDeletionQueue.push_back(
    [=]()
    {
        SDL_DestroyWindow(mainwindow.sdl_handle);
    });

    std::cout << "[LunarGE] Window creation has been completed in " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms" << std::endl;
    lunar::AppendFile("log.txt", string("\nWindow creation has been completed in ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.milliseconds) + "ms");
    lunar::ResetStopwatch(&stopwatch);

    //==============================GPU SELECTION================================
    vkb::PhysicalDeviceSelector phys_selector{ vkb_inst };
    vkb::PhysicalDevice phys_device = phys_selector
        .set_minimum_version(1, 3)
        .set_surface(mainwindow.surface)
        .select()
        .value();
    std::cout << "[LunarGE] Physical device selected in " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms" << std::endl;
    lunar::AppendFile("log.txt", string("\nPhysical device selected in ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.milliseconds) + "ms");
    lunar::ResetStopwatch(&stopwatch);

    vkb::DeviceBuilder deviceBuilder{ phys_device };

    VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
	shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
	shader_draw_parameters_features.pNext = nullptr;
	shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

    vkb::Device vkbDevice = deviceBuilder.add_pNext(&shader_draw_parameters_features).build().value();
    std::cout << "[LunarGE] Device has been selected in " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms" << std::endl;
    lunar::AppendFile("log.txt", string("\nDevice has been selected in ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.milliseconds) + "ms");
    lunar::ResetStopwatch(&stopwatch);

    MasterDeletionQueue.push_back(
    [=]()
    {
        vkDestroySurfaceKHR(vkb_inst.instance, mainwindow.surface, nullptr);
        vkb::destroy_device(vkbDevice);
        printf("Destroyed vulkan device\n");
    });
    //================================RUNTIME====================================
    SDL_Event e;
    bool die = false;
    while (!die)
    while(SDL_PollEvent(&e) != 0)
    {
        if (e.key.keysym.sym == SDLK_ESCAPE)
        {
            die=true;
        }
    }
    
    //============================END OF RUNTIME=================================
    
    std::cout << "\n[Lunarge] Lunarge is shutting down\n";
    lunar::AppendFile("log.txt", "\n\nLunar is shutting down...");
    std::cout << "[Lunarge.log] Runtime : " << lunar::CheckStopwatch(stopwatch).result.minutes << "m\n"; 
    lunar::AppendFile("log.txt", string("\nLunarge runtime : ") + std::to_string(lunar::CheckStopwatch(stopwatch).result.minutes) + " minutes");
    
    lunar::RqueueUse(MasterDeletionQueue);
    
    SDL_Quit();

    string ending;
    std::getline(std::cin, ending);
    return EXIT_SUCCESS;
}
