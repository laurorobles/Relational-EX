#pragma once
#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace rel {

struct TuningPoint {
    double ratio = 1.0;
    double cents = 0.0;
};

class TuningSystem {
public:
    TuningSystem();

    void setEqualTemperament(int divisions, double periodRatio = 2.0);
    void setPeriod(double ratio);
    void setPoints(std::vector<TuningPoint> points);
    void setRootFrequency(double hz);

    double frequencyForMidi(int midiNote) const;
    double ratioForDegree(int degree) const;
    double centsForDegree(int degree) const;
    const std::vector<TuningPoint>& points() const noexcept { return points_; }
    double periodRatio() const noexcept { return periodRatio_; }
    int degreesPerPeriod() const noexcept { return static_cast<int>(points_.size()); }

private:
    std::vector<TuningPoint> points_;
    double periodRatio_ = 2.0;
    double rootFrequency_ = 440.0;
};

} // namespace rel
