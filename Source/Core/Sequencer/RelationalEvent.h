#pragma once

namespace rel {

struct RelationalEvent {
    double position = 0.0;          // 0..1 cycle position
    double pitch = 0.0;             // semitone offset from root
    double ratio = 1.61803398875;   // spectral / FM relationship
    double velocity = 1.0;
    double probability = 1.0;
    double fmIndex = 2.0;
    double tuningOffsetCents = 0.0;
    double timingOffset = 0.0;     // percentage of step
    bool active = true;
};

} // namespace rel
