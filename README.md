# Cpp23_Module-Modular_Modern_Odds_Lib

Small, fast C++23 "1 in N" odds library (header-only core), with optional C++23 module wrapper.

## Build

### Windows (Ninja + MSVC)
```powershell
cmake --preset windows-msvc
cmake --build build/windows-msvc
```

### Windows (Ninja + clang-cl)
```powershell
cmake --preset windows-clang
cmake --build build/windows-clang
```

### Linux (Ninja + Clang)
```bash
cmake --preset linux-clang
cmake --build build/linux-clang
```

### Linux (Ninja + GCC; modules OFF)
```bash
cmake --preset linux-gcc
cmake --build build/linux-gcc
```
