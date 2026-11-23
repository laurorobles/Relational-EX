#!/usr/bin/env bash
# Build Relational EX (Release). Usage: scripts/build.sh [--install] [--launch] [--clean]
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$ROOT/build"
INSTALL=OFF
LAUNCH=0

for arg in "$@"; do
  case "$arg" in
    --install) INSTALL=ON ;;
    --launch)  LAUNCH=1 ;;
    --clean)   rm -rf "$BUILD_DIR" ;;
    *) echo "Unknown option: $arg" >&2; exit 2 ;;
  esac
done

EXTRA=()
if [[ -n "${JUCE_DIR:-}" ]]; then EXTRA+=("-DJUCE_DIR=$JUCE_DIR"); fi

cmake -S "$ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release \
      -DRELATIONAL_COPY_PLUGIN="$INSTALL" ${EXTRA[@]+"${EXTRA[@]}"}
cmake --build "$BUILD_DIR" --config Release --parallel "$(sysctl -n hw.logicalcpu 2>/dev/null || nproc)"

APP="$(find "$BUILD_DIR" -type d -name 'Relational EX.app' -print -quit)"
VST3="$(find "$BUILD_DIR" -type d -name 'Relational EX.vst3' -print -quit)"
echo
echo "Standalone: ${APP:-<not built>}"
echo "VST3:       ${VST3:-<not built>}"

if [[ "$LAUNCH" == 1 && -n "$APP" ]]; then open "$APP"; fi
