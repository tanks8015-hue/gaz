#include "Resource.h"
#include "FileSystemException.h"
#include <regex>

void validateName(const std::string& name) {
    std::regex invalidChars(R"([\\/:*?"<>|])");
    if (std::regex_search(name, invalidChars)) {
        throw FileSystemException("Ошибка: Имя содержит запрещенные символы!");
    }
    if (name.empty()) {
        throw FileSystemException("Ошибка: Имя не может быть пустым!");
    }
}

Resource::Resource(const std::string& name, AccessLevel level)
    : accessLevel(level) {
    validateName(name);
    this->name = name;
    creationDate = std::time(nullptr);
}
void Resource::search(const std::function<bool(const Resource*)>& predicate, std::vector<const Resource*>& results) const {
    if (predicate(this)) {
        results.push_back(this);
    }
}
void Resource::setCreationDate(std::time_t date) {
    creationDate = date;
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