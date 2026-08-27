# Contributing to Relational EX

Relational EX is an experimental audio software project.

## Development rules

- Keep realtime audio code allocation-free where practical.
- Never perform network access in `processBlock()`.
- Keep external integrations behind small interfaces.
- Preserve English UI text.
- Add hover help for every interactive control.
- Update `Docs/DECISION_LOG.md` when a design decision changes architecture or UX.
- Update `Docs/IMPLEMENTATION_STATUS.md` when implementation status changes.
- Update the manual when a user-facing control is added, renamed or changed.
- Record third-party sources in `Docs/REFERENCE_BIBLIOGRAPHY.md` and `Docs/THIRD_PARTY_NOTICES.md`.
