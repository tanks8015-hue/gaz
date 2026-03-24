#define _CRT_SECURE_NO_WARNINGS 
#include "Logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>

void Logger::log(const std::string& message, bool isError) {
    std::ofstream file("history.log", std::ios::app);
    if (!file.is_open()) return;

    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    file << "[" << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S") << "] "
        << (isError ? "[ERROR] " : "[INFO]  ")
        << message << "\n";
}