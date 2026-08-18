#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(dirname "$0")"
CPACK="/opt/homebrew/bin/cpack"
CODESIGN_IDENTITY="Developer ID Application: jeff elkins (9Q77WK7W3R)"

BUILD_DIR="/Users/jeff/source/glucocalc/glucocalc-qt/build"

codesign --force --deep --sign "$CODESIGN_IDENTITY" --timestamp \
  --options=runtime \
  --entitlements "${SCRIPT_DIR}/packaging/glucocalc.entitlements" \
  "$BUILD_DIR/glucocalc.app"
$CPACK --config "$BUILD_DIR/CPackConfig.cmake" \
  -D CPACK_PRE_BUILD_SCRIPTS="${SCRIPT_DIR}/packaging/sign-after-install.cmake" \
  -D ENTITLEMENTS_PATH="${SCRIPT_DIR}/packaging/glucocalc.entitlements" \
  -B "$BUILD_DIR/packages"

NOTARY_PROFILE="adblink-notary"
DMG="$BUILD_DIR/packages/glucocalc-2.0-Darwin.dmg"
xcrun notarytool submit "$DMG" --keychain-profile "$NOTARY_PROFILE" --wait
xcrun stapler staple "$DMG"
ls -lh "$BUILD_DIR/packages/"