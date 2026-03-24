#include "Resource.h"
#include <regex>
#include "FileSystemException.h"

Resource::Resource(const std::string& name, AccessLevel level)
    : accessLevel(level) {
    validateName(name); // Проверяем имя перед присвоением
    this->name = name;
    creationDate = std::time(nullptr);
}
void validateName(const std::string& name) {
    std::regex invalidChars(R"([\\/:*?"<>|])");
    if (std::regex_search(name, invalidChars)) {
        throw FileSystemException("Ошибка: Имя содержит запрещенные символы!");
    }
    if (name.empty()) {
        throw FileSystemException("Ошибка: Имя не может быть пустым!");
    }
}
void Resource::setNameInternal(const std::string& newName) {
    validateName(newName); 
    name = newName;
}

std::string Resource::getName() const {
    return name;
}

std::time_t Resource::getCreationDate() const {
    return creationDate;
}

AccessLevel Resource::getAccessLevel() const {
    return accessLevel;
}

void Resource::setAccessLevel(AccessLevel level) {
    accessLevel = level;
}