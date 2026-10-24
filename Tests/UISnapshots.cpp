#include "PluginEditor.h"
#include <iostream>
#include <limits>
void collect(juce::Component &root, std::vector<da::ui::ParameterControl *> &result) {
    if (auto *control = dynamic_cast<da::ui::ParameterControl *>(&root))
        result.push_back(control);
    for (auto *child : root.getChildren())
        collect(*child, result);
}
juce::TextButton *findButton(juce::Component &root, const juce::String &text) {
    if (auto *button = dynamic_cast<juce::TextButton *>(&root))
        if (button->getButtonText() == text)
            return button;
    for (auto *child : root.getChildren())
        if (auto *result = findButton(*child, text))
            return result;
    return nullptr;
}
juce::TextEditor *findLog(juce::Component &root) {
    if (auto *text = dynamic_cast<juce::TextEditor *>(&root))
        if (text->getComponentID() == "learning-log")
            return text;
    for (auto *child : root.getChildren())
        if (auto *result = findLog(*child))
            return result;
    return nullptr;
}
juce::Component *findById(juce::Component &root, const juce::String &id) {
    if (root.getComponentID() == id)
        return &root;
    for (auto *child : root.getChildren())
        if (auto *result = findById(*child, id))
            return result;
    return nullptr;
}
struct TempoPlayHead : juce::AudioPlayHead {
    double bpm = 120;
    bool available = true;
    juce::Optional<PositionInfo> getPosition() const override {
        if (!available)
            return {};
        PositionInfo p;
        p.setBpm(bpm);
        p.setIsPlaying(true);
        return p;
    }
};
int fail(const char *name, int code) {
    std::cerr << "Native check failed: " << name << " / " << code << '\n';
    return code;
}
bool near(float a, float b) {
    return std::abs(a - b) <= 1e-5f * std::max(1.f, std::max(std::abs(a), std::abs(b)));
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
        if (processor.getNumPrograms() != 50)
            return fail(name, 19);
        for (int index = 0; index < 50; ++index) {
            processor.setCurrentProgram(index);
            if (processor.getCurrentProgram() != index || processor.getProgramName(index).isEmpty())
                return fail(name, 20);
            auto values = processor.currentValues();
            for (size_t i = 0; i < values.size(); ++i)
                if (!near(values[i], processor.factoryBank[size_t(index)].values[i]))
                    return fail(name, 21);
        }
        juce::MemoryBlock saved;
        processor.getStateInformation(saved);
        processor.setCurrentProgram(0);
        processor.setStateInformation(saved.getData(), int(saved.getSize()));
        if (processor.getCurrentProgram() != 49)
            return fail(name, 22);
        auto restoredProgram = processor.currentValues();
        for (size_t i = 0; i < restoredProgram.size(); ++i)
            if (!near(restoredProgram[i], processor.factoryBank[49].values[i]))
                return fail(name, 23);
        processor.setCurrentProgram(0);
        if (processor.kind == da::Kind::Delay) {
            TempoPlayHead playhead;
            playhead.bpm = 96;
            processor.setPlayHead(&playhead);
            juce::AudioBuffer<float> audio(2, 64);
            audio.clear();
            juce::MidiBuffer midi;
            processor.processBlock(audio, midi);
            if (processor.projectBpm.load() != 96 || processor.tempoStatus.load() != 1)
                return fail(name, 24);
            playhead.available = false;
            processor.processBlock(audio, midi);
            if (processor.projectBpm.load() != 96 || processor.tempoStatus.load() != 2)
                return fail(name, 25);
            auto legacy = processor.parameters.copyState();
            legacy.removeChild(legacy.getChildWithProperty("id", "sync"), nullptr);
            legacy.removeChild(legacy.getChildWithProperty("id", "division"), nullptr);
            auto xml = legacy.createXml();
            juce::MemoryBlock legacyBytes;
            juce::AudioProcessor::copyXmlToBinary(*xml, legacyBytes);
            processor.setCurrentProgram(5);
            processor.setStateInformation(legacyBytes.getData(), int(legacyBytes.getSize()));
            if (processor.parameters.getRawParameterValue("sync")->load() != 0 ||
                processor.parameters.getRawParameterValue("division")->load() != 6)
                return fail(name, 26);
            processor.setPlayHead(nullptr);
            processor.setCurrentProgram(0);
        }
        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        editor->setSize(1200, 1240);
        std::vector<da::ui::ParameterControl *> controls;
        collect(*editor, controls);
        if (controls.size() != processor.metadata.size())
            return fail(name, 5);
        for (auto *c : controls) {
            if (!c->isVisible())
                return fail(name, 27);
            auto id = c->getComponentID().toStdString();
            auto it = std::find_if(processor.metadata.begin(), processor.metadata.end(),
                                   [&](const da::Parameter &m) { return m.id == id; });
            if (it == processor.metadata.end())
                return fail(name, 6);
            auto *learnButton = findButton(*c, "LEARN");
            if (!learnButton || learnButton->isVisible() != da::canLearn(processor.kind, id))
                return fail(name, 16);
            auto original = processor.currentValues();
            c->slider.setValue(it->max, juce::sendNotificationSync);
            if (!near(processor.parameters.getRawParameterValue(id)->load(), it->max))
                return fail(name, 7);
            c->slider.setValue(it->initial, juce::sendNotificationSync);
            auto restored = processor.currentValues();
            for (size_t i = 0; i < restored.size(); ++i)
                if (!near(restored[i], original[i]))
                    return fail(name, 8);
            if (it->unit == "Hz" && !c->slider.getTextFromValue(it->initial).contains("Hz"))
                return fail(name, 9);
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
        if (!image.isValid() || image.getWidth() != 2400 || image.getHeight() != 2480)
            return fail(name, 2);
        auto stream = out.getChildFile(juce::String(name) + ".png").createOutputStream();
        if (!stream || !juce::PNGImageFormat().writeImageToStream(image, *stream))
            return fail(name, 3);
        std::cout << "Native JUCE snapshot: " << name << " / " << processor.metadata.size()
                  << " controls\n";
        auto *learn = findButton(*editor, "LEARN ALL");
        auto *log = findLog(*editor);
        auto *details = findButton(*editor, "DETAILS / LOG");
        auto *refBtn = findButton(*editor, "REFERENCE...");
        auto *refPanel = findById(*editor, "reference-panel");
        auto *closeRef = findButton(*editor, "CLOSE");
        if (!learn || !log || !details || !refBtn || !refPanel || !closeRef ||
            log->getParentComponent()->isVisible() || refPanel->isVisible())
            return fail(name, 10);
        refBtn->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if (!refPanel->isVisible())
            return fail(name, 14);
        closeRef->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if (refPanel->isVisible())
            return fail(name, 15);
        auto beforeLearn = processor.currentValues();
        learn->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        for (int block = 0; block < 300; ++block) {
            b.clear();
            for (int i = 0; i < 256; ++i)
                for (int ch = 0; ch < 2; ++ch)
                    b.setSample(ch, i, .2f * float(std::sin((block * 256 + i) * .037)));
            processor.processBlock(b, midi);
            juce::MessageManager::getInstance()->runDispatchLoopUntil(2);
        }
        juce::MessageManager::getInstance()->runDispatchLoopUntil(100);
        if (!log->getText().contains("confidence") || log->getParentComponent()->isVisible())
            return fail(name, 11);
        details->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if (!log->getParentComponent()->isVisible())
            return fail(name, 12);
        details->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        if (log->getParentComponent()->isVisible())
            return fail(name, 13);
        auto *audition = findButton(*editor, "PREVIEW");
        if (!audition)
            return fail(name, 17);
        audition->triggerClick();
        juce::MessageManager::getInstance()->runDispatchLoopUntil(10);
        const auto chosen = controls.front()->getComponentID().toStdString();
        auto chosenMeta = std::find_if(processor.metadata.begin(), processor.metadata.end(),
                                       [&](const da::Parameter &p) { return p.id == chosen; });
        const auto chosenIndex = size_t(std::distance(processor.metadata.begin(), chosenMeta));
        controls.front()->slider.setValue(chosenMeta->max, juce::sendNotificationSync);
        juce::MessageManager::getInstance()->runDispatchLoopUntil(70);
        auto afterManual = processor.currentValues();
        for (size_t i = 0; i < afterManual.size(); ++i)
            if (!near(afterManual[i], i == chosenIndex ? chosenMeta->max : beforeLearn[i]))
                return fail(name, 18);
        controls.front()->slider.setValue(chosenMeta->initial, juce::sendNotificationSync);
        juce::MessageManager::getInstance()->runDispatchLoopUntil(70);
        editor->setSize(750, 775); // exercise the supported minimum aspect-ratio layout
        auto small = editor->createComponentSnapshot(editor->getLocalBounds());
        if (!small.isValid())
            return fail(name, 4);
    }
    return 0;
}
