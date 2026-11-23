#pragma once
#include <array>
#include <cmath>
#include <cstddef>

namespace rel {

class ResonatorBank {
public:
    void prepare(double sampleRate) noexcept { sampleRate_ = std::max(1000.0, sampleRate); }

    void setFundamental(double hz) noexcept { targetFundamental_ = std::max(1.0, hz); }
    void setInharmonicity(double amount) noexcept { targetInharmonicity_ = std::clamp(amount, 0.0, 1.0); }
    void setDamping(double damping) noexcept { targetDamping_ = std::clamp(damping, 0.001, 1.0); }
    void reset() noexcept { 
        modes_.fill({0.0, 0.0, 0.0, 0.0}); 
        fundamental_ = targetFundamental_;
        inharmonicity_ = targetInharmonicity_;
        damping_ = targetDamping_;
    }

    void excite(float amount = 1.0f) noexcept {
        for (auto& m : modes_)
            m.velocity += amount;
    }

    float process() noexcept {
        constexpr double twoPi = 6.2831853071795864769;
        
        // One-pole smoothing for targets
        fundamental_ += (targetFundamental_ - fundamental_) * 0.005;
        inharmonicity_ += (targetInharmonicity_ - inharmonicity_) * 0.005;
        damping_ += (targetDamping_ - damping_) * 0.005;

        // Rebuild frequencies smoothly
        static constexpr std::array<double, 6> base = {1.0, 2.01, 3.02, 4.08, 5.15, 6.31};
        for (std::size_t i = 0; i < modes_.size(); ++i) {
            const double ratio = base[i] + (base[i] - (i + 1.0)) * inharmonicity_;
            modes_[i].frequency = fundamental_ * ratio;
        }

        double out = 0.0;
        for (auto& m : modes_) {
            if (m.frequency <= 0.0 || std::abs(m.velocity) < 1.0e-9)
                continue;
            const double phaseInc = m.frequency / sampleRate_;
            m.phase += phaseInc;
            m.phase -= std::floor(m.phase);
            out += std::sin(twoPi * m.phase) * m.velocity;
            m.velocity *= damping_;
        }
        return static_cast<float>(out * 0.18);
    }

private:
    struct Mode { double frequency = 0.0; double targetFrequency = 0.0; double phase = 0.0; double velocity = 0.0; };

    double sampleRate_ = 48000.0;
    double fundamental_ = 220.0;
    double targetFundamental_ = 220.0;
    double inharmonicity_ = 0.25;
    double targetInharmonicity_ = 0.25;
    double damping_ = 0.995;
    double targetDamping_ = 0.995;
    std::array<Mode, 6> modes_{};
};

} // namespace rel
