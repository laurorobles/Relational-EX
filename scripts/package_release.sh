#!/usr/bin/env bash
# Package the Release build into dist/ as a zip ready to upload to a GitHub Release.
# Usage: scripts/package_release.sh   (run scripts/build.sh first)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
VERSION="$(sed -n 's/^project(RelationalEX VERSION \([0-9.]*\).*/\1/p' "$ROOT/CMakeLists.txt")"
APP="$(find "$ROOT/build" -type d -name 'Relational EX.app' -print -quit)"
VST3="$(find "$ROOT/build" -type d -name 'Relational EX.vst3' -print -quit)"

[[ -n "$APP" && -n "$VST3" ]] || { echo "Build artefacts missing. Run scripts/build.sh first." >&2; exit 1; }

NAME="RelationalEX-v${VERSION}-macOS"
STAGE="$ROOT/dist/$NAME"
rm -rf "$ROOT/dist"
mkdir -p "$STAGE"

ditto "$APP"  "$STAGE/Relational EX.app"
ditto "$VST3" "$STAGE/Relational EX.vst3"
cp "$ROOT/LICENSE" "$ROOT/README.md" "$ROOT/CHANGELOG.md" "$STAGE/"

( cd "$ROOT/dist" && ditto -c -k --keepParent "$NAME" "$NAME.zip" && shasum -a 256 "$NAME.zip" > "$NAME.zip.sha256" )

echo "Created dist/$NAME.zip"
cat "$ROOT/dist/$NAME.zip.sha256"
echo
echo "Binaries are ad-hoc signed only. For wide distribution, sign with a Developer ID and notarize:"
echo "  xcrun notarytool submit dist/$NAME.zip --keychain-profile <profile> --wait"
