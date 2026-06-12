# Readiness Review M2 Boot Image dan Early Serial Console

## Identitas
- Proyek: MCSOS 260502
- Praktikum: M2
- Target: x86_64, QEMU, OVMF, Limine
- Nama/Kelompok: Muhamad Nabil / 2583207073017
- Commit hash: 80cbc2f
- Tanggal: 2026-06-12

## Ringkasan Status
Status yang diajukan: siap uji QEMU tahap M2.
Alasan ringkas: Seluruh artefak wajib (kernel ELF, kernel map, ISO) telah berhasil dibangun secara konsisten. QEMU berhasil mengeksekusi image menggunakan OVMF, dan serial log membuktikan kernel masuk ke dalam *controlled halt loop* sesuai desain.

## Evidence Matrix
| Evidence | Lokasi | Status | Catatan |
|---|---|---|---|
| Preflight M2 | `build/meta/m2-preflight.txt` | PASS | |
| Kernel ELF | `build/kernel.elf` | PASS | |
| Kernel map | `build/kernel.map` | PASS | |
| readelf header | `build/inspect/readelf-header.txt` | PASS | |
| readelf PHDR | `build/inspect/readelf-program-headers.txt` | PASS | |
| objdump | `build/inspect/objdump-disassembly.txt` | PASS | |
| ISO | `build/mcsos.iso` | PASS | |
| ISO checksum | `build/mcsos.iso.sha256` | PASS | |
| Serial log | `build/qemu-serial.log` | PASS | |
| Git commit | `build/meta/m2-commit.txt` | PASS | |

## Invariants yang Diperiksa
1. Kernel adalah ELF64 x86_64.
2. Entry point sesuai linker script.
3. Kernel tidak memakai hosted libc.
4. Source dikompilasi dengan `-ffreestanding` dan `-mno-red-zone`.
5. Serial console tersedia sebelum subsistem kompleks.
6. Kernel tidak kembali setelah `kmain`.
7. Output QEMU disimpan sebagai log file.

## Failure Modes yang Diuji atau Dianalisis
| Failure mode | Pernah terjadi? | Diagnosis | Perbaikan |
|---|---|---|---|
| Toolchain salah | Tidak | - | - |
| OVMF tidak ditemukan | Tidak | - | - |
| Limine gagal fetch | Tidak | - | - |
| ISO gagal dibuat | Tidak | - | - |
| QEMU log kosong | Tidak | - | - |
| Entry point salah | Tidak | - | - |
| Reboot loop | Tidak | - | - |
| CRLF script | Tidak | - | - |
| ShellCheck error pada script | Ya | Target `make check-scripts` gagal (Error 1) karena `m2_preflight.sh` memiliki sintaks redirection tanpa command `> "$REPORT"`. | Menggantinya dengan operasi no-op `:` menjadi `: > "$REPORT"` menggunakan perintah sed. |

## Keputusan Readiness
[X] Lulus M2: siap uji QEMU tahap M2.

## Catatan Reviewer
