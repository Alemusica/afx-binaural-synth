
# Matematica (sintesi HRTF)

## ITD (Woodworth, sfera rigida)
\\[ \\tau(\\theta) = \\frac{a}{c}\\left(\\theta + \\sin\\theta\\right) \\]
Campioni: \\( n = \\tau \\cdot f_s \\). Applicato come ritardo frazionario Thiran.

## Ritardo frazionario (Thiran, ordine 3)
Usiamo un all-pass di ordine 3 con risposta in fase quasi-lineare in banda bassa.
Dato un ritardo totale desiderato \( D_{\text{tot}} \) in campioni:

1. Decomponiamo \( D_{\text{tot}} = M + \mu \) con \( M = \lfloor D_{\text{tot}} \rfloor \) e \( \mu \in [0,1) \).
2. Applichiamo un ritardo intero \( L = \max(M - 3, 0) \) tramite buffer circolare.
3. Configuriamo l'all-pass Thiran con \( \Delta = 3 + \mu \).

Per \(N = 3\) i coefficienti di \(A(z) = 1 + a_1 z^{-1} + a_2 z^{-2} + a_3 z^{-3}\) sono:
\[
 a_1 = -\frac{3\mu}{\mu + 4},\qquad
 a_2 = \frac{3\mu(\mu + 1)}{(\mu + 4)(\mu + 5)},\qquad
 a_3 = -\frac{\mu(\mu + 1)(\mu + 2)}{(\mu + 4)(\mu + 5)(\mu + 6)}.
\]
La realizzazione usa una struttura DF-II trasposta; lo sfasamento \( z^{-3} \) è incorporato nell'all-pass.

## ILD (head shadow semplificato)
Guadagno direzione-dipendente: \\( g_L = 1 + k\\cos\\theta \\), \\( g_R = 1 + k\\cos(-\\theta) \\).
(Sostituire con modello migliore se disponibile.)

## Pinna
Notch filters (biquad) con frequenze/Q/gain funzione di elevazione (placeholder). Vedi `data/pinna_defaults.json`.
