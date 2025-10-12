
#include <cstdio>
#include <vector>
#include <algorithm>
#include <cmath>
#include "../src/engine/CpuBinauralEngine.hpp"
#include "../src/dsp/SynthHRTF.hpp"

namespace {

int firstNonZeroIndex(const std::vector<float>& buffer, float threshold) {
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (std::fabs(buffer[i]) > threshold) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int estimateLag(const std::vector<float>& reference, const std::vector<float>& shifted, int maxLag) {
    float bestValue = -1.0f;
    int bestLag = 0;
    for (int lag = -maxLag; lag <= maxLag; ++lag) {
        float sum = 0.0f;
        for (int n = 0; n < static_cast<int>(reference.size()); ++n) {
            int j = n + lag;
            if (j < 0 || j >= static_cast<int>(shifted.size())) {
                continue;
            }
            sum += reference[n] * shifted[j];
        }
        float absSum = std::fabs(sum);
        if (absSum > bestValue) {
            bestValue = absSum;
            bestLag = lag;
        }
    }
    return bestLag;
}

}

int main() {
    using namespace afx;
    const double sr = 48000.0;
    const int N = 1024;
    CpuBinauralEngine eng(4);
    eng.prepare(sr, N);
    eng.setHeadRadius(0.088f);
    SourcePose poseTemplate; poseTemplate.el_deg = 20.f; poseTemplate.dist_m = 1.0f;

    auto checkImpulseLead = [&](float azimuth, bool expectRightLead) -> bool {
        SourcePose pose = poseTemplate;
        pose.az_deg = azimuth;
        eng.setPose(0, pose);

        SynthParams params{};
        buildSynthParams(sr, 0.088f, pose, params);
        if (std::fabs(params.ildL_lin - params.ildR_lin) < 1e-5f) {
            std::fprintf(stderr, "ILD identici per azimut %.1f°.\n", azimuth);
            return false;
        }

        std::vector<float> in(N, 0.0f), L(N), R(N);
        in[0] = 1.0f; // impulso per misurare i tempi di arrivo
        const float* ins[1] = { in.data() };
        eng.process(ins, 1, L.data(), R.data(), N);

        int firstL = firstNonZeroIndex(L, 1e-4f);
        int firstR = firstNonZeroIndex(R, 1e-4f);
        if (firstL < 0 || firstR < 0) {
            std::fprintf(stderr, "Segnale non rilevato all'uscita (az=%.1f°).\n", azimuth);
            return false;
        }
        if (firstL == firstR) {
            std::fprintf(stderr, "I tempi di arrivo coincidono inaspettatamente (az=%.1f°).\n", azimuth);
            return false;
        }

        int lag = estimateLag(L, R, 64);
        if (expectRightLead) {
            if (firstR > firstL) {
                std::fprintf(stderr, "Il canale destro non anticipa (az=%.1f°).\n", azimuth);
                return false;
            }
            if (lag >= 0) {
                std::fprintf(stderr, "La correlazione non mostra lag negativo per R (lag=%d, az=%.1f°).\n", lag, azimuth);
                return false;
            }
        } else {
            if (firstL > firstR) {
                std::fprintf(stderr, "Il canale sinistro non anticipa (az=%.1f°).\n", azimuth);
                return false;
            }
            if (lag <= 0) {
                std::fprintf(stderr, "La correlazione non mostra lag positivo per R (lag=%d, az=%.1f°).\n", lag, azimuth);
                return false;
            }
        }

        std::printf("az=%5.1f° -> primo L=%d, primo R=%d, lag=%d\n", azimuth, firstL, firstR, lag);
        return true;
    };

    if (!checkImpulseLead(60.f, true)) {
        return 1;
    }
    if (!checkImpulseLead(-60.f, false)) {
        return 1;
    }

    // Test di continuità dell'ampiezza su blocchi consecutivi
    const int blockSize = 256;
    const int numBlocks = 5;
    const int warmupBlocks = 2;
    eng.prepare(sr, blockSize);
    SourcePose steadyPose = poseTemplate;
    steadyPose.az_deg = 30.f;
    eng.setPose(0, steadyPose);
    std::vector<float> blockIn(blockSize), blockOutL(blockSize), blockOutR(blockSize);
    std::vector<float> peakL(numBlocks, 0.0f), peakR(numBlocks, 0.0f);
    const float frequency = 440.0f;
    const float twoPi = 6.28318530717958647692f;
    float phase = 0.0f;
    const float phaseInc = twoPi * frequency / static_cast<float>(sr);
    for (int b = 0; b < numBlocks; ++b) {
        for (int i = 0; i < blockSize; ++i) {
            blockIn[i] = std::sin(phase);
            phase += phaseInc;
            if (phase >= twoPi) phase -= twoPi;
        }
        const float* blockPtr[1] = { blockIn.data() };
        eng.process(blockPtr, 1, blockOutL.data(), blockOutR.data(), blockSize);
        float maxAbsL = 0.0f;
        float maxAbsR = 0.0f;
        for (int i = 0; i < blockSize; ++i) {
            maxAbsL = std::max(maxAbsL, std::fabs(blockOutL[i]));
            maxAbsR = std::max(maxAbsR, std::fabs(blockOutR[i]));
        }
        peakL[b] = maxAbsL;
        peakR[b] = maxAbsR;
    }
    const float amplitudeTolerance = 5e-3f;
    for (int b = warmupBlocks + 1; b < numBlocks; ++b) {
        if (std::fabs(peakL[b] - peakL[b - 1]) > amplitudeTolerance) {
            std::fprintf(stderr, "Ampiezza instabile L sui blocchi %d/%d: %.6f vs %.6f\n",
                         b - 1, b, peakL[b - 1], peakL[b]);
            return 1;
        }
        if (std::fabs(peakR[b] - peakR[b - 1]) > amplitudeTolerance) {
            std::fprintf(stderr, "Ampiezza instabile R sui blocchi %d/%d: %.6f vs %.6f\n",
                         b - 1, b, peakR[b - 1], peakR[b]);
            return 1;
        }
    }

    return 0;
}
