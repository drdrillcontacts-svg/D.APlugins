#pragma once
#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
namespace da {
struct ScopeFrame {
    float input = 0, output = 0, left = 0, right = 0;
};
// One audio producer, one editor consumer. Full queue drops new frames, never blocks.
// The queue is processor-owned, so closing the editor cannot invalidate audio memory.
class ScopeFifo {
  public:
    static constexpr size_t capacity = 16384;
    static_assert(std::atomic<size_t>::is_always_lock_free, "Scope indices must be lock-free");
    size_t push(const ScopeFrame *frames, size_t count) noexcept {
        auto w = write.load(std::memory_order_relaxed), r = read.load(std::memory_order_acquire);
        size_t n = std::min(count, capacity - 1 - ((w + capacity - r) % capacity));
        for (size_t i = 0; i < n; ++i)
            data[(w + i) % capacity] = frames[i];
        write.store((w + n) % capacity, std::memory_order_release);
        return n;
    }
    size_t pop(ScopeFrame *frames, size_t maximum) noexcept {
        auto r = read.load(std::memory_order_relaxed), w = write.load(std::memory_order_acquire);
        size_t n = std::min(maximum, (w + capacity - r) % capacity);
        for (size_t i = 0; i < n; ++i)
            frames[i] = data[(r + i) % capacity];
        read.store((r + n) % capacity, std::memory_order_release);
        return n;
    }

  private:
    std::array<ScopeFrame, capacity> data{};
    alignas(64) std::atomic<size_t> write{0};
    alignas(64) std::atomic<size_t> read{0};
};
} // namespace da
