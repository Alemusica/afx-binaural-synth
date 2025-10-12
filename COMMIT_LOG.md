
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
