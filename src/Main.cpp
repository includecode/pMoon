#include "Common.h"
#include "ExceptionHandler.h"
#include "Daemon.h"
#include "Process.h"

bool checkArguments(const int argc, char const **argv, string& configFile, uint32_t &scanfFrequency);
void displayHelp();

int main(int argc, char const **argv)
{
    string configFilePath("");
    uint32_t scanfFrequency = 0;
    bool argError = checkArguments(argc, argv, configFilePath, scanfFrequency);
    if(argError)
    {
        displayHelp();
    }
    else
    {
        std::unique_ptr<Daemon> daemon = std::unique_ptr<Daemon>(new Daemon());
        try
        {
            daemon->readConfigFile(configFilePath);
        }
        catch(const ExceptionHandler& e)
        {
            exit(EXIT_FAILURE);
        }
        
        daemon->setAllPID();

        // Minimum frequency allowed is 100 milliseconds
        scanfFrequency = (scanfFrequency > 99) ? scanfFrequency : 100;
        daemon->setFrequency(scanfFrequency);
        daemon->run();
    }

    return 0;
}

/**
 * @brief check command line arguments from user
 * @return \c bool True if arguments are good, false otherwise
*/
bool checkArguments(const int argc, char const **argv, string& configFile, uint32_t& scanfFrequency)
{
    bool argError = true;
    if(argc == 3)
    {
        configFile = string(argv[1]);
        scanfFrequency = std::stoi(argv[2]);
        if(configFile.size())
        {
            argError = false;
        }
    }
    else
    {
        LOG_TO_FILE("Insufficient or bad arguments supplied");
    }

    return argError;
}

/**
 * @brief Display CLI usage
*/
void displayHelp()
{
    std::cout << "\n\n**********" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "\t./pMoon   <config file> <scan frequency>" << std::endl;
    std::cout << "<scan frequency> represents the time for pMoon to recheck all processes (in milliseconds). Minimun is 100 ms " << std::endl;
    std::cout << "**********" << std::endl;
}