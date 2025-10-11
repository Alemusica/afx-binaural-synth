
#pragma once
#include <array>
#include <algorithm>

namespace afx {

// Thiran all-pass, ordine 3 (buon compromesso latenza/errore)
struct ThiranDelay3 {
    float D{0.f}; // ritardo frazionario (campioni), 0..3
    std::array<float, 3> a{{0,0,0}};
    std::array<float, 3> z{{0,0,0}}; // z^-1 states

    void setDelay(float d) {
        // vincola a [0,3)
        D = std::clamp(d, 0.0f, 2.9999f);
        float N = 3.0f;
        // Coefficienti Thiran per ordine 3 (formule standard)
        float D1 = D - 1.0f, D2 = D - 2.0f, D3 = D - 3.0f;
        a[0] = ( -D1 * -D2 * -D3 ) / ( (D) * ( D-1.0f ) * ( D-2.0f ) );
        a[1] = ( 3*D * -D2 * -D3 ) / ( (D+1.0f) * ( D ) * ( D-1.0f ) );
        a[2] = ( 3*D*(D+1.0f) * -D3 ) / ( (D+2.0f) * ( D+1.0f ) * ( D ) );
        // Nota: in pratica è meglio derivare simbolicamente o usare libreria; qui è placeholder.
    }

    inline float process(float x) {
        // catena 3 all-pass di ordine 1 per stabilità (approssimazione)
        float y = x;
        for (int i=0;i<3;i++) {
            float v = y - a[i]*z[i];
            float out = a[i]*v + z[i];
            z[i] = v;
            y = out;
        }
        return y;
    }

    void reset() { z = {{0,0,0}}; }
};

} // namespace afx
