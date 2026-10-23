#include "PluginProcessor.h"
#include "PluginEditor.h"
static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout(){
 using P=juce::AudioParameterFloat; juce::AudioProcessorValueTreeState::ParameterLayout l;
 l.add(std::make_unique<P>("amount","Amount",juce::NormalisableRange<float>(0.f,1.f),.5f));
 l.add(std::make_unique<P>("mix","Mix",juce::NormalisableRange<float>(0.f,1.f),.5f));
 l.add(std::make_unique<P>("tone","Tone",juce::NormalisableRange<float>(0.f,1.f),.5f));
 l.add(std::make_unique<P>("width","Width",juce::NormalisableRange<float>(0.f,1.f),.5f)); return l;
}
DAPluginAudioProcessor::DAPluginAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,"PARAMETERS",makeLayout()){}
void DAPluginAudioProcessor::prepareToPlay(double,int){} void DAPluginAudioProcessor::releaseResources(){}
bool DAPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& l)const{return l.getMainInputChannelSet()==l.getMainOutputChannelSet()&&(l.getMainOutputChannelSet()==juce::AudioChannelSet::mono()||l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo());}
void DAPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&){
 juce::ScopedNoDenormals n; float a=parameters.getRawParameterValue("amount")->load(),m=parameters.getRawParameterValue("mix")->load(),t=parameters.getRawParameterValue("tone")->load();
 for(int c=0;c<b.getNumChannels();++c){auto*d=b.getWritePointer(c);for(int i=0;i<b.getNumSamples();++i){float dry=d[i],wet=std::tanh(dry*(1.f+2.f*t))*(.5f+a);d[i]=dry*(1.f-m)+wet*m;}}
}
juce::AudioProcessorEditor* DAPluginAudioProcessor::createEditor(){return new DAPluginAudioProcessorEditor(*this);}
void DAPluginAudioProcessor::getStateInformation(juce::MemoryBlock& d){auto s=parameters.copyState();std::unique_ptr<juce::XmlElement>x(s.createXml());copyXmlToBinary(*x,d);}
void DAPluginAudioProcessor::setStateInformation(const void*d,int n){std::unique_ptr<juce::XmlElement>x(getXmlFromBinary(d,n));if(x&&x->hasTagName(parameters.state.getType()))parameters.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new DAPluginAudioProcessor();}
