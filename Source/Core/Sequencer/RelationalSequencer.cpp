#include "RelationalSequencer.h"

namespace rel {

void RelationalSequencer::setLength(int length)
{
    const int newLength = std::clamp(length, 1, maxEvents);
    editEvents_.resize(static_cast<std::size_t>(newLength));
    for (int i = 0; i < newLength; ++i) {
        auto& e = editEvents_[static_cast<std::size_t>(i)];
        e.position = static_cast<double>(i) / static_cast<double>(newLength);
        e.active = true;
    }
    length_.store(newLength, std::memory_order_release);
    currentStep_.store(-1, std::memory_order_release);
    publishToAudio();
}

void RelationalSequencer::publishToAudio() noexcept
{
    auto snapshot = std::make_shared<Snapshot>();
    const int currentLength = length();
    for (int i = 0; i < maxEvents; ++i)
        (*snapshot)[static_cast<std::size_t>(i)] = (i < currentLength) ? editEvents_[static_cast<std::size_t>(i)] : RelationalEvent{};
    std::atomic_store_explicit(&audioSnapshot_, std::shared_ptr<const Snapshot>(std::move(snapshot)), std::memory_order_release);
}

bool RelationalSequencer::copyEventForAudio(int index, RelationalEvent& destination) const noexcept
{
    if (index < 0 || index >= maxEvents) return false;
    const auto snapshot = std::atomic_load_explicit(&audioSnapshot_, std::memory_order_acquire);
    if (!snapshot || index >= length()) return false;
    destination = (*snapshot)[static_cast<std::size_t>(index)];
    return true;
}

bool RelationalSequencer::shouldTrigger(const RelationalEvent& event) noexcept
{
    if (!event.active) return false;
    std::uniform_real_distribution<double> d(0.0, 1.0);
    return d(rng_) <= std::clamp(event.probability, 0.0, 1.0);
}

std::vector<bool> RelationalSequencer::euclideanPattern(int steps, int pulses, int rotation)
{
    steps = std::clamp(steps, 1, maxEvents);
    pulses = std::clamp(pulses, 0, steps);
    std::vector<bool> result(static_cast<std::size_t>(steps), false);
    int bucket = 0;
    for (int i = 0; i < steps; ++i) {
        bucket += pulses;
        if (bucket >= steps) {
            bucket -= steps;
            result[static_cast<std::size_t>(i)] = true;
        }
    }
    if (rotation != 0) {
        const auto r = ((rotation % steps) + steps) % steps;
        std::rotate(result.begin(), result.begin() + r, result.end());
    }
    return result;
}

void RelationalSequencer::applyEuclidean(int pulses, int rotation)
{
    const auto pattern = euclideanPattern(length(), pulses, rotation);
    for (int i = 0; i < length_.load(std::memory_order_acquire); ++i)
        editEvents_[static_cast<std::size_t>(i)].active = pattern[static_cast<std::size_t>(i)];
    publishToAudio();
}

int RelationalSequencer::advance() noexcept
{
    const auto next = (currentStep_.load(std::memory_order_relaxed) + 1) % std::max(1, length());
    currentStep_.store(next, std::memory_order_release);
    return next;
}

} // namespace rel
