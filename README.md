# Car Acceleration x86-64

This project computes the acceleration of multiple cars using a C reference
implementation and a Windows x86-64 NASM implementation with scalar SIMD
floating-point instructions.

## Project structure

```text
car-acceleration-x64/
├── car-acceleration-x64.slnx
├── .gitignore
├── README.md
└── car-acceleration-x64/
    ├── main.c
    ├── acceleration.h
    ├── acceleration_c.c
    ├── acceleration_asm.asm
    ├── car-acceleration-x64.vcxproj
    └── car-acceleration-x64.vcxproj.filters
```

## File responsibilities

- `main.c`: input collection, allocation, output, correctness checking, and benchmarking.
- `acceleration.h`: declarations shared by C and assembly.
- `acceleration_c.c`: C reference implementation.
- `acceleration_asm.asm`: x86-64 implementation using scalar SIMD instructions.
- `car-acceleration-x64.vcxproj`: Visual Studio build configuration.
