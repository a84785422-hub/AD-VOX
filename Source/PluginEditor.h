#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class ADVoxAudioProcessorEditor:public juce::AudioProcessorEditor{
public: explicit ADVoxAudioProcessorEditor(ADVoxAudioProcessor&);void paint(juce::Graphics&)override;void resized()override;
private: ADVoxAudioProcessor& p;juce::ComboBox presets;juce::Label title,subtitle;juce::Slider s[6];juce::Label labels[6];std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att[6];JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADVoxAudioProcessorEditor)
};
