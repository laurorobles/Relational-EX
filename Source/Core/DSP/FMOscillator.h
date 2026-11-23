#pragma once
#include <algorithm>
#include <cmath>

namespace rel {

class FMOscillator {
public:
    void prepare(double sampleRate) noexcept { sr_ = std::max(1000.0, sampleRate); }
    void setCarrier(double hz) noexcept { targetCarrierHz_ = std::max(0.1, hz); }
    void setRatio(double ratio) noexcept { targetRatio_ = std::clamp(ratio, 0.001, 32.0); }
    void setIndex(double index) noexcept { targetIndex_ = std::clamp(index, 0.0, 50.0); }
    void setFeedback(double feedback) noexcept { targetFeedback_ = std::clamp(feedback, 0.0, 0.95); }
    
    void reset() noexcept { 
        phase_ = modPhase_ = 0.0; 
        last_ = 0.0f; 
        ratio_ = targetRatio_;
        index_ = targetIndex_;
        feedback_ = targetFeedback_;
        carrierHz_ = targetCarrierHz_;
    }

    float process(float extraPhase = 0.0f) noexcept {
        constexpr double twoPi = 6.2831853071795864769;
        
        // One-pole smoothing
        ratio_ += (targetRatio_ - ratio_) * 0.005;
        index_ += (targetIndex_ - index_) * 0.005;
        feedback_ += (targetFeedback_ - feedback_) * 0.005;
        carrierHz_ += (targetCarrierHz_ - carrierHz_) * 0.005;
        
        modHz_ = carrierHz_ * ratio_;

        modPhase_ += modHz_ / sr_; modPhase_ -= std::floor(modPhase_);
        const double mod = std::sin(twoPi * modPhase_) * index_;
        phase_ += carrierHz_ / sr_; phase_ -= std::floor(phase_);
        const double fb = static_cast<double>(last_) * feedback_;
        last_ = static_cast<float>(std::sin(twoPi * (phase_ + extraPhase + mod / twoPi + fb)));
        return last_;
    }

private:
    double sr_ = 48000.0;
    double carrierHz_ = 220.0;
    double targetCarrierHz_ = 220.0;
    double modHz_ = 220.0;
    double ratio_ = 1.0;
    double targetRatio_ = 1.0;
    double index_ = 0.0;
    double targetIndex_ = 0.0;
    double feedback_ = 0.0;
    double targetFeedback_ = 0.0;
    double phase_ = 0.0;
    double modPhase_ = 0.0;
    float last_ = 0.0f;
};

} // namespace rel
