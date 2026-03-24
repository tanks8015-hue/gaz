#include "Directory.h"
#include "FileSystemException.h"
#include <iostream>
#include <algorithm>

Directory::Directory(const std::string& name, AccessLevel level)
    : Resource(name, level) {
}

void Directory::addResource(std::unique_ptr<Resource> resource) {
    contents.push_back(std::move(resource));
}

void Directory::removeResource(const std::string& name) {
    contents.erase(
        std::remove_if(contents.begin(), contents.end(),
            [&name](const std::unique_ptr<Resource>& res) {
                return res->getName() == name;
            }),
        contents.end()
    );
}

size_t Directory::calculateSize() const {
    size_t total = 0;
    for (const auto& res : contents) {
        total += res->calculateSize();
    }
    return total;
}

void Directory::printInfo(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "+ [Папка] " << getName() << "\n";
    for (const auto& res : contents) {
        res->printInfo(depth + 1);
    }
}
void Directory::clear() {
    contents.clear(); // Вектор очищается, деструкторы unique_ptr удаляют всё дерево
}
std::unique_ptr<Resource> Directory::clone() const {
    auto copyDir = std::make_unique<Directory>(getName(), getAccessLevel());
    for (const auto& res : contents) {
        // Рекурсивно клонируем всё содержимое
        copyDir->addResource(res->clone());
    }
    return copyDir;
}
std::unique_ptr<Resource> Directory::extractResource(const std::string& name) {
    auto it = std::find_if(contents.begin(), contents.end(),
        [&name](const std::unique_ptr<Resource>& res) {
            return res->getName() == name;
        });

    if (it == contents.end()) {
        throw FileSystemException("Ошибка: Ресурс для перемещения не найден!");
    }

    std::unique_ptr<Resource> extracted = std::move(*it);
    contents.erase(it);
    return extracted;
}

