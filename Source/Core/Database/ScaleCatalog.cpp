#include "ScaleCatalog.h"
#include <algorithm>
#include <set>

namespace rel {

void ScaleCatalog::scan(const std::vector<std::filesystem::path>& roots)
{
    entries_.clear();
    std::set<std::string> seenPaths;

    for (const auto& root : roots) {
        std::error_code ec;
        if (!std::filesystem::exists(root, ec)) continue;
        for (auto it = std::filesystem::recursive_directory_iterator(root, ec);
             it != std::filesystem::recursive_directory_iterator();
             it.increment(ec)) {
            if (ec) break;
            if (!it->is_regular_file(ec) || ec) continue;
            const auto ext = it->path().extension().u8string();
            if (ext != ".scl" && ext != ".SCL") continue;
            const auto canonical = std::filesystem::weakly_canonical(it->path(), ec);
            const auto key = (ec ? it->path() : canonical).u8string();
            if (!seenPaths.insert(key).second) continue;

            ScaleEntry entry;
            entry.name = it->path().stem().u8string();
            entry.path = key;
            entry.source = root.u8string();
            entries_.push_back(std::move(entry));
        }
    }
    std::sort(entries_.begin(), entries_.end(), [](const auto& a, const auto& b) { return a.name < b.name; });
}

} // namespace rel
