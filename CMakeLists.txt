cmake_minimum_required(VERSION 3.22)

project(Relational VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# JUCE: allow an override, otherwise use the standard developer checkout.
set(JUCE_ROOT "$ENV{HOME}/JUCE")
if (DEFINED JUCE_DIR AND NOT JUCE_DIR STREQUAL "")
    set(JUCE_ROOT "${JUCE_DIR}")
endif()

if (NOT EXISTS "${JUCE_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE was not found at: ${JUCE_ROOT}. Set JUCE_DIR to your JUCE checkout.")
endif()

add_subdirectory("${JUCE_ROOT}" JUCE)

juce_add_plugin(Relational
    COMPANY_NAME "LAO"
    BUNDLE_ID "com.lao.relational"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3"
    PLUGIN_MANUFACTURER_CODE LaRo
    PLUGIN_CODE Relt
    FORMATS VST3 Standalone
    PRODUCT_NAME "Relational"
)

juce_generate_juce_header(Relational)

target_sources(Relational PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Core/Tuning/TuningSystem.cpp
    Source/Core/Tuning/TuningSystem.h
    Source/Core/Tuning/ScalaParser.cpp
    Source/Core/Tuning/ScalaParser.h
    Source/Core/Tuning/MidiTuningStandard.cpp
    Source/Core/Tuning/MidiTuningStandard.h
    Source/Core/Tuning/MTSClientBridge.cpp
    Source/Core/Tuning/MTSClientBridge.h
    Source/Core/Sequencer/RelationalEvent.h
    Source/Core/Sequencer/RelationalSequencer.cpp
    Source/Core/Sequencer/RelationalSequencer.h
    Source/Core/DSP/SineOscillator.h
    Source/Core/DSP/FMOscillator.h
    Source/UI/TooltipSupport.h
)

target_compile_definitions(Relational PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Relational PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
)

# Keep the build warning-clean for our own sources without making JUCE noisy.
target_compile_options(Relational PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>
)
