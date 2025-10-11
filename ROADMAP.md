
# ROADMAP — AFX Binaural Synth (Brown–Duda style)

> **Sintesi**: external `afx.binaural~` per Max/MSP che esegue binauralizzazione **sintetica** (no BRIR) su **CPU/GPU (Metal)**.
> Questa roadmap definisce attività, deliverable, test e criteri di accettazione (QA) per ogni fase.

## Principi
- **Real-time safe**: nessuna allocazione nel perform, latenza stabile con block 128/256 @ 48 kHz.
- **Determinismo**: CPU e GPU devono produrre risultati equivalenti (entro una tolleranza).
- **Fattorizzazione**: math/DSP separati da engine e glue Max.
- **Tracciabilità**: ogni commit deve riferire i task (`[Txx]`) e aggiornare `COMMIT_LOG.md` (vedi AGENT.md).

## Definition of Done (DoD) — generale
- ✅ Compila in **Release** con Xcode (Apple Silicon) e con **clang++** per i test offline.
- ✅ Test unitari/integrazione **passano** (vedi sotto).
- ✅ QA minima: niente click/glitch muovendo le sorgenti; output L/R in fase attesa.
- ✅ Documentazione aggiornata (`README`, `API`, `MATH`, eventuali snippet in `DESIGN`).
- ✅ `COMMIT_LOG.md` aggiornato (task, test eseguiti, esito QA).

---

## Milestone M0 — “CPU solida + API pulita”
**Obiettivo**: pipeline CPU affidabile, API Max definite, qualità percepita credibile.

### T01 · Thiran Delay corretto
- **File**: `src/dsp/FractionalDelay.hpp`
- **Deliverable**: implementazione Thiran ordine 3 o 4 con coeff. corretti e `reset()` stabile.
- **Test**:
  - Unit: sweep di ritardi 0..3 campioni; errore di fase < 2° fino a 8 kHz @ 48 kHz.
  - Integr.: confronto di un tono 1 kHz e 6 kHz col delay frazionario vs reference analitica.
- **QA**:
  - Nessun artefatto su rumore bianco e sweep quando si cambia ritardo lentamente (crossfade on-change).

### T02 · ILD/Head-Shadow frequenza‑dipendente
- **File**: `src/dsp/SynthHRTF.hpp::computeILD`
- **Deliverable**: 1–2 biquad shelf/all-pass per emulare attenuazione controlaterale alle alte freq.
- **Test**:
  - Unit: Bode magnitude: ILD ≈ 0 dB a basse freq; 5–15 dB @ 6–10 kHz sul lato controlaterale.
  - Integr.: confronto L/R su sweep log; curva ILD calcolata dal segnale corrisponde (±2 dB).
- **QA**: timbrica non eccessivamente scura/chiara; ascolto A/B vs baseline.

### T03 · Pinna notches calibrati
- **File**: `data/pinna_defaults.json`, `SynthHRTF.hpp::pinnaToBiquad`
- **Deliverable**: LUT per 3–6 notches con (freq,Q,gain) vs **elevazione** (e opz. azimut) — anche L/R distinti.
- **Test**:
  - Unit: verifica stabilità biquad (poli dentro unit circle).
  - Integr.: sweep verticale → notches si spostano nella banda 3–12 kHz come da LUT.
- **QA**: localizzazione verticale percepita e assenza di forte colorazione.

### T04 · Crossfade parametri
- **File**: `src/engine/CpuBinauralEngine.hpp`
- **Deliverable**: interpolazione coeff. (20–50 ms) quando az/el/dist cambiano.
- **Test**:
  - Integr.: movimento continuo dell’azimut su 2 s: nessun click, nessun salto di timbro.
- **QA**: “smoothness” percepita; latenza invariata.

### T05 · Multi‑sorgente & API Max
- **File**: `src/max/afx.binaural~.cpp`
- **Deliverable**: attributo `@sources <N>` → N inlets; messaggi `pinna`, `head_radius`, `reset`.
- **Test**:
  - Integr.: 8 sorgenti indipendenti; livelli coerenti; nessuna contaminazione tra canali.
- **QA**: patch di esempio aggiornata e documentata.

---

## Milestone M1 — “GPU reale (Metal)”
**Obiettivo**: portare pipeline su GPU Apple Silicon, parità numerica con CPU entro tolleranza.

### T06 · Infrastruttura Metal
- **File**: `src/engine/GpuBinauralEngine.cpp` (struct `Impl`)
- **Deliverable**: `MTLDevice`, `MTLCommandQueue`, caricamento `binaural.metallib`, `MTLBuffers` shared.
- **Test**: init senza errori; `ok()` true; metallib presente nel bundle.
- **QA**: log di init chiaro e silenzio in DSP se GPU non disponibile (fallback).

### T07 · Kernel: ritardo frazionario + cascata biquad + ILD
- **File**: `metal/binaural_kernels.metal`
- **Deliverable**: kernel per **sorgente × ear** che elabora un blocco (128/256) con stato (DF‑II T + all-pass).
- **Test**:
  - Unit: con 1 sorgente statica, **RMS diff CPU↔GPU < −60 dBFS** su 10 blocchi.
  - Integr.: 8 sorgenti statiche; nessun buffer underrun; latenza invariata.
- **QA**: nessun click con movimento + crossfade; suono indistinguibile dalla CPU.

### T08 · Scheduler & batching
- **File**: `GpuBinauralEngine.cpp`
- **Deliverable**: upload parametri per frame, una dispatch/blk, riduzione su ear (somma sorgenti).
- **Test**:
  - Perf: 64 sorgenti @48 kHz, block 256 — real‑time stabile su M‑series (no xruns).
- **QA**: uso GPU visibile in Activity Monitor; CPU audio < 20% (indicativo).

### T09 · Validazione CPU↔GPU
- **File**: `tests/offline_synth.cpp` (+ test GPU analogo)
- **Deliverable**: harness che confronta output e stampa RMS/peak diff + grafico (facoltativo).
- **Test**:
  - Varie pose e movimenti (az/el) → **RMS diff < −50 dBFS** mediamente, **< −40 dBFS** in transitori.
- **QA**: log test allegato in `COMMIT_LOG.md` per i commit che chiudono T07–T08.

---

## Milestone M2 — “Prodotto & UX”
**Obiettivo**: usabilità, manutenzione e packaging.

### T10 · MC/Max e routing
- **File**: `src/max/afx.binaural~.cpp`
- **Deliverable**: alternativa con `mc.` o routing più flessibile per N sorgenti.
- **Test**: patch MC con 16 voci; livelli e pan coerenti.
- **QA**: documentazione esempio MC.

### T11 · Profiling & tuning
- **Deliverable**: benchmark documentato (CPU vs GPU), scelta block (128/256), fp16 dove possibile su GPU.
- **Test**: tabella performance (fonti × biquad × block). 
- **QA**: parametri consigliati aggiunti a `README`/`DESIGN`.

### T12 · Packaging & CI
- **File**: `.github/workflows/macos-build.yml`, script install
- **Deliverable**: artifact `.mxo` nei release, script `install.sh`, docs aggiornate.
- **Test**: job CI compila senza variabili nascoste; bundle installabile manualmente.
- **QA**: checklist release (vedi sotto).

---

## Test Matrix (minima)
- **Sample rate**: 48 kHz (obbligatorio), 96 kHz (opzionale).
- **Block size**: 128, 256.
- **Sorgenti**: 1, 8, 64 (GPU).
- **Modalità**: CPU, GPU.
- **M‑Chip**: M1/M2/M3 (almeno uno).

**Accettazione**:
- Glitch‑free in tutti i casi provati.
- CPU↔GPU: RMS diff ≤ −50 dBFS (statico), ≤ −40 dBFS (movimento).

## Release Checklist (QA)
- [ ] Compila in Release, bundle `.mxo` presente.
- [ ] Esempio Max aggiornato e funzionante.
- [ ] Test offline passano; log incluso nel `COMMIT_LOG.md`.
- [ ] `README`, `API`, `MATH` aggiornati.
- [ ] Tag git creato; note di release con numeri di performance.

---

## Convenzioni di commit
- Prefissi: `feat:`, `fix:`, `docs:`, `refactor:`, `perf:`, `test:`
- Ogni commit che tocca un task include **ID** in titolo: es. `feat: [T07] kernel DF-II + ILD`
- Aggiornare **`COMMIT_LOG.md`** ad ogni commit rilevante (vedi template).

