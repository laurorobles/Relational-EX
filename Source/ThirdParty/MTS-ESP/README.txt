MTS-ESP integration target

Copy libMTSClient.h and libMTSClient.cpp from the official ODDSound/MTS-ESP Client directory into this folder.
Source: https://github.com/ODDSound/MTS-ESP/tree/main/Client
License: 0BSD as provided by ODDSound.

The implementation should register the plugin as an MTS-ESP client, query tuning for MIDI notes continuously while notes are active, and parse MTS SysEx messages locally when no MTS-ESP master is present.
