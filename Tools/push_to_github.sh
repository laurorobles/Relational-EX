#!/bin/bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

REMOTE_URL="${1:-}"
if [[ -z "$REMOTE_URL" ]]; then
  echo "Usage: ./Tools/push_to_github.sh git@github.com:YOUR_ACCOUNT/Relational-EX.git"
  exit 2
fi

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  git init
  git branch -M main
fi

if ! git config user.name >/dev/null; then git config user.name "Relational EX Development"; fi
if ! git config user.email >/dev/null; then git config user.email "relational-ex@localhost"; fi

git add .
git commit -m "Sync Relational EX documentation and development tree" || true

git remote get-url origin >/dev/null 2>&1 || git remote add origin "$REMOTE_URL"
git branch -M main
git push -u origin main
