#include <cstdio>
#include <cmath>
#include <complex>
#include "../src/dsp/FractionalDelay.hpp"

// Stima group delay a w->0 tramite differenze finite del phase response
static float group_delay_at_zero(const afx::ThiranAP3& ap) {
    // ricostruisce H(e^{jw}) dai coeff., senza iterare il filtro nel tempo
    auto H = [&](float w) {
        float a1 = ap.a1, a2 = ap.a2, a3 = ap.a3;
        std::complex<float> z1 = {std::cos(w), -std::sin(w)};
        std::complex<float> z2 = {std::cos(2*w), -std::sin(2*w)};
        std::complex<float> z3 = {std::cos(3*w), -std::sin(3*w)};
        std::complex<float> num = a3 + a2*z1 + a1*z2 + z3;
        std::complex<float> den = 1.f + a1*z1 + a2*z2 + a3*z3;
        return num/den;
    };
    float h = 1e-4f;
    auto Hp = H(h);
    auto Hm = H(-h);
    auto unwrap = [](float a, float b) {
        float d = b - a;
        if (d >  M_PI) b -= 2*M_PI;
        if (d < -M_PI) b += 2*M_PI;
        return b;
    };
    float ph_p = std::arg(Hp);
    float ph_m = std::arg(Hm);
    ph_p = unwrap(0.f, ph_p);
    ph_m = unwrap(0.f, ph_m);
    return -(ph_p - ph_m)/(2*h);
}

int main() {
    using namespace afx;
    for (float mu : {0.1f, 0.5f, 0.9f}) {
        ThiranAP3 ap;
        ap.setMu(mu);
        float tau0 = group_delay_at_zero(ap);
        // atteso: ~ 3 + mu
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
