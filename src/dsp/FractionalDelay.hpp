#pragma once
#include <array>
#include <algorithm>
#include <vector>
#include <cmath>

namespace afx {

// -----------------------------
// Allpass Thiran ordine 3 (DF-II-T)
// H(z) = z^{-3} * A(z^{-1}) / A(z)
// A(z) = 1 + a1 z^{-1} + a2 z^{-2} + a3 z^{-3}
// Coefficienti chiusi per Δ = 3 + μ, con μ in [0,1)
// -----------------------------
struct ThiranAP3 {
    float mu{0.5f};
    float a1{0.f}, a2{0.f}, a3{0.f};
    float s1{0.f}, s2{0.f}, s3{0.f}; // stati DF-II-T

    void setMu(float mu_) {
        mu = std::clamp(mu_, 0.0f, 0.9999f);
        const float x = mu;
        a1 = -3.f * x / (x + 4.f);
        a2 =  3.f * x * (x + 1.f) / ((x + 4.f) * (x + 5.f));
        a3 = -      x * (x + 1.f) * (x + 2.f) / ((x + 4.f) * (x + 5.f) * (x + 6.f));
    }

    inline float process(float x) {
        // b = [a3, a2, a1, 1], a = [1, a1, a2, a3]
        const float b0 = a3, b1 = a2, b2 = a1, b3 = 1.f;
        float y  = b0 * x + s1;
        s1       = b1 * x - a1 * y + s2;
        s2       = b2 * x - a2 * y + s3;
        s3       = b3 * x - a3 * y;
        return y; // include z^{-3}
    }
    void reset() { s1 = s2 = s3 = 0.f; }
};

// Ritardo intero semplice (circular buffer)
struct IntDelay {
    std::vector<float> buf;
    size_t w{0};
    void setLen(int L) {
        if (L < 0) L = 0;
        buf.assign(L > 0 ? (size_t)L : (size_t)0, 0.f);
        w = 0;
    }
    inline float process(float x) {
        if (buf.empty()) return x;
        float y = buf[w];
        buf[w] = x;
        w++; if (w >= buf.size()) w = 0;
        return y;
    }
    void reset() { std::fill(buf.begin(), buf.end(), 0.f); w = 0; }
    int len() const { return (int)buf.size(); }
};

// Wrapper: D_tot = M + mu, con allpass N=3 -> parte intera L = max(M-3, 0)
struct ThiranDelay {
    IntDelay    intd;
    ThiranAP3   ap3;
    float Dtot{0.f};

    void setDelay(float D) {
        Dtot = std::max(0.f, D);
        int   M  = (int)std::floor(Dtot);
        float mu = Dtot - (float)M;    // [0,1)
        int   L  = M - 3;              // parte intera residua (>=0)
        if (L < 0) L = 0;
        intd.setLen(L);
        ap3.setMu(mu);
    }
    inline float process(float x) {
        return ap3.process(intd.process(x));
    }
    void reset() { intd.reset(); ap3.reset(); }
};

} // namespace afx
