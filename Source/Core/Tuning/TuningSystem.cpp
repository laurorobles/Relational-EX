#include "TuningSystem.h"

namespace rel {

TuningSystem::TuningSystem() { setEqualTemperament(12); }

void TuningSystem::setEqualTemperament(int divisions, double periodRatio)
{
    divisions = std::clamp(divisions, 1, 128);
    periodRatio_ = std::max(1.0000001, periodRatio);
    points_.clear();
    points_.reserve(static_cast<std::size_t>(divisions));
    const double logPeriod = std::log(periodRatio_);
    for (int i = 0; i < divisions; ++i) {
        const double ratio = std::exp(logPeriod * static_cast<double>(i) / divisions);
        points_.push_back({ ratio, 1200.0 * std::log2(ratio) });
    }
}

void TuningSystem::setPeriod(double ratio) noexcept
{
    periodRatio_ = std::max(1.0000001, ratio);
}

void TuningSystem::setPoints(std::vector<TuningPoint> points)
{
    if (points.empty()) points.push_back({ 1.0, 0.0 });
    for (auto& p : points) normalizePoint(p);
    points_ = std::move(points);
}

void TuningSystem::setRootFrequency(double hz) noexcept
{
    rootFrequency_ = std::clamp(hz, 1.0, 20000.0);
}

double TuningSystem::ratioForDegree(int degree) const noexcept
{
    if (points_.empty()) return 1.0;
    const int n = static_cast<int>(points_.size());
    const int octave = degree >= 0 ? degree / n : (degree - (n - 1)) / n;
    const int index = ((degree % n) + n) % n;
    return std::pow(periodRatio_, static_cast<double>(octave)) * points_[static_cast<std::size_t>(index)].ratio;
}

double TuningSystem::centsForDegree(int degree) const noexcept
{
    return 1200.0 * std::log2(ratioForDegree(degree));
}

double TuningSystem::frequencyForDegree(int degree) const noexcept
{
    return rootFrequency_ * ratioForDegree(degree);
}

double TuningSystem::frequencyForMidi(int midiNote) const noexcept
{
    return frequencyForDegree(midiNote - 69);
}

void TuningSystem::setPointRatio(std::size_t index, double ratio)
{
    if (index >= points_.size()) return;
    points_[index].ratio = std::max(0.000001, ratio);
    points_[index].cents = 1200.0 * std::log2(points_[index].ratio);
}

void TuningSystem::shiftPointCents(std::size_t index, double deltaCents)
{
    if (index >= points_.size()) return;
    setPointRatio(index, points_[index].ratio * std::pow(2.0, deltaCents / 1200.0));
}

void TuningSystem::normalizePoint(TuningPoint& p) noexcept
{
    p.ratio = std::max(0.000001, p.ratio);
    p.cents = 1200.0 * std::log2(p.ratio);
}

} // namespace rel
