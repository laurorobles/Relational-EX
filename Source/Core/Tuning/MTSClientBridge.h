#pragma once

namespace rel {

class MTSClientBridge {
public:
    MTSClientBridge();
    ~MTSClientBridge();

    MTSClientBridge(const MTSClientBridge&) = delete;
    MTSClientBridge& operator=(const MTSClientBridge&) = delete;

    void registerClient() noexcept;
    void deregisterClient() noexcept;

    bool available() const noexcept { return library_ != nullptr && registerFn_ && deregisterFn_ && hasMasterFn_ && noteFrequencyFn_; }
    bool hasMaster() const noexcept;
    bool shouldFilterNote(int midiNote, int midiChannel) const noexcept;
    double noteFrequency(int midiNote, int midiChannel = -1) const noexcept;
    void parseMidiData(const unsigned char* data, int size) noexcept;
    bool hasReceivedMTSSysEx() const noexcept;
    bool shouldUpdateLibrary() const noexcept;
    const char* scaleName() const noexcept;
    double periodRatio() const noexcept;

private:
    struct MTSClient;
    using RegisterFn = MTSClient* (*)();
    using DeregisterFn = void (*)(MTSClient*);
    using HasMasterFn = bool (*)(MTSClient*);
    using ShouldFilterNoteFn = bool (*)(MTSClient*, char, signed char);
    using NoteFrequencyFn = double (*)(MTSClient*, char, signed char);
    using GetScaleNameFn = const char* (*)(MTSClient*);
    using GetPeriodRatioFn = double (*)(MTSClient*);
    using ParseMIDIDataFn = void (*)(MTSClient*, const unsigned char*, int);
    using HasReceivedSysExFn = bool (*)(MTSClient*);
    using ShouldUpdateLibraryFn = bool (*)(MTSClient*);

    void loadSymbols() noexcept;
    void* library_ = nullptr;
    MTSClient* client_ = nullptr;

    RegisterFn registerFn_ = nullptr;
    DeregisterFn deregisterFn_ = nullptr;
    HasMasterFn hasMasterFn_ = nullptr;
    ShouldFilterNoteFn shouldFilterNoteFn_ = nullptr;
    NoteFrequencyFn noteFrequencyFn_ = nullptr;
    GetScaleNameFn getScaleNameFn_ = nullptr;
    GetPeriodRatioFn getPeriodRatioFn_ = nullptr;
    ParseMIDIDataFn parseMIDIDataFn_ = nullptr;
    HasReceivedSysExFn hasReceivedSysExFn_ = nullptr;
    ShouldUpdateLibraryFn shouldUpdateLibraryFn_ = nullptr;
};

} // namespace rel
