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
| Windows (Win11VM) | `C:\source\glucocalc\glucocalc-qt` | `C:\Users\jeff\build-glucocalc` (`glucocalc.exe`) |

## Build & test

### macOS
```sh
cmake -S /Users/jeff/source/glucocalc/glucocalc-qt -B /Users/jeff/source/glucocalc/glucocalc-qt/build
cmake --build /Users/jeff/source/glucocalc/glucocalc-qt/build
# -> build/glucocalc.app (20M arm64, AppIcon.icns)
```
Or `open build/glucocalc.app`

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

### Windows (Win11VM 192.168.1.137 / win11.lan)
```sh
scp -r /Users/jeff/source/glucocalc/glucocalc-qt jeff@win11.lan:'C:/source/glucocalc-qt_new'
ssh jeff@win11.lan 'powershell -Command "Remove-Item -Recurse -Force C:/source/glucocalc/glucocalc-qt -ErrorAction Ignore; New-Item -ItemType Directory -Force -Path C:/source/glucocalc | Out-Null; Rename-Item C:/source/glucocalc-qt_new C:/source/glucocalc/glucocalc-qt"'
# Build requires vcvars64.bat (MSVC 18 BuildTools)
# Use bundled script:
scp /tmp/build-glucocalc-win.ps1 jeff@win11.lan:'C:/source/build-glucocalc-win.ps1'
ssh jeff@win11.lan 'powershell -ExecutionPolicy Bypass -File C:/source/build-glucocalc-win.ps1'
# -> C:/Users/jeff/build-glucocalc/glucocalc.exe (23M, AppIcon.ico via glucocalc.rc)
```
`C:/source/build-glucocalc-win.ps1`:
```ps
$vcvars="C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
cmd /c "`"$vcvars`" > nul 2>&1 && set" | ForEach-Object { if($_ -match '^(\w+)=(.*)') { Set-Item env:$matches[1] $matches[2] } }
cmake -S C:\source\glucocalc\glucocalc-qt -B C:\Users\jeff\build-glucocalc -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\Qt.6.11.1-static"
cmake --build C:\Users\jeff\build-glucocalc
```

## CMakeLists.txt key details

- `if(NOT CMAKE_PREFIX_PATH) if(WIN32) C:/Qt/... elseif(APPLE) /Users/jeff/Qt/... else() /home/jeff/Qt/...`
- `if(APPLE) qt_add_executable(... resources/AppIcon.icns) elseif(WIN32) ... resources/glucocalc.rc else() ... resources/resources.qrc` (`WIN32_EXECUTABLE TRUE` on Win, `MACOSX_BUNDLE` on mac)
- `resources.qrc` (`/AppIcon-512.png`, `/AppIcon-256.png`, `/assets/DejaVuSans.ttf`, `/style.qss`) — Linux window icon + fonts
- No generator expressions in install; simple `install(TARGETS glucocalc BUNDLE DESTINATION .)`

## Architecture

- `src/main.cpp`: `qputenv QT_AUTO_SCREEN_SCALE_FACTOR 0 + QT_ENABLE_HIGHDPI_SCALING 0`, `QApplication::setApplicationVersion("2.0")`, `setWindowIcon(":/AppIcon-512.png")`, `addApplicationFont(":/assets/DejaVuSans.ttf")` 12px, `Fusion` style, `style.qss`, centered via `QStyle::alignedRect`.
- `src/calculator.h/.cpp`: `Calculator` static math — `eAGFromHbA1c` (`28.7*a1c-46.7`), `hbA1cFromEAG` (`(eAG+46.7)/28.7`), `mgDlToMmolL`/`mmolLToMgDl` (`/18`/`*18`), `percentToMmolMol` (`10.93*pct-23.5`), `mmolMolToPercent` (`0.09148*mmol+2.152`) — matches Swift `GlucocalcMath`.
- `src/mainwindow.h/.cpp` (322 lines): `MainWindow` owns `mode` (`HbA1cMode`=`Calculate eAG`, `EAGMode`=`Calculate HbA1c` default `EAGMode` like Swift `.a1c`), `uk`, `didCompute`, `QGridLayout *keypad`, `QButtonGroup` segmented `Calculate eAG/HbA1c` (`007AFF` checked), `QCheckBox UK (IFCC)`, `displayCard` (`#9A9A9A 16px` rounded, 3 rows: prompt `Enter HbA1c/eAG below` dynamic per `uk`/`mode` + `NGSP HbA1c (%)`/`Calculated eAG (mg/dl)` + `IFCC HbA1c (mmol/mol)`/`Calculated eAG (mmol/L)`), `QGridLayout` 4-col keypad (`7 8 9 C / 4 5 6 ⌫ / 1 2 3 = / 0 . empty empty`, `54pt 14px`, digit `E5E5EA` utility `AEAEB2` accent `007AFF`), top `?` `helpButton` (`32px` `DejaVu Sans 18 bold` `#007AFF` on white, `Help` menu fallback), `updateResultLabels()`, `toggleUKUS()` converts input (`%↔mmol/mol`/`mg/dL↔mmol/L`) and recomputes if `didCompute`, `equalsPressed()` always fills `NGSP`/`IFCC`/`eAG mg/dL / mmol/L` dual, `setFixedSize(sizeHint())` with `minimumWidth` pre-size (not `FixedWidth`) for Linux HiDPI.
- `src/helpdialog.h/.cpp`: `QDialog` `QScrollArea` verbatim `Glucocalc/HelpView.swift` (header `Glucocalc — Glucose/HbA1c Calculator` + `Version 2.0` via `applicationVersion` + `jocala@jocala.com`/`jocala.com`, sections Usage/Terminology/Formulas/About HbA1c, `Done`).
- `resources/`: `AppIcon.icns` 755K rounded 22% (`iconutil` from 1024), `AppIcon.ico` 71K 6 sizes, `AppIcon-512.png` 115K / `AppIcon-256.png` 42K rounded, `DejaVuSans.ttf` 740K, `style.qss` 5.7K (Fusion uniform), `glucocalc.rc` (`IDI_ICON1 ICON "AppIcon.ico"`), `resources.qrc`; source root `icon.png`/`glucocalc.png` 115K 512 rounded for Linux `.desktop`.
- `Info.plist` + `CMakeLists PROJECT VERSION 2.0` → `CFBundleVersion`/`CFBundleShortVersionString` `2.0`.
- Linux `build` also produces `glucocalc.desktop` (`Science` category, `Exec=glucocalc Icon=glucocalc`) + `glucocalc.png` alongside binary for `desktop-file-validate`.

## Version

`2.0` — `Info.plist` `2.0`, `CMake 2.0`, `applicationVersion 2.0` → Help shows `Version 2.0`.

## Build outputs

- macOS: `build/glucocalc.app/Contents/MacOS/glucocalc` `20M arm64`, `Contents/Resources/AppIcon.icns`, `Contents/Info.plist` `2.0`
- Linux: `/home/jeff/build-glucocalc/glucocalc` `32M ELF`, `glucocalc.desktop` `168` + `glucocalc.png` `113K`
- Windows: `C:/Users/jeff/build-glucocalc/glucocalc.exe` `~23M` (`23163392` with icon, `5/8 Building RC`)

## Known quirks

- Qt static disables `xcb` plugin warnings: `qt.core.plugin.factoryloader: ignoring ... since plugins are disabled in static builds` + `QFontDatabase: Cannot find font directory .../lib/fonts` — expected, bundled `DejaVuSans` via `:/assets` is used.
- Linux screenshots via `xvfb-run -a --server-args='-screen 0 1280x1024x24' /tmp/capture.sh` + `import -window root`.
- Windows `LINK : fatal error LNK1104: cannot open file 'glucocalc.exe'` if still running — `taskkill /F /IM glucocalc.exe` first.
