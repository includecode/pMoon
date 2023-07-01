/**
 *  Project pMoon by Pavel SAMENE TIAH; FILE Common.h, GNU GENERAL PUBLIC LICENSE, July 2023
 * 
*/
#pragma once

// STL
#include <iostream>
#include <vector>
#include <memory>
#include <cstring>
#include "stdint.h"

// User defined
#include "ExceptionHandler.h"

#define LOG_TO_FILE(msg) ExceptionHandler::writeTologFile(std::string( __PRETTY_FUNCTION__).append(" ") + msg)