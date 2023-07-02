/**
 *  Project pMoon by Pavel SAMENE TIAH; Process.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#include "Process.h"

Process::Process(const std::string& command, const std::string& name, const processRules_e rule):
    command(command), name(name), rule(rule)
{

}

Process::~Process()
{
}

void Process::setPid(pid_t pid)
{
    this->pid = pid;
}

pid_t Process::getPid()
{
    return pid;
}

const std::string& Process::getName()
{
    return name;
}

const std::string& Process::getCommand()
{
    return command;
}

processRules_e Process::getProcessRule()
{
    return rule;
}
