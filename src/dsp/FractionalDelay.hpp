#pragma once
#include <algorithm>
#include <vector>
#include <cmath>

namespace afx {

// Thiran all-pass di ordine 3, realizzazione diretta (DF-II Transposed).
// Modella H(z) = z^{-3} A(z^{-1}) / A(z), con A(z)=1 + a1 z^{-1} + a2 z^{-2} + a3 z^{-3}
struct ThiranAP3 {
    float mu{0.5f};          // frazione in [0,1)
    float a1{0.f}, a2{0.f}, a3{0.f};
    // stati DF-II-T
    float s1{0.f}, s2{0.f}, s3{0.f};

    void setMu(float mu_) {
        // clamp e formule chiuse (J.O. Smith, N=3 con Δ = 3 + mu)
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
        return y; // include z^{-3} interno
    }

    void reset() { s1 = s2 = s3 = 0.f; }
};

// Ritardo frazionario con ordine N=3: parte intera gestita da ring buffer, parte frazionaria da ThiranAP3.
struct FractionalDelay3 {
    void setDelay(float totalDelay) {
        const float d = std::max(totalDelay, 0.0f);
        const float M = std::floor(d);
        const float frac = d - M; // μ ∈ [0,1)
        int newL = static_cast<int>(M) - 3;
        if (newL < 0) {
            newL = 0;
        }
        if (newL != L || ring.empty()) {
            L = newL;
            ring.assign(static_cast<size_t>(L > 0 ? L : 0), 0.0f);
            writeIndex = 0;
        }
        ap.setMu(frac);
    }

    inline float process(float x) {
        float delayed = x;
        if (L > 0) {
            if (ring.empty()) {
                ring.assign(static_cast<size_t>(L), 0.0f);
                writeIndex = 0;
            }
            float y = ring[writeIndex];
            ring[writeIndex] = x;
            writeIndex = (writeIndex + 1) % ring.size();
            delayed = y;
        }
        return ap.process(delayed);
    }

    void reset() {
        ap.reset();
        std::fill(ring.begin(), ring.end(), 0.0f);
        writeIndex = 0;
    }

private:
    ThiranAP3 ap{};
    std::vector<float> ring;
    size_t writeIndex{0};
    int L{0};
};

} // namespace afx
