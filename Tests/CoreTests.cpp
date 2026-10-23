#include "Core/Suite.h"
#include <iostream>
#include <limits>
#include <stdexcept>
void check(bool b, const char *msg) {
    if (!b)
        throw std::runtime_error(msg);
}
std::vector<float> defaults(da::Kind kind) {
    std::vector<float> result;
    for (const auto &p : da::parameters(kind))
        result.push_back(p.initial);
    return result;
}
void set(da::Kind kind, std::vector<float> &values, const std::string &id, float value) {
    auto meta = da::parameters(kind);
    for (size_t i = 0; i < meta.size(); ++i)
        if (meta[i].id == id) {
            values[i] = value;
            return;
        }
    throw std::runtime_error("Unknown test parameter");
}
double energy(const std::vector<float> &v, size_t start = 0) {
    double result = 0;
    for (size_t i = start; i < v.size(); ++i)
        result += double(v[i]) * v[i];
    return result;
}
void characteristicTests() {
    constexpr size_t n = 96000;
    for (int k = 0; k < 10; ++k) {
        auto kind = static_cast<da::Kind>(k);
        auto values = defaults(kind);
        da::Engine engine(kind);
        engine.prepare(48000);
        std::vector<float> l(n), r(n), source(n);
        if (kind == da::Kind::Delay || kind == da::Kind::Verb || kind == da::Kind::FX) {
            l[0] = r[0] = .5f;
            source = l;
            engine.process(l.data(), r.data(), n, values.data());
            check(energy(l, 1024) > 1e-7, "Time effect must produce a real tail");
        } else {
            for (size_t i = 0; i < n; ++i)
                source[i] = .4f * float(std::sin(2 * da::pi * 1000 * double(i) / 48000));
            l = source;
            r = source;
            if (kind == da::Kind::EQ)
                set(kind, values, "b2gain", 12);
            if (kind == da::Kind::Trans)
                set(kind, values, "sustain", -12);
            if (kind == da::Kind::Stereo) {
                for (auto &x : r)
                    x = -x;
                set(kind, values, "width", 0);
            }
            if (kind == da::Kind::Sess) {
                for (size_t i = 0; i < n; ++i)
                    source[i] = .4f * float(std::sin(2 * da::pi * 9000 * double(i) / 48000));
                l = source;
                r = source;
            }
            if (kind == da::Kind::Gate)
                set(kind, values, "threshold", 0);
            engine.process(l.data(), r.data(), n, values.data());
            double original = energy(source, n / 2), processed = energy(l, n / 2);
            if (kind == da::Kind::EQ || kind == da::Kind::Sat)
                check(processed > original * 1.1, "EQ/saturation must change level");
            else
                check(processed < original * .9, "Dynamics/image effect must reduce test energy");
        }
        // Non-default rates and short host blocks exercise delay wrapping and filter limits.
        for (double rate : {8000., 44100., 96000.}) {
            engine.prepare(rate);
            std::vector<float> a(257, .1f), b(257, -.1f);
            engine.process(a.data(), b.data(), a.size(), values.data());
            for (auto x : a)
                check(std::isfinite(x), "Non-default-rate output finite");
        }
    }
    // Reference guidance uses features extracted from actual source/target samples.
    std::vector<float> source(n), target(n);
    for (size_t i = 0; i < n; ++i) {
        float low = .2f * float(std::sin(2 * da::pi * 440 * double(i) / 48000));
        float high = .2f * float(std::sin(2 * da::pi * 8000 * double(i) / 48000));
        source[i] = low + high;
        target[i] = low + high * .2f;
    }
    auto f = da::analyze(source.data(), nullptr, n, 48000),
         ref = da::analyze(target.data(), nullptr, n, 48000);
    da::Objective objective;
    objective.mode = da::Mode::Reference;
    auto p = da::learn(da::Kind::EQ, f, &ref, objective, defaults(da::Kind::EQ), "b4gain", 0);
    check(p.values[0].value < -1, "Darker reference must propose upper-band attenuation");
    da::LearningSession session(defaults(da::Kind::EQ));
    auto proposal =
        da::learn(da::Kind::EQ, f, nullptr, {}, session.values, "all", session.revision);
    session.manual(0, 200);
    check(!session.offer(proposal), "Manual decisions invalidate pending proposals");
    std::cout << "PASS characteristic DSP, non-default rates, actual reference signal and manual "
                 "invalidation checks\n";
}
int main() {
    try {
        constexpr size_t n = 48000;
        std::vector<float> left(n), right(n);
        for (size_t i = 0; i < n; ++i) {
            left[i] = .15f * float(std::sin(2 * da::pi * 440 * double(i) / 48000));
            right[i] = left[i] * .7f;
        }
        auto features = da::analyze(left.data(), right.data(), n, 48000);
        check(features.valid(), "Analysis failed");
        check(features.rms > .08 && features.rms < .12, "RMS accuracy");
        auto reference = features;
        reference.bands[3] *= .5;
        reference.sideRatio = .2;
        for (int k = 0; k < 10; ++k) {
            auto kind = static_cast<da::Kind>(k);
            auto meta = da::parameters(kind);
            std::vector<float> values;
            for (auto &p : meta)
                values.push_back(p.initial);
            for (size_t i = 0; i < meta.size(); ++i) {
                auto proposal = da::learn(kind, features, nullptr, {}, values, meta[i].id, 0);
                check(proposal.values.size() == 1 && proposal.values[0].index == i,
                      "Per-parameter isolation");
                check(proposal.values[0].value >= meta[i].min &&
                          proposal.values[0].value <= meta[i].max,
                      "Bounded proposal");
                check(!proposal.values[0].reason.empty(), "Explanation missing");
            }
            for (auto &p : meta) {
                auto proposal = da::learn(kind, features, nullptr, {}, values, p.module, 0);
                for (auto &s : proposal.values)
                    check(meta[s.index].module == p.module, "Module isolation");
            }
            auto all = da::learn(kind, features, nullptr, {}, values, "all", 0);
            check(all.values.size() == meta.size(), "Learn All coverage");
            da::LearningSession session(values);
            check(session.offer(all), "Offer");
            check(session.preview(), "Preview");
            session.reject();
            check(session.values == values, "Reject restores");
            check(session.offer(all) && session.apply(), "Apply");
            check(session.undo() && session.values == values, "Undo restores");
            check(!session.offer(all), "Reject stale result");
            auto objective = da::describe("dark warm wide smooth cinematic");
            check(!da::learn(kind, features, nullptr, objective, values, "all", 0).values.empty(),
                  "Description");
            objective.mode = da::Mode::Reference;
            check(
                !da::learn(kind, features, &reference, objective, values, "all", 0).values.empty(),
                "Reference");
            bool failed = false;
            try {
                da::learn(kind, features, nullptr, objective, values, "all", 0);
            } catch (const std::invalid_argument &) {
                failed = true;
            }
            check(failed, "Missing reference must fail");
            da::Engine engine(kind);
            engine.prepare(48000);
            for (int channelMode = 0; channelMode < 2; ++channelMode)
                for (int extreme = 0; extreme < 3; ++extreme) {
                    for (size_t j = 0; j < meta.size(); ++j)
                        values[j] = extreme == 0   ? meta[j].initial
                                    : extreme == 1 ? meta[j].min
                                                   : meta[j].max;
                    auto l = left, r = right;
                    l[0] = std::numeric_limits<float>::quiet_NaN();
                    l[1] = std::numeric_limits<float>::infinity();
                    l[2] = 20;
                    engine.process(l.data(), channelMode ? r.data() : nullptr, n, values.data());
                    for (auto x : l)
                        check(std::isfinite(x), "Finite mono/left DSP");
                    for (auto x : r)
                        check(std::isfinite(x), "Finite right DSP");
                }
            std::vector<float> silence(n);
            engine.prepare(48000);
            engine.process(silence.data(), nullptr, n, values.data());
            for (auto x : silence)
                check(std::abs(x) < 1e-6, "Silence stays silent");
            std::cout << "PASS kind " << k << ": " << meta.size()
                      << " parameter scopes, module/all/reference/description, lifecycle, "
                         "mono/stereo/extremes/silence\n";
        }
        bool failed = false;
        try {
            da::describe("purple unicorn");
        } catch (...) {
            failed = true;
        }
        check(failed, "Unsupported description");
        std::vector<float> silence(n);
        auto silent = da::analyze(silence.data(), nullptr, n, 48000);
        check(!silent.valid(), "Silence invalid for learning");
        // Audio responsiveness: a louder source must yield a higher compressor threshold.
        auto louder = left;
        for (auto &x : louder)
            x *= 2;
        auto loud = da::analyze(louder.data(), nullptr, n, 48000);
        std::vector<float> values;
        for (auto &p : da::parameters(da::Kind::Comp))
            values.push_back(p.initial);
        auto a = da::learn(da::Kind::Comp, features, nullptr, {}, values, "threshold", 0),
             b = da::learn(da::Kind::Comp, loud, nullptr, {}, values, "threshold", 0);
        check(b.values[0].value > a.values[0].value + 4, "Audio-driven compressor threshold");
        characteristicTests();
        std::cout << "All core checks passed\n";
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
