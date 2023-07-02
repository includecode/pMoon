/**
 *  Project pMoon by Pavel SAMENE TIAH; Processes.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/
#pragma once

#include "Common.h"

enum class processRules_e
{
    KILL,   // If process is running, kill it
    LAUNCH, // If process is NOT running, launch it
};

class Process
{

private:
    std::string command = std::string("");  // Command (and args) for the process
    std::string name = std::string("");     // Linux process name to monitor
    processRules_e rule = processRules_e::KILL;
    uint64_t pid = -1;


public:
    Process(const std::string& command, const std::string& name, const processRules_e rule);
    ~Process();

    /* Getters and Setters */

    void setPid(pid_t pid);

    const std::string& getName();

    pid_t getPid();

    processRules_e getProcessRule();

    const std::string& getCommand();
};
