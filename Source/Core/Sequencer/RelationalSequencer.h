#pragma once
#include "RelationalEvent.h"
#include <array>
#include <atomic>
#include <algorithm>
#include <random>
#include <memory>
#include <vector>

namespace rel {

class RelationalSequencer {
public:
    static constexpr int maxEvents = 32;

    void setLength(int length);
    int length() const noexcept { return length_.load(std::memory_order_acquire); }

    // UI/editor state. The realtime thread never reads this buffer.
    std::vector<RelationalEvent>& events() noexcept { return editEvents_; }
    const std::vector<RelationalEvent>& events() const noexcept { return editEvents_; }

    // Publish an immutable snapshot for the realtime thread.
    void publishToAudio() noexcept;
    bool copyEventForAudio(int index, RelationalEvent& destination) const noexcept;

    void setSteps(int steps) { setLength(steps); }
    void setSeed(unsigned seed) noexcept { rng_.seed(seed); }
    bool shouldTrigger(const RelationalEvent& event) noexcept;
    static std::vector<bool> euclideanPattern(int steps, int pulses, int rotation = 0);
    void applyEuclidean(int pulses, int rotation = 0);
    int advance() noexcept;
    int currentStep() const noexcept { return currentStep_.load(std::memory_order_acquire); }
    void reset() noexcept { currentStep_.store(-1, std::memory_order_release); }

private:
    std::atomic<int> length_{16};
    std::atomic<int> currentStep_{-1};
    std::vector<RelationalEvent> editEvents_;

    using Snapshot = std::array<RelationalEvent, maxEvents>;
    std::shared_ptr<const Snapshot> audioSnapshot_;
    std::mt19937 rng_{0x52454C54u};
};

} // namespace rel
