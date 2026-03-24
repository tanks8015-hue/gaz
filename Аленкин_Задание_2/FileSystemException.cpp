#include <iostream>
#include "FileSystemException.h"
//#include "Resource.h"

int main() {
    try {
        std::cout << "Система виртуального архива инициализирована." << std::endl;
    }
    catch (const FileSystemException& e) {
        std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
    }
    return 0;
}