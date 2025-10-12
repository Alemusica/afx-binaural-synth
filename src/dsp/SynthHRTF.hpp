
#pragma once
#include <cmath>
#include "Params.hpp"

namespace afx {

constexpr float kSpeedOfSound = 343.0f;
constexpr float kHeadRadiusDefault = 0.088f; // m

inline float deg2rad(float d) { return d * 3.14159265358979323846f / 180.f; }

// ITD Woodworth (far-field) per sfera rigida — semplificato
inline void computeITD(float sr, float head_radius_m, float az_deg, float& itdL, float& itdR) {
    float az = deg2rad(az_deg);
    float c = kSpeedOfSound;
    float a = head_radius_m;
    float tau = (a / c) * (az + std::sin(az)); // s
    float samples = tau * sr;
    // Consegna ritardi additivi: ipsilaterale più corto, controlaterale più lungo.
    float half = 0.5f * samples;
    itdL = half;
    itdR = -half;
}

// ILD/head-shadow semplice: fattore di guadagno funzione di azimut
inline void computeILD(float az_deg, float& gL, float& gR) {
    float az = deg2rad(az_deg);
    // Modello sinusoidale minimale: l'ILD varia con sin(az) così da favorire
    // l'orecchio ipsilaterale. Il guadagno risultante resta nel range
    // [base-depth, base+depth] = [0.7, 1.3].
    constexpr float depth = 0.3f;
    constexpr float base = 1.0f;
    float delta = depth * std::sin(az);
    gL = base - delta;
    gR = base + delta;
}

// Placeholder pinna: mappa elevazione->3 notches con Q costante
inline PinnaParams defaultPinna(float el_deg) {
    PinnaParams p{};
    p.count = 3;
    float el = (el_deg + 90.f) / 180.f; // 0..1
    p.freq[0] = 3500.f + 2500.f * el;
    p.freq[1] = 7000.f + 3000.f * el;
    p.freq[2] = 10000.f + 3000.f * el;
    for (int i=0;i<p.count;i++) { p.Q[i] = 4.0f; p.gain_db[i] = -6.0f; }
    return p;
}

inline void pinnaToBiquad(const PinnaParams& pinna, float sr, EarCoeffs& ear) {
    ear.nbiquad = 0;
    for (int i=0;i<pinna.count && ear.nbiquad<8; ++i) {
        float f0 = pinna.freq[i];
        float Q = pinna.Q[i];
        float A = std::pow(10.f, pinna.gain_db[i]/40.f); // sqrt(gain)
        float w0 = 2.0f * 3.14159265358979323846f * f0 / sr;
        float alpha = std::sin(w0)/(2.0f*Q);

        float b0 = 1 + alpha/A;
        float b1 = -2 * std::cos(w0);
        float b2 = 1 - alpha/A;
        float a0 = 1 + alpha*A;
        float a1 = -2 * std::cos(w0);
        float a2 = 1 - alpha*A;

        int k = ear.nbiquad++;
        ear.b0[k] = b0/a0; ear.b1[k] = b1/a0; ear.b2[k] = b2/a0;
        ear.a1[k] = a1/a0; ear.a2[k] = a2/a0;
    }
}

inline void buildSynthParams(float sr, float head_radius_m, const SourcePose& pose, SynthParams& sp) {
    computeITD(sr, head_radius_m, pose.az_deg, sp.itdL, sp.itdR);
    computeILD(pose.az_deg, sp.ildL_lin, sp.ildR_lin);
    // Pinna uguale per L/R qui; puoi divergere se vuoi
    auto pin = defaultPinna(pose.el_deg);
    sp.left.nbiquad = sp.right.nbiquad = 0;
    pinnaToBiquad(pin, sr, sp.left);
    pinnaToBiquad(pin, sr, sp.right);
}

} // namespace afx
