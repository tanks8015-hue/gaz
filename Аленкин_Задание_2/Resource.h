#pragma once
#include <string>
#include <ctime>

enum class AccessLevel {
    GUEST,
    USER,
    ADMIN
};

class Resource {
private:
    std::string name;
    std::time_t creationDate;
    AccessLevel accessLevel;

protected:
    // Защищенный метод, чтобы наследники могли менять имя после валидации
    void setNameInternal(const std::string& newName);

public:
    Resource(const std::string& name, AccessLevel level);
    virtual ~Resource() = default;

    // Геттеры и сеттеры
    std::string getName() const;
    std::time_t getCreationDate() const;
    AccessLevel getAccessLevel() const;

    void setAccessLevel(AccessLevel level);

    // Чисто виртуальные методы
    virtual size_t calculateSize() const = 0;
    virtual void printInfo(int depth = 0) const = 0;
    virtual std::unique_ptr<Resource> clone() const = 0;
};