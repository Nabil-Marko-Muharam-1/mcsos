.PHONY: meta check smoke clean

meta:
	./tools/check_env.sh

check:
	@echo "Checking scripts..."
	shellcheck tools/*.sh

smoke:
	@echo "Menjalankan smoke test compile..."
	mkdir -p build/smoke
	clang -target x86_64-pc-none-elf -ffreestanding -mno-red-zone -c smoke/smoke.c -o build/smoke/freestanding.o
	@echo " "
	@echo "=== BUKTI SMOKE TEST (READELF) ==="
	readelf -h build/smoke/freestanding.o
	@echo " "
	@echo "=== BUKTI SMOKE TEST (FILE) ==="
	file build/smoke/freestanding.o

clean:
	rm -rf build/
