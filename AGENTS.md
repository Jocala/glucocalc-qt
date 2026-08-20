# Glucocalc — agent guidance

Qt port of Swift `Glucocalc` (`/Users/jeff/source/glucocalc/glucocalc-apple`) — Glucose/HbA1c calculator (ADAG 2008).

## Qt paths (CMAKE_PREFIX_PATH)

- **Windows**: `C:\Qt\Qt.6.11.1-static`
- **macOS**: `/Users/jeff/Qt/Qt.6.11.1-static`
- **Linux**: `/home/jeff/Qt/Qt.6.11.1-static`

Static Qt 6.11.1, `CMAKE_AUTOMOC/AUTORCC/AUTOUIC ON`, `C++17`, `Qt6::Core Gui Widgets`.

## Source locations

| Platform | Source | Build |
|---|---|---|
| macOS | `/Users/jeff/source/glucocalc/glucocalc-qt` | `/Users/jeff/source/glucocalc/glucocalc-qt/build` (`glucocalc.app`) |
| Linux (Debian) | `/zstore/source/glucocalc/glucocalc-qt` (`/Volumes/source/glucocalc/glucocalc-qt` via Samba) | `/home/jeff/build-glucocalc` (`glucocalc` ELF) |
| Windows (Win10 192.168.1.42) | `C:\source\glucocalc` | `C:\Users\jeff\build-glucocalc` (`glucocalc.exe`) |

## Build & test

### macOS
```sh
cmake -S /Users/jeff/source/glucocalc/glucocalc-qt -B /Users/jeff/source/glucocalc/glucocalc-qt/build
cmake --build /Users/jeff/source/glucocalc/glucocalc-qt/build
# -> build/glucocalc.app (universal x86_64;arm64 after build-glucocalc-macos.sh, AppIcon.icns)
```
Or `open build/glucocalc.app`
For the universal (x86_64;arm64) signing build, use `./build-glucocalc-macos.sh` (see "Packaging & signing").

### Linux (Debian 192.168.1.39 / debian.lan)
```sh
tar cf - -C /Users/jeff/source/glucocalc --exclude=glucocalc-qt/build --exclude=glucocalc-qt/*.tar.gz glucocalc-qt | ssh jeff@debian.lan 'rm -rf /zstore/source/glucocalc/glucocalc-qt && mkdir -p /zstore/source/glucocalc && tar xf - -C /zstore/source/glucocalc'
ssh jeff@debian.lan 'cmake -S /zstore/source/glucocalc/glucocalc-qt -B /home/jeff/build-glucocalc && cmake --build /home/jeff/build-glucocalc'
# -> /home/jeff/build-glucocalc/glucocalc (32M ELF x86-64) + glucocalc.desktop + glucocalc.png
```
Run headless screenshot: `ssh jeff@debian.lan "cat > /tmp/capture.sh <<'EOS'
#!/bin/bash
/home/jeff/build-glucocalc/glucocalc & sleep 4; import -window root /tmp/out.png
EOS
xvfb-run -a --server-args='-screen 0 1280x1024x24' /tmp/capture.sh"`

### Windows (Win10 192.168.1.42)
```sh
tar cf - -C /Users/jeff/source/glucocalc --exclude=glucocalc-qt/build --exclude=glucocalc-qt/.qt glucocalc-qt | ssh jeff@192.168.1.42 'powershell -Command "Remove-Item -Recurse -Force C:/source/glucocalc -ErrorAction Ignore; tar xf - -C C:/source; Move-Item C:/source/glucocalc-qt C:/source/glucocalc"'
# Legacy Win11VM was 192.168.1.137 / win11.lan (C:/source/glucocalc/glucocalc-qt nested)
# Build requires vcvars64.bat (MSVC 18 BuildTools)
# Use bundled script:
scp /tmp/build-glucocalc-win.ps1 jeff@192.168.1.42:'C:/source/build-glucocalc-win.ps1'
ssh jeff@192.168.1.42 'powershell -ExecutionPolicy Bypass -File C:/source/build-glucocalc-win.ps1'
# -> C:/Users/jeff/build-glucocalc/glucocalc.exe (23M, AppIcon.ico via glucocalc.rc)
```
`C:/source/build-glucocalc-win.ps1`:
```ps
$vcvars="C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmd /c "`"$vcvars`" > nul 2>&1 && set" | ForEach-Object { if($_ -match '^(\w+)=(.*)') { Set-Item env:$matches[1] $matches[2] } }
cmake -S C:\source\glucocalc -B C:\Users\jeff\build-glucocalc -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build C:\Users\jeff\build-glucocalc
```
Inno Setup (`C:\bin\inno\ISCC.exe`): `C:\bin\inno\ISCC.exe C:\source\glucocalc\packaging\glucocalc.iss` → `C:\Users\jeff\build-glucocalc\glucocalc-2.0-Windows-Setup.exe` (9M) → `scp` to `debian:/zstore/source/www/jocala.com/glucocalc/`

## CMakeLists.txt key details

- `if(NOT CMAKE_PREFIX_PATH) if(WIN32) C:/Qt/... elseif(APPLE) /Users/jeff/Qt/... else() /home/jeff/Qt/...`
- `if(APPLE) qt_add_executable(... resources/AppIcon.icns resources/resources.qrc) elseif(WIN32) ... resources/glucocalc.rc resources/resources.qrc else() ... resources/resources.qrc` (`WIN32_EXECUTABLE TRUE` on Win, `MACOSX_BUNDLE` on mac) — `resources.qrc` is wired into ALL three branches (macOS/Win previously omitted it: no QSS/font/window-icon at runtime there).
- `resources.qrc` (`/AppIcon-512.png`, `/AppIcon-256.png`, `/assets/DejaVuSans.ttf`, `/style.qss`, `/style-dark.qss`) — window icon + fonts + themes on every platform
- All three branches also compile `src/theme.cpp`.
- No generator expressions in install; simple `install(TARGETS glucocalc BUNDLE DESTINATION .)`

## Architecture

- `src/main.cpp`: `qputenv QT_AUTO_SCREEN_SCALE_FACTOR 0 + QT_ENABLE_HIGHDPI_SCALING 0`, `QApplication::setApplicationVersion("2.0")`, `setWindowIcon(":/AppIcon-512.png")`, `addApplicationFont(":/assets/DejaVuSans.ttf")` 12px, `Fusion` style, `Theme::apply(&app)` (loads `:/style.qss` or `:/style-dark.qss` per saved theme + Fusion palette), centered via `QStyle::alignedRect`.
- `src/theme.h/.cpp`: theme state + persistence — `Theme::isDark()`/`setDark(bool)` backed by `QSettings("jocala","Glucocalc")` key `darkMode`; `Theme::styleSheet()` returns the QSS for the current theme; `Theme::colors()` returns `Theme::Colors` (`cardBg`/`segTrack`/`keyUtility`/`keyPressed`/`accent`/`textPrimary`/`textSecondary`/`border`/`helpBg`/`helpPressed`) for MainWindow's inline styles; `Theme::apply(QApplication*)` swaps app stylesheet + Fusion palette (dark palette: window `#1C1C1E`, base `#2C2C2E`, highlight/link `#0A84FF`).
- `src/calculator.h/.cpp`: `Calculator` static math — `eAGFromHbA1c` (`28.7*a1c-46.7`), `hbA1cFromEAG` (`(eAG+46.7)/28.7`), `mgDlToMmolL`/`mmolLToMgDl` (`/18`/`*18`), `percentToMmolMol` (`10.93*pct-23.5`), `mmolMolToPercent` (`0.09148*mmol+2.152`) — matches Swift `GlucocalcMath`.
- `src/mainwindow.h/.cpp` (~370 lines): `MainWindow` owns `mode` (`HbA1cMode`=`Calculate eAG`, `EAGMode`=`Calculate HbA1c` default `EAGMode` like Swift `.a1c`), `uk`, `didCompute`, `QGridLayout *keypad`, `QButtonGroup` segmented `Calculate eAG/HbA1c` (accent checked), options row `QCheckBox UK (IFCC)` (left) + `QCheckBox Dark Mode` (right, persists), `displayCard` (`16px` rounded, theme-adaptive: light `#9A9A9A`/black, dark `#2C2C2E`/white values + `#AEAEB2` labels; 3 rows: prompt `Enter HbA1c/eAG below` dynamic per `uk`/`mode` + `NGSP HbA1c (%)`/`Calculated eAG (mg/dl)` + `IFCC HbA1c (mmol/mol)`/`Calculated eAG (mmol/L)`), `QGridLayout` 4-col keypad (`7 8 9 C / 4 5 6 ⌫ / 1 2 3 = / 0 . empty empty`, digit/utility/accent keys stored in `digitKeys`/`utilityKeys`/`accentKey` for re-theming), top `?` `helpButton` (`32px` `DejaVu Sans 18 bold` accent-on-`helpBg`, `Help` menu fallback), `applyTheme()` re-applies ALL inline stylesheets from `Theme::colors()` (called at construction + on toggle), `toggleTheme()` = `Theme::setDark(themeBox->isChecked())` + `Theme::apply(qApp)` + `applyTheme()`, `updateResultLabels()`, `toggleUKUS()` converts input (`%↔mmol/mol`/`mg/dL↔mmol/L`) and recomputes if `didCompute`, `equalsPressed()` always fills `NGSP`/`IFCC`/`eAG mg/dL / mmol/L` dual, geometry locked via `layout->setSizeConstraint(QLayout::SetFixedSize)` + `setFixedSize(sizeHint())`; prompt/result labels reserve the WIDEST string across all mode/UK states (`QFontMetrics::horizontalAdvance`) so the sizeHint is constant and toggling UK/mode/theme never shifts or clips the window.
- `src/helpdialog.h/.cpp`: `QDialog` `QScrollArea` verbatim `Glucocalc/HelpView.swift` (header `Glucocalc — Glucose/HbA1c Calculator` + `Version 2.0` via `applicationVersion` + `jocala@jocala.com`/`jocala.com`, sections Usage/Terminology/Formulas/About HbA1c, `Done`).
- `resources/`: `AppIcon.icns` 755K rounded 22% (`iconutil` from 1024), `AppIcon.ico` 71K 6 sizes, `AppIcon-512.png` 115K / `AppIcon-256.png` 42K rounded, `DejaVuSans.ttf` 740K, `style.qss` 5.7K + `style-dark.qss` 6.0K (Fusion uniform, dark variant, both with `QToolTip`), `glucocalc.rc` (`IDI_ICON1 ICON "AppIcon.ico"`), `resources.qrc`; source root `icon.png`/`glucocalc.png` 115K 512 rounded for Linux `.desktop`.
- `packaging/Info.plist.in` + `CMakeLists PROJECT VERSION 2.0` → `CFBundleVersion`/`CFBundleShortVersionString` `2.0`, bundle id `com.jocala.glucocalc`.
- Linux `build` also produces `glucocalc.desktop` (`Science` category, `Exec=glucocalc Icon=glucocalc`) + `glucocalc.png` alongside binary for `desktop-file-validate`.

## Version

`2.0` — `packaging/Info.plist.in` `2.0`, `CMake 2.0`, `applicationVersion 2.0` → Help shows `Version 2.0`.

## Packaging & signing (macOS)

- `./build-glucocalc-macos.sh` — universal reconfigure (`CMAKE_OSX_ARCHITECTURES="x86_64;arm64"`, deployment target 14.0) + build. Run before packaging.
- `./package-glucocalc-macos.sh` — codesign (Developer ID, hardened runtime `--options=runtime`, `--timestamp`, empty `packaging/glucocalc.entitlements`), `cpack` DragNDrop with `packaging/sign-after-install.cmake` re-sign in staging dir, `xcrun notarytool submit --keychain-profile adblink-notary --wait`, `xcrun stapler staple`. Output: `build/packages/glucocalc-2.0-Darwin.dmg` (17M).
- Identity: `Developer ID Application: jeff elkins (9Q77WK7W3R)` — team `9Q77WK7W3R`, notary profile `adblink-notary` (same Apple ID/team as adblink).
- Bundle id `com.jocala.glucocalc` comes from `MACOSX_BUNDLE_GUI_IDENTIFIER` in CMakeLists + `packaging/Info.plist.in`. Stale root `Info.plist` was removed (was dead: no `MACOSX_BUNDLE_INFO_PLIST` wired, bogus `CFBundleIconFile appiconset`).
- Verification:
  - `codesign --verify --deep --strict --verbose=2 build/glucocalc.app`
  - `codesign -dv --verbose=4 build/glucocalc.app` (expect `Identifier=com.jocala.glucocalc`, `Format=app bundle with Mach-O universal (x86_64 arm64)`, flags `runtime`)
  - `xcrun stapler validate build/packages/glucocalc-2.0-Darwin.dmg`
  - `xcrun notarytool history --keychain-profile adblink-notary`
  - `hdiutil attach build/packages/glucocalc-2.0-Darwin.dmg` → confirm `glucocalc.app` + `Applications` symlink
- Note: `packaging/glucocalc.entitlements` is an EMPTY dict — no `com.apple.security.cs.disable-library-validation` (that's adblink-only, for its nested adb tools; glucocalc has no nested code).

## Build outputs

- macOS: `build/glucocalc.app/Contents/MacOS/glucocalc` `41M universal x86_64;arm64`, `Contents/Resources/AppIcon.icns`, `Contents/Info.plist` `2.0`
- Linux: `/home/jeff/build-glucocalc/glucocalc` `32M ELF`, `glucocalc.desktop` `168` + `glucocalc.png` `113K`
- Windows: `C:/Users/jeff/build-glucocalc/glucocalc.exe` `~23M` (`23163392` with icon, `5/8 Building RC`)

## Known quirks

- Qt static disables `xcb` plugin warnings: `qt.core.plugin.factoryloader: ignoring ... since plugins are disabled in static builds` + `QFontDatabase: Cannot find font directory .../lib/fonts` — expected, bundled `DejaVuSans` via `:/assets` is used.
- macOS now loads the QSS (resources.qrc wired into all branches), so the harmless `qt.qpa.fonts: Populating font family aliases ... missing font family "-apple-system"` warning appears at startup — the QSS font list falls back to the app font; expected.
- Linux screenshots via `xvfb-run -a --server-args='-screen 0 1280x1024x24' /tmp/capture.sh` + `import -window root`.
- Windows `LINK : fatal error LNK1104: cannot open file 'glucocalc.exe'` if still running — `taskkill /F /IM glucocalc.exe` first.
