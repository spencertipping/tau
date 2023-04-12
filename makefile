linux_cc = g++
wasm_cc  = dev/emsdk em++

linux_cflags = $(shell cat compile_flags.txt) -O3
wasm_cflags  = $(shell cat compile_flags.txt) -O3 -flto -fexceptions \
               -Wno-mathematical-notation-identifier-extension

linux_ldflags =
wasm_ldflags  = -flto -sASYNCIFY -sTOTAL_MEMORY=1024MB

linux_libs = -lsqlite3 -lboost_context -lzstd
  #-lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0
  #-lharfbuzz -lcairo
  #-lxcb -lX11 -lGL -lX11-xcb


# Define the source and object files
tau_cs = $(wildcard tau/*.cc)
try_cs = $(wildcard try/*.cc)

tau_linux_os = $(patsubst tau/%.cc, bin/linux/%.o,     $(tau_cs))
try_linux_os = $(patsubst try/%.cc, bin/linux-bin/%.o, $(try_cs))
tau_wasm_os  = $(patsubst tau/%.cc, bin/wasm/%.o,      $(tau_cs))
try_wasm_os  = $(patsubst try/%.cc, bin/wasm-bin/%.o,  $(try_cs))

try_linux_bins = $(patsubst try/%.cc, bin/%,    $(try_cs))
try_wasm_bins  = $(patsubst try/%.cc, bin/%.js, $(try_cs))


all: linux wasm

linux: $(try_linux_bins) bin/tau-linux.a
wasm:  $(try_wasm_bins)  bin/tau-wasm.a


bin/tau-linux.a: $(tau_linux_os)
	ar rcs bin/tau-linux.a $^

bin/tau-wasm.a: $(tau_wasm_os)
	dev/emsdk emar rcs bin/tau-wasm.a $^

bin/%: $(tau_linux_os) bin/linux-bin/%.o
	$(linux_cc) $(linux_ldflags) -o $@ $^ $(linux_libs)

bin/%.js: $(tau_wasm_os) bin/wasm-bin/%.o
	$(wasm_cc) $(wasm_ldflags) -o $@ $^


bin/linux/%.o: tau/%.cc bin/linux/%.d | bin
	$(linux_cc) $(linux_cflags) -c -o $@ $<

bin/linux-bin/%.o: try/%.cc bin/linux-bin/%.d | bin
	$(linux_cc) $(linux_cflags) -c -o $@ $<

bin/wasm/%.o: tau/%.cc bin/wasm/%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<

bin/wasm-bin/%.o: try/%.cc bin/wasm-bin/%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<


bin:
	mkdir -p bin/linux bin/linux-bin \
                 bin/wasm  bin/wasm-bin


# Header file tracing
header-deps: linux-header-deps wasm-header-deps
linux-header-deps: $(tau_linux_os:.o=.d) $(try_linux_os:.o=.d)
wasm-header-deps:  $(tau_wasm_os:.o=.d)  $(try_wasm_os:.o=.d)

-include $(tau_linux_os:.o=.d)
-include $(tau_wasm_os:.o=.d)

bin/linux/%.d: tau/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst tau/%.cc, bin/linux/%.o, $<) -MF $@ $<

bin/linux-bin/%.d: try/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst try/%.cc, bin/linux-bin/%.o, $<) -MF $@ $<

bin/wasm/%.d: tau/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst tau/%.cc, bin/wasm/%.o, $<) -o $@ $<

bin/wasm-bin/%.d: try/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst try/%.cc, bin/wasm-bin/%.o, $<) -o $@ $<


# Clean up
.PHONY: clean linux-clean wasm-clean
clean:
	rm -rf bin

linux-clean:
	rm -f bin/linux*/*

wasm-clean:
	rm -f bin/wasm*/*
