
#!/usr/bin/env bash
set -euo pipefail
if [[ -z "${MAX_SDK_DIR:-}" ]]; then
  echo "Errore: devi esportare MAX_SDK_DIR=/percorso/al/max-sdk"
  exit 1
fi
if ! command -v cmake >/dev/null 2>&1; then
  echo "Installa CMake (brew install cmake)"
  exit 1
fi
if ! xcrun -sdk macosx --find metal >/dev/null 2>&1; then
  echo "Xcode Command Line Tools mancanti (xcode-select --install)"
  exit 1
fi
echo "OK: requisiti soddisfatti."
