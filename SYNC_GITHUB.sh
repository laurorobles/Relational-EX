#!/bin/bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"
REMOTE="${1:-git@github.com:laurorobles/Relational-EX.git}"
BRANCH="main"

git init >/dev/null 2>&1 || true
git branch -M "$BRANCH"
if git remote get-url origin >/dev/null 2>&1; then
  git remote set-url origin "$REMOTE"
else
  git remote add origin "$REMOTE"
fi

# Bring in remote history when it already exists. Never force-push and never overwrite remote commits.
git fetch origin "$BRANCH" >/dev/null 2>&1 || true
if git rev-parse --verify "origin/$BRANCH" >/dev/null 2>&1; then
  if git rev-parse --verify HEAD >/dev/null 2>&1; then
    if git merge-base --is-ancestor HEAD "origin/$BRANCH"; then
      git merge --ff-only "origin/$BRANCH" >/dev/null
    elif ! git merge-base --is-ancestor "origin/$BRANCH" HEAD; then
      echo "ERROR: Local and GitHub histories have diverged. No force-push was performed."
      echo "Resolve the divergence manually, then rerun this script."
      exit 1
    fi
  else
    git reset --hard "origin/$BRANCH" >/dev/null
  fi
fi

if ! git config user.name >/dev/null; then git config user.name "Relational EX Development"; fi
if ! git config user.email >/dev/null; then git config user.email "relational-ex@localhost"; fi

git add -A
if ! git diff --cached --quiet; then
  git commit -m "Sync Relational EX development tree"
fi

git push -u origin "$BRANCH"
echo "Relational EX synchronized with $REMOTE ($BRANCH)."
