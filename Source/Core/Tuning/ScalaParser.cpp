#include "ScalaParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>

namespace rel {
namespace {
std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    size_t i = 0; while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}
}

bool ScalaParser::loadScl(const std::string& text, TuningSystem& destination, std::string& error)
{
    std::istringstream in(text);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '!') continue;
        lines.push_back(line);
    }
    if (lines.size() < 2) { error = "Invalid Scala file: missing header or tone count."; return false; }

    int count = 0;
    try { count = std::stoi(lines[1]); } catch (...) { error = "Invalid Scala tone count."; return false; }
    if (count <= 0 || static_cast<size_t>(count + 2) > lines.size()) { error = "Invalid Scala interval data."; return false; }

    std::vector<TuningPoint> points;
    points.reserve(static_cast<size_t>(count));
    points.push_back({1.0, 0.0});

    auto parseRatio = [](const std::string& s, double& out) -> bool {
        const auto slash = s.find('/');
        if (slash == std::string::npos) return false;
        try {
            const double a = std::stod(s.substr(0, slash));
            const double b = std::stod(s.substr(slash + 1));
            if (b == 0.0 || a <= 0.0) return false;
            out = a / b; return true;
        } catch (...) { return false; }
    };

    for (int i = 0; i < count; ++i) {
        const auto token = lines[static_cast<size_t>(i + 2)];
        double ratio = 0.0;
        if (parseRatio(token, ratio)) {
            points.push_back({ratio, 1200.0 * std::log2(ratio)});
            continue;
        }
        try {
            const double cents = std::stod(token);
            const double r = std::pow(2.0, cents / 1200.0);
            points.push_back({r, cents});
        } catch (...) {
            error = "Could not parse Scala interval: " + token;
            return false;
        }
    }

    // A Scala scale has count intervals and the last interval normally defines the period.
    const double period = points.back().ratio;
    points.pop_back();
    destination.setPeriod(period);
    destination.setPoints(std::move(points));
    return true;
}

bool ScalaParser::loadSclFile(const std::string& path, TuningSystem& destination, std::string& error)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) { error = "Could not open Scala file: " + path; return false; }
    std::ostringstream buffer; buffer << file.rdbuf();
    return loadScl(buffer.str(), destination, error);
}

} // namespace rel
