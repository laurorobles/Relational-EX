#!/bin/bash
set -u
ROOT="$(cd "$(dirname "$0")" && pwd)"
VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
APP_PATH="$ROOT/build/RelationalEX_artefacts/Release/Standalone/Relational EX.app"
VST3_PATH="$ROOT/build/RelationalEX_artefacts/Release/VST3/Relational EX.vst3"

"$ROOT/PREBUILD_CHECK.sh" || exit 1

printf '\n=== BUILDING RELATIONAL EX ===\n\n'
if [[ "${1:-}" != "--skip-build" ]]; then
  cmake --build "$ROOT/build" --config Release --parallel "$(sysctl -n hw.logicalcpu)" || exit 1
fi

[ -d "$VST3_PATH" ] || { echo "ERROR: VST3 bundle not found: $VST3_PATH"; exit 1; }
[ -d "$APP_PATH" ] || { echo "ERROR: Standalone not found: $APP_PATH"; exit 1; }

mkdir -p "$VST3_DIR"
rm -rf "$VST3_DIR/Relational EX.vst3"
cp -R "$VST3_PATH" "$VST3_DIR/Relational EX.vst3"

[ -d "$VST3_DIR/Relational EX.vst3" ] || { echo "ERROR: VST3 installation failed"; exit 1; }

printf '\n=== INSTALLATION COMPLETE ===\n'
printf 'Standalone: %s\n' "$APP_PATH"
printf 'VST3:       %s\n\n' "$VST3_DIR/Relational EX.vst3"

if command -v codesign >/dev/null 2>&1; then
  codesign --force --deep --sign - "$VST3_DIR/Relational EX.vst3" >/dev/null 2>&1 || true
fi
