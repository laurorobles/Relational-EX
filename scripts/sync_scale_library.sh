#!/bin/bash
set -euo pipefail
# Must match JUCE's userApplicationDataDirectory used by the plugin.
case "$(uname -s)" in
  Darwin) DEST="$HOME/Library/RELATIONAL EX/Scales" ;;
  MINGW*|MSYS*|CYGWIN*) DEST="${APPDATA}/RELATIONAL EX/Scales" ;;
  *) DEST="${XDG_CONFIG_HOME:-$HOME/.config}/RELATIONAL EX/Scales" ;;
esac
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$DEST"

if ! command -v git >/dev/null 2>&1; then
  echo "git is required to sync the public scale library."
  exit 1
fi

git clone --depth 1 https://github.com/narenratan/scale-library.git "$TMP/scale-library"
find "$TMP/scale-library" -type f \( -iname '*.scl' -o -iname '*.kbm' \) -print0 | while IFS= read -r -d '' f; do
  cp "$f" "$DEST/"
done

COUNT=$(find "$DEST" -maxdepth 1 -type f -iname '*.scl' | wc -l | tr -d ' ')
echo "RELATIONAL scale cache updated: $COUNT .scl files"
echo "Location: $DEST"
