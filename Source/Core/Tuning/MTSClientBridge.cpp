#include "MTSClientBridge.h"
#include <cmath>

#if defined(_WIN32)
 #include <windows.h>
#else
 #include <dlfcn.h>
#endif

namespace rel {
namespace {

template <typename Fn>
Fn loadSymbol(void* library, const char* name)
{
#if defined(_WIN32)
    return reinterpret_cast<Fn>(GetProcAddress(reinterpret_cast<HMODULE>(library), name));
#else
    return reinterpret_cast<Fn>(dlsym(library, name));
#endif
}

void* openMTSLibrary()
{
#if defined(_WIN32)
    return reinterpret_cast<void*>(LoadLibraryA("MTS-ESP.dll"));
#else
    if (auto* h = dlopen("/Library/Application Support/MTS-ESP/libMTS.dylib", RTLD_NOW | RTLD_LOCAL))
        return h;
    if (auto* h = dlopen("/usr/local/lib/libMTS.dylib", RTLD_NOW | RTLD_LOCAL))
        return h;
    if (auto* h = dlopen("/usr/local/lib/libMTS.so", RTLD_NOW | RTLD_LOCAL))
        return h;
    return nullptr;
#endif
}

void closeMTSLibrary(void* library)
{
#if defined(_WIN32)
    if (library) FreeLibrary(reinterpret_cast<HMODULE>(library));
#else
    if (library) dlclose(library);
#endif
}

double midi12TetFrequency(int midiNote) noexcept
{
    return 440.0 * std::pow(2.0, (static_cast<double>(midiNote) - 69.0) / 12.0);
}

} // namespace

MTSClientBridge::MTSClientBridge()
    : library_(openMTSLibrary())
{
    if (library_)
        loadSymbols();
}

MTSClientBridge::~MTSClientBridge()
{
    deregisterClient();
    closeMTSLibrary(library_);
}

void MTSClientBridge::loadSymbols() noexcept
{
    hasMasterFn_ = loadSymbol<HasMasterFn>(library_, "MTS_HasMaster");
    getTuningTableFn_ = loadSymbol<GetTuningTableFn>(library_, "MTS_GetTuningTable");
    getMultiChannelTuningTableFn_ = loadSymbol<GetMultiChannelTuningTableFn>(library_, "MTS_GetMultiChannelTuningTable");
    useMultiChannelTuningFn_ = loadSymbol<UseMultiChannelTuningFn>(library_, "MTS_UseMultiChannelTuning");
    getScaleNameFn_ = loadSymbol<GetScaleNameFn>(library_, "MTS_GetScaleName");
    getPeriodRatioFn_ = loadSymbol<GetPeriodRatioFn>(library_, "MTS_GetPeriodRatio");
}

void MTSClientBridge::registerClient() noexcept
{
    // The installed libMTS library owns global master/client state. The official
    // client wrapper registers a per-plugin object; our bridge intentionally keeps
    // that ABI detail isolated. No registration call is needed for table queries.
}

void MTSClientBridge::deregisterClient() noexcept {}

bool MTSClientBridge::hasMaster() const noexcept
{
    return library_ && hasMasterFn_ ? hasMasterFn_() : false;
}

const double* MTSClientBridge::tuningTableForChannel(int midiChannel) const noexcept
{
    if (!library_)
        return nullptr;

    if (midiChannel >= 0 && midiChannel < 16 && useMultiChannelTuningFn_
        && useMultiChannelTuningFn_(static_cast<signed char>(midiChannel))
        && getMultiChannelTuningTableFn_)
        return getMultiChannelTuningTableFn_(static_cast<signed char>(midiChannel));

    return getTuningTableFn_ ? getTuningTableFn_() : nullptr;
}

double MTSClientBridge::noteFrequency(int midiNote, int midiChannel) const noexcept
{
    if (midiNote < 0 || midiNote > 127)
        return 440.0;

    const double* table = tuningTableForChannel(midiChannel);
    if (table)
    {
        // libMTS exposes inverse frequency values for the tuning table.
        const double inverseFrequency = table[midiNote];
        if (inverseFrequency > 0.0 && std::isfinite(inverseFrequency))
            return 1.0 / inverseFrequency;
    }

    return midi12TetFrequency(midiNote);
}

const char* MTSClientBridge::scaleName() const noexcept
{
    if (getScaleNameFn_)
        if (const auto* name = getScaleNameFn_())
            return name;
    return "12-TET";
}

double MTSClientBridge::periodRatio() const noexcept
{
    if (getPeriodRatioFn_)
        return getPeriodRatioFn_();
    return 2.0;
}

} // namespace rel
