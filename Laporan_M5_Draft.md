# Laporan Praktikum Modul 5: Physical Memory Management, Paging, dan Virtual Memory

**Disusun oleh:**
* Nama: Nabil (TAPQI)
* NIM: [Isi NIM-mu]
* Kelas: [Isi Kelasmu]

## 0. Metadata Laporan
| Atribut | Isi |
|---|---|
| Kode praktikum | `M5` |
| Tanggal praktikum | `[Tunggu modul]` |
| Repository | `~/src/mcsos` |
| Branch | `[Tunggu nama branch resmi]` |

## 3. Tujuan Praktikum
1. **PMM:** Menguraikan peta memori dari antarmuka Limine dan membuat alokator frame fisik berbasis Bitmap/Free List.
2. **VMM:** Menginisiasi tabel halaman 4-tingkat (PML4) untuk mode x86_64.
3. **Paging:** Memetakan alamat kernel ke Higher-Half Memory dan menangani Page Fault secara terstruktur.

## 9. Desain Teknis (Prediksi)
* **PMM:** Menggunakan metode [Bitmap / Free list - tunggu konfirmasi modul] karena efisiensi ruang pencarian untuk arsitektur memori berkapasitas di atas 2GB.
* **Granularitas:** Ukuran frame dipatok pada 4096 Bytes (4 KB).
* **VMM:** Menggunakan arsitektur 4-level paging standar AMD64/Intel64.

*(Bagian log, skrip bash, dan validasi QEMU akan diisi segera setelah skrip evaluasi M5 dari dosen dibagikan).*
