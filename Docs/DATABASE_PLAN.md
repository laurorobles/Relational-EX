# Scale Database & Connectivity Plan

## Design

RELATIONAL ships with a small curated offline core and can maintain a larger local cache.

## Candidate sources

1. Scala archive / Huygens-Fokker ecosystem
2. narenratan/scale-library metadata + .scl files
3. User-provided .scl/.kbm/.tun files
4. Optional Wilsonic tuning exports / generated .scl files

## Synchronization behavior

- User initiates `Sync Scale Library` from the UI.
- Background thread performs download/update.
- Download is written to a temporary file.
- Validate checksum/format.
- Update local index atomically.
- Notify UI.
- Audio engine uses the previously loaded snapshot until the user selects the new tuning.

## Metadata

Each scale record should retain:
- source URL
- author/source attribution when present
- filename
- description
- tone count
- period ratio
- tags
- local file hash
- retrieval date

## Commercial distribution

Third-party data licensing must be audited before bundling archives into a commercial installer. Network access is preferable for large optional catalogs when licensing permits.
