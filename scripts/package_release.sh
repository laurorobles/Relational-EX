#!/usr/bin/env bash
# Package the Release build into dist/ (zip + sha256) for the current OS.
# Usage: scripts/package_release.sh [build-dir]    (run a Release build first)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${1:-$ROOT/build}"
VERSION="$(sed -n 's/^project(RelationalEX VERSION \([0-9.]*\).*/\1/p' "$ROOT/CMakeLists.txt")"

case "$(uname -s)" in
  Darwin) PLATFORM=macOS ;;
  MINGW*|MSYS*|CYGWIN*) PLATFORM=Windows ;;
  *) PLATFORM=Linux ;;
esac

VST3="$(find "$BUILD" -type d -name 'Relational EX.vst3' -print -quit)"
case "$PLATFORM" in
  macOS)   STANDALONE="$(find "$BUILD" -type d -name 'Relational EX.app' -print -quit)" ;;
  Windows) STANDALONE="$(find "$BUILD" -type f -name 'Relational EX.exe' -print -quit)" ;;
  Linux)   STANDALONE="$(find "$BUILD" -type f -path '*Standalone/Relational EX' -print -quit)" ;;
esac

[[ -n "$VST3" ]] || { echo "VST3 not found in $BUILD. Build first." >&2; exit 1; }

NAME="RelationalEX-v${VERSION}-${PLATFORM}"
STAGE="$ROOT/dist/$NAME"
rm -rf "$ROOT/dist"
mkdir -p "$STAGE"

copy() { if command -v ditto >/dev/null 2>&1; then ditto "$1" "$2"; else cp -R "$1" "$2"; fi; }

copy "$VST3" "$STAGE/Relational EX.vst3"
if [[ -n "${STANDALONE:-}" ]]; then
  if [[ "$PLATFORM" == "macOS" ]]; then
    copy "$STANDALONE" "$STAGE/Relational EX.app"
  else
    # Standalone binary + its bundled Resources/ (scales) sit next to each other.
    mkdir -p "$STAGE/Standalone"
    cp "$STANDALONE" "$STAGE/Standalone/"
    SDIR="$(dirname "$STANDALONE")"
    [[ -d "$SDIR/Resources" ]] && cp -R "$SDIR/Resources" "$STAGE/Standalone/Resources"
  fi
fi
cp "$ROOT/LICENSE" "$ROOT/README.md" "$ROOT/CHANGELOG.md" "$STAGE/"

( cd "$ROOT/dist" && cmake -E tar cf "$NAME.zip" --format=zip "$NAME" )
( cd "$ROOT/dist" && if command -v sha256sum >/dev/null 2>&1; then sha256sum "$NAME.zip"; else shasum -a 256 "$NAME.zip"; fi > "$NAME.zip.sha256" )

echo "Created dist/$NAME.zip"
cat "$ROOT/dist/$NAME.zip.sha256"
