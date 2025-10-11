
#include <cstdio>
#include <vector>
#include <cmath>
#include "../src/engine/CpuBinauralEngine.hpp"

int main() {
    using namespace afx;
    const double sr = 48000.0;
    const int N = 1024;
    CpuBinauralEngine eng(4);
    eng.prepare(sr, N);
    eng.setHeadRadius(0.088f);
    SourcePose p; p.az_deg = 60.f; p.el_deg = 20.f; p.dist_m = 1.0f;
    eng.setPose(0, p);

    std::vector<float> in(N), L(N), R(N);
    for (int i=0;i<N;i++) in[i] = std::sin(2*M_PI*440.0*(i/sr));
    const float* ins[1] = { in.data() };
    eng.process(ins, 1, L.data(), R.data(), N);

    // stampa prime 8 samples
    for (int i=0;i<8;i++) {
        std::printf("%d: L=%f R=%f\n", i, L[i], R[i]);
    }
    return 0;
}
