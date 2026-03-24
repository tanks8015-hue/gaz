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
    void clear();
    std::unique_ptr<Resource> clone() const override;
    std::unique_ptr<Resource> extractResource(const std::string& name);
    void collectAudit(AuditInfo& info) const override;
    void printGlobalAudit() const;
    void sortContents(const std::function<bool(const std::unique_ptr<Resource>&, const std::unique_ptr<Resource>&)>& comparator);
    void search(const std::function<bool(const Resource*)>& predicate, std::vector<const Resource*>& results) const override;
};