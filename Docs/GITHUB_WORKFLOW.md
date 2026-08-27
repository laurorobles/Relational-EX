# Relational EX — GitHub Workflow

Repository: `laurorobles/Relational-EX`

## Local sync

From the repository root:

```bash
./SYNC_GITHUB.sh
```

The script defaults to:

```text
git@github.com:laurorobles/Relational-EX.git
```

You can override the remote explicitly:

```bash
./SYNC_GITHUB.sh git@github.com:laurorobles/Relational-EX.git
```

The script fetches first, fast-forwards when safe, commits local changes, and pushes. It refuses to overwrite divergent history and never uses `git push --force`.

## Build before sync

For development on macOS:

```bash
./BUILD_RELATIONAL_EX.sh
```

To build, install and launch the Standalone in one operation:

```bash
./BUILD_RELATIONAL_EX.sh --launch
```

## CI

`.github/workflows/build-macos.yml` builds the VST3 and Standalone targets on both Apple Silicon and Intel macOS runners. JUCE is pinned to a release revision in CI to avoid tracking a moving dependency.

Before the first public release, freeze the exact JUCE commit, plugin identifiers and distribution-signing policy.
