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

using lunar::Lambda;

constexpr Uint32 FRAME_WAIT = std::numeric_limits<Uint32>::max();
constexpr Uint8 FRAME_OVERLAP = 2;
Uint64 FRAME_NUMBER = 0;

Uint8 GET_FRAME()
{
    return FRAME_NUMBER % 2;
}

void labort()
{
    lunar::WaitMS(5000);
    abort();
}

int main()
{
    lunar::StopWatch starting_time;
    lunar::StartStopwatch( &starting_time );
    Uint32 runtime_status = LUNAR_STATUS_IDLE;

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
    if( config.size() == 0 )
    {
        lunar::WriteFile( "config.txt", string_format("%i\n%i\n%i\n%i\nLunarge", 960, 540, 0, 0) );
        config = lunar::GetLines("config.txt").result;
    }
    mainwindow.size.width = stoi(config[0]);
    mainwindow.size.height = stoi(config[1]);
    mainwindow.pos.x = stoi( config[2] ) == 0 ? SDL_WINDOWPOS_CENTERED : stoi( config[2] );
    mainwindow.pos.y = stoi( config[3] ) == 0 ? SDL_WINDOWPOS_CENTERED : stoi( config[3] );
    mainwindow.title = config[4].c_str();
    
    mainwindow.sdl_handle = SDL_CreateWindow(mainwindow.title,
    mainwindow.pos.x,
    mainwindow.pos.y,
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

    std::function<void(VkRenderPass*, vector<VkFramebuffer>*, lunar::swapchain*, lunar::Window*, vkb::Swapchain*)> swipychain([=](VkRenderPass *rdpass, vector<VkFramebuffer> *fmrbufs, lunar::swapchain *swapchain, lunar::Window *window, vkb::Swapchain* swph){
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

    std::function<void(vector<lunar::frameData>)> waitAllFences([=](vector<lunar::frameData> Frames)
    {
        VkFence allfences[FRAME_OVERLAP];
        for( int i = 0; i < FRAME_OVERLAP; ++i )
        {
            allfences[i] = Frames[i].render_fence;
        }
        VK_CHECK(vkWaitForFences( vkbDevice.device, FRAME_OVERLAP, allfences, true, FRAME_WAIT ));
    });
    swipychain(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);


    lunar_log( "Swapchain created in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds )
    lunar::ResetStopwatch(&stopwatch);

    vector<lunar::frameData> frames ( FRAME_OVERLAP );
    
    VkSemaphoreCreateInfo renderSemaphoreInfo = vkinit::semaphore_create_info();
    VkSemaphoreCreateInfo presentSemaphoreInfo = vkinit::semaphore_create_info();

    VkFenceCreateInfo fenceInfo = vkinit::fence_create_info( VK_FENCE_CREATE_SIGNALED_BIT );
    VkCommandPoolCreateInfo cmdPoolInfo = vkinit::command_pool_create_info( (Uint32)LUNAR_QUEUETYPE_GRAPHICS, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

    for ( int i = 0; i < frames.size(); ++i )
    {
        VK_CHECK(vkCreateCommandPool( vkbDevice.device, &cmdPoolInfo, nullptr, &frames[i].cmdPool ));
        auto cmdAllocInfo = vkinit::command_buffer_allocate_info( frames[i].cmdPool );
        VK_CHECK(vkAllocateCommandBuffers( vkbDevice.device, &cmdAllocInfo, &frames[i].cmdBuf ));

        VK_CHECK(vkCreateSemaphore( vkbDevice.device, &renderSemaphoreInfo, nullptr, &frames[i].render_semaphore ));
        VK_CHECK(vkCreateSemaphore( vkbDevice.device, &presentSemaphoreInfo, nullptr, &frames[i].present_semaphore ));
        VK_CHECK(vkCreateFence( vkbDevice.device, &fenceInfo, nullptr, &frames[i].render_fence ));
    }

    MasterDeletionQueue.push_back([=]()
    {
        for( auto o : frames )
        {
            vkDestroyFence( vkbDevice.device, o.render_fence, nullptr );
            vkDestroySemaphore( vkbDevice.device, o.render_semaphore, nullptr );
            vkDestroySemaphore( vkbDevice.device, o.present_semaphore, nullptr );

            vkDestroyCommandPool( vkbDevice.device, o.cmdPool, nullptr );
        }

        SDL_DestroyWindow( mainwindow.sdl_handle );
        vkDestroySurfaceKHR( vkb_inst.instance, mainwindow.surface, nullptr );
        vkb::destroy_device( vkbDevice );
        //lunar_log( "Destroyed vulkan (vkb) device :)\n" )
    });

    //Its about time to load those sexy shaders
    vector<string> shadersToLoad = { "basicVert.spv", "basicFrag.spv" };
    vector<VkShaderModule> shadersLoaded;

    //omg a pipeline *flush*
    VkPipelineLayout trianglePipelineLayout;
    VkPipeline trianglePipeline;

    runtime_status |= LUNAR_STATUS_SHADERLOAD;
    for( auto o : shadersToLoad )
    {
        string nO = "shaders/" + o;
        if( !lunar::DoesFileExist( nO ) )
        {
            lunar_log( "uh oh! shader %s doesnt exist\n", nO.c_str() )
            runtime_status |= LUNAR_STATUS_QUIT;
            break;
        }
        //lets get this bread i mean file, we want to load it with the cursor at the end and in binary
        std::ifstream file( nO.c_str(), std::ios::ate | std::ios::binary );
        if( !file.is_open() )
        {
            lunar_log( "uh oh! shader %s failed to load\n", nO.c_str() )
            runtime_status |= LUNAR_STATUS_QUIT;
            break;
        }

        //because we set the cursor at the end, we can get the size if the entire file
        size_t fileSize = (size_t)file.tellg();

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
        shadersLoaded.push_back( Module );
        //lunar_log( "our good friend %p\n", Module );

        MasterDeletionQueue.push_back([=]()
        {
            vkDestroyShaderModule( vkbDevice.device, Module, nullptr );
        });
    }
    runtime_status &= LUNAR_STATUS_SHADERLOAD;
    lunar_log("Shader modules created in %0.1fms\n", lunar::CheckStopwatch( stopwatch ).result.milliseconds)
    lunar::ResetStopwatch( &stopwatch );
    
    lunar::Lambda< void, vector<VkShaderModule>, VkPipelineLayout*, VkPipeline* > createPipelines( [&]( vector<VkShaderModule> shaders, VkPipelineLayout* pipelineLayout, VkPipeline *pipeline )
    {
        //first, as per the standard, lets get the pipeline layout ready for use
        auto pipelineLayoutInfo = vkinit::pipeline_layout_create_info();
        VK_CHECK(vkCreatePipelineLayout( vkbDevice.device, &pipelineLayoutInfo, nullptr, pipelineLayout ));

        //Im getting turned on already! lets put those throbing shader modules to use~
        auto basicVert = shadersLoaded[0];
        auto basicFrag = shadersLoaded[1];

        //lets build this cock! (what am i even saying)
        lunar::PipelineBuilder PipelineBuilder;

        //we need to give our pipeline our shaders to work, obviously
        PipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info( VK_SHADER_STAGE_VERTEX_BIT, basicVert )
        );
        PipelineBuilder._shaderStages.push_back(
            vkinit::pipeline_shader_stage_create_info( VK_SHADER_STAGE_FRAGMENT_BIT, basicFrag )
        );

        PipelineBuilder._vertexInputInfo = vkinit::vertex_input_state_create_info();

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
        PipelineBuilder._rasterizer = vkinit::rasterization_state_create_info(VK_POLYGON_MODE_FILL);

        //we don't use multisampling, so just run the default one
        PipelineBuilder._multisampling = vkinit::multisampling_state_create_info();

        //a single blend attachment with no blending and writing to RGBA
        PipelineBuilder._colorBlendAttachment = vkinit::color_blend_attachment_state();

        //default depthtesting
        //PipelineBuilder._depthStencil = vkinit::depth_stencil_create_info(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

        //now, we finally give the pipeline our layout. then we build it
        PipelineBuilder._pipelineLayout = trianglePipelineLayout;
        *pipeline = PipelineBuilder.build_pipeline( vkbDevice.device, renderPass );
        SwapchainDeletionQueue.push_back([=]()
        {
            vkDestroyPipelineLayout( vkbDevice.device, *pipelineLayout, nullptr );
            vkDestroyPipeline( vkbDevice.device, *pipeline, nullptr );
        });
    });
    createPipelines( shadersLoaded, &trianglePipelineLayout, &trianglePipeline );
    lunar_log( "Pipelines created in %0.1fms\n", lunar::CheckStopwatch(stopwatch).result.milliseconds )
    lunar::ResetStopwatch( &stopwatch );
    
    lunar_log( "Beggining runtime, init sequence took %0.1fms\n\n", lunar::CheckStopwatch(starting_time).result.milliseconds )
    //================================RUNTIME====================================
    std::unordered_map<string, bool> windowthings;

    SDL_Event e;
    
    while (!lunar::CompareFlags(runtime_status, LUNAR_STATUS_QUIT))
    {
        lunar::StopWatch frameDelta;
        lunar::StartStopwatch(&frameDelta);
        while(SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                runtime_status |= LUNAR_STATUS_QUIT;
                break;
            }
            switch( e.button.button )
            {
                case SDL_BUTTON_LEFT:
                    if( e.type == SDL_MOUSEBUTTONDOWN )
                    windowthings["Left mouse"] = true;
                    else if( e.type == SDL_MOUSEBUTTONUP )
                    windowthings["Left mouse"] = false;
                break;
                case SDL_BUTTON_RIGHT:
                    if( e.type == SDL_MOUSEBUTTONDOWN )
                    windowthings["Right mouse"] = true;
                    else if( e.type == SDL_MOUSEBUTTONUP )
                    windowthings["Right mouse"] = false;
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
                case SDL_WINDOWEVENT_MINIMIZED:
                    //lunar_log( "Window minimized\n" );
                    while ( lunar::CompareFlags( SDL_GetWindowFlags( mainwindow.sdl_handle ), SDL_WINDOW_MINIMIZED ))
                    {
                        SDL_PollEvent( &e );
                        lunar::WaitMS( 5 );
                    }
                break;
                case SDL_WINDOWEVENT_RESIZED:
                    lunar::StopWatch swpchainrecreation;
                    lunar::StartStopwatch( &swpchainrecreation );
                    waitAllFences( frames );
                    mainwindow.size.width = e.window.data1;
                    mainwindow.size.height = e.window.data2;
                    lunar::RqueueUse(SwapchainDeletionQueue);
                    SwapchainDeletionQueue.clear();
                    (swipychain)(&renderPass, &frameBufs, &swpchain, &mainwindow, &vkbSwapchain);
                    createPipelines( shadersLoaded, &trianglePipelineLayout, &trianglePipeline );
                    //lunar_log("uhhh %lu\n", vkbSwapchain.get_image_views().value().size())
                    //lunar_log("uhhh %lu\n", swpchain.image_views.size())
                    //lunar_log("uhhh %lu\n", SwapchainDeletionQueue.size())
                    //lunar_log("Swapchain re-created in %0.1fms! | w: %lu, h: %lu\n", lunar::CheckStopwatch( swpchainrecreation ).result.milliseconds, e.window.data1, e.window.data2)
            }
        }
        //lunar::CheckStopwatch(frame_delta).result.milliseconds;

        #if 1
        //Frame data of the current frame
        

        auto curFrame = &frames[GET_FRAME()];
        //Wait for the fences so we stay synced with the GPU, then reset the fence so it can be used again
        VK_CHECK(vkWaitForFences( vkbDevice.device, 1, &curFrame->render_fence, true, FRAME_WAIT ));
        VK_CHECK(vkResetFences( vkbDevice.device, 1, &curFrame->render_fence ));

        //Get the index of the swapchain thing that we are in, then (i think) signal the present semaphore of our current frame
        Uint32 swpchainIndex = 0U;
        VK_CHECK(vkAcquireNextImageKHR( vkbDevice.device, swpchain.swapchain, FRAME_WAIT, curFrame->present_semaphore, nullptr, &swpchainIndex ));

        //Because we KNOW that our command buffer is no longer in use, we will reset it
        VK_CHECK(vkResetCommandBuffer( curFrame->cmdBuf, NULL ));

        //A variable with our current frame command buffer. Just for ease of use
        auto cmd = curFrame->cmdBuf;

        //Lets get our command buffer all lubed up. the cmd buffer will only be submitted once, at the end of the frame, so lets tell vulkan that
        auto cmdBeginInfo = vkinit::command_buffer_begin_info( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );
        VK_CHECK(vkBeginCommandBuffer( cmd, &cmdBeginInfo ));

        VkClearValue clearColor;
        glm::vec4 rgba = { .2f, .2f, .2f, 1.f };
        if( (*windowthings.find("Left mouse")).second )
            rgba.r += .3f;

        if( (*windowthings.find("Right mouse")).second )
            rgba.b += .3f;

        clearColor.color = { rgba.r, rgba.g, rgba.b, rgba.a };


        //Now its time to stretch out the render pass for use, lets give it our current frame buffer using the swapchain image index
        auto rpBeginInfo = vkinit::renderpass_begin_info( renderPass, mainwindow.size, frameBufs[swpchainIndex] );
        //set our clear value
        rpBeginInfo.pClearValues = &clearColor;
        

        //Lets get this show on the road by attributing our renderpass to the command buffer. idk what the whole subpass stuff means
        vkCmdBeginRenderPass( cmd, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE );

        //Now that weve given the renderpass to the cmd buffer we can now begin giving commands!
        //=====================================RENDERPASS STARTS HERE IDIOT========================================

        //lets see if it works (it probably wont lmao)
        //first we bind the pipeline to the command buffer, of course its a graphics type, so lets specify
        vkCmdBindPipeline( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeline );
        vkCmdDraw( cmd, 3, 1, 0, 0 );
        //wtf it worked

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

        //We now have our submit info, so lets submit our command buffer to the queue and execute it, it will then block the render semaphore, sexy
        VkQueue Queue = vkbDevice.get_queue((vkb::QueueType)LUNAR_QUEUETYPE_GRAPHICS).value();

        VK_CHECK(vkQueueSubmit( Queue, 1, &submitInfo, curFrame->render_fence ));
        //lunar_log("uhhh %p\n", renderPass)

        //Lets present this bitch, we have our rendered stuff, so we can now present it to the swapchain, hope it likes it...
        VkPresentInfoKHR presentInfo = vkinit::present_info();
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swpchain.swapchain;

        //We want it to wait for the render semaphore
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &curFrame->render_semaphore;

        //We also want to give it the image index of the swapchain that we are using
        presentInfo.pImageIndices = &swpchainIndex;

        //Lets finally present our image! Show them what a gaping hole we left~ lmao
        VK_CHECK(vkQueuePresentKHR( Queue, &presentInfo ));

        //lunar::CheckStopwatch(frameDelta).result.milliseconds;

        ++FRAME_NUMBER;
        #else
        lunar::WaitMS(17);
        #endif
    }

    //string hc[] = {"hello", "goodbye"};
    //string hg[] = {"screw you", "sdfoiajd"};
    //string hd[4];
    //memcpy(hd, hc, sizeof(hc));
    //memcpy(&hd[2], hg, sizeof(hg));
    
    //============================END OF RUNTIME=================================
    
    lunar_log( "Lunarge is shutting down... code = %lu\n", runtime_status )
    lunar_log( "Runtime duration was %0.2f minutes\n", lunar::CheckStopwatch(stopwatch).result.minutes )

    waitAllFences( frames );
    
    lunar::RqueueUse( SwapchainDeletionQueue );
    vkDestroySwapchainKHR( vkbDevice.device, swpchain.swapchain, nullptr );

    int w, h;
    SDL_GetWindowSize( mainwindow.sdl_handle, &w, &h );
    string wh = string_format( "%i\n%i\n", w, h );

    SDL_GetWindowPosition( mainwindow.sdl_handle, &w, &h );
    wh += string_format( "%i\n%i\n%s", w, h, config[4].c_str() );
    lunar::WriteFile( "config.txt", wh );
    
    lunar::RqueueUse( MasterDeletionQueue );
    
    SDL_Quit();

    string ending;
    std::getline( std::cin, ending );
    return EXIT_SUCCESS;
}
