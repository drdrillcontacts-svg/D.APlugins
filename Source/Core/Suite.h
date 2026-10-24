#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace da {
constexpr double pi = 3.14159265358979323846;
enum class Kind { Verb, Delay, Comp, EQ, Sat, Trans, Stereo, Sess, Gate, FX };
struct Parameter {
    std::string id, name, module, unit;
    float min, max, initial;
    bool discrete = false;
    float constrain(float x) const {
        return std::isfinite(x) ? std::clamp(discrete ? std::round(x) : x, min, max) : initial;
    }
};
std::vector<Parameter> parameters(Kind);
Kind kindForName(const std::string &);
// Product policy: audio-informed controls only. Manual/taste/routing controls never change.
bool canLearn(Kind, const std::string &id);
inline constexpr std::array<const char*,15> divisionNames{{"1/32","1/32 D","1/32 T","1/16","1/16 D","1/16 T","1/8","1/8 D","1/8 T","1/4","1/4 D","1/4 T","1/2","1/2 D","1/1"}};
double syncedDelayMs(double bpm, int division) noexcept;
float db(float);
float gain(float);
struct Features {
    double rms = 0, peak = 0, crest = 0, correlation = 1, sideRatio = 0;
    std::array<double, 4> bands{};
    double transient = 0, duration = 0;
    size_t samples = 0;
    bool valid() const { return samples >= 1024 && rms > 1e-7 && std::isfinite(rms); }
};
Features analyze(const float *left, const float *right, size_t count, double sampleRate);
enum class Mode { Input, Reference, Description };
struct Objective {
    Mode mode = Mode::Input;
    bool dark = false, warm = false, wide = false, smooth = false, cinematic = false;
};
Objective describe(const std::string &); // Throws for unsupported/ambiguous text.
struct Suggestion {
    size_t index;
    float value;
    float confidence;
    std::string reason;
};
struct Proposal {
    std::vector<Suggestion> values;
    uint64_t revision = 0;
    std::string limitation;
};
Proposal learn(Kind, const Features &, const Features *reference, Objective,
               const std::vector<float> &current, const std::string &scope, uint64_t revision);
// Optional inference boundary. No trained model is shipped or silently simulated.
class ModelInference {
  public:
    virtual ~ModelInference() = default;
    virtual bool available() const noexcept = 0;
    virtual std::string modelVersion() const = 0;
    virtual Proposal infer(Kind, const Features &, const Features *, Objective,
                           const std::vector<float> &, const std::string &, uint64_t) = 0;
};
// State machine independent of host/UI. Preview is reversible; stale proposals never apply.
class LearningSession {
  public:
    explicit LearningSession(std::vector<float> initial) : values(std::move(initial)) {}
    std::vector<float> values;
    uint64_t revision = 0;
    std::string status = "Ready";
    bool offer(Proposal);
    bool preview();
    bool apply();
    void reject();
    bool undo();
    void manual(size_t, float);

  private:
    Proposal pending;
    std::vector<float> before, history;
    bool available = false, previewing = false;
};
// All buffers are allocated in prepare; process is allocation-free, bounded and noexcept.
class Engine {
  public:
    explicit Engine(Kind k) : kind(k), meta(parameters(k)) {}
    void prepare(double sampleRate);
    void process(float *left, float *right, size_t count, const float *values) noexcept;
    float reductionDb() const { return reduction; }
    void setTempo(double bpm) noexcept;
    double tempo() const noexcept { return hostBpm; }
    bool tempoIsValid() const noexcept { return validTempo; }
    bool hasSeenTempo() const noexcept { return seenTempo; }

  private:
    Kind kind;
    double hostBpm=120;
    bool validTempo=false,seenTempo=false;
    size_t controlCounter=0;
    std::array<std::vector<float>,8> inputDiffuser;
    std::array<size_t,8> diffuserPosition{};
    std::array<float,2> inputBass{},inputMidLow{},inputMidHigh{};
    std::array<float,8> reverbLength{},reverbGain{};
    float reverbLp=0,reverbHp=0,reverbMidLp=0,reverbMidHp=0,reverbMidGain=1;
    void refreshReverb() noexcept;
    std::vector<Parameter> meta;
    double sr = 48000;
    std::vector<float> delayL, delayR;
    size_t cursor = 0;
    std::array<float, 64> smoothed{};
    std::array<float, 2> low{}, fast{}, slow{}, envelope{}, gateGain{}, hold{};
    struct Biquad {
        double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
        float tick(float) noexcept;
    };
    std::array<std::array<Biquad, 4>, 2> eq;
    float phase = 0, reduction = 0;
    bool gateOpen = false;
    float get(const char *) const noexcept;
    void coefficients() noexcept;
    void ambience(float, float, float &, float &) noexcept;
    std::array<std::vector<float>, 8> tank;
    std::array<size_t, 8> tankPosition{};
    std::array<float, 8> damping{}, bass{}, midLow{}, midHigh{};
    std::array<std::vector<float>, 2> pre, shimmer;
    size_t prePosition = 0, shimmerPosition = 0;
    float grainPhase = 0;
};
} // namespace da
