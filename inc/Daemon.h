/**
 *  Project pMoon by Pavel TIAH; Daemon.h, GNU GENERAL PUBLIC LICENSE, May 2023
 * 
*/

#pragma once

#include "Common.h"
#include "Process.h"
#include <vector>

class Daemon
{
private:
    uint32_t scanFrequency = 100; // in ms
    std::vector<std::shared_ptr<Process>> processList;

public:
    Daemon(/* args */);
    ~Daemon();
    void run();
    void setAllPID();
    void readConfigFile(const std::string& filePath);
    bool isProcessAlive(pid_t processId);
    void addProcessToList(std::shared_ptr<Process>& process);

    /* Getters and Setters */
    void setFrequency(uint32_t freq);
};
