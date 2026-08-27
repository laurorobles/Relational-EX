#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorEditor* RelationalProcessor::createEditor()
{
    return new RelationalEditor(*this);
}

RelationalProcessor::RelationalProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "APVTS", createParameterLayout())
{
    tuning.setEqualTemperament(12);
    tuning.setRootFrequency(440.0);
    mts.registerClient();
    sequencer.setLength(13);
}

juce::AudioProcessorValueTreeState::ParameterLayout RelationalProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<juce::AudioParameterFloat>("level", "Output Level", 0.0f, 1.0f, 0.75f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fmRatio", "FM Ratio", 0.01f, 8.0f, 1.618f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("fmIndex", "FM Index", 0.0f, 20.0f, 2.0f));
    p.push_back(std::make_unique<juce::AudioParameterFloat>("tuningDrift", "Tuning Drift", 0.0f, 50.0f, 0.0f));
    return { p.begin(), p.end() };
}

void RelationalProcessor::prepareToPlay(double sr, int)
{
    sampleRate_ = sr; fm_.prepare(sr);
}
void RelationalProcessor::releaseResources() {}

bool RelationalProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void RelationalProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    buffer.clear();
    const auto ratio = apvts.getRawParameterValue("fmRatio")->load();
    const auto index = apvts.getRawParameterValue("fmIndex")->load();
    const auto level = apvts.getRawParameterValue("level")->load();
    juce::ScopedNoDenormals noDenormals;

    for (const auto metadata : midi) {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            double freq = tuning.frequencyForMidi(msg.getNoteNumber());
            if (mts.available() && mts.hasMaster())
                freq = mts.noteFrequency(msg.getNoteNumber(), msg.getChannel());
            fm_.setCarrier(freq); fm_.setRatio(ratio); fm_.setIndex(index);
            voiceActive_ = true;
        } else if (msg.isNoteOff()) {
            voiceActive_ = false;
        }
    }

    for (int s = 0; s < buffer.getNumSamples(); ++s) {
        const float x = voiceActive_ ? fm_.process() * level * 0.18f : 0.0f;
        buffer.setSample(0, s, x);
        buffer.setSample(1, s, x);
    }
}

void RelationalProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary(*xml, dest);
}

void RelationalProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new RelationalProcessor(); }
