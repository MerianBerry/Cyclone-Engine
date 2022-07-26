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
    auto Lunr_start = std::chrono::high_resolution_clock::now();
    auto _start = std::chrono::system_clock::now();
	std::time_t sys_start_time = std::chrono::system_clock::to_time_t(_start);
	std::cout << "LunarGE init time:: " << std::ctime(&sys_start_time) << std::endl;

    lunar::StopWatch stopwatch;
    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        std::cout << "[Lunarge] SDL error: SDL failed to initilise\n";
        labort();
    }
    std::cout << "[Lunarge.log] SDL init time : " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms\n";
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
    });
    MasterDeletionQueue.push_back(
    [=]()
    {
        vkb::destroy_debug_utils_messenger(vkb_inst.instance, debugger);
    });
    
    std::cout << "[Lunarge.log] Instance init time : " << lunar::CheckStopwatch(stopwatch).result.milliseconds << "ms\n";
    lunar::ResetStopwatch(&stopwatch);
    
    
    std::cout << "\n[Lunarge] Lunar is shutting down\n";
    std::cout << "[Lunarge.log] Runtime : " << lunar::CheckStopwatch(stopwatch).result.minutes << "m\n"; 
    lunar::RqueueUse(MasterDeletionQueue);
    return EXIT_SUCCESS;
}
