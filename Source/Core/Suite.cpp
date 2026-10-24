#include "Suite.h"
#include <cctype>
#include <stdexcept>
namespace da {
double syncedDelayMs(double bpm, int division) noexcept {
    static constexpr double beats[]{.125,.1875,1./12,.25,.375,1./6,.5,.75,1./3,1,1.5,2./3,2,3,4};
    if(!std::isfinite(bpm)||bpm<10||bpm>400)bpm=120;
    return 60000./bpm*beats[std::clamp(division,0,14)];
}
void Engine::setTempo(double bpm) noexcept {
    validTempo=std::isfinite(bpm)&&bpm>=10&&bpm<=400;
    if(validTempo){hostBpm=bpm;seenTempo=true;}
}
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
        add("tone", "Tone", "Colour", "Hz", 500, 18000, 10000);
        add("mix", "Mix", "Space", "", 0, 1, .25f);
        add("width", "Width", "Space", "", 0, 2, 1);
        add("density", "Density", "Space", "", 0, 1, .7f);
        add("diffusion", "Diffusion", "Space", "", 0, 1, .7f);
        add("modulation", "Modulation", "Space", "", 0, 1, .1f);
        add("lowcut", "Low cut", "Colour", "Hz", 20, 1000, 120);
        add("midcut", "Mid cut", "Colour", "dB", -12, 0, -1);
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
        // Append IDs: existing eight parameter IDs and their host ordering stay unchanged.
        add("sync", "Tempo sync", "Echo", "", 0, 1, 0, true);
        add("division", "Note division", "Echo", "", 0, 14, 6, true);
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
    double sum = 0, ll = 0, rr = 0, lr = 0, side = 0, delta = 0;
    std::array<double, 2> previous{};
    std::array<std::array<double, 3>, 2> low{};
    double cuts[]{200, 1000, 5000};
    std::array<double, 3> alpha{};
    for (size_t j=0;j<3;++j) alpha[j]=1-std::exp(-2*pi*cuts[j]/sr);
    for (size_t i = 0; i < n; ++i) {
        double a = std::isfinite(l[i]) ? l[i] : 0;
        double b = r ? (std::isfinite(r[i]) ? r[i] : 0) : a;
        sum += (a * a + b * b) * .5;
        ll += a * a;
        rr += b * b;
        lr += a * b;
        side += (a - b) * (a - b) * .25;
        f.peak = std::max(f.peak, std::max(std::abs(a), std::abs(b)));
        const double channels[]{a,b};
        for(size_t c=0;c<2;++c) {
            double x=channels[c];
            for(size_t j=0;j<3;++j)low[c][j]+=alpha[j]*(x-low[c][j]);
            const double bands[]{low[c][0],low[c][1]-low[c][0],low[c][2]-low[c][1],x-low[c][2]};
            for(size_t j=0;j<4;++j)f.bands[j]+=.5*bands[j]*bands[j];
            delta+=.5*std::abs(x-previous[c]);previous[c]=x;
        }
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
    std::vector<std::string> tokens;
    std::string token;
    for (unsigned char c : s) {
        if(std::isalnum(c)||c=='\'')token+=char(c);
        else if(!token.empty()){tokens.push_back(token);token.clear();}
    }
    if(!token.empty())tokens.push_back(token);
    const auto has=[&](const char *word){return std::find(tokens.begin(),tokens.end(),word)!=tokens.end();};
    for(auto word:{"not","no","never","without","isn't","isnt","don't","dont","less"})
        if(has(word))throw std::invalid_argument("Negated goals are unsupported; use positive keywords only");
    o.dark=has("dark");o.warm=has("warm");o.wide=has("wide");
    o.smooth=has("smooth");o.cinematic=has("cinematic");
    if (!(o.dark || o.warm || o.wide || o.smooth || o.cinematic))
        throw std::invalid_argument(
            "Supported goals: dark, warm, wide, smooth, cinematic; no goal recognized");
    return o;
}
bool canLearn(Kind k, const std::string &id) {
    const char *allowed = "";
    switch (k) {
    case Kind::Verb: allowed = " predelay decay tone width lowcut "; break;
    case Kind::Delay: allowed = " filter width "; break;
    case Kind::Comp: allowed = " threshold ratio attack release makeup "; break;
    case Kind::EQ: allowed = " b1gain b2gain b3gain b4gain "; break;
    case Kind::Sat: allowed = " drive tone "; break;
    case Kind::Trans: allowed = " attack sensitivity "; break;
    case Kind::Stereo: allowed = " width side "; break;
    case Kind::Sess: allowed = " frequency threshold range focus release "; break;
    case Kind::Gate: allowed = " threshold attack release "; break;
    case Kind::FX: allowed = " drive filter decay "; break;
    }
    return !id.empty() && std::string(allowed).find(" " + id + " ") != std::string::npos;
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
    for (size_t i = 0; i < meta.size(); ++i) {
        const auto &p = meta[i];
        if (!canLearn(k, p.id) || (scope != "all" && scope != p.id && scope != p.module))
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
            v = float(std::clamp(difference, -6., 6.));
            reason = "Broad-band normalized tonal gain correction; frequency, Q and shape retained";
            confidence = .65f;
        } else if (id == "threshold") {
            v = float(k == Kind::Gate ? level-18 : k == Kind::Sess ? db(float(f.bands[3]))-4 : level+crest*.25);
            reason = k==Kind::Gate ? "Input RMS minus 18 dB starting point; not a measured noise floor" :
                     k==Kind::Sess ? "Measured upper-band RMS threshold" : "Measured RMS and crest-factor threshold";
            confidence=.65f;
        } else if(id=="ratio") {
            v=float(std::clamp(crest/3,1.5,6.));reason="Crest-factor compression objective";
        } else if(id=="attack") {
            v=k==Kind::Trans ? float(std::clamp(6.-crest*.4,-6.,6.)) : float(crest>12?20:5);
            reason="Source-crest transient preservation starting point";
        } else if(id=="release") {
            v=float(std::clamp((100.+crest*5)*(o.smooth?1.5:1.),50.,600.));
            reason="Source-crest recovery estimate; smooth extends recovery; not detected tempo";
        } else if(id=="makeup") {
            v=float(std::clamp(targetLevel-level,0.,12.));reason="Target/input RMS difference; not measured processed GR";
        } else if(id=="frequency") {
            v=f.bands[2]>f.bands[3]?4500:7000;reason="Upper-band energy crossover choice; not precise sibilance classification";
        } else if(id=="range") {
            v=float(std::clamp(6.+20*std::log10(std::max(f.bands[3]/f.rms,1e-6)),2.,10.));
            reason="Relative upper-band energy reduction range";
        } else if(id=="focus") {
            v=float(std::clamp(f.bands[3]/f.rms,.2,1.));reason="Relative upper-band energy reduction strength";
        } else if(id=="width") {
            double target=o.mode==Mode::Reference?ref->sideRatio:o.wide?.65:.4;
            v=float(std::clamp(target/std::max(f.sideRatio,.1),.5,1.5));
            if(f.correlation<0)v=.5f;
            reason="Measured side energy and correlation; cannot widen identical dual mono";confidence=.7f;
        } else if(id=="side") {
            v=f.correlation<0?-6:0;reason="Negative input correlation narrows Side; verify output mono compatibility";
        } else if(id=="drive") {
            v=float(std::clamp(12.-db(float(f.peak))-crest*.4,0.,18.));reason="Measured input peak/crest headroom heuristic";
        } else if(id=="tone"||id=="filter") {
            v=o.dark?3500:o.warm?6000:float(std::clamp(12000.-f.bands[3]/f.rms*12000,2000.,14000.));
            reason="Upper-band energy and dark/warm objective";
        } else if(id=="sensitivity") {
            v=float(std::clamp(1.-crest/30,.2,.8));reason="Measured source crest-factor sensitivity starting point";
        } else if(id=="decay") {
            v=o.cinematic?4:float(std::clamp(1.+crest*.05,.5,3.));reason="Source crest/ambience objective; not reference RT60";
        } else if(id=="predelay") {
            v=float(std::clamp(crest*2,5.,60.));reason="Measured source crest-factor separation estimate";
        } else if(id=="lowcut") {
            v=f.bands[0]/f.rms>.5?200:80;reason="Relative low-band energy; audition source fundamentals";
        } else throw std::logic_error("Missing strategy for an eligible parameter");
        result.values.push_back({i, p.constrain(v), confidence, reason});
    }
    if (result.values.empty())
        throw std::invalid_argument("Scope has no audio-learnable controls");
    return result;
}
bool LearningSession::offer(Proposal p) {
    if (p.revision != revision)
        return false;
    reject();
    pending = std::move(p);
    available = true;
    status = "Ready to audition / Preview or Apply";
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
    history.clear(); // Never let an old full-state Undo overwrite newer manual/automation edits.
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
    delayL.assign(size_t(sr * (kind==Kind::Delay ? 25 : 3)) + 8, 0);
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
    controlCounter=0;diffuserPosition={};inputBass={};inputMidLow={};inputMidHigh={};
    if(kind==Kind::Verb||kind==Kind::FX) {
        constexpr double times[]{.0047,.0083,.0127,.0179};
        for(size_t i=0;i<8;++i)inputDiffuser[i].assign(size_t(sr*times[i%4]*(i<4?1.:1.071))+1,0);
    }
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
void Engine::refreshReverb() noexcept {
    const bool fx=kind==Kind::FX;
    const float mode=fx?1:get("mode"),size=fx?.6f:get("size"),decay=std::max(.1f,get("decay"));
    constexpr float times[]{.0297f,.0371f,.0411f,.0437f,.0531f,.0613f,.0719f,.0797f};
    for(size_t j=0;j<8;++j) {
        reverbLength[j]=times[j]*(.5f+size*1.5f)*(mode<.5f?.7f:mode<1.5f?1.5f:1.f)*float(sr);
        reverbGain[j]=std::pow(.001f,reverbLength[j]/float(sr)/decay)*(mode>2.5f?.92f:1.f);
    }
    const auto pole=[&](double f){return float(1-std::exp(-2*pi*std::min(f,sr*.45)/sr));};
    reverbLp=pole(fx?9000:get("tone"));reverbHp=pole(fx?100:get("lowcut"));
    reverbMidLp=pole(250);reverbMidHp=pole(1500);reverbMidGain=gain(fx?-1:get("midcut"));
}
void Engine::ambience(float l, float r, float &outL, float &outR) noexcept {
    const bool fx=kind==Kind::FX;
    const float mode=fx?1:get("mode"),density=fx?.8f:get("density"),diffusion=fx?.8f:get("diffusion");
    const float predelay=fx?.015f:get("predelay")*.001f,modulation=fx?.1f:get("modulation");
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
    // High-pass / broad-mid color once at the entrance, not on every feedback circulation.
    float inputs[]{inputL,inputR};
    for(size_t c=0;c<2;++c) {
        inputBass[c]+=reverbHp*(inputs[c]-inputBass[c]);
        float x=inputs[c]-inputBass[c];
        inputMidLow[c]+=reverbMidLp*(x-inputMidLow[c]);
        inputMidHigh[c]+=reverbMidHp*(x-inputMidHigh[c]);
        x+=(reverbMidGain-1)*(inputMidHigh[c]-inputMidLow[c]);
        // Four energy-preserving allpasses build early density without damping the late network.
        const float g=.25f+.45f*diffusion;
        for(size_t stage=0;stage<4;++stage) {
            const auto k=c*4+stage;auto &line=inputDiffuser[k];auto &pos=diffuserPosition[k];
            const float y=line[pos]-g*x;line[pos]=x+g*y;x=y;
            pos=(pos+1)%line.size();
        }
        inputs[c]=x;
    }
    inputL=inputs[0];inputR=inputs[1];
    std::array<float,8> feedback{},raw{};
    constexpr float sj[]{0,.84147098f,.90929743f,.14112001f,-.7568025f,-.95892427f,-.2794155f,.6569866f};
    constexpr float cj[]{1,.5403023f,-.4161468f,-.9899925f,-.6536436f,.2836622f,.9601703f,.7539023f};
    const float sp=std::sin(phase),cp=std::cos(phase);
    for(size_t j=0;j<8;++j) {
        const float length=reverbLength[j]+(sp*cj[j]+cp*sj[j])*modulation*float(sr)*.0007f;
        raw[j]=tap(tank[j],tankPosition[j],length);
        damping[j]+=reverbLp*(raw[j]-damping[j]);
        feedback[j]=damping[j];
    }
    // Orthogonal, all-line stereo decoding. First arrivals keep their high-frequency detail.
    outL=(raw[0]+raw[1]-raw[2]-raw[3]+raw[4]+raw[5]-raw[6]-raw[7])*.12374369f;
    outR=(raw[0]-raw[1]+raw[2]-raw[3]-raw[4]+raw[5]-raw[6]+raw[7])*.12374369f;
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
        const float feedbackGain=reverbGain[j];
        const float scattered=mixed[j]*.35355339f;
        const float excitation=(j%2?inputR:inputL)*(j<2?1.f:density)/std::sqrt(2+6*density*density);
        float pitch = j % 2 ? shimmerR : shimmerL;
        const float value=excitation+scattered*feedbackGain+pitch*.025f;
        // Transparent at normal levels; exceptional overload protection only.
        tank[j][tankPosition[j]]=std::abs(value)>4 ? std::copysign(4+4*std::tanh((std::abs(value)-4)*.25f),value) : value;
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
        if(controlCounter==0) {
            if(kind==Kind::EQ)coefficients();
            if(kind==Kind::Verb||kind==Kind::FX)refreshReverb();
        }
        controlCounter=(controlCounter+1)%32;
        float dry[2]{std::isfinite(l[i]) ? std::clamp(l[i], -32.f, 32.f) : 0,
                     r && std::isfinite(r[i]) ? std::clamp(r[i], -32.f, 32.f) : 0};
        if (!r)
            dry[1] = dry[0];
        float wet[2]{dry[0], dry[1]};
        if(kind==Kind::Gate) {
            const float level=db(std::max(std::abs(dry[0]),std::abs(dry[1])));
            if(level>get("threshold")){gateOpen=true;hold[0]=get("hold")*.001f*float(sr);}
            if(level<get("threshold")-get("hysteresis")) {
                if(hold[0]>0)--hold[0];else gateOpen=false;
            }
        }
        reduction=0;
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
                reduction = std::max(reduction,gr);
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
                reduction = std::max(reduction,gr);
                wet[c] = x - high * (1 - gain(-gr)) * get("focus");
            } else if (kind == Kind::Gate) {
                float target = gateOpen ? 1.f : gain(-get("range")),
                      a = coefficient(target > gateGain[c] ? get("attack") : get("release"));
                gateGain[c] = a * gateGain[c] + (1 - a) * target;
                wet[c] = x * gateGain[c];
                reduction = std::max(reduction,-db(gateGain[c]));
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
            const bool sync=kind==Kind::Delay&&get("sync")>.5f;
            if(sync)time=float(syncedDelayMs(hostBpm,int(get("division"))));
            float mod = kind == Kind::FX || sync ? 0 : get("modulation");
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
            float dl = read(delayL, seconds), dr = read(delayR, sync ? seconds : seconds * 1.011f);
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
