#!/usr/bin/env bash
set -euo pipefail

out="evidence/M4"
mkdir -p "$out"

cp -f build/kernel.elf "$out/" 2>/dev/null || true
cp -f build/kernel.map "$out/" 2>/dev/null || true
cp -f build/kernel.syms.txt "$out/" 2>/dev/null || true
cp -f build/kernel.disasm.txt "$out/" 2>/dev/null || true
cp -f build/kernel.readelf.header.txt "$out/" 2>/dev/null || true
cp -f build/kernel.readelf.programs.txt "$out/" 2>/dev/null || true
cp -f build/m4-qemu-serial.log "$out/" 2>/dev/null || true
cp -f build/m4-qemu-breakpoint.log "$out/" 2>/dev/null || true

{
    echo "MCSOS M4 evidence manifest"
    date -u +"timestamp_utc=%Y-%m-%dT%H:%M:%SZ"
    git rev-parse HEAD 2>/dev/null | sed 's/^/commit=/' || true
    clang --version | head -n 1 | sed 's/^/clang=/' || true
    ld.lld --version | head -n 1 | sed 's/^/lld=/' || true
    qemu-system-x86_64 --version 2>/dev/null | head -n 1 | sed 's/^/qemu=/' || true
    find "$out" -maxdepth 1 -type f -printf '%f\n' | LC_ALL=C sort
} > "$out/manifest.txt"

echo "[M4] [PASS] Evidence dikumpulkan di $out"
