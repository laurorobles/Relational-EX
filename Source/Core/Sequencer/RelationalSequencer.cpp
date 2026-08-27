#include "RelationalSequencer.h"
#include <algorithm>

namespace rel {
void RelationalSequencer::setLength(int length)
{
    length_ = std::max(1, length);
    events_.resize(static_cast<size_t>(length_));
    for (int i = 0; i < length_; ++i) events_[static_cast<size_t>(i)].position = static_cast<double>(i) / length_;
}

std::vector<bool> RelationalSequencer::euclideanPattern(int steps, int pulses, int rotation)
{
    steps = std::max(1, steps); pulses = std::clamp(pulses, 0, steps);
    std::vector<bool> result(static_cast<size_t>(steps), false);
    int bucket = 0;
    for (int i = 0; i < steps; ++i) {
        bucket += pulses;
        if (bucket >= steps) { bucket -= steps; result[static_cast<size_t>(i)] = true; }
    }
    if (rotation != 0) {
        std::rotate(result.begin(), result.begin() + ((rotation % steps) + steps) % steps, result.end());
    }
    return result;
}
} // namespace rel
