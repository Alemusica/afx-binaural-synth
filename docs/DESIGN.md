
# DESIGN

## Obiettivi
- Binaurale sintetico **senza BRIR**: calcolo coeff. **on-the-fly** a partire da posa (az, el, dist).
- Throughput elevato su Apple Silicon sfruttando **GPU** per parallelo per sorgente.

## Componenti
- **ITD**: ritardo frazionario (Thiran ordine 3).
- **ILD/head-shadow**: guadagno direzione-dipendente + eventuale shelf/all-pass.
- **Pinna**: 3–6 notches (biquad) funzione di elevazione (e opz. azimut).
- **Scheduler**: batch per blocco audio, riduzione su ear e i/o L/R.

## Estensioni
- Near-field ILD più accurato, differenziazione L/R della pinna, crossfade coeff. al movimento.
- Head-tracking: rotazione delle direzioni per frame.
