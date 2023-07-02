/**
 *  Project pMoon by Pavel TIAH; Daemon.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 *
 */

#include "Daemon.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <chrono>
#include <thread>

Daemon::Daemon()
{
}

Daemon::~Daemon()
{
}

void Daemon::run()
{
    while (1)
    {
        for (Process& process : this->processList)
        {
            if(isProcessAlive(process.getPid()))
            {
                if(process.getProcessRule() == processRules_e::KILL)
                {
                    LOG_TO_FILE("Trying to kill process" + process.getName());
                    if (kill(process.getPid(), SIGKILL) == -1)
                    {
                        LOG_TO_FILE("Failed to kill process" + process.getName());
                    }
                }
                else // Relaunch process
                {
                    LOG_TO_FILE("Trying to launch process" + process.getName());
                    system(process.getCommand().c_str());
                }
            }
        }

        std::this_thread::sleep_for(chrono::milliseconds(this->scanFrequency));
    }
}

void Daemon::readConfigFile(const std::string& filePath)
{
}

bool Daemon::isProcessAlive(pid_t processId)
{
    bool running = true;
    // Wait for child process, this should clean up defunct processes
    waitpid(processId, nullptr, WNOHANG);

    // kill failed let's see why..
    if (kill(processId, 0) == -1)
    {
        // First of all kill may fail with EPERM if we run as a different user and we have no access,
        // so let's make sure the errno is ESRCH (Process not found!)
        if (errno == ESRCH)
        {
            running = false;
        }
    }

    // If kill didn't fail the process is still running
    return running;
}

/**
 * Find all PIDs from thier names and sore in process list
*/
void Daemon::setAllPID()
{
    std::vector<char> buff(PATH_MAX);
    std::string command("");
    for (Process& process : this->processList)
    {
        /* pidof -s process_name, "r" */
        command = std::string("pidof -s " + process.getName());
        FILE *cmd_pipe = popen(command.c_str(), "r");

        fgets(buff.data(), PATH_MAX, cmd_pipe);
        pid_t pid = strtoul(buff.data(), NULL, 10);
        process.setPid(pid);
        std::fill(buff.begin(), buff.end(), 0); // Prepare for next iteration

        pclose(cmd_pipe);
    }

    LOG_TO_FILE("Finished setting PIDs");
}


/**
 * @brief Adds a new process to process list
 * @param process   : Process to add
*/
void Daemon::addProcessToList(const Process& process)
{
    this->processList.push_back(process);
}