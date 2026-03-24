#include "File.h"
#include <iostream>
#include <regex>
#include "FileSystemException.h"
File::File(const std::string& name, const std::string& ext, size_t size, AccessLevel level)
    : Resource(name, level), extension(ext), size(size) {

    // Проверка расширения: должно начинаться с точки и содержать только буквы/цифры
    std::regex validExt(R"(^\.[a-zA-Z0-9]+$)");
    if (!ext.empty() && !std::regex_match(ext, validExt)) {
        throw FileSystemException("Ошибка: Некорректный формат расширения файла!");
    }
}

std::string File::getExtension() const {
    return extension;
}

size_t File::calculateSize() const {
    return size;
}
void File::collectAudit(AuditInfo& info) const {
    info.fileCount++;
    info.totalSize += size;
}
void File::printInfo(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- [Файл] " << getName() << extension << " (" << size << " байт)\n";
}
std::unique_ptr<Resource> File::clone() const {
    
    return std::make_unique<File>(getName(), extension, size, getAccessLevel());
}