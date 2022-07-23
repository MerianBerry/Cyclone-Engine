#pragma once
#include "VkBootstrap.h"

#include "SDL.h"
#include "SDL_vulkan.h"
#include "glm.hpp" 
#include "vulkan.h"
#include "math.h"
#include <vector>
#include <chrono>
#include <thread>
#include <functional>


#undef main

#define LUNAR_POLL_APPLICATION while(!Lunar::getQuitStatus()) {
#define LUNAR_POLL_EVENTS(x, y) while(SDL_PollEvent(&x) != 0) { if(x.type == SDL_QUIT) Lunar::setQuitStatus(); if(y.status == LUNAR_STATUS_EVENT_IDLE) break;
#define LUNAR_POLL_EVENTS_END }
#define LUNAR_POLL_APPLICATION_END Lunar::advance_frame();}


#define LUNAR_WINDOW_SIZE_DEFAULT { 960, 540 }

#define LUNAR_COLOR_ATTATCHMENT_DEFAULT NULL, (VkFormat)0, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

#define pi 3.141592654f
#define pi2 6.2821853072f

#define LUNAR_KEYCODE key.keysym.sym
#define LUNAR_BUTTONCODE button.button


enum LUNAR_ERROR
{
    LUNAR_ERROR_SUCCESS = 0U,
    LUNAR_ERROR_SDL_INIT_ERROR = 1U
};
enum LUNAR_TIME
{
    LUNAR_TIME_MILLISECONDS = 0U,
    LUNAR_TIME_SECONDS = 1U,
    LUNAR_TIME_MINUTES = 2U
};
enum LUNAR_QUEUETYPE
{
    LUNAR_QUEUETYPE_GRAPHICS = (Uint32)vkb::QueueType::graphics,
    LUNAR_QUEUETYPE_COMPUTE = (Uint32)vkb::QueueType::compute,
    LUNAR_QUEUETYPE_TRANSFER = (Uint32)vkb::QueueType::transfer,
    LUNAR_QUEUETYPE_PRESENT = (Uint32)vkb::QueueType::present
};
enum LUNAR_CMDBUFFER
{
    LUNAR_CMDBUFFER_LEVEL_PRIMARY = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    LUNAR_CMDBUFFER_LEVEL_SECONDARY = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
    LUNAR_CMDBUFFER_LEVEL_MAX_ENUM = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM
};
enum LUNAR_PRESENT_MODE
{
    LUNAR_PRESENT_MODE_VSYNC = VK_PRESENT_MODE_FIFO_KHR,
    LUNAR_PRESENT_MODE_IMMEDIATE = VK_PRESENT_MODE_IMMEDIATE_KHR
};
enum LUNAR_WINDOW
{
    LUNAR_WINDOW_FLAGS_DEFUALT = SDL_WINDOW_RESIZABLE,
    LUNAR_WINDOW_FLAGS_NONE = 0U
};
enum LUNAR_STATUS
{
    LUNAR_STATUS_OK = 0U,
    LUNAR_STATUS_QUIT = 1U,
    LUNAR_STATUS_MINIMIZED = 2U,
    LUNAR_STATUS_EVENT_IDLE = 4U,
    LUNAR_STATUS_IDLE = 8U,
    LUNAR_STATUS_RENDERING = 16U,
    LUNAR_STATUS_AWAITING_RENDERING = 32U
};
enum LUNAR_PIPELINE
{
    LUNAR_PIPELINE_BIND_POINT_GRAPHICS = 0U,
    LUNAR_PIPELINE_BIND_POINT_COMPUTE = 1U,
    LUNAR_PIPELINE_BIND_POINT_MAX_ENUM = 2147483647U,
    LUNAR_PIPELINE_BIND_POINT_RAY_TRACING_KHR = 1000165000U
};