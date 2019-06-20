#include <string>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
private:
    std::string pid;
    std::string user;
    std::string cmd;
    std::string cpu;
    std::string mem;
    std::string upTime;

public:
    Process(std::string pid)
    {
        this->pid = pid;
        this->user = ProcessParser::getProcUser(pid);
        this->cmd = ProcessParser::getCmd(pid);
        this->cpu = ProcessParser::getCpuPercent(pid);
        this->mem = ProcessParser::getVmSize(pid);
        this->upTime = ProcessParser::getProcUpTime(pid);
    }
    void setPid(int pid);
    std::string getPid() const;
    std::string getUser() const;
    std::string getCmd() const;
    int getCpu() const;
    int getMem() const;
    std::string getUpTime() const;
    std::string getProcess();
};
void Process::setPid(int pid)
{
    this->pid = pid;
}
std::string Process::getPid() const
{
    return this->pid;
}
std::string Process::getProcess()
{
    if(!ProcessParser::isPidExisting(this->pid)) 
        return "";

    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);

    return this->pid + "   " + this->user + "   " + this->mem.substr(0, 5) + "   " + this->cpu.substr(0, 5) + "   " + this->upTime.substr(0, 5) + "   " + this->cmd.substr(0, 30) + "...";
}

std::string Process::getUser() const
{
    return this->user;
}

std::string Process::getCmd() const
{
    return this->cmd;
}

int Process::getCpu() const
{
    return std::stoi(this->cpu);
}

int Process::getMem() const
{
    return std::stoi(this->mem);
}
std::string Process::getUpTime() const
{
    return this->upTime;
}
