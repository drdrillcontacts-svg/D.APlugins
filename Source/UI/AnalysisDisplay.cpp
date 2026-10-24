#include "AnalysisDisplay.h"
#include <complex>
namespace da::ui {
namespace {
void waveform(juce::Graphics &g, const HardwareLook &look, juce::Rectangle<float> r, const float *v,
              size_t n, bool ready) {
    if (!ready) {
        label(g, look, "No reference file selected", {r.getX(), r.getY() + 36.f, r.getWidth(), 28.f},
              19.f, juce::Colour(0xffe0e0ea), juce::Justification::centred, true);
        label(g, look, "Load a file when you need reference-based analysis.",
              {r.getX(), r.getY() + 72.f, r.getWidth(), 28.f}, 17.f, juce::Colour(0xffbabac8),
              juce::Justification::centred, false);
        return;
    }
    g.setColour(juce::Colour(0xff25252c));
    g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY());
    float maximum = .02f;
    for (size_t i = 0; i < n; ++i)
        maximum = std::max(maximum, std::abs(v[i]));
    g.setColour(look.accentColour.withAlpha(.88f));
    for (int x = 0; x < int(r.getWidth()); ++x) {
        size_t start = size_t(x) * n / size_t(r.getWidth()),
               end = std::max(start + 1, size_t(x + 1) * n / size_t(r.getWidth()));
        float peak = 0;
        for (size_t j = start; j < std::min(end, n); ++j)
            peak = std::max(peak, std::abs(v[j]));
        float h = peak / maximum * r.getHeight() * .43f;
        g.drawLine(r.getX() + float(x), r.getCentreY() - h, r.getX() + float(x),
                   r.getCentreY() + h, 1.2f);
    }
}
} // namespace
void meter(juce::Graphics &g, const HardwareLook &look, juce::Rectangle<float> r, float db,
           bool scale) {
    int segments = 40;
    float step = r.getHeight() / float(segments);
    auto hiCol = look.accentColour.interpolatedWith(juce::Colours::white, .25f);
    for (int j = 0; j < segments; ++j) {
        float threshold = -6.f - float(j) * 66.f / float(segments);
        bool on = db >= threshold;
        g.setColour(on ? (j < 7    ? look.accentColour
                          : j < 16 ? hiCol
                                   : juce::Colour(0xffdedee4))
                       : juce::Colour(0xff26262c));
        g.fillRect(r.getX(), r.getY() + float(j) * step, r.getWidth(), step * .56f);
    }
    if (scale)
        for (int i = 0; i <= 6; ++i) {
            float d = float(i) * -12;
            label(g, look, juce::String(int(d)),
                  {r.getRight() + 6, r.getY() + r.getHeight() * float(i) / 6 - 7, 30, 14}, 14,
                  muted);
        }
}
AnalysisDisplay::AnalysisDisplay(da::Kind k, const HardwareLook &f)
    : kind(k), look(f), metadata(da::parameters(k)) {
    setInterceptsMouseClicks(false, false);
    for (auto &p : metadata)
        parameters.push_back(p.initial);
    spectrum.fill(-90);
    outSpectrum.fill(-90);
    for (auto &h : history)
        h.fill(-90);
}
float AnalysisDisplay::value(const char *id, float fallback) const {
    for (size_t i = 0; i < metadata.size() && i < parameters.size(); ++i)
        if (metadata[i].id == id)
            return parameters[i];
    return fallback;
}
void AnalysisDisplay::transform(const std::array<float, 2048> &source,
                                std::array<float, 128> &bins) {
    std::array<float, 4096> data{};
    for (size_t i = 0; i < source.size(); ++i)
        data[i] = source[(position + i) % source.size()] *
                  float(.5 - .5 * std::cos(2 * da::pi * double(i) / 2047));
    fft.performFrequencyOnlyForwardTransform(data.data());
    for (size_t i = 0; i < bins.size(); ++i) {
        double f = 20 * std::pow(1000., double(i) / 127.);
        int index = std::clamp(int(f / rate * 2048), 1, 1023);
        float d = da::db(data[size_t(index)] / 512.f);
        bins[i] = std::max(std::clamp(d, -90.f, 6.f), bins[i] - .9f);
    }
}
void AnalysisDisplay::consume(da::ScopeFifo &fifo, double sr, float ip, float op, float reduction) {
    rate = sr;
    std::array<da::ScopeFrame, 1024> frames{};
    size_t n = 0, total = 0;
    while ((n = fifo.pop(frames.data(), frames.size())) != 0) {
        total += n;
        for (size_t j = 0; j < n; ++j) {
            input[position] = frames[j].input;
            output[position] = frames[j].output;
            position = (position + 1) % input.size();
            phase[phasePosition++ % phase.size()] = frames[j];
        }
    }
    if (total) {
        lastAudio = juce::Time::currentTimeMillis();
        transform(input, spectrum);
        transform(output, outSpectrum);
        history[historyPosition++ % history.size()] = spectrum;
    }
    bool recent = juce::Time::currentTimeMillis() - lastAudio < 350;
    inDb = std::max(recent ? da::db(ip) : -90.f, inDb - 2.5f);
    outDb = std::max(recent ? da::db(op) : -90.f, outDb - 2.5f);
    gr = recent ? reduction : 0;
    if (!recent)
        for (auto &x : spectrum)
            x = std::max(-90.f, x - 2);
    repaint();
}
void AnalysisDisplay::paint(juce::Graphics &g) {
    const char *roles[]{"REVERBERATION", "STEREO DELAY",      "DYNAMICS",          "FOUR-BAND EQ",
                        "SATURATION",    "TRANSIENT SHAPING", "STEREO IMAGE",      "SIBILANCE CONTROL",
                        "NOISE GATE",    "MULTI EFFECT"};
    const float w = float(getWidth()), h = float(getHeight());
    label(g, look, "IN", {4.f, 10.f, 40.f, 20.f}, 16.5f, look.accentColour,
          juce::Justification::centred, true);
    meter(g, look, {12.f, 34.f, 9.f, h - 44.f}, inDb);
    meter(g, look, {25.f, 34.f, 9.f, h - 44.f}, inDb);

    label(g, look, "INPUT / " + juce::String(roles[int(kind)]) + " / " + state.toUpperCase(),
          {54.f, 6.f, w - 108.f, 22.f}, 16.5f, look.accentColour, juce::Justification::centredLeft,
          true);

    graph(g, {54.f, 32.f, w - 108.f, h - 42.f});

    label(g, look, "OUT", {w - 44.f, 10.f, 40.f, 20.f}, 16.5f, look.accentColour,
          juce::Justification::centred, true);
    meter(g, look, {w - 34.f, 34.f, 9.f, h - 44.f}, outDb);
    meter(g, look, {w - 21.f, 34.f, 9.f, h - 44.f}, outDb);
}
void AnalysisDisplay::paintReferencePanel(juce::Graphics &g, juce::Rectangle<float> box) const {
    g.setColour(juce::Colour(0xff111114));
    g.fillRoundedRectangle(box, 16.f);
    g.setColour(juce::Colour(0xff42424d));
    g.drawRoundedRectangle(box.reduced(.5f), 16.f, 1.2f);
    label(g, look, "REFERENCE", {box.getX() + 28.f, box.getY() + 18.f, 300.f, 30.f}, 24.f,
          look.accentColour, juce::Justification::centredLeft, true);
    label(g, look, "Optional workspace - opened only when requested",
          {box.getX() + 28.f, box.getY() + 56.f, 680.f, 24.f}, 18.f, juce::Colour(0xffe0e0ea),
          juce::Justification::centredLeft, false);
    auto waveBox = juce::Rectangle<float>(box.getX() + 28.f, box.getY() + 94.f,
                                          box.getWidth() - 56.f, 150.f);
    g.setColour(juce::Colour(0xff0d0d10));
    g.fillRoundedRectangle(waveBox, 10.f);
    g.setColour(juce::Colour(0xff41414a));
    g.drawRoundedRectangle(waveBox, 10.f, 1.f);
    waveform(g, look, waveBox.reduced(16.f, 12.f), reference.waveform.data(),
             reference.waveform.size(), reference.features.valid());
    juce::String foot = reference.features.valid()
                            ? ("Loaded: " + reference.name + "  /  RMS " +
                               juce::String(da::db(float(reference.features.rms)), 1) + " dBFS")
                            : "Reference guidance is optional - no file loaded by default.";
    label(g, look, foot,
          {box.getX() + 28.f, box.getBottom() - 38.f, box.getWidth() - 56.f, 24.f}, 16.5f,
          juce::Colour(0xffcbc8d8), juce::Justification::centredLeft, false);
}
void AnalysisDisplay::graph(juce::Graphics &g, juce::Rectangle<float> r) {
    g.saveState();
    g.reduceClipRegion(r.toNearestInt());
    auto gridCol = juce::Colour(0xff25252d).interpolatedWith(look.accentColour, .22f);
    auto midCol = juce::Colour(0xff2b2b34).interpolatedWith(look.accentColour, .46f);
    auto darkFill = juce::Colour(0xff121216).interpolatedWith(look.accentColour, .18f);

    // Optional live FFT spectrum underlay whenever audio is playing
    if (inDb > -85.f || outDb > -85.f) {
        juce::Path specPath;
        specPath.startNewSubPath(r.getX(), r.getBottom());
        for (size_t i = 0; i < outSpectrum.size(); ++i) {
            float tx = float(i) / float(outSpectrum.size() - 1);
            float norm = std::clamp((outSpectrum[i] + 78.f) / 84.f, 0.f, 1.f);
            specPath.lineTo(r.getX() + tx * r.getWidth(), r.getBottom() - norm * r.getHeight() * .85f);
        }
        specPath.lineTo(r.getRight(), r.getBottom());
        specPath.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.09f));
        g.fillPath(specPath);
    }

    // -------------------------------------------------------------------------
    // 1. DComp — Live Soft-Knee Transfer Curve + Attack/Release Ballistics
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Comp) {
        float thresh = value("threshold", -18.f); // -60..0 dB
        float ratio = std::max(1.f, value("ratio", 4.f)); // 1..20:1
        float attack = value("attack", 15.f); // 0.1..200 ms
        float release = value("release", 150.f); // 10..2000 ms
        float knee = value("knee", 6.f); // 0..24 dB
        float makeup = value("makeup", 0.f); // 0..24 dB
        float mix = value("mix", 1.f); // 0..1
        float output = value("output", 0.f); // -24..12 dB

        auto compBox = juce::Rectangle<float>(r.getX(), r.getY(), r.getWidth() * .66f, r.getHeight());
        auto envBox = juce::Rectangle<float>(r.getX() + r.getWidth() * .69f, r.getY(),
                                             r.getWidth() * .31f, r.getHeight());

        // Grid & 1:1 unity diagonal inside compBox (-60..0 dB)
        g.setColour(gridCol.withAlpha(.65f));
        for (int dbLine : {-48, -36, -24, -12}) {
            float t = (float(dbLine) + 60.f) / 60.f;
            float gx = compBox.getX() + t * compBox.getWidth();
            float gy = compBox.getBottom() - t * compBox.getHeight();
            g.drawLine(gx, compBox.getY(), gx, compBox.getBottom(), 1.f);
            g.drawLine(compBox.getX(), gy, compBox.getRight(), gy, 1.f);
        }
        g.setColour(midCol.withAlpha(.7f));
        g.drawLine(compBox.getX(), compBox.getBottom(), compBox.getRight(), compBox.getY(), 1.2f);

        // Shaded knee region around threshold
        if (knee > 0.2f) {
            float k0 = std::clamp((thresh - knee * .5f + 60.f) / 60.f, 0.f, 1.f);
            float k1 = std::clamp((thresh + knee * .5f + 60.f) / 60.f, 0.f, 1.f);
            g.setColour(look.accentColour.withAlpha(.09f));
            g.fillRect(compBox.getX() + k0 * compBox.getWidth(), compBox.getY(),
                       std::max(2.f, (k1 - k0) * compBox.getWidth()), compBox.getHeight());
        }

        // Threshold vertical marker line
        float thrNorm = std::clamp((thresh + 60.f) / 60.f, 0.f, 1.f);
        float thrX = compBox.getX() + thrNorm * compBox.getWidth();
        g.setColour(look.accentColour.withAlpha(.45f));
        g.drawLine(thrX, compBox.getY(), thrX, compBox.getBottom(), 1.2f);

        auto computeOutDb = [&](float inLevelDb) {
            float over = inLevelDb - thresh;
            float gainChange = 0.f;
            float slope = 1.f / ratio - 1.f;
            if (knee > .01f && std::abs(over) < knee * .5f) {
                float u = over + knee * .5f;
                gainChange = slope * (u * u) / (2.f * knee);
            } else if (over >= knee * .5f) {
                gainChange = slope * over;
            }
            float wetDb = inLevelDb + gainChange + makeup;
            float blendedDb = inLevelDb * (1.f - mix) + wetDb * mix + output;
            return blendedDb;
        };

        juce::Path curve, fill;
        fill.startNewSubPath(compBox.getX(), compBox.getBottom());
        for (int i = 0; i <= 120; ++i) {
            float t = float(i) / 120.f;
            float inLevelDb = -60.f + t * 60.f;
            float outLevelDb = computeOutDb(inLevelDb);
            float outNorm = std::clamp((outLevelDb + 60.f) / 60.f, 0.f, 1.05f);
            float px = compBox.getX() + t * compBox.getWidth();
            float py = compBox.getBottom() - outNorm * compBox.getHeight();
            if (i == 0)
                curve.startNewSubPath(px, py);
            else
                curve.lineTo(px, py);
            fill.lineTo(px, py);
        }
        fill.lineTo(compBox.getRight(), compBox.getBottom());
        fill.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.15f));
        g.fillPath(fill);
        g.setColour(look.accentColour);
        g.strokePath(curve, juce::PathStrokeType(2.8f));

        // Knee handle circle at threshold
        float kneeOutNorm = std::clamp((computeOutDb(thresh) + 60.f) / 60.f, 0.f, 1.f);
        float kneeY = compBox.getBottom() - kneeOutNorm * compBox.getHeight();
        g.setColour(look.accentColour);
        g.fillEllipse(thrX - 5.f, kneeY - 5.f, 10.f, 10.f);
        g.setColour(juce::Colours::white);
        g.drawEllipse(thrX - 5.f, kneeY - 5.f, 10.f, 10.f, 1.5f);

        // Right sub-panel: Attack / Release Ballistics & Gain Reduction envelope
        g.setColour(darkFill.withAlpha(.65f));
        g.fillRoundedRectangle(envBox, 6.f);
        g.setColour(gridCol);
        g.drawRoundedRectangle(envBox, 6.f, 1.f);

        float atkNorm = std::clamp(std::log10(std::max(.1f, attack) / .1f) / std::log10(2000.f), .04f, .45f);
        float relNorm = std::clamp(std::log10(std::max(10.f, release) / 10.f) / std::log10(200.f), .08f, .48f);
        float depthNorm = std::clamp((-thresh / 60.f) * (1.f - 1.f / ratio) * mix, .08f, .82f);

        juce::Path ballistics;
        float topY = envBox.getY() + 26.f;
        float botY = envBox.getBottom() - 22.f;
        float dipY = topY + depthNorm * (botY - topY);
        float x0 = envBox.getX() + 10.f;
        float x1 = x0 + (envBox.getWidth() - 20.f) * .12f;
        float x2 = x1 + (envBox.getWidth() - 20.f) * atkNorm;
        float x3 = x2 + (envBox.getWidth() - 20.f) * .18f;
        float x4 = std::min(envBox.getRight() - 10.f, x3 + (envBox.getWidth() - 20.f) * relNorm);

        ballistics.startNewSubPath(x0, topY);
        ballistics.lineTo(x1, topY);
        ballistics.quadraticTo(x1 + (x2 - x1) * .45f, dipY, x2, dipY);
        ballistics.lineTo(x3, dipY);
        ballistics.quadraticTo(x3 + (x4 - x3) * .5f, topY, x4, topY);
        ballistics.lineTo(envBox.getRight() - 10.f, topY);
        g.setColour(look.accentColour);
        g.strokePath(ballistics, juce::PathStrokeType(2.4f));

        label(g, look,
              "THR " + juce::String(thresh, 0) + " dB  |  " + juce::String(ratio, 1) +
                  ":1  |  KNEE " + juce::String(knee, 0) + " dB  |  MAKEUP +" +
                  juce::String(makeup, 1) + " dB",
              {compBox.getX() + 8.f, compBox.getY() + 4.f, compBox.getWidth() - 16.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        label(g, look,
              "ATK " + juce::String(attack, attack < 10 ? 1 : 0) + " ms / REL " +
                  juce::String(release, 0) + " ms",
              {envBox.getX() + 8.f, envBox.getY() + 4.f, envBox.getWidth() - 16.f, 18.f}, 14.5f,
              look.accentColour, juce::Justification::centred, true);
        label(g, look,
              "MIX " + juce::String(int(std::round(mix * 100.f))) + "%  |  OUT " +
                  (output >= 0 ? "+" : "") + juce::String(output, 1) + " dB",
              {envBox.getX() + 8.f, envBox.getBottom() - 20.f, envBox.getWidth() - 16.f, 18.f},
              14.f, juce::Colour(0xffd8d8e4), juce::Justification::centred, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 2. DEQ — Live 4-Band Parametric Frequency Response + Movable Band Nodes
    // -------------------------------------------------------------------------
    if (kind == da::Kind::EQ) {
        g.setColour(gridCol);
        g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 1.2f);
        for (float dbGuide : {-12.f, -6.f, 6.f, 12.f}) {
            float gy = r.getCentreY() - dbGuide * r.getHeight() / 36.f;
            g.setColour(gridCol.withAlpha(.45f));
            g.drawLine(r.getX(), gy, r.getRight(), gy, 1.f);
        }
        for (double fGuide : {50., 200., 1000., 5000., 15000.}) {
            float gx = r.getX() + float(std::log(fGuide / 20.) / std::log(1000.)) * r.getWidth();
            g.setColour(gridCol.withAlpha(.45f));
            g.drawLine(gx, r.getY(), gx, r.getBottom(), 1.f);
        }

        auto bandMagDb = [&](int b, double f) {
            size_t p = size_t(b) * 4;
            if (p + 3 >= parameters.size())
                return 0.0;
            double fc = std::clamp(double(parameters[p]), 20., rate * .45);
            double gainDb = double(parameters[p + 1]);
            double qv = std::max(0.2, double(parameters[p + 2]));
            int shape = int(std::round(parameters[p + 3]));
            double A = std::pow(10., gainDb / 40.);
            double w = 2 * da::pi * std::min(f, rate * .49) / rate;
            std::complex<double> z = std::polar(1., -w), z2 = z * z;
            double c = std::cos(2 * da::pi * fc / rate), s = std::sin(2 * da::pi * fc / rate),
                   alpha = s / (2 * qv);
            double b0 = 1 + alpha * A, b1 = -2 * c, b2 = 1 - alpha * A, a0 = 1 + alpha / A,
                   a1 = -2 * c, a2 = 1 - alpha / A;
            double t = 2 * std::sqrt(A) * s / std::sqrt(2.);
            if (shape == 1) {
                b0 = A * ((A + 1) - (A - 1) * c + t);
                b1 = 2 * A * ((A - 1) - (A + 1) * c);
                b2 = A * ((A + 1) - (A - 1) * c - t);
                a0 = (A + 1) + (A - 1) * c + t;
                a1 = -2 * ((A - 1) + (A + 1) * c);
                a2 = (A + 1) + (A - 1) * c - t;
            } else if (shape == 2) {
                b0 = A * ((A + 1) + (A - 1) * c + t);
                b1 = -2 * A * ((A - 1) + (A + 1) * c);
                b2 = A * ((A + 1) + (A - 1) * c - t);
                a0 = (A + 1) - (A - 1) * c + t;
                a1 = 2 * ((A - 1) - (A + 1) * c);
                a2 = (A + 1) - (A - 1) * c - t;
            }
            double mag = std::abs((b0 + b1 * z + b2 * z2) / (a0 + a1 * z + a2 * z2));
            return 20.0 * std::log10(std::max(mag, 1e-12));
        };

        // Per-band individual response curves (uses a phantom preview guide if gain == 0 dB so Q & Shape are always visible)
        for (int b = 0; b < 4; ++b) {
            size_t p = size_t(b) * 4;
            if (p + 3 >= parameters.size())
                continue;
            double fc = std::clamp(double(parameters[p]), 20., 20000.);
            double gainDb = double(parameters[p + 1]);
            double qv = std::max(0.2, double(parameters[p + 2]));
            int shape = int(std::round(parameters[p + 3]));
            juce::Path bp;
            for (int i = 0; i < 160; ++i) {
                double f = 20. * std::pow(1000., double(i) / 159.);
                double dbVal = bandMagDb(b, f);
                if (std::abs(gainDb) < 0.25) {
                    double logDist = std::log2(f / fc) * qv;
                    if (shape == 0)
                        dbVal = 4.5 * std::exp(-logDist * logDist * 1.8);
                    else if (shape == 1)
                        dbVal = 4.5 / (1.0 + std::exp(logDist * 2.5));
                    else
                        dbVal = 4.5 / (1.0 + std::exp(-logDist * 2.5));
                }
                float px = r.getX() + r.getWidth() * float(i) / 159.f;
                float py = r.getCentreY() - float(dbVal) * r.getHeight() / 40.f;
                if (i == 0)
                    bp.startNewSubPath(px, py);
                else
                    bp.lineTo(px, py);
            }
            g.setColour(look.accentColour.withAlpha(.28f));
            g.strokePath(bp, juce::PathStrokeType(1.2f));
        }

        // Composite 4-band response curve + fill
        juce::Path compPath, compFill;
        compFill.startNewSubPath(r.getX(), r.getCentreY());
        for (int i = 0; i < 256; ++i) {
            double f = 20. * std::pow(1000., double(i) / 255.);
            double totalDb = bandMagDb(0, f) + bandMagDb(1, f) + bandMagDb(2, f) + bandMagDb(3, f);
            float px = r.getX() + r.getWidth() * float(i) / 255.f;
            float py = std::clamp(r.getCentreY() - float(totalDb) * r.getHeight() / 40.f,
                                  r.getY() + 2.f, r.getBottom() - 2.f);
            if (i == 0)
                compPath.startNewSubPath(px, py);
            else
                compPath.lineTo(px, py);
            compFill.lineTo(px, py);
        }
        compFill.lineTo(r.getRight(), r.getCentreY());
        compFill.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.18f));
        g.fillPath(compFill);
        g.setColour(look.accentColour);
        g.strokePath(compPath, juce::PathStrokeType(2.8f));

        // 4 Live Band Nodes positioned at each band's exact (freq, gain)
        for (int b = 0; b < 4; ++b) {
            size_t p = size_t(b) * 4;
            if (p + 1 >= parameters.size())
                continue;
            double fc = std::clamp(double(parameters[p]), 20., 20000.);
            double gainDb = double(parameters[p + 1]);
            float nx = r.getX() + float(std::log(fc / 20.) / std::log(1000.)) * r.getWidth();
            float ny = std::clamp(r.getCentreY() - float(gainDb) * r.getHeight() / 40.f,
                                  r.getY() + 10.f, r.getBottom() - 10.f);
            g.setColour(look.accentColour.withAlpha(.35f));
            g.drawLine(nx, r.getCentreY(), nx, ny, 1.2f);
            g.setColour(look.accentColour);
            g.fillEllipse(nx - 8.f, ny - 8.f, 16.f, 16.f);
            g.setColour(juce::Colour(0xff101014));
            label(g, look, juce::String(b + 1), {nx - 8.f, ny - 8.f, 16.f, 16.f}, 14.f,
                  juce::Colour(0xff101014), juce::Justification::centred, true);
        }
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 3. DVerb — Live Pre-Delay, RT60 Decay Tail, Density/Diffusion & Filter
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Verb) {
        float predelay = value("predelay", 20.f); // 0..200 ms
        float size = value("size", .6f); // 0..1
        float decay = value("decay", 2.f); // 0.1..12 s
        float tone = value("tone", 10000.f); // 500..18000 Hz
        float mix = value("mix", .25f); // 0..1
        float width = value("width", 1.f); // 0..2
        float density = value("density", .7f); // 0..1
        float diffusion = value("diffusion", .7f); // 0..1
        float mod = value("modulation", .1f); // 0..1
        float lowcut = value("lowcut", 120.f); // 20..1000 Hz
        float midcut = value("midcut", -1.f); // -12..0 dB
        int mode = int(std::round(value("mode", 1.f))); // 0..3

        g.setColour(gridCol);
        g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 1.f);

        float preFrac = std::clamp(predelay / 200.f, 0.f, 1.f) * .18f;
        float startX = r.getX() + preFrac * r.getWidth();
        g.setColour(look.accentColour.withAlpha(.45f));
        g.drawLine(startX, r.getY() + 20.f, startX, r.getBottom() - 6.f, 1.4f);

        float decayNorm = std::clamp((std::log10(std::max(.1f, decay) / .1f) / std::log10(120.f)) *
                                         (.65f + .35f * size),
                                     .08f, 1.f);
        float rateDecay = 1.2f + (1.f - decayNorm) * 7.5f;
        float modeScale = mode == 0 ? 1.35f : (mode == 1 ? .95f : (mode == 2 ? 1.1f : .75f));
        float ampScale = (.28f + .68f * mix) * (.7f + .3f * std::min(width, 1.5f));

        juce::Path upperEnv, lowerEnv, fillEnv;
        fillEnv.startNewSubPath(startX, r.getCentreY());
        int N = 136;
        for (int i = 0; i < N; ++i) {
            float t = float(i) / float(N - 1);
            float xx = startX + t * (r.getRight() - startX);
            float env = (1.f - std::exp(-t * (18.f + 25.f * diffusion))) *
                        std::exp(-t * rateDecay * modeScale);
            float ripple = 1.f - (1.f - density) * .45f * std::abs(std::sin(float(i) * 1.9f)) +
                           mod * .22f * std::sin(t * 24.f + float(mode));
            float a = r.getHeight() * .44f * ampScale * std::clamp(env * ripple, 0.f, 1.f);
            float asymL = a * (1.f + .12f * (width - 1.f) * std::sin(t * 9.f));
            float asymR = a * (1.f - .12f * (width - 1.f) * std::cos(t * 9.f));
            if (i == 0) {
                upperEnv.startNewSubPath(xx, r.getCentreY() - asymL);
                lowerEnv.startNewSubPath(xx, r.getCentreY() + asymR);
            } else {
                upperEnv.lineTo(xx, r.getCentreY() - asymL);
                lowerEnv.lineTo(xx, r.getCentreY() + asymR);
            }
            fillEnv.lineTo(xx, r.getCentreY() - asymL);
            g.setColour(look.accentColour.withAlpha(.58f));
            g.drawLine(xx, r.getCentreY() - asymL, xx, r.getCentreY() + asymR, 1.5f);
        }
        fillEnv.lineTo(r.getRight(), r.getCentreY());
        fillEnv.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.16f));
        g.fillPath(fillEnv);
        g.setColour(look.accentColour);
        g.strokePath(upperEnv, juce::PathStrokeType(2.4f));
        g.strokePath(lowerEnv, juce::PathStrokeType(2.0f));

        // Spectral filter overlay curve (lowcut, midcut, tone)
        juce::Path eqOverlay;
        float lcNorm = std::clamp(std::log10(std::max(20.f, lowcut) / 20.f) / std::log10(50.f), 0.f, 1.f);
        float hcNorm = std::clamp(std::log10(std::max(500.f, tone) / 500.f) / std::log10(36.f), 0.f, 1.f);
        for (int i = 0; i <= 80; ++i) {
            float u = float(i) / 80.f;
            float hp = 1.f / (1.f + std::exp(-(u - lcNorm * .35f) * 18.f));
            float lp = 1.f / (1.f + std::exp((u - (.35f + hcNorm * .6f)) * 16.f));
            float midDip = 1.f + (midcut / 12.f) * .35f * std::exp(-std::pow((u - .5f) / .18f, 2.f));
            float y = r.getBottom() - 8.f - (hp * lp * midDip) * (r.getHeight() * .32f);
            float x = r.getX() + u * r.getWidth();
            if (i == 0)
                eqOverlay.startNewSubPath(x, y);
            else
                eqOverlay.lineTo(x, y);
        }
        g.setColour(juce::Colours::white.withAlpha(.65f));
        g.strokePath(eqOverlay, juce::PathStrokeType(1.6f));

        const char *modeNames[]{"ROOM", "HALL", "PLATE", "SHIMMER"};
        label(g, look,
              juce::String(modeNames[std::clamp(mode, 0, 3)]) + "  |  DECAY " +
                  juce::String(decay, 2) + " s  |  PRE " + juce::String(predelay, 0) +
                  " ms  |  SIZE " + juce::String(int(std::round(size * 100.f))) + "%  |  TONE " +
                  juce::String(tone / 1000.f, 1) + " kHz",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 4. DDelay — Live Tap Timeline, Feedback Envelope, Ping-Pong & Filter
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Delay) {
        float timeMs = value("time", 375.f); // 1..2000 ms
        float fb = std::clamp(value("feedback", .35f), .02f, .95f);
        float filterHz = value("filter", 6000.f); // 200..18000 Hz
        bool pingpong = value("pingpong", 1.f) > .5f;
        float mod = value("modulation", .1f);
        float mix = value("mix", .25f);
        float width = value("width", 1.f);
        float outGain = value("output", 0.f);
        bool sync = value("sync", 0.f) > .5f;
        int div = std::clamp(int(std::round(value("division", 6.f))), 0, 14);

        float effectiveMs = sync ? float(da::syncedDelayMs(120.0, div)) : timeMs;
        float spacing = std::clamp(0.055f + (effectiveMs / 2000.f) * 0.105f, 0.055f, 0.145f);
        float baseY = pingpong ? r.getCentreY() : r.getY() + r.getHeight() * .80f;

        g.setColour(gridCol);
        g.drawLine(r.getX(), baseY, r.getRight(), baseY, 1.2f);

        float gainScale = std::clamp((.35f + .65f * mix) * std::pow(10.f, outGain / 40.f), .2f, 1.15f);
        float filterNorm = std::clamp(std::log10(std::max(200.f, filterHz) / 200.f) / std::log10(90.f), .1f, 1.f);

        juce::Path envPath;
        for (int j = 0; j < 8; ++j) {
            float xx = r.getX() + r.getWidth() * (.05f + float(j) * spacing);
            if (xx > r.getRight() - 12.f)
                break;
            float decayAmp = std::pow(fb, float(j) * .55f) * gainScale;
            float maxH = pingpong ? r.getHeight() * .42f : r.getHeight() * .66f;
            float amp = std::min(maxH, maxH * decayAmp);
            float dir = (pingpong && (j % 2 == 1)) ? 1.f : -1.f;
            float spread = pingpong ? (.55f + .45f * (width * .5f)) : 1.f;
            float tipY = baseY + dir * amp * spread;

            g.setColour(look.accentColour.withAlpha(.35f + .65f * filterNorm));
            g.drawLine(xx, baseY, xx, tipY, 3.6f);
            g.fillEllipse(xx - 4.5f, tipY - 4.5f, 9.f, 9.f);

            float envY = baseY - amp;
            if (j == 0)
                envPath.startNewSubPath(xx, envY);
            else
                envPath.lineTo(xx, envY);
            label(g, look, juce::String(j + 1), {xx - 14.f, baseY + 4.f, 28.f, 18.f}, 14.5f,
                  midCol, juce::Justification::centred, true);
        }
        g.setColour(look.accentColour.withAlpha(.55f));
        g.strokePath(envPath, juce::PathStrokeType(1.6f));

        // Modulation & filter wave across bottom/top
        if (mod > .01f || filterHz < 18000.f) {
            juce::Path modWave;
            for (int i = 0; i <= 90; ++i) {
                float u = float(i) / 90.f;
                float x = r.getX() + u * r.getWidth();
                float y = r.getY() + 26.f + (1.f - filterNorm) * 24.f +
                          std::sin(u * 18.f) * (2.f + mod * 14.f);
                if (i == 0)
                    modWave.startNewSubPath(x, y);
                else
                    modWave.lineTo(x, y);
            }
            g.setColour(juce::Colours::white.withAlpha(.55f));
            g.strokePath(modWave, juce::PathStrokeType(1.5f));
        }

        label(g, look,
              (sync ? ("SYNC " + juce::String(da::divisionNames[size_t(div)]))
                    : ("TIME " + juce::String(timeMs, 0) + " ms")) +
                  "  |  FB " + juce::String(int(std::round(fb * 100.f))) + "%  |  FILTER " +
                  juce::String(filterHz >= 1000.f ? filterHz / 1000.f : filterHz,
                               filterHz >= 1000.f ? 1 : 0) +
                  (filterHz >= 1000.f ? " kHz  |  " : " Hz  |  ") +
                  (pingpong ? "PING-PONG" : "STEREO"),
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 5. DSat — Live Nonlinear Waveshaper Curve + Driven Harmonic Waveform
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Sat) {
        float driveDb = value("drive", 6.f); // 0..30 dB
        float toneHz = value("tone", 10000.f); // 500..18000 Hz
        int mode = int(std::round(value("mode", 0.f))); // 0=Soft, 1=Hard, 2=Asymmetric
        float density = value("density", .5f); // 0..1
        float mix = value("mix", .5f); // 0..1
        float output = value("output", 0.f); // -24..12 dB

        auto leftBox = juce::Rectangle<float>(r.getX(), r.getY(), r.getWidth() * .52f, r.getHeight());
        auto rightBox = juce::Rectangle<float>(r.getX() + r.getWidth() * .55f, r.getY(),
                                               r.getWidth() * .45f, r.getHeight());

        g.setColour(gridCol);
        g.drawLine(leftBox.getX(), leftBox.getBottom(), leftBox.getRight(), leftBox.getY(), 1.f);
        g.drawLine(leftBox.getX(), leftBox.getCentreY(), leftBox.getRight(), leftBox.getCentreY(), 1.f);
        g.drawLine(leftBox.getCentreX(), leftBox.getY(), leftBox.getCentreX(), leftBox.getBottom(), 1.f);

        float gain = std::pow(10.f, driveDb / 20.f) * (.65f + .7f * density);
        float outScale = std::pow(10.f, output / 40.f);
        auto shapeSample = [&](float x) {
            float driven = x * gain;
            float wet = 0.f;
            if (mode == 0)
                wet = std::tanh(driven);
            else if (mode == 1)
                wet = std::clamp(driven, -1.f, 1.f);
            else
                wet = std::tanh(driven + .35f * density * driven * driven) - .15f * density;
            float blended = x * (1.f - mix) + wet * mix;
            return std::clamp(blended * outScale, -1.1f, 1.1f);
        };

        juce::Path curve;
        for (int i = 0; i <= 100; ++i) {
            float t = float(i) / 100.f;
            float xIn = t * 2.f - 1.f;
            float yOut = shapeSample(xIn);
            float px = leftBox.getX() + t * leftBox.getWidth();
            float py = leftBox.getCentreY() - yOut * (leftBox.getHeight() * .42f);
            if (i == 0)
                curve.startNewSubPath(px, py);
            else
                curve.lineTo(px, py);
        }
        g.setColour(look.accentColour);
        g.strokePath(curve, juce::PathStrokeType(2.8f));

        // Right box: driven harmonic sine wave tilted by tone
        g.setColour(darkFill.withAlpha(.55f));
        g.fillRoundedRectangle(rightBox, 6.f);
        g.setColour(gridCol);
        g.drawRoundedRectangle(rightBox, 6.f, 1.f);
        g.drawLine(rightBox.getX(), rightBox.getCentreY(), rightBox.getRight(), rightBox.getCentreY(), 1.f);

        float toneNorm = std::clamp(std::log10(std::max(500.f, toneHz) / 500.f) / std::log10(36.f), 0.f, 1.f);
        juce::Path wavePath;
        for (int i = 0; i <= 100; ++i) {
            float u = float(i) / 100.f;
            float sIn = std::sin(u * juce::MathConstants<float>::twoPi * 2.f);
            float sOut = shapeSample(sIn) + .18f * (toneNorm - .5f) * std::sin(u * juce::MathConstants<float>::twoPi * 6.f);
            float px = rightBox.getX() + 8.f + u * (rightBox.getWidth() - 16.f);
            float py = rightBox.getCentreY() - std::clamp(sOut, -1.1f, 1.1f) * (rightBox.getHeight() * .38f);
            if (i == 0)
                wavePath.startNewSubPath(px, py);
            else
                wavePath.lineTo(px, py);
        }
        g.setColour(look.accentColour);
        g.strokePath(wavePath, juce::PathStrokeType(2.4f));

        const char *modes[]{"SOFT", "HARD", "ASYMMETRIC"};
        label(g, look,
              juce::String(modes[std::clamp(mode, 0, 2)]) + "  |  DRIVE +" +
                  juce::String(driveDb, 1) + " dB  |  TONE " +
                  juce::String(toneHz / 1000.f, 1) + " kHz  |  DENSITY " +
                  juce::String(int(std::round(density * 100.f))) + "%  |  MIX " +
                  juce::String(int(std::round(mix * 100.f))) + "%",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 6. DTrans — Live Attack & Sustain Transient Envelope
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Trans) {
        float attackDb = value("attack", 0.f); // -18..18 dB
        float sustainDb = value("sustain", 0.f); // -18..18 dB
        float sens = value("sensitivity", .5f); // 0..1
        float shape = value("shape", 1.f); // 0.2..3
        float mix = value("mix", 1.f); // 0..1
        float output = value("output", 0.f); // -24..12 dB

        g.setColour(gridCol);
        g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 1.f);

        // Sensitivity threshold guide line
        float sensY = r.getBottom() - (.18f + sens * .64f) * r.getHeight();
        g.setColour(look.accentColour.withAlpha(.38f));
        g.drawLine(r.getX(), sensY, r.getRight(), sensY, 1.2f);

        float atkGain = std::pow(10.f, (attackDb * mix) / 28.f);
        float susGain = std::pow(10.f, (sustainDb * mix) / 28.f);
        float outScale = std::pow(10.f, output / 40.f);

        juce::Path envUpper, envLower, fillPath;
        fillPath.startNewSubPath(r.getX(), r.getCentreY());
        int N = 130;
        for (int i = 0; i < N; ++i) {
            float t = float(i) / float(N - 1);
            float xx = r.getX() + t * r.getWidth();
            float transPart = std::exp(- std::pow(t / .08f, shape)) * (1.f - std::exp(-t * 65.f));
            float susPart = (1.f - std::exp(-t * 14.f)) * std::exp(-std::pow(t * 2.2f, .7f * shape));
            float env = (transPart * .68f * atkGain + susPart * .48f * susGain) * outScale;
            float amp = std::clamp(env, 0.f, 1.05f) * r.getHeight() * .44f;
            float wave = amp * (.35f + .65f * std::abs(std::sin(float(i) * 1.65f)));
            g.setColour(look.accentColour.withAlpha(.58f));
            g.drawLine(xx, r.getCentreY() - wave, xx, r.getCentreY() + wave, 1.5f);
            if (i == 0) {
                envUpper.startNewSubPath(xx, r.getCentreY() - amp);
                envLower.startNewSubPath(xx, r.getCentreY() + amp);
            } else {
                envUpper.lineTo(xx, r.getCentreY() - amp);
                envLower.lineTo(xx, r.getCentreY() + amp);
            }
            fillPath.lineTo(xx, r.getCentreY() - amp);
        }
        fillPath.lineTo(r.getRight(), r.getCentreY());
        fillPath.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.16f));
        g.fillPath(fillPath);
        g.setColour(look.accentColour);
        g.strokePath(envUpper, juce::PathStrokeType(2.6f));
        g.strokePath(envLower, juce::PathStrokeType(1.8f));

        label(g, look,
              juce::String("ATTACK ") + (attackDb >= 0 ? "+" : "") + juce::String(attackDb, 1) +
                  " dB  |  SUSTAIN " + (sustainDb >= 0 ? "+" : "") + juce::String(sustainDb, 1) +
                  " dB  |  SENS " + juce::String(int(std::round(sens * 100.f))) + "%  |  SHAPE " +
                  juce::String(shape, 2) + "  |  MIX " +
                  juce::String(int(std::round(mix * 100.f))) + "%",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 7. DStereo — Live Mid/Side Vectorfield & Low-Frequency Depth Crossover
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Stereo) {
        float width = value("width", 1.f); // 0..2
        float depth = value("depth", 0.f); // 0..1
        float centerDb = value("center", 0.f); // -18..6 dB
        float sideDb = value("side", 0.f); // -18..6 dB
        float mix = value("mix", 1.f); // 0..1
        bool mono = value("mono", 0.f) > .5f;

        auto c = juce::Point<float>(r.getX() + r.getWidth() * .36f, r.getCentreY() + 6.f);
        float rad = std::min(r.getWidth() * .34f, r.getHeight() * .44f);
        g.setColour(gridCol);
        g.drawEllipse(c.x - rad * .5f, c.y - rad * .5f, rad, rad, 1.f);
        g.drawEllipse(c.x - rad, c.y - rad, rad * 2.f, rad * 2.f, 1.f);
        g.drawLine(c.x - rad, c.y, c.x + rad, c.y, 1.f);
        g.drawLine(c.x, c.y - rad, c.x, c.y + rad, 1.f);

        float effWidth = mono ? 0.02f : (1.f * (1.f - mix) + width * std::pow(10.f, sideDb / 20.f) * mix);
        float effMid = 1.f * (1.f - mix) + std::pow(10.f, centerDb / 20.f) * mix;
        float rx = std::clamp(effWidth * .55f, .02f, 1.15f) * rad;
        float ry = std::clamp(effMid * .68f, .15f, 1.05f) * rad;

        for (int j = 0; j < 8; ++j) {
            juce::Path liss;
            for (int i = 0; i <= 100; ++i) {
                float t = float(i) / 100.f * juce::MathConstants<float>::twoPi;
                float px = c.x + rx * std::sin(t + float(j) * .08f);
                float py = c.y + ry * std::sin(t * 2.f + float(j) * .16f);
                if (i == 0)
                    liss.startNewSubPath(px, py);
                else
                    liss.lineTo(px, py);
            }
            g.setColour(look.accentColour.withAlpha(j == 0 ? .85f : .26f));
            g.strokePath(liss, juce::PathStrokeType(j == 0 ? 2.2f : 1.1f));
        }

        // Right sub-panel: Width vs Frequency curve showing Depth mono safeguard
        auto rightBox = juce::Rectangle<float>(r.getX() + r.getWidth() * .66f, r.getY() + 24.f,
                                               r.getWidth() * .32f, r.getHeight() - 32.f);
        g.setColour(darkFill.withAlpha(.55f));
        g.fillRoundedRectangle(rightBox, 6.f);
        g.setColour(gridCol);
        g.drawRoundedRectangle(rightBox, 6.f, 1.f);

        juce::Path widthCurve;
        for (int i = 0; i <= 80; ++i) {
            float u = float(i) / 80.f;
            float hp = 1.f / (1.f + std::exp(-(u - depth * .55f) * 14.f));
            float wVal = mono ? 0.f : std::clamp((effWidth * .5f) * hp, 0.f, 1.f);
            float px = rightBox.getX() + 8.f + u * (rightBox.getWidth() - 16.f);
            float py = rightBox.getBottom() - 10.f - wVal * (rightBox.getHeight() - 24.f);
            if (i == 0)
                widthCurve.startNewSubPath(px, py);
            else
                widthCurve.lineTo(px, py);
        }
        g.setColour(look.accentColour);
        g.strokePath(widthCurve, juce::PathStrokeType(2.4f));

        label(g, look,
              "WIDTH " + juce::String(int(std::round(width * 100.f))) + "%  |  CENTER " +
                  (centerDb >= 0 ? "+" : "") + juce::String(centerDb, 1) + " dB  |  SIDE " +
                  (sideDb >= 0 ? "+" : "") + juce::String(sideDb, 1) + " dB  |  DEPTH " +
                  juce::String(int(std::round(depth * 100.f))) + "%  |  " +
                  (mono ? "MONO CHECK ON" : "STEREO"),
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 8. DSess — Live Sibilance Detector Band, Threshold & Attenuation Notch
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Sess) {
        float freq = value("frequency", 6000.f); // 2000..12000 Hz
        float thresh = value("threshold", -24.f); // -60..0 dB
        float range = value("range", 6.f); // 0..18 dB
        float attack = value("attack", 1.f); // 0.1..30 ms
        float release = value("release", 80.f); // 10..500 ms
        float focus = value("focus", .6f); // 0.2..1
        float mix = value("mix", 1.f); // 0..1
        float output = value("output", 0.f); // -24..12 dB

        float freqNorm = float(std::log(std::max(2000.f, freq) / 2000.f) / std::log(12000.f / 2000.f));
        float bandX = r.getX() + r.getWidth() * (.25f + .62f * std::clamp(freqNorm, 0.f, 1.f));
        float bandW = r.getWidth() * (.26f - .18f * std::clamp((focus - .2f) / .8f, 0.f, 1.f));

        g.setColour(look.accentColour.withAlpha(.16f));
        g.fillRoundedRectangle(bandX - bandW * .5f, r.getY() + 22.f, bandW, r.getHeight() - 24.f, 4.f);
        g.setColour(look.accentColour.withAlpha(.55f));
        g.drawRoundedRectangle(bandX - bandW * .5f, r.getY() + 22.f, bandW, r.getHeight() - 24.f, 4.f, 1.2f);

        // Threshold horizontal guide line
        float thrY = r.getY() + 26.f + (1.f - std::clamp((thresh + 60.f) / 60.f, 0.f, 1.f)) * (r.getHeight() - 34.f);
        g.setColour(juce::Colours::white.withAlpha(.45f));
        g.drawLine(r.getX(), thrY, r.getRight(), thrY, 1.2f);

        float centerIdx = ((bandX - r.getX()) / r.getWidth()) * 100.f;
        float sigma = std::max(3.5f, (bandW / r.getWidth()) * 42.f);
        float depthFrac = (range / 18.f) * mix * (.4f + .6f * (-thresh / 60.f));

        juce::Path notchCurve;
        for (int i = 0; i < 100; ++i) {
            float gauss = std::exp(-std::pow((float(i) - centerIdx) / sigma, 2.f));
            float hh = r.getHeight() * (.14f + .14f * std::abs(std::sin(float(i) * .14f)) +
                                        .52f * gauss * (1.f - .65f * depthFrac));
            float xx = r.getX() + r.getWidth() * float(i) / 100.f;
            bool inBand = std::abs(xx - bandX) <= bandW * .55f;
            g.setColour(inBand ? look.accentColour : midCol);
            g.drawLine(xx, r.getBottom(), xx, r.getBottom() - hh, 3.2f);

            float ny = r.getY() + 34.f + gauss * depthFrac * (r.getHeight() * .55f) -
                       (output / 24.f) * 14.f;
            if (i == 0)
                notchCurve.startNewSubPath(xx, ny);
            else
                notchCurve.lineTo(xx, ny);
        }
        g.setColour(juce::Colours::white.withAlpha(.85f));
        g.strokePath(notchCurve, juce::PathStrokeType(2.2f));

        label(g, look,
              "FREQ " + juce::String(freq / 1000.f, 2) + " kHz  |  THR " +
                  juce::String(thresh, 0) + " dB  |  RANGE " + juce::String(range, 1) +
                  " dB  |  FOCUS " + juce::String(int(std::round(focus * 100.f))) + "%  |  ATK " +
                  juce::String(attack, 1) + " ms / REL " + juce::String(release, 0) + " ms",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 9. DGate — Live Open/Close Hysteresis Threshold & Gate Trapezoid Envelope
    // -------------------------------------------------------------------------
    if (kind == da::Kind::Gate) {
        float thresh = value("threshold", -45.f); // -80..0 dB
        float attack = value("attack", 2.f); // 0.1..100 ms
        float hold = value("hold", 50.f); // 0..500 ms
        float release = value("release", 150.f); // 5..2000 ms
        float range = value("range", 60.f); // 0..90 dB
        float hyst = value("hysteresis", 6.f); // 0..18 dB
        float output = value("output", 0.f); // -24..12 dB

        float openNorm = std::clamp((thresh + 80.f) / 80.f, .05f, .95f);
        float closeNorm = std::clamp((thresh - hyst + 80.f) / 80.f, .02f, openNorm);
        float openY = r.getBottom() - openNorm * (r.getHeight() - 28.f);
        float closeY = r.getBottom() - closeNorm * (r.getHeight() - 28.f);

        g.setColour(look.accentColour.withAlpha(.45f));
        g.drawLine(r.getX(), openY, r.getRight(), openY, 1.4f);
        g.setColour(juce::Colours::white.withAlpha(.32f));
        g.drawLine(r.getX(), closeY, r.getRight(), closeY, 1.1f);

        float atkFrac = .03f + std::clamp(attack / 100.f, 0.f, 1.f) * .18f;
        float holdFrac = .08f + std::clamp(hold / 500.f, 0.f, 1.f) * .28f;
        float relFrac = .06f + std::clamp(std::log10(std::max(5.f, release) / 5.f) / std::log10(400.f), 0.f, 1.f) * .32f;
        float floorFrac = std::clamp(range / 90.f, .05f, 1.f);

        float topY = std::clamp(r.getY() + 28.f - (output / 24.f) * 14.f, r.getY() + 20.f, r.getCentreY());
        float closedY = topY + floorFrac * (r.getBottom() - 8.f - topY);

        float xStart = r.getX() + r.getWidth() * .10f;
        float xPeak = xStart + r.getWidth() * atkFrac;
        float xHold = xPeak + r.getWidth() * holdFrac;
        float xRel = std::min(r.getRight() - 12.f, xHold + r.getWidth() * relFrac);

        juce::Path env, fill;
        env.startNewSubPath(r.getX(), closedY);
        env.lineTo(xStart, closedY);
        env.lineTo(xPeak, topY);
        env.lineTo(xHold, topY);
        env.lineTo(xRel, closedY);
        env.lineTo(r.getRight(), closedY);

        fill = env;
        fill.lineTo(r.getRight(), r.getBottom());
        fill.lineTo(r.getX(), r.getBottom());
        fill.closeSubPath();
        g.setColour(look.accentColour.withAlpha(.18f));
        g.fillPath(fill);
        g.setColour(look.accentColour);
        g.strokePath(env, juce::PathStrokeType(2.8f));

        label(g, look,
              "OPEN " + juce::String(thresh, 0) + " dB  |  CLOSE " +
                  juce::String(thresh - hyst, 0) + " dB  |  ATK " +
                  juce::String(attack, attack < 10 ? 1 : 0) + " ms  |  HOLD " +
                  juce::String(hold, 0) + " ms  |  REL " + juce::String(release, 0) +
                  " ms  |  RANGE -" + juce::String(range, 0) + " dB",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    // -------------------------------------------------------------------------
    // 10. DFX — 4 Live Stage Cards (Drive, Filter, Delay, Space) + Master Mix
    // -------------------------------------------------------------------------
    if (kind == da::Kind::FX) {
        const char *mods[4]{"DRIVE", "FILTER", "DELAY", "SPACE"};
        const char *enables[4]{"driveOn", "filterOn", "delayOn", "reverbOn"};
        float drive = value("drive", 3.f); // 0..24 dB
        float filterHz = value("filter", 12000.f); // 200..18000 Hz
        float timeMs = value("time", 250.f); // 1..2000 ms
        float fb = value("feedback", .3f); // 0..0.9
        float dlyMix = value("delayMix", .2f); // 0..1
        float decay = value("decay", 1.5f); // 0.1..8 s
        float revMix = value("reverbMix", .2f); // 0..1
        float masterMix = value("mix", .5f); // 0..1
        float output = value("output", 0.f); // -24..12 dB

        for (int j = 0; j < 4; ++j) {
            float ww = r.getWidth() * .225f;
            float xx = r.getX() + float(j) * r.getWidth() * .258f;
            bool on = value(enables[j], 1.f) > .5f;
            auto box = juce::Rectangle<float>(xx, r.getY() + 22.f, ww, r.getHeight() - 26.f);
            g.setColour(darkFill.withAlpha(on ? .95f : .35f));
            g.fillRoundedRectangle(box, 6.f);
            g.setColour(on ? look.accentColour.withAlpha(.75f) : gridCol);
            g.drawRoundedRectangle(box, 6.f, on ? 1.4f : 1.f);

            label(g, look, juce::String(mods[j]) + (on ? " ON" : " OFF"),
                  {box.getX() + 8.f, box.getY() + 4.f, box.getWidth() - 16.f, 18.f}, 15.f,
                  on ? look.accentColour : muted, juce::Justification::centredLeft, true);

            auto sub = box.reduced(10.f, 26.f).withTrimmedBottom(6.f);
            juce::Path p;
            if (j == 0) {
                float k = 1.f + drive * .35f;
                for (int i = 0; i <= 40; ++i) {
                    float u = float(i) / 40.f;
                    float xIn = u * 2.f - 1.f;
                    float yOut = std::tanh(xIn * k);
                    float px = sub.getX() + u * sub.getWidth();
                    float py = sub.getCentreY() - yOut * sub.getHeight() * .45f;
                    if (i == 0)
                        p.startNewSubPath(px, py);
                    else
                        p.lineTo(px, py);
                }
            } else if (j == 1) {
                float fcN = std::clamp(std::log10(std::max(200.f, filterHz) / 200.f) / std::log10(90.f), .08f, .95f);
                for (int i = 0; i <= 40; ++i) {
                    float u = float(i) / 40.f;
                    float lp = 1.f / (1.f + std::exp((u - fcN) * 14.f));
                    float px = sub.getX() + u * sub.getWidth();
                    float py = sub.getBottom() - lp * sub.getHeight() * .85f;
                    if (i == 0)
                        p.startNewSubPath(px, py);
                    else
                        p.lineTo(px, py);
                }
            } else if (j == 2) {
                float sp = .14f + (timeMs / 2000.f) * .14f;
                for (int t = 0; t < 5; ++t) {
                    float tx = sub.getX() + float(t) * sp * sub.getWidth();
                    if (tx > sub.getRight())
                        break;
                    float th = sub.getHeight() * (.35f + .65f * dlyMix) * std::pow(fb + .25f, float(t) * .6f);
                    th = std::min(sub.getHeight(), th);
                    g.setColour(on ? look.accentColour : muted);
                    g.drawLine(tx, sub.getBottom(), tx, sub.getBottom() - th, 3.f);
                }
            } else {
                float decRate = 1.5f + (1.f - std::clamp(decay / 8.f, .05f, 1.f)) * 5.f;
                for (int i = 0; i <= 40; ++i) {
                    float u = float(i) / 40.f;
                    float env = std::exp(-u * decRate) * (.35f + .65f * revMix);
                    float px = sub.getX() + u * sub.getWidth();
                    float py = sub.getBottom() - env * sub.getHeight() * .9f;
                    if (i == 0)
                        p.startNewSubPath(px, py);
                    else
                        p.lineTo(px, py);
                }
            }
            if (!p.isEmpty()) {
                g.setColour(on ? look.accentColour : muted);
                g.strokePath(p, juce::PathStrokeType(2.2f));
            }
            if (j < 3) {
                g.setColour(midCol);
                g.drawLine(box.getRight(), box.getCentreY(),
                           r.getX() + float(j + 1) * r.getWidth() * .258f, box.getCentreY(), 1.5f);
            }
        }
        label(g, look,
              "MASTER MIX " + juce::String(int(std::round(masterMix * 100.f))) + "%  |  OUT " +
                  (output >= 0 ? "+" : "") + juce::String(output, 1) + " dB  |  DRIVE +" +
                  juce::String(drive, 1) + " dB  |  LP " + juce::String(filterHz / 1000.f, 1) +
                  " kHz  |  TIME " + juce::String(timeMs, 0) + " ms  |  DECAY " +
                  juce::String(decay, 2) + " s",
              {r.getX() + 6.f, r.getY() + 2.f, r.getWidth() - 12.f, 20.f}, 15.f,
              juce::Colour(0xffeaeaf2), juce::Justification::centredLeft, true);
        g.restoreState();
        return;
    }

    g.restoreState();
}
void AnalysisDisplay::paintDetail(juce::Graphics &g, juce::Rectangle<float> r) {
    label(g, look, juce::String(inDb, 1) + " dBFS  /  " + juce::String(outDb, 1) + " dBFS", r,
          16.f, silver, juce::Justification::centredRight);
}
ReferenceDisplay summarizeReference(const juce::AudioBuffer<float> &b, double sr,
                                    const juce::String &name) {
    ReferenceDisplay r;
    r.features = da::analyze(b.getReadPointer(0), b.getReadPointer(b.getNumChannels() > 1 ? 1 : 0),
                             size_t(b.getNumSamples()), sr);
    r.name = name;
    for (size_t i = 0; i < r.waveform.size(); ++i) {
        int start = int(i * size_t(b.getNumSamples()) / r.waveform.size()),
            end = int((i + 1) * size_t(b.getNumSamples()) / r.waveform.size());
        float peak = 0;
        for (int j = start; j < end; ++j) {
            float v = b.getSample(0, j);
            if (std::isfinite(v))
                peak = std::max(peak, std::abs(v));
        }
        r.waveform[i] = peak;
    }
    return r;
}
} // namespace da::ui
