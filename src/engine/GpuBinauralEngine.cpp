
#include "GpuBinauralEngine.hpp"
#include "../dsp/SynthHRTF.hpp"
#include "../dsp/Biquad.hpp"
#include "../dsp/FractionalDelay.hpp"

namespace afx {

struct GpuBinauralEngine::Impl {
    // Qui andranno device/queue/pipeline Metal, buffers condivisi ecc.
};

GpuBinauralEngine::GpuBinauralEngine(int max_sources) : poses(max_sources) {
    // Inizializza risorse GPU (deferred per semplicità)
    initialized = true;
}
GpuBinauralEngine::~GpuBinauralEngine() {
    delete impl; impl = nullptr;
}

void GpuBinauralEngine::prepare(double sr, int block) {
    sampleRate = (float)sr; blockSize = block;
    // TODO: creare pipeline Metal, caricare binaural.metallib, allocare buffers
}
void GpuBinauralEngine::setHeadRadius(float r) { headRadius = r; }
void GpuBinauralEngine::setPose(int idx, const SourcePose& p) {
    if (idx>=0 && idx<(int)poses.size()) poses[idx] = p;
}

void GpuBinauralEngine::process(const float** inputs, int nsources, float* outL, float* outR, int n) {
    // Per ora esegue sintesi su CPU (baseline), poi migrare al kernel Metal
    // Riutilizziamo lo stesso codice del CPU engine inline.
    for (int i=0;i<n;i++){ outL[i]=0.f; outR[i]=0.f; }

    BiquadCascade pinnaL, pinnaR;
    ThiranDelay3 dL, dR;

    for (int s=0; s<nsources; ++s) {
        SynthParams sp{};
        buildSynthParams(sampleRate, headRadius, poses[s], sp);
        // carica filtri
        pinnaL.count = sp.left.nbiquad;
        pinnaR.count = sp.right.nbiquad;
        for (int k=0;k<sp.left.nbiquad;k++) {
            pinnaL.s[k].b0 = sp.left.b0[k];
            pinnaL.s[k].b1 = sp.left.b1[k];
            pinnaL.s[k].b2 = sp.left.b2[k];
            pinnaL.s[k].a1 = sp.left.a1[k];
            pinnaL.s[k].a2 = sp.left.a2[k];
        }
        for (int k=0;k<sp.right.nbiquad;k++) {
            pinnaR.s[k].b0 = sp.right.b0[k];
            pinnaR.s[k].b1 = sp.right.b1[k];
            pinnaR.s[k].b2 = sp.right.b2[k];
            pinnaR.s[k].a1 = sp.right.a1[k];
            pinnaR.s[k].a2 = sp.right.a2[k];
        }
        dL.setDelay(std::fabs(sp.itdL)); dR.setDelay(std::fabs(sp.itdR));
        const float* x = inputs[s];
        for (int i=0;i<n;i++) {
            float in = x[i];
            float xl = pinnaL.process(dL.process(in)) * sp.ildL_lin;
            float xr = pinnaR.process(dR.process(in)) * sp.ildR_lin;
            outL[i] += xl;
            outR[i] += xr;
        }
    }
}

} // namespace afx
