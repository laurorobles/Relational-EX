# Third-Party Notices

Relational EX itself is licensed under **AGPL-3.0** (see `LICENSE`).

## JUCE 9.0.1
Used under the **AGPLv3** option of JUCE's dual license (https://juce.com/legal/juce-9-licence/).
Because of this, any distributed binary of Relational EX must be accompanied by (or offer) its complete corresponding source.
JUCE bundles further components (VST3 SDK — MIT, and others) documented in JUCE's `LICENSE.md`.

## VST3
VST3 is a trademark of Steinberg Media Technologies GmbH. The VST3 SDK shipped with JUCE is MIT-licensed.

## MTS-ESP (ODDSound)
Relational EX does **not** bundle MTS-ESP code. If the user has the MTS-ESP library installed
(`/Library/Application Support/MTS-ESP/libMTS.dylib`), it is loaded dynamically at runtime. Because the
library is loaded by the Standalone app, the dev build uses `Resources/RelationalEX.entitlements`
(disable library validation). MTS-ESP support is **partially validated**.

## Scale files
`Resources/Scales/*.scl` (12-TET, 19-TET, 31-TET, Bohlen-Pierce, Pythagorean) are standard mathematical
tunings written for this project. Scala is the file format by Manuel Op de Coul.
`scripts/sync_scale_library.sh` can download a public library to the user's own machine; those files keep their own licenses and are not redistributed here.
