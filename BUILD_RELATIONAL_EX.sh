#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
./PREBUILD_CHECK.sh
cmake --build build --config Release --parallel "$(sysctl -n hw.logicalcpu)"
./build_and_install.sh --skip-build
APP="$ROOT/build/RelationalEX_artefacts/Release/Standalone/Relational EX.app"
if [[ "${1:-}" == "--launch" && -d "$APP" ]]; then
  open "$APP"
fi
