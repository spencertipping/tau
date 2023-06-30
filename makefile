linux_cc = g++
debug_cc = g++
wasm_cc  = dev/emsdk em++

linux_cflags = $(shell cat compile_flags.txt) -O3
debug_cflags = $(shell cat compile_flags.txt) -O0 -g -DDEBUG
wasm_cflags  = $(shell cat compile_flags.txt) -O3 -flto -fexceptions \
               -Wno-mathematical-notation-identifier-extension

linux_ldflags =
debug_ldflags =
wasm_ldflags  = -flto -sASYNCIFY -sTOTAL_MEMORY=1024MB

linux_libs = -lsqlite3 -lboost_context -lzstd
debug_libs = -lsqlite3 -lboost_context -lzstd
  #-lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0
  #-lharfbuzz -lcairo
  #-lxcb -lX11 -lGL -lX11-xcb

wasm_libs =


# Define the source and object files
tau_cs   = $(wildcard tau/*.cc)
sigma_cs = $(wildcard sigma/*.cc)
try_cs   = $(wildcard try/*.cc)

tau_linux_cs   = $(wildcard tau/linux/*.cc)
sigma_linux_cs = $(wildcard sigma/linux/*.cc)
try_linux_cs   = $(wildcard try/linux/*.cc)
tau_wasm_cs    = $(wildcard tau/wasm/*.cc)
sigma_wasm_cs  = $(wildcard sigma/wasm/*.cc)
try_wasm_cs    = $(wildcard try/wasm/*.cc)

tau_linux_os   = $(patsubst tau/%.cc,         bin/linux/tau-%.o,    $(tau_cs)) \
		 $(patsubst tau/linux/%.cc,   bin/linux/taup-%.o,   $(tau_linux_cs))
sigma_linux_os = $(patsubst sigma/%.cc,       bin/linux/sigma-%.o,  $(sigma_cs)) \
		 $(patsubst sigma/linux/%.cc, bin/linux/sigmap-%.o, $(sigma_linux_cs))
try_linux_os   = $(patsubst try/%.cc,         bin/linux-bin/%.o,    $(try_cs)) \
		 $(patsubst try/linux/%.cc,   bin/linux-bin/%.o,    $(try_linux_cs))

tau_debug_os   = $(patsubst tau/%.cc,         bin/debug/tau-%.o,    $(tau_cs)) \
		 $(patsubst tau/debug/%.cc,   bin/debug/taup-%.o,   $(tau_debug_cs))
sigma_debug_os = $(patsubst sigma/%.cc,       bin/debug/sigma-%.o,  $(sigma_cs)) \
		 $(patsubst sigma/debug/%.cc, bin/debug/sigmap-%.o, $(sigma_debug_cs))
try_debug_os   = $(patsubst try/%.cc,         bin/debug-bin/%.o,    $(try_cs)) \
		 $(patsubst try/debug/%.cc,   bin/debug-bin/%.o,    $(try_debug_cs))

tau_wasm_os    = $(patsubst tau/%.cc,        bin/wasm/tau-%.o,    $(tau_cs)) \
		 $(patsubst tau/wasm/%.cc,   bin/wasm/taup-%.o,   $(tau_wasm_cs))
sigma_wasm_os  = $(patsubst sigma/%.cc,      bin/wasm/sigma-%.o,  $(sigma_cs)) \
                 $(patsubst sigma/wasm/%.cc, bin/wasm/sigmap-%.o, $(sigma_wasm_cs))
try_wasm_os    = $(patsubst try/%.cc,        bin/wasm-bin/%.o,    $(try_cs)) \
		 $(patsubst try/wasm/%.cc,   bin/wasm-bin/%.o,    $(try_wasm_cs))

try_linux_bins = $(patsubst try/%.cc,       bin/%,       $(try_cs)) \
		 $(patsubst try/linux/%.cc, bin/%,       $(try_linux_cs))
try_debug_bins = $(patsubst try/%.cc,       bin/%-debug, $(try_cs)) \
		 $(patsubst try/debug/%.cc, bin/%-debug, $(try_debug_cs))
try_wasm_bins  = $(patsubst try/%.cc,       bin/%.js,    $(try_cs)) \
		 $(patsubst try/wasm/%.cc,  bin/%.js,    $(try_wasm_cs))


all: linux wasm
	./test

linux: $(try_linux_bins) bin/tau-linux.a bin/sigma-linux.a
debug: $(try_debug_bins) bin/tau-debug.a bin/sigma-debug.a
wasm:  $(try_wasm_bins)  bin/tau-wasm.a  bin/sigma-wasm.a


linux-size: bin/sigma
	@echo -n "wc -c bin/sigma = "; wc -c bin/sigma

wasm-size: bin/sigma.js
	@echo -n "gzip -9 bin/sigma.js   = "; cat bin/sigma.js   | gzip -9 | wc -c
	@echo -n "gzip -9 bin/sigma.wasm = "; cat bin/sigma.wasm | gzip -9 | wc -c


bin/tau-linux.a: $(tau_linux_os)
	ar rcs $@ $^
bin/tau-debug.a: $(tau_debug_os)
	ar rcs $@ $^
bin/tau-wasm.a: $(tau_wasm_os)
	dev/emsdk emar rcs $@ $^

bin/sigma-linux.a: $(sigma_linux_os)
	ar rcs $@ $^
bin/sigma-debug.a: $(sigma_debug_os)
	ar rcs $@ $^
bin/sigma-wasm.a: $(sigma_wasm_os)
	dev/emsdk emar rcs $@ $^

bin/%: $(tau_linux_os) $(sigma_linux_os) bin/linux-bin/%.o
	$(linux_cc) $(linux_ldflags) -o $@ $^ $(linux_libs)
bin/%-debug: $(tau_debug_os) $(sigma_debug_os) bin/debug-bin/%.o
	$(debug_cc) $(debug_ldflags) -o $@ $^ $(debug_libs)
bin/%.js: $(tau_wasm_os) $(sigma_wasm_os) bin/wasm-bin/%.o
	$(wasm_cc) $(wasm_ldflags) -o $@ $^ $(wasm_libs)


bin/linux/tau-%.o: tau/%.cc bin/linux/tau-%.d | bin
	$(linux_cc) $(linux_cflags) -c -o $@ $<
bin/linux/taup-%.o: tau/linux/%.cc bin/linux/taup-%.d | bin
	$(linux_cc) $(linux_cflags) -c -o $@ $<

bin/linux/sigma-%.o: sigma/%.cc bin/linux/sigma-%.d bin/tau.pch | bin
	$(linux_cc) $(linux_cflags) -Ibin -c -o $@ $<
bin/linux/sigmap-%.o: sigma/linux/%.cc bin/linux/sigmap-%.d bin/tau.pch | bin
	$(linux_cc) $(linux_cflags) -Ibin -c -o $@ $<

bin/linux-bin/%.o: try/%.cc bin/linux-bin/%.d bin/tau.pch | bin
	$(linux_cc) $(linux_cflags) -Ibin -c -o $@ $<
bin/linux-bin/%.o: try/linux/%.cc bin/linux-bin/%.d bin/tau.pch | bin
	$(linux_cc) $(linux_cflags) -Ibin -c -o $@ $<


bin/debug/tau-%.o: tau/%.cc bin/debug/tau-%.d | bin
	$(debug_cc) $(debug_cflags) -c -o $@ $<
bin/debug/taup-%.o: tau/debug/%.cc bin/debug/taup-%.d | bin
	$(debug_cc) $(debug_cflags) -c -o $@ $<

bin/debug/sigma-%.o: sigma/%.cc bin/debug/sigma-%.d bin/tau.pch | bin
	$(debug_cc) $(debug_cflags) -Ibin -c -o $@ $<
bin/debug/sigmap-%.o: sigma/debug/%.cc bin/debug/sigmap-%.d bin/tau.pch | bin
	$(debug_cc) $(debug_cflags) -Ibin -c -o $@ $<

bin/debug-bin/%.o: try/%.cc bin/debug-bin/%.d bin/tau.pch | bin
	$(debug_cc) $(debug_cflags) -Ibin -c -o $@ $<
bin/debug-bin/%.o: try/debug/%.cc bin/debug-bin/%.d bin/tau.pch | bin
	$(debug_cc) $(debug_cflags) -Ibin -c -o $@ $<


bin/wasm/tau-%.o: tau/%.cc bin/wasm/tau-%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<
bin/wasm/taup-%.o: tau/wasm/%.cc bin/wasm/taup-%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<

bin/wasm/sigma-%.o: sigma/%.cc bin/wasm/sigma-%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<
bin/wasm/sigmap-%.o: sigma/wasm/%.cc bin/wasm/sigmap-%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<

bin/wasm-bin/%.o: try/%.cc bin/wasm-bin/%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<
bin/wasm-bin/%.o: try/wasm/%.cc bin/wasm-bin/%.d | bin
	$(wasm_cc) $(wasm_cflags) -c -o $@ $<


bin/%.pch: %.hh | bin
	$(linux_cc) $(linux_cflags) -c -x c++-header -o $@ $<


bin:
	mkdir -p bin/linux bin/linux-bin \
	         bin/debug bin/debug-bin \
                 bin/wasm  bin/wasm-bin


.PHONY: clean linux-clean debug-clean wasm-clean
clean:
	rm -rf bin

linux-clean:
	rm -f bin/linux*/*

debug-clean:
	rm -f bin/debug*/*

wasm-clean:
	rm -f bin/wasm*/*


# Header file tracing
header-deps: linux-header-deps debug-header-deps wasm-header-deps
linux-header-deps: $(tau_linux_os:.o=.d) $(sigma_linux_os:.o=.d) $(try_linux_os:.o=.d)
debug-header-deps: $(tau_debug_os:.o=.d) $(sigma_debug_os:.o=.d) $(try_debug_os:.o=.d)
wasm-header-deps:  $(tau_wasm_os:.o=.d)  $(sigma_wasm_os:.o=.d)  $(try_wasm_os:.o=.d)

-include $(tau_linux_os:.o=.d)
-include $(sigma_linux_os:.o=.d)
-include $(tau_debug_os:.o=.d)
-include $(sigma_debug_os:.o=.d)
-include $(tau_wasm_os:.o=.d)
-include $(sigma_wasm_os:.o=.d)


# Linux header deps
bin/linux/tau-%.d: tau/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst tau/%.cc, bin/linux/tau-%.o, $<) -MF $@ $<
bin/linux/taup-%.d: tau/linux/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst tau/linux/%.cc, bin/linux/taup-%.o, $<) -MF $@ $<

bin/linux/sigma-%.d: sigma/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst sigma/%.cc, bin/linux/sigma-%.o, $<) -MF $@ $<
bin/linux/sigmap-%.d: sigma/linux/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst sigma/linux/%.cc, bin/linux/sigmap-%.o, $<) -MF $@ $<

bin/linux-bin/%.d: try/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst try/%.cc, bin/linux-bin/%.o, $<) -MF $@ $<
bin/linux-bin/%.d: try/linux/%.cc | bin
	$(linux_cc) $(linux_cflags) -MM -MT $(patsubst try/linux/%.cc, bin/linux-bin/%.o, $<) -MF $@ $<


# Debug header deps
bin/debug/tau-%.d: tau/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst tau/%.cc, bin/debug/tau-%.o, $<) -MF $@ $<
bin/debug/taup-%.d: tau/debug/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst tau/debug/%.cc, bin/debug/taup-%.o, $<) -MF $@ $<

bin/debug/sigma-%.d: sigma/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst sigma/%.cc, bin/debug/sigma-%.o, $<) -MF $@ $<
bin/debug/sigmap-%.d: sigma/debug/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst sigma/debug/%.cc, bin/debug/sigmap-%.o, $<) -MF $@ $<

bin/debug-bin/%.d: try/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst try/%.cc, bin/debug-bin/%.o, $<) -MF $@ $<
bin/debug-bin/%.d: try/debug/%.cc | bin
	$(debug_cc) $(debug_cflags) -MM -MT $(patsubst try/debug/%.cc, bin/debug-bin/%.o, $<) -MF $@ $<


# WASM header deps
bin/wasm/tau-%.d: tau/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst tau/%.cc, bin/wasm/tau-%.o, $<) -o $@ $<
bin/wasm/taup-%.d: tau/wasm/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst tau/wasm/%.cc, bin/wasm/taup-%.o, $<) -o $@ $<

bin/wasm/sigma-%.d: sigma/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst sigma/%.cc, bin/wasm/sigma-%.o, $<) -o $@ $<
bin/wasm/sigmap-%.d: sigma/wasm/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst sigma/wasm/%.cc, bin/wasm/sigmap-%.o, $<) -o $@ $<

bin/wasm-bin/%.d: try/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst try/%.cc, bin/wasm-bin/%.o, $<) -o $@ $<
bin/wasm-bin/%.d: try/wasm/%.cc | bin
	$(wasm_cc) $(wasm_cflags) -MM -MT $(patsubst try/wasm/%.cc, bin/wasm-bin/%.o, $<) -o $@ $<
