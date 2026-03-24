#include "FileSystemException.h"

FileSystemException::FileSystemException(const std::string& msg) : message(msg) {}

const char* FileSystemException::what() const noexcept {
    return message.c_str();
}