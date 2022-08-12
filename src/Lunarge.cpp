#include "Lunarge.h"
#include <iostream>
#include <filesystem>


void __cdecl lunar::WaitMS(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds (milliseconds) );
}
void __cdecl lunar::WaitMCS(Uint32 microseconds)
{
    std::this_thread::sleep_for((std::chrono::microseconds)microseconds);
}
lunar::Lresult<lunar::SteadyTimePoint> __cdecl lunar::StartStopwatch(StopWatch *timer)
{
    Lresult<SteadyTimePoint> res;
    timer->start_time = std::chrono::high_resolution_clock::now();

    res.result = timer->start_time;
    res.message = "Stopwatch started";
    return res;
}
lunar::Lresult<lunar::times> __cdecl lunar::CheckStopwatch(StopWatch timer, SteadyTimePoint comparitor)
{
    Lresult<lunar::times> res;

    res.result.microseconds = (float)std::chrono::duration_cast<std::chrono::microseconds>(comparitor - timer.start_time).count();
    res.result.milliseconds =  res.result.microseconds/1000.f;
    res.result.seconds = res.result.milliseconds/1000.f;
    res.result.minutes = res.result.seconds/60.f;
    res.result.hours = res.result.minutes/60.f;

    res.message = "Stopwatch checked";
    return res;
}
lunar::Lresult<lunar::times> __cdecl lunar::PauseStopwatch(StopWatch *timer)
{
    Lresult<lunar::times> res;

    timer->pause_time = std::chrono::high_resolution_clock::now();

    res.result.microseconds = (float)std::chrono::duration_cast<std::chrono::microseconds>(timer->pause_time - timer->start_time).count();
    res.result.milliseconds =  res.result.microseconds/1000.f;
    res.result.seconds = res.result.milliseconds/1000.f;
    res.result.minutes = res.result.seconds/60.f;
    res.result.hours = res.result.minutes/60.f;

    res.message = "Stopwatch paused";
    return res;
}
lunar::Lresult<void*> __cdecl lunar::ResetStopwatch(StopWatch *timer)
{
    Lresult<void*> res;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return res;
}

bool __cdecl lunar::CompareFlags(uint32_t lflag, uint32_t lcompare)
{
    return (lflag & lcompare) == lcompare;
}

bool __cdecl lunar::DoesFileExist(string path)
{
    return std::filesystem::exists(path);
}
lunar::Lresult<string> __cdecl lunar::ReadFile(string path)
{
    Lresult<string> res;
    if (!DoesFileExist(path))
    {
        res.error_code = LUNAR_ERROR_FILE_DOESNT_EXIST;
        res.message = "Path: " + path + " does not exist";
        return res;
    }
    //opens an in file stream operation using the 'path' string
    std::ifstream t(path);
    //creates a stringstream buffer to hold the file data
    std::stringstream buffer;
    //reads the ifstream buffer data (file data) and pushes it into the stringstream buffer
    buffer << t.rdbuf();
    //converts the stringstream buffer into a string, then returns it
    res.result = buffer.str();
    res.message = "File [ " + path + " ] has been opened";
    return res;
}
lunar::Lresult<void*> __cdecl lunar::WriteFile(string path, string contents) {
    Lresult<void*> res;
    std::ofstream outFile(path);
    if (outFile.is_open())
    {
        outFile << contents;
        res.message = string_format( "File %s opened and written\n", path.c_str() );
    }
    else
    {
        res.message = "Failed to open file [ " + path + " ]";
        outFile.close();
    }
    return res;
}
lunar::Lresult<void*> __cdecl lunar::AppendFile(string path, string addition)
{
    Lresult<void*> res;
    if (!DoesFileExist(path))
    {
        res.error_code = LUNAR_ERROR_FILE_DOESNT_EXIST;
        res.message = "Path: " + path + " does not exist";
        return res;
    }
    std::ofstream outfile;
    outfile.open(path, std::ios_base::app);
    if ( outfile.is_open() )
        outfile << addition;
    else
    {
        res.message = "Failed to open file [ " + path + " ]";
        outfile.close();
    }
    return res;
}
lunar::Lresult<vector<string>> __cdecl lunar::GetFiles(string path, string extention_filter) {
    Lresult<vector<string>> result;

    using std::filesystem::directory_iterator;
    vector<string> goodFiles;
    //A for loop that iterates through all the files in the file path
    if (!DoesFileExist(path))
    {
        result.error_code = LUNAR_ERROR_FILE_DOESNT_EXIST;
        result.message = "Path: " + path + " does not exist";
        return result;
    }
    for (const auto& file : directory_iterator(path)) {
        string help = file.path().string();
        //checks if the file extention is txt
        if (help.substr(help.length() - extention_filter.length(), extention_filter.length()) == extention_filter) {
            //Pushes the now string file path of the file into goodFiles
            goodFiles.push_back(help);
        }
    }
    result.result = goodFiles;
    result.message = string(""+goodFiles.size()) + " Files have been found with extention [ " + extention_filter + " ] in path [ " + path + " ]";
    return result;
}
lunar::Lresult<vector<string>> __cdecl lunar::GetLines(string path)
{
    Lresult<vector<string>> result;

    vector<string> got;
    if ( !DoesFileExist(path) )
    {
        result.error_code = LUNAR_ERROR_FILE_DOESNT_EXIST;
        result.message = "Path: " + path + " does not exist";
        return result;
    }
    std::ifstream input(path);

    for (string line; getline(input, line);)
    {
        got.push_back(line);
    }
    result.result = got;
    return result;
}
lunar::Lresult<string> __cdecl lunar::GetLine(string path, int line)
{
    Lresult<string> res;

    std::ifstream input(path);

    int a = 1;
    for (string strres; getline(input, strres);)
    {
        if (a == line)
        {
            res.result = strres;
            res.message = string("Line found at index [ " + a) + " ] for file [ " + path + " ]"; 
            return res;
        }
        ++a;
    }
    res.result = "";
    res.message = string("Failed to get line [ " + line) + " ] for file + [ " + path + " ]" ; 
    return res;
}
string __cdecl lunar::GetCurrentDir()
{
    return std::filesystem::current_path().u8string();
}

VkPipeline lunar::PipelineBuilder::build_pipeline(VkDevice device, VkRenderPass pass) {
	//make viewport state from our stored viewport and scissor.
	//at the moment we won't support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;

	viewportState.viewportCount = 1;
	viewportState.pViewports = &_viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &_scissor;

	//setup dummy color blending. We aren't using transparent objects yet
	//the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;

	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &_colorBlendAttachment;

	//build the actual pipeline
	//we now use all of the info structs we have been writing into into this one to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.pNext = nullptr;

	pipelineInfo.stageCount = _shaderStages.size();
	pipelineInfo.pStages = _shaderStages.data();
	pipelineInfo.pVertexInputState = &_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &_inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &_rasterizer;
	pipelineInfo.pMultisampleState = &_multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = pass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	//pipelineInfo.pDepthStencilState = &_depthStencil;

	//it's easy to error out on create graphics pipeline, so we handle it a bit better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(
		device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
		std::cout << "failed to create pipeline\n";
		return VK_NULL_HANDLE; // failed to create graphics pipeline
	}
	else
	{
		return newPipeline;
	}
}
