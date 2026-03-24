#pragma once
#include "Directory.h"
#include <string>
#include <memory>

class ArchiveManager {
public:
    static void saveToFile(const Directory& rootDir, const std::string& filename);
    static std::unique_ptr<Directory> loadFromFile(const std::string& filename);
};