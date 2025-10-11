
# AGENT.md — Istruzioni per Assistenti (Codex/AI)

**Obiettivo:** mantenere e migliorare `afx.binaural~`, un external Max che esegue binauralizzazione sintetica (no BRIR) con GPU Apple Silicon.

## Principi
- Non introdurre dipendenze pesanti. Il core deve rimanere C++17 + Metal, compatibile con Max SDK.
- Preservare l'API esterna (messaggi/attributi) documentata in `docs/API.md`.
- Mantenere la separazione: `max/` (glue), `engine/` (orchestrazione), `dsp/` (filtri e modelli), `metal/` (kernel), `util/` (infra).

## Attività comuni
- **DSP**: completare `SynthHRTF.hpp` con formule ITD/ILD, head-shadow, e pinna notch mapping.
- **GPU**: estendere `binaural_kernels.metal` per N sorgenti × blocco, stato per biquad, ritardo frazionario.
- **Scheduler**: batching in `GpuBinauralEngine` con ring buffer lock-free.
- **Testing**: ampliare `tests/offline_synth.cpp` con golden references (CPU vs GPU).

## Regole di qualità
- Copertura test per `dsp/` ≥ 80%.
- No regressioni di latenza: blocco audio predefinito 128/256; evitare allocazioni in real-time.
- Documenta *ogni* nuova formula o tabella in `docs/MATH.md` / `docs/PARAMS.md`.

## Come proporre cambi
1. Apri una branch `feature/<nome>`.
2. Aggiungi/aggiorna doc in `/docs` e esempi in `/examples`.
3. Aggiorna `CHANGELOG.md` (se presente) e `README.md` se l'API cambia.
