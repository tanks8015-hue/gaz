#include "FileSystemException.h"

// Конструктор
FileSystemException::FileSystemException(const std::string& msg) : message(msg) {}

// Переопределение метода what()
const char* FileSystemException::what() const noexcept {
    return message.c_str();
}