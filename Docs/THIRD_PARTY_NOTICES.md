# Third-Party Notices

## JUCE

Relational EX is built with the JUCE framework. Review the exact JUCE version and applicable JUCE 9 licensing terms before any public/commercial release.

## MTS-ESP

MTS-ESP client sources are from ODDSound Ltd. The upstream client files carry a permissive license notice. Keep the original copyright/license notice with redistributed copies.

## Scala scale files

Scale files sourced from public libraries must be audited individually for provenance and redistribution permissions before inclusion in a commercial release bundle.

## macOS library validation note

MTS-ESP's official macOS documentation states that standalone client/master applications need the Disable Library Validation entitlement to load the MTS-ESP dynamic library. The development Standalone target therefore references `Resources/RelationalEX.entitlements`. Distribution signing policy must be reviewed before release.
