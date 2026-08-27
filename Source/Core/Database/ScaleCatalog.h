#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace rel {

struct ScaleEntry {
    std::string name;
    std::string path;
    std::string source;
};

class ScaleCatalog {
public:
    void scan(const std::vector<std::filesystem::path>& roots);
    const std::vector<ScaleEntry>& entries() const noexcept { return entries_; }
    std::size_t size() const noexcept { return entries_.size(); }

private:
    std::vector<ScaleEntry> entries_;
};

} // namespace rel
