#include "ArchiveManager.h"
#include "File.h"
#include "FileSystemException.h"
#include "Logger.h"
#include <fstream>

// Уникальный заголовок для проверки валидности файла
const uint32_t MAGIC_NUMBER = 0x41524348;

// --- Вспомогательные функции для бинарной записи/чтения строк ---
void writeString(std::ofstream& out, const std::string& str) {
    size_t len = str.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len)); // Пишем длину
    out.write(str.c_str(), len); // Пишем сами символы
}

std::string readString(std::ifstream& in) {
    size_t len;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    std::string str(len, '\0');
    in.read(&str[0], len);
    return str;
}

// --- Рекурсивная запись ресурса ---
void serializeResource(std::ofstream& out, const Resource* res) {
    // 1. Записываем тип: 0 для файла, 1 для папки
    const File* filePtr = dynamic_cast<const File*>(res);
    char type = (filePtr != nullptr) ? 0 : 1;
    out.write(&type, sizeof(type));

    // 2. Пишем базовые поля Resource
    writeString(out, res->getName());
    AccessLevel level = res->getAccessLevel();
    out.write(reinterpret_cast<const char*>(&level), sizeof(level));
    std::time_t date = res->getCreationDate();
    out.write(reinterpret_cast<const char*>(&date), sizeof(date));

    // 3. Пишем специфичные поля
    if (type == 0) { // Если это Файл
        writeString(out, filePtr->getExtension());
        size_t size = filePtr->calculateSize();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    }
    else { // Если это Папка
        const Directory* dirPtr = dynamic_cast<const Directory*>(res);
        const auto& contents = dirPtr->getContents();
        size_t count = contents.size();
        out.write(reinterpret_cast<const char*>(&count), sizeof(count)); // Количество вложенных элементов

        for (const auto& child : contents) {
            serializeResource(out, child.get()); // Рекурсивно сохраняем содержимое
        }
    }
}

// --- Рекурсивное чтение ресурса ---
std::unique_ptr<Resource> deserializeResource(std::ifstream& in) {
    char type;
    in.read(&type, sizeof(type));

    std::string name = readString(in);

    AccessLevel level;
    in.read(reinterpret_cast<char*>(&level), sizeof(level));

    std::time_t date;
    in.read(reinterpret_cast<char*>(&date), sizeof(date));

    std::unique_ptr<Resource> res;

    if (type == 0) { // Читаем Файл
        std::string ext = readString(in);
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
        auto file = std::make_unique<File>(name, ext, size, level);
        res = std::move(file);
    }
    else if (type == 1) { // Читаем Папку
        auto dir = std::make_unique<Directory>(name, level);
        size_t count;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        for (size_t i = 0; i < count; ++i) {
            dir->addResource(deserializeResource(in)); // Рекурсивно читаем вложения
        }
        res = std::move(dir);
    }
    else {
        throw FileSystemException("Критическая ошибка: неизвестный тип ресурса в файле!");
    }

    res->setCreationDate(date); // Восстанавливаем оригинальную дату
    return res;
}
void ArchiveManager::saveToFile(const Directory& rootDir, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out.is_open()) {
        throw FileSystemException("Ошибка: не удалось открыть файл для записи архива!");
    }

    out.write(reinterpret_cast<const char*>(&MAGIC_NUMBER), sizeof(MAGIC_NUMBER)); // Запись магического числа
    serializeResource(out, &rootDir);
    Logger::log("Архив успешно сохранен в файл: " + filename);
}

std::unique_ptr<Directory> ArchiveManager::loadFromFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        throw FileSystemException("Ошибка: файл архива не найден!");
    }

    uint32_t magic;
    in.read(reinterpret_cast<char*>(&magic), sizeof(magic));

    if (magic != MAGIC_NUMBER) {
        throw FileSystemException("ОШИБКА ЦЕЛОСТНОСТИ: Неверный формат файла или данные повреждены! (Магическое число не совпадает)");
    }

    auto rootRes = deserializeResource(in);
    Directory* rawDir = dynamic_cast<Directory*>(rootRes.get());
    if (!rawDir) {
        throw FileSystemException("Ошибка структуры: корень архива не является директорией!");
    }
    rootRes.release();
    Logger::log("Архив успешно загружен из файла: " + filename);
    return std::unique_ptr<Directory>(rawDir);
}