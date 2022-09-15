#include "Cyclone.h"
#include "Cyclone-inits.h"

//#define SHADER_VERBOSE
#define NOWAIT_VERBOSE
#define SCRIPT_VERBOSE
#define VULKAN_DEBUG
#undef pi

#define VK_CHECK(result)                           \
	do                                             \
	{                                              \
		VkResult err = result;                     \
		if (err)                                   \
		{                                          \
            cyc::WaitMS(5000);                     \
			abort();                               \
		}                                          \
	} while (0)

#ifdef VULKAN_DEBUG
constexpr bool enableValidation = true;
#else
constexpr bool enableValidation = false;
#endif

cyc::Lambda_vec< void > MasterDeletionQueue;
cyc::Lambda_vec< void > SwapchainDeletionQueue;

using cyc::Lambda; using std::pair;

constexpr Uint32 FRAME_WAIT = std::numeric_limits< Uint32 >::max();
constexpr Uint32 WinWMin = 960;
constexpr Uint32 WinHMin = 540;

vector< Lambda< void > > _NOWAITLIST;
#define NoWait _NOWAITLIST.push_back

Uint32 runtime_status = CYC_STATUS_IDLE;

std::unordered_map< string, cyc::Dmesh > Meshes;
vector< string > MeshKeys;

cyc::Mouse Mouse;
cyc::Keyboard Keys;

glm::vec4 Clearrgba = { .0f, .0f, .0f, 1.f };

//look at this man
//This is the main() local gpu mem allocator. this is used to allocate memory on the gpu, and cpu
VmaAllocator vmaAllocator;


/**
 * It runs all the functions in the _NOWAITLIST vector, then clears the vector, then waits 5
 * microseconds
 * 
 * @param status The status of the main thread.
 */
void main_( Uint32 *status )
{
    #ifdef NOWAIT_VERBOSE
    cyc_log( "No wait thread created\n" )
    #endif
    while( !cyc::CompareFlags( *status, CYC_STATUS_QUIT ) )
    {
        for( auto o : _NOWAITLIST )
        {
            #ifdef NOWAIT_VERBOSE
            cyc_log( "No wait item is being excecuted\n" );
            #endif
            o();
        }
        _NOWAITLIST.clear();
        cyc::WaitUS( 5 );
    }
    #ifdef NOWAIT_VERBOSE
    cyc_log( "No wait thread dead\n" )
    #endif
}

void cabort()
{
    cyc::WaitMS(5000);
    abort();
}

int __cdecl lua_LoadMesh( lua_State *L )
{
    //printf( "[C++] lua_LoadMesh function called\n" );
    string path = lua_tostring( L, 1 );
    string name = lua_tostring( L, 2 );

    #ifdef SCRIPT_VERBOSE
    cyc_log( "[C++] LoadMesh( \"%s\", \"%s\" ) called, file exists: %i\n", path.c_str(), name.c_str(), cyc::DoesFileExist( path ) );
    #endif

    if ( !cyc::UnorderedExists( &Meshes, name ) )
    {
        Meshes[ name ] = {};
        auto o = &(*Meshes.find( name )).second;
        Meshes[ name ].x.name = name;

        NoWait( [=]()
        -> void
        {
            cyc::LoadMesh( &o->x, path );
            MeshKeys.push_back( name );

            cyc_log( "Mesh has a vertice count of %lu.\n", o->x.vertices.size() );
            #ifdef SCRIPT_VERBOSE
            cyc_log( "Loaded mesh \"%s\", preparing to upload...\n", name.c_str() );
            #endif
            
            cyc::UploadMesh( o, vmaAllocator );

            #ifdef SCRIPT_VERBOSE
            cyc_log( "Uploaded mesh \"%s\". Is rendering %i\n", name.c_str(), o->x.render );
            #endif
        });
        #ifdef SCRIPT_VERBOSE
        // cyc_log( "New mesh \'%s\' appended with path \'%s\'\n", name.c_str(), path.c_str() );
        #endif
    }
    else
    {
        #ifdef SCRIPT_VERBOSE
        cyc_log( "Mesh \'%s\' already exists, no new mesh loaded\n", name.c_str() );
        #endif
    }
    lua_pushstring( L, name.c_str() );
    return 1;
}

int __cdecl lua_GetKeyState( lua_State *L )
{
    int key = lua_tonumber( L, 1 );
    string mode = lua_tostring( L, 2 );

    if ( mode == "hold" )
    {
        lua_pushboolean( L, Keys.hold[ key ] );
    }
    else if ( mode == "pulse" )
    {
        lua_pushboolean( L, Keys.pulse[ key ] );
    }
    else
    {
        cyc_log( "\'%s\' is not a valid mode\n", mode.c_str() )
    }
    return 1;
}

int __cdecl lua_GetButtonState( lua_State *L )
{
    int btn = lua_tonumber( L, 1 );
    string mode = lua_tostring( L, 2 );

    if ( mode == "hold" )
    {
        lua_pushboolean( L, Mouse.hold[ btn ] );
    }
    else if ( mode == "pulse" )
    {
        lua_pushboolean( L, Mouse.pulse[ btn ] );
    }
    else
    {
        cyc_log( "\'%s\' is not a valid mode\n", mode.c_str() )
    }
    return 1;
}

int __cdecl lua_DrawClear( lua_State *L )
{
    float r = (float)lua_tonumber( L, 1 );
    float g = (float)lua_tonumber( L, 2 );
    float b = (float)lua_tonumber( L, 3 );
    float a = (float)lua_tonumber( L, 4 );

    Clearrgba = { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
    return 0;
}

int __cdecl lua_Exit( lua_State *L )
{
    int code = lua_tonumber( L, 1 );
    string mesg = lua_tostring( L, 2 );
    
    if ( mesg != "" )
    {
        cyc_log( "A lua file has called Exit with code #%i, and a message: \'%s\'\n", code, mesg.c_str() )
    }
    else
        cyc_log( "A lua file has called Exit with code #%i\n", code )
    runtime_status |= CYC_STATUS_QUIT;

    return 0;
}

cyc::Window mainwindow;
Uint32 WindowScriptStats = 0;

int __cdecl lua_SetWindowSize( lua_State *L )
{
    int w = lua_tonumber( L, 1 );
    int h = lua_tonumber( L, 2 );

    WindowScriptStats |= 1;
    mainwindow.size.width = ( w < WinWMin ) ? WinWMin : w;
    mainwindow.size.height = ( h < WinHMin ) ? WinHMin : w;
    return 0;
}

int __cdecl lua_SetWindowPos( lua_State *L )
{
    int x = lua_tonumber( L, 1 );
    int y = lua_tonumber( L, 2 );

    mainwindow.pos.x = x;
    mainwindow.pos.y = y;
    SDL_SetWindowPosition( mainwindow.sdl_handle, x, y );
    return 0;
}

int __cdecl lua_SetWindowState( lua_State *L )
{
    string mode = lua_tostring( L, 1 );
    
    if ( mode == "min" )
    {
        WindowScriptStats |= 2 << 0;
    }
    else if ( mode == "max" )
    {
        WindowScriptStats |= 2 << 1;
    }
    else if ( mode == "restore" )
    {
        WindowScriptStats |= 2 << 2;
    }
    return 0;
}

vector< cyc::LuaScript > Lua_Scripts;

/**
 * It loads all the lua scripts in the scripts folder, and returns a vector of Script objects
 * 
 * @return A vector of Scripts.
 */
vector< cyc::LuaScript > __cdecl InitLua()
{
    using cyc::LuaScript;
    vector< LuaScript > scripts;

    //Get the scripts in the scripts folder
    auto scriptpaths = cyc::GetFiles( "scripts", ".lua" ).result;
    size_t scriptcnt = scriptpaths.size();

    for ( size_t i = 0; i < scriptcnt; ++i )
    {
        //Quick shortenings that will come in handy later
        LuaScript o = { };
        auto u = scriptpaths[ i ];

        //We need to create a new 'state' or Lua vm for each lua file
        //We also want to open all default libs
        o.L = luaL_newstate( );
        luaL_openlibs( o.L );

        lua_register( o.L, "cpp_loadmesh", lua_LoadMesh );
        lua_register( o.L, "cpp_GetKeyState", lua_GetKeyState );
        lua_register( o.L, "cpp_GetBtnState", lua_GetButtonState );
        lua_register( o.L, "cpp_DrawClear", lua_DrawClear );
        lua_register( o.L, "cpp_Exit", lua_Exit );
        lua_register( o.L, "cpp_SetWindowSize", lua_SetWindowSize );
        lua_register( o.L, "cpp_SetWindowPos", lua_SetWindowPos );
        lua_register( o.L, "cpp_SetWindowState", lua_SetWindowState );

        #ifdef SCRIPT_VERBOSE
        cyc_log( "Lua script detected, path is %s, name is %s\n",
            u.c_str(),
            u.substr( u.find_last_of( '\\')+1 ).c_str() );
        #endif
        o.name = u.substr( u.find_last_of( '\\' )+1 );

        //If luaL_dofile returns LUA_OK, then, set the script to active
        if ( cyc::CheckLua( o.L, luaL_dofile( o.L, u.c_str()) ) )
        {
            o.active = true;
            scripts.push_back( o );
        }
    }
    cyc_log( "Finished loading Lua scripts. %lu out of %lu scripts had errors.\n",
        scriptcnt - scripts.size(),
        scripts.size() );
    

    return scripts;
}

int main()
{
    cyc::StopWatch starting_time;
    cyc::StartStopwatch( &starting_time );

    //clear the log file
    cyc::WriteFile("log.txt", "");

    auto sys_start_time = std::chrono::system_clock::to_time_t( std::chrono::system_clock::now() );
    cyc_log( "Lunarge initiated at %s\n", std::ctime( &sys_start_time ) )

    cyc::StopWatch stopwatch;
    cyc::StartStopwatch(&stopwatch);


    //Initilise SDL with every subsystem
    if ( SDL_Init(SDL_INIT_EVERYTHING) != 0 )
    {
        cyc_log( "SDL error: SDL failed to initilise :(\n" )
        cabort();
    }
    cyc_log( "SDL initiated in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds )
    cyc::ResetStopwatch( &stopwatch );

    //set some main() locals ( very important )
    vkb::InstanceBuilder inst_builder;
    vkb::Device vkbDevice;
    vkb::PhysicalDevice physDevice;
    vector< VkSubmitInfo > GQueueSubmits;

    vkb::Instance vkbInstance = inst_builder
    .set_app_name( "Cyclone GUI" )
    .set_engine_name( "Cyclone engine" )
    .request_validation_layers()
    .require_api_version(1, 3, 0)
    .set_debug_callback(
        [] ( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	    VkDebugUtilsMessageTypeFlagsEXT messageType,
	    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void *pUserData )
        -> VkBool32
        {
            auto severity = vkb::to_string_message_severity( messageSeverity );
            auto type = vkb::to_string_message_type( messageType );
            if ( enableValidation )
                cyc_log( "SEVERITY[ %s ] TYPE[ %s ]: \n%s\n\n", severity, type, pCallbackData->pMessage );
            return VK_FALSE;
        }
    ).build().value();
    
    //Pushback the vkb::destroy_instance function, this will be called very last in the deltion queue
    MasterDeletionQueue.push_back(
    [=]()
    {
        vkb::destroy_instance(vkbInstance);
        //cyc_log( "Destroyed vulkan (vkb) instance :)\n" )
    });
    cyc_log("Vulkan instance initiated in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds)
    cyc::ResetStopwatch( &stopwatch );

    //=================================================CONFIG LOADING=======================================================
    //Config loading! This mostly/entirely effects the window. theres also some checks for if it exists, or if it doent have anything in it
    if( !cyc::DoesFileExist( "config.txt" ) )
    {
        cyc::WriteFile( "config.txt", "" );
    }
    vector<string> config = cyc::GetLines("config.txt").result;
    {
        if( config.size() == 0 )
        {
            cyc::WriteFile( "config.txt", string_format( "%i\n%i\n%i\n%i\nCyclone\n%i", WinWMin, WinHMin, 0, 0, 0 ) );
            config = cyc::GetLines("config.txt").result;
        }
        mainwindow.size.width = stoi(config[0]);
        mainwindow.size.height = stoi(config[1]);
        mainwindow.pos.x = stoi( config[2] ) == 0 ? SDL_WINDOWPOS_CENTERED : stoi( config[2] );
        mainwindow.pos.y = stoi( config[3] ) == 0 ? SDL_WINDOWPOS_CENTERED : stoi( config[3] );
        mainwindow.title = config[4].c_str();
        if ( stoi( config[ 5 ] ) == 1 )
        {
            mainwindow.flags |= SDL_WINDOW_MAXIMIZED;
            mainwindow.size.width = WinWMin;
            mainwindow.size.height = WinHMin;
            mainwindow.pos.x = SDL_WINDOWPOS_CENTERED;
            mainwindow.pos.y = SDL_WINDOWPOS_CENTERED;
        }
        
	    //Window creation! we already have the config data, so lets make the window with that data. also, we pass the SDL_WINDOW_HIDDEN flag so the window isnt show until runtime. 
        mainwindow.sdl_handle = SDL_CreateWindow(mainwindow.title,
        mainwindow.pos.x,
        mainwindow.pos.y,
        mainwindow.size.width,
        mainwindow.size.height,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MOUSE_FOCUS | SDL_WINDOW_HIDDEN );

        if ( stoi( config[ 5 ] ) == 1 )
           SDL_MaximizeWindow( mainwindow.sdl_handle );
        SDL_SetWindowMinimumSize(mainwindow.sdl_handle, WinWMin, WinHMin);
        SDL_Vulkan_CreateSurface(mainwindow.sdl_handle, vkbInstance.instance, &mainwindow.surface);

        cyc_log("Window creation has been completed in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds);
        cyc::ResetStopwatch(&stopwatch);
    }
    
    //==============================GPU SELECTION================================
    //We want to ask the gpu to have/expose some features, such as wireframe mode (fillModeNonSolid) and > 1.f wide line for wireframe mode (wideLines)
    //We can then create it using the vkb::Intance, we want it to have vulkan 1.3 minimun. Also, the gpu needs to know where to render, so we set the surface created in window creation
    {
        VkPhysicalDeviceFeatures gpuFeatures = { VK_FALSE };
        gpuFeatures.fillModeNonSolid = VK_TRUE;
        gpuFeatures.wideLines = VK_TRUE;
        gpuFeatures.largePoints = VK_TRUE;

        vkb::PhysicalDeviceSelector phys_selector{ vkbInstance };
        physDevice = phys_selector
            .set_minimum_version( 1, 3 )
            .set_surface( mainwindow.surface )
            .set_required_features( gpuFeatures )
            .select()
            .value();
    }
    cyc_log("Physical device selected in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds);
    cyc::ResetStopwatch(&stopwatch);
    
    //============================DEVICE SELECTION===============================
    //the vkDevice is one of if not the most important vulkan types in the entire API, nearly everything uses it
    //IDK what the VkPhysicalDeviceShaderDrawParametersFeatures bullshit is
    //theeee maintnance4features were important... for something, i forgot what they were doing here
    //We then simply create the vkbDevice with the features as pNexts
    {
        vkb::DeviceBuilder deviceBuilder{ physDevice };

        VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features = {};
        shader_draw_parameters_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETERS_FEATURES;
        shader_draw_parameters_features.pNext = nullptr;
        shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

        VkPhysicalDeviceMaintenance4Features maintenance4Features;
        maintenance4Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_4_FEATURES;
        maintenance4Features.pNext = nullptr;
        maintenance4Features.maintenance4 = VK_TRUE;

        vkbDevice = deviceBuilder
            .add_pNext( &shader_draw_parameters_features )
            .add_pNext( &maintenance4Features )
            .build().value();
        
    }
    cyc_log("Device has been selected in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds);
    cyc::ResetStopwatch(&stopwatch);

    //We need to give it a decent assortment of data, but we also want to tell it to use vulkan 1.3 ( important )
    {
        VmaAllocatorCreateInfo info = {};
        info.physicalDevice = physDevice.physical_device;
        info.device = vkbDevice.device;
        info.instance = vkbInstance;
        info.vulkanApiVersion = VK_API_VERSION_1_3;
        vmaCreateAllocator( &info, &vmaAllocator );
    }
    
    //=============================MF SWAPCHAIN==================================
    vkb::Swapchain vkbSwapchain;
    cyc::T_Swapchain swpchain;
    
    VkRenderPass renderPass;
    vector<VkFramebuffer> frameBufs;

    std::function< void(VkRenderPass*, vector<VkFramebuffer>*, cyc::T_Swapchain*, cyc::Window*, vkb::Swapchain*)> swipychain([=](VkRenderPass *rdpass, vector<VkFramebuffer> *fmrbufs, cyc::T_Swapchain *swapchain, cyc::Window *window, vkb::Swapchain* swph){
        vkb::SwapchainBuilder vkswapchain_builder{ physDevice, vkbDevice.device, window->surface };
        
        //ooooooook, here we go. we want it to use the simple SRGB color format, and we want the color space to be nonlinear SRGB
        //I THINK the format feature means the swapchain stores the image that its presenting
        //We want to set the old swapchain so vkb is happy during swapchain recreation
        //We want to set it to relaxed VSYNC, but we can change that to immediate, or regular VSYNC
        //The swapchain also want to know how much girth it should have, so lets tell it
        auto tmpswapchain = vkswapchain_builder
        .set_desired_format((VkSurfaceFormatKHR){ VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        .set_format_feature_flags(VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT)
        .set_old_swapchain(*swph)
        //vulkan present mode
        .set_desired_present_mode( (VkPresentModeKHR)CYC_PRESENT_MODE_VSYNC )
        .set_desired_extent(window->size.width, window->size.height)
        .build()
        .value();
       
	
	    //Set the old swapchain to the current build of the swapchain
	    //Modify the pointed swapchain ( the main() local swaphain) (important)
	    *swph = tmpswapchain;
        swapchain->swapchain = tmpswapchain.swapchain;
        swapchain->image_views = tmpswapchain.get_image_views().value();
        swapchain->image_format = tmpswapchain.image_format;
        swapchain->images = tmpswapchain.get_images().value();
        swapchain->presentmode = (VkPresentModeKHR)CYC_PRESENT_MODE_VSYNC;

	    //2 vectors to of swaphchain image views that dont need to exist
        vector<VkImageView> h1 = swph->get_image_views().value();
        vector<VkImageView> h2 = tmpswapchain.get_image_views().value();

	    //Destroy the said swapchain image views
        for ( auto o : h2 )
        {
            //cyc_log( "tmpswapchain: %p\n", o )
            vkDestroyImageView( vkbDevice.device, o, nullptr );
        }

        for ( auto o : h1 )
        {
            //cyc_log( "swph: %p\n", o )
            vkDestroyImageView( vkbDevice.device, o, nullptr );
        }
	
	    //Make sure to pushback the command to delete the actual image views, but since we dont want to do it just yet, we need to put it in the swapchain deletion queue
        SwapchainDeletionQueue.push_back([=]()
        {
            for ( auto o : swapchain->image_views )
            {
                //cyc_log("help? %p\n", o)
                vkDestroyImageView( vkbDevice.device, o, nullptr );
            }
        });
	    
	 
	    //uhhhh, i dont really know how to explain this
        cyc::attachment_info mainColorInfo;
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
        

        cyc::attachment_reference mainColorReference;
        mainColorReference.attachment = 0;
        mainColorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        cyc::subpass_description subpass = { 0 };
        subpass.colorAttachmentCount = 1;
        subpass.pipelineBindPoint = (VkPipelineBindPoint)CYC_PIPELINE_BIND_POINT_GRAPHICS;

        cyc::attachment_reference attachmentrefs[] = { mainColorReference };
        subpass.pColorAttachments = attachmentrefs;
	    /*
        subpass.inputAttachmentCount = NULL;
        subpass.pInputAttachments = NULL;
        subpass.pResolveAttachments = NULL;
        subpass.preserveAttachmentCount = NULL;
        subpass.pPreserveAttachments = NULL;
        subpass.pResolveAttachments = NULL;
        subpass.pDepthStencilAttachment = NULL;
        subpass.flags = 0;
	    */

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        cyc::renderpass_create_info renderPassInfo;
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

        VK_CHECK(vkCreateRenderPass(vkbDevice.device, &renderPassInfo, nullptr, rdpass));
        SwapchainDeletionQueue.push_back([=](){
            vkDestroyRenderPass(vkbDevice.device, *rdpass, nullptr);
        });

        VkFramebufferCreateInfo frameBufInfo;
        frameBufInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufInfo.pNext = nullptr;

        frameBufInfo.layers = 1U;
        frameBufInfo.width = window->size.width;
        frameBufInfo.height = window->size.height;
        frameBufInfo.renderPass = *rdpass;
        frameBufInfo.flags = 0;
        

        const uint32_t swapchain_imagecount = swapchain->images.size();
        //cyc_log( "swpchain image count %lu\n", swapchain->images.size() )
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

    std::function< void(vector< cyc::frameData >) > waitAllFences([=](vector< cyc::frameData > Frames)
    {
        VkFence allfences[FRAME_OVERLAP];
        for( int i = 0; i < FRAME_OVERLAP; ++i )
        {
            allfences[i] = Frames[i].render_fence;
        }
        VK_CHECK(vkWaitForFences( vkbDevice.device, FRAME_OVERLAP, allfences, true, FRAME_WAIT ));
    });

    swipychain(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);

    cyc_log( "Swapchain created in %0.1fms\n", cyc::CheckStopwatch(stopwatch).result.milliseconds )
    cyc::ResetStopwatch( &stopwatch );

    vector< cyc::frameData > frames ( FRAME_OVERLAP );
    VkFence uploadFence = { };
    VkCommandBuffer uploadCommandBuffer = { };
    VkCommandPool uploadCommandPool = { };

    //==========================================SYNC OBJECTS CREATION===================================================
    {
        VkSemaphoreCreateInfo renderSemaphoreInfo = vkinit::semaphore_create_info();
        VkSemaphoreCreateInfo presentSemaphoreInfo = vkinit::semaphore_create_info();

        VkFenceCreateInfo fenceInfo = vkinit::fence_create_info( VK_FENCE_CREATE_SIGNALED_BIT );
        VkCommandPoolCreateInfo cmdPoolInfo = vkinit::command_pool_create_info( (Uint32)CYC_QUEUETYPE_GRAPHICS, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

        VkFenceCreateInfo uploadFenceInfo = vkinit::fence_create_info();
        VkCommandPoolCreateInfo uploadCommandPoolInfo = vkinit::command_pool_create_info( (Uint32)CYC_QUEUETYPE_GRAPHICS );
        VK_CHECK(vkCreateCommandPool( vkbDevice.device, &uploadCommandPoolInfo, nullptr, &uploadCommandPool ));

        VkCommandBufferAllocateInfo uploadCmdBufAllocInfo = vkinit::command_buffer_allocate_info( uploadCommandPool );
        VK_CHECK(vkAllocateCommandBuffers( vkbDevice.device, &uploadCmdBufAllocInfo, &uploadCommandBuffer ));

        VK_CHECK(vkCreateFence( vkbDevice.device, &uploadFenceInfo, nullptr, &uploadFence ));

        for ( int i = 0; i < frames.size(); ++i )
        {
            VK_CHECK(vkCreateCommandPool( vkbDevice.device, &cmdPoolInfo, nullptr, &frames[i].cmdPool ));
            auto cmdAllocInfo = vkinit::command_buffer_allocate_info( frames[i].cmdPool );
            VK_CHECK(vkAllocateCommandBuffers( vkbDevice.device, &cmdAllocInfo, &frames[i].cmdBuf ));

            VK_CHECK(vkCreateSemaphore( vkbDevice.device, &renderSemaphoreInfo, nullptr, &frames[i].render_semaphore ));
            VK_CHECK(vkCreateSemaphore( vkbDevice.device, &presentSemaphoreInfo, nullptr, &frames[i].present_semaphore ));
            VK_CHECK(vkCreateFence( vkbDevice.device, &fenceInfo, nullptr, &frames[i].render_fence ));
        }
    }

    ImmediateSubmit = [&]( Lambda< void, VkCommandBuffer > _func )
    -> void
    {
        VkCommandBuffer cmd = uploadCommandBuffer;

        //begin the command buffer recording. We will use this command buffer exactly once before resetting, so we tell vulkan that
	    VkCommandBufferBeginInfo cmdBeginInfo = vkinit::command_buffer_begin_info( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

        VK_CHECK( vkBeginCommandBuffer( cmd, &cmdBeginInfo ) );

        //hit up the lambda and give it our juicy command buffer
        _func( cmd );
        
        VK_CHECK( vkEndCommandBuffer( cmd ) );

        //force the commands into submition i mean submit the commands
        VkSubmitInfo submit = vkinit::submit_info( &cmd );
        
        //submit command buffer to the queue and execute it.
        // uploadFence will now block until the graphic commands finish execution
        VK_CHECK(vkQueueSubmit( vkbDevice.get_queue((vkb::QueueType)CYC_QUEUETYPE_GRAPHICS).value(), 1, &submit, uploadFence ));

        //Lets wait for the queue to finish, then reset the fences as they are done with their job
        vkWaitForFences( vkbDevice.device, 1, &uploadFence, true, std::numeric_limits<Uint64>::max() );
        vkResetFences( vkbDevice.device, 1, &uploadFence );

        // reset the command buffers inside the command pool
        vkResetCommandPool( vkbDevice.device, uploadCommandPool, 0 );
    };

    MasterDeletionQueue.push_back([=]()
    {
        vmaDestroyAllocator( vmaAllocator );
        vkDestroyFence( vkbDevice.device, uploadFence, nullptr );
        vkDestroyCommandPool( vkbDevice.device, uploadCommandPool, nullptr );
        for( auto o : frames )
        {
            vkDestroyFence( vkbDevice.device, o.render_fence, nullptr );
            vkDestroySemaphore( vkbDevice.device, o.render_semaphore, nullptr );
            vkDestroySemaphore( vkbDevice.device, o.present_semaphore, nullptr );

            vkDestroyCommandPool( vkbDevice.device, o.cmdPool, nullptr );
        }

        SDL_DestroyWindow( mainwindow.sdl_handle );
        vkDestroySurfaceKHR( vkbInstance.instance, mainwindow.surface, nullptr );
        vkb::destroy_device( vkbDevice );
    });

    //================================================IMGUI???===========================================================
    #define RGB( r, g, b ) r / 255.f, g / 255.f, b / 255.f
    #define RGBA( r, g, b, a ) r / 255.f, g / 255.f, b / 255.f, a / 255.f

    auto WinBGCol = ImVec4{ 15 / 255.f, 15 / 255.f, 15 / 255.f, 1.f };
    auto ExDarkGray = ImVec4{ 24 / 255.f, 24 / 255.f, 24 / 255.f, 1.f };
    auto DarkGray = ImVec4{ 37 / 255.f, 37 / 255.f, 37 / 255.f, 1.f };
    auto DMedGray = ImVec4{ 44 / 255.f, 44 / 255.f, 44 / 255.f, 1.f };
    auto MedGray = ImVec4{ 57 / 255.f, 57 / 255.f, 57 / 255.f, 1.f };
    auto LMedGray = ImVec4{ 75 / 255.f, 75 / 255.f, 75 / 255.f, 1.f };
    auto Gray = ImVec4{ 119 / 255.f, 119 / 255.f, 119 / 255.f, 1.f };

    auto DarkBabyGray = ImVec4{ 37 / 255.f, 77 / 255.f, 99 / 255.f, 1.f };
    auto MedBabyGray = ImVec4{ 57 / 255.f, 122 / 255.f, 157 / 255.f, 1.f };
    auto LMedBabyGray = ImVec4{ 75 / 255.f, 91 / 255.f, 99 / 255.f, 1.f };
    auto BabyGray = ImVec4{ 119 / 255.f, 149 / 255.f, 165 / 255.f, 1.f };
    auto LBabyGray = ImVec4{ RGB( 136, 162, 176 ), 1.f };
    
    auto DarkishGreen = ImVec4{ RGBA( 75, 200, 72, 151 ) };
    auto MedGreen = ImVec4{ RGBA( 28, 206, 22, 190 ) };
    auto Green = ImVec4{ RGBA( 32, 255, 25, 200 ) };

    auto MedPurple = ImVec4{ RGB( 128, 86, 143 ), 1.f };
    auto Purple = ImVec4{ RGB( 141, 82, 162 ), 1.f };
    auto HPurple = ImVec4{ RGB( 183, 83, 218 ), 1.f };

    #define BeginGreenButton ImGui::PushStyleColor( ImGuiCol_Button, DarkishGreen );    \
                             ImGui::PushStyleColor( ImGuiCol_ButtonHovered, MedGreen ); \
                             ImGui::PushStyleColor( ImGuiCol_ButtonActive, Green )

    auto LoadImGui = [=]()
    -> void
    {
        //1: create descriptor pool for IMGUI
        // the size of the pool is very oversize, but it's copied from imgui demo itself.
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        VkDescriptorPool imguiPool;
        VK_CHECK(vkCreateDescriptorPool( vkbDevice.device, &pool_info, nullptr, &imguiPool ) );


        // 2: initialize imgui library

        //this initializes the core structures of imgui and implot
        ImGui::CreateContext();
        ImPlot::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

        io.Fonts->AddFontFromFileTTF("fonts/consola.ttf", 15.0f);
        // io.Fonts->AddFontFromFileTTF( "fonts/segoeui.ttf", 18.0f );
        
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        {
            style.Colors[ ImGuiCol_Header ] = WinBGCol;
            style.Colors[ ImGuiCol_HeaderHovered ] = ExDarkGray;
            style.Colors[ ImGuiCol_HeaderActive ]  = DarkGray;

            style.Colors[ ImGuiCol_FrameBg ] = DarkGray;
            style.Colors[ ImGuiCol_FrameBgHovered ] = MedGray;
            style.Colors[ ImGuiCol_FrameBgActive ] = LMedGray;

            style.Colors[ ImGuiCol_TitleBg ] = ExDarkGray;
            style.Colors[ ImGuiCol_TitleBgActive ] = ExDarkGray;
            style.Colors[ ImGuiCol_TitleBgCollapsed ] = ExDarkGray;

            style.Colors[ ImGuiCol_CheckMark ] = Purple;

            style.Colors[ ImGuiCol_SliderGrab ] = Gray;
            style.Colors[ ImGuiCol_SliderGrabActive ] = HPurple;

            style.Colors[ ImGuiCol_Tab ] = DarkGray;
            style.Colors[ ImGuiCol_TabUnfocused ] = DarkGray;
            style.Colors[ ImGuiCol_TabHovered ] = MedGray;
            style.Colors[ ImGuiCol_TabActive ] = MedGray;
            style.Colors[ ImGuiCol_TabUnfocusedActive ] = MedGray;

            style.Colors[ ImGuiCol_PlotHistogram ] = Purple;
            style.Colors[ ImGuiCol_PlotHistogramHovered ] = HPurple;

            style.Colors[ ImGuiCol_Button ] = MedGray;
            style.Colors[ ImGuiCol_ButtonHovered ] = LMedGray;
            style.Colors[ ImGuiCol_ButtonActive ] = MedPurple;

            style.TabRounding = 3.f;
            style.TabBorderSize = 0.f;

            style.FramePadding = ImVec2{ 14, 2 };
            style.FrameRounding = 3.f;

            style.GrabRounding = 2.f;
        }
        
        //style.WindowBorderSize = 0.0f;
        if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
        {
            style.WindowRounding = 0.0f;
        }

        //this initializes imgui for SDL
        ImGui_ImplSDL2_InitForVulkan( mainwindow.sdl_handle );

        //this initializes imgui for Vulkan
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vkbInstance.instance;
        init_info.PhysicalDevice = physDevice.physical_device;
        init_info.Device = vkbDevice.device;
        init_info.Queue = vkbDevice.get_queue( (vkb::QueueType)CYC_QUEUETYPE_GRAPHICS ).value();
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, renderPass);

        //execute a gpu command to upload imgui font textures
        ImmediateSubmit( [&](VkCommandBuffer cmd )
        {
            ImGui_ImplVulkan_CreateFontsTexture(cmd);
        });

        //clear font textures from cpu data
        
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        //add the destroy the imgui created structures
        MasterDeletionQueue.push_back([=]() {

            vkDestroyDescriptorPool( vkbDevice.device, imguiPool, nullptr );
            ImGui_ImplVulkan_Shutdown();
        });
    };

    LoadImGui();
    //Its about time to load those sexy shaders
    vector< cyc::Shader > Shaders;

    Lambda< vector<cyc::Shader>> LoadShaders([&]()
    {
        cyc::StopWatch exec;
        cyc::StartStopwatch( &exec );
        vector< cyc::Shader > shaders;
        auto lines = cyc::GetLines( "shaderload.txt" ).result;
        #ifdef SHADER_VERBOSE
        cyc_log( "Shader load file has been loaded, it has a line count of %i\n", int( lines.size() ) )
        #endif

        for ( size_t i = 0; i < lines.size(); ++i )
        {
            cyc::Shader tShader = {};
            if ( lines[ i ]._Starts_with( "#MESH" ) )
            {
                #ifdef SHADER_VERBOSE
                cyc_log( "Graphics shader found\nName: %s\nFill mode: %i\nVert shader: %s\nFrag shader: %s\n\n", lines[ i + 1 ].substr( 6 ).c_str(),
                    std::stoi( lines[ i + 2 ].substr( 6 )),
                    lines[ i + 3 ].substr( 8 ).c_str(),
                    lines[ i + 4 ].substr( 10 ).c_str() );
                #endif

                tShader.Name = lines[ i + 1 ].substr( 6 );
                tShader.Type = CYC_SHADER_GRAPHICS_MESH;
                tShader.FillMode = std::stoi( lines[ i + 2 ].substr( 6 ) );
                tShader.VertShader = lines[ i + 3 ].substr( 8 );
                tShader.FragShader = lines[ i + 4 ].substr( 10 );

                i += 4;
                shaders.push_back( tShader );
            }
            else if ( lines[ i ]._Starts_with( "#COMPUTE" ) )
            {
                #ifdef SHADER_VERBOSE
                cyc_log( "Compute shader found\nName: %s\nComp shader: %s\n\n", lines[ i + 1 ].substr( 6 ).c_str(), 
                    lines[ i + 2 ].substr( 8 ).c_str() );
                #endif

                tShader.Name = lines[ i + 1 ].substr( 6 );
                tShader.CompShader = lines[ i + 2 ].substr( 8 );

                i += 2;
                shaders.push_back( tShader );
            }
        }
        #ifdef SHADER_VERBOSE
        printf( "Shader loading complete. %i shaders loaded\n", int( shaders.size() ));
        #endif

        for ( int i = 0; i < shaders.size(); ++i )
        {
            auto o = &shaders[ i ];
            
            if ( o->Type == CYC_SHADER_GRAPHICS_MESH )
            {
                vector< string > shadload = { o->VertShader, o->FragShader };
                runtime_status |= CYC_STATUS_SHADERLOAD;
                for( auto u : shadload )
                {
                    string nO = "shaders/" + u;
                    if( !cyc::DoesFileExist( nO ) )
                    {
                        cyc_log( "uh oh! shader %s doesnt exist\n", nO.c_str() )
                        runtime_status |= CYC_STATUS_QUIT;
                        break;
                    }
                    //lets get this bread i mean file, we want to load it with the cursor at the end and in binary (beautiful)
                    std::ifstream file( nO.c_str(), std::ios::ate | std::ios::binary );
                    if( !file.is_open() )
                    {
                        cyc_log( "uh oh! shader %s failed to load\n", nO.c_str() )
                        runtime_status |= CYC_STATUS_QUIT;
                        break;
                    }

                    //because we set the cursor at the end, we can get the size if the entire file
                    size_t fileSize = (size_t)file.tellg();
                    #ifdef SHADER_VERBOSE
                    cyc_log( "Loaded shader file %s, its size is %lu in bytes\n", u.c_str(), fileSize )
                    #endif

                    //why does vulkan expect the spirv code to be in Uint32? idk, but lets give it an array large enough
                    Uint32 buffer[fileSize / sizeof(Uint32)];

                    //set the cursor at the beginning for whatever reason
                    file.seekg( 0 );

                    //lets load this bitch!
                    file.read( (char*)buffer, fileSize );

                    //were done with the file, so we can unload it
                    file.close();

                    auto moduleCreateInfo = vkinit::shader_module_create_info( );

                    moduleCreateInfo.pCode = buffer;
                    moduleCreateInfo.codeSize = fileSize;

                    VkShaderModule Module;
                    VK_CHECK(vkCreateShaderModule( vkbDevice.device, &moduleCreateInfo, nullptr, &Module ));
                    o->ShaderModules.push_back( Module );
                    #ifdef SHADER_VERBOSE
                    cyc_log( "Created shader module %p\n", Module )
                    #endif

                    o->Deletion.push_back([=]()
                    {
                        vkDestroyShaderModule( vkbDevice.device, Module, nullptr );
                        #ifdef SHADER_VERBOSE
                        cyc_log( "Destroyed shader module %p\n", Module )
                        #endif
                    });
                }
                {
                    //first, as per the standard, lets get the pipeline layout ready for use
                    auto pipelineLayoutInfo = vkinit::pipeline_layout_create_info();

                    //create our mesh constant
                    VkPushConstantRange meshPushConst{};
                    //this push constant range starts at the beginning
                    meshPushConst.offset = 0;
                    //this push constant range takes up the size of a MeshPushConstant struct
                    meshPushConst.size = sizeof( cyc::MeshPushConstant );
                    //this push constant range is accessible only in the vertex shader
                    meshPushConst.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

                    pipelineLayoutInfo.pPushConstantRanges = &meshPushConst;
                    pipelineLayoutInfo.pushConstantRangeCount = 1;

                    //HELP ME!
                    VK_CHECK(vkCreatePipelineLayout( vkbDevice.device, &pipelineLayoutInfo, nullptr, &o->PipelineLayout ));
                    #ifdef SHADER_VERBOSE
                    cyc_log( "Created pipeline layout %p\n", o->PipelineLayout )
                    #endif

                    //lets put those throbing shader modules to use~ (what??)
                    auto basicVert = o->ShaderModules[0];
                    auto basicFrag = o->ShaderModules[1];

                    //lets get the PipelineBuilder struct for sorting reasons
                    cyc::PipelineBuilder PipelineBuilder;

                    //we need to give our shaders to the pipeline in order work, obviously
                    PipelineBuilder._shaderStages.push_back(
                        vkinit::pipeline_shader_stage_create_info( VK_SHADER_STAGE_VERTEX_BIT, basicVert )
                    );
                    PipelineBuilder._shaderStages.push_back(
                        vkinit::pipeline_shader_stage_create_info( VK_SHADER_STAGE_FRAGMENT_BIT, basicFrag )
                    );

                    auto vertDesc = cyc::Vertex::GetVertexDescription();
                    PipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

                    PipelineBuilder._vertexInputInfo.pVertexAttributeDescriptions = vertDesc.attribs.data();
                    PipelineBuilder._vertexInputInfo.vertexAttributeDescriptionCount = vertDesc.attribs.size();

                    PipelineBuilder._vertexInputInfo.pVertexBindingDescriptions = vertDesc.bindings.data();
                    PipelineBuilder._vertexInputInfo.vertexBindingDescriptionCount = vertDesc.bindings.size();
                    

                    //lets set its girthyness
                    PipelineBuilder._viewport.x = 0.0f;
                    PipelineBuilder._viewport.y = 0.0f;
                    PipelineBuilder._viewport.width = (float)mainwindow.size.width;
                    PipelineBuilder._viewport.height = (float)mainwindow.size.height;
                    PipelineBuilder._viewport.minDepth = 0.0f;
                    PipelineBuilder._viewport.maxDepth = 1.0f;

                    PipelineBuilder._scissor.offset = { 0, 0 };
                    PipelineBuilder._scissor.extent = mainwindow.size;

                    //input assembly is the configuration for drawing triangle lists, strips, or individual points.
                    //we are just going to draw triangle list
                    PipelineBuilder._inputAssembly = vkinit::input_assembly_create_info( VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );

                    //configure the rasterizer to draw filled triangles
                    PipelineBuilder._rasterizer = vkinit::rasterization_state_create_info( VkPolygonMode( o->FillMode ), 1.f );

                    //we don't use multisampling, so just run the default one
                    PipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

                    //a single blend attachment with no blending and writing to RGBA
                    PipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

                    //default depthtesting
                    //PipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);
                    #ifdef SHADER_VERBOSE
                    cyc_log( "Finished filling out pipeline builder %p\n", &PipelineBuilder )
                    #endif
                    //now, we finally give the pipeline our layout. then we build it
                    PipelineBuilder._pipelineLayout = o->PipelineLayout;
                    o->Pipeline = PipelineBuilder.build_pipeline( vkbDevice.device, renderPass );
                    #ifdef SHADER_VERBOSE
                    cyc_log( "New pipeline %p\n", o->Pipeline )
                    cyc_log( "Pipeline layout %p is attributed with pipeline %p\n", o->PipelineLayout, o->Pipeline )
                    #endif
                    o->Deletion.push_back([=]()
                    {
                        vkDestroyPipelineLayout( vkbDevice.device, o->PipelineLayout, nullptr );
                        vkDestroyPipeline( vkbDevice.device, o->Pipeline, nullptr );
                        #ifdef SHADER_VERBOSE
                        cyc_log( "Destroyed pipeline layout %p\n", o->PipelineLayout )
                        cyc_log( "Destroyed pipeline %p\n", o->Pipeline )
                        #endif
                    });
                }
                runtime_status |= CYC_STATUS_SHADERLOAD;
            }
        }
        #ifdef SHADER_VERBOSE
        cyc_log( "%i Shaders built, build time was %0.1fms\n", int( shaders.size() ), cyc::CheckStopwatch( exec ).result.milliseconds )
        #endif
        return shaders;
    });

    Shaders = LoadShaders( );

    std::thread _nowaitthread( main_, &runtime_status );
    _nowaitthread.detach();
    
    cyc_log( "Beginning runtime, init sequence took %0.1fms\n\n", cyc::CheckStopwatch(starting_time).result.milliseconds )

    //==============================================SCRIPT LOADING======================================================
    //---------------------------------------------------LUA------------------------------------------------------------

    Lua_Scripts = InitLua( );

    SDL_ShowWindow( mainwindow.sdl_handle );

    //================================RUNTIME====================================

    cyc::Camera Cam;
    Cam.pos = { 0.f, 0.f, -4.f };

    float DeltaTime = 0.f;
    vector< float > DeltaTimeVec( 600 );
    for ( size_t i = 0; i < DeltaTimeVec.size(); ++i )
    {
        DeltaTimeVec[ i ] = 0.f;
    }
    vector< float > FPSVec( 600 );
    for ( size_t i = 0; i < FPSVec.size(); ++i )
    {
        FPSVec[ i ] = 0.f;
    }

    SDL_Event e;
    while ( !cyc::CompareFlags( runtime_status, CYC_STATUS_QUIT ) )
    {
        cyc::StopWatch frameDelta;
        cyc::StartStopwatch( &frameDelta );

        Keys.pulse = { false };
        Mouse.pulse = { false };
        Mouse.hold[ 3 ] = false; Mouse.hold[ 4 ] = false;
        Mouse.velocity = { 0.f, 0.f };
        glm::vec2 MovementInputs = { 0.f, 0.f };
        Clearrgba = { .0f, .0f, .0f, 1.f };

        while(SDL_PollEvent( &e ) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent( &e );
            switch( e.type )
            {
                case SDL_KEYDOWN:
                switch( e.key.keysym.sym )
                {
                    case SDLK_a : Keys.hold[ 0 ] = true; break;
                    case SDLK_b : Keys.hold[ 1 ] = true; break;
                    case SDLK_c : Keys.hold[ 2 ] = true; break;
                    case SDLK_d : Keys.hold[ 3 ] = true; break;
                    case SDLK_e : Keys.hold[ 4 ] = true; break;
                    case SDLK_f : Keys.hold[ 5 ] = true; break;
                    case SDLK_g : Keys.hold[ 6 ] = true; break;
                    case SDLK_h : Keys.hold[ 7 ] = true; break;
                    case SDLK_i : Keys.hold[ 8 ] = true; break;
                    case SDLK_j : Keys.hold[ 9 ] = true; break;
                    case SDLK_k : Keys.hold[ 10 ] = true; break;
                    case SDLK_l : Keys.hold[ 11 ] = true; break;
                    case SDLK_m : Keys.hold[ 12 ] = true; break;
                    case SDLK_n : Keys.hold[ 13 ] = true; break;
                    case SDLK_o : Keys.hold[ 14 ] = true; break;
                    case SDLK_p : Keys.hold[ 15 ] = true; break;
                    case SDLK_q : Keys.hold[ 16 ] = true; break;
                    case SDLK_r : Keys.hold[ 17 ] = true; break;
                    case SDLK_s : Keys.hold[ 18 ] = true; break;
                    case SDLK_t : Keys.hold[ 19 ] = true; break;
                    case SDLK_u : Keys.hold[ 20 ] = true; break;
                    case SDLK_v : Keys.hold[ 21 ] = true; break;
                    case SDLK_w : Keys.hold[ 22 ] = true; break;
                    case SDLK_x : Keys.hold[ 23 ] = true; break;
                    case SDLK_y : Keys.hold[ 24 ] = true; break;
                    case SDLK_z : Keys.hold[ 25 ] = true; break;
                    case SDLK_1 : Keys.hold[ 26 ] = true; break;
                    case SDLK_2 : Keys.hold[ 27 ] = true; break;
                    case SDLK_3 : Keys.hold[ 28 ] = true; break;
                    case SDLK_4 : Keys.hold[ 29 ] = true; break;
                    case SDLK_5 : Keys.hold[ 30 ] = true; break;
                    case SDLK_6 : Keys.hold[ 31 ] = true; break;
                    case SDLK_7 : Keys.hold[ 32 ] = true; break;
                    case SDLK_8 : Keys.hold[ 33 ] = true; break;
                    case SDLK_9 : Keys.hold[ 34 ] = true; break;
                    case SDLK_SPACE : Keys.hold[ 35 ] = true; break;
                    case SDLK_ESCAPE : Keys.hold[ 36 ] = true; break;
                    case SDLK_LALT : Keys.hold[ 37 ] = true; break;
                    case SDLK_LCTRL : Keys.hold[ 38 ] = true; break;
                    case SDLK_LSHIFT : Keys.hold[ 39 ] = true; break;
                    case SDLK_CAPSLOCK : Keys.hold[ 40 ] = true; break;
                    case SDLK_TAB : Keys.hold[ 41 ] = true; break;
                    case SDLK_BACKQUOTE : Keys.hold[ 42 ] = true; break;
                    case SDLK_COMMA : Keys.hold[ 43 ] = true; break;
                    case SDLK_PERIOD : Keys.hold[ 44 ] = true; break;
                    case SDLK_RALT : Keys.hold[ 45 ] = true; break;
                    case SDLK_RCTRL : Keys.hold[ 46 ] = true; break;
                    case SDLK_RSHIFT : Keys.hold[ 47 ] = true; break;
                    case SDLK_SLASH : Keys.hold[ 48 ] = true; break;
                    case SDLK_SEMICOLON : Keys.hold[ 49 ] = true; break;
                    case SDLK_QUOTE : Keys.hold[ 50 ] = true; break;
                    case SDLK_RETURN : Keys.hold[ 51 ] = true; break;
                    case SDLK_LEFTBRACKET : Keys.hold[ 52 ] = true; break;
                    case SDLK_RIGHTBRACKET : Keys.hold[ 53 ] = true; break;
                    case SDLK_BACKSLASH : Keys.hold[ 54 ] = true; break;
                    case SDLK_BACKSPACE : Keys.hold[ 55 ] = true; break;
                    case SDLK_EQUALS : Keys.hold[ 56 ] = true; break;
                    case SDLK_MINUS : Keys.hold[ 57 ] = true; break;
                    case SDLK_F1 : Keys.hold[ 58 ] = true; break;
                    case SDLK_F2 : Keys.hold[ 59 ] = true; break;
                    case SDLK_F3 : Keys.hold[ 60 ] = true; break;
                    case SDLK_F4 : Keys.hold[ 61 ] = true; break;
                    case SDLK_F5 : Keys.hold[ 62 ] = true; break;
                    case SDLK_F6 : Keys.hold[ 63 ] = true; break;
                    case SDLK_F7 : Keys.hold[ 64 ] = true; break;
                    case SDLK_F8 : Keys.hold[ 65 ] = true; break;
                    case SDLK_F9 : Keys.hold[ 66 ] = true; break;
                    case SDLK_F10: Keys.hold[ 67 ] = true; break;
                    case SDLK_F11: Keys.hold[ 68 ] = true; break;
                    case SDLK_F12: Keys.hold[ 69 ] = true; break;
                    case SDLK_LEFT : Keys.hold[ 70 ] = true; break;
                    case SDLK_RIGHT : Keys.hold[ 71 ] = true; break;
                    case SDLK_UP : Keys.hold[ 72 ] = true; break;
                    case SDLK_DOWN : Keys.hold[ 73 ] = true; break;
                    case SDLK_INSERT : Keys.hold[ 74 ] = true; break;
                    case SDLK_HOME : Keys.hold[ 75 ] = true; break;
                    case SDLK_DELETE : Keys.hold[ 76 ] = true; break;
                    case SDLK_END : Keys.hold[ 77 ] = true; break;
                    case SDLK_PAGEUP : Keys.hold[ 78 ] = true; break;
                    case SDLK_PAGEDOWN : Keys.hold[ 79 ] = true; break;
                    case SDLK_F13 : Keys.hold[ 80 ] = true; break;
                    case SDLK_F14 : Keys.hold[ 81 ] = true; break;
                    case SDLK_F15 : Keys.hold[ 82 ] = true; break;
                }
                break;
                case SDL_KEYUP:
                switch( e.key.keysym.sym )
                {
                    case SDLK_a : Keys.hold[ 0 ] = false; break;
                    case SDLK_b : Keys.hold[ 1 ] = false; break;
                    case SDLK_c : Keys.hold[ 2 ] = false; break;
                    case SDLK_d : Keys.hold[ 3 ] = false; break;
                    case SDLK_e : Keys.hold[ 4 ] = false; break;
                    case SDLK_f : Keys.hold[ 5 ] = false; break;
                    case SDLK_g : Keys.hold[ 6 ] = false; break;
                    case SDLK_h : Keys.hold[ 7 ] = false; break;
                    case SDLK_i : Keys.hold[ 8 ] = false; break;
                    case SDLK_j : Keys.hold[ 9 ] = false; break;
                    case SDLK_k : Keys.hold[ 10 ] = false; break;
                    case SDLK_l : Keys.hold[ 11 ] = false; break;
                    case SDLK_m : Keys.hold[ 12 ] = false; break;
                    case SDLK_n : Keys.hold[ 13 ] = false; break;
                    case SDLK_o : Keys.hold[ 14 ] = false; break;
                    case SDLK_p : Keys.hold[ 15 ] = false; break;
                    case SDLK_q : Keys.hold[ 16 ] = false; break;
                    case SDLK_r : Keys.hold[ 17 ] = false; break;
                    case SDLK_s : Keys.hold[ 18 ] = false; break;
                    case SDLK_t : Keys.hold[ 19 ] = false; break;
                    case SDLK_u : Keys.hold[ 20 ] = false; break;
                    case SDLK_v : Keys.hold[ 21 ] = false; break;
                    case SDLK_w : Keys.hold[ 22 ] = false; break;
                    case SDLK_x : Keys.hold[ 23 ] = false; break;
                    case SDLK_y : Keys.hold[ 24 ] = false; break;
                    case SDLK_z : Keys.hold[ 25 ] = false; break;
                    case SDLK_1 : Keys.hold[ 26 ] = false; break;
                    case SDLK_2 : Keys.hold[ 27 ] = false; break;
                    case SDLK_3 : Keys.hold[ 28 ] = false; break;
                    case SDLK_4 : Keys.hold[ 29 ] = false; break;
                    case SDLK_5 : Keys.hold[ 30 ] = false; break;
                    case SDLK_6 : Keys.hold[ 31 ] = false; break;
                    case SDLK_7 : Keys.hold[ 32 ] = false; break;
                    case SDLK_8 : Keys.hold[ 33 ] = false; break;
                    case SDLK_9 : Keys.hold[ 34 ] = false; break;
                    case SDLK_SPACE : Keys.hold[ 35 ] = false; break;
                    case SDLK_ESCAPE : Keys.hold[ 36 ] = false; break;
                    case SDLK_LALT : Keys.hold[ 37 ] = false; break;
                    case SDLK_LCTRL : Keys.hold[ 38 ] = false; break;
                    case SDLK_LSHIFT : Keys.hold[ 39 ] = false; break;
                    case SDLK_CAPSLOCK : Keys.hold[ 40 ] = false; break;
                    case SDLK_TAB : Keys.hold[ 41 ] = false; break;
                    case SDLK_BACKQUOTE : Keys.hold[ 42 ] = false; break;
                    case SDLK_COMMA : Keys.hold[ 43 ] = false; break;
                    case SDLK_PERIOD : Keys.hold[ 44 ] = false; break;
                    case SDLK_RALT : Keys.hold[ 45 ] = false; break;
                    case SDLK_RCTRL : Keys.hold[ 46 ] = false; break;
                    case SDLK_RSHIFT : Keys.hold[ 47 ] = false; break;
                    case SDLK_SLASH : Keys.hold[ 48 ] = false; break;
                    case SDLK_SEMICOLON : Keys.hold[ 49 ] = false; break;
                    case SDLK_QUOTE : Keys.hold[ 50 ] = false; break;
                    case SDLK_RETURN : Keys.hold[ 51 ] = false; break;
                    case SDLK_LEFTBRACKET : Keys.hold[ 52 ] = false; break;
                    case SDLK_RIGHTBRACKET : Keys.hold[ 53 ] = false; break;
                    case SDLK_BACKSLASH : Keys.hold[ 54 ] = false; break;
                    case SDLK_BACKSPACE : Keys.hold[ 55 ] = false; break;
                    case SDLK_EQUALS : Keys.hold[ 56 ] = false; break;
                    case SDLK_MINUS : Keys.hold[ 57 ] = false; break;
                    case SDLK_F1 : Keys.hold[ 58 ] = false; break;
                    case SDLK_F2 : Keys.hold[ 59 ] = false; break;
                    case SDLK_F3 : Keys.hold[ 60 ] = false; break;
                    case SDLK_F4 : Keys.hold[ 61 ] = false; break;
                    case SDLK_F5 : Keys.hold[ 62 ] = false; break;
                    case SDLK_F6 : Keys.hold[ 63 ] = false; break;
                    case SDLK_F7 : Keys.hold[ 64 ] = false; break;
                    case SDLK_F8 : Keys.hold[ 65 ] = false; break;
                    case SDLK_F9 : Keys.hold[ 66 ] = false; break;
                    case SDLK_F10: Keys.hold[ 67 ] = false; break;
                    case SDLK_F11: Keys.hold[ 68 ] = false; break;
                    case SDLK_F12: Keys.hold[ 69 ] = false; break;
                    case SDLK_LEFT : Keys.hold[ 70 ] = false; break;
                    case SDLK_RIGHT : Keys.hold[ 71 ] = false; break;
                    case SDLK_UP : Keys.hold[ 72 ] = false; break;
                    case SDLK_DOWN : Keys.hold[ 73 ] = false; break;
                    case SDLK_INSERT : Keys.hold[ 74 ] = false; break;
                    case SDLK_HOME : Keys.hold[ 75 ] = false; break;
                    case SDLK_DELETE : Keys.hold[ 76 ] = false; break;
                    case SDLK_END : Keys.hold[ 77 ] = false; break;
                    case SDLK_PAGEUP : Keys.hold[ 78 ] = false; break;
                    case SDLK_PAGEDOWN : Keys.hold[ 79 ] = false; break;
                    case SDLK_F13 : Keys.hold[ 80 ] = false; break;
                    case SDLK_F14 : Keys.hold[ 81 ] = false; break;
                    case SDLK_F15 : Keys.hold[ 82 ] = false; break;
                }
                break;
                case SDL_MOUSEBUTTONDOWN:
                switch( e.button.button )
                {
                    case SDL_BUTTON_LEFT: Mouse.hold[ 0 ] = true; break;
                    case SDL_BUTTON_RIGHT: Mouse.hold[ 1 ] = true; break;
                    case SDL_BUTTON_MIDDLE: Mouse.hold[ 2 ] = true; break;
                }
                break;
                case SDL_MOUSEBUTTONUP:
                switch( e.button.button )
                {
                    case SDL_BUTTON_LEFT: Mouse.hold[ 0 ] = false; break;
                    case SDL_BUTTON_RIGHT: Mouse.hold[ 1 ] = false; break;
                    case SDL_BUTTON_MIDDLE: Mouse.hold[ 2 ] = false; break;
                }
                break;
                case SDL_MOUSEWHEEL:
                if( e.wheel.y > 0 )
                {
                    Mouse.hold[ CYCM_WHEELUP ] = true;
                } 
                else if( e.wheel.y < 0 )
                {
                    Mouse.hold[ CYCM_WHEELDOWN ] = true;
                }
                break;
                case SDL_MOUSEMOTION:
                Mouse.velocity = { e.motion.xrel, e.motion.yrel };
                break;
            }
            switch( e.window.event )
            {
                case SDL_WINDOWEVENT_MINIMIZED:
                    //cyc_log( "Window minimized\n" );
                break;
                case SDL_WINDOWEVENT_RESIZED:
                    waitAllFences( frames );
                    mainwindow.size.width = e.window.data1;
                    mainwindow.size.height = e.window.data2;
                    cyc::RqueueUse( SwapchainDeletionQueue );
                    SwapchainDeletionQueue.clear();
                    cyc::RCqueueUse( &Shaders[ 0 ].Deletion );


                    (swipychain)(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);
                    Shaders = LoadShaders();
                break;
            }
            if ( e.type == SDL_QUIT )//&& !( Keys.hold[ CYCK_LALT ] && Keys.hold[ CYCK_F4 ] ) )
            {
                runtime_status |= CYC_STATUS_QUIT;
                break;
            }
        }
        //=====================================PRE-LOGIC=========================================
        for( int i = 0; i < Keys.hold.size(); ++i )
        {
            Keys.pulse[ i ] = Keys.hold[ i ] && !Keys.holdCpy[ i ];
        }
        for( int i = 0; i < Mouse.hold.size(); ++i )
        {
            Mouse.pulse[ i ] = Mouse.hold[ i ] && !Mouse.holdCpy[ i ];
        }
        if ( Keys.pulse[ CYCK_F1 ] )
        {
            for ( auto o : Lua_Scripts )
            {
                lua_close( o.L );
            }
            Lua_Scripts.clear( );
            Lua_Scripts = InitLua( );
        }
        //===================================LUA SCRIPT UPDATING==============================

        for ( auto u : Lua_Scripts )
        {
            auto o = &u;

            lua_getglobal( o->L, "Update" );
            if ( lua_isfunction( o->L, -1 ) )
            {
                lua_pushnumber( o->L, DeltaTime );
                if ( !cyc::CheckLua( o->L, lua_pcall( o->L, 1, 0, 0)) )
                {
                    #ifdef SCRIPT_VERBOSE
                    cyc_log( "Lua err: Error when calling \'Update\'\n" );
                    #endif
                }
            }
        }
        if ( cyc::CompareFlags( WindowScriptStats, 1 ) )
        {
            WindowScriptStats &= !1;
            SDL_SetWindowSize( mainwindow.sdl_handle, mainwindow.size.width, mainwindow.size.height );
            waitAllFences( frames );
            //mainwindow.size.width = e.window.data1;
            //mainwindow.size.height = e.window.data2;
            cyc::RqueueUse( SwapchainDeletionQueue );
            SwapchainDeletionQueue.clear();
            cyc::RCqueueUse( &Shaders[ 0 ].Deletion );


            (swipychain)(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);
            Shaders = LoadShaders();
        }
        if ( cyc::CompareFlags( WindowScriptStats, 2 << 0 ) )
        {   
            WindowScriptStats &= !2 << 0;
            SDL_MinimizeWindow( mainwindow.sdl_handle );
        }
        if ( cyc::CompareFlags( WindowScriptStats, 2 << 1 ) )
        {
            WindowScriptStats &= !2 << 1;
            SDL_MaximizeWindow( mainwindow.sdl_handle );
            waitAllFences( frames );
            //mainwindow.size.width = e.window.data1;
            //mainwindow.size.height = e.window.data2;
            cyc::RqueueUse( SwapchainDeletionQueue );
            SwapchainDeletionQueue.clear();
            cyc::RCqueueUse( &Shaders[ 0 ].Deletion );


            (swipychain)(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);
            Shaders = LoadShaders();
        }
        if ( cyc::CompareFlags( WindowScriptStats, 2 << 2 ) )
        {   
            WindowScriptStats &= !2 << 2;
            SDL_RestoreWindow( mainwindow.sdl_handle );
        }
        
        //=======================================LOGIC===========================================
        //cyc::CheckStopwatch(frame_delta).result.milliseconds;

        if ( ( SDL_GetWindowFlags( mainwindow.sdl_handle ) & SDL_WINDOW_MINIMIZED ) == SDL_WINDOW_MINIMIZED )
        {
            DeltaTime = cyc::CheckStopwatch(frameDelta).result.milliseconds;
            Keys.holdCpy = Keys.hold;
            Mouse.holdCpy = Mouse.hold;

            ++FRAME_NUMBER;
            continue;
        }

        DeltaTimeVec.insert( DeltaTimeVec.begin(), DeltaTime );
        DeltaTimeVec.erase( DeltaTimeVec.begin() + ( DeltaTimeVec.size() - 1 ) );

        FPSVec.insert( FPSVec.begin(), 1000.f / DeltaTime );
        FPSVec.erase( FPSVec.begin() + ( FPSVec.size() - 1 ) );
        
        
        //=====================================RENDERING=========================================
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame( mainwindow.sdl_handle );

        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        //=====================================IMGUI LOGIC=======================================
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
            ImGui::SetNextWindowSizeConstraints( ImVec2( 100, 400 ), ImVec2( 800, 4000 ) );

            ImGui::Begin( "Content", NULL, window_flags );

            ImGuiStyle& style = ImGui::GetStyle();

            {
                auto TreeNodeFlags = ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_AllowItemOverlap;
                auto t_open = ImGui::TreeNodeEx( "Your Mother", TreeNodeFlags );

                auto contentRegionAvail = ImGui::GetContentRegionAvail();
                ImGui::SameLine( ImGui::GetWindowWidth() - 41.f, 0.f );
                if ( ImGui::Button( "*" ) )
                    cyc_log( "HELP ME\n" );

                if ( t_open )
                {
                    char Hbuf[ 512 ];

                    ImGui::Indent();
                    ImGui::InputTextWithHint( "Name", "Filter or smth", Hbuf, sizeof( Hbuf ), ImGuiInputTextFlags_AllowTabInput );

                    /*vector< float > x;
                    for ( size_t i = 0; i < DeltaTimeVec.size(); ++i )
                    {
                        x.push_back( (float)i * 2 );
                    }

                    ImPlot::SetNextAxesLimits( 0, DeltaTimeVec.size(), 0, 255 );
                    ImPlotFlags plotFlags = ImPlotFlags_NoInputs;
                    if ( ImPlot::BeginPlot( "Frame times/FPS", "Frames", "", ImVec2( 0, 0 ), plotFlags, ImPlotAxisFlags_NoTickLabels ) )
                    {
                        ImPlot::PlotLine( "Delta Time", x.data(), DeltaTimeVec.data(), DeltaTimeVec.size() );
                        ImPlot::PlotLine( "FPS", x.data(), FPSVec.data(), FPSVec.size() );

                        ImPlot::EndPlot();
                    }*/
                    
                    ImGui::Unindent();
                }
            }
            ImGui::Spacing();
            
            BeginGreenButton;

            if ( ImGui::Button( "Push me" ) )
            {
                cyc_log( "He pushed me!\n" );
            }
            ImGui::SameLine( );
            if ( ImGui::Button( "Exit" ) )
            {
                runtime_status |= CYC_STATUS_QUIT;
            }

            ImGui::PopStyleColor( 3 );

            ImGui::End();
        }
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground; //| ImGuiWindowFlags_NoDecoration;
            //ImGui::SetNextWindowSizeConstraints( ImVec2( 100, 400 ), ImVec2( 800, 4000 ) );

            ImGui::SetNextWindowBgAlpha( 0.f );

            ImGui::Begin( "Viewport", NULL, window_flags );
            
            BeginGreenButton;

            if ( ImGui::Button( "Kill me" ) )
                runtime_status |= CYC_STATUS_QUIT;

            ImGui::PopStyleColor( 3 );

            ImGui::End();

        }
        
        

        ImGui::Render();
        //Frame data of the current frame
        auto curFrame = &frames[ GET_FRAME() ];
        //Wait for the fences so we stay synced with the GPU, then reset the fence so it can be used again
        VK_CHECK(vkWaitForFences( vkbDevice.device, 1, &curFrame->render_fence, true, FRAME_WAIT ));
        VK_CHECK(vkResetFences( vkbDevice.device, 1, &curFrame->render_fence ));

        //Get the index of the swapchain thing that we are in, then (i think) signal the present semaphore of our current frame
        Uint32 swpchainIndex = 0U;
        VK_CHECK(vkAcquireNextImageKHR( vkbDevice.device, swpchain.swapchain, FRAME_WAIT, curFrame->present_semaphore, nullptr, &swpchainIndex ));

        //Because we KNOW that our command buffer is no longer in use, we will reset it
        VK_CHECK( vkResetCommandBuffer( curFrame->cmdBuf, NULL ) );

        //A cmdbuffer with our current frame command buffer. Just for ease of use
        auto cmd = curFrame->cmdBuf;

        //Lets get our command buffer all lubed up. the cmd buffer will only be submitted once, at the end of the frame, so lets tell vulkan that
        auto cmdBeginInfo = vkinit::command_buffer_begin_info( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
        VK_CHECK( vkBeginCommandBuffer( cmd, &cmdBeginInfo ) );

        //Call the Render functions in the lua scripts
        for ( auto u : Lua_Scripts )
        {
            auto o = &u;

            lua_getglobal( o->L, "Render" );
            if ( lua_isfunction( o->L, -1 ) )
            {
                if ( !cyc::CheckLua( o->L, lua_pcall( o->L, 0, 0, 0)) )
                printf( "Lua err: Error when calling \'Render\'\n" );
            }
        }

        VkClearValue clearColor;

        clearColor.color = { Clearrgba.r, Clearrgba.g, Clearrgba.b, Clearrgba.a };


        //Now its time to stretch out the render pass for use, lets give it our current frame buffer using the swapchain image index
        auto rpBeginInfo = vkinit::renderpass_begin_info( renderPass, mainwindow.size, frameBufs[swpchainIndex] );
        //set our clear value
        rpBeginInfo.pClearValues = &clearColor;
        

        //Lets get this show on the road by attributing our renderpass to the command buffer. idk what the whole subpass stuff means
        vkCmdBeginRenderPass( cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

        //Now that weve given the renderpass to the cmd buffer we can now begin giving commands!
        //=====================================RENDERPASS STARTS HERE IDIOT========================================
        auto ThirdFillPipe = Shaders[ 0 ].Pipeline;
        auto ThirdFillPipeLayout = Shaders[ 0 ].PipelineLayout;

        //lets see if it works (it probably wont lmao)
        //first we bind the pipeline to the command buffer, of course its a graphics type, so lets specify
        vkCmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ThirdFillPipe );
        
        using glm::radians; using glm::vec3;
        /*
        Cam.pos += glm::vec3( MovementInputs.x * -sin(radians(Cam.rotation.x)) * cos(radians(Cam.rotation.y)) + MovementInputs.y * -cos(radians(Cam.rotation.x)),
            MovementInputs.x * -sin(radians(Cam.rotation.y)),
            MovementInputs.x * cos(radians(Cam.rotation.x)) * cos(radians(Cam.rotation.y)) + MovementInputs.y * -sin(radians(Cam.rotation.x)) );

        glm::mat4 view = glm::lookAt( Cam.pos, Cam.pos + glm::vec3(-sin(radians(Cam.rotation.x)) * cos(radians(Cam.rotation.y)),
            -sin(radians(Cam.rotation.y)),
            cos(radians(Cam.rotation.x) ) * cos(radians(Cam.rotation.y) )),
        glm::vec3(-sin(radians(Cam.rotation.x)) * sin(radians(Cam.rotation.y)),
            cos(radians(Cam.rotation.y)),
            cos(radians(Cam.rotation.x)) * sin(radians(Cam.rotation.y)) ));
        */

        //camera projection
        glm::mat4 projection = glm::perspective(radians(Cam.FOV), (float)mainwindow.size.width / (float)mainwindow.size.height, Cam.cuttof.x, Cam.cuttof.y);
        projection[1][1] *= -1;

        for( size_t i = 0; i < MeshKeys.size(); ++i )
        {
            auto o = cyc::UnorderedGet( &Meshes, MeshKeys[ i ] );
            auto u = o.x;
            
            if ( !o.x.render )
                continue;
            VkBuffer buf = u._vertexBuffer._buffer;
            VkDeviceSize offset = 0;
            //time to bind this massive dong i mean triangle
            vkCmdBindVertexBuffers( cmd, 0, 1, &buf, &offset );
            float pi = 3.141592654f;

            //model translation
            glm::mat4 model = glm::translate( glm::mat4( 1.f ), -u.position );

            model = glm::rotate( model, u.rotation.x*pi/2, vec3( 1.f, 0.f, 0.f ) );
            model = glm::rotate( model, u.rotation.y*pi/2, vec3( 0.f, 1.f, 0.f ) );
            model = glm::rotate( model, u.rotation.z*pi/2, vec3( 0.f, 0.f, 1.f ) );
            //model scale
            model = glm::scale( model, u.scale );
            
            //calculate final mesh matrix
            glm::mat4 mesh_matrix = projection * glm::mat4( 1.f ) * model;

            cyc::MeshPushConstant constants;
            constants.renderMatrix = mesh_matrix;

            vkCmdPushConstants( cmd, ThirdFillPipeLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( cyc::MeshPushConstant ), &constants );

            vkCmdDraw( cmd, u.vertices.size(), 1, 0, i );
        }
        
        ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), cmd );
        //======================================RENDERPASS ENDS HERE IDIOT=========================================
        vkCmdEndRenderPass( cmd );

        //Now that we are done with the command buffer, its time to finish
        VK_CHECK(vkEndCommandBuffer( cmd ));
        
        //Yay command submittion queue time!
        //We want the commands to wait for the present semaphore to finish up, then signal the render semaphore to become active
        auto submitInfo = vkinit::submit_info( &cmd );
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &curFrame->present_semaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &curFrame->render_semaphore;

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = &waitStage;
        GQueueSubmits.push_back( submitInfo );

        //We now have our submit info, so lets submit our command buffer to the queue and execute it, it will then block the render semaphore, sexy
        VkQueue GQueue = vkbDevice.get_queue( (vkb::QueueType)CYC_QUEUETYPE_GRAPHICS).value();

        VK_CHECK( vkQueueSubmit( GQueue, GQueueSubmits.size(), GQueueSubmits.data(), curFrame->render_fence ) );
        GQueueSubmits.clear();
        //cyc_log("uhhh %p\n", renderPass)

        //Its presentation time, we have our rendered stuff, so we can now present it to the swapchain, hope it likes it...
        VkPresentInfoKHR presentInfo = vkinit::present_info();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swpchain.swapchain;

        //We want it to wait for the render semaphore
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &curFrame->render_semaphore;

        //We also want to give it the image index of the swapchain that we are using
        presentInfo.pImageIndices = &swpchainIndex;

        //Lets finally present our image! Show them what a gaping hole we left~ lmao
        VK_CHECK(vkQueuePresentKHR( GQueue, &presentInfo ));

        DeltaTime = cyc::CheckStopwatch(frameDelta).result.milliseconds;
        Keys.holdCpy = Keys.hold;
        Mouse.holdCpy = Mouse.hold;

        ++FRAME_NUMBER;

        ImGui::EndFrame();
        //ImGui::UpdatePlatformWindows();

    }
    
    //============================END OF RUNTIME=================================
    
    cyc_log( "Cyclone is shutting down... code = %lu\n", runtime_status )
    cyc_log( "Runtime duration was %0.2f minutes\n", cyc::CheckStopwatch(stopwatch).result.minutes )

    waitAllFences( frames );

    size_t keys_s = MeshKeys.size();
    for ( size_t i = 0; i < keys_s; ++i )
    {
        //cyc::UnorderedGet( &Meshes, MeshKeys[ i ] ).d( &Meshes, &MeshKeys );
    }
    Meshes.clear();
    MeshKeys.clear();

    //ImPlot::DestroyContext( );
    //ImGui::DestroyContext( );
    
    cyc::RqueueUse( SwapchainDeletionQueue );
    vkDestroySwapchainKHR( vkbDevice.device, swpchain.swapchain, nullptr );

    int w, h;
    SDL_GetWindowSize( mainwindow.sdl_handle, &w, &h );
    string wh = string_format( "%i\n%i\n", w, h );

    SDL_GetWindowPosition( mainwindow.sdl_handle, &w, &h );
    bool maxed = false;
    if ( cyc::CompareFlags( SDL_GetWindowFlags( mainwindow.sdl_handle ), SDL_WINDOW_MAXIMIZED ))
        maxed = true;
    wh += string_format( "%i\n%i\n%s\n%i", w, h, config[4].c_str(), maxed );
    cyc::WriteFile( "config.txt", wh );
    
    cyc::RCqueueUse( &Shaders[ 0 ].Deletion );

    cyc::RqueueUse( MasterDeletionQueue );
    
    SDL_Quit();

    system( "pause" );
    return EXIT_SUCCESS;
}
