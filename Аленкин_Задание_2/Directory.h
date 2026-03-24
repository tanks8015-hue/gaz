#pragma once
#include "Resource.h"
#include <vector>
#include <memory>

class Directory : public Resource {
private:
    std::vector<std::unique_ptr<Resource>> contents;

public:
    Directory(const std::string& name, AccessLevel level);

    void addResource(std::unique_ptr<Resource> resource);
    void removeResource(const std::string& name);

    size_t calculateSize() const override;
    void printInfo(int depth = 0) const override;
};