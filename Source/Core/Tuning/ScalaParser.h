#pragma once
#include "TuningSystem.h"
#include <string>
#include <vector>

namespace rel {

class ScalaParser {
public:
    static bool loadScl(const std::string& text, TuningSystem& destination, std::string& error);
    static bool loadSclFile(const std::string& path, TuningSystem& destination, std::string& error);
};

} // namespace rel
