#!/bin/bash
set -u
cd "$(dirname "$0")"
fail=0
check(){ if eval "$2" >/dev/null 2>&1; then echo "[PASS] $1"; else echo "[FAIL] $1"; fail=1; fi; }

echo "=== RELATIONAL PREFLIGHT ==="
check "macOS" 'test "$(uname -s)" = "Darwin"'
check "AppleClang" 'command -v clang'
check "CMake" 'command -v cmake'
check "Xcode Command Line Tools" 'xcode-select -p'
check "JUCE checkout" 'test -f "$HOME/JUCE/CMakeLists.txt"'
check "Project CMakeLists" 'test -f CMakeLists.txt'
check "JUCE header generation call" 'grep -q "juce_generate_juce_header(Relational)" CMakeLists.txt'
check "VST3 user directory" 'mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"'
check "Standalone format" 'grep -q "FORMATS VST3 Standalone" CMakeLists.txt'
check "VST3 copy target" 'grep -q "VST3_COPY_DIR" CMakeLists.txt'
check "Plugin editor implementation" 'grep -q "RelationalProcessor::createEditor" Source/PluginProcessor.cpp'
check "Tooltip implementation" 'grep -q "SettableTooltipClient" Source/UI/TooltipSupport.h'

if [ "$fail" -ne 0 ]; then echo; echo "=== PREFLIGHT FAILED ==="; exit 1; fi
echo; echo "=== PREFLIGHT PASSED ==="
