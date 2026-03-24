#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include "FileSystemException.h"
#include "File.h"
#include "Directory.h"
#include "Logger.h"
#include "ArchiveManager.h"

int getIntInput(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Ошибка ввода! Введите число.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Вспомогательная функция для выбора уровня доступа
AccessLevel getAccessLevelInput() {
    std::cout << "Выберите уровень доступа (0 - GUEST, 1 - USER, 2 - ADMIN): ";
    int level = getIntInput("");
    if (level == 0) return AccessLevel::GUEST;
    if (level == 1) return AccessLevel::USER;
    return AccessLevel::ADMIN;
}

void printMenu() {
    std::cout << "\n========================================\n";
    std::cout << "       СИСТЕМА ВИРТУАЛЬНОГО АРХИВА      \n";
    std::cout << "========================================\n";
    std::cout << "1. Вывести структуру дерева\n";
    std::cout << "2. Создать папку (в корне)\n";
    std::cout << "3. Создать файл (в корне)\n";
    std::cout << "4. Удалить ресурс (из корня)\n";
    std::cout << "5. Глобальный аудит и статистика\n";
    std::cout << "6. Сохранить структуру в archive.dat\n";
    std::cout << "7. Загрузить структуру из archive.dat\n";
    std::cout << "8. Экспорт структуры в CSV\n";
    std::cout << "9. Переместить файл в папку 'System'\n";
    std::cout << "10. Очистить весь архив (Удалить всё)\n"; // <--- НОВЫЙ ПУНКТ
    std::cout << "0. Выход\n";
    std::cout << "========================================\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    // Инициализируем корень
    std::unique_ptr<Directory> root = std::make_unique<Directory>("Root", AccessLevel::ADMIN);

    // Добавим пару базовых ресурсов для наглядности
    root->addResource(std::make_unique<File>("readme", ".txt", 512, AccessLevel::GUEST));
    root->addResource(std::make_unique<Directory>("System", AccessLevel::ADMIN));

    bool isRunning = true;

    while (isRunning) {
        printMenu();
        int choice = getIntInput("Выберите действие: ");

        try {
            switch (choice) {
            case 1: {
                std::cout << "\n--- Текущая структура ---\n";
                root->printInfo();
                std::cout << "Общий размер: " << root->calculateSize() << " байт\n";
                break;
            }
            case 2: { // Интерактивное создание папки
                std::cout << "\n--- Создание папки ---\n";
                std::string name;
                std::cout << "Введите имя папки: ";
                std::getline(std::cin, name);
                AccessLevel level = getAccessLevelInput();

                root->addResource(std::make_unique<Directory>(name, level));
                std::cout << "Папка успешно создана!\n";
                break;
            }
            case 3: { // Интерактивное создание файла
                std::cout << "\n--- Создание файла ---\n";
                std::string name, ext;
                std::cout << "Введите имя файла: ";
                std::getline(std::cin, name);
                std::cout << "Введите расширение (например, .txt): ";
                std::getline(std::cin, ext);
                int size = getIntInput("Введите размер файла (в байтах): ");
                AccessLevel level = getAccessLevelInput();

                root->addResource(std::make_unique<File>(name, ext, size, level));
                std::cout << "Файл успешно создан!\n";
                break;
            }
            case 4: { // Удаление ресурса
                std::cout << "\n--- Удаление ресурса ---\n";
                std::string name;
                std::cout << "Введите точное имя ресурса для удаления: ";
                std::getline(std::cin, name);

                std::cout << "Под какими правами вы сейчас находитесь?\n";
                AccessLevel currentLevel = getAccessLevelInput();

                root->removeResource(name, currentLevel);
                std::cout << "Команда удаления выполнена.\n";
                break;
            }
            case 5: { // Аудит
                root->printGlobalAudit();
                break;
            }
            case 6: { // Сериализация
                ArchiveManager::saveToFile(*root, "archive.dat");
                std::cout << "Архив сохранен в файл archive.dat!\n";
                break;
            }
            case 7: { // Десериализация
                root = ArchiveManager::loadFromFile("archive.dat");
                std::cout << "Архив успешно загружен!\n";
                break;
            }
            case 8: { // Экспорт в CSV
                root->exportToCSV("archive_structure.csv");
                std::cout << "Данные выгружены в archive_structure.csv!\n";
                break;
            }
            case 9: { // Перемещение
                std::cout << "\n--- Перемещение ресурса ---\n";
                std::string name;
                std::cout << "Введите имя файла/папки из корня, которую нужно переместить: ";
                std::getline(std::cin, name);

                // 1. Вырезаем ресурс из корня
                auto extractedRes = root->extractResource(name);

                // 2. Ищем папку System внутри корня, чтобы положить ресурс туда
                bool moved = false;
                for (const auto& res : root->getContents()) {
                    if (res->getName() == "System") {
                        // Приводим базовый указатель к папке
                        Directory* targetDir = dynamic_cast<Directory*>(res.get());
                        if (targetDir) {
                            targetDir->addResource(std::move(extractedRes));
                            moved = true;
                            std::cout << "Ресурс '" << name << "' успешно перемещен в папку 'System'!\n";
                            break;
                        }
                    }
                }

                if (!moved) {
                    // Если папки System нет, возвращаем ресурс обратно в корень, чтобы не потерять
                    root->addResource(std::move(extractedRes));
                    std::cout << "Ошибка: Папка 'System' не найдена. Перемещение отменено.\n";
                }
                break;
            }
            case 10: { // Полная очистка структуры
                std::cout << "\n--- Полная очистка архива ---\n";
                std::cout << "Вы уверены? (1 - Да, 0 - Нет): ";
                int confirm = getIntInput("");
                if (confirm == 1) {
                    root->clear(); // Вызов функции очистки из 5 коммита
                    std::cout << "Все файлы и папки успешно удалены! Архив пуст.\n";
                }
                else {
                    std::cout << "Очистка отменена.\n";
                }
                break;
            }
            case 0: {
                isRunning = false;
                std::cout << "Завершение работы программы...\n";
                break;
            }
            default: {
                std::cout << "Неизвестная команда. Попробуйте снова.\n";
                break;
            }
            }
        }
        catch (const FileSystemException& e) {
            std::cerr << "\n[ОШИБКА ФАЙЛОВОЙ СИСТЕМЫ] " << e.what() << std::endl;
            Logger::log(e.what(), true);
        }
        catch (const std::exception& e) {
            std::cerr << "\n[КРИТИЧЕСКАЯ ОШИБКА] " << e.what() << std::endl;
        }
    }

    return 0;
}