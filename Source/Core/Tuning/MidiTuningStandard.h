#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace rel {

class MidiTuningStandard {
public:
    MidiTuningStandard();
    void reset12TET(double a4 = 440.0);
    bool parseSysEx(const uint8_t* data, size_t size);
    bool parseSysEx(const std::vector<uint8_t>& bytes) { return parseSysEx(bytes.data(), bytes.size()); }
    double frequencyForNote(int midiNote) const noexcept;
    bool hasPendingChanges() const noexcept { return dirty_; }
    void clearDirty() noexcept { dirty_ = false; }

private:
    std::array<double, 128> frequencies_{};
    bool dirty_ = false;
};

} // namespace rel
