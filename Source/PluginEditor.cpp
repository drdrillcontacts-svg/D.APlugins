#include "PluginEditor.h"
#include <chrono>
namespace {
template <class T> bool ready(std::future<T> &f) {
    return f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
} // namespace
DAPluginAudioProcessorEditor::DAPluginAudioProcessorEditor(DAPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processor(p), layout(p.getName()), hardware(layout.accent()),
      chassis(layout, hardware), analysis(p.kind, hardware), session(p.currentValues()),
      lastValues(session.values) {
    setLookAndFeel(&hardware);
    setResizable(true, true);
    setResizeLimits(896, 688, 1792, 1376);
    getConstrainer()->setFixedAspectRatio(1120.0 / 860.0);
    addAndMakeVisible(stage);
    stage.setSize(1120, 860);
    stage.addAndMakeVisible(chassis);
    chassis.setBounds(0, 0, 1120, 860);
    chassis.overlay = [this](juce::Graphics &g) { paintDetails(g); };
    stage.addAndMakeVisible(analysis);
    analysis.setBounds(22, 82, 1076, 204);
    analysis.setParameters(lastValues);
    if (auto *specs = layout.data["controls"].getArray())
        for (auto &spec : *specs) {
            auto id = spec["id"].toString().toStdString();
            auto it = std::find_if(p.metadata.begin(), p.metadata.end(),
                                   [&](const da::Parameter &m) { return m.id == id; });
            if (it == p.metadata.end())
                continue;
            auto c = std::make_unique<da::ui::ParameterControl>(
                p.parameters, *it, spec, hardware, [this, id] { startLearn(id); },
                [this] { analysis.setParameters(processor.currentValues()); });
            stage.addAndMakeVisible(*c);
            c->setBounds(da::ui::rectangle(spec["rect"]));
            controls.push_back(std::move(c));
        }
    if (auto *specs = layout.data["modules"].getArray())
        for (auto &spec : *specs) {
            auto b = std::make_unique<juce::TextButton>(spec["title"].toString());
            b->onClick = [this, scopeName = spec["scope"].toString().toStdString()] {
                startLearn(scopeName);
            };
            stage.addAndMakeVisible(*b);
            b->setBounds(da::ui::rectangle(spec["rect"]));
            modules.push_back(std::move(b));
        }
    for (auto *b : {&captureInput, &openReference, &learnAll, &showResults, &preview, &apply,
                    &reject, &undo})
        stage.addAndMakeVisible(*b);
    learnAll.getProperties().set("accent", true);
    apply.getProperties().set("accent", true);
    loadReference.getProperties().set("accent", true);

    observedPresetGeneration = processor.presetGeneration();
    for (int i = 0; i < 50; ++i)
        presetMenu.addItem(
            juce::String(i + 1).paddedLeft('0', 2) + " / " + processor.getProgramName(i), i + 1);
    presetMenu.setSelectedId(processor.getCurrentProgram() + 1, juce::dontSendNotification);
    previousPreset.setTooltip("Previous factory preset");
    nextPreset.setTooltip("Next factory preset");
    presetMenu.setTooltip(
        "50 factory presets. Loading replaces the full state; save your project first.");
    stage.addAndMakeVisible(presetMenu);
    stage.addAndMakeVisible(previousPreset);
    stage.addAndMakeVisible(nextPreset);
    presetMenu.onChange = [this] {
        if (presetMenu.getSelectedId() > 0) {
            processor.setCurrentProgram(presetMenu.getSelectedId() - 1);
            syncManual();
            analysis.setParameters(processor.currentValues());
        }
    };
    previousPreset.onClick = [this] {
        processor.setCurrentProgram((processor.getCurrentProgram() + 49) % 50);
        syncManual();
        analysis.setParameters(processor.currentValues());
    };
    nextPreset.onClick = [this] {
        processor.setCurrentProgram((processor.getCurrentProgram() + 1) % 50);
        syncManual();
        analysis.setParameters(processor.currentValues());
    };

    // Separate optional Reference workspace — closed by default and opened only on request.
    referenceDrawer.setComponentID("reference-panel");
    referenceDrawer.onPaint = [this](juce::Graphics &g) {
        analysis.paintReferencePanel(g, referenceDrawer.getLocalBounds().toFloat());
    };
    referenceDrawer.addAndMakeVisible(loadReference);
    referenceDrawer.addAndMakeVisible(useReferenceMode);
    referenceDrawer.addAndMakeVisible(closeReference);
    stage.addChildComponent(referenceDrawer);
    referenceDrawer.setVisible(false);
    useReferenceMode.setClickingTogglesState(true);
    useReferenceMode.onClick = [this] {
        referenceGuidanceEnabled = useReferenceMode.getToggleState();
        setStatus(referenceGuidanceEnabled
                      ? "Reference guidance enabled for Learn"
                      : "Input-aware Learn active (reference guidance off)");
    };
    closeReference.onClick = [this] { referenceDrawer.setVisible(false); };
    openReference.onClick = [this] {
        drawer.setVisible(false);
        referenceDrawer.setVisible(!referenceDrawer.isVisible());
        if (referenceDrawer.isVisible())
            referenceDrawer.toFront(false);
    };

    // Optional Details / Log drawer — closed by default.
    results.setMultiLine(true);
    results.setReadOnly(true);
    results.setFont(hardware.font(17.5f));
    results.setBorder(juce::BorderSize<int>(48, 18, 18, 18));
    results.setText(
        "D.A PLUGINS / LEARNING WORKBENCH\n\nPress CAPTURE INPUT, LEARN ALL, or a parameter/module "
        "LEARN button while playing audio. Results stay in this optional log and do not change your "
        "settings until Preview or Apply.\n\nInput-aware learning uses measured features and "
        "deterministic estimates, not a trained neural network. Open REFERENCE... only when you "
        "want optional reference-file guidance (currently limited to EQ gains, stereo width and "
        "compressor makeup; other proposals are input/objective starting points, not exact "
        "reference matches).\n\nPreview auditions; Reject restores; Apply keeps; Undo restores one "
        "learning operation. Manual changes invalidate older proposals.");
    drawer.addAndMakeVisible(results);
    drawer.addAndMakeVisible(closeResults);
    stage.addChildComponent(drawer);
    drawer.setVisible(false);
    results.setComponentID("learning-log");
    closeResults.onClick = [this] { drawer.setVisible(false); };
    showResults.onClick = [this] {
        referenceDrawer.setVisible(false);
        drawer.setVisible(!drawer.isVisible());
        if (drawer.isVisible())
            drawer.toFront(false);
    };

    status.setFont(hardware.font(17.5f, true));
    status.setJustificationType(juce::Justification::centredRight);
    status.setColour(juce::Label::textColourId, juce::Colour(0xffe0e0ec));
    stage.addAndMakeVisible(status);

    captureInput.onClick = [this] { startLearn("all"); };
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
    setStatus("Ready / Manual control always");
    setSize(1120, 860);
    startTimerHz(30);
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
    analysis.setStatus(s.upToFirstOccurrenceOf(" - ", false, false));
}
void DAPluginAudioProcessorEditor::syncManual() {
    if (processor.isLoadingProgram())
        return;
    const auto generation = processor.presetGeneration();
    auto values = processor.currentValues();
    if (generation != observedPresetGeneration) {
        observedPresetGeneration = generation;
        const auto revision = session.revision + 1;
        session = da::LearningSession(values);
        session.revision = revision;
        lastValues = values;
        capturing = false;
        processor.cancelCapture();
        presetMenu.setSelectedId(processor.getCurrentProgram() + 1, juce::dontSendNotification);
        setStatus("Preset / session loaded - pending learning cancelled");
        return;
    }
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
        setStatus("Manual/host change - unaccepted learning cancelled");
    }
}
void DAPluginAudioProcessorEditor::write() {
    processor.writeValues(session.values);
    lastValues = processor.currentValues();
    analysis.setParameters(lastValues);
    setStatus(session.status);
}
void DAPluginAudioProcessorEditor::startLearn(std::string requestedScope) {
    syncManual();
    if (capturing || learnWork.valid() || referenceWork.valid()) {
        setStatus("Analysis busy - cancel or wait");
        return;
    }
    session.reject();
    ++session.revision;
    write();
    try {
        objective = {};
        if (referenceGuidanceEnabled) {
            objective.mode = da::Mode::Reference;
            if (!reference.valid())
                throw std::runtime_error("Reference unavailable - load a non-silent audio file");
        }
        if (!processor.requestCapture())
            throw std::runtime_error("Capture busy - play audio to finish the pending capture");
        scope = std::move(requestedScope);
        captureRevision = session.revision;
        capturing = true;
        captureStarted = juce::Time::currentTimeMillis();
        setStatus("Analyzing - capturing 65,536 input samples; keep audio playing");
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
                safe->setStatus("Cancelled - no reference selected");
                return;
            }
            safe->syncManual();
            safe->session.reject();
            ++safe->session.revision;
            safe->write();
            safe->reference = {};
            safe->analysis.setReference({});
            safe->referenceRevision = safe->session.revision;
            safe->setStatus("Analyzing reference - decoding up to 30 seconds on worker");
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
    if (processor.isLoadingProgram())
        return;
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
        setStatus("Failed - capture timed out; transport must be running");
    }
    if (ready(referenceWork))
        try {
            auto analyzedReference = referenceWork.get();
            if (referenceRevision != session.revision) {
                setStatus("Cancelled - stale reference analysis discarded");
                return;
            }
            reference = analyzedReference.features;
            analysis.setReference(analyzedReference);
            referenceGuidanceEnabled = true;
            useReferenceMode.setToggleState(true, juce::dontSendNotification);
            referenceDrawer.repaint();
            setStatus("Reference available - RMS " + juce::String(da::db(float(reference.rms)), 1) +
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
                setStatus("Cancelled - stale analysis discarded");
        } catch (const std::exception &e) {
            setStatus(juce::String("Failed: ") + e.what());
        }
    analysis.setParameters(processor.currentValues());
    analysis.consume(processor.scopeFifo, processor.displaySampleRate.load(),
                     processor.inputPeak.load(), processor.outputPeak.load(),
                     processor.reduction.load());
    if (processor.kind == da::Kind::Delay) {
        const bool sync = processor.parameters.getRawParameterValue("sync")->load() > .5f;
        for (auto &c : controls)
            if (c->getComponentID() == "time" || c->getComponentID() == "modulation")
                c->slider.setEnabled(!sync);
    }
    const auto current = processor.getCurrentProgram();
    bool modified = false;
    for (size_t i = 0; i < lastValues.size(); ++i)
        modified |= std::abs(lastValues[i] - processor.factoryBank[size_t(current)].values[i]) > 1e-4f;
    presetMenu.setText(juce::String(current + 1).paddedLeft('0', 2) + " / " +
                           processor.getProgramName(current) + (modified ? " *" : ""),
                       juce::dontSendNotification);
    presetMenu.setTooltip(modified ? "Modified factory preset / save project to retain edits"
                                   : "Factory preset / 50 available");
    chassis.repaint();
}
void DAPluginAudioProcessorEditor::paint(juce::Graphics &g) { g.fillAll(juce::Colour(0xff131316)); }
void DAPluginAudioProcessorEditor::paintDetails(juce::Graphics &g) {
    if (processor.kind == da::Kind::Delay) {
        const auto state = processor.tempoStatus.load();
        const auto bpm = processor.projectBpm.load();
        const auto sync = processor.parameters.getRawParameterValue("sync")->load() > .5f;
        const auto division = int(processor.parameters.getRawParameterValue("division")->load());
        auto text = juce::String(state == 1 ? "SYNC / " : state == 2 ? "HOLD / " : "FALLBACK / ") +
                    juce::String(bpm, 0) + " BPM";
        if (sync)
            text += " / " + juce::String(da::divisionNames[size_t(std::clamp(division, 0, 14))]) +
                    " / " + juce::String(da::syncedDelayMs(bpm, division), 0) + " ms";
        da::ui::label(g, hardware, text, {230.f, 355.f, 640.f, 24.f}, 16.5f,
                      hardware.accentColour.interpolatedWith(juce::Colour(0xffe0e0ea), .35f),
                      juce::Justification::centredLeft, true);
    }
}
void DAPluginAudioProcessorEditor::resized() {
    // Stretch stage cleanly across the full editor bounds so there are zero side margins.
    stage.setTransform(juce::AffineTransform::scale(float(getWidth()) / 1120.f,
                                                    float(getHeight()) / 860.f));
    previousPreset.setBounds(550, 28, 42, 36);
    presetMenu.setBounds(600, 28, 452, 36);
    nextPreset.setBounds(1060, 28, 42, 36);

    captureInput.setBounds(170, 308, 220, 36);
    openReference.setBounds(402, 308, 220, 36);
    learnAll.setBounds(634, 308, 220, 36);
    showResults.setBounds(866, 308, 226, 36);

    int x = 14;
    for (auto *b : {&preview, &apply, &reject, &undo}) {
        b->setBounds(x, 814, 148, 34);
        x += 158;
    }
    status.setBounds(650, 814, 456, 34);

    referenceDrawer.setBounds(160, 140, 800, 360);
    loadReference.setBounds(28, 258, 220, 44);
    useReferenceMode.setBounds(264, 258, 260, 44);
    closeReference.setBounds(648, 18, 124, 36);

    drawer.setBounds(14, 382, 1092, 420);
    results.setBounds(drawer.getLocalBounds());
    closeResults.setBounds(856, 10, 220, 32);
}
