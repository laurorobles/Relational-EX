#pragma once
#include <algorithm>
#include <cmath>

namespace rel {

class LFO {
public:
    void prepare(double sampleRate) noexcept { sampleRate_ = std::max(1000.0, sampleRate); reset(); }
    void setRate(double hz) noexcept { rate_ = std::clamp(hz, 0.01, 50.0); }
    void setShape(int shape) noexcept { shape_ = std::clamp(shape, 0, 3); }
    void reset() noexcept { phase_ = 0.0; }
    float process() noexcept
    {
        phase_ += rate_ / sampleRate_;
        phase_ -= std::floor(phase_);
        const double p = phase_;
        switch (shape_) {
            case 1: return static_cast<float>(2.0 * p - 1.0); // ramp
            case 2: return static_cast<float>(p < 0.5 ? 1.0 : -1.0); // square
            case 3: return static_cast<float>(4.0 * std::abs(p - 0.5) - 1.0); // triangle
            default: return static_cast<float>(std::sin(2.0 * 3.14159265358979323846 * p));
        }
    }
private:
    double sampleRate_ = 48000.0;
    double rate_ = 1.0;
    double phase_ = 0.0;
    int shape_ = 0;
};

} // namespace rel
