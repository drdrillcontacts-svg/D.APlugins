#include "PluginEditor.h"

namespace {
const juce::Colour bg(0xff070809), panel(0xff0e1012), panel2(0xff151719),
 red(0xffff1738), redDark(0xff6f0b1c), white(0xffe8e9e9), grey(0xff85898e), grid(0xff292c30);
juce::StringArray controls(juce::String n) {
 if(n=="DVerb") return {"PRE-DELAY","SIZE","DECAY","AI MATCH","TONE","MIX","WIDTH"};
 if(n=="DDelay") return {"TIME","FEEDBACK","FILTER","AI MATCH","TONE","MIX","WIDTH"};
 if(n=="DComp") return {"THRESHOLD","RATIO","ATTACK","AI MATCH","RELEASE","MIX","MAKE UP"};
 if(n=="DEQ") return {"LOW","BODY","PRESENCE","AI MATCH","AIR","MIX","GAIN"};
 if(n=="DSat") return {"DRIVE","TONE","DENSITY","AI MATCH","COLOR","MIX","OUTPUT"};
 if(n=="DTrans") return {"ATTACK","SUSTAIN","SENSITIVITY","AI MATCH","SHAPE","MIX","OUTPUT"};
 if(n=="DStereo") return {"WIDTH","DEPTH","CENTER","AI MATCH","SIDE","MIX","MONO"};
 if(n=="DSess") return {"FREQ","THRESHOLD","RANGE","AI MATCH","FOCUS","MIX","OUTPUT"};
 if(n=="DGate") return {"THRESHOLD","HOLD","ATTACK","AI MATCH","RELEASE","RANGE","OUTPUT"};
 return {"SPACE","COLOR","MOTION","AI MATCH","TONE","MIX","OUTPUT"};
}
juce::String subtitle(juce::String n) {
 if(n=="DVerb") return "AI REVERB LEARNING"; if(n=="DDelay") return "AI DELAY LEARNING";
 if(n=="DComp") return "AI COMPRESSION"; if(n=="DEQ") return "AI EQUALIZER";
 if(n=="DSat") return "AI SATURATION"; if(n=="DTrans") return "AI TRANSIENT DESIGNER";
 if(n=="DStereo") return "AI STEREO DESIGNER"; if(n=="DSess") return "AI DE-ESSER";
 if(n=="DGate") return "AI GATE & EXPANDER"; return "AI MULTI-EFFECTS";
}
}
DAPluginAudioProcessorEditor::DAPluginAudioProcessorEditor(DAPluginAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p) {
 setSize(900,620);
 auto names=controls(p.getName());
 const char* ids[]={"amount","mix","tone","width"};
 for(int i=0;i<7;++i) {
   knobs[i].setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
   knobs[i].setTextBoxStyle(juce::Slider::TextBoxBelow,false,55,17);
   knobs[i].setColour(juce::Slider::rotarySliderFillColourId,red);
   knobs[i].setColour(juce::Slider::rotarySliderOutlineColourId,grid);
   knobs[i].setColour(juce::Slider::thumbColourId,white);
   knobs[i].setColour(juce::Slider::textBoxTextColourId,white);
   knobs[i].setColour(juce::Slider::textBoxBackgroundColourId,panel);
   knobs[i].setColour(juce::Slider::textBoxOutlineColourId,panel);
   addAndMakeVisible(knobs[i]);
   knobLabels[i].setText(names[i],juce::dontSendNotification);
   knobLabels[i].setJustificationType(juce::Justification::centred);
   knobLabels[i].setColour(juce::Label::textColourId,grey);
   knobLabels[i].setFont(juce::Font(9.0f,juce::Font::bold));
   addAndMakeVisible(knobLabels[i]);
 }
 for(int i=0;i<4;++i) attachments[i]=std::make_unique<Attachment>(p.parameters,ids[i],knobs[i]);
 for(auto* b:{&loadReference,&learnButton,&matchButton}) {
   b->setColour(juce::TextButton::buttonColourId,b==&learnButton?red:(b==&matchButton?redDark:panel2));
   b->setColour(juce::TextButton::textColourOffId,white);
   b->setColour(juce::TextButton::textColourOnId,white);
   addAndMakeVisible(*b);
 }
 loadReference.onClick=[this]{statusText="REFERENCE SLOT SELECTED  /  FILE LOADING NOT IMPLEMENTED";status.setText(statusText,juce::dontSendNotification);};
 learnButton.onClick=[this]{statusText="LEARN MODE  /  CONNECT AN AUDIO ANALYSIS MODEL";status.setText(statusText,juce::dontSendNotification);};
 matchButton.onClick=[this]{statusText="AI MATCH IS A UI PLACEHOLDER IN THIS BUILD";status.setText(statusText,juce::dontSendNotification);};
 modeBox.addItem("ROOM",1);modeBox.addItem("HALL",2);modeBox.addItem("PLATE",3);modeBox.addItem("SHIMMER",4);modeBox.setSelectedId(2);
 modeBox.setColour(juce::ComboBox::backgroundColourId,panel2);modeBox.setColour(juce::ComboBox::textColourId,white);modeBox.setColour(juce::ComboBox::outlineColourId,grid);addAndMakeVisible(modeBox);
 status.setText(statusText,juce::dontSendNotification);status.setColour(juce::Label::textColourId,grey);status.setFont(juce::Font(9.0f,juce::Font::bold));addAndMakeVisible(status);
}
void DAPluginAudioProcessorEditor::paint(juce::Graphics& g) {
 auto n=processor.getName();g.fillAll(bg);
 // brushed-metal top rail and red edge
 g.setColour(red);g.fillRect(0,0,getWidth(),3);g.setColour(juce::Colour(0xff202326));g.fillRect(0,3,getWidth(),43);
 g.setColour(white);g.setFont(juce::Font(22.0f,juce::Font::bold));g.drawText("D.A",20,7,48,27,juce::Justification::centredLeft);
 g.setColour(red);g.drawText("PLUGINS",69,9,86,24,juce::Justification::centredLeft);
 g.setColour(grey);g.setFont(juce::Font(8.0f,juce::Font::bold));g.drawText("LEARN THE SOUND  /  RECREATE THE FEEL",20,31,330,12,juce::Justification::left);
 g.setColour(white);g.setFont(juce::Font(17.0f,juce::Font::bold));g.drawText(n.toUpperCase(),getWidth()/2-100,7,200,23,juce::Justification::centred);
 g.setColour(grey);g.setFont(juce::Font(8.0f,juce::Font::bold));g.drawText(subtitle(n),getWidth()/2-100,28,200,12,juce::Justification::centred);
 g.setColour(grey);g.drawText("V1.0     ◉  ⚙  ×",getWidth()-130,14,112,18,juce::Justification::right);
 // Main analysis screen
 g.setColour(panel);g.fillRoundedRectangle(14,55,getWidth()-28,152,8);
 g.setColour(grid);g.drawRoundedRectangle(14,55,getWidth()-28,152,8,1);
 g.setColour(panel2);g.fillRoundedRectangle(24,65,160,132,5);g.fillRoundedRectangle(194,65,420,132,5);g.fillRoundedRectangle(624,65, getWidth()-658,132,5);
 g.setColour(grey);g.setFont(juce::Font(8.0f,juce::Font::bold));
 g.drawText("INPUT",32,71,100,12,juce::Justification::left);g.drawText("AI MATCH / ANALYSIS",205,71,180,12,juce::Justification::left);g.drawText("REFERENCE",635,71,120,12,juce::Justification::left);
 // red analysis grid and curve
 for(int i=0;i<8;++i)g.setColour(grid),g.drawLine(205.0f+i*56,89,205.0f+i*56,184,1.0f);
 for(int i=0;i<5;++i)g.setColour(grid),g.drawLine(205,91+i*22,604,91+i*22,1.0f);
 juce::Path curve;curve.startNewSubPath(207,102);
 for(int i=1;i<100;++i){float x=207+i*3.95f;float y=105+66.0f*(float(i)/100.0f)+10.0f*std::sin(i*.19f);curve.lineTo(x,y);}
 g.setColour(red.withAlpha(.22f));juce::Path fill=curve;fill.lineTo(600,184);fill.lineTo(207,184);fill.closeSubPath();g.fillPath(fill);
 g.setColour(red);g.strokePath(curve,juce::PathStrokeType(1.6f));
 // waveform strokes
 for(int i=0;i<65;++i){float x=35+i*2.0f;float h=8+std::abs(std::sin(i*.62f))*36;g.setColour(i%5==0?red:grey);g.drawLine(x,143-h/2,x,143+h/2,1.2f);}
 for(int i=0;i<65;++i){float x=638+i*2.0f;float h=4+std::abs(std::sin(i*.48f+1))*19;g.setColour(grey.withAlpha(.75f));g.drawLine(x,143-h/2,x,143+h/2,1.0f);}
 // lower modules
 g.setColour(panel);g.fillRoundedRectangle(14,217,getWidth()-28,118,8);g.setColour(grid);g.drawRoundedRectangle(14,217,getWidth()-28,118,8,1);
 g.setColour(panel);g.fillRoundedRectangle(14,344,430,210,8);g.fillRoundedRectangle(454,344,310,210,8);g.fillRoundedRectangle(774,217,112,337,8);
 g.setColour(grid);g.drawRoundedRectangle(14,344,430,210,8,1);g.drawRoundedRectangle(454,344,310,210,8,1);g.drawRoundedRectangle(774,217,112,337,8,1);
 g.setColour(grey);g.setFont(juce::Font(8.0f,juce::Font::bold));g.drawText("REFERENCE MODE",25,352,150,14,juce::Justification::left);g.drawText("ADVANCED / CHARACTER",466,352,220,14,juce::Justification::left);g.drawText("OUTPUT",785,226,90,14,juce::Justification::centred);
 // output meter
 for(int i=0;i<22;++i){int y=257+i*11;g.setColour(i<15?red:(i<19?juce::Colour(0xff8e182a):grid));g.fillRect(796,y,10,7);g.fillRect(813,y,10,7);}
 g.setColour(grid);g.drawLine(790,505,870,505,1);
 g.setColour(grey);g.setFont(juce::Font(8.0f));g.drawText("IN / OUT",790,516,80,12,juce::Justification::centred);
 // footer frame
 g.setColour(panel2);g.fillRect(14,563,getWidth()-28,39);g.setColour(redDark);g.fillRect(14,563,getWidth()-28,1);
 g.setColour(grey);g.setFont(juce::Font(8.0f,juce::Font::bold));g.drawText("D.A  |  "+n.toUpperCase(),25,576,250,15,juce::Justification::left);
 g.drawText("POWERED BY D.A NEURAL ENGINE  /  PROTOTYPE",getWidth()-340,576,310,15,juce::Justification::right);
}
void DAPluginAudioProcessorEditor::resized() {
 modeBox.setBounds(25,374,185,25);loadReference.setBounds(25,406,185,27);learnButton.setBounds(25,443,185,30);matchButton.setBounds(25,483,185,27);
 status.setBounds(25,520,410,20);
 for(int i=0;i<7;++i){int x=24+i*123;knobLabels[i].setBounds(x,224,110,17);knobs[i].setBounds(x+5,239,100,88);}
}
