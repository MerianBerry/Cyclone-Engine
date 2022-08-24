#pragma once
#include "VkBootstrap.h"

#include "SDL.h"
#include "SDL_vulkan.h"
#include "glm.hpp"
#include "vulkan.h"
#include "math.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <array>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <functional>
#include "gtx/transform.hpp"

#include "vk_mem_alloc.h"


#undef main

#define CYC_POLL_APPLICATION while(!Lunar::getQuitStatus()) {
#define CYC_POLL_EVENTS(x, y) while(SDL_PollEvent(&x) != 0) { if(x.type == SDL_QUIT) Lunar::setQuitStatus(); if(y.status == CYC_STATUS_EVENT_IDLE) break;
#define CYC_POLL_EVENTS_END }
#define CYC_POLL_APPLICATION_END Lunar::advance_frame();}


#define CYC_WINDOW_SIZE_DEFAULT { 960, 540 }

#define CYC_COLOR_ATTATCHMENT_DEFAULT NULL, (VkFormat)0, VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR

#define pi 3.141592654f
#define pi2 6.2821853072f

#define CYC_KEYCODE key.keysym.sym
#define CYC_BUTTONCODE button.button


enum CYC_ERROR
{
    CYC_ERROR_SUCCESS = 0U,
    CYC_ERROR_SDL_INIT_ERROR = 1U,
    CYC_ERROR_FILE_DOESNT_EXIST = 2U
};
enum CYC_TIME
{
    CYC_TIME_MILLISECONDS = 0U,
    CYC_TIME_SECONDS = 1U,
    CYC_TIME_MINUTES = 2U
};
enum CYC_QUEUETYPE
{
    CYC_QUEUETYPE_GRAPHICS = (Uint32)vkb::QueueType::graphics,
    CYC_QUEUETYPE_COMPUTE = (Uint32)vkb::QueueType::compute,
    CYC_QUEUETYPE_TRANSFER = (Uint32)vkb::QueueType::transfer,
    CYC_QUEUETYPE_PRESENT = (Uint32)vkb::QueueType::present
};
enum CYC_CMDBUFFER
{
    CYC_CMDBUFFER_LEVEL_PRIMARY = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    CYC_CMDBUFFER_LEVEL_SECONDARY = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
    CYC_CMDBUFFER_LEVEL_MAX_ENUM = VK_COMMAND_BUFFER_LEVEL_MAX_ENUM
};
enum CYC_PRESENT_MODE
{
    CYC_PRESENT_MODE_VSYNC = VK_PRESENT_MODE_FIFO_KHR,
    CYC_PRESENT_MODE_IMMEDIATE = VK_PRESENT_MODE_IMMEDIATE_KHR
};
enum CYC_WINDOW
{
    CYC_WINDOW_FLAGS_DEFUALT = SDL_WINDOW_RESIZABLE,
    CYC_WINDOW_FLAGS_NONE = 0U
};
enum CYC_STATUS
{
    CYC_STATUS_OK = 0U,
    CYC_STATUS_QUIT = 1U,
    CYC_STATUS_MINIMIZED = 2U,
    CYC_STATUS_IDLE = 4U,
    CYC_STATUS_SHADERLOAD = 8U
};
enum CYC_PIPELINE
{
    CYC_PIPELINE_BIND_POINT_GRAPHICS = 0U,
    CYC_PIPELINE_BIND_POINT_COMPUTE = 1U,
    CYC_PIPELINE_BIND_POINT_MAX_ENUM = 2147483647U,
    CYC_PIPELINE_BIND_POINT_RAY_TRACING_KHR = 1000165000U
};
enum CYC_SHADER
{
    CYC_SHADER_GRAPHICS = 0U,
    CYC_SHADER_GRAPHICS_MESH = 1U,
    CYC_SHADER_COMPUTE = 2U
};
enum LUANR_KEY
{
    CYCK_A = 0U,
    CYCK_B = 1U,
    CYCK_C = 2U,
    CYCK_D = 3U,
    CYCK_E = 4U,
    CYCK_F = 5U,
    CYCK_G = 6U,
    CYCK_H = 7U,
    CYCK_I = 8U,
    CYCK_J = 9U,
    CYCK_K = 10U,
    CYCK_L = 11U,
    CYCK_M = 12U,
    CYCK_N = 13U,
    CYCK_O = 14U,
    CYCK_P = 15U,
    CYCK_Q = 16U,
    CYCK_R = 17U,
    CYCK_S = 18U,
    CYCK_T = 19U,
    CYCK_U = 20U,
    CYCK_V = 21U,
    CYCK_W = 22U,
    CYCK_X = 23U,
    CYCK_Y = 24U,
    CYCK_Z = 25U,
    CYCK_1 = 26U,
    CYCK_2 = 27U,
    CYCK_3 = 28U,
    CYCK_4 = 29U,
    CYCK_5 = 30U,
    CYCK_6 = 31U,
    CYCK_7 = 32U,
    CYCK_8 = 33U,
    CYCK_9 = 34U,
    CYCK_SPACE = 35U,
    CYCK_ESC = 36U,
    CYCK_LALT = 37U,
    CYCK_LCTRL = 38U,
    CYCK_LSHIFT = 39U,
    CYCK_CAPSLOCK = 40U,
    CYCK_TAB = 41U,
    CYCK_BACKQUOTE = 42U,
    CYCK_COMMA = 43U,
    CYCK_PERIOD = 44U,
    CYCK_RALT = 45U,
    CYCK_RCTRL = 46U,
    CYCK_RSHIFT = 47U,
    CYCK_SLASH = 48U,
    CYCK_SEMICOLON = 49U,
    CYCK_QUOTE = 50U,
    CYCK_ENTER = 51U,
    CYCK_LBRACKET = 52U,
    CYCK_RBRACKET = 53U,
    CYCK_BACKSLASH = 54U,
    CYCK_BACKSPACE = 55U,
    CYCK_EQUALS = 56U,
    CYCK_DASH = 57U,
    CYCK_F1 = 58U,
    CYCK_F2 = 59U,
    CYCK_F3 = 60U,
    CYCK_F4 = 61U,
    CYCK_F5 = 62U,
    CYCK_F6 = 63U,
    CYCK_F7 = 64U,
    CYCK_F8 = 65U,
    CYCK_F9 = 66U,
    CYCK_F10 = 67U,
    CYCK_F11 = 68U,
    CYCK_F12 = 69U,
    CYCK_LEFT = 70U,
    CYCK_RIGHT = 71U,
    CYCK_UP = 72U,
    CYCK_DOWN = 73U,
    CYCK_INSERT = 74U,
    CYCK_HOME = 75U,
    CYCK_DELETE = 76U,
    CYCK_END = 77U,
    CYCK_PAGEUP = 78U,
    CYCK_PAGEDOWN = 79U,
    CYCK_F13 = 80U,
    CYCK_F14 = 81U,
    CYCK_F15 = 82U
};
enum CYC_MOUSE
{
    CYCM_LEFT = 0U,
    CYCM_RIGHT = 1U,
    CYCM_MIDDLE = 2U,
    CYCM_WHEELUP = 3U,
    CYCM_WHEELDOWN = 4U
};