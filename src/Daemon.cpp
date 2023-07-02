/**
 *  Project pMoon by Pavel TIAH; Daemon.cpp, GNU GENERAL PUBLIC LICENSE, May 2023
 *
 */

#include "Daemon.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <unistd.h>

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
        for (std::shared_ptr<Process>& process : this->processList)
        {
            pid_t processPID = process->getPid();

            bool processAlive = isProcessAlive(processPID);

            if(process->getProcessRule() == processRules_e::KILL)
            {
                    LOG_TO_FILE("Trying to kill process " + process->getName() + " with PID: " + std::to_string(process->getPid()));
                if(processAlive && processPID > 0)
                {
                    LOG_TO_FILE("Trying to kill process " + process->getName() + " with PID: " + std::to_string(process->getPid()));
                    if (kill(process->getPid(), SIGKILL) == -1)
                    {
                        LOG_TO_FILE("Failed to kill process " + process->getName());
                    }
                    LOG_TO_FILE("Killed");
                }
            }
            else // Relaunch process ?
            {
                if(!processAlive || processPID < 1)
                {
                    LOG_TO_FILE("Trying to launch process" + process->getName());
                    //execlp(process->getCommand().c_str(), NULL, NULL);
                    std::thread launcher(system, process->getCommand().c_str());
                    launcher.detach();
                    LOG_TO_FILE("Launched");
                }
            }
        }

        std::this_thread::sleep_for(chrono::milliseconds(this->scanFrequency));
    }
}

/**
 * @brief Read the configuration file to load all processes
*/
void Daemon::readConfigFile(const std::string& filePath)
{
    std::ifstream configFile (filePath.c_str(), std::ios_base::app);
    if(configFile.is_open())
    {
        std::string processName;
        while (std::getline(configFile, processName))
        {
            if(processName.size() && (processName.at(0) != '#')) // Not a blank line and not a comment line
            {
                std::string processCommand("");
                std::string rule("");
                processRules_e processRule = processRules_e::KILL;
                bool readOk = false;
                
                    if(std::getline(configFile, processCommand))
                    {
                        if(std::getline(configFile, rule))
                        {
                            if(rule.size() && rule.at(0) == 'L')
                            {
                                processRule = processRules_e::LAUNCH;
                            }
                            else
                            {
                                if(rule.size() && rule.at(0) != 'K')
                                {
                                    std::string logMessage = std::string("Bad value " + rule + " for process: " + processName);
                                    LOG_TO_FILE(logMessage);
                                    throw ExceptionHandler(logMessage, exceptionType_e::WARNING);
                                }
                            }

                            std::shared_ptr<Process>process = std::make_shared<Process>(processCommand, processName, processRule);
                            addProcessToList(process);

                            LOG_TO_FILE("Success reading: " + processName + " | " + processCommand + " | " + rule);
                            readOk = true;          
                        }
                    }

                 if(!readOk)
                 {
                    std::string logMessage = std::string("Failed to read a process from config file");
                    LOG_TO_FILE(logMessage);
                    throw ExceptionHandler(logMessage, exceptionType_e::WARNING);
                 }
            }
        }

        LOG_TO_FILE("Finished parsing configuration file");
        configFile.close();
    }
    else
    {
        std::string logMessage = std::string("Failed to open configuration file ") + filePath;
        LOG_TO_FILE(logMessage);
        throw ExceptionHandler(logMessage, exceptionType_e::CRITICAL);
    }
}


/**
 * @brief Check if a process is running, with it's pid
 * @param processId : pid to check
*/
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
    for (std::shared_ptr<Process>& process : this->processList)
    {
        /* pidof -s process_name, "r" */
        command = std::string("pidof -s " + process->getName());
        FILE *cmd_pipe = popen(command.c_str(), "r");

        fgets(buff.data(), PATH_MAX, cmd_pipe);
        pid_t pid = strtoul(buff.data(), NULL, 10);
        process->setPid(pid);
        std::fill(buff.begin(), buff.end(), 0); // Prepare for next iteration

        pclose(cmd_pipe);
    }

    LOG_TO_FILE("Finished setting PIDs");
}


/**
 * @brief Adds a new process to process list
 * @param process   : Process to add
*/
void Daemon::addProcessToList(std::shared_ptr<Process>& process)
{
    this->processList.push_back(process);
}

/**
 * @brief Set new scan frequency for the daemon function
*/
void Daemon::setFrequency(uint32_t freq)
{
    scanFrequency = freq;
}
