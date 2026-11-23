#include "RelationalSequencer.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <numeric>

namespace rel
{

RelationalSequencer::RelationalSequencer()
{
    setLength(length_);
    reset();
    publishToAudio();
}

int RelationalSequencer::clampCycleLength(int length) noexcept
{
    return std::clamp(length, 1, 32);
}

int RelationalSequencer::wrapIndex(int value, int length) noexcept
{
    if (length <= 0)
        return 0;

    const int result = value % length;
    return result < 0 ? result + length : result;
}

void RelationalSequencer::setLength(int length)
{
    length_ = std::clamp(length, 1, 128);
    events_.resize(static_cast<std::size_t>(length_));

    for (int i = 0; i < length_; ++i)
    {
        auto& event = events_[static_cast<std::size_t>(i)];
        event.position = static_cast<double>(i)
            / static_cast<double>(std::max(1, length_));

        if (!std::isfinite(event.ratio) || event.ratio <= 0.0)
            event.ratio = 1.0;

        if (!std::isfinite(event.velocity))
            event.velocity = 1.0;
        event.velocity = std::clamp(event.velocity, 0.0, 1.0);

        if (!std::isfinite(event.probability))
            event.probability = 1.0;
        event.probability = std::clamp(event.probability, 0.0, 1.0);

        if (!std::isfinite(event.fmIndex) || event.fmIndex < 0.0)
            event.fmIndex = 2.0;

        if (!std::isfinite(event.tuningOffsetCents))
            event.tuningOffsetCents = 0.0;

        if (!std::isfinite(event.timingOffset))
            event.timingOffset = 0.0;
    }

    reset();
    ++generation_;
}

void RelationalSequencer::setCycleLengths(
    const CycleLengths& lengths) noexcept
{
    cycleLengths_.rhythm = clampCycleLength(lengths.rhythm);
    cycleLengths_.pitch = clampCycleLength(lengths.pitch);
    cycleLengths_.fm = clampCycleLength(lengths.fm);
    cycleLengths_.tuning = clampCycleLength(lengths.tuning);
    cycleLengths_.motion = clampCycleLength(lengths.motion);

    reset();
    ++generation_;
}

void RelationalSequencer::setRhythmCycle(int length) noexcept
{
    cycleLengths_.rhythm = clampCycleLength(length);
    reset();
    ++generation_;
}

void RelationalSequencer::setPitchCycle(int length) noexcept
{
    cycleLengths_.pitch = clampCycleLength(length);
    reset();
    ++generation_;
}

void RelationalSequencer::setFMCycle(int length) noexcept
{
    cycleLengths_.fm = clampCycleLength(length);
    reset();
    ++generation_;
}

void RelationalSequencer::setTuningCycle(int length) noexcept
{
    cycleLengths_.tuning = clampCycleLength(length);
    reset();
    ++generation_;
}

void RelationalSequencer::setMotionCycle(int length) noexcept
{
    cycleLengths_.motion = clampCycleLength(length);
    reset();
    ++generation_;
}

void RelationalSequencer::setCyclePhase(int cycle, int phase) noexcept
{
    // wrap phase around cycle length safely
    auto wrap = [](int p, int l) { return ((p % l) + l) % std::max(1, l); };
    
    switch (cycle) {
        case 0: rhythmPosition_.store(wrap(phase, cycleLengths_.rhythm), std::memory_order_relaxed); break;
        case 1: pitchPosition_.store(wrap(phase, cycleLengths_.pitch), std::memory_order_relaxed); break;
        case 2: fmPosition_.store(wrap(phase, cycleLengths_.fm), std::memory_order_relaxed); break;
        case 3: tuningPosition_.store(wrap(phase, cycleLengths_.tuning), std::memory_order_relaxed); break;
        case 4: motionPosition_.store(wrap(phase, cycleLengths_.motion), std::memory_order_relaxed); break;
    }
}

void RelationalSequencer::reset() noexcept
{
    rhythmPosition_.store(0, std::memory_order_relaxed);
    pitchPosition_.store(0, std::memory_order_relaxed);
    fmPosition_.store(0, std::memory_order_relaxed);
    tuningPosition_.store(0, std::memory_order_relaxed);
    motionPosition_.store(0, std::memory_order_relaxed);
    currentStep_.store(0, std::memory_order_relaxed);
    cycleTick_.store(0, std::memory_order_relaxed);
}

RelationalSequencer::CyclePositions
RelationalSequencer::cyclePositions() const noexcept
{
    CyclePositions positions;
    positions.rhythm = rhythmPosition_.load(std::memory_order_relaxed);
    positions.pitch = pitchPosition_.load(std::memory_order_relaxed);
    positions.fm = fmPosition_.load(std::memory_order_relaxed);
    positions.tuning = tuningPosition_.load(std::memory_order_relaxed);
    positions.motion = motionPosition_.load(std::memory_order_relaxed);
    return positions;
}

int RelationalSequencer::advance() noexcept
{
    const auto snapshot = std::atomic_load_explicit(
        &audioSnapshot_,
        std::memory_order_acquire);

    if (!snapshot || snapshot->events.empty())
        return -1;

    const auto cycles = snapshot->cycleLengths;
    const int rhythm = rhythmPosition_.load(std::memory_order_relaxed);
    const int pitch = pitchPosition_.load(std::memory_order_relaxed);
    const int fm = fmPosition_.load(std::memory_order_relaxed);
    const int tuning = tuningPosition_.load(std::memory_order_relaxed);
    const int motion = motionPosition_.load(std::memory_order_relaxed);

    rhythmPosition_.store(
        wrapIndex(rhythm + 1, cycles.rhythm),
        std::memory_order_relaxed);
    pitchPosition_.store(
        wrapIndex(pitch + 1, cycles.pitch),
        std::memory_order_relaxed);
    fmPosition_.store(
        wrapIndex(fm + 1, cycles.fm),
        std::memory_order_relaxed);
    tuningPosition_.store(
        wrapIndex(tuning + 1, cycles.tuning),
        std::memory_order_relaxed);
        
    const int nextMotion = wrapIndex(motion + 1, cycles.motion);
    motionPosition_.store(
        nextMotion,
        std::memory_order_relaxed);

    // Relational Euclid: When Motion cycle wraps, modulate density using Tuning phase
    if (nextMotion == 0 && cycles.motion > 1) {
        int dynamicPulses = 1 + (tuning % std::max(1, length_));
        applyEuclideanRealtime(dynamicPulses, rhythm);
        ++generation_; // Trigger UI redraws
    }

    currentStep_.store(rhythm, std::memory_order_relaxed);
    cycleTick_.fetch_add(1, std::memory_order_relaxed);

    return rhythm;
}

std::vector<bool>
RelationalSequencer::euclideanPattern(
    int steps,
    int pulses,
    int rotation)
{
    steps = std::max(1, steps);
    pulses = std::clamp(pulses, 0, steps);

    std::vector<bool> result(
        static_cast<std::size_t>(steps), false);

    if (pulses == 0)
        return result;

    if (pulses == steps)
    {
        std::fill(result.begin(), result.end(), true);
        return result;
    }

    int bucket = 0;

    for (int i = 0; i < steps; ++i)
    {
        bucket += pulses;

        if (bucket >= steps)
        {
            bucket -= steps;
            result[static_cast<std::size_t>(i)] = true;
        }
    }

    const int r = wrapIndex(rotation, steps);

    if (r != 0)
    {
        std::rotate(
            result.begin(),
            result.begin() + r,
            result.end());
    }

    return result;
}

void RelationalSequencer::applyEuclidean(
    int pulses,
    int rotation)
{
    pushUndoState();
    applyEuclideanRealtime(pulses, rotation);
    reset();
    ++generation_;
}

void RelationalSequencer::applyEuclideanRealtime(int pulses, int rotation) noexcept
{
    if (length_ <= 0) return;
    pulses = std::clamp(pulses, 0, length_);
    rotation = wrapIndex(rotation, length_);

    bool pattern[128] = { false };
    
    if (pulses == length_) {
        for (int i = 0; i < length_; ++i) pattern[i] = true;
    } else if (pulses > 0) {
        int bucket = 0;
        for (int i = 0; i < length_; ++i) {
            bucket += pulses;
            if (bucket >= length_) {
                bucket -= length_;
                pattern[i] = true;
            }
        }
    }
    
    // Apply with rotation
    for (int i = 0; i < length_; ++i) {
        int src = wrapIndex(i - rotation, length_);
        auto& event = events_[static_cast<std::size_t>(i)];
        event.active = pattern[src];
        
        event.position = static_cast<double>(i) / static_cast<double>(std::max(1, length_));
        if (!std::isfinite(event.velocity)) event.velocity = 1.0;
        event.velocity = std::clamp(event.velocity, 0.0, 1.0);
        if (!std::isfinite(event.probability)) event.probability = 1.0;
        event.probability = std::clamp(event.probability, 0.0, 1.0);
    }
}

void RelationalSequencer::generateGenerativePreset()
{
    pushUndoState();
    setLength(16);
    applyEuclideanRealtime(11, 2);

    // Use mathematically interesting prime/fibonacci numbers for cycle lengths
    std::array<int, 5> primes = {5, 7, 11, 13, 17};
    setCycleLengths({
        primes[std::rand() % 5],
        primes[std::rand() % 5],
        primes[std::rand() % 5],
        primes[std::rand() % 5],
        primes[std::rand() % 5]
    });

    for (int i = 0; i < length_; ++i)
    {
        auto& event = events_[static_cast<std::size_t>(i)];

        if (!event.active)
            continue;

        // Pentatonic constraints for the base pitches (degrees)
        int pentatonic[5] = {0, 2, 4, 7, 9};
        event.pitch = static_cast<double>(pentatonic[i % 5] + ((i % 3 == 0) ? -12 : 0));

        const double oscillation =
            0.5 + 0.5 * std::sin(static_cast<double>(i) * 1.61803398875);

        event.fmIndex =
            std::clamp(2.0 + oscillation * 4.0, 0.0, 50.0);

        // Harmonic ratios based on overtone series
        event.ratio =
            (i % 2 == 0) ? 2.0 : (i % 3 == 0) ? 1.5 : 3.0;

        event.probability =
            (i % 4 == 0) ? 1.0 : 0.85;

        event.tuningOffsetCents =
            (i % 5 == 0) ? 17.0 : (i % 7 == 0) ? -12.0 : 0.0;

        event.timingOffset =
            (i % 3 == 0) ? 0.015 : (i % 2 == 0) ? -0.01 : 0.0;
    }

    reset();
    ++generation_;
    publishToAudio();
}

std::uint64_t
RelationalSequencer::combinedCycleLength() const noexcept
{
    std::uint64_t result = 1;

    const int lengths[] = {
        cycleLengths_.rhythm,
        cycleLengths_.pitch,
        cycleLengths_.fm,
        cycleLengths_.tuning,
        cycleLengths_.motion
    };

    for (const int length : lengths)
    {
        const std::uint64_t a = result;
        const std::uint64_t b =
            static_cast<std::uint64_t>(std::max(1, length));
        const std::uint64_t divisor = std::gcd(a, b);

        if (divisor == 0
            || a / divisor
                > std::numeric_limits<std::uint64_t>::max() / b)
        {
            return std::numeric_limits<std::uint64_t>::max();
        }

        result = (a / divisor) * b;
    }

    return result;
}

double RelationalSequencer::combinedCycleProgress() const noexcept
{
    const auto period = combinedCycleLength();

    if (period <= 1)
        return 0.0;

    const auto position = cycleTick() % period;

    return static_cast<double>(position)
        / static_cast<double>(period);
}

std::uint64_t RelationalSequencer::stepsUntilAlignment() const noexcept
{
    const auto period = combinedCycleLength();

    if (period <= 1)
        return 0;

    const auto position = cycleTick() % period;

    return position == 0 ? period : period - position;
}

double RelationalSequencer::normalizedComplexity() const noexcept
{
    const int lengths[] = {
        cycleLengths_.rhythm,
        cycleLengths_.pitch,
        cycleLengths_.fm,
        cycleLengths_.tuning,
        cycleLengths_.motion
    };

    double total = 0.0;

    for (const int length : lengths)
        total += std::log2(static_cast<double>(std::max(1, length)));

    return std::clamp(total / 25.0, 0.0, 1.0);
}

bool RelationalSequencer::isFullyAligned() const noexcept
{
    const auto period = combinedCycleLength();
    return period > 0 && cycleTick() % period == 0;
}

void RelationalSequencer::publishToAudio() noexcept
{
    auto snapshot = std::make_shared<Snapshot>();

    snapshot->length = length_;
    snapshot->events = events_;
    snapshot->cycleLengths = cycleLengths_;
    snapshot->generation = generation_;

    std::shared_ptr<const Snapshot> immutable =
        std::move(snapshot);

    std::atomic_store_explicit(
        &audioSnapshot_,
        std::move(immutable),
        std::memory_order_release);
}

RelationalEvent
RelationalSequencer::resolveRelationalEvent(
    int rhythmIndex,
    const CyclePositions& phase,
    const Snapshot& snapshot) const noexcept
{
    RelationalEvent result{};

    if (snapshot.events.empty())
        return result;

    const int baseLength =
        static_cast<int>(snapshot.events.size());

    const int rhythmSource =
        wrapIndex(rhythmIndex, baseLength);

    const int pitchSource =
        wrapIndex(phase.pitch, baseLength);

    const int fmSource =
        wrapIndex(phase.fm, baseLength);

    const int tuningSource =
        wrapIndex(phase.tuning, baseLength);

    const int motionSource =
        wrapIndex(phase.motion, baseLength);

    const auto& rhythmEvent =
        snapshot.events[static_cast<std::size_t>(rhythmSource)];

    const auto& pitchEvent =
        snapshot.events[static_cast<std::size_t>(pitchSource)];

    const auto& fmEvent =
        snapshot.events[static_cast<std::size_t>(fmSource)];

    const auto& tuningEvent =
        snapshot.events[static_cast<std::size_t>(tuningSource)];

    const auto& motionEvent =
        snapshot.events[static_cast<std::size_t>(motionSource)];

    result = rhythmEvent;
    result.pitch = pitchEvent.pitch;
    result.ratio = fmEvent.ratio;
    result.fmIndex = fmEvent.fmIndex;
    result.tuningOffsetCents = tuningEvent.tuningOffsetCents;
    result.timingOffset = motionEvent.timingOffset;

    return result;
}

bool RelationalSequencer::copyEventForAudio(
    int rhythmIndex,
    RelationalEvent& destination) const noexcept
{
    const auto phase = cyclePositions();
    return copyEventForAudio(
        rhythmIndex,
        phase,
        destination);
}

bool RelationalSequencer::copyEventForAudio(
    int rhythmIndex,
    const CyclePositions& phase,
    RelationalEvent& destination) const noexcept
{
    const auto snapshot = std::atomic_load_explicit(
        &audioSnapshot_,
        std::memory_order_acquire);

    if (!snapshot || snapshot->events.empty())
        return false;

    destination = resolveRelationalEvent(
        rhythmIndex,
        phase,
        *snapshot);

    return true;
}

bool RelationalSequencer::shouldTrigger(
    const RelationalEvent& event) noexcept
{
    if (!event.active)
        return false;

    const double probability =
        std::clamp(event.probability, 0.0, 1.0);

    if (probability >= 1.0)
        return true;

    if (probability <= 0.0)
        return false;

    std::uniform_real_distribution<double>
        distribution(0.0, 1.0);

    return distribution(randomEngine_) <= probability;
}

double RelationalSequencer::cycleProgress(
    int cycleLength,
    int position) const noexcept
{
    cycleLength = clampCycleLength(cycleLength);

    if (cycleLength <= 1)
        return 0.0;

    position = wrapIndex(position, cycleLength);

    return static_cast<double>(position)
        / static_cast<double>(cycleLength);
}

bool RelationalSequencer::isCycleBoundary(
    int cycleLength,
    int position) const noexcept
{
    if (cycleLength <= 0)
        return true;

    return wrapIndex(position, cycleLength) == 0;
}

} // namespace rel
