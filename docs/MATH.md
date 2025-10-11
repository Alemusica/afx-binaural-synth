
# Matematica (sintesi HRTF)

## ITD (Woodworth, sfera rigida)
\\[ \\tau(\\theta) = \\frac{a}{c}\\left(\\theta + \\sin\\theta\\right) \\]
Campioni: \\( n = \\tau \\cdot f_s \\). Applicato come ritardo frazionario Thiran.

## ILD (head shadow semplificato)
Guadagno direzione-dipendente: \\( g_L = 1 + k\\cos\\theta \\), \\( g_R = 1 + k\\cos(-\\theta) \\).
(Sostituire con modello migliore se disponibile.)

## Pinna
Notch filters (biquad) con frequenze/Q/gain funzione di elevazione (placeholder). Vedi `data/pinna_defaults.json`.
