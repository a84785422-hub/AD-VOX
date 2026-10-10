#include "PluginProcessor.h"
#include "PluginEditor.h"
ADVoxAudioProcessor::ADVoxAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,juce::Identifier("AD_VOX_STATE"),createParameterLayout()){}
juce::AudioProcessorValueTreeState::ParameterLayout ADVoxAudioProcessor::createParameterLayout(){
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"GAIN",1},"Output",0.0f,2.0f,1.0f));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"LOW",1},"Low",-12.0f,12.0f,0.0f));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"MID",1},"Mid",-12.0f,12.0f,0.0f));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"HIGH",1},"High",-12.0f,12.0f,0.0f));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"REVERB",1},"Reverb",0.0f,100.0f,18.0f));
 p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"DECAY",1},"Decay",0.1f,8.0f,1.8f)); return p;
}
void ADVoxAudioProcessor::prepareToPlay(double sr,int block){sampleRate=sr;juce::dsp::ProcessSpec s{sr,(juce::uint32)block,(juce::uint32)getTotalNumOutputChannels()};eq.prepare(s);reverb.prepare(s);updateDsp();}
void ADVoxAudioProcessor::releaseResources(){}
bool ADVoxAudioProcessor::isBusesLayoutSupported(const BusesLayout& l)const{auto o=l.getMainOutputChannelSet();return(o==juce::AudioChannelSet::mono()||o==juce::AudioChannelSet::stereo())&&l.getMainInputChannelSet()==o;}
void ADVoxAudioProcessor::updateDsp(){
 auto v=[this](const char* id){return parameters.getRawParameterValue(id)->load();};
 *eq.get<0>().coefficients=*juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate,180.0,0.707,juce::Decibels::decibelsToGain(v("LOW")));
 *eq.get<1>().coefficients=*juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate,1200.0,0.8,juce::Decibels::decibelsToGain(v("MID")));
 *eq.get<2>().coefficients=*juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate,6500.0,0.707,juce::Decibels::decibelsToGain(v("HIGH")));
 reverbParams.roomSize=juce::jlimit(0.1f,0.95f,v("DECAY")/8.0f);reverbParams.damping=0.55f;reverbParams.wetLevel=v("REVERB")/100.0f;reverbParams.dryLevel=1.0f-reverbParams.wetLevel*0.45f;reverbParams.width=1.0f;reverbParams.freezeMode=0.0f;reverb.setParameters(reverbParams);
}
void ADVoxAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer& m){juce::ignoreUnused(m);juce::ScopedNoDenormals n;for(int c=getTotalNumInputChannels();c<getTotalNumOutputChannels();++c)b.clear(c,0,b.getNumSamples());updateDsp();juce::dsp::AudioBlock<float> block(b);juce::dsp::ProcessContextReplacing<float> ctx(block);eq.process(ctx);reverb.process(ctx);b.applyGain(parameters.getRawParameterValue("GAIN")->load());}
void ADVoxAudioProcessor::loadPreset(int i){
 struct P{float l,m,h,w,d,g;};static constexpr P ps[20]={{0,1,2,12,1.2f,1},{-2,2,4,18,1.8f,1},{2,0,1,10,1,1},{-3,1,5,24,2.4f,.95f},{0,-2,3,30,3,.95f},{1,2,2,15,1.4f,1},{-1,3,5,20,1.8f,.95f},{2,-1,4,28,2.8f,.95f},{-2,4,3,16,1.5f,1},{1,1,6,22,2,.9f},{0,-3,5,35,3.6f,.9f},{3,0,2,14,1.3f,1},{-4,2,6,25,2.2f,.9f},{1,4,4,18,1.6f,.95f},{-1,0,7,32,3.2f,.9f},{2,2,5,20,1.9f,.95f},{-3,-1,4,38,4.2f,.9f},{0,3,6,26,2.5f,.9f},{2,-2,3,16,1.4f,1},{-2,3,7,30,3,.9f}};
 auto& p=ps[juce::jlimit(0,19,i)];auto set=[this](const char* id,float x){auto* q=parameters.getParameter(id);q->setValueNotifyingHost(q->getNormalisableRange().convertTo0to1(x));};
 set("LOW",p.l);set("MID",p.m);set("HIGH",p.h);set("REVERB",p.w);set("DECAY",p.d);set("GAIN",p.g);
}
juce::AudioProcessorEditor* ADVoxAudioProcessor::createEditor(){return new ADVoxAudioProcessorEditor(*this);}
void ADVoxAudioProcessor::getStateInformation(juce::MemoryBlock& d){auto s=parameters.copyState();std::unique_ptr<juce::XmlElement>x(s.createXml());copyXmlToBinary(*x,d);}
void ADVoxAudioProcessor::setStateInformation(const void* d,int n){std::unique_ptr<juce::XmlElement>x(getXmlFromBinary(d,n));if(x&&x->hasTagName(parameters.state.getType()))parameters.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new ADVoxAudioProcessor();}
