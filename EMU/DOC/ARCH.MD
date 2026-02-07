# Aurora Emulator Architecture (AEMU)

## 1. Overview
AEMU is a multi-architecture emulation layer designed to support both the legacy Aurora32 (32-bit) and the next-generation Aurora128 (128-bit) execution environments.

## 2. Component Layout
- **PE (Processor Engine):** Contains the core ALU and register logic for A32 and A128.
- **MM (Memory Manager):** Implements physical memory system.
- **LDR (Loader API):** Handles the loading of binary images into the emulator's address space.

## 3. Machine Initialization
The `PIINIT.C` module determines the machine type at runtime. If the user specifies `TYPE_AUR128`, the emulator initializes the 128-bit processor and enters the execution loop.
