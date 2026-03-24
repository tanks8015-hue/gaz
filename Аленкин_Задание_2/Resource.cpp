#include "Resource.h"

Resource::Resource(const std::string& name, AccessLevel level)
    : name(name), accessLevel(level) {
    creationDate = std::time(nullptr); // Устанавливаем текущее время при создании
}

void Resource::setNameInternal(const std::string& newName) {
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