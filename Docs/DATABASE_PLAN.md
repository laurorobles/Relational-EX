# Relational EX — Scale Database Plan

## Goal

Provide a searchable local catalogue of tuning systems without making the audio engine depend on internet access.

## Sources

Primary interchange format: Scala `.scl`.
Optional mapping format: `.kbm`.
Research sources may include public scale collections such as `narenratan/scale-library`.

## Runtime architecture

```text
External source
      ↓
Sync tool (non-realtime)
      ↓
Local catalogue
      ↓
Metadata index
      ↓
Tuning Browser
      ↓
TuningSystem
      ↓
Realtime frequency snapshot
```

## Rules

- Never perform network access from the audio thread.
- Never parse a large scale collection on the audio thread.
- Treat third-party scale files as content with provenance and licensing metadata.
- Keep the plugin usable offline.
- Cache imported files locally.

## Planned metadata

- display name
- file path / source URL
- source collection
- author where provided
- period ratio
- number of degrees
- interval representation
- tags
- license/provenance note
- import date

## Synchronization

`Tools/sync_scale_library.sh` is a development tool. It should populate a local scale directory only after the user understands the source's redistribution terms.

## Future database options

The first implementation can remain filesystem/index based. SQLite becomes justified when search facets, aliases, ratings, user collections and tens of thousands of entries materially improve the workflow.
