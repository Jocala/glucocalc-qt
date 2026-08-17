# Glucocalc-Qt — Glucose/HbA1c Calculator (Qt Desktop)

Qt 6 Widgets port of Glucocalc — estimates HbA1c and average blood glucose using the **2008 ADAG Study Group formulas**, plus glucose/HbA1c unit conversions.

Runs on **macOS**, **Windows**, and **Linux** from a single CMake + Qt 6.11 codebase. Static Qt builds.

## Features

- Keypad-driven input: `7 8 9 C / 4 5 6 ⌫ / 1 2 3 = / 0 .`, with `=` computing, `C` clearing, `⌫` backspacing
- **Calculate eAG** (from HbA1c) and **Calculate HbA1c** (from eAG) modes
- **UK (IFCC) switch** converts current input on the fly (mg/dL ↔ mmol/L, % ↔ mmol/mol) and recomputes
- Results always show both units (mg/dL + mmol/L, % + mmol/mol)
- Help dialog with usage, terminology, formulas

## Math (2008 ADAG)

- eAG = 28.7 × A1c − 46.7 (mg/dL); mmol/L = /18
- A1c = (eAG + 46.7) / 28.7 (%); IFCC = 10.93 × A1c − 23.50 (mmol/mol)
- NGSP = 0.09148 × IFCC + 2.152; mmol/L ↔ mg/dL: ×18 / ÷18

## Build

Requires static Qt 6.11.1 (`CMAKE_PREFIX_PATH` per `AGENTS.md`).

```sh
# macOS
cmake -S . -B build
cmake --build build
open build/glucocalc.app

# Linux / Windows — same, with platform Qt kit
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt.6.11.1-static
cmake --build build
```

See `AGENTS.md` for per-platform paths and packaging.

## Related

- `glucocalc-apple` — SwiftUI (iOS + macOS): https://github.com/Jocala/glucocalc-apple
- `glucocalc-android` — Android: https://github.com/Jocala/glucocalc-android

## License

GPL-3.0 — see `LICENSE` (copied from glucocalc-apple).
