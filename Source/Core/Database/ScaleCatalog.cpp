#include "ScaleCatalog.h"
#include <algorithm>

namespace rel {

void ScaleCatalog::scan(const std::vector<std::filesystem::path>& roots)
{
    entries_.clear();
    for (const auto& root : roots) {
        std::error_code ec;
        if (!std::filesystem::exists(root, ec)) continue;
        for (auto it = std::filesystem::recursive_directory_iterator(root, ec); it != std::filesystem::recursive_directory_iterator(); it.increment(ec)) {
            if (ec) break;
            if (!it->is_regular_file()) continue;
            const auto ext = it->path().extension().string();
            if (ext != ".scl" && ext != ".SCL") continue;
            ScaleEntry entry;
            entry.name = it->path().stem().string();
            entry.path = it->path().string();
            entry.source = root.string();
            entries_.push_back(std::move(entry));
        }
    }
    std::sort(entries_.begin(), entries_.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
}

} // namespace rel
