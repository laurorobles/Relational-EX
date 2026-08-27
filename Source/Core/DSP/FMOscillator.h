#pragma once
#include <cmath>
namespace rel {
class FMOscillator {
public:
    void prepare(double sampleRate) { sr_ = sampleRate; }
    void setCarrier(double hz) { carrierHz_ = hz; update(); }
    void setRatio(double ratio) { ratio_ = ratio; modHz_ = carrierHz_ * ratio_; update(); }
    void setIndex(double index) { index_ = index; }
    void setFeedback(double feedback) { feedback_ = feedback; }
    float process() {
        constexpr double twoPi = 6.2831853071795864769;
        modPhase_ += modHz_ / sr_; modPhase_ -= std::floor(modPhase_);
        const double mod = std::sin(twoPi * modPhase_) * index_;
        phase_ += carrierHz_ / sr_; phase_ -= std::floor(phase_);
        const double fb = last_ * feedback_;
        last_ = static_cast<float>(std::sin(twoPi * (phase_ + mod / twoPi + fb)));
        return last_;
    }
private:
    void update() { modHz_ = carrierHz_ * ratio_; }
    double sr_ = 48000.0;
    double carrierHz_ = 220.0, modHz_ = 220.0, ratio_ = 1.0, index_ = 0.0, feedback_ = 0.0;
    double phase_ = 0.0, modPhase_ = 0.0;
    float last_ = 0.0f;
};
}
