cmake_minimum_required(VERSION 3.22)

project(RelationalEX VERSION 0.8.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- JUCE ------------------------------------------------------------------
# Resolution order: -DJUCE_DIR=<path>  ->  ~/JUCE  ->  FetchContent (pinned tag).
option(RELATIONAL_COPY_PLUGIN "Copy the built VST3 to the user plug-in folder" ${APPLE})

set(JUCE_ROOT "")
if (DEFINED JUCE_DIR AND NOT JUCE_DIR STREQUAL "")
    set(JUCE_ROOT "${JUCE_DIR}")
elseif (EXISTS "$ENV{HOME}/JUCE/CMakeLists.txt")
    set(JUCE_ROOT "$ENV{HOME}/JUCE")
endif()

if (JUCE_ROOT)
    if (NOT EXISTS "${JUCE_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR "JUCE was not found at: ${JUCE_ROOT}")
    endif()
    add_subdirectory("${JUCE_ROOT}" JUCE)
else()
    include(FetchContent)
    FetchContent_Declare(JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 9.0.1
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(RelationalEX
    COMPANY_NAME "Extasis Records"
    BUNDLE_ID "com.extasisrecords.relationalex"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${RELATIONAL_COPY_PLUGIN}
    VST3_COPY_DIR "$ENV{HOME}/Library/Audio/Plug-Ins/VST3/Extasis Records"
    PLUGIN_MANUFACTURER_CODE LAOR
    PLUGIN_CODE RLX2
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
    $<$<COMPILE_LANGUAGE:CXX>:-Wall;-Wextra>
)

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
