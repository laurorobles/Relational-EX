#include "PresetLibrary.h"

namespace rel {

const std::vector<FactoryPreset>& PresetLibrary::factoryPresets()
{
    static const std::vector<FactoryPreset> presets = {
        {"INIT / PURE",          "FOUNDATION", 1.000f, 0.0f, 0.00f, 0.72f, 0.00f, 120.0f, 0.0f, 0.05f, 12, 16, 4, 16, 16, 1, 16, 16},
        {"PRIME LATTICE",        "TUNING",     1.414f, 4.8f, 0.08f, 0.70f, 0.62f, 118.0f, 1.7f, 0.62f, 31, 13, 5, 13, 7, 5, 11, 17},
        {"FRACTURED GLASS",      "FM",         2.173f, 8.9f, 0.34f, 0.64f, 0.81f, 126.0f, 3.0f, 0.81f, 19, 16, 7, 16, 5, 7, 9, 13},
        {"PHASE RAIN",           "MOTION",     1.618f, 5.7f, 0.21f, 0.66f, 0.35f, 92.0f, 7.0f, 0.35f, 24, 11, 5, 11, 7, 13, 5, 17},
        {"RATIONAL GHOST",       "RATIO",      1.250f, 3.1f, 0.04f, 0.73f, 0.22f, 110.0f, 0.8f, 0.22f, 17, 17, 7, 17, 9, 5, 13, 11},
        {"TIDAL METAL",           "RESONANT",   2.701f, 10.5f, 0.55f, 0.58f, 0.92f, 78.0f, 4.5f, 0.92f, 12, 15, 7, 15, 11, 7, 13, 17},
        {"PAPER CIRCUIT",         "PERCUSSIVE", 1.330f, 6.2f, 0.12f, 0.68f, 0.48f, 126.0f, 2.5f, 0.48f, 22, 13, 7, 13, 5, 8, 11, 3},
        {"NULL MACHINE",          "EXTREME",    3.141f, 15.0f, 0.72f, 0.50f, 1.00f, 138.0f, 12.0f, 1.00f, 53, 7, 3, 7, 11, 3, 13, 17},
        {"SOFT COLLISION",        "ORGANIC",    1.173f, 2.0f, 0.03f, 0.76f, 0.15f, 88.0f, 9.0f, 0.15f, 12, 12, 5, 12, 7, 5, 11, 13},
        {"ALIEN MARIMBA",         "MODAL",      1.732f, 4.1f, 0.17f, 0.67f, 0.88f, 104.0f, 2.1f, 0.88f, 22, 13, 5, 13, 7, 5, 11, 17},
        {"MICRO POLAROID",        "MINIMAL",    0.731f, 1.6f, 0.00f, 0.74f, 0.40f, 72.0f, 5.0f, 0.40f, 31, 19, 7, 19, 13, 7, 17, 11},
        {"BROKEN GENERATOR",      "CHAOS",      2.718f, 12.0f, 0.81f, 0.48f, 1.00f, 132.0f, 18.0f, 1.00f, 41, 23, 9, 23, 17, 11, 19, 29},
        {"CRYSTAL CAVE",          "RESONANT",   3.5f,   6.4f, 0.23f, 0.60f, 0.95f, 96.0f,  4.2f, 0.95f, 12, 16, 5, 16, 8, 5, 12, 16},
        {"FIBONACCI SPIRAL",      "TUNING",     1.618f, 3.1f, 0.05f, 0.71f, 0.50f, 108.0f, 1.6f, 0.50f, 55, 34, 13,34, 21,13,55, 34},
        {"DEEP WATER",            "MOTION",     0.500f, 2.2f, 0.01f, 0.78f, 0.11f, 65.0f,  12.0f,0.11f, 24, 16, 8, 16, 12, 8, 24, 16},
        {"NEURAL NET",            "EXTREME",    4.332f, 22.0f,0.95f, 0.40f, 1.00f, 145.0f, 28.0f,1.00f, 72, 11, 3, 11, 5, 3,  7, 13},
        {"EUCLIDEAN CLOCK",       "PERCUSSIVE", 1.000f, 4.0f, 0.00f, 0.70f, 0.33f, 120.0f, 0.5f, 0.33f, 12, 16, 4, 16, 16, 1, 16, 16},
        {"GHOST IN THE MACHINE",  "ORGANIC",    1.995f, 3.8f, 0.07f, 0.75f, 0.28f, 85.0f,  6.6f, 0.28f, 19, 13, 7, 13, 11, 7, 17, 19},
        {"OVERTONE CASCADE",      "FM",         3.000f, 7.5f, 0.42f, 0.62f, 0.77f, 112.0f, 3.3f, 0.77f, 22, 15, 6, 15, 9, 6, 15, 21},
        {"SUBATOMIC PARTICLE",    "MINIMAL",    0.250f, 1.1f, 0.00f, 0.79f, 0.10f, 90.0f,  0.2f, 0.10f, 12, 5,  2, 5,  3, 2,  7,  5}
    };
    return presets;
}

} // namespace rel
