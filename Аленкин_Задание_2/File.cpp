#include "File.h"
#include <iostream>

File::File(const std::string& name, const std::string& ext, size_t size, AccessLevel level)
    : Resource(name, level), extension(ext), size(size) {
}

std::string File::getExtension() const {
    return extension;
}

size_t File::calculateSize() const {
    return size;
}

void File::printInfo(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "- [Ôאיכ] " << getName() << extension << " (" << size << " באיע)\n";
}