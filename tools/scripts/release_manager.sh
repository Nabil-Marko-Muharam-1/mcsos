#!/bin/bash
echo ""
echo "=== [M16] NABIL OS RELEASE & UPDATE MANAGER ==="

# Simulasi Rollback Check
if [ -f "build/mcsos.iso" ]; then
    echo "[BACKUP] Mengamankan versi sebelumnya ke build/mcsos.iso.bak (Rollback Ready)..."
    cp build/mcsos.iso build/mcsos.iso.bak
fi

# Simulasi Update & Release
if [ -f "build/mcsos.iso" ]; then
    echo "[RELEASE] Mencetak Production Release Image..."
    cp build/mcsos.iso build/NABIL_OS_v1.0_Release.iso
    echo "[SUCCESS] Image rilis siap diedarkan: build/NABIL_OS_v1.0_Release.iso"
else
    echo "[ERROR] File ISO utama tidak ditemukan! Lakukan 'make build' terlebih dahulu."
fi
echo "==============================================="
echo ""
