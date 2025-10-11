
#pragma once
#include <vector>
#include <atomic>
#include <cstdint>

namespace afx {

// Semplice ring lock-free per blocchi di float (solo per prototipo)
struct RingBuffer {
    std::vector<float> buf;
    std::atomic<size_t> r{0}, w{0};
    size_t mask;

    RingBuffer(size_t capacity_pow2) {
        size_t cap = 1; while (cap < capacity_pow2) cap <<= 1;
        buf.resize(cap, 0.f);
        mask = cap - 1;
    }

    size_t write(const float* src, size_t n) {
        size_t wi = w.load(std::memory_order_relaxed);
        for (size_t i=0;i<n;i++) buf[(wi+i) & mask] = src[i];
        w.store(wi+n, std::memory_order_release);
        return n;
    }
    size_t read(float* dst, size_t n) {
        size_t ri = r.load(std::memory_order_relaxed);
        size_t wi = w.load(std::memory_order_acquire);
        size_t avail = wi - ri;
        if (n > avail) n = avail;
        for (size_t i=0;i<n;i++) dst[i] = buf[(ri+i) & mask];
        r.store(ri+n, std::memory_order_release);
        return n;
    }
};

} // namespace afx
