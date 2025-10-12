
#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include "../dsp/Biquad.hpp"
#include "../dsp/FractionalDelay.hpp"
#include "../dsp/SynthHRTF.hpp"
#include "../util/Logging.hpp"

namespace afx {

struct CpuBinauralEngine {
    explicit CpuBinauralEngine(int max_sources=16)
    : poses(max_sources), delaysL(max_sources), delaysR(max_sources),
      pinnaL(max_sources), pinnaR(max_sources),
      lastDelayL(max_sources, std::numeric_limits<float>::quiet_NaN()),
      lastDelayR(max_sources, std::numeric_limits<float>::quiet_NaN()) {}

    void prepare(double sr, int block) {
        sampleRate = (float)sr;
        blockSize = block;
        for (auto& d: delaysL) d.reset();
        for (auto& d: delaysR) d.reset();
        for (auto& p: pinnaL) p.reset();
        for (auto& p: pinnaR) p.reset();
        std::fill(lastDelayL.begin(), lastDelayL.end(), std::numeric_limits<float>::quiet_NaN());
        std::fill(lastDelayR.begin(), lastDelayR.end(), std::numeric_limits<float>::quiet_NaN());
    }

    void setHeadRadius(float r) { headRadius = r; }
    void setPose(int idx, const SourcePose& p) {
        if (idx>=0 && idx<(int)poses.size()) poses[idx] = p;
    }

    // Processa N sorgenti mono -> L/R
    void process(const float** inputs, int nsources, float* outL, float* outR, int n) {
        // azzera out
        for (int i=0;i<n;i++){ outL[i]=0.f; outR[i]=0.f; }

        for (int s=0; s<nsources; ++s) {
            SynthParams sp{};
            buildSynthParams(sampleRate, headRadius, poses[s], sp);
            // aggiorna biquad cascade per ear
            pinnaL[s].count = sp.left.nbiquad;
            pinnaR[s].count = sp.right.nbiquad;
            for (int k=0;k<sp.left.nbiquad;k++) {
                pinnaL[s].s[k].b0 = sp.left.b0[k];
                pinnaL[s].s[k].b1 = sp.left.b1[k];
                pinnaL[s].s[k].b2 = sp.left.b2[k];
                pinnaL[s].s[k].a1 = sp.left.a1[k];
                pinnaL[s].s[k].a2 = sp.left.a2[k];
            }
            for (int k=0;k<sp.right.nbiquad;k++) {
                pinnaR[s].s[k].b0 = sp.right.b0[k];
                pinnaR[s].s[k].b1 = sp.right.b1[k];
                pinnaR[s].s[k].b2 = sp.right.b2[k];
                pinnaR[s].s[k].a1 = sp.right.a1[k];
                pinnaR[s].s[k].a2 = sp.right.a2[k];
            }
            // approssima ITD con Thiran per ear mantenendo il segno del ritardo relativo
            constexpr float kBaseDelay = 3.0f;
            float delayL = kBaseDelay + std::max(sp.itdL, 0.0f);
            float delayR = kBaseDelay + std::max(sp.itdR, 0.0f);
            constexpr float kDelayUpdateThreshold = 1e-4f;
            if (!std::isfinite(lastDelayL[s]) || std::fabs(delayL - lastDelayL[s]) > kDelayUpdateThreshold) {
                delaysL[s].setDelay(delayL);
                lastDelayL[s] = delayL;
            }
            if (!std::isfinite(lastDelayR[s]) || std::fabs(delayR - lastDelayR[s]) > kDelayUpdateThreshold) {
                delaysR[s].setDelay(delayR);
                lastDelayR[s] = delayR;
            }
            const float* x = inputs[s];
            for (int i=0;i<n;i++) {
                float in = x[i];
                float xl = delaysL[s].process(in) * sp.ildL_lin;
                float xr = delaysR[s].process(in) * sp.ildR_lin;
                xl = pinnaL[s].process(xl);
                xr = pinnaR[s].process(xr);
                outL[i] += xl;
                outR[i] += xr;
            }
        }
    }

    int numSources() const { return (int)poses.size(); }

private:
    float sampleRate{48000.f};
    int blockSize{256};
    float headRadius{kHeadRadiusDefault};

    std::vector<SourcePose> poses;
    std::vector<ThiranDelay> delaysL, delaysR;
    std::vector<BiquadCascade> pinnaL, pinnaR;
    std::vector<float> lastDelayL, lastDelayR;
};

} // namespace afx
