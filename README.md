
# AFX Binaural Synth (Brown–Duda style) — Max External + Metal (Apple Silicon)

**Obiettivo**: external `afx.binaural~` per Max/MSP che esegue **binauralizzazione sintetica** (no BRIR) usando modelli parametrici
(Brown & Duda e varianti moderne a basso ordine: ITD/ILD, head-shadow, pinna con pochi biquad) con **parallelismo massivo su GPU Apple Silicon** (Metal).
CPU fallback incluso.

> Stato: **skeleton funzionante** per compilazione e prototipo. Le parti DSP sono modulari e pronte a essere riempite con i tuoi coefficienti/tabelle.

## Caratteristiche
- Modello **parametrico**: ITD (ritardo frazionario Thiran), head-shadow (shelf/all-pass), **pinna** con 3–6 biquad dipendenti da direzione.
- **GPU path (Metal)** per molte sorgenti in parallelo, con memoria unificata.
- **CPU fallback** per test/CI e benchmarking offline.
- Architettura **fattorizzata**: `max` (hosting), `engine` (GPU/CPU), `dsp` (matematica/filtri), `metal` (kernel), `util`, `data`.
- Repo pronto per assistenti (Codex/AI): `AGENT.md`, `DESIGN.md`, `PIPELINE.md`, `PARAMS.md` e `API.md`.

## Build rapido (macOS + Xcode + Max SDK)
1. **Prerequisiti**
   - Xcode 15+ su Apple Silicon
   - Max 8.5+
   - Max SDK clonato o scaricato (imposta `MAX_SDK_DIR` all'interno del repo SDK):
     ```bash
     export MAX_SDK_DIR=/percorso/al/max-sdk
     ```
2. **Compila con CMake (Generator Xcode)**
   ```bash
   ./scripts/bootstrap.sh   # verifica requisiti
   ./scripts/build.sh       # genera Xcode project e builda
   ```
   Il bundle risultante sarà in `build/Release/afx.binaural~.mxo`.

3. **Installa nell'Packages** (facoltativo)
   Copia `afx.binaural~.mxo` in `~/Documents/Max 8/Packages/afx-binaural/externals/`.

4. **Esempio Max**
   Apri `examples/max/README.md` e/o il patcher per un test rapido.

## Struttura del progetto
```
afx-binaural-synth/
├── CMakeLists.txt
├── cmake/MaxSDK.cmake
├── src/
│   ├── max/afx.binaural~.cpp
│   ├── engine/{GpuBinauralEngine,CpuBinauralEngine}.(hpp|cpp)
│   ├── dsp/{Biquad.hpp,FractionalDelay.hpp,SynthHRTF.hpp,Params.hpp}
│   ├── util/{RingBuffer.hpp,Logging.hpp}
│   └── data/ (tabelle)
├── metal/binaural_kernels.metal
├── data/pinna_defaults.json
├── tests/offline_synth.cpp
├── scripts/{bootstrap.sh,build.sh}
├── docs/{PIPELINE.md,DESIGN.md,API.md,MATH.md,PARAMS.md}
├── examples/max/
│   ├── afx-binaural.maxpat
│   └── README.md
├── AGENT.md, CONTRIBUTING.md, CODE_OF_CONDUCT.md, LICENSE, README.md
└── .github/workflows/macos-build.yml
```

## Uso (oggetto Max)
```
[afx.binaural~ @sources 4 @mode gpu @head_radius 0.088]
```
- Ingressi: `N` inlets mono (una sorgente per inlet); Uscite: L/R.
- Messaggi:
  - `pos <src> <az_deg> <el_deg> <dist_m>` — Aggiorna posa sorgente
  - `mode <cpu|gpu>` — Cambia motore
  - `pinna <src> <n1_freq> <n1_Q> ...` — Override notches pinna
  - `reset` — Reset stati filtri

## Avvertenze
- Questo è un **scheletro**: i mapping direzione→notch in `data/pinna_defaults.json` sono placeholder. Sostituiscili con i tuoi.
- Il kernel Metal è pronto ma minimale; l'engine CPU è utile per validare la pipeline e i coefficienti prima di accelerare tutto sulla GPU.
