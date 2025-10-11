
# AGENT.md — Istruzioni per Assistenti (Codex/AI)

**Obiettivo:** mantenere e migliorare `afx.binaural~`, un external Max che esegue binauralizzazione sintetica (no BRIR) su CPU/GPU (Metal).

## Principi
- Evita dipendenze pesanti; core C++17 + Metal; compatibile con Max SDK.
- Mantieni l'API documentata in `docs/API.md` stabile.
- Separa chiaramente: `max/` (glue), `engine/` (orchestrazione), `dsp/` (filtri e matematica), `metal/` (kernel), `util/` (infra).

## Attività tipiche
- **DSP**: ITD/ILD/head-shadow, pinna (notches) e conversione in biquad.
- **GPU**: kernel Metal (DF‑II T + ritardo frazionario) e scheduling batched.
- **Scheduler**: ring buffers lock‑free; param update per blocco.
- **Testing**: CPU↔GPU parity; performance e glitch‑free.

## Regole di qualità
- Test per `dsp/` con copertura adeguata (≥ 80% consigliata).
- Niente allocazioni nel perform; blocchi 128/256 testati a 48 kHz.
- Documenta formule e LUT in `docs/MATH.md` / `docs/PARAMS.md`.

---
## Workflow operativo (OBBLIGATORIO)

1. **Consulta `ROADMAP.md`** prima di iniziare; identifica i task (`T01`..`T12`) che tocchi.
2. **Per ogni commit** (o PR):
   - Includi nel **titolo** l’ID task: es. `feat: [T06] init Metal Impl`.
   - **Aggiorna `COMMIT_LOG.md`** aggiungendo una voce con:
     - Data, autore, **hash** del commit
     - Task toccati (`[Txx]`)
     - Cosa è stato fatto (breve)
     - **Test eseguiti** (unit/integr./parity) e risultati
     - **QA**: criteri minimi soddisfatti? (sì/no) + note
     - Stato (in corso/chiuso). Se chiudi un task, scrivi **“Task closed: Txx”**.
3. Se modifichi formule/LUT, aggiorna **`docs/MATH.md`** o **`docs/PARAMS.md`** nello stesso branch.
4. Se l’API cambia, aggiorna **`docs/API.md`** e `README.md` nello stesso commit/PR.
5. Nessun commit “magico”: la storia deve permettere di ricostruire test e QA eseguiti.

---
## Riferimenti
- **Roadmap**: vedi `ROADMAP.md`.
- **Test offline**: `tests/offline_synth.cpp` (CPU) e harness GPU (quando disponibile).
- **CI**: `.github/workflows/macos-build.yml` (build macOS).
