
# Matematica (sintesi HRTF)

## ITD (Woodworth, sfera rigida)
\\[ \\tau(\\theta) = \\frac{a}{c}\\left(\\theta + \\sin\\theta\\right) \\]
Campioni: \\( n = \\tau \\cdot f_s \\). Applicato come ritardo frazionario Thiran.

### Ritardo frazionario Thiran (N = 3)
Dato un ritardo desiderato totale \( D_{\text{tot}} \) (in campioni) per ciascun orecchio:

1. Scomponi \( D_{\text{tot}} = M + \mu \) con \( M = \lfloor D_{\text{tot}} \rfloor \) e \( \mu \in [0,1) \).
2. Applica un ritardo intero \( L = M - 3 \) campioni tramite ring buffer.
3. Configura l'all-pass Thiran di ordine 3 con \( \mu \) (ossia \( \Delta = 3 + \mu \)).

I coefficienti risultano:
\[
\begin{aligned}
a_1 &= -\frac{3\mu}{\mu + 4},\\
a_2 &= \frac{3\mu(\mu + 1)}{(\mu + 4)(\mu + 5)},\\
a_3 &= -\frac{\mu(\mu + 1)(\mu + 2)}{(\mu + 4)(\mu + 5)(\mu + 6)}.
\end{aligned}
\]
La realizzazione usa una struttura DF-II trasposta: gli stati sono inizializzati a zero e la parte \( z^{-3} \) è inclusa nell'all-pass.

## ILD (head shadow semplificato)
Guadagno direzione-dipendente: \\( g_L = 1 + k\\cos\\theta \\), \\( g_R = 1 + k\\cos(-\\theta) \\).
(Sostituire con modello migliore se disponibile.)

## Pinna
Notch filters (biquad) con frequenze/Q/gain funzione di elevazione (placeholder). Vedi `data/pinna_defaults.json`.
