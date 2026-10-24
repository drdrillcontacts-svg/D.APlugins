#include "AnalysisDisplay.h"
#include <complex>
namespace da::ui {
namespace {
void waveform(juce::Graphics &g, const HardwareLook &look, juce::Rectangle<float> r, const float *v,
              size_t n, bool ready) {
    if (!ready) {
        label(g, look, "No reference file selected", {r.getX(), r.getY() + 36.f, r.getWidth(), 28.f},
              18.f, juce::Colour(0xffd0d0d6), juce::Justification::centred, true);
        label(g, look, "Load a file when you need reference-based analysis.",
              {r.getX(), r.getY() + 72.f, r.getWidth(), 28.f}, 16.f, juce::Colour(0xffa9a9b5),
              juce::Justification::centred, false);
        return;
    }
    g.setColour(juce::Colour(0xff25252c));
    g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY());
    float maximum = .02f;
    for (size_t i = 0; i < n; ++i)
        maximum = std::max(maximum, std::abs(v[i]));
    g.setColour(look.accentColour.withAlpha(.85f));
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
    int segments = 48;
    float step = r.getHeight() / float(segments);
    auto hiCol = look.accentColour.interpolatedWith(juce::Colours::white, .25f);
    for (int j = 0; j < segments; ++j) {
        float threshold = -6.f - float(j) * 66.f / float(segments);
        bool on = db >= threshold;
        g.setColour(on ? (j < 8    ? look.accentColour
                          : j < 18 ? hiCol
                                   : juce::Colour(0xffd5d5d7))
                       : juce::Colour(0xff29292e));
        g.fillRect(r.getX(), r.getY() + float(j) * step, r.getWidth(), step * .54f);
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
    for (size_t i = 0; i < metadata.size(); ++i)
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
    label(g, look, "IN", {4.f, 38.f, 42.f, 22.f}, 16.f, look.accentColour,
          juce::Justification::centred, true);
    meter(g, look, {14.f, 70.f, 9.f, 206.f}, inDb);
    meter(g, look, {27.f, 70.f, 9.f, 206.f}, inDb);

    label(g, look, "INPUT / " + juce::String(roles[int(kind)]) + " / " + state.toUpperCase(),
          {81.f, 22.f, 880.f, 24.f}, 16.f, look.accentColour, juce::Justification::centredLeft,
          true);

    graph(g, {93.f, 77.f, 900.f, 209.f});

    label(g, look, "OUT", {1017.f, 38.f, 42.f, 22.f}, 16.f, look.accentColour,
          juce::Justification::centred, true);
    meter(g, look, {1027.f, 70.f, 9.f, 206.f}, outDb);
    meter(g, look, {1040.f, 70.f, 9.f, 206.f}, outDb);
}
void AnalysisDisplay::paintReferencePanel(juce::Graphics &g, juce::Rectangle<float> box) const {
    g.setColour(juce::Colour(0xff111114));
    g.fillRoundedRectangle(box, 18.f);
    g.setColour(juce::Colour(0xff393942));
    g.drawRoundedRectangle(box.reduced(.5f), 18.f, 1.2f);
    label(g, look, "REFERENCE", {box.getX() + 32.f, box.getY() + 22.f, 300.f, 32.f}, 24.f,
          look.accentColour, juce::Justification::centredLeft, true);
    label(g, look, "Optional workspace — opened only when requested",
          {box.getX() + 32.f, box.getY() + 74.f, 680.f, 26.f}, 18.f, juce::Colour(0xffd0d0d6),
          juce::Justification::centredLeft, false);
    auto waveBox = juce::Rectangle<float>(box.getX() + 32.f, box.getY() + 128.f, 736.f, 160.f);
    g.setColour(juce::Colour(0xff0e0e11));
    g.fillRoundedRectangle(waveBox, 10.f);
    g.setColour(juce::Colour(0xff414146));
    g.drawRoundedRectangle(waveBox, 10.f, 1.f);
    waveform(g, look, waveBox.reduced(16.f, 12.f), reference.waveform.data(),
             reference.waveform.size(), reference.features.valid());
    juce::String foot = reference.features.valid()
                            ? ("Loaded: " + reference.name + "  /  RMS " +
                               juce::String(da::db(float(reference.features.rms)), 1) + " dBFS")
                            : "Reference guidance is optional — no file loaded by default.";
    label(g, look, foot, {box.getX() + 32.f, box.getY() + 382.f, 736.f, 26.f}, 16.f,
          juce::Colour(0xffb8b8c4), juce::Justification::centredLeft, false);
}
void AnalysisDisplay::graph(juce::Graphics &g, juce::Rectangle<float> r) {
    g.saveState();
    g.reduceClipRegion(r.toNearestInt());
    auto gridCol = juce::Colour(0xff24242a).interpolatedWith(look.accentColour, .22f);
    auto midCol = juce::Colour(0xff222228).interpolatedWith(look.accentColour, .42f);
    auto darkFill = juce::Colour(0xff121216).interpolatedWith(look.accentColour, .15f);

    if (kind == da::Kind::Delay) {
        g.setColour(gridCol);
        float baseY = r.getY() + r.getHeight() * .78f;
        g.drawLine(r.getX(), baseY, r.getRight(), baseY, 1.f);
        float fb = std::clamp(value("feedback", .35f), .05f, .95f);
        for (int j = 0; j < 8; ++j) {
            float xx = r.getX() + r.getWidth() * (.06f + float(j) * .128f);
            float amp = r.getHeight() * .66f * std::pow(fb + .35f, float(j) * .55f);
            amp = std::min(amp, r.getHeight() * .72f);
            g.setColour(look.accentColour);
            g.drawLine(xx, baseY, xx, baseY - amp, 3.f);
            g.fillEllipse(xx - 3.f, baseY - amp - 3.f, 6.f, 6.f);
            label(g, look, juce::String(j + 1), {xx - 16.f, r.getY() + r.getHeight() * .82f, 32.f, 22.f},
                  16.f, midCol, juce::Justification::centred, true);
        }
        g.restoreState();
        return;
    }
    if (kind == da::Kind::FX) {
        const char *mods[4]{"DRIVE", "FILTER", "DELAY", "SPACE"};
        const char *enables[4]{"driveOn", "filterOn", "delayOn", "reverbOn"};
        for (int j = 0; j < 4; ++j) {
            float ww = r.getWidth() * .21f;
            float xx = r.getX() + float(j) * r.getWidth() * .25f;
            bool on = value(enables[j], 1.f) > .5f;
            auto box = juce::Rectangle<float>(xx, r.getY(), ww, r.getHeight());
            g.setColour(darkFill.withAlpha(on ? 1.f : .45f));
            g.fillRoundedRectangle(box, 5.f);
            g.setColour(gridCol);
            g.drawRoundedRectangle(box, 5.f, 1.f);
            label(g, look, mods[j], box, 17.f, on ? look.accentColour : muted,
                  juce::Justification::centred, true);
            if (j < 3) {
                g.setColour(midCol);
                g.drawLine(box.getRight(), box.getCentreY(),
                           r.getX() + float(j + 1) * r.getWidth() * .25f, box.getCentreY(), 1.f);
            }
        }
        g.restoreState();
        return;
    }
    if (kind == da::Kind::Stereo) {
        auto c = r.getCentre();
        float rad = std::min(r.getWidth(), r.getHeight()) * .46f;
        g.setColour(gridCol);
        g.drawEllipse(c.x - rad * .5f, c.y - rad * .5f, rad, rad, 1.f);
        g.drawEllipse(c.x - rad, c.y - rad, rad * 2.f, rad * 2.f, 1.f);
        g.drawLine(c.x - rad, c.y, c.x + rad, c.y, 1.f);
        g.drawLine(c.x, c.y - rad, c.x, c.y + rad, 1.f);
        float wScale = std::clamp(value("width", 1.f), .1f, 2.f);
        for (int j = 0; j < 9; ++j) {
            juce::Path liss;
            for (int i = 0; i <= 100; ++i) {
                float t = float(i) / 100.f * juce::MathConstants<float>::twoPi;
                float px = c.x + rad * .65f * wScale * .65f * std::sin(t);
                float py = c.y + rad * .6f * std::sin(t * 2.f + float(j) * .14f);
                if (i == 0)
                    liss.startNewSubPath(px, py);
                else
                    liss.lineTo(px, py);
            }
            g.setColour(look.accentColour.withAlpha(.24f));
            g.strokePath(liss, juce::PathStrokeType(1.f));
        }
        g.restoreState();
        return;
    }
    if (kind == da::Kind::Sess) {
        float freqNorm = float(std::log(std::max(2000.f, value("frequency", 6000.f)) / 2000.f) /
                               std::log(12000.f / 2000.f));
        float bandX = r.getX() + r.getWidth() * (.45f + .38f * std::clamp(freqNorm, 0.f, 1.f));
        g.setColour(darkFill);
        g.fillRoundedRectangle(bandX - r.getWidth() * .08f, r.getY(), r.getWidth() * .16f,
                               r.getHeight(), 3.f);
        for (int i = 0; i < 100; ++i) {
            float hh = r.getHeight() *
                       (.12f + .16f * std::abs(std::sin(float(i) * .12f)) +
                        .56f * std::exp(-std::pow((float(i) - 76.f) / 8.f, 2.f)));
            float xx = r.getX() + r.getWidth() * float(i) / 100.f;
            g.setColour(i > 68 ? look.accentColour : midCol);
            g.drawLine(xx, r.getBottom(), xx, r.getBottom() - hh, 3.2f);
        }
        g.restoreState();
        return;
    }
    if (kind == da::Kind::Verb || kind == da::Kind::Trans || kind == da::Kind::Gate) {
        int count = kind == da::Kind::Verb ? 135 : 131;
        for (int i = 0; i < count; ++i) {
            float t = float(i) / float(count - 1);
            float a = kind == da::Kind::Verb
                          ? r.getHeight() * .4f * (1.f - std::exp(-t * 35.f)) *
                                std::exp(-t * 3.5f) * (.3f + .7f * std::abs(std::sin(float(i) * 1.82f)))
                          : r.getHeight() * .39f * (std::exp(-t * 7.f) + .22f * std::exp(-t)) *
                                std::abs(std::sin(float(i) * 1.6f));
            float xx = r.getX() + t * r.getWidth();
            g.setColour(look.accentColour);
            g.drawLine(xx, r.getCentreY() - a, xx, r.getCentreY() + a, 1.5f);
        }
        if (kind == da::Kind::Gate || kind == da::Kind::Trans) {
            juce::Path env;
            if (kind == da::Kind::Gate) {
                env.startNewSubPath(r.getX(), r.getY() + r.getHeight() * .87f);
                env.lineTo(r.getX() + r.getWidth() * .12f, r.getY() + r.getHeight() * .87f);
                env.lineTo(r.getX() + r.getWidth() * .12f, r.getY() + r.getHeight() * .12f);
                env.lineTo(r.getX() + r.getWidth() * .49f, r.getY() + r.getHeight() * .12f);
                env.lineTo(r.getX() + r.getWidth() * .68f, r.getY() + r.getHeight() * .87f);
                env.lineTo(r.getRight(), r.getY() + r.getHeight() * .87f);
            } else {
                env.startNewSubPath(r.getX(), r.getY() + r.getHeight() * .8f);
                env.quadraticTo(r.getX() + r.getWidth() * .06f, r.getY() - r.getHeight() * .2f,
                                r.getX() + r.getWidth() * .16f, r.getY() + r.getHeight() * .2f);
                env.quadraticTo(r.getX() + r.getWidth() * .4f, r.getY() + r.getHeight() * .7f,
                                r.getRight(), r.getY() + r.getHeight() * .8f);
            }
            g.setColour(look.accentColour.withAlpha(.65f));
            g.strokePath(env, juce::PathStrokeType(1.5f));
        }
        g.restoreState();
        return;
    }

    juce::Path path;
    if (kind == da::Kind::Comp || kind == da::Kind::Sat) {
        g.setColour(gridCol);
        g.drawLine(r.getX(), r.getBottom(), r.getRight(), r.getY(), 1.f);
        g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 1.f);
        for (int i = 0; i <= 100; ++i) {
            float t = float(i) / 100.f;
            float z = t < .52f ? t : .52f + (t - .52f) * .25f;
            if (kind == da::Kind::Sat)
                z = .5f + .43f * std::tanh((t - .5f) * 5.f);
            float px = r.getX() + t * r.getWidth(), py = r.getY() + r.getHeight() * (1.f - z);
            if (i == 0)
                path.startNewSubPath(px, py);
            else
                path.lineTo(px, py);
        }
        g.setColour(look.accentColour);
        g.strokePath(path, juce::PathStrokeType(2.5f));
    } else if (kind == da::Kind::EQ) {
        g.setColour(gridCol);
        g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY(), 1.f);
        for (int i = 0; i < 256; ++i) {
            double f = 20 * std::pow(1000., double(i) / 255),
                   w = 2 * da::pi * std::min(f, rate * .49) / rate;
            std::complex<double> z = std::polar(1., -w), z2 = z * z;
            double response = 1;
            for (int b = 0; b < 4; ++b) {
                size_t p = size_t(b) * 4;
                double fc = std::min(double(parameters[p]), rate * .45),
                       A = std::pow(10., parameters[p + 1] / 40.),
                       c = std::cos(2 * da::pi * fc / rate), s = std::sin(2 * da::pi * fc / rate),
                       alpha = s / (2 * parameters[p + 2]);
                double b0 = 1 + alpha * A, b1 = -2 * c, b2 = 1 - alpha * A, a0 = 1 + alpha / A,
                       a1 = -2 * c, a2 = 1 - alpha / A;
                int shape = int(parameters[p + 3]);
                double t = 2 * std::sqrt(A) * s / std::sqrt(2.);
                if (shape == 1) {
                    b0 = A * ((A + 1) - (A - 1) * c + t);
                    b1 = 2 * A * ((A - 1) - (A + 1) * c);
                    b2 = A * ((A + 1) - (A - 1) * c - t);
                    a0 = (A + 1) + (A - 1) * c + t;
                    a1 = -2 * ((A - 1) + (A + 1) * c);
                    a2 = (A + 1) + (A - 1) * c - t;
                }
                if (shape == 2) {
                    b0 = A * ((A + 1) + (A - 1) * c + t);
                    b1 = -2 * A * ((A - 1) + (A + 1) * c);
                    b2 = A * ((A + 1) + (A - 1) * c - t);
                    a0 = (A + 1) - (A - 1) * c + t;
                    a1 = 2 * ((A - 1) - (A + 1) * c);
                    a2 = (A + 1) - (A - 1) * c - t;
                }
                response *= std::abs((b0 + b1 * z + b2 * z2) / (a0 + a1 * z + a2 * z2));
            }
            float px = r.getX() + r.getWidth() * float(i) / 255,
                  py = r.getCentreY() -
                       float(20 * std::log10(std::max(response, 1e-12))) * r.getHeight() / 36;
            if (i == 0)
                path.startNewSubPath(px, py);
            else
                path.lineTo(px, py);
        }
        g.setColour(look.accentColour);
        g.strokePath(path, juce::PathStrokeType(2.2f));
        for (float t : {.12f, .33f, .57f, .82f})
            g.fillEllipse(r.getX() + t * r.getWidth() - 3.f, r.getCentreY() - 3.f, 6.f, 6.f);
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
