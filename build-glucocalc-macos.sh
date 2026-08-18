#!/usr/bin/env bash
set -euo pipefail

CMAKE="/opt/homebrew/bin/cmake"

SOURCE_DIR="/Users/jeff/source/glucocalc/glucocalc-qt"
BUILD_DIR="/Users/jeff/source/glucocalc/glucocalc-qt/build"

$CMAKE -S "$SOURCE_DIR" -B "$BUILD_DIR" \
  -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" \
  -DCMAKE_OSX_DEPLOYMENT_TARGET="14.0"
$CMAKE --build "$BUILD_DIR"