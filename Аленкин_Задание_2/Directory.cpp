#include "Directory.h"
#include "FileSystemException.h"
#include <iostream>
#include <algorithm>
#include "Logger.h"
#include <fstream>
#include <iomanip>
#include "File.h"
Directory::Directory(const std::string& name, AccessLevel level)
    : Resource(name, level) {
}

void Directory::addResource(std::unique_ptr<Resource> resource) {
    std::string resName = resource->getName(); // Запоминаем имя до std::move
    contents.push_back(std::move(resource));
    Logger::log("Добавлен ресурс '" + resName + "' в папку '" + getName() + "'");
}

void Directory::search(const std::function<bool(const Resource*)>& predicate, std::vector<const Resource*>& results) const {
    if (predicate(this)) {
        results.push_back(this);
    }
    for (const auto& res : contents) {
        res->search(predicate, results);
    }
}
void Directory::removeResource(const std::string& name, AccessLevel currentUserLevel) {
    auto originalSize = contents.size();

    auto it = std::find_if(contents.begin(), contents.end(),
        [&name](const std::unique_ptr<Resource>& res) {
            return res->getName() == name;
        });

    if (it != contents.end()) {
        if (currentUserLevel < (*it)->getAccessLevel()) {
            throw FileSystemException("Отказано в доступе: недостаточно прав для удаления ресурса '" + name + "'!");
        }

        contents.erase(it);
        Logger::log("Удален ресурс '" + name + "' из папки '" + getName() + "'");
    }
    else {
        Logger::log("Попытка удаления несуществующего ресурса '" + name + "'", true);
    }
}
void Directory::removeResource(const std::string& name) {
    auto originalSize = contents.size();
    contents.erase(
        std::remove_if(contents.begin(), contents.end(),
            [&name](const std::unique_ptr<Resource>& res) {
                return res->getName() == name;
            }),
        contents.end()
    );

    if (contents.size() < originalSize) {
        Logger::log("Удален ресурс '" + name + "' из папки '" + getName() + "'");
    }
    else {
        Logger::log("Попытка удаления несуществующего ресурса '" + name + "'", true);
    }
}
void Directory::sortContents(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comparator) {
    std::sort(contents.begin(), contents.end(), comparator);
    Logger::log("Выполнена сортировка содержимого папки '" + getName() + "'");
}
size_t Directory::calculateSize() const {
    size_t total = 0;
    for (const auto& res : contents) {
        total += res->calculateSize();
    }
    return total;
}
const std::vector<std::unique_ptr<Resource>>& Directory::getContents() const {
    return contents;
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
void Directory::collectAudit(AuditInfo& info) const {
    info.dirCount++; // Считаем саму папку
    for (const auto& res : contents) {
        res->collectAudit(info); // Рекурсивно собираем данные с содержимого
    }
}

void Directory::printGlobalAudit() const {
    AuditInfo info;
    collectAudit(info); // Запускаем сбор

    std::cout << "\n--- Глобальный аудит хранилища ---\n";
    std::cout << "Всего папок: " << info.dirCount << "\n";
    std::cout << "Всего файлов: " << info.fileCount << "\n";
    std::cout << "Общий объем: " << info.totalSize << " байт\n";
    std::cout << "Средний размер файла: "
        << (info.fileCount > 0 ? info.totalSize / info.fileCount : 0)
        << " байт\n";
}
void Directory::exportToCSV(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw FileSystemException("Ошибка: не удалось создать CSV файл!");
    }

    // Записываем заголовки столбцов
    out << "Имя,Тип,Размер(Байт),Уровень Доступа\n";

    // Собираем абсолютно все ресурсы (условие всегда возвращает true)
    std::vector<const Resource*> allResources;
    search([](const Resource*) { return true; }, allResources);

    for (const auto* res : allResources) {
        // Определяем тип (Файл или Папка)
        std::string type = dynamic_cast<const File*>(res) ? "Файл" : "Папка";

        // Расшифровываем enum AccessLevel в строку
        std::string aclStr = "GUEST";
        if (res->getAccessLevel() == AccessLevel::USER) aclStr = "USER";
        if (res->getAccessLevel() == AccessLevel::ADMIN) aclStr = "ADMIN";

        // Пишем строку в CSV
        out << res->getName() << ","
            << type << ","
            << res->calculateSize() << ","
            << aclStr << "\n";
    }

    Logger::log("Структура успешно экспортирована в CSV: " + filename);
}
