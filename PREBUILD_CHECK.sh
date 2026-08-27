#!/bin/bash
set -u
ROOT="$(cd "$(dirname "$0")" && pwd)"
JUCE_ROOT="${JUCE_DIR:-$HOME/JUCE}"
FAIL=0

pass(){ printf '  [OK] %s\n' "$1"; }
fail(){ printf '  [FAIL] %s\n' "$1"; FAIL=1; }

printf '\n=== RELATIONAL EX PREFLIGHT v0.3.2 ===\n\n'
command -v cmake >/dev/null && pass "CMake found" || fail "CMake not found"
command -v clang >/dev/null && pass "clang found" || fail "clang not found"
command -v xcrun >/dev/null && xcrun --find clang >/dev/null 2>&1 && pass "Apple Clang toolchain" || fail "Xcode Command Line Tools"
[ -f "$JUCE_ROOT/CMakeLists.txt" ] && pass "JUCE at $JUCE_ROOT" || fail "JUCE not found at $JUCE_ROOT"
[ -f "$ROOT/CMakeLists.txt" ] && pass "Project CMakeLists" || fail "Project CMakeLists missing"
[ -f "$ROOT/Source/PluginProcessor.cpp" ] && pass "Processor source" || fail "Processor source missing"
[ -f "$ROOT/Source/PluginEditor.cpp" ] && pass "Editor source" || fail "Editor source missing"
[ -d "$HOME/Library/Audio/Plug-Ins/VST3" ] || mkdir -p "$HOME/Library/Audio/Plug-Ins/VST3"
[ -d "$HOME/Library/Audio/Plug-Ins/VST3" ] && pass "User VST3 directory" || fail "Cannot create user VST3 directory"

if [ "$FAIL" -eq 0 ]; then
  rm -rf "$ROOT/build"
  cmake -S "$ROOT" -B "$ROOT/build" -DCMAKE_BUILD_TYPE=Release >/tmp/relational_cmake.log 2>&1 || { cat /tmp/relational_cmake.log; fail "CMake configure"; }
fi

if [ "$FAIL" -eq 0 ]; then
  printf '\n=== PREFLIGHT PASSED ===\nSafe to run: ./build_and_install.sh\n\n'
else
  printf '\n=== PREFLIGHT FAILED ===\nFix the failed checks before building.\n\n'
fi
exit "$FAIL"
