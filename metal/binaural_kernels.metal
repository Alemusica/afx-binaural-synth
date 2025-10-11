
#include <metal_stdlib>
using namespace metal;

// Placeholder: kernel per futura elaborazione parallela sorgenti×campioni.
// Al momento non utilizzato dal path CPU; mantenuto per accelerazione futura.

struct BiquadCoeffs { float b0,b1,b2,a1,a2; };
struct SourceParams {
    int nbiquad;
    float ildL, ildR;
    float itdL, itdR;
};

kernel void process_sources(
    device const float *in,                 // [S*N]
    device float *outL,
    device float *outR,
    device const BiquadCoeffs *biquadsL,   // [S*maxBq]
    device const BiquadCoeffs *biquadsR,   // [S*maxBq]
    device const SourceParams *params,     // [S]
    constant int &N,                        // samples per block
    constant int &S,                        // num sources
    uint gid [[thread_position_in_grid]]
){
    // TODO: implementare pipeline su GPU
    // Questo file resta come placeholder per creare la metallib e agganciare la pipeline.
    if (gid < N) {
        outL[gid] = in[gid];
        outR[gid] = in[gid];
    }
}
