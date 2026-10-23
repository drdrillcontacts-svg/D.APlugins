#include "AnalysisDisplay.h"
#include <complex>
namespace da::ui {
namespace {
void waveform(juce::Graphics &g, const HardwareLook &look, juce::Rectangle<float> r, const float *v,
              size_t n, bool ready) {
    g.setColour(juce::Colour(0xff171c1e));
    g.drawLine(r.getX(), r.getCentreY(), r.getRight(), r.getCentreY());
    if (!ready) {
        label(g, look, "AWAITING AUDIO", r, 12, muted.withAlpha(.6f), juce::Justification::centred);
        return;
    }
    float maximum = .02f;
    for (size_t i = 0; i < n; ++i)
        maximum = std::max(maximum, std::abs(v[i]));
    g.setColour(silver.withAlpha(.55f));
    for (int x = 0; x < int(r.getWidth()); ++x) {
        size_t start = size_t(x) * n / size_t(r.getWidth()),
               end = std::max(start + 1, size_t(x + 1) * n / size_t(r.getWidth()));
        float peak = 0;
        for (size_t j = start; j < std::min(end, n); ++j)
            peak = std::max(peak, std::abs(v[j]));
        float h = peak / maximum * r.getHeight() * .43f;
        g.drawLine(r.getX() + x, r.getCentreY() - h, r.getX() + x, r.getCentreY() + h, .8f);
    }
}
} // namespace
void meter(juce::Graphics &g, const HardwareLook &look, juce::Rectangle<float> r, float db,
           bool scale) {
    int segments = 40;
    float step = r.getHeight() / segments;
    for (int i = 0; i < segments; ++i) {
        float threshold = -72.f + float(i) * 72.f / segments;
        bool on = db >= threshold;
        g.setColour(on ? (threshold > -6    ? crimson
                          : threshold > -30 ? juce::Colour(0xffc21a34)
                                            : silver.withAlpha(.65f))
                       : juce::Colour(0xff202628));
        g.fillRect(r.getX(), r.getBottom() - (i + 1) * step, r.getWidth(), step * .58f);
    }
    if (scale)
        for (int i = 0; i <= 6; ++i) {
            float d = float(i) * -12;
            label(g, look, juce::String(int(d)),
                  {r.getRight() + 6, r.getY() + r.getHeight() * float(i) / 6 - 7, 30, 14}, 11,
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
    label(g, look, "IN", {6, 4, 42, 20}, 12, silver);
    meter(g, look, {10, 38, 7, 151}, inDb);
    meter(g, look, {21, 38, 7, 151}, inDb);
    label(g, look, state.toUpperCase(), {60, 4, 550, 22}, 12, crimson);
    graph(g, {60, 35, 570, 155});
    g.setColour(juce::Colour(0xff303638)); g.drawLine(647, 8, 647, 209);
    label(g, look, "REFERENCE", {661, 5, 140, 21}, 13, silver);
    waveform(g, look, {661, 40, 140, 146}, reference.waveform.data(), reference.waveform.size(),
             reference.features.valid());
    label(g, look, reference.features.valid() ? reference.name : "NOT LOADED",
          {661, 198, 140, 19}, 11, muted);
    meter(g, look, {827, 38, 7, 151}, outDb);
    meter(g, look, {839, 38, 7, 151}, outDb);
    label(g, look, "OUT", {817, 4, 42, 20}, 12, silver);
    const char *captions[]{
        "SPECTRAL HISTORY", "ECHO / FREQUENCY FIELD", "DYNAMICS TRANSFER", "FOUR-BAND RESPONSE",
        "HARMONIC TRANSFER", "TRANSIENT / SUSTAIN", "STEREO VECTORSCOPE", "SIBILANCE DETECTION",
        "GATE TRANSFER", "CHAIN / FREQUENCY FIELD"};
    label(g, look, captions[int(kind)], {60, 198, 570, 19}, 11, muted,
          juce::Justification::centredRight);
}
void AnalysisDisplay::graph(juce::Graphics &g, juce::Rectangle<float> r) {
    g.saveState();
    g.reduceClipRegion(r.toNearestInt());
    g.setColour(juce::Colour(0xff22292b));
    for (int i = 0; i < 8; ++i)
        g.drawVerticalLine(int(r.getX() + r.getWidth() * i / 7), r.getY(), r.getBottom());
    for (int i = 0; i < 5; ++i)
        g.drawHorizontalLine(int(r.getY() + r.getHeight() * i / 4), r.getX(), r.getRight());
    juce::Path path;
    if (kind == da::Kind::Stereo) {
        auto c = r.getCentre();
        g.setColour(muted.withAlpha(.4f));
        g.drawEllipse(r.withSizeKeepingCentre(130, 130), .6f);
        g.drawLine(c.x - 70, c.y, c.x + 70, c.y);
        g.drawLine(c.x, c.y - 65, c.x, c.y + 65);
        g.setColour(crimson.withAlpha(.6f));
        for (auto &f : phase) {
            float mid = (f.left + f.right) * .5f, side = (f.left - f.right) * .5f;
            g.fillEllipse(c.x + side * 140 - 1, c.y - mid * 140 - 1, 2, 2);
        }
        g.restoreState();
        return;
    }
    if (kind == da::Kind::Trans) {
        float max = .001f;
        for (float x : input)
            max = std::max(max, std::abs(x));
        for (size_t i = 0; i < 256; ++i) {
            float x = r.getX() + r.getWidth() * float(i) / 255,
                  y = r.getBottom() -
                      std::abs(input[(position + i * 8) % 2048]) / max * r.getHeight() * .9f;
            if (i == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);
        }
    } else if (kind == da::Kind::Comp || kind == da::Kind::Gate || kind == da::Kind::Sat) {
        for (int i = 0; i < 256; ++i) {
            float x = float(i) / 255, normalized = x;
            if (kind == da::Kind::Comp) {
                float inputDb = -60 + x * 60, over = inputDb - value("threshold"),
                      ratio = value("ratio", 4);
                normalized =
                    (inputDb - std::max(0.f, over) * (1 - 1 / ratio) + value("makeup") + 60) / 60;
            } else if (kind == da::Kind::Gate) {
                float inputDb = -80 + x * 80;
                normalized = x + (inputDb < value("threshold") ? -value("range") / 80 : 0);
            } else {
                float driven = (x * 2 - 1) * da::gain(value("drive"));
                int mode = int(value("mode"));
                float y = mode == 0   ? std::tanh(driven)
                          : mode == 1 ? std::clamp(driven, -1.f, 1.f)
                                      : std::tanh(driven + .3f) - std::tanh(.3f);
                normalized = y * .45f + .5f;
            }
            float px = r.getX() + x * r.getWidth(), py = r.getBottom() - normalized * r.getHeight();
            if (i == 0)
                path.startNewSubPath(px, py);
            else
                path.lineTo(px, py);
        }
    } else if (kind == da::Kind::EQ) {
        // Actual RBJ frequency response for current parameters, including shelf shapes.
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
                       float(20 * std::log10(std::max(response, 1e-12))) * r.getHeight() / 48;
            if (i == 0)
                path.startNewSubPath(px, py);
            else
                path.lineTo(px, py);
        }
    } else {
        if (kind == da::Kind::Sess) {
            float x = r.getX() +
                      float(std::log(std::max(20.f, value("frequency")) / 20) / std::log(1000.)) *
                          r.getWidth();
            g.setColour(crimson.withAlpha(.09f));
            g.fillRect(x - 22, r.getY(), 44.f, r.getHeight());
        }
        if (kind == da::Kind::Verb)
            for (size_t h = 0; h < history.size(); ++h) {
                const auto &values = history[(historyPosition + h) % history.size()];
                juce::Path trace;
                for (size_t j = 0; j < 128; ++j) {
                    float x = r.getX() + r.getWidth() * float(j) / 127,
                          y = r.getBottom() - (values[j] + 90) / 96 * r.getHeight() +
                              float(history.size() - h) * 1.8f;
                    if (j == 0)
                        trace.startNewSubPath(x, y);
                    else
                        trace.lineTo(x, y);
                }
                g.setColour(crimson.withAlpha(.035f + float(h) * .017f));
                g.strokePath(trace, juce::PathStrokeType(.7f));
            }
        for (size_t i = 0; i < spectrum.size(); ++i) {
            float x = r.getX() + r.getWidth() * float(i) / 127,
                  y = r.getBottom() - (spectrum[i] + 90) / 96 * r.getHeight();
            if (i == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);
        }
        juce::Path wet;
        for (size_t i = 0; i < outSpectrum.size(); ++i) {
            float x = r.getX() + r.getWidth() * float(i) / 127,
                  y = r.getBottom() - (outSpectrum[i] + 90) / 96 * r.getHeight();
            if (i == 0)
                wet.startNewSubPath(x, y);
            else
                wet.lineTo(x, y);
        }
        g.setColour(silver.withAlpha(.3f));
        g.strokePath(wet, juce::PathStrokeType(.7f));
    }
    auto fill = path;
    fill.lineTo(r.getRight(), r.getBottom());
    fill.lineTo(r.getX(), r.getBottom());
    fill.closeSubPath();
    g.setGradientFill(juce::ColourGradient(crimson.withAlpha(.17f), r.getCentreX(), r.getY(),
                                           crimson.withAlpha(.005f), r.getCentreX(), r.getBottom(),
                                           false));
    g.fillPath(fill);
    g.setColour(crimson.withAlpha(.12f));
    g.strokePath(path, juce::PathStrokeType(6));
    g.setColour(crimson);
    g.strokePath(path, juce::PathStrokeType(1.3f));
    g.restoreState();
}
void AnalysisDisplay::paintDetail(juce::Graphics &g, juce::Rectangle<float> r) {
    if (kind == da::Kind::Comp || kind == da::Kind::Sess || kind == da::Kind::Gate) {
        label(g, look, "GAIN REDUCTION", r.removeFromTop(24), 13, muted,
              juce::Justification::centred);
        label(g, look, juce::String(gr, 1) + " dB", r.removeFromTop(53), 38, crimson,
              juce::Justification::centred);
    } else if (kind == da::Kind::Stereo) {
        double l = 0, rr = 0, lr = 0;
        for (auto &f : phase) {
            l += f.left * f.left;
            rr += f.right * f.right;
            lr += f.left * f.right;
        }
        double correlation = l * rr > 1e-12 ? lr / std::sqrt(l * rr) : 1;
        label(g, look, "CHANNEL CORRELATION", r.removeFromTop(28), 13, muted,
              juce::Justification::centred);
        label(g, look, juce::String(correlation, 2), r.removeFromTop(55), 36,
              correlation < 0 ? crimson : silver, juce::Justification::centred);
        label(g, look, "-1                  0                  +1", r.removeFromTop(28), 12, muted,
              juce::Justification::centred);
    } else
        label(g, look, "INPUT / OUTPUT", r.removeFromTop(26), 13, muted,
              juce::Justification::centred);
    label(g, look, juce::String(inDb, 1) + " dBFS   /   " + juce::String(outDb, 1) + " dBFS",
          r.removeFromTop(30), 18, silver, juce::Justification::centred);
    if (kind == da::Kind::Trans)
        label(g, look, "LIVE ENVELOPE ABOVE  /  MANUAL CONTROL ALWAYS", r.removeFromTop(26), 12,
              muted, juce::Justification::centred);
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
