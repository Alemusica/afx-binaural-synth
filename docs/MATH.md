# Matematica (sintesi HRTF)

## ITD (Woodworth, sfera rigida)
\[ \tau(\theta) = \frac{a}{c}\left(\theta + \sin\theta\right) \]
Campioni: \( n = \tau \cdot f_s \). Applicato come ritardo frazionario Thiran.

## Ritardo frazionario (Thiran, ordine 3)
Usiamo un all-pass di ordine 3 che approssima una group delay piatta vicino a DC.
Definiamo il ritardo desiderato per ear come \(D_{tot}\) in campioni e poniamo:
\[
 D_{tot} = M + \mu, \quad M = \lfloor D_{tot} \rfloor, \quad \mu \in [0,1).
\]
Realizziamo:
1) ritardo intero di lunghezza \(L = \max(M-3, 0)\);
2) allpass Thiran con \(\Delta = 3 + \mu\).

Per \(N=3\), i coefficienti di \(A(z)=1+a_1 z^{-1}+a_2 z^{-2}+a_3 z^{-3}\) sono:
\[
 a_1 = -\frac{3\mu}{\mu+4},\qquad
 a_2 = \frac{3\mu(\mu+1)}{(\mu+4)(\mu+5)},\qquad
 a_3 = -\frac{\mu(\mu+1)(\mu+2)}{(\mu+4)(\mu+5)(\mu+6)}.
\]
L'allpass `\mu` è configurato in forma DF-II trasposta e include lo shift \(z^{-3}\).

## ILD (head shadow semplificato)
Guadagno direzione-dipendente: \( g_L = 1 + k\cos\theta \), \( g_R = 1 + k\cos(-\theta) \).
(Sostituire con modello migliore se disponibile.)

## Pinna
Notch filters (biquad) con frequenze/Q/gain funzione di elevazione (placeholder). Vedi `data/pinna_defaults.json`.
