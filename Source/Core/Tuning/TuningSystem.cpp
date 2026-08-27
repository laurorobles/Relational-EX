#include "TuningSystem.h"
#include <algorithm>

namespace rel {

TuningSystem::TuningSystem() { setEqualTemperament(12); }

void TuningSystem::setEqualTemperament(int divisions, double periodRatio)
{
    divisions = std::max(1, divisions);
    periodRatio_ = std::max(1.0000001, periodRatio);
    points_.clear();
    points_.reserve(static_cast<size_t>(divisions));
    const double log2Period = std::log2(periodRatio_);
    for (int i = 0; i < divisions; ++i) {
        const double ratio = std::pow(periodRatio_, static_cast<double>(i) / divisions);
        points_.push_back({ ratio, 1200.0 * std::log2(ratio) / log2Period });
    }
}

void TuningSystem::setPeriod(double ratio) { periodRatio_ = std::max(1.0000001, ratio); }
void TuningSystem::setPoints(std::vector<TuningPoint> points)
{
    if (points.empty()) points.push_back({1.0, 0.0});
    points_ = std::move(points);
}
void TuningSystem::setRootFrequency(double hz) { rootFrequency_ = std::max(1.0, hz); }

double TuningSystem::ratioForDegree(int degree) const
{
    if (points_.empty()) return 1.0;
    const int n = static_cast<int>(points_.size());
    const int octave = degree >= 0 ? degree / n : (degree - (n - 1)) / n;
    const int index = ((degree % n) + n) % n;
    return std::pow(periodRatio_, octave) * points_[static_cast<size_t>(index)].ratio;
}

double TuningSystem::centsForDegree(int degree) const
{
    return 1200.0 * std::log2(ratioForDegree(degree));
}

double TuningSystem::frequencyForMidi(int midiNote) const
{
    const int degree = midiNote - 69;
    return rootFrequency_ * ratioForDegree(degree);
}

} // namespace rel
