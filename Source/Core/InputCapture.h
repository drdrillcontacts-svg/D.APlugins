#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <vector>
namespace da {
// One audio producer, one message-thread consumer. Never reset producer memory from the UI.
// request/cancel/take are message-thread operations; prepare/process are audio-side operations.
class InputCapture {
  public:
    static constexpr size_t size = 65536;
    static_assert(std::atomic<int>::is_always_lock_free, "Capture state must be lock-free");
    void prepare(double rate) noexcept { cancel(); configuredRate = std::isfinite(rate) ? std::clamp(rate,8000.,384000.) : 48000.; }
    bool request() noexcept {
        int expected = 0;
        if (state.compare_exchange_strong(expected, 1, std::memory_order_acq_rel)) return true;
        expected = 4;
        return state.compare_exchange_strong(expected, 1, std::memory_order_acq_rel);
    }
    void cancel() noexcept {
        int s = state.load(std::memory_order_acquire);
        while (s != 0 && s != 4 &&
               !state.compare_exchange_weak(s, s == 3 ? 0 : 4, std::memory_order_acq_rel)) {}
    }
    void process(const float *l, const float *r, size_t count) noexcept {
        int expected = 4;
        state.compare_exchange_strong(expected, 0, std::memory_order_acq_rel);
        expected = 1;
        if (state.compare_exchange_strong(expected, 2, std::memory_order_acq_rel)) {
            position = 0; completedRate = configuredRate;
        }
        if (state.load(std::memory_order_acquire) != 2) return;
        const auto n = std::min(count, size-position);
        for (size_t i=0; i<n; ++i) { left[position+i]=l[i]; right[position+i]=r ? r[i] : l[i]; }
        position += n;
        if (position == size) {
            expected = 2;
            state.compare_exchange_strong(expected, 3, std::memory_order_release);
        }
    }
    bool take(std::vector<float> &l, std::vector<float> &r, double &rate) {
        if (state.load(std::memory_order_acquire) != 3) return false;
        l.assign(left.begin(),left.end()); r.assign(right.begin(),right.end()); rate=completedRate;
        state.store(0,std::memory_order_release); return true;
    }
  private:
    std::array<float,size> left{},right{};
    std::atomic<int> state{0}; // idle / requested / writing / ready / cancelled
    size_t position=0;
    double configuredRate=48000,completedRate=48000;
};
}
