#include "Common.h"
#include "ExceptionHandler.h"
#include "Daemon.h"
#include "Process.h"

bool checkArguments(const int argc, char const **argv, string& configFile);
void displayHelp();

int main(int argc, char const **argv)
{
    string configFilePath("");

    bool argError = checkArguments(argc, argv, configFilePath);
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
        daemon->run();
    }

    return 0;
}

/**
 * @brief check command line arguments from user
 * @return \c bool True if arguments are good, false otherwise
*/
bool checkArguments(const int argc, char const **argv, string& configFile)
{
    bool argError = true;
    if(argc == 2)
    {
        configFile = string(argv[1]);
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
    std::cout << "\t./pMoon   <config file>" << std::endl;
    std::cout << "**********" << std::endl;
}