#include "Resource.h"
#include "FileSystemException.h"
#include <regex>

// 1. Сначала ОПРЕДЕЛЯЕМ функцию валидации
void validateName(const std::string& name) {
    std::regex invalidChars(R"([\\/:*?"<>|])");
    if (std::regex_search(name, invalidChars)) {
        throw FileSystemException("Ошибка: Имя содержит запрещенные символы!");
    }
    if (name.empty()) {
        throw FileSystemException("Ошибка: Имя не может быть пустым!");
    }
}

// 2. Только ПОСЛЕ этого используем её в методах класса
Resource::Resource(const std::string& name, AccessLevel level)
    : accessLevel(level) {
    validateName(name); // Теперь компилятор знает, что это за функция
    this->name = name;
    creationDate = std::time(nullptr);
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