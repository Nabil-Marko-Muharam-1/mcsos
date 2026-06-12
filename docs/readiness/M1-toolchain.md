# Readiness Review M1 Toolchain Reproducible

## Identitas
- Nama mahasiswa/kelompok: Muhamad Nabil
- NIM anggota: 2583207073017
- Kelas: 1B
- Dosen: Muhaemin Sidiq, S.Pd., M.Pd.
- Program Studi: Pendidikan Teknologi Informasi, Institut Pendidikan Indonesia
- Tanggal: 2026-06-12
- Commit hash: (akan diisi setelah commit)

## Ringkasan hasil
Lingkungan pengembangan telah terbukti stabil dan reproducible. Seluruh toolchain wajib tersedia di WSL 2, kompilasi freestanding berhasil menghasilkan ELF64 x86_64 murni tanpa undefined symbol, dan hash build terbukti konsisten. Lingkungan siap untuk M2.

## Evidence checklist
| Evidence | Path | Status | Catatan |
|---|---|---|---|
| Toolchain versions | `build/meta/toolchain-versions.txt` | OK | |
| Host readiness | `build/meta/host-readiness.txt` | OK | |
| QEMU capabilities | `build/meta/qemu-capabilities.txt` | OK | |
| Freestanding object | `build/proof/freestanding_probe.o` | OK | |
| Freestanding ELF | `build/proof/freestanding_probe.elf` | OK | |
| ELF header | `build/proof/readelf-header.txt` | OK | |
| ELF sections | `build/proof/readelf-sections.txt` | OK | |
| Disassembly | `build/proof/objdump-disassembly.txt` | OK | |
| Undefined symbol report | `build/proof/nm-undefined.txt` | OK | Kosong |
| Reproducibility hash | `build/repro/sha256-run1.txt`, `build/repro/sha256-run2.txt` | OK | Hash identik |

## Acceptance criteria M1
| Kriteria | Lulus/Gagal | Bukti |
|---|---|---|
| Repository berada di filesystem Linux WSL | Lulus | Output check_toolchain.sh |
| Semua tool wajib tersedia | Lulus | Output check_toolchain.sh |
| `make meta` berhasil | Lulus | toolchain-versions.txt |
| `make check` berhasil | Lulus | Output terminal |
| `make proof` berhasil | Lulus | freestanding_probe.elf |
| `make qemu-probe` berhasil | Lulus | qemu-capabilities.txt |
| `make repro` berhasil | Lulus | sha256-diff.txt kosong |
| `make test` berhasil dari clean checkout | Lulus | Output terminal |
| `nm-undefined.txt` kosong | Lulus | nm-undefined.txt |
| Hasil `readelf` menunjukkan ELF64 x86_64 | Lulus | readelf-header.txt |

## Known limitations
Belum ada OS image yang di-boot, belum ada CI otomatis, dan pengujian berjalan terbatas di atas emulator QEMU.

## Risiko dan mitigasi
1. Linker memakai library host -> Mitigasi: Cek dengan `nm -u`.
2. Repository kotor oleh artefak -> Mitigasi: `.gitignore` dan `make distclean`.
3. Versi toolchain tidak sama -> Mitigasi: Catat di `toolchain-versions.txt`.

## Readiness decision
- [X] Siap lanjut M2.

Alasan keputusan:
Semua kriteria acceptance M1 terpenuhi tanpa error. Freestanding build terbukti berhasil direproduksi dan environment sudah menggunakan WSL 2 filesystem yang stabil.
