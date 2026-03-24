#include <iostream>
#include "FileSystemException.h"
#include "File.h"
#include "Directory.h"
#include "Logger.h"
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
        std::cout << "Общий размер: " << root.calculateSize() << " байт\n\n";
        auto clonedDir = root.clone();
        clonedDir->setAccessLevel(AccessLevel::ADMIN);
        std::cout << "--- Клонированное дерево ---\n";
        clonedDir->printInfo();
    }
    catch (const FileSystemException& e) {
        std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
        Logger::log(e.what(), true);
    }
    return 0;
}