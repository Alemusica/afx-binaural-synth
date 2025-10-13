
#include "GpuBinauralEngine.hpp"
#include "../dsp/SynthHRTF.hpp"
#include "../dsp/Biquad.hpp"
#include "../dsp/FractionalDelay.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace afx {

struct GpuBinauralEngine::Impl {
    // Qui andranno device/queue/pipeline Metal, buffers condivisi ecc.
};

GpuBinauralEngine::GpuBinauralEngine(int max_sources)
: poses(max_sources),
  delaysL(max_sources), delaysR(max_sources),
  pinnaL(max_sources), pinnaR(max_sources),
  lastEarL(max_sources), lastEarR(max_sources),
  lastDelayL(max_sources, std::numeric_limits<float>::quiet_NaN()),
  lastDelayR(max_sources, std::numeric_limits<float>::quiet_NaN()) {
    // Inizializza risorse GPU (deferred per semplicità)
    initialized = true;
}
GpuBinauralEngine::~GpuBinauralEngine() {
    delete impl; impl = nullptr;
}

void GpuBinauralEngine::prepare(double sr, int block) {
    sampleRate = (float)sr; blockSize = block;
    // TODO: creare pipeline Metal, caricare binaural.metallib, allocare buffers
    for (auto& d : delaysL) d.reset();
    for (auto& d : delaysR) d.reset();
    for (auto& p : pinnaL) p.reset();
    for (auto& p : pinnaR) p.reset();
    for (auto& e : lastEarL) e.nbiquad = -1;
    for (auto& e : lastEarR) e.nbiquad = -1;
    std::fill(lastDelayL.begin(), lastDelayL.end(), std::numeric_limits<float>::quiet_NaN());
    std::fill(lastDelayR.begin(), lastDelayR.end(), std::numeric_limits<float>::quiet_NaN());
}
void GpuBinauralEngine::setHeadRadius(float r) { headRadius = r; }
void GpuBinauralEngine::setPose(int idx, const SourcePose& p) {
    if (idx>=0 && idx<(int)poses.size()) poses[idx] = p;
}

void GpuBinauralEngine::process(const float** inputs, int nsources, float* outL, float* outR, int n) {
    // Per ora esegue sintesi su CPU (baseline), poi migrare al kernel Metal
    // Riutilizziamo lo stesso codice del CPU engine inline.
    for (int i=0;i<n;i++){ outL[i]=0.f; outR[i]=0.f; }

    for (int s=0; s<nsources; ++s) {
        SynthParams sp{};
        buildSynthParams(sampleRate, headRadius, poses[s], sp);
        auto updateCascadeIfNeeded = [](const EarCoeffs& src, EarCoeffs& cache, BiquadCascade& cascade) {
            constexpr float kCoeffThreshold = 1e-6f;
            bool changed = cache.nbiquad != src.nbiquad;
            if (!changed) {
                for (int k = 0; k < src.nbiquad; ++k) {
                    if (std::fabs(cache.b0[k] - src.b0[k]) > kCoeffThreshold ||
                        std::fabs(cache.b1[k] - src.b1[k]) > kCoeffThreshold ||
                        std::fabs(cache.b2[k] - src.b2[k]) > kCoeffThreshold ||
                        std::fabs(cache.a1[k] - src.a1[k]) > kCoeffThreshold ||
                        std::fabs(cache.a2[k] - src.a2[k]) > kCoeffThreshold) {
                        changed = true;
                        break;
                    }
                }
            }
            if (changed) {
                cascade.count = src.nbiquad;
                for (int k = 0; k < src.nbiquad; ++k) {
                    cascade.s[k].b0 = src.b0[k];
                    cascade.s[k].b1 = src.b1[k];
                    cascade.s[k].b2 = src.b2[k];
                    cascade.s[k].a1 = src.a1[k];
                    cascade.s[k].a2 = src.a2[k];
                }
                cache = src;
            }
        };
        updateCascadeIfNeeded(sp.left, lastEarL[s], pinnaL[s]);
        updateCascadeIfNeeded(sp.right, lastEarR[s], pinnaR[s]);
        constexpr float kBaseDelay = 3.0f; // mantiene ritardi totali positivi
        float delayL = kBaseDelay + sp.itdL;
        float delayR = kBaseDelay + sp.itdR;
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
            float delayedL = delaysL[s].process(in);
            float delayedR = delaysR[s].process(in);
            float xl = pinnaL[s].process(delayedL) * sp.ildL_lin;
            float xr = pinnaR[s].process(delayedR) * sp.ildR_lin;
            outL[i] += xl;
            outR[i] += xr;
        }
    }
}

} // namespace afx
