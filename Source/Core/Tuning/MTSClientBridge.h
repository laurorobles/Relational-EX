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

    bool available() const noexcept { return library_ != nullptr; }
    bool hasMaster() const noexcept;
    double noteFrequency(int midiNote, int midiChannel = -1) const noexcept;
    const char* scaleName() const noexcept;
    double periodRatio() const noexcept;

private:
    using HasMasterFn = bool (*)();
    using GetTuningTableFn = const double* (*)();
    using GetMultiChannelTuningTableFn = const double* (*)(signed char);
    using UseMultiChannelTuningFn = bool (*)(signed char);
    using GetScaleNameFn = const char* (*)();
    using GetPeriodRatioFn = double (*)();

    void loadSymbols() noexcept;
    const double* tuningTableForChannel(int midiChannel) const noexcept;

    void* library_ = nullptr;
    HasMasterFn hasMasterFn_ = nullptr;
    GetTuningTableFn getTuningTableFn_ = nullptr;
    GetMultiChannelTuningTableFn getMultiChannelTuningTableFn_ = nullptr;
    UseMultiChannelTuningFn useMultiChannelTuningFn_ = nullptr;
    GetScaleNameFn getScaleNameFn_ = nullptr;
    GetPeriodRatioFn getPeriodRatioFn_ = nullptr;
};

} // namespace rel
