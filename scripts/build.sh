
#!/usr/bin/env bash
set -euo pipefail
BUILD_DIR=${BUILD_DIR:-build}
GEN=${GEN:-Xcode}
CONFIG=${CONFIG:-Release}
mkdir -p "$BUILD_DIR"
cmake -S . -B "$BUILD_DIR" -G "$GEN" -DMAX_SDK_DIR="${MAX_SDK_DIR}"
cmake --build "$BUILD_DIR" --config "$CONFIG" --target afx.binaural~
echo "Bundle: $BUILD_DIR/$CONFIG/afx.binaural~.mxo"
