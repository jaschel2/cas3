# MAT267 CAS — Build & Install Guide

## What you're getting
A native `.g3a` add-in for the **Casio FX-CG50** that runs directly from the
calculator's main menu.  No typing required — navigate with arrow keys and EXE.

---

## Commands in the add-in

| Key | Operation | What it computes |
|-----|-----------|-----------------|
| 1 | CURL & DIV | curl F and div F at a point |
| 2 | SURFACE AREA | ∬ ‖rᵤ × rᵥ‖ du dv |
| 3 | SURF INTEGRAL | ∬ f dS over parametric surface |
| 4 | FLUX INTEGRAL | ∬ F·dS, choose orientation |
| 5 | VOLUME | Volume between two surfaces (polar) |
| 6 | TRIPLE INTEGRAL | ∭ f dV — Cartesian / Cylindrical / Spherical |
| 7 | TANGENT PLANE | z = f(x₀,y₀) + fₓ(x−x₀) + f_y(y−y₀) |

**Navigation:**
- `↑ ↓` — move between menu items or input fields
- `EXE` — select / advance to next field
- `F6 (CALC)` — compute immediately from any field
- `F5 (NEXT)` — advance field without computing
- `F3 (PI)` — insert `pi` into current field
- `F4 (NEG)` — toggle leading minus
- `F1 (DEL)` — backspace
- `F2 (CLR)` — clear entire field
- `EXIT` — go back one screen

---

## Recommended toolchain: fxSDK + gint  (fully free, open-source)

This is the easiest path — no Casio account, no proprietary SDK.

### Step 1 — Install fxSDK

**Linux / WSL (recommended on Windows):**
```bash
# Install build dependencies
sudo apt install git cmake python3 python3-pip binutils-dev

# Clone and build the SH3-EB cross-compiler (takes ~10 min)
git clone https://gitea.planet-casio.com/Lephenixnoir/sh-elf-binutils-gdb.git
cd sh-elf-binutils-gdb && mkdir build && cd build
cmake .. -DTARGET=sh3eb && make -j$(nproc) && sudo make install
cd ../..

git clone https://gitea.planet-casio.com/Lephenixnoir/sh-elf-gcc.git
cd sh-elf-gcc && mkdir build && cd build
cmake .. -DTARGET=sh3eb && make -j$(nproc) && sudo make install
cd ../..

# Install fxSDK itself
pip3 install fxsdk
```

**macOS (Homebrew):**
```bash
# The cross-compiler packages are available via a tap
brew tap lephenixnoir/software
brew install lephenixnoir/software/sh-elf-gcc
pip3 install fxsdk
```

**Windows (native, no WSL):**
Download pre-built binaries from:
https://gitea.planet-casio.com/Lephenixnoir/fxsdk/releases
Then add them to your PATH.

### Step 2 — Install gint (the open-source CG50 kernel)

```bash
git clone https://gitea.planet-casio.com/Lephenixnoir/gint.git
cd gint
fxsdk build-cg   # builds and installs gint for the CG50
cd ..
```

### Step 3 — Generate the icon

```bash
pip3 install Pillow       # one-time
python3 gen_icon.py       # creates src/icon.png
```

### Step 4 — Build the add-in

```bash
cd mat267cas
fxsdk build-cg
```

This outputs `MAT267CAS.g3a` in the project folder.

---

## Install onto the calculator

1. Connect the FX-CG50 to your computer via USB
2. On the calculator: `MENU` → `LINK` → `F1 (USB Flash)`
3. The calculator appears as a USB drive on your computer
4. Copy `MAT267CAS.g3a` into the `@ADDIN` folder on the drive
5. Safely eject, then on the calculator press `EXIT`
6. The add-in now appears in the main menu — press EXE to launch

---

## Troubleshooting

| Problem | Fix |
|---------|-----|
| `sh-elf-gcc: not found` | Add `/usr/local/bin` to PATH, or reinstall |
| `fxsdk: command not found` | Run `pip3 install fxsdk` again; check `~/.local/bin` in PATH |
| `gint not found` during build | Re-run `fxsdk build-cg` inside the gint directory |
| Icon build error | Run `python3 gen_icon.py` first |
| Calc doesn't show the add-in | Make sure the file is in `@ADDIN`, not the root |
| Crash on launch | Rebuild with `-DCMAKE_BUILD_TYPE=Debug`; check for missing `\0` in strings |

---

## Project file structure

```
mat267cas/
  CMakeLists.txt   ← build script (don't edit unless adding files)
  gen_icon.py      ← run once to make src/icon.png
  src/
    cas.h          ← shared types, constants, inline helpers
    main.c         ← entry point, main loop, screen routing
    eval.c         ← expression parser (handles sin, cos, pi, ^, etc.)
    fields.c       ← input field definitions + defaults for each operation
    compute.c      ← all 7 numerical routines (Simpson, cross products, etc.)
    ui.c           ← all drawing + key handling
```

---

## Useful links

- fxSDK docs: https://gitea.planet-casio.com/Lephenixnoir/fxsdk
- gint docs: https://gitea.planet-casio.com/Lephenixnoir/gint
- Planète Casio forum (French but very active):
  https://www.planet-casio.com/Fr/forums/topic17706-1-fxsdk-un-sdk-moderne-pour-les-calculatrices-casio.html
- Pre-built toolchain Windows releases:
  https://gitea.planet-casio.com/Lephenixnoir/fxsdk/releases
