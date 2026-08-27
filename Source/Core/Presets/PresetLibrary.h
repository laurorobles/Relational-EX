#pragma once
#include <string>
#include <vector>

namespace rel {

struct FactoryPreset {
    std::string name;
    std::string category;
    float fmRatio = 1.618f;
    float fmIndex = 2.0f;
    float feedback = 0.0f;
    float output = 0.72f;
    float resonance = 0.0f;
    float bpm = 120.0f;
    float drift = 0.0f;
    float inharmonicity = 0.2f;
    int tuningDivisions = 12;
    int steps = 16;
    int pulses = 8;
};

class PresetLibrary {
public:
    static const std::vector<FactoryPreset>& factoryPresets();
};

} // namespace rel
