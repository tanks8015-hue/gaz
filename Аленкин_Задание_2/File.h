#pragma once
#include "Resource.h"
#include <memory>
class File : public Resource {
private:
    std::string extension;
    size_t size;

public:
    File(const std::string& name, const std::string& ext, size_t size, AccessLevel level);
    std::unique_ptr<Resource> clone() const override;
    std::string getExtension() const;

    size_t calculateSize() const override;
    void printInfo(int depth = 0) const override;
    void collectAudit(AuditInfo& info) const override;
};