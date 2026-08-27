#include "MidiTuningStandard.h"
#include <cmath>

namespace rel {

MidiTuningStandard::MidiTuningStandard() { reset12TET(); }

void MidiTuningStandard::reset12TET(double a4)
{
    for (int n = 0; n < 128; ++n)
        frequencies_[static_cast<size_t>(n)] = a4 * std::pow(2.0, (n - 69) / 12.0);
    dirty_ = true;
}

bool MidiTuningStandard::parseSysEx(const uint8_t* d, size_t size)
{
    if (d == nullptr || size < 7 || d[0] != 0xF0 || d[size - 1] != 0xF7) return false;
    // Universal real-time / non-real-time MIDI Tuning Standard messages.
    if (d[1] != 0x7F && d[1] != 0x7E) return false;
    if (d[3] != 0x08) return false; // MIDI Tuning Standard

    // Single Note Tuning Change, no bank: F0 7F 08 02 tt ll [kk xx yy zz]... F7
    if (d[4] == 0x02) {
        if (size < 9) return false;
        const int count = d[6];
        const size_t needed = 8u + static_cast<size_t>(count) * 4u;
        if (size < needed) return false;
        size_t p = 7;
        for (int i = 0; i < count; ++i) {
            const int note = d[p++];
            const uint32_t a = static_cast<uint32_t>(d[p++]);
            const uint32_t b = static_cast<uint32_t>(d[p++]);
            const uint32_t c = static_cast<uint32_t>(d[p++]);
            const uint32_t packed = (a << 14) | (b << 7) | c;
            if (note > 127) continue;
            // MTS encodes frequency as a 21-bit fixed-point representation in fractional semitones.
            const double semitoneFraction = static_cast<double>(packed) / 16384.0;
            frequencies_[static_cast<size_t>(note)] = 440.0 * std::pow(2.0, ((note - 69) + semitoneFraction) / 12.0);
        }
        dirty_ = true;
        return true;
    }

    // Scale/Octave tuning, one-byte cent format: store as offsets from 12-TET.
    if (d[4] == 0x08 && size >= 24) {
        const int channel = d[5] & 0x0F;
        (void) channel;
        // Detailed variants differ in payload layout; leave the table untouched here.
        return false;
    }

    return false;
}

double MidiTuningStandard::frequencyForNote(int midiNote) const noexcept
{
    if (midiNote < 0 || midiNote > 127) return 440.0;
    return frequencies_[static_cast<size_t>(midiNote)];
}

} // namespace rel
