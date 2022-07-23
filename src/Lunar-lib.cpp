#include "Lunar-lib.h"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>

void __cdecl lunar::WaitMS(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds (milliseconds) );
}
void __cdecl lunar::WaitMCS(Uint32 microseconds)
{
    std::this_thread::sleep_for((std::chrono::microseconds)microseconds);
}
lunar::Lresult<std::chrono::steady_clock::time_point> __cdecl lunar::StartStopwatch(timer *timer)
{
    Lresult<std::chrono::steady_clock::time_point> res;
    timer->start_time = std::chrono::high_resolution_clock::now();

    res.result = timer->start_time;
    res.message = "Stopwatch started";
    return res;
}
lunar::Lresult<lunar::times> __cdecl lunar::CheckStopwatch(timer *timer, std::chrono::steady_clock::time_point comparitor)
{
    Lresult<lunar::times> res;

    res.result.milliseconds =  std::chrono::duration_cast<std::chrono::milliseconds>(comparitor - timer->start_time).count()/1.f;
    res.result.seconds =  std::chrono::duration_cast<std::chrono::milliseconds>(comparitor - timer->start_time).count()/1000.f;
    res.result.minutes =  std::chrono::duration_cast<std::chrono::milliseconds>(comparitor - timer->start_time).count()/60000.f;
    res.result.hours = std::chrono::duration_cast<std::chrono::milliseconds>(comparitor - timer->start_time).count()/3600000.f;

    res.message = "Stopwatch checked";
    return res;
}
lunar::Lresult<lunar::times> __cdecl lunar::PauseStopwatch(timer *timer)
{
    Lresult<lunar::times> res;

    timer->pause_time = std::chrono::high_resolution_clock::now();

    res.result.milliseconds =  std::chrono::duration_cast<std::chrono::milliseconds>(timer->pause_time - timer->start_time).count()/1.f;
    res.result.seconds =  std::chrono::duration_cast<std::chrono::milliseconds>(timer->pause_time - timer->start_time).count()/1000.f;
    res.result.minutes =  std::chrono::duration_cast<std::chrono::milliseconds>(timer->pause_time - timer->start_time).count()/60000.f;
    res.result.hours = std::chrono::duration_cast<std::chrono::milliseconds>(timer->pause_time - timer->start_time).count()/3600000.f;

    res.message = "Stopwatch paused";
    return res;
}
lunar::Lresult<void> __cdecl lunar::ResetStopwatch(timer *timer)
{
    Lresult<void> res;
    timer->start_time = std::chrono::high_resolution_clock::now();
    return res;
}

bool __cdecl lunar::CompareFlags(uint32_t lflag, uint32_t lcompare)
{
    return (lflag & lcompare) == lcompare;
}

lunar::Lresult<string> __cdecl lunar::ReadFile(string path)
{
    Lresult<string> result;
    //opens an in file stream operation using the 'path' string
    std::ifstream t(path);
    //creates a stringstream buffer to hold the file data
    std::stringstream buffer;
    //reads the ifstream buffer data (file data) and pushes it into the stringstream buffer
    buffer << t.rdbuf();
    //converts the stringstream buffer into a string, then returns it
    result.result = buffer.str();

    result.message = "File [ " + path + " ] has been opened";
    return result;
}
template<class T>
lunar::Lresult<void> __cdecl lunar::WriteFile(string path, T contents) {
    std::ofstream outFile(path);
    if (outFile.is_open())
        outFile << contents;
    else
        outFile.close();
}
lunar::Lresult<vector<string>> __cdecl lunar::GetFiles(string path, string extention_filter) {
    Lresult<vector<string>> result;

    using std::filesystem::directory_iterator;
    vector<string> goodFiles;
    //A for loop that iterates through all the files in the file path
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
string __cdecl lunar::CurrentPath()
{
    return std::filesystem::current_path().u8string();
}