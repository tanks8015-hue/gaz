#include <iostream>
#include <vector>
#include "FileSystemException.h"
#include "File.h"
#include "Directory.h"
#include "Logger.h"
#include "ArchiveManager.h"
int main() {
    try {
        setlocale(LC_ALL, "Russian");

        Directory root("Root", AccessLevel::ADMIN);

        auto doc = std::make_unique<File>("report", ".txt", 1024, AccessLevel::USER);
        auto subFolder = std::make_unique<Directory>("Images", AccessLevel::USER);
        subFolder->addResource(std::make_unique<File>("photo", ".png", 2048, AccessLevel::GUEST));

        root.addResource(std::move(doc));
        root.addResource(std::move(subFolder));

        std::cout << "--- Исходное дерево ---\n";
        root.printInfo();
        std::cout << "Общий размер: " << root.calculateSize() << " байт\n";

        auto clonedDir = root.clone();
        clonedDir->setAccessLevel(AccessLevel::ADMIN);
        std::cout << "\n--- Клонированное дерево ---\n";
        clonedDir->printInfo();
        root.printGlobalAudit();
        struct SortByName {
            bool operator()(const std::unique_ptr<Resource>& a, const std::unique_ptr<Resource>& b) const {
                return a->getName() < b->getName();
            }
        };

        std::cout << "\n--- Папка Root после сортировки по имени ---\n";
        root.sortContents(SortByName());
        root.printInfo();
        std::cout << "\n--- Результаты поиска (файлы с 'photo' > 1000 байт) ---\n";
        std::vector<const Resource*> foundItems;

        auto searchCondition = [](const Resource* res) {
            const File* filePtr = dynamic_cast<const File*>(res);
            if (filePtr != nullptr) {
                return filePtr->getName().find("photo") != std::string::npos && filePtr->calculateSize() > 1000;
            }
            return false;
            };

        root.search(searchCondition, foundItems);

        if (foundItems.empty()) {
            std::cout << "Ничего не найдено.\n";
        }
        else {
            for (const auto* item : foundItems) {
                std::cout << "Найдено: " << item->getName() << " (" << item->calculateSize() << " байт)\n";
            }
        }
        std::cout << "\n";
        std::cout << "Сохранение архива в файл...\n";
        ArchiveManager::saveToFile(root, "archive.dat");
        std::cout << "Загрузка архива из файла...\n";
        auto loadedRoot = ArchiveManager::loadFromFile("archive.dat");
        std::cout << "\n--- Загруженное из archive.dat дерево ---\n";
        loadedRoot->printInfo();
    }
    catch (const FileSystemException& e) {
        std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
        Logger::log(e.what(), true);
    }

    return 0;
}