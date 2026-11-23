#pragma once
#include <algorithm>
#include <cmath>

namespace rel {

class ChaosDrift {
public:
    void setAmount(double amount) noexcept { amount_ = std::clamp(amount, 0.0, 1.0); }
    void reset(double x = 0.3127) noexcept { x_ = std::clamp(x, 0.0001, 0.9999); }
    float process() noexcept
    {
        // Logistic map: deterministic chaos, useful as a controllable modulation source.
        x_ = 3.99 * x_ * (1.0 - x_);
        return static_cast<float>((x_ * 2.0 - 1.0) * amount_);
    }
    float value() const noexcept { return static_cast<float>((x_ * 2.0 - 1.0) * amount_); }
private:
    double x_ = 0.3127;
    double amount_ = 0.0;
};

} // namespace rel
