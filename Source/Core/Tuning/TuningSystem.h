#pragma once
#include <algorithm>
#include <cmath>
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
    void setPeriod(double ratio) noexcept;
    void setPoints(std::vector<TuningPoint> points);
    void setRootFrequency(double hz) noexcept;

    double frequencyForMidi(int midiNote) const noexcept;
    double frequencyForDegree(int degree) const noexcept;
    double ratioForDegree(int degree) const noexcept;
    double centsForDegree(int degree) const noexcept;

    // Editable field support. Degree wraps inside one period.
    void setPointRatio(std::size_t index, double ratio);
    void shiftPointCents(std::size_t index, double deltaCents);

    const std::vector<TuningPoint>& points() const noexcept { return points_; }
    double periodRatio() const noexcept { return periodRatio_; }
    double rootFrequency() const noexcept { return rootFrequency_; }
    int degreesPerPeriod() const noexcept { return static_cast<int>(points_.size()); }

private:
    void normalizePoint(TuningPoint& p) noexcept;

    std::vector<TuningPoint> points_;
    double periodRatio_ = 2.0;
    double rootFrequency_ = 440.0;
};

} // namespace rel
