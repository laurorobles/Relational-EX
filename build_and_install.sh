#!/bin/bash
set -euo pipefail

cd "$(dirname "$0")"
ROOT="$(pwd)"
BUILD="$ROOT/build"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"

echo "=== RELATIONAL BUILD + INSTALL ==="
echo "Project: $ROOT"
echo "JUCE:    $HOME/JUCE"
echo "VST3:    $VST3_DIR"

if [ ! -f "$HOME/JUCE/CMakeLists.txt" ]; then
  echo "ERROR: JUCE not found at $HOME/JUCE"
  exit 1
fi

rm -rf "$BUILD"
cmake -S . -B "$BUILD" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD" --config Release --parallel "$(sysctl -n hw.logicalcpu)"

VST3=$(find "$BUILD" -type d -name 'Relational.vst3' -print -quit)
APP=$(find "$BUILD" -type d -path '*/Standalone/Relational.app' -print -quit)

[ -n "$VST3" ] || { echo "ERROR: Relational.vst3 not found"; exit 1; }
[ -n "$APP" ] || { echo "ERROR: Relational.app not found"; exit 1; }

mkdir -p "$VST3_DIR"
rm -rf "$VST3_DIR/Relational.vst3"
cp -R "$VST3" "$VST3_DIR/Relational.vst3"

[ -d "$VST3_DIR/Relational.vst3/Contents" ] || { echo "ERROR: VST3 install verification failed"; exit 1; }

if command -v codesign >/dev/null 2>&1; then
  codesign --force --deep --sign - "$VST3_DIR/Relational.vst3" >/dev/null 2>&1 || true
  codesign --force --deep --sign - "$APP" >/dev/null 2>&1 || true
fi

echo
echo "=== SUCCESS ==="
echo "Standalone: $APP"
echo "VST3:      $VST3_DIR/Relational.vst3"
echo
echo "Launch Standalone with:"
echo "open "$APP""
