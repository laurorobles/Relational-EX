#include "MTSClientBridge.h"
#include <cmath>
#include <algorithm>

#if defined(_WIN32)
 #ifndef NOMINMAX
  #define NOMINMAX
 #endif
 #ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
 #endif
 #include <windows.h>
#else
 #include <dlfcn.h>
#endif

namespace rel {
namespace {

template <typename Fn>
Fn loadSymbol(void* library, const char* name) noexcept
{
#if defined(_WIN32)
    return library ? reinterpret_cast<Fn>(GetProcAddress(reinterpret_cast<HMODULE>(library), name)) : nullptr;
#else
    return library ? reinterpret_cast<Fn>(dlsym(library, name)) : nullptr;
#endif
}

void* openLibrary()
{
#if defined(_WIN32)
    return reinterpret_cast<void*>(LoadLibraryA("C:\\ProgramData\\MTS-ESP\\libMTS.dll"));
#else
    if (auto* h = dlopen("/Library/Application Support/MTS-ESP/libMTS.dylib", RTLD_NOW | RTLD_LOCAL)) return h;
    if (auto* h = dlopen("/usr/local/lib/libMTS.dylib", RTLD_NOW | RTLD_LOCAL)) return h;
    if (auto* h = dlopen("/usr/local/lib/libMTS.so", RTLD_NOW | RTLD_LOCAL)) return h;
    if (auto* h = dlopen("/usr/lib/libMTS.so", RTLD_NOW | RTLD_LOCAL)) return h;
    return nullptr;
#endif
}

void closeLibrary(void* library)
{
#if defined(_WIN32)
    if (library) FreeLibrary(reinterpret_cast<HMODULE>(library));
#else
    if (library) dlclose(library);
#endif
}

double fallbackFrequency(int midiNote) noexcept
{
    return 440.0 * std::pow(2.0, (static_cast<double>(midiNote) - 69.0) / 12.0);
}

} // namespace

MTSClientBridge::MTSClientBridge() : library_(openLibrary())
{
    if (library_) loadSymbols();
}

MTSClientBridge::~MTSClientBridge()
{
    deregisterClient();
    closeLibrary(library_);
}

void MTSClientBridge::loadSymbols() noexcept
{
    registerFn_ = loadSymbol<RegisterFn>(library_, "MTS_RegisterClient");
    deregisterFn_ = loadSymbol<DeregisterFn>(library_, "MTS_DeregisterClient");
    hasMasterFn_ = loadSymbol<HasMasterFn>(library_, "MTS_HasMaster");
    shouldFilterNoteFn_ = loadSymbol<ShouldFilterNoteFn>(library_, "MTS_ShouldFilterNote");
    noteFrequencyFn_ = loadSymbol<NoteFrequencyFn>(library_, "MTS_NoteToFrequency");
    getScaleNameFn_ = loadSymbol<GetScaleNameFn>(library_, "MTS_GetScaleName");
    getPeriodRatioFn_ = loadSymbol<GetPeriodRatioFn>(library_, "MTS_GetPeriodRatio");
    parseMIDIDataFn_ = loadSymbol<ParseMIDIDataFn>(library_, "MTS_ParseMIDIDataU");
    hasReceivedSysExFn_ = loadSymbol<HasReceivedSysExFn>(library_, "MTS_HasReceivedMTSSysEx");
    shouldUpdateLibraryFn_ = loadSymbol<ShouldUpdateLibraryFn>(library_, "MTS_Client_ShouldUpdateLibrary");
}

void MTSClientBridge::registerClient() noexcept
{
    if (!client_ && available()) client_ = registerFn_();
}

void MTSClientBridge::deregisterClient() noexcept
{
    if (client_ && deregisterFn_) {
        deregisterFn_(client_);
        client_ = nullptr;
    }
}

bool MTSClientBridge::hasMaster() const noexcept
{
    return client_ && hasMasterFn_ ? hasMasterFn_(client_) : false;
}

bool MTSClientBridge::shouldFilterNote(int midiNote, int midiChannel) const noexcept
{
    if (!client_ || !shouldFilterNoteFn_ || midiNote < 0 || midiNote > 127) return false;
    const signed char ch = static_cast<signed char>(std::clamp(midiChannel, -1, 15));
    return shouldFilterNoteFn_(client_, static_cast<char>(midiNote), ch);
}

double MTSClientBridge::noteFrequency(int midiNote, int midiChannel) const noexcept
{
    if (!client_ || !noteFrequencyFn_ || midiNote < 0 || midiNote > 127) return fallbackFrequency(midiNote);
    const signed char ch = static_cast<signed char>(std::clamp(midiChannel, -1, 15));
    const double hz = noteFrequencyFn_(client_, static_cast<char>(midiNote), ch);
    return std::isfinite(hz) && hz > 0.0 ? hz : fallbackFrequency(midiNote);
}

void MTSClientBridge::parseMidiData(const unsigned char* data, int size) noexcept
{
    if (client_ && parseMIDIDataFn_ && data && size > 0) parseMIDIDataFn_(client_, data, size);
}

bool MTSClientBridge::hasReceivedMTSSysEx() const noexcept
{
    return client_ && hasReceivedSysExFn_ ? hasReceivedSysExFn_(client_) : false;
}

bool MTSClientBridge::shouldUpdateLibrary() const noexcept
{
    return client_ && shouldUpdateLibraryFn_ ? shouldUpdateLibraryFn_(client_) : false;
}

const char* MTSClientBridge::scaleName() const noexcept
{
    if (client_ && getScaleNameFn_) {
        if (const auto* name = getScaleNameFn_(client_)) return name;
    }
    return "Internal Tuning";
}

double MTSClientBridge::periodRatio() const noexcept
{
    if (client_ && getPeriodRatioFn_) {
        const double ratio = getPeriodRatioFn_(client_);
        if (std::isfinite(ratio) && ratio > 1.0) return ratio;
    }
    return 2.0;
}

} // namespace rel
