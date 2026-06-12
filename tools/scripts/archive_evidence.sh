#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
ARCHIVE_DIR="$ROOT/build/archive"
TIMESTAMP=$(date -u +"%Y%m%d_%H%M%S")
ARCHIVE_NAME="M1_evidence_${TIMESTAMP}.tar.gz"

mkdir -p "$ARCHIVE_DIR"

if [ ! -d "$ROOT/build/meta" ] || [ ! -d "$ROOT/build/proof" ]; then
  echo "ERROR: Evidence directories not found. Run 'make test' first." >&2
  exit 1
fi

echo "Archiving M1 evidence..."
tar -czvf "$ARCHIVE_DIR/$ARCHIVE_NAME" \
  -C "$ROOT" \
  build/meta/ \
  build/proof/ \
  build/repro/ \
  docs/readiness/ \
  docs/architecture/ \
  docs/security/ \
  .gitignore Makefile

echo "OK: Evidence archived to build/archive/$ARCHIVE_NAME"
