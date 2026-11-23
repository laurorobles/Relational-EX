#pragma once

#include <JuceHeader.h>
#include "RelationalEvent.h"

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace rel
{

class RelationalSequencer
{
public:
    struct CycleLengths
    {
        int rhythm = 13;
        int pitch = 7;
        int fm = 5;
        int tuning = 11;
        int motion = 17;
    };

    struct CyclePositions
    {
        int rhythm = 0;
        int pitch = 0;
        int fm = 0;
        int tuning = 0;
        int motion = 0;
    };

    RelationalSequencer();

    // Base event pattern ----------------------------------------------------
    void setLength(int length);
    int length() const noexcept { return length_; }

    std::vector<RelationalEvent>& events() noexcept { return events_; }
    const std::vector<RelationalEvent>& events() const noexcept { return events_; }

    // Independent relational cycles ---------------------------------------
    void setCycleLengths(const CycleLengths& lengths) noexcept;
    CycleLengths cycleLengths() const noexcept { return cycleLengths_; }

    void setRhythmCycle(int length) noexcept;
    void setPitchCycle(int length) noexcept;
    void setFMCycle(int length) noexcept;
    void setTuningCycle(int length) noexcept;
    void setMotionCycle(int length) noexcept;

    int rhythmCycle() const noexcept { return cycleLengths_.rhythm; }
    int pitchCycle() const noexcept { return cycleLengths_.pitch; }
    int fmCycle() const noexcept { return cycleLengths_.fm; }
    int tuningCycle() const noexcept { return cycleLengths_.tuning; }
    int motionCycle() const noexcept { return cycleLengths_.motion; }

    void setCyclePhase(int cycle, int phase) noexcept;

    void pushUndoState() {
        if (undoStack_.size() > 16) undoStack_.erase(undoStack_.begin());
        undoStack_.push_back(events_);
    }
    
    bool undo() {
        if (undoStack_.empty()) return false;
        events_ = undoStack_.back();
        undoStack_.pop_back();
        publishToAudio();
        return true;
    }

    // Runtime ---------------------------------------------------------------
    void reset() noexcept;
    int advance() noexcept;

    int currentStep() const noexcept
    {
        return currentStep_.load(std::memory_order_relaxed);
    }

    CyclePositions cyclePositions() const noexcept;

    std::uint64_t cycleTick() const noexcept
    {
        return cycleTick_.load(std::memory_order_relaxed);
    }

    std::uint64_t combinedCycleLength() const noexcept;
    double combinedCycleProgress() const noexcept;
    std::uint64_t stepsUntilAlignment() const noexcept;
    double normalizedComplexity() const noexcept;
    bool isFullyAligned() const noexcept;

    // Pattern generation ---------------------------------------------------
    void applyEuclidean(int pulses, int rotation = 0);
    void applyEuclideanRealtime(int pulses, int rotation = 0) noexcept;
    void generateGenerativePreset();

    static std::vector<bool>
    euclideanPattern(int steps, int pulses, int rotation = 0);

    // Realtime snapshot ----------------------------------------------------
    void publishToAudio() noexcept;

    bool copyEventForAudio(
        int rhythmIndex,
        RelationalEvent& destination) const noexcept;

    bool copyEventForAudio(
        int rhythmIndex,
        const CyclePositions& phase,
        RelationalEvent& destination) const noexcept;

    // Probability ----------------------------------------------------------
    bool shouldTrigger(const RelationalEvent& event) noexcept;

    // Visual helpers -------------------------------------------------------
    double cycleProgress(int cycleLength, int position) const noexcept;
    bool isCycleBoundary(int cycleLength, int position) const noexcept;

private:
    struct Snapshot
    {
        int length = 13;
        std::vector<RelationalEvent> events;
        CycleLengths cycleLengths;
        std::uint64_t generation = 0;
    };

    static int clampCycleLength(int length) noexcept;
    static int wrapIndex(int value, int length) noexcept;

    RelationalEvent resolveRelationalEvent(
        int rhythmIndex,
        const CyclePositions& phase,
        const Snapshot& snapshot) const noexcept;

    int length_ = 13;
    std::vector<RelationalEvent> events_;
    std::vector<std::vector<RelationalEvent>> undoStack_;
    CycleLengths cycleLengths_;

    std::atomic<int> rhythmPosition_{0};
    std::atomic<int> pitchPosition_{0};
    std::atomic<int> fmPosition_{0};
    std::atomic<int> tuningPosition_{0};
    std::atomic<int> motionPosition_{0};
    std::atomic<int> currentStep_{0};
    std::atomic<std::uint64_t> cycleTick_{0};

    std::uint64_t generation_ = 0;

    std::shared_ptr<const Snapshot> audioSnapshot_;

    std::mt19937 randomEngine_{0x52455835u};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RelationalSequencer)
};

} // namespace rel
