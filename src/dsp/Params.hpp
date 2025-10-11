
#pragma once
#include <cstdint>
#include <vector>

namespace afx {

struct SourcePose {
    float az_deg{0.f};
    float el_deg{0.f};
    float dist_m{1.f};
};

struct PinnaParams {
    // Fino a 6 notches: freq [Hz], Q, gain [dB] (gain < 0 per notch)
    int count{0};
    float freq[6]{};
    float Q[6]{};
    float gain_db[6]{};
};

struct EarCoeffs {
    // Biquad cascade per ear
    int nbiquad{0};
    float b0[8]{};
    float b1[8]{};
    float b2[8]{};
    float a1[8]{};
    float a2[8]{};
    // Shelf/tilt opzionali
    float gain_lin{1.f};
};

struct SynthParams {
    // Parametri sintetici per una sorgente (per blocco)
    float itdL{0.f}; // campioni (ritardo relativo per L)
    float itdR{0.f}; // campioni (ritardo relativo per R)
    float ildL_lin{1.f};
    float ildR_lin{1.f};
    EarCoeffs left;
    EarCoeffs right;
};

} // namespace afx
