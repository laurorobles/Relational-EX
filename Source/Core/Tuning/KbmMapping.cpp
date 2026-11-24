#include "KbmMapping.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <locale>
#include <sstream>

namespace rel {
namespace {
std::string trim(std::string s) {
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    std::size_t i = 0; while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(i);
}
std::vector<std::string> contentLines(const std::string& text) {
    std::istringstream in(text); std::string line; std::vector<std::string> out;
    while (std::getline(in, line)) { line = trim(line); if (!line.empty() && line[0] != '!') out.push_back(line); }
    return out;
}
bool parseDoubleClassic(const std::string& s, double& out) {
    std::istringstream ss(s);
    ss.imbue(std::locale::classic());
    double v = 0.0;
    if (!(ss >> v) || !std::isfinite(v)) return false;
    out = v;
    return true;
}
}

bool KbmMapping::loadText(const std::string& text, std::string& error)
{
    const auto lines = contentLines(text);
    if (lines.size() < 8) { error = "Invalid KBM: missing header fields."; return false; }
    try {
        mapSize_ = std::stoi(lines[0]);
        firstNote_ = std::stoi(lines[1]);
        lastNote_ = std::stoi(lines[2]);
        middleNote_ = std::stoi(lines[3]);
        referenceNote_ = std::stoi(lines[4]);
        if (!parseDoubleClassic(lines[5], referenceFrequency_)) { error = "Invalid KBM reference frequency."; return false; }
        referenceDegree_ = std::stoi(lines[6]);
        formalOctave_ = std::stoi(lines[7]);
    } catch (...) { error = "Invalid KBM header."; return false; }
    if (mapSize_ < 0 || mapSize_ > 2048 || firstNote_ < 0 || lastNote_ > 127 || firstNote_ > lastNote_
        || referenceFrequency_ <= 0.0 || formalOctave_ < 0) { error = "Invalid KBM values."; return false; }

    mapping_.clear(); mapping_.reserve(static_cast<std::size_t>(mapSize_));
    for (int i = 0; i < mapSize_; ++i) {
        const std::size_t lineIndex = 8u + static_cast<std::size_t>(i);
        if (lineIndex >= lines.size()) { error = "Invalid KBM: mapping table is incomplete."; return false; }
        const std::string token = lines[lineIndex];
        if (token == "x" || token == "X" || token == "-1") mapping_.push_back(-1);
        else {
            try { mapping_.push_back(std::stoi(token)); }
            catch (...) { error = "Invalid KBM mapping entry: " + token; return false; }
        }
    }
    valid_ = true;
    return true;
}

bool KbmMapping::loadFile(const std::string& path, std::string& error)
{
    std::ifstream file(path, std::ios::binary);
    if (!file) { error = "Could not open KBM file: " + path; return false; }
    std::ostringstream ss; ss << file.rdbuf(); return loadText(ss.str(), error);
}

int KbmMapping::degreeForMidiNote(int midiNote) const noexcept
{
    if (!valid_ || midiNote < firstNote_ || midiNote > lastNote_ || mapping_.empty()) return -1;
    const int period = mapSize_;
    if (period <= 0 || mapping_.size() != static_cast<std::size_t>(period)) return -1;
    const int relative = midiNote - middleNote_;
    int octave = relative / period;
    int index = relative % period;
    if (index < 0) { index += period; --octave; }
    const int mappingDegree = mapping_[static_cast<std::size_t>(index)];
    if (mappingDegree < 0) return -1;
    return referenceDegree_ + mappingDegree + octave * formalOctave_;
}

} // namespace rel
