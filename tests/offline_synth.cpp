
#include <cstdio>
#include <vector>
#include <cmath>
#include "../src/engine/CpuBinauralEngine.hpp"

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
    SourcePose p; p.az_deg = 60.f; p.el_deg = 20.f; p.dist_m = 1.0f;
    eng.setPose(0, p);

    std::vector<float> in(N, 0.0f), L(N), R(N);
    in[0] = 1.0f; // impulso per misurare i tempi di arrivo
    const float* ins[1] = { in.data() };
    eng.process(ins, 1, L.data(), R.data(), N);

    int firstL = firstNonZeroIndex(L, 1e-4f);
    int firstR = firstNonZeroIndex(R, 1e-4f);
    if (firstL < 0 || firstR < 0) {
        std::fprintf(stderr, "Segnale non rilevato all'uscita.\n");
        return 1;
    }
    if (firstL == firstR) {
        std::fprintf(stderr, "I tempi di arrivo iniziali coincidono inaspettatamente.\n");
        return 1;
    }

    int lag = estimateLag(L, R, 64);
    if (lag <= 0) {
        std::fprintf(stderr, "La correlazione massima non evidenzia un ritardo destro positivo (lag=%d).\n", lag);
        return 1;
    }

    std::printf("Primo campione L=%d, R=%d, lag stimato=%d\n", firstL, firstR, lag);
    return 0;
}
