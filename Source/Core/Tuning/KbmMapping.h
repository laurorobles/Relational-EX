#pragma once
#include <string>
#include <vector>

namespace rel {

class KbmMapping {
public:
    bool loadText(const std::string& text, std::string& error);
    bool loadFile(const std::string& path, std::string& error);

    bool valid() const noexcept { return valid_; }
    int mapSize() const noexcept { return mapSize_; }
    int firstNote() const noexcept { return firstNote_; }
    int lastNote() const noexcept { return lastNote_; }
    int referenceNote() const noexcept { return referenceNote_; }
    double referenceFrequency() const noexcept { return referenceFrequency_; }
    int referenceDegree() const noexcept { return referenceDegree_; }
    int formalOctave() const noexcept { return formalOctave_; }

    // Returns scale-degree offset for a MIDI note, or -1 if unmapped.
    int degreeForMidiNote(int midiNote) const noexcept;

private:
    int mapSize_ = 0;
    int firstNote_ = 0;
    int lastNote_ = 127;
    int middleNote_ = 60;
    int referenceNote_ = 69;
    double referenceFrequency_ = 440.0;
    int referenceDegree_ = 0;
    int formalOctave_ = 0;
    std::vector<int> mapping_;
    bool valid_ = false;
};

} // namespace rel
