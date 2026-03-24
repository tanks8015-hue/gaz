#pragma once
#include <exception>
#include <string>

class FileSystemException : public std::exception {
private:
    std::string message;
public:
    explicit FileSystemException(const std::string& msg);
    const char* what() const noexcept override;
};