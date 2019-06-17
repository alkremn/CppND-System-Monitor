#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:

std::string ProcessParser::getCmd(std::string pid)
{
    std::string path = Path::basePath + pid + Path::cmdPath;
    std::string line;
    
    Util::getStream(path, stream);
    if(stream.is_open()){
        std::getline(stream, line);
        return line;
    }
}

std::vector<std::string> ProcessParser::getPidList()
{
    std::string base_path = Path::basePath();
    std::vector<std::string> pid_list;
    DIR * dr
    dirent* drnt;
    if((dr = opendir(base_path.c_str())) != NULL)
    {
        while((drnt = readdir(dr)) != NULL)
        {
            if(dr->d_type != DT_DIR)
                continue;

            try
            {
                std::stoi(drnt->d_name);
            }
            catch(std::invalid_argument& e)
            {
                continue;
            }
            pid_list.push_back(drnt->d_name);
        }
        if(closedir(dr))
            throw std::runtime_error(std::strerror(errno));
        
        return pid_list;
    }
}

std::string ProcessParser::getVmSize(std::string pid)
{
    std::string status_path = Path::basePath() + pid + Path::statusPath();
    std::string vm_data = "VmData";

    Util::getStream(status_path, stream);
    if(stream.is_open())
    {
        for(std::string line; std::getline(stream, line); )
        {
            if(line.compare(0, vm_data.size(), vm_data) == 0)
            {
                std::istringstream buf(line);
                std::istream_iterator<std::string> beg(buf), end;
                std::vector<std::string> values(beg, end);
                float data = std::stof(values[1]) / (1024*1024);
                return std::to_string(data);
            }
        }
    }
}

std::string ProcessParser::getCpuPercent(std::string pid)
{
    std::string stat_path = Path::basePath() + pid +"/"+ Path::statPath();
    Util::getStream(paht, stream);
    
    if(stream.is_open()){
        std::string line;
        std::getline(stream, line);

        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> values(beg, end);

        float utime = std::stof(ProcessParser::getProcUpTime(pid));

        float stime = std::stof(values[14]);
        float cutime = std::stof(values[15]);
        float cstime = std::stof(values[16]);
        float starttime = std::stof(values[21]);
        
        float uptime = ProcessParser::getProcUpTime(pid);
        
        float freq = sysconf(_SC_CLK_TCK);
        
        float total_time = utime + stime + cutime + cstime;
        float seconds = uptime - (starttime / seconds);
        
        return to_string(100.0 * (total_time/ freq / seconds));
    }
    return "";
}

std::string ProcessParser::getProcUpTime(std::string pid)
{
    std::string path = Path::basePath + pid + "/" + Path::statPath();
    Util::getStream(path, stream);
    if(stream.is_open()){
        std::string line;
        std::getline(stream, line);

        std::istringstream buf(line);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> values(beg, end);

        return to_string(float(std::stof(values[13])/sysconf(_SC_CLK_TCK)));
    }
    return "";
}


long int ProcessParser::getSysUpTime()
{
    std::string path = Path::basePath() + Path::upTimePath();
    std::string line;

    Util::getStream(path, stream);
    std::getline(stream, line);

    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf), end;
    std::vector<std::string> values(beg, end);
    
    return std::stoi(values[0]);
}

std::string ProcessParser::getProcUser(std::string pid)
{
    std::string name = "Uid:";
    std::string path = Path::basePath() + pid + Path::statusPath();
    Util::getStream(path, stream);
    std::string line;
    std::string user_id;

    while(std::getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values(beg, end);
            user_id = values[1];
            break;
        }
    }
    Util::getStream("/etc/passwd", stream);

    name = "x:" + user_id;
    while(std::getline(line))
    {
        if(line.find(name) != std::string::npos)
        {
            return line.substr(0, line.find(":"));
        }
    }
    return "";
}

int ProcessParser::getNumberOfCores()
{
    std::string line;
    std::string name = "cpu cores";
    std::string path = Path::basePath + "cpuinfo";
    Util::getStream(path, stream);
    
    if(stream.is_open())
    {
        while(std::getline(stream, line))
        {
            if(line.compare(0, name.size(), name) == 0)
            {
                std::istringstream buf(line);
                std::istream_iterator<std::string> beg(buf), end;
                std::vector<std::string> values(beg, end);
                return stoi(values[3]);
            }
        }
    }
    return 0;
}
std::vector<std::string> ProcessParser::getSysCpuPercent(std::string coreNumber)
{
    std::string line;
    std::string name = "cpu" + coreNumber;
    std::string path = Path::basePath() + Path::statPath();
    Util::getStream(path, stream);
    if(stream.is_open())
    {
        while (std::getline(stream, line)) 
        {
            if (line.compare(0, name.size(),name) == 0) {
                std::istringstream buf(line);
                std::istream_iterator<string> beg(buf), end;
                std::vector<std::string> values(beg, end);
                
                return values;
            }
        }
    }
    return (std::vector<string>());
}
 int ProcessParser::getNumberOfCores()
 {
    std::string path = Path::basePath() + "cpuinfo";
    std::string cpu_cores = "cpu cores";
    Util::getStream(path, stream);
    if(stream.is_open())
    {
        while(std::getline(stream, line))
        {
            if(line.compare(0, cpu_cures.size(), cpu_cores) == 0)
            {
                std::istringstream buf(line);
                std::istream_iterator<std::string> beg(buf), end;
                std::vector<std::string> values(beg, end);
                return std::stoi(values[3]);
            }
        }
    }
 }
 float getSysActiveCpuTime(std::vector<std::string> values)
{
    return (std::stof(values[S_USER]) +
            std::stof(values[S_NICE]) +
            std::stof(values[S_SYSTEM]) +
            std::stof(values[S_IRQ]) +
            std::stof(values[S_SOFTIRQ]) +
            std::stof(values[S_STEAL]) +
            std::stof(values[S_GUEST]) +
            std::stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(std::vector<std::string>values)
{
    return (std::stof(values[S_IDLE]) + std::stof(values[S_IOWAIT]));
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2)
{
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(value1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

float ProcessParser::getSysRamPercent()
{
    std::string path = Path::basePath() + Path::memInfoPath();
    std::string name1 = "MemAvailable:";
    std::string name2 = "MemFree:";
    std::string name3 = "Buffers:";
    
    float totalMem = 0;
    float freeMem = 0;
    float buffers = 0;

    Util::getStream(path, stream);

    std::string line;
    while(std::getline(stream, line))
    {
        if(totalMem != 0 && freeMem != 0)
            break;

        if(line.compare(0, name1.size(), name1) == 0)
        {
            std::istringsream buf(line);
            std::isream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values(beg, end);
            totalMem = std::stof(values[1]);
        }

        if(line.compare(0, name2.size(), name2) == 0)
        {
            std::istringsream buf(line);
            std::isream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values(beg, end);
            freeMem = std::stof(values[1]);
        }

        if(line.compare(0, name3.size(), name3) == 0)
        {
            std::istringsream buf(line);
            std::isream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values(beg, end);
            buffers = std::stof(values[1]);
        }
    }
    return float(100.0 * (1 - (freeMme/(totalMem - buffers))));
}

std::string ProcessParser::getSysKernelVersion()
{
    std::string path = Path::basePath() + Path::versionPath();
    std::string name = "Linux version ";
    std::string line;
    
    Util::getStream(path, stream);
    while(std::getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator beg(buf), end;
            std::vector<std::string> values(beg, end);
            return values[2];
        }
    }
    return "";
}

std::string ProcessParser::getOSName()
{
    std::string name = "PRETTY_NAME=";
    std::path = "/etc/os-release";
    std::string line;
    
    Util::getStream(path, stream);

    while(std::getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0)
        {
            std::size_t found = line.find("=");
            found++;
            std::string result = line.substr(found);
            result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
            return result;
        }
    }
    return "";
}

int ProcessParser::getTotalThreads()
{
    std::string name = "Threads:";
    std::vector<std::string> list = ProcessParser::getPidList();
    std::string line;
    int result = 0;
    
    for (int i=0 ; i<list.size();i++) 
    {
        std::string pid = list[i];

        std::path = Path::basePath() + pid + Path::statusPath();
        Util::getStream(path, stream);
        while (std::getline(stream, line)) 
        {
            if (line.compare(0, name.size(), name) == 0) {
                std::istringstream buf(line);
                std::istream_iterator<std::string> beg(buf), end;
                std::vector<std::string> values(beg, end);
                result += std::stoi(values[1]);
                break;
            }
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses()
{
    std::string path = Path::basePath() + Path::statPath();
    std::string name = "processes";
    std:string line;
    int result = 0;

    Util::getStream(path, stream);
    while(std::getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0) 
        {
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values (beg, end);
            result += std::stoi(values[1]);
            break;
        }
    }
    return result;
}

int ProcessParser::getNumberOfRunningProcesses() 
{
    std::string path = Path::basePath() + Path::statPath();
    std::string name = "procs_running";
    std:string line;
    int result = 0;

    Util::getStream(path, stream);
    while(std::getline(stream, line))
    {
        if(line.compare(0, name.size(), name) == 0) 
        {
            std::istringstream buf(line);
            std::istream_iterator<std::string> beg(buf), end;
            std::vector<std::string> values (beg, end);
            result += std::stoi(values[1]);
            break;
        }
    }
    return result;
}