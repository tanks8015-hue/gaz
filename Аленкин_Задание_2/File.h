#pragma once
#include "Resource.h"

class File : public Resource {
private:
    std::string extension;
    size_t size;

public:
    File(const std::string& name, const std::string& ext, size_t size, AccessLevel level);

    std::string getExtension() const;

    size_t calculateSize() const override;
    void printInfo(int depth = 0) const override;
};