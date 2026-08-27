#pragma once
#include <cmath>
namespace rel {
class SineOscillator {
public:
    void prepare(double sampleRate) { sr_ = sampleRate; }
    void setFrequency(double hz) { phaseInc_ = hz / sr_; }
    float process(float phaseMod = 0.0f) {
        constexpr double twoPi = 6.2831853071795864769;
        phase_ += phaseInc_;
        phase_ -= std::floor(phase_);
        return static_cast<float>(std::sin(twoPi * (phase_ + phaseMod)));
    }
private:
    double sr_ = 48000.0;
    double phase_ = 0.0;
    double phaseInc_ = 440.0 / 48000.0;
};
}
