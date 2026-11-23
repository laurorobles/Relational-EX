# Contributing

Thanks for your interest! Relational EX is open source (AGPL-3.0); contributions are welcome.

## Ground rules

- Read [Docs/SCOPE.md](Docs/SCOPE.md) first: the goal is deeper *relationships* between existing domains,
  not more synthesis families.
- **Audio thread rules** (`processBlock` and everything it calls): no allocation, no locks, no file/IO,
  no `std::string`/`juce::String` construction. Communicate with the UI via atomics or lock-free FIFOs.
- Keep the GUI uncluttered; every new control needs a tooltip and an entry in
  [Docs/PARAMETER_REFERENCE.md](Docs/PARAMETER_REFERENCE.md).
- Never rename parameter IDs (it breaks saved sessions); add new ones instead.

## Workflow

1. Fork, create a branch (`feature/<name>` or `fix/<name>`).
2. Build with `./scripts/build.sh` and test in the Standalone and in a DAW.
3. Update `CHANGELOG.md` under *Unreleased*.
4. Open a pull request describing what changed and why.

By contributing you agree your work is licensed under AGPL-3.0.
