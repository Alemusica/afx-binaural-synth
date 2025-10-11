
# Esempio Max

1. Compila l'external: `scripts/build.sh`
2. Copia `build/Release/afx.binaural~.mxo` in una cartella del Package.
3. Crea un patcher con:
   - `[cycle~ 440]` → `[afx.binaural~ @mode cpu]` → `[ezdac~]`
   - Messaggio: `pos 0 60 20 1.` collegato all'oggetto.
