# Relational EX — Change Management

A change is considered complete only when code, behavior and documentation agree.

## Required update chain

```text
Code
 ↓
Implementation Status
 ↓
Technical Specification / Parameter Reference
 ↓
Manual when user-facing
 ↓
Decision Log when architectural
 ↓
Changelog
 ↓
Repository metadata when version/build identity changes
```

## Versioning

Development builds use `0.x.y-dev` until the first public release. Bugfixes increment the patch component; architectural milestones increment minor; release candidates and public releases freeze plugin identifiers and legal notices.

## Scope guard

New features must fit the four-domain concept: frequency, rhythm, spectrum or motion. A feature that belongs to a generic synth but does not reinforce these relationships should be challenged before implementation.
