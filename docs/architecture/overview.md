# MCSOS Architecture Overview
- Target arsitektur: x86_64
- Host pengembangan: Windows 11 x64
- Lingkungan build: WSL 2
- Emulator: QEMU/OVMF
- Model: Kernel monolitik pendidikan
- Bahasa: C freestanding
- Non-goals: Belum ada manajemen memori lanjut, filesystem, atau GUI.
- Readiness gate: Validasi ketat pada setiap modul (M0, M1, M2).
