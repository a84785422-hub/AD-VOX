#pragma once
#include <JuceHeader.h>
class ADVoxAudioProcessor : public juce::AudioProcessor {
public:
 ADVoxAudioProcessor(); ~ADVoxAudioProcessor() override = default;
 void prepareToPlay(double,int) override; void releaseResources() override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 juce::AudioProcessorEditor* createEditor() override; bool hasEditor() const override{return true;}
 const juce::String getName() const override{return JucePlugin_Name;}
 bool acceptsMidi() const override{return false;} bool producesMidi() const override{return false;}
 bool isMidiEffect() const override{return false;} double getTailLengthSeconds() const override{return 2.0;}
 int getNumPrograms() override{return 1;} int getCurrentProgram() override{return 0;}
 void setCurrentProgram(int) override{} const juce::String getProgramName(int) override{return {};}
 void changeProgramName(int,const juce::String&) override{}
 void getStateInformation(juce::MemoryBlock&) override; void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState parameters; void loadPreset(int);
private:
 static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
 juce::dsp::ProcessorChain<juce::dsp::IIR::Filter<float>,juce::dsp::IIR::Filter<float>,juce::dsp::IIR::Filter<float>> eq;
 juce::dsp::Reverb reverb; juce::dsp::Reverb::Parameters reverbParams;
 double sampleRate=44100.0; void updateDsp();
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADVoxAudioProcessor)
};
