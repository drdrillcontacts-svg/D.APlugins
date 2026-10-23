#include "Suite.h"
#include <cctype>
#include <stdexcept>
namespace da {
float db(float x) { return 20.f * std::log10(std::max(x, 1e-9f)); }
float gain(float x) { return std::pow(10.f, x / 20.f); }
Kind kindForName(const std::string &n) {
    const std::array<std::string, 10> names{"DVerb",  "DDelay",  "DComp", "DEQ",   "DSat",
                                            "DTrans", "DStereo", "DSess", "DGate", "DFX"};
    auto it = std::find(names.begin(), names.end(), n);
    if (it == names.end())
        throw std::invalid_argument("Unknown plugin");
    return static_cast<Kind>(it - names.begin());
}
std::vector<Parameter> parameters(Kind k) {
    std::vector<Parameter> p;
    auto add = [&](const char *id, const char *name, const char *module, const char *unit, float lo,
                   float hi, float initial, bool discrete = false) {
        p.push_back({id, name, module, unit, lo, hi, initial, discrete});
    };
    if (k == Kind::Verb) {
        add("predelay", "Pre-delay", "Space", "ms", 0, 200, 20);
        add("size", "Size", "Space", "%", 0, 1, .6f);
        add("decay", "Decay", "Space", "s", .1f, 12, 2);
        add("tone", "Tone", "Colour", "Hz", 500, 18000, 6000);
        add("mix", "Mix", "Space", "", 0, 1, .25f);
        add("width", "Width", "Space", "", 0, 2, 1);
        add("density", "Density", "Space", "", 0, 1, .7f);
        add("diffusion", "Diffusion", "Space", "", 0, 1, .7f);
        add("modulation", "Modulation", "Space", "", 0, 1, .1f);
        add("lowcut", "Low cut", "Colour", "Hz", 20, 1000, 120);
        add("midcut", "Mid cut", "Colour", "dB", -12, 0, -2);
        add("mode", "Room/Hall/Plate/Shimmer", "Space", "", 0, 3, 1, true);
    } else if (k == Kind::Delay) {
        add("time", "Time", "Echo", "ms", 1, 2000, 375);
        add("feedback", "Feedback", "Echo", "", 0, .95f, .35f);
        add("filter", "Filter", "Echo", "Hz", 200, 18000, 6000);
        add("pingpong", "Ping-pong", "Echo", "", 0, 1, 1, true);
        add("modulation", "Modulation", "Echo", "", 0, 1, .1f);
        add("mix", "Mix", "Echo", "", 0, 1, .25f);
        add("width", "Width", "Echo", "", 0, 2, 1);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else if (k == Kind::Comp) {
        add("threshold", "Threshold", "Dynamics", "dB", -60, 0, -18);
        add("ratio", "Ratio", "Dynamics", ":1", 1, 20, 4);
        add("attack", "Attack", "Dynamics", "ms", .1f, 200, 15);
        add("release", "Release", "Dynamics", "ms", 10, 2000, 150);
        add("knee", "Knee", "Dynamics", "dB", 0, 24, 6);
        add("makeup", "Makeup", "Dynamics", "dB", 0, 24, 0);
        add("mix", "Mix", "Dynamics", "", 0, 1, 1);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else if (k == Kind::EQ) {
        for (int b = 0; b < 4; ++b) {
            std::string prefix = "b" + std::to_string(b + 1),
                        module = "Band " + std::to_string(b + 1);
            float frequencies[]{100, 400, 2500, 10000};
            p.push_back({prefix + "freq", "Frequency", module, "Hz", 20, 20000, frequencies[b]});
            p.push_back({prefix + "gain", "Gain", module, "dB", -18, 18, 0});
            p.push_back({prefix + "q", "Q", module, "", .2f, 8, .707f});
            p.push_back({prefix + "shape", "Bell/Low shelf/High shelf", module, "", 0, 2, 0, true});
        }
    } else if (k == Kind::Sat) {
        add("drive", "Drive", "Colour", "dB", 0, 30, 6);
        add("tone", "Tone", "Colour", "Hz", 500, 18000, 10000);
        add("mode", "Soft/Hard/Asymmetric", "Colour", "", 0, 2, 0, true);
        add("density", "Density", "Colour", "", 0, 1, .5f);
        add("mix", "Mix", "Colour", "", 0, 1, .5f);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else if (k == Kind::Trans) {
        add("attack", "Attack", "Envelope", "dB", -18, 18, 0);
        add("sustain", "Sustain", "Envelope", "dB", -18, 18, 0);
        add("sensitivity", "Sensitivity", "Envelope", "", 0, 1, .5f);
        add("shape", "Shape", "Envelope", "", .2f, 3, 1);
        add("mix", "Mix", "Envelope", "", 0, 1, 1);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else if (k == Kind::Stereo) {
        add("width", "Width", "Image", "", 0, 2, 1);
        add("depth", "Depth", "Image", "", 0, 1, 0);
        add("center", "Center", "Image", "dB", -18, 6, 0);
        add("side", "Side", "Image", "dB", -18, 6, 0);
        add("mix", "Mix", "Image", "", 0, 1, 1);
        add("mono", "Mono monitor", "Image", "", 0, 1, 0, true);
    } else if (k == Kind::Sess) {
        add("frequency", "Detection frequency", "Sibilance", "Hz", 2000, 12000, 6000);
        add("threshold", "Threshold", "Sibilance", "dB", -60, 0, -24);
        add("range", "Range", "Sibilance", "dB", 0, 18, 6);
        add("attack", "Attack", "Sibilance", "ms", .1f, 30, 1);
        add("release", "Release", "Sibilance", "ms", 10, 500, 80);
        add("focus", "Focus", "Sibilance", "", .2f, 1, .6f);
        add("mix", "Mix", "Sibilance", "", 0, 1, 1);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else if (k == Kind::Gate) {
        add("threshold", "Threshold", "Gate", "dB", -80, 0, -45);
        add("attack", "Attack", "Gate", "ms", .1f, 100, 2);
        add("hold", "Hold", "Gate", "ms", 0, 500, 50);
        add("release", "Release", "Gate", "ms", 5, 2000, 150);
        add("range", "Range", "Gate", "dB", 0, 90, 60);
        add("hysteresis", "Hysteresis", "Gate", "dB", 0, 18, 6);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    } else {
        add("driveOn", "Enable saturation", "Saturation", "", 0, 1, 1, true);
        add("drive", "Drive", "Saturation", "dB", 0, 24, 3);
        add("filterOn", "Enable filter", "Filter", "", 0, 1, 1, true);
        add("filter", "Low pass", "Filter", "Hz", 200, 18000, 12000);
        add("delayOn", "Enable delay", "Delay", "", 0, 1, 1, true);
        add("time", "Time", "Delay", "ms", 1, 2000, 250);
        add("feedback", "Feedback", "Delay", "", 0, .9f, .3f);
        add("delayMix", "Delay mix", "Delay", "", 0, 1, .2f);
        add("reverbOn", "Enable ambience", "Ambience", "", 0, 1, 1, true);
        add("decay", "Decay", "Ambience", "s", .1f, 8, 1.5f);
        add("reverbMix", "Ambience mix", "Ambience", "", 0, 1, .2f);
        add("mix", "Mix", "Output", "", 0, 1, .5f);
        add("output", "Output", "Output", "dB", -24, 12, 0);
    }
    return p;
}
Features analyze(const float *l, const float *r, size_t n, double sr) {
    Features f;
    if (!l || n == 0 || !std::isfinite(sr) || sr < 8000)
        return f;
    double sum = 0, ll = 0, rr = 0, lr = 0, side = 0, delta = 0, previous = 0;
    std::array<double, 3> low{};
    double cuts[]{200, 1000, 5000};
    for (size_t i = 0; i < n; ++i) {
        double a = std::isfinite(l[i]) ? l[i] : 0, b = r && std::isfinite(r[i]) ? r[i] : a;
        double x = (a + b) * .5;
        sum += (a * a + b * b) * .5;
        ll += a * a;
        rr += b * b;
        lr += a * b;
        side += (a - b) * (a - b) * .25;
        f.peak = std::max(f.peak, std::max(std::abs(a), std::abs(b)));
        for (size_t j = 0; j < 3; ++j)
            low[j] += (1 - std::exp(-2 * pi * cuts[j] / sr)) * (x - low[j]);
        double bands[]{low[0], low[1] - low[0], low[2] - low[1], x - low[2]};
        for (size_t j = 0; j < 4; ++j)
            f.bands[j] += bands[j] * bands[j];
        delta += std::abs(x - previous);
        previous = x;
    }
    f.samples = n;
    f.duration = double(n) / sr;
    f.rms = std::sqrt(sum / n);
    f.crest = db(float(f.peak / std::max(f.rms, 1e-9)));
    f.correlation = ll * rr > 1e-18 ? lr / std::sqrt(ll * rr) : 1;
    f.sideRatio = std::sqrt(side / std::max(sum, 1e-18));
    f.transient = delta / n / std::max(f.rms, 1e-9);
    for (auto &b : f.bands)
        b = std::sqrt(b / n);
    return f;
}
Objective describe(const std::string &text) {
    std::string s = text;
    for (auto &c : s)
        c = char(std::tolower(static_cast<unsigned char>(c)));
    Objective o;
    o.mode = Mode::Description;
    o.dark = s.find("dark") != std::string::npos;
    o.warm = s.find("warm") != std::string::npos;
    o.wide = s.find("wide") != std::string::npos;
    o.smooth = s.find("smooth") != std::string::npos;
    o.cinematic = s.find("cinematic") != std::string::npos;
    if (!(o.dark || o.warm || o.wide || o.smooth || o.cinematic))
        throw std::invalid_argument(
            "Supported goals: dark, warm, wide, smooth, cinematic; no goal recognized");
    return o;
}
Proposal learn(Kind k, const Features &f, const Features *ref, Objective o,
               const std::vector<float> &current, const std::string &scope, uint64_t revision) {
    if (!f.valid())
        throw std::invalid_argument("Need at least 1024 non-silent input samples");
    if (o.mode == Mode::Reference && (!ref || !ref->valid()))
        throw std::invalid_argument("Reference unavailable or silent");
    auto meta = parameters(k);
    if (current.size() != meta.size())
        throw std::invalid_argument("Parameter state mismatch");
    Proposal result;
    result.revision = revision;
    result.limitation = "Deterministic estimates, not trained inference. Ambience/rhythm cannot be "
                        "uniquely inferred. No iterative output matching yet.";
    if (o.mode == Mode::Reference)
        result.limitation +=
            " Reference currently informs EQ gain, width and compressor makeup only; other "
            "suggestions remain input/objective-based, not reference matches.";
    double level = db(float(f.rms)), crest = f.crest;
    double targetLevel = o.mode == Mode::Reference ? db(float(ref->rms)) : -18;
    float bandCentres[]{100, 400, 2500, 10000};
    for (size_t i = 0; i < meta.size(); ++i) {
        const auto &p = meta[i];
        if (scope != "all" && scope != p.id && scope != p.module)
            continue;
        float v = current[i], confidence = .45f;
        std::string reason =
            "Objective-dependent starting point, not uniquely inferred; audition before applying";
        auto id = p.id;
        if (k == Kind::EQ) {
            size_t band = i / 4;
            double desired =
                o.mode == Mode::Reference ? ref->bands[band] / std::max(ref->rms, 1e-9) : .42;
            if (o.dark && band >= 2)
                desired *= .7;
            if (o.warm && band == 1)
                desired *= 1.2;
            double difference =
                20 * std::log10(std::max(desired, 1e-6) / std::max(f.bands[band] / f.rms, 1e-6));
            if (i % 4 == 0)
                v = bandCentres[band];
            if (i % 4 == 1)
                v = float(std::clamp(difference, -6., 6.));
            if (i % 4 == 2)
                v = .707f;
            if (i % 4 == 3)
                v = 0;
            reason = i % 4 == 1 ? "Broad-band normalized tonal correction, limited to 6 dB"
                                : "Broad tonal objective uses fixed band centres and broad bell; "
                                  "frequency/Q not uniquely identifiable";
            confidence = i % 4 == 1 ? .65f : .2f;
        } else if (id == "threshold") {
            v = float(k == Kind::Gate   ? level - 18
                      : k == Kind::Sess ? db(float(f.bands[3])) - 4
                                        : level + crest * .25);
            reason =
                k == Kind::Gate
                    ? "Conservative threshold below average level; no noise-floor estimator yet"
                : k == Kind::Sess ? "Threshold follows measured high-band RMS"
                                  : "Threshold follows RMS and crest factor";
            confidence = .65f;
        } else if (id == "ratio") {
            v = float(std::clamp(crest / 3, 1.5, 6.));
            reason = "Crest-factor controlled compression objective";
        } else if (id == "attack") {
            v = k == Kind::Trans  ? float(std::clamp(6. - crest * .4, -6., 6.))
                : k == Kind::Sess ? 1.f
                                  : float(crest > 12 ? 20 : 5);
            reason = "Transient preservation from measured crest factor";
        } else if (id == "release") {
            v = float(std::clamp(100. + crest * 5, 50., 400.));
            reason = "Crest-dependent recovery; tempo not estimated";
        } else if (id == "knee")
            v = o.smooth ? 12 : 6;
        else if (id == "makeup")
            v = float(std::clamp(targetLevel - level, 0., 12.));
        else if (id == "output") {
            v = float(std::clamp(-3. - db(float(f.peak)), -12., 0.));
            reason = "Reserve 3 dB input peak headroom; verify processed output";
        } else if (id == "frequency") {
            v = f.bands[2] > f.bands[3] ? 4500 : 7000;
            reason = "Detection crossover chosen from upper-band energy";
        } else if (id == "range")
            v = k == Kind::Gate
                    ? 40
                    : float(std::clamp(6. + 20 * std::log10(std::max(f.bands[3] / f.rms, 1e-6)), 2.,
                                       10.));
        else if (id == "focus")
            v = float(std::clamp(f.bands[3] / f.rms, .2, 1.));
        else if (id == "width") {
            double target = o.mode == Mode::Reference ? ref->sideRatio : o.wide ? .65 : .4;
            v = float(std::clamp(target / std::max(f.sideRatio, .1), .5, 1.5));
            if (f.correlation < 0)
                v = .5f;
            reason =
                "Side energy target, narrowed when correlation is negative; cannot widen dual mono";
            confidence = .7f;
        } else if (id == "side")
            v = f.correlation < 0 ? -6 : 0;
        else if (id == "center" || id == "mono")
            v = 0;
        else if (id == "depth")
            v = o.cinematic ? .15f : 0;
        else if (id == "drive") {
            v = float(std::clamp(12. - db(float(f.peak)) - crest * .4, 0., 18.));
            reason = "Drive limited by measured peak and crest factor";
        } else if (id == "tone" || id == "filter") {
            v = o.dark   ? 3500
                : o.warm ? 6000
                         : float(std::clamp(12000. - f.bands[3] / f.rms * 12000, 2000., 14000.));
            reason = "Upper-band energy and brightness objective";
        } else if (id == "sensitivity")
            v = float(std::clamp(1. - crest / 30, .2, .8));
        else if (id == "sustain")
            v = o.smooth ? 2 : -2;
        else if (id == "shape")
            v = o.smooth ? .7f : 1.2f;
        else if (id == "decay")
            v = o.cinematic ? 4 : float(std::clamp(1. + crest * .05, .5, 3.));
        else if (id == "predelay")
            v = float(std::clamp(crest * 2, 5., 60.));
        else if (id == "size")
            v = o.cinematic ? .85f : .5f;
        else if (id == "lowcut")
            v = f.bands[0] / f.rms > .5 ? 200 : 80;
        else if (id == "midcut")
            v = o.warm ? -1 : -3;
        else if (id == "mode")
            v = k == Kind::Verb ? (o.cinematic ? 1 : 0) : (o.warm ? 2 : 0);
        else if (id == "density" || id == "diffusion")
            v = o.smooth ? .85f : .6f;
        else if (id == "modulation")
            v = o.cinematic ? .2f : .05f;
        else if (id == "time") {
            v = crest > 12 ? 250 : 375;
            reason = "Conservative echo objective; not detected tempo or reference echo timing";
            confidence = .15f;
        } else if (id == "feedback")
            v = o.cinematic ? .5f : .25f;
        else if (id == "pingpong")
            v = o.wide ? 1 : 0;
        else if (id == "hold")
            v = 50;
        else if (id == "hysteresis")
            v = 6;
        else if (id == "mix" || id == "delayMix" || id == "reverbMix")
            v = (k == Kind::Comp || k == Kind::Trans || k == Kind::Sess || k == Kind::Stereo) ? 1
                : o.cinematic                                                                 ? .35f
                                                                                              : .2f;
        else {
            reason = "Enable/routing is a user decision; retained";
            confidence = .1f;
        }
        result.values.push_back({i, p.constrain(v), confidence, reason});
    }
    if (result.values.empty())
        throw std::invalid_argument("Unknown learning scope");
    return result;
}
bool LearningSession::offer(Proposal p) {
    if (p.revision != revision)
        return false;
    reject();
    pending = std::move(p);
    available = true;
    status = "Suggestion available";
    return true;
}
bool LearningSession::preview() {
    if (!available || pending.revision != revision)
        return false;
    if (!previewing)
        before = values;
    for (auto &s : pending.values)
        if (s.index < values.size())
            values[s.index] = s.value;
    previewing = true;
    status = "Previewing";
    return true;
}
bool LearningSession::apply() {
    if (!preview())
        return false;
    history = before;
    before.clear();
    previewing = false;
    available = false;
    ++revision;
    status = "Applied";
    return true;
}
void LearningSession::reject() {
    if (previewing)
        values = before;
    before.clear();
    previewing = false;
    available = false;
    status = "Cancelled";
}
bool LearningSession::undo() {
    if (history.empty())
        return false;
    reject();
    values.swap(history);
    history.clear();
    ++revision;
    status = "Undone";
    return true;
}
void LearningSession::manual(size_t i, float value) {
    reject();
    if (i < values.size()) {
        values[i] = value;
        ++revision;
        status = "Ready";
    }
}
float Engine::Biquad::tick(float x) noexcept {
    double y = b0 * x + z1;
    z1 = b1 * x - a1 * y + z2;
    z2 = b2 * x - a2 * y;
    if (!std::isfinite(y) || std::abs(y) > 1e8) {
        z1 = z2 = 0;
        return 0;
    }
    return float(y);
}
void Engine::prepare(double sampleRate) {
    sr = std::isfinite(sampleRate) ? std::clamp(sampleRate, 8000., 384000.) : 48000.;
    delayL.assign(size_t(sr * 3) + 8, 0);
    delayR.assign(delayL.size(), 0);
    cursor = 0;
    low = {};
    fast = {};
    slow = {};
    envelope = {};
    gateGain = {};
    hold = {};
    eq = {};
    phase = 0;
    reduction = 0;
    gateOpen = false;
    for (size_t i = 0; i < meta.size(); ++i)
        smoothed[i] = meta[i].initial;
    if (kind == Kind::Verb || kind == Kind::FX) {
        for (auto &line : tank)
            line.assign(size_t(sr * .5) + 8, 0);
        for (auto &line : pre)
            line.assign(size_t(sr * .25) + 8, 0);
        for (auto &line : shimmer)
            line.assign(size_t(sr * .12) + 8, 0);
    }
    tankPosition = {};
    damping = {};
    bass = {};
    midLow = {};
    midHigh = {};
    prePosition = shimmerPosition = 0;
    grainPhase = 0;
}
float Engine::get(const char *id) const noexcept {
    for (size_t i = 0; i < meta.size(); ++i)
        if (meta[i].id == id)
            return smoothed[i];
    return 0;
}
void Engine::coefficients() noexcept {
    for (size_t b = 0; b < 4; ++b) {
        double f = std::clamp(double(smoothed[b * 4]), 20., sr * .45),
               A = std::pow(10., smoothed[b * 4 + 1] / 40.);
        double w = 2 * pi * f / sr, c = std::cos(w), s = std::sin(w),
               alpha = s / (2 * smoothed[b * 4 + 2]);
        double b0 = 1 + alpha * A, b1 = -2 * c, b2 = 1 - alpha * A, a0 = 1 + alpha / A, a1 = -2 * c,
               a2 = 1 - alpha / A;
        int shape = int(std::round(smoothed[b * 4 + 3]));
        if (shape != 0) {
            // RBJ shelf with fixed S=1; Q applies to bells only.
            double t = 2 * std::sqrt(A) * s / std::sqrt(2.);
            if (shape == 1) {
                b0 = A * ((A + 1) - (A - 1) * c + t);
                b1 = 2 * A * ((A - 1) - (A + 1) * c);
                b2 = A * ((A + 1) - (A - 1) * c - t);
                a0 = (A + 1) + (A - 1) * c + t;
                a1 = -2 * ((A - 1) + (A + 1) * c);
                a2 = (A + 1) + (A - 1) * c - t;
            } else {
                b0 = A * ((A + 1) + (A - 1) * c + t);
                b1 = -2 * A * ((A - 1) + (A + 1) * c);
                b2 = A * ((A + 1) + (A - 1) * c - t);
                a0 = (A + 1) - (A - 1) * c + t;
                a1 = 2 * ((A - 1) - (A + 1) * c);
                a2 = (A + 1) - (A - 1) * c - t;
            }
        }
        for (auto &channel : eq) {
            auto &q = channel[b];
            q.b0 = b0 / a0;
            q.b1 = b1 / a0;
            q.b2 = b2 / a0;
            q.a1 = a1 / a0;
            q.a2 = a2 / a0;
        }
    }
}
void Engine::ambience(float l, float r, float &outL, float &outR) noexcept {
    const bool fx = kind == Kind::FX;
    const float mode = fx ? 1 : get("mode"), size = fx ? .6f : get("size"), decay = get("decay");
    const float density = fx ? .8f : get("density"), diffusion = fx ? .8f : get("diffusion");
    const float tone = fx ? 6000 : get("tone"), lowcut = fx ? 100 : get("lowcut"),
                midcut = fx ? -2 : get("midcut");
    const float predelay = fx ? .015f : get("predelay") * .001f;
    const float modulation = fx ? .1f : get("modulation");
    auto tap = [](const std::vector<float> &data, size_t position, float delay) {
        double x = double(position) - std::clamp(double(delay), 1., double(data.size() - 2));
        if (x < 0)
            x += double(data.size());
        size_t a = size_t(x);
        float frac = float(x - double(a));
        return data[a] * (1 - frac) + data[(a + 1) % data.size()] * frac;
    };
    pre[0][prePosition] = l;
    pre[1][prePosition] = r;
    float inputL = predelay > 0 ? tap(pre[0], prePosition, predelay * float(sr)) : l;
    float inputR = predelay > 0 ? tap(pre[1], prePosition, predelay * float(sr)) : r;
    prePosition = (prePosition + 1) % pre[0].size();
    float times[]{.0297f, .0371f, .0411f, .0437f, .0531f, .0613f, .0719f, .0797f};
    std::array<float, 8> feedback{}, lengths{};
    float lp = float(1 - std::exp(-2 * pi * std::min(double(tone), sr * .45) / sr));
    float hp = float(1 - std::exp(-2 * pi * lowcut / sr));
    for (size_t j = 0; j < 8; ++j) {
        float modeScale = mode < .5f ? .7f : mode < 1.5f ? 1.5f : 1.f;
        lengths[j] = times[j] * (.5f + size * 1.5f) * modeScale * float(sr);
        lengths[j] += std::sin(phase + float(j)) * modulation * float(sr) * .0007f;
        float x = tap(tank[j], tankPosition[j], lengths[j]);
        damping[j] += lp * (x - damping[j]);
        bass[j] += hp * (damping[j] - bass[j]);
        x = damping[j] - bass[j];
        midLow[j] += float(1 - std::exp(-2 * pi * 250 / sr)) * (x - midLow[j]);
        midHigh[j] += float(1 - std::exp(-2 * pi * 1500 / sr)) * (x - midHigh[j]);
        x += (gain(midcut) - 1) * (midHigh[j] - midLow[j]);
        feedback[j] = x;
    }
    outL = (feedback[0] + feedback[2] - feedback[4] + feedback[6]) * .25f;
    outR = (feedback[1] - feedback[3] + feedback[5] + feedback[7]) * .25f;
    float shimmerL = 0, shimmerR = 0;
    // Two overlapping variable-delay grains read at 2x speed for an octave-up feedback voice.
    if (mode > 2.5f) {
        shimmer[0][shimmerPosition] = outL;
        shimmer[1][shimmerPosition] = outR;
        grainPhase += float(1. / (.04 * sr));
        if (grainPhase >= 1)
            grainPhase -= 1;
        float second = std::fmod(grainPhase + .5f, 1.f), window = std::sin(float(pi) * grainPhase);
        window *= window;
        auto pitch = [&](size_t c) {
            return tap(shimmer[c], shimmerPosition, (1 - grainPhase) * float(sr) * .04f + 2) *
                       window +
                   tap(shimmer[c], shimmerPosition, (1 - second) * float(sr) * .04f + 2) *
                       (1 - window);
        };
        shimmerL = pitch(0);
        shimmerR = pitch(1);
        shimmerPosition = (shimmerPosition + 1) % shimmer[0].size();
        outL += shimmerL * .3f;
        outR += shimmerR * .3f;
    }
    auto mixed = feedback;
    for (size_t step = 1; step < 8; step *= 2)
        for (size_t start = 0; start < 8; start += step * 2)
            for (size_t j = 0; j < step; ++j) {
                float a = mixed[start + j], b = mixed[start + j + step];
                mixed[start + j] = a + b;
                mixed[start + j + step] = a - b;
            }
    for (size_t j = 0; j < 8; ++j) {
        float feedbackGain = std::pow(.001f, lengths[j] / float(sr) / decay);
        float scattered = mixed[j] * .35355339f * diffusion + feedback[j] * (1 - diffusion);
        float excitation = (j % 2 ? inputR : inputL) * (j < 2 ? 1.f : density);
        float pitch = j % 2 ? shimmerR : shimmerL;
        tank[j][tankPosition[j]] =
            std::tanh(excitation * .3f + scattered * feedbackGain * .9f + pitch * .08f);
        tankPosition[j] = (tankPosition[j] + 1) % tank[j].size();
    }
}
void Engine::process(float *l, float *r, size_t n, const float *values) noexcept {
    if (!l || !values || delayL.empty())
        return;
    float smoothing = float(1 - std::exp(-1. / (.02 * sr)));
    auto coefficient = [&](float ms) {
        return float(std::exp(-1. / (std::max(ms, .1f) * .001 * sr)));
    };
    for (size_t i = 0; i < n; ++i) {
        for (size_t p = 0; p < meta.size(); ++p) {
            float target = meta[p].constrain(values[p]);
            smoothed[p] =
                meta[p].discrete ? target : smoothed[p] + smoothing * (target - smoothed[p]);
        }
        if (kind == Kind::EQ && i % 32 == 0)
            coefficients();
        float dry[2]{std::isfinite(l[i]) ? std::clamp(l[i], -32.f, 32.f) : 0,
                     r && std::isfinite(r[i]) ? std::clamp(r[i], -32.f, 32.f) : 0};
        if (!r)
            dry[1] = dry[0];
        float wet[2]{dry[0], dry[1]};
        for (int c = 0; c < 2; ++c) {
            float x = dry[c], absx = std::abs(x);
            fast[c] += float(1 - std::exp(-1. / (.001 * sr))) * (absx - fast[c]);
            slow[c] += float(1 - std::exp(-1. / (.05 * sr))) * (absx - slow[c]);
            if (kind == Kind::Comp) {
                float a = coefficient(absx > envelope[c] ? get("attack") : get("release"));
                envelope[c] = a * envelope[c] + (1 - a) * absx;
                float over = db(envelope[c]) - get("threshold"), knee = get("knee"), gr = 0,
                      slope = 1 - 1 / get("ratio");
                if (knee > 0 && over > -knee * .5f && over < knee * .5f)
                    gr = slope * (over + knee * .5f) * (over + knee * .5f) / (2 * knee);
                else if (over > 0)
                    gr = slope * over;
                reduction = gr;
                wet[c] = x * gain(get("makeup") - gr);
            } else if (kind == Kind::EQ) {
                for (auto &b : eq[c])
                    wet[c] = b.tick(wet[c]);
            } else if (kind == Kind::Sat) {
                float driven = x * gain(get("drive")), mode = get("mode"), d = get("density");
                float nonlinear = mode < .5f    ? std::tanh(driven)
                                  : mode < 1.5f ? std::clamp(driven, -1.f, 1.f)
                                                : std::tanh(driven + .3f) - std::tanh(.3f);
                float a =
                    float(1 - std::exp(-2 * pi * std::min(double(get("tone")), sr * .45) / sr));
                low[c] += a * (nonlinear - low[c]);
                wet[c] = (1 - d) * nonlinear + d * low[c];
            } else if (kind == Kind::Trans) {
                float transient = std::clamp(
                    (fast[c] - slow[c]) / (slow[c] + .001f) * (get("sensitivity") + .1f), 0.f, 1.f);
                transient = std::pow(transient, get("shape"));
                wet[c] = x * gain(get("attack") * transient + get("sustain") * (1 - transient));
            } else if (kind == Kind::Sess) {
                float a = float(
                    1 - std::exp(-2 * pi * std::min(double(get("frequency")), sr * .45) / sr));
                low[c] += a * (x - low[c]);
                float high = x - low[c];
                float e = std::abs(high),
                      ac = coefficient(e > envelope[c] ? get("attack") : get("release"));
                envelope[c] = ac * envelope[c] + (1 - ac) * e;
                float gr = std::clamp(db(envelope[c]) - get("threshold"), 0.f, get("range"));
                reduction = gr;
                wet[c] = x - high * (1 - gain(-gr)) * get("focus");
            } else if (kind == Kind::Gate) {
                // Stereo-linked detection; shared opening avoids image shifts.
                float level = db(std::max(std::abs(dry[0]), std::abs(dry[1])));
                if (level > get("threshold")) {
                    gateOpen = true;
                    hold[c] = get("hold") * .001f * float(sr);
                }
                if (level < get("threshold") - get("hysteresis")) {
                    if (hold[c] > 0)
                        --hold[c];
                    else
                        gateOpen = false;
                }
                float target = gateOpen ? 1.f : gain(-get("range")),
                      a = coefficient(target > gateGain[c] ? get("attack") : get("release"));
                gateGain[c] = a * gateGain[c] + (1 - a) * target;
                wet[c] = x * gateGain[c];
                reduction = -db(gateGain[c]);
            }
        }
        if (kind == Kind::Stereo) {
            float mid = (dry[0] + dry[1]) * .5f, side = (dry[0] - dry[1]) * .5f;
            mid *= gain(get("center"));
            side *= get("width") * gain(get("side"));
            // Depth is a mono-safe high-frequency side attenuation, not a phase delay.
            low[0] += .05f * (side - low[0]);
            side = side * (1 - get("depth")) + low[0] * get("depth");
            if (get("mono") > .5f)
                side = 0;
            wet[0] = mid + side;
            wet[1] = mid - side;
        }
        if (kind == Kind::Delay || kind == Kind::Verb || kind == Kind::FX) {
            float time =
                kind == Kind::Verb ? (30 + get("size") * 120 + get("predelay")) : get("time");
            float mod = kind == Kind::FX ? 0 : get("modulation");
            phase += float(2 * pi * .4 / sr);
            if (phase > 2 * pi)
                phase -= float(2 * pi);
            float seconds = time * .001f + std::sin(phase) * mod * .002f;
            auto read = [&](const std::vector<float> &data, float secondsOffset) {
                double pos = double(cursor) - std::clamp(double(secondsOffset * float(sr)), 1.,
                                                         double(data.size() - 2));
                if (pos < 0)
                    pos += double(data.size());
                auto idx = size_t(pos);
                float frac = float(pos - double(idx));
                return data[idx] * (1 - frac) + data[(idx + 1) % data.size()] * frac;
            };
            float dl = read(delayL, seconds), dr = read(delayR, seconds * 1.011f);
            if (kind == Kind::Verb) {
                ambience(dry[0], dry[1], wet[0], wet[1]);
            } else {
                float a =
                    float(1 - std::exp(-2 * pi * std::min(double(get("filter")), sr * .45) / sr));
                low[0] += a * (dl - low[0]);
                low[1] += a * (dr - low[1]);
                float inputL = dry[0], inputR = dry[1];
                if (kind == Kind::FX) {
                    if (get("driveOn") > .5f) {
                        inputL = std::tanh(inputL * gain(get("drive")));
                        inputR = std::tanh(inputR * gain(get("drive")));
                    }
                    if (get("filterOn") > .5f) {
                        slow[0] += a * (inputL - slow[0]);
                        slow[1] += a * (inputR - slow[1]);
                        inputL = slow[0];
                        inputR = slow[1];
                    }
                }
                bool ping = kind == Kind::Delay && get("pingpong") > .5f;
                delayL[cursor] = std::tanh(inputL + low[ping ? 1 : 0] * get("feedback"));
                delayR[cursor] = std::tanh(inputR + low[ping ? 0 : 1] * get("feedback"));
                wet[0] = low[0];
                wet[1] = low[1];
                if (kind == Kind::FX) {
                    float dm = get("delayOn") > .5f ? get("delayMix") : 0;
                    wet[0] = inputL * (1 - dm) + wet[0] * dm;
                    wet[1] = inputR * (1 - dm) + wet[1] * dm;
                    float verbL = 0, verbR = 0;
                    ambience(wet[0], wet[1], verbL, verbR);
                    if (get("reverbOn") > .5f) {
                        float rm = get("reverbMix");
                        wet[0] = wet[0] * (1 - rm) + verbL * rm;
                        wet[1] = wet[1] * (1 - rm) + verbR * rm;
                    }
                }
            }
            if (kind != Kind::FX) {
                float m = (wet[0] + wet[1]) * .5f, s = (wet[0] - wet[1]) * .5f * get("width");
                wet[0] = m + s;
                wet[1] = m - s;
            }
            cursor = (cursor + 1) % delayL.size();
        }
        float mix = (kind == Kind::EQ || kind == Kind::Gate) ? 1 : get("mix"),
              out = gain(get("output"));
        for (int c = 0; c < (r ? 2 : 1); ++c) {
            float y = (dry[c] + mix * (wet[c] - dry[c])) * out;
            if (!std::isfinite(y))
                y = 0;
            (c == 0 ? l : r)[i] = std::clamp(y, -64.f, 64.f);
        }
    }
}
} // namespace da
