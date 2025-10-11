
#pragma once
#include <vector>
#include <string>
#include "../dsp/SynthHRTF.hpp"

namespace afx {

struct GpuBinauralEngine {
    explicit GpuBinauralEngine(int max_sources=128);
    ~GpuBinauralEngine();

    void prepare(double sr, int block);
    void setHeadRadius(float r);
    void setPose(int idx, const SourcePose& p);

    // Processa N sorgenti mono -> L/R (stub: per ora copia CPU-like)
    void process(const float** inputs, int nsources, float* outL, float* outR, int n);

    bool ok() const { return initialized; }
    std::string error() const { return lastError; }

private:
    bool initialized{false};
    std::string lastError;
    float sampleRate{48000.f};
    int blockSize{256};
    float headRadius{kHeadRadiusDefault};

    // Placeholder per stati/risorse GPU (device, queue, pipeline, buffers)
    struct Impl;
    Impl* impl{nullptr};

    std::vector<SourcePose> poses;
};

} // namespace afx
