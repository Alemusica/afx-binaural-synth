
#pragma once
#include <cmath>
#include <array>

namespace afx {

struct Biquad {
    // Direct Form II Transposed
    float b0{1.f}, b1{0.f}, b2{0.f}, a1{0.f}, a2{0.f};
    float z1{0.f}, z2{0.f};

    inline float process(float x) {
        float y = b0 * x + z1;
        z1 = b1 * x - a1 * y + z2;
        z2 = b2 * x - a2 * y;
        return y;
    }
    inline void reset() { z1 = z2 = 0.f; }
};

struct BiquadCascade {
    std::array<Biquad, 8> s{}; // max 8 biquad
    int count{0};
    inline float process(float x) {
        for (int i=0;i<count;i++) x = s[i].process(x);
        return x;
    }
    inline void reset() { for (int i=0;i<count;i++) s[i].reset(); }
};

} // namespace afx
