#include "Directory.h"
#include <iostream>
#include <algorithm>

Directory::Directory(const std::string& name, AccessLevel level)
    : Resource(name, level) {
}

void Directory::addResource(std::unique_ptr<Resource> resource) {
    contents.push_back(std::move(resource));
}

void Directory::removeResource(const std::string& name) {
    // std::remove_if сдвигает элементы, которые нужно удалить, в конец вектора, а erase их уничтожает
    // Деструкторы std::unique_ptr автоматически очистят память
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
        res->printInfo(depth + 1); // Рекурсивный вывод
    }
}