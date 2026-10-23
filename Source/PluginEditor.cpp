#include "PluginEditor.h"
#include <chrono>
namespace {
template <class T> bool ready(std::future<T> &f) {
    return f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
} // namespace
DAPluginAudioProcessorEditor::DAPluginAudioProcessorEditor(DAPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processor(p), layout(p.getName()), chassis(layout, hardware),
      analysis(p.kind, hardware), session(p.currentValues()), lastValues(session.values) {
    setLookAndFeel(&hardware);
    setResizable(true, true);
    setResizeLimits(900, 615, 1800, 1230);
    getConstrainer()->setFixedAspectRatio(1200.0 / 820.0);
    addAndMakeVisible(stage);
    stage.setSize(1200, 820);
    stage.addAndMakeVisible(chassis);
    chassis.setBounds(0, 0, 1200, 820);
    chassis.overlay = [this](juce::Graphics &g) { paintDetails(g); };
    stage.addAndMakeVisible(analysis);
    analysis.setBounds(54, 142, 870, 224);
    if (auto *specs = layout.data["controls"].getArray())
        for (auto &spec : *specs) {
            auto id = spec["id"].toString().toStdString();
            auto it = std::find_if(p.metadata.begin(), p.metadata.end(),
                                   [&](const da::Parameter &m) { return m.id == id; });
            if (it == p.metadata.end())
                continue;
            auto c = std::make_unique<da::ui::ParameterControl>(p.parameters, *it, spec, hardware,
                                                                [this, id] { startLearn(id); });
            stage.addAndMakeVisible(*c);
            c->setBounds(da::ui::rectangle(spec["rect"]));
            controls.push_back(std::move(c));
        }
    if (auto *specs = layout.data["modules"].getArray())
        for (auto &spec : *specs) {
            auto b = std::make_unique<juce::TextButton>(spec["title"].toString());
            b->onClick = [this, scope = spec["scope"].toString().toStdString()] {
                startLearn(scope);
            };
            stage.addAndMakeVisible(*b);
            b->setBounds(da::ui::rectangle(spec["rect"]));
            modules.push_back(std::move(b));
        }
    for (auto *b : {&learnAll, &loadReference, &preview, &apply, &reject, &undo, &showResults, &advanced})
        stage.addAndMakeVisible(*b);
    learnAll.getProperties().set("accent", true);
    apply.getProperties().set("accent", true);
    mode.addItem("INPUT-AWARE LEARN", 1);
    mode.addItem("REFERENCE GUIDANCE", 2);
    mode.addItem("DESCRIPTION GUIDANCE", 3);
    mode.setSelectedId(1);
    stage.addAndMakeVisible(mode);
    description.setMultiLine(true);
    description.setFont(hardware.font(15));
    description.setTextToShowWhenEmpty("Dark, warm, wide, smooth, cinematic…", da::ui::muted);
    stage.addAndMakeVisible(description);
    results.setMultiLine(true);
    results.setReadOnly(true);
    results.setFont(hardware.font(16));
    results.setBorder(juce::BorderSize<int>(46, 16, 16, 16));
    results.setText(
        "D.A PLUGINS / LEARNING WORKBENCH\n\nChoose a mode and press LEARN for a parameter, "
        "band/module, or all eligible controls. Play audio during capture. Results stay in this optional log and do not change your "
        "settings until Preview or Apply.\n\nInput-aware learning uses measured features and "
        "deterministic estimates, not a trained neural network. Reference guidance is currently "
        "limited to EQ gains, stereo width and compressor makeup. Other proposals are "
        "input/objective starting points, not exact reference matches.\n\nRecognized description "
        "words: dark, warm, wide, smooth, cinematic. Negated goals are rejected; other unsupported words "
        "are ignored.\n\nPreview auditions; Reject restores; Apply keeps; Undo restores one "
        "learning operation. Manual changes invalidate older proposals.");
    drawer.addAndMakeVisible(results);
    drawer.addAndMakeVisible(closeResults);
    stage.addChildComponent(drawer);
    closeResults.onClick = [this] { drawer.setVisible(false); };
    showResults.onClick = [this] {
        drawer.setVisible(!drawer.isVisible());
        if (drawer.isVisible())
            drawer.toFront(false);
    };
    status.setFont(hardware.font(12));
    status.setColour(juce::Label::textColourId, da::ui::muted);
    stage.addAndMakeVisible(status);
    advanced.onClick = [this] {
        advancedView = !advancedView;
        updateControlPage();
    };
    mode.onChange = [this] {
        description.setEnabled(mode.getSelectedId() == 3);
        loadReference.setEnabled(mode.getSelectedId() == 2);
    };
    mode.onChange();
    updateControlPage();
    learnAll.onClick = [this] { startLearn("all"); };
    loadReference.onClick = [this] { load(); };
    preview.onClick = [this] {
        syncManual();
        if (session.preview())
            write();
        else
            setStatus("No current suggestion to preview");
    };
    apply.onClick = [this] {
        syncManual();
        if (session.apply())
            write();
        else
            setStatus("No current suggestion to apply");
    };
    reject.onClick = [this] {
        syncManual();
        session.reject();
        ++session.revision;
        capturing = false;
        processor.cancelCapture();
        write();
    };
    undo.onClick = [this] {
        syncManual();
        if (session.undo())
            write();
        else
            setStatus("No learning operation to undo");
    };
    setStatus("Ready / Play audio to analyze / Manual control always");
    setSize(1200, 820);
    startTimerHz(24);
}
DAPluginAudioProcessorEditor::~DAPluginAudioProcessorEditor() {
    stopTimer();
    processor.cancelCapture();
    syncManual();
    if (session.status == "Previewing") {
        session.reject();
        processor.writeValues(session.values);
    }
    setLookAndFeel(nullptr);
}
void DAPluginAudioProcessorEditor::setStatus(const juce::String &s) {
    status.setText(s, juce::dontSendNotification);
    analysis.setStatus(s.upToFirstOccurrenceOf("—", false, false));
}
void DAPluginAudioProcessorEditor::syncManual() {
    auto values = processor.currentValues();
    bool changed = false;
    for (size_t i = 0; i < values.size(); ++i)
        changed |= std::abs(values[i] - lastValues[i]) > 1e-5f;
    if (changed) {
        // Restore unaccepted preview values, but preserve each real host/manual edit.
        session.reject();
        for (size_t i = 0; i < values.size(); ++i)
            if (std::abs(values[i] - lastValues[i]) > 1e-5f)
                session.manual(i, values[i]);
        ++session.revision;
        processor.writeValues(session.values);
        lastValues = processor.currentValues();
        processor.cancelCapture();
        capturing = false;
        setStatus("Manual/host change — unaccepted learning cancelled");
    }
}
void DAPluginAudioProcessorEditor::write() {
    processor.writeValues(session.values);
    lastValues = processor.currentValues();
    setStatus(session.status);
}
void DAPluginAudioProcessorEditor::startLearn(std::string requestedScope) {
    syncManual();
    if (capturing || learnWork.valid() || referenceWork.valid()) {
        setStatus("Analysis busy — cancel or wait");
        return;
    }
    session.reject();
    ++session.revision;
    write();
    try {
        objective = {};
        if (mode.getSelectedId() == 2) {
            objective.mode = da::Mode::Reference;
            if (!reference.valid())
                throw std::runtime_error("Reference unavailable — load a non-silent audio file");
        }
        if (mode.getSelectedId() == 3)
            objective = da::describe(description.getText().toStdString());
        if (!processor.requestCapture())
            throw std::runtime_error("Capture busy — play audio to finish the pending capture");
        scope = std::move(requestedScope);
        captureRevision = session.revision;
        capturing = true;
        captureStarted = juce::Time::currentTimeMillis();
        setStatus("Analyzing — capturing 65,536 input samples; keep audio playing");
    } catch (const std::exception &e) {
        setStatus(juce::String("Failed: ") + e.what());
    }
}
void DAPluginAudioProcessorEditor::launch(std::vector<float> l, std::vector<float> r, double sr) {
    auto kind = processor.kind;
    auto values = session.values;
    auto target = reference;
    auto goal = objective;
    auto selected = scope;
    auto revision = captureRevision;
    learnWork = std::async(std::launch::async, [l = std::move(l), r = std::move(r), sr, kind,
                                                values, target, goal, selected, revision] {
        auto features = da::analyze(l.data(), r.data(), l.size(), sr);
        return da::learn(kind, features, &target, goal, values, selected, revision);
    });
}
void DAPluginAudioProcessorEditor::load() {
    if (capturing || learnWork.valid() || referenceWork.valid()) {
        setStatus("Analysis busy");
        return;
    }
    chooser = std::make_unique<juce::FileChooser>("Choose reference (first 30 seconds analyzed)",
                                                  juce::File{}, "*.wav;*.aiff;*.aif;*.flac");
    auto safe = juce::Component::SafePointer<DAPluginAudioProcessorEditor>(this);
    chooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [safe](const juce::FileChooser &c) {
            if (!safe)
                return;
            auto file = c.getResult();
            if (!file.existsAsFile()) {
                safe->setStatus("Cancelled — no reference selected");
                return;
            }
            safe->syncManual();
            safe->session.reject();
            ++safe->session.revision;
            safe->write();
            safe->reference = {};
            safe->analysis.setReference({});
            safe->referenceRevision = safe->session.revision;
            safe->setStatus("Analyzing reference — decoding up to 30 seconds on worker");
            safe->referenceWork = std::async(std::launch::async, [file] {
                juce::AudioFormatManager formats;
                formats.registerBasicFormats();
                std::unique_ptr<juce::AudioFormatReader> reader(formats.createReaderFor(file));
                if (!reader || reader->numChannels == 0 || reader->sampleRate < 8000 ||
                    reader->sampleRate > 384000 || reader->lengthInSamples < 1024)
                    throw std::runtime_error("Unsupported, invalid or too-short reference file");
                int length = int(std::min<juce::int64>(reader->lengthInSamples,
                                                       juce::int64(reader->sampleRate * 30)));
                juce::AudioBuffer<float> buffer(2, length);
                buffer.clear();
                if (!reader->read(&buffer, 0, length, 0, true, true))
                    throw std::runtime_error("Reference decode failed");
                if (reader->numChannels == 1)
                    buffer.copyFrom(1, 0, buffer, 0, 0, length);
                auto f = da::ui::summarizeReference(buffer, reader->sampleRate,
                                                    file.getFileNameWithoutExtension());
                if (!f.features.valid())
                    throw std::runtime_error("Reference is silent or invalid");
                return f;
            });
        });
}
void DAPluginAudioProcessorEditor::timerCallback() {
    syncManual();
    std::vector<float> l, r;
    double sr = 0;
    if (processor.takeCapture(l, r, sr)) {
        if (capturing && captureRevision == session.revision) {
            capturing = false;
            launch(std::move(l), std::move(r), sr);
        } else
            capturing = false;
    }
    if (capturing && juce::Time::currentTimeMillis() - captureStarted > 15000) {
        capturing = false;
        processor.cancelCapture();
        ++session.revision;
        setStatus("Failed — capture timed out; transport must be running");
    }
    if (ready(referenceWork))
        try {
            auto analyzedReference = referenceWork.get();
            if (referenceRevision != session.revision) {
                setStatus("Cancelled — stale reference analysis discarded");
                return;
            }
            reference = analyzedReference.features;
            analysis.setReference(analyzedReference);
            setStatus("Reference available — RMS " + juce::String(da::db(float(reference.rms)), 1) +
                      " dBFS; guidance is not an exact match");
        } catch (const std::exception &e) {
            setStatus(juce::String("Reference unavailable: ") + e.what());
        }
    if (ready(learnWork))
        try {
            auto proposal = learnWork.get();
            juce::String message(proposal.limitation);
            for (auto &s : proposal.values) {
                const auto &p = processor.metadata[s.index];
                message += "\n" + juce::String(p.module + " / " + p.name) + ": " +
                           juce::String(session.values[s.index], 2) + " -> " +
                           juce::String(s.value, 2) + " " + juce::String(p.unit) + " (confidence " +
                           juce::String(s.confidence, 2) + ")\n" + juce::String(s.reason);
            }
            if (session.offer(std::move(proposal))) {
                results.setText(message);
                setStatus(session.status);
            } else
                setStatus("Cancelled — stale analysis discarded");
        } catch (const std::exception &e) {
            setStatus(juce::String("Failed: ") + e.what());
        }
    analysis.setParameters(lastValues);
    analysis.consume(processor.scopeFifo, processor.displaySampleRate.load(),
                     processor.inputPeak.load(), processor.outputPeak.load(),
                     processor.reduction.load());
    chassis.repaint();
}
void DAPluginAudioProcessorEditor::paint(juce::Graphics &g) { g.fillAll(juce::Colour(0xff030405)); }
void DAPluginAudioProcessorEditor::paintDetails(juce::Graphics &g) {
    da::ui::label(g, hardware, "LEARNING", {956, 151, 180, 24}, 15, da::ui::silver);
    da::ui::label(g, hardware, "DESCRIBE / OPTIONAL ENGLISH KEYWORDS", {68, 643, 600, 20}, 12);
    da::ui::label(g, hardware, "PREVIEW AUDITIONS / APPLY KEEPS / REJECT RESTORES",
                  {68, 712, 800, 18}, 11);
    da::ui::label(g, hardware, advancedView ? layout.title("advancedTitle") : layout.title("slogan"),
                  {64, 377, 920, 24}, 13, da::ui::muted);
}
void DAPluginAudioProcessorEditor::updateControlPage() {
    auto *specs = layout.data["controls"].getArray();
    if (specs)
        for (size_t i = 0; i < controls.size(); ++i)
            controls[i]->setVisible(bool((*specs)[int(i)]["advanced"]) == advancedView);
    for (auto &b : modules) b->setVisible(!advancedView);
    advanced.setButtonText(advancedView ? "MAIN CONTROLS" : "ADVANCED");
    advanced.setToggleState(advancedView, juce::dontSendNotification);
    chassis.repaint();
}
void DAPluginAudioProcessorEditor::resized() {
    const float scale = std::min(float(getWidth()) / 1200, float(getHeight()) / 820);
    stage.setTransform(juce::AffineTransform::scale(scale).translated(
        (getWidth() - 1200 * scale) * .5f, (getHeight() - 820 * scale) * .5f));
    mode.setBounds(956, 185, 180, 30);
    loadReference.setBounds(956, 224, 180, 30);
    learnAll.setBounds(956, 263, 180, 34);
    showResults.setBounds(956, 308, 180, 30);
    advanced.setBounds(978, 376, 160, 28);
    description.setBounds(68, 666, 1064, 40);
    int x = 68;
    for (auto *b : {&preview, &apply, &reject, &undo}) {
        b->setBounds(x, 738, 150, 28); x += 162;
    }
    status.setBounds(68, 775, 1064, 22);
    drawer.setBounds(54, 370, 1092, 398);
    results.setBounds(drawer.getLocalBounds());
    closeResults.setBounds(862, 9, 212, 28);
}
