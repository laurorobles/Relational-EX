#include "PresetLibrary.h"

namespace rel {

const std::vector<FactoryPreset>& PresetLibrary::factoryPresets()
{
    static const std::vector<FactoryPreset> presets = {
        {"INIT / PURE",          "FOUNDATION", 1.000f, 0.0f, 0.00f, 0.72f, 0.00f, 120.0f, 0.0f, 0.05f, 12, 16, 4},
        {"PRIME LATTICE",        "TUNING",     1.414f, 4.8f, 0.08f, 0.70f, 0.62f, 118.0f, 1.7f, 0.62f, 31, 13, 5},
        {"FRACTURED GLASS",      "FM",         2.173f, 8.9f, 0.34f, 0.64f, 0.81f, 126.0f, 3.0f, 0.81f, 19, 16, 7},
        {"PHASE RAIN",            "MOTION",     1.618f, 5.7f, 0.21f, 0.66f, 0.35f, 92.0f, 7.0f, 0.35f, 24, 11, 5},
        {"RATIONAL GHOST",        "RATIO",      1.250f, 3.1f, 0.04f, 0.73f, 0.22f, 110.0f, 0.8f, 0.22f, 17, 17, 7},
        {"TIDAL METAL",            "RESONANT",   2.701f, 10.5f, 0.55f, 0.58f, 0.92f, 78.0f, 4.5f, 0.92f, 12, 15, 7},
        {"PAPER CIRCUIT",          "PERCUSSIVE", 1.330f, 6.2f, 0.12f, 0.68f, 0.48f, 126.0f, 2.5f, 0.48f, 22, 13, 7},
        {"NULL MACHINE",           "EXTREME",    3.141f, 15.0f, 0.72f, 0.50f, 1.00f, 138.0f, 12.0f, 1.00f, 53, 7, 3},
        {"SOFT COLLISION",         "ORGANIC",    1.173f, 2.0f, 0.03f, 0.76f, 0.15f, 88.0f, 9.0f, 0.15f, 12, 12, 5},
        {"ALIEN MARIMBA",          "MODAL",      1.732f, 4.1f, 0.17f, 0.67f, 0.88f, 104.0f, 2.1f, 0.88f, 22, 13, 5},
        {"MICRO POLAROID",         "MINIMAL",    0.731f, 1.6f, 0.00f, 0.74f, 0.40f, 72.0f, 5.0f, 0.40f, 31, 19, 7},
        {"BROKEN GENERATOR",       "CHAOS",      2.718f, 12.0f, 0.81f, 0.48f, 1.00f, 132.0f, 18.0f, 1.00f, 41, 23, 9}
    };
    return presets;
}

} // namespace rel
