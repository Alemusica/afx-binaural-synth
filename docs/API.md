
# API (Max)

Oggetto: `afx.binaural~`

## Attributi
- `@sources <int>` (TODO: attualmente fisso a 1 inlet)
- `@mode <cpu|gpu>`

## Messaggi
- `pos <src> <az_deg> <el_deg> <dist_m>`
- `mode <cpu|gpu>`
- `reset` (TODO)

## I/O
- Ingressi: N segnali mono (una sorgente per inlet).
- Uscite: 2 segnali (L, R).
