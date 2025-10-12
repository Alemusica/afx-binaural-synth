#include <cstdio>
#include <cmath>
#include <complex>
#include "../src/dsp/FractionalDelay.hpp"

// Stima la group delay vicino a w -> 0 tramite differenza finita della fase
static float group_delay_at_zero(const afx::ThiranAP3& ap) {
    auto H = [&](float w) {
        const float a1 = ap.a1;
        const float a2 = ap.a2;
        const float a3 = ap.a3;
        std::complex<float> z1{std::cos(w), -std::sin(w)};
        std::complex<float> z2{std::cos(2.f * w), -std::sin(2.f * w)};
        std::complex<float> z3{std::cos(3.f * w), -std::sin(3.f * w)};
        std::complex<float> num = a3 + a2 * z1 + a1 * z2 + z3;
        std::complex<float> den = 1.f + a1 * z1 + a2 * z2 + a3 * z3;
        return num / den;
    };
    const float h = 1e-4f;
    auto Hp = H(h);
    auto Hm = H(-h);
    constexpr float pi = 3.14159265358979323846f;
    auto unwrap = [pi](float ref, float p) {
        float d = p - ref;
        if (d >  pi) p -= 2.f * pi;
        if (d < -pi) p += 2.f * pi;
        return p;
    };
    float ph_p = std::arg(Hp);
    float ph_m = std::arg(Hm);
    ph_p = unwrap(0.f, ph_p);
    ph_m = unwrap(0.f, ph_m);
    return -(ph_p - ph_m) / (2.f * h);
}

int main() {
    using namespace afx;
    for (float mu : {0.1f, 0.5f, 0.9f}) {
        ThiranAP3 ap;
        ap.setMu(mu);
        float tau0 = group_delay_at_zero(ap);
        float expected = 3.f + mu;
        float err = std::fabs(tau0 - expected);
        std::printf("mu=%.2f  tau0=%.5f  expected=%.5f  err=%.5f\n", mu, tau0, expected, err);
        if (err > 0.05f) {
            std::fprintf(stderr, "FAIL: group delay out of tolerance\n");
            return 1;
        }
    }
    std::puts("OK");
    return 0;
}
