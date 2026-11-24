#include "ScalaParser.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <locale>
#include <sstream>

namespace rel {
namespace {
std::string trim(std::string s)
{
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    std::size_t i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}

bool parseDoubleClassic(const std::string& token, double& out)
{
    std::istringstream ss(token);
    ss.imbue(std::locale::classic());
    double value = 0.0;
    if (!(ss >> value) || !std::isfinite(value)) return false;
    out = value;
    return true;
}

bool parseRatioToken(const std::string& s, double& out)
{
    const auto slash = s.find('/');
    if (slash == std::string::npos) return false;
    double a = 0.0;
    double b = 0.0;
    if (!parseDoubleClassic(s.substr(0, slash), a)
        || !parseDoubleClassic(s.substr(slash + 1), b)
        || a <= 0.0 || b <= 0.0) {
        return false;
    }
    out = a / b;
    return std::isfinite(out);
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
    if (lines.size() < 3) { error = "Invalid Scala file: missing description or tone count."; return false; }

    int count = 0;
    try { count = std::stoi(lines[1]); } catch (...) { error = "Invalid Scala tone count."; return false; }
    if (count <= 0 || static_cast<std::size_t>(count + 2) > lines.size()) {
        error = "Invalid Scala interval data."; return false;
    }

    std::vector<TuningPoint> points;
    points.reserve(static_cast<std::size_t>(count));
    points.push_back({ 1.0, 0.0 });

    for (int i = 0; i < count; ++i) {
        const auto& token = lines[static_cast<std::size_t>(i + 2)];
        double ratio = 0.0;
        if (parseRatioToken(token, ratio)) {
            points.push_back({ ratio, 1200.0 * std::log2(ratio) });
            continue;
        }
        double cents = 0.0;
        if (parseDoubleClassic(token, cents)) {
            points.push_back({ std::pow(2.0, cents / 1200.0), cents });
        } else {
            error = "Could not parse Scala interval: " + token;
            return false;
        }
    }

    // Final interval is the period in .scl.
    const double period = points.back().ratio;
    points.pop_back();
    if (points.empty()) points.push_back({ 1.0, 0.0 });
    destination.setPeriod(period);
    destination.setPoints(std::move(points));
    return true;
}

bool ScalaParser::loadSclFile(const std::string& path, TuningSystem& destination, std::string& error)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) { error = "Could not open Scala file: " + path; return false; }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return loadScl(buffer.str(), destination, error);
}

} // namespace rel
