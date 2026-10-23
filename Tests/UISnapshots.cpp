#include "PluginEditor.h"
#include <iostream>
void collect(juce::Component &root, std::vector<da::ui::ParameterControl *> &result) {
    if (auto *control = dynamic_cast<da::ui::ParameterControl *>(&root))
        result.push_back(control);
    for (auto *child : root.getChildren())
        collect(*child, result);
}
juce::TextButton *findButton(juce::Component &root, const juce::String &text) {
    if (auto *button=dynamic_cast<juce::TextButton *>(&root))
        if(button->getButtonText()==text)return button;
    for(auto *child:root.getChildren())if(auto *result=findButton(*child,text))return result;
    return nullptr;
}
juce::TextEditor *findLog(juce::Component &root) {
    if(auto *text=dynamic_cast<juce::TextEditor *>(&root))if(text->isReadOnly())return text;
    for(auto *child:root.getChildren())if(auto *result=findLog(*child))return result;
    return nullptr;
}
// Run with a display server, e.g. xvfb-run -a ./da_ui_snapshots <output-directory>.
// These are actual JUCE component snapshots; the SVG design studies are not used here.
int main(int argc, char **argv) {
    juce::ScopedJuceInitialiser_GUI init;
    juce::File out =
        argc > 1 ? juce::File(argv[1])
                 : juce::File::getCurrentWorkingDirectory().getChildFile("native-ui-previews");
    if (!out.createDirectory())
        return 1;
    const char *names[]{"DVerb",  "DDelay",  "DComp", "DEQ",   "DSat",
                        "DTrans", "DStereo", "DSess", "DGate", "DFX"};
    for (auto name : names) {
        DAPluginAudioProcessor processor(name);
        processor.prepareToPlay(48000, 256);
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        editor->setSize(1200, 820);
        std::vector<da::ui::ParameterControl *> controls;
        collect(*editor, controls);
        if (controls.size() != processor.metadata.size())
            return 5;
        for (auto *c : controls) {
            auto id = c->getComponentID().toStdString();
            auto it = std::find_if(processor.metadata.begin(), processor.metadata.end(),
                                   [&](const da::Parameter &m) { return m.id == id; });
            if (it == processor.metadata.end())
                return 6;
            auto *learnButton=findButton(*c,"LEARN");
            if(!learnButton||learnButton->isVisible()!=da::canLearn(processor.kind,id))return 16;
            auto original = processor.currentValues();
            c->slider.setValue(it->max, juce::sendNotificationSync);
            if (std::abs(processor.parameters.getRawParameterValue(id)->load() - it->max) > 1e-4f)
                return 7;
            c->slider.setValue(it->initial, juce::sendNotificationSync);
            auto restored = processor.currentValues();
            for (size_t i = 0; i < restored.size(); ++i)
                if (std::abs(restored[i] - original[i]) > 1e-4f)
                    return 8;
            if (it->unit == "Hz" && !c->slider.getTextFromValue(it->initial).contains("Hz"))
                return 9;
        }
        juce::AudioBuffer<float> b(2, 256);
        juce::MidiBuffer midi;
        for (int block = 0; block < 80; ++block) {
            for (int i = 0; i < 256; ++i) {
                double t = (block * 256 + i) / 48000.;
                float sample = .18f * float(std::sin(2 * da::pi * 220 * t)) +
                               .05f * float(std::sin(2 * da::pi * 3000 * t));
                b.setSample(0, i, sample);
                b.setSample(1, i, sample * .8f);
            }
            processor.processBlock(b, midi);
            juce::MessageManager::getInstance()->runDispatchLoopUntil(3);
        }
        juce::MessageManager::getInstance()->runDispatchLoopUntil(90);
        auto image = editor->createComponentSnapshot(editor->getLocalBounds(), true, 2.f);
        if (!image.isValid() || image.getWidth() != 2400 || image.getHeight() != 1640)
            return 2;
        auto stream = out.getChildFile(juce::String(name) + ".png").createOutputStream();
        if (!stream || !juce::PNGImageFormat().writeImageToStream(image, *stream))
            return 3;
        std::cout << "Native JUCE snapshot: " << name << " / " << processor.metadata.size()
                  << " controls\n";
        auto *learn=findButton(*editor,"LEARN ALL");
        auto *log=findLog(*editor);
        auto *details=findButton(*editor,"DETAILS / LOG");
        if(!learn||!log||!details||log->getParentComponent()->isVisible())return 10;
        auto beforeLearn=processor.currentValues();
        learn->triggerClick();juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        for(int block=0;block<300;++block) {
            b.clear();
            for(int i=0;i<256;++i)for(int ch=0;ch<2;++ch)
                b.setSample(ch,i,.2f*float(std::sin((block*256+i)*.037)));
            processor.processBlock(b,midi);
            juce::MessageManager::getInstance()->runDispatchLoopUntil(2);
        }
        juce::MessageManager::getInstance()->runDispatchLoopUntil(100);
        if(!log->getText().contains("confidence")||log->getParentComponent()->isVisible())return 11;
        details->triggerClick();juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if(!log->getParentComponent()->isVisible())return 12;
        details->triggerClick();juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if(log->getParentComponent()->isVisible())return 13;
        auto *audition=findButton(*editor,"PREVIEW");if(!audition)return 17;
        audition->triggerClick();juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        const auto chosen=controls.front()->getComponentID().toStdString();
        auto chosenMeta=std::find_if(processor.metadata.begin(),processor.metadata.end(),[&](const da::Parameter &p){return p.id==chosen;});
        const auto chosenIndex=size_t(std::distance(processor.metadata.begin(),chosenMeta));
        controls.front()->slider.setValue(chosenMeta->max,juce::sendNotificationSync);
        juce::MessageManager::getInstance()->runDispatchLoopUntil(70);
        auto afterManual=processor.currentValues();
        for(size_t i=0;i<afterManual.size();++i)
            if(std::abs(afterManual[i]-(i==chosenIndex?chosenMeta->max:beforeLearn[i]))>1e-4f)return 18;
        controls.front()->slider.setValue(chosenMeta->initial,juce::sendNotificationSync);
        juce::MessageManager::getInstance()->runDispatchLoopUntil(70);
        auto *advanced=findButton(*editor,"ADVANCED");if(!advanced)return 14;
        advanced->triggerClick();juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        auto advancedImage=editor->createComponentSnapshot(editor->getLocalBounds(),true,2.f);
        auto advancedStream=out.getChildFile(juce::String(name)+"-Advanced.png").createOutputStream();
        if(!advancedStream||!juce::PNGImageFormat().writeImageToStream(advancedImage,*advancedStream))return 15;
        editor->setSize(900, 615); // exercise the supported minimum aspect-ratio layout
        auto small = editor->createComponentSnapshot(editor->getLocalBounds());
        if (!small.isValid())
            return 4;
    }
    return 0;
}
