cmake_minimum_required(VERSION 3.22)

project(RelationalEX VERSION 0.3.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JUCE_ROOT "$ENV{HOME}/JUCE")
if (DEFINED JUCE_DIR AND NOT JUCE_DIR STREQUAL "")
    set(JUCE_ROOT "${JUCE_DIR}")
endif()

if (NOT EXISTS "${JUCE_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE was not found at: ${JUCE_ROOT}. Set JUCE_DIR to your JUCE checkout.")
endif()

add_subdirectory("${JUCE_ROOT}" JUCE)

juce_add_plugin(RelationalEX
    COMPANY_NAME "LAO"
    BUNDLE_ID "com.lao.relationalex"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3"
    PLUGIN_MANUFACTURER_CODE LaRo
    PLUGIN_CODE Rlt2
    FORMATS VST3 Standalone
    PRODUCT_NAME "Relational EX"
    PLUGIN_NAME "Relational EX"
)

juce_generate_juce_header(RelationalEX)

target_sources(RelationalEX PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h

    Source/Core/DSP/FMOscillator.h
    Source/Core/DSP/SineOscillator.h
    Source/Core/DSP/ResonatorBank.h

    Source/Core/Tuning/TuningSystem.cpp
    Source/Core/Tuning/TuningSystem.h
    Source/Core/Tuning/ScalaParser.cpp
    Source/Core/Tuning/ScalaParser.h
    Source/Core/Tuning/MidiTuningStandard.cpp
    Source/Core/Tuning/MidiTuningStandard.h
    Source/Core/Tuning/KbmMapping.cpp
    Source/Core/Tuning/KbmMapping.h
    Source/Core/Tuning/MTSClientBridge.cpp
    Source/Core/Tuning/MTSClientBridge.h

    Source/Core/Sequencer/RelationalEvent.h
    Source/Core/Sequencer/RelationalSequencer.cpp
    Source/Core/Sequencer/RelationalSequencer.h
    Source/Core/Relations/RelationEngine.h
    Source/Core/Modulation/LFO.h
    Source/Core/Modulation/ChaosDrift.h

    Source/Core/Presets/PresetLibrary.cpp
    Source/Core/Presets/PresetLibrary.h
    Source/Core/Database/ScaleCatalog.cpp
    Source/Core/Database/ScaleCatalog.h

    Source/UI/TooltipSupport.h
)

target_compile_definitions(RelationalEX PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(RelationalEX PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
)

target_compile_options(RelationalEX PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra;-Wpedantic>
)

# Reproducible developer install for macOS. JUCE's COPY_PLUGIN_AFTER_BUILD is
# still enabled above; this explicit target documents the intended destination
# and gives build_and_install.sh a deterministic path to verify.
set(RELATIONAL_EX_VST3_USER_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3")

# The MTS-ESP macOS dynamic library is loaded from /Library/Application Support/MTS-ESP.
# Standalone development builds therefore opt into Apple's library-validation exception.
if (APPLE AND TARGET RelationalEX_Standalone)
    set_target_properties(RelationalEX_Standalone PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_SOURCE_DIR}/Resources/RelationalEX.entitlements"
    )
endif()

# Copy human-readable scale resources into packaged macOS bundles at build time.
if (APPLE)
    set(RELATIONAL_EX_SCALE_SOURCE "${CMAKE_SOURCE_DIR}/Resources/Scales")
    add_custom_command(TARGET RelationalEX_Standalone POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:RelationalEX_Standalone>/Contents/Resources/Scales"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${RELATIONAL_EX_SCALE_SOURCE}" "$<TARGET_BUNDLE_DIR:RelationalEX_Standalone>/Contents/Resources/Scales"
    )
    add_custom_command(TARGET RelationalEX_VST3 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:RelationalEX_VST3>/Contents/Resources/Scales"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${RELATIONAL_EX_SCALE_SOURCE}" "$<TARGET_BUNDLE_DIR:RelationalEX_VST3>/Contents/Resources/Scales"
    )
endif()
