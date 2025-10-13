
# COMMIT_LOG

Annota qui ogni commit/PR rilevante secondo il workflow in AGENT.md.

## Template
- **Data**: YYYY-MM-DD
- **Autore**: Nome
- **Commit**: `<hash>`
- **Task**: [Txx, Tyy]
- **Descrizione**: cosa è stato fatto
- **Test eseguiti**:
  - Unit: ...
  - Integrazione: ...
  - GPU/CPU parity: RMS diff = ...
- **QA**:
  - Glitch-free (y/n):
  - Note:
- **Stato**: in corso / chiuso (se chiuso, “Task closed: Txx”)

---

## Log
- Data: 2025-10-12
  - Autore: AFX
  - Commit: INIT
  - Task: [bootstrap]
  - Descrizione: Aggiunta `ROADMAP.md`, `AGENT.md`, `COMMIT_LOG.md`.
  - Test eseguiti: N/A
  - QA: N/A
  - Stato: in corso
- Data: 2025-10-12
  - Autore: ChatGPT
  - Commit: e133be2
  - Task: [T01]
  - Descrizione: Implementato Thiran N=3 DF-II-T con ring buffer per parte intera e documentazione aggiornata.
  - Test eseguiti:
    - Build: `cmake -S . -B build` (fallito: cc1obj non disponibile in container)
  - QA:
    - Glitch-free (y/n): n/d
    - Note: Fallback GPU ancora CPU-side; necessaria integrazione futura.
  - Stato: in corso
- Data: 2025-10-12
  - Autore: ChatGPT
  - Commit: TBD
  - Task: [T01]
  - Descrizione: Rifattorizzato ritardo Thiran con wrapper `ThiranDelay`, test di group delay e Makefile per harness offline.
  - Test eseguiti:
    - Unit: `make test` (pass)
  - QA:
    - Glitch-free (y/n): n/d (offline harness)
    - Note: GPU ancora stub CPU; verificare integrazione futura.
  - Stato: in corso

- Data: 2025-10-12
  Autore: <AGENT/Codex>
  Commit: e6b592ed3a0952f9a92499afdc8ba6f0020390af
  Task: [T01]
  Descrizione: Implementato ThiranAP3 ordine 3 con buffer intero opzionale; documentazione formule e test group delay.
  Test eseguiti:
    - Unit: make test (offline_synth + test_thiran_phase)
  QA:
    - Glitch-free: y (offline harness, ritardi positivi)
    - Note: GPU path ancora usa CPU fallback
  Stato: chiuso (Task closed: T01)
- Data: 2025-10-12
  - Autore: ChatGPT
  - Commit: 5d2fd7a9ea2e55c94d90f3cc699de95cc15ec238
  - Task: [T01, T06]
  - Descrizione: Preservata la storia nel ritardo Thiran e memoizzazione dei ritardi per CPU/GPU; aggiunto test di stabilità ampiezza.
  - Test eseguiti:
    - Integrazione: `make test` (pass)
  - QA:
    - Glitch-free (y/n): n/d (offline harness)
    - Note: GPU ancora fallback CPU; monitorare durante porting Metal.
  - Stato: in corso
- Data: 2025-10-12
  - Autore: ChatGPT
  - Commit: <pending>
  - Task: [T01, T09]
  - Descrizione: Corretto il segno dell'ITD per anticipare l'orecchio ipsilaterale, rimossi i clamp sui ritardi relativi e ampliati i test offline con casi speculari; documentata la convenzione del segno.
  - Test eseguiti:
    - Integrazione: `make test` (pass)
  - QA:
    - Glitch-free (y/n): n/d (offline harness)
    - Note: controllare anche percorso GPU reale quando disponibile.
  - Stato: in corso
- Data: 2025-10-12
  - Autore: ChatGPT
  - Commit: f0fe506e3035efd4b994b0801f6c25bffa4683cb
  - Task: [T06, T09]
  - Descrizione: Stati GPU fallback resi persistenti con aggiornamento coeff solo quando cambia e nuovo test di continuità multi-blocco.
  - Test eseguiti:
    - Integrazione: `make test` (pass)
  - QA:
    - Glitch-free (y/n): n/d (offline harness CPU fallback GPU)
    - Note: Continuity harness GPU fallback aggiunto; verificare su Metal reale.
  - Stato: in corso
