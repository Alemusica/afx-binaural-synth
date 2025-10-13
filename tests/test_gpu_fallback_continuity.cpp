#include <vector>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "../src/engine/GpuBinauralEngine.hpp"

int main() {
    using namespace afx;

    const double sampleRate = 48000.0;
    const int blockSize = 256;
    const int numBlocks = 6;
    const int totalSamples = blockSize * numBlocks;

    std::vector<float> input(totalSamples);
    const float frequency = 440.0f;
    const float twoPi = 6.28318530717958647692f;
    float phase = 0.0f;
    const float phaseInc = twoPi * frequency / static_cast<float>(sampleRate);
    for (int i = 0; i < totalSamples; ++i) {
        input[i] = std::sin(phase);
        phase += phaseInc;
        if (phase >= twoPi) phase -= twoPi;
    }

    SourcePose pose;
    pose.az_deg = 25.f;
    pose.el_deg = 15.f;
    pose.dist_m = 1.0f;

    // Reference: singola chiamata con tutto il segnale.
    std::vector<float> referenceL(totalSamples), referenceR(totalSamples);
    GpuBinauralEngine fullEngine(1);
    fullEngine.prepare(sampleRate, totalSamples);
    fullEngine.setHeadRadius(0.088f);
    fullEngine.setPose(0, pose);
    const float* fullInputPtr[1] = { input.data() };
    fullEngine.process(fullInputPtr, 1, referenceL.data(), referenceR.data(), totalSamples);

    // Percorso testato: più blocchi consecutivi senza reset esplicito.
    std::vector<float> blockL(totalSamples), blockR(totalSamples);
    GpuBinauralEngine blockEngine(1);
    blockEngine.prepare(sampleRate, blockSize);
    blockEngine.setHeadRadius(0.088f);
    blockEngine.setPose(0, pose);

    for (int b = 0; b < numBlocks; ++b) {
        const float* blockInputPtr[1] = { input.data() + b * blockSize };
        blockEngine.process(blockInputPtr, 1,
                            blockL.data() + b * blockSize,
                            blockR.data() + b * blockSize,
                            blockSize);
    }

    auto compareBuffers = [](const std::vector<float>& a, const std::vector<float>& b) {
        float maxDiff = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            maxDiff = std::max(maxDiff, std::fabs(a[i] - b[i]));
        }
        return maxDiff;
    };

    const float maxDiffL = compareBuffers(referenceL, blockL);
    const float maxDiffR = compareBuffers(referenceR, blockR);
    const float tolerance = 1e-5f;

    if (maxDiffL > tolerance || maxDiffR > tolerance) {
        std::fprintf(stderr,
                     "Controllo continuit\xC3\xA0 fallito: diffL=%.6e diffR=%.6e (tol=%.1e)\n",
                     maxDiffL, maxDiffR, tolerance);
        return 1;
    }

    std::printf("GPU fallback continuity OK: diffL=%.6e diffR=%.6e\n", maxDiffL, maxDiffR);
    return 0;
}
