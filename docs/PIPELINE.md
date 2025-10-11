
# Pipeline di progetto

1. **max/** — glue layer Max/MSP (`afx.binaural~`): gestione inlets/outlets, messaggi (`pos`, `mode`), scheduling DSP 64-bit.
2. **engine/** — orchestrazione:
   - `CpuBinauralEngine`: baseline e golden reference.
   - `GpuBinauralEngine`: path accelerato (Metal) con batching sorgenti×blocchi.
3. **dsp/** — componenti DSP riusabili:
   - `FractionalDelay` (Thiran), `Biquad` (DF-II T), `SynthHRTF` (ITD/ILD/pinna).
4. **metal/** — kernel GPU: applicazione cascata biquad + ritardo frazionario per ear/sorgente, riduzione su ear.
5. **data/** — tabelle e LUT (pinna notch).

**Real-time safety**
- Zero allocazioni nell'audio thread.
- Aggiornamento parametri per blocco con crossfade (TODO).
- Memoria unificata: buffers in `MTLStorageModeShared` (TODO nella `Impl` GPU).
