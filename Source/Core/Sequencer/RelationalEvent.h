#pragma once
namespace rel {
struct RelationalEvent {
    double position = 0.0;
    double pitch = 0.0;
    double ratio = 1.0;
    double velocity = 1.0;
    double probability = 1.0;
    double fmIndex = 0.0;
    double tuningOffsetCents = 0.0;
    double timingOffset = 0.0;
};
} // namespace rel
