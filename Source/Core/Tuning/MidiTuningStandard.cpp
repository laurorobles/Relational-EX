#include "MidiTuningStandard.h"
#include <cmath>

namespace rel {

MidiTuningStandard::MidiTuningStandard() { reset12TET(); }

void MidiTuningStandard::reset12TET(double a4)
{
    for (int n = 0; n < 128; ++n)
        frequencies_[static_cast<size_t>(n)] = a4 * std::pow(2.0, (n - 69) / 12.0);
    dirty_ = true;
    hasTuning_ = false;
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
        hasTuning_ = true;
        return true;
    }

    // Scale/Octave tuning (0x08 = 1-byte, 0x09 = 2-byte format).
    // The last 13 bytes are always the 12 tuning offsets (plus F7).
    if ((d[4] == 0x08 || d[4] == 0x09) && size >= 18) {
        const bool isTwoByte = (d[4] == 0x09);
        const size_t tuningStart = isTwoByte ? (size - 25) : (size - 13);
        
        for (int note = 0; note < 128; ++note) {
            int pitchClass = note % 12;
            double centsOffset = 0.0;
            
            if (isTwoByte) {
                const uint32_t msb = d[tuningStart + (pitchClass * 2)];
                const uint32_t lsb = d[tuningStart + (pitchClass * 2) + 1];
                const int combined = (static_cast<int>(msb) << 7) | lsb;
                centsOffset = (combined - 8192) / 100.0;
            } else {
                centsOffset = static_cast<int>(d[tuningStart + pitchClass]) - 64;
            }
            
            frequencies_[static_cast<size_t>(note)] = 440.0 * std::pow(2.0, ((note - 69) + (centsOffset / 100.0)) / 12.0);
        }
        
        dirty_ = true;
        hasTuning_ = true;
        return true;
    }

    return false;
}

double MidiTuningStandard::frequencyForNote(int midiNote) const noexcept
{
    if (midiNote < 0 || midiNote > 127) return 440.0;
    return frequencies_[static_cast<size_t>(midiNote)];
}

} // namespace rel
