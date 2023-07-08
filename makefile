cc_linux  = g++
cc_fast   = g++
cc_clang  = clang++
cc_debug  = g++
cc_wasm   = dev/emsdk em++
cc_wdebug = dev/emsdk em++

ar_linux  = ar
ar_fast   = ar
ar_clang  = ar
ar_debug  = ar
ar_wasm   = dev/emsdk emar
ar_wdebug = dev/emsdk emar

cflags = $(shell cat compile_flags.txt)

cflags_linux  = $(cflags) -O3 -flto -DBOOST_STACKTRACE_USE_NOOP
cflags_fast   = $(cflags) -O1 -DBOOST_STACKTRACE_USE_NOOP
cflags_clang  = $(cflags) -O0 -DBOOST_STACKTRACE_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -gdwarf-4 -DDEBUG
cflags_debug  = $(cflags) -O0 -DBOOST_STACKTRACE_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -g -DDEBUG
cflags_wasm   = $(cflags) -O3 -DBOOST_STACKTRACE_USE_NOOP -flto -fexceptions \
                -Wno-mathematical-notation-identifier-extension
cflags_wdebug = $(cflags) -O1 -DBOOST_STACKTRACE_USE_NOOP -g -fexceptions \
                -Wno-mathematical-notation-identifier-extension

ldflags_linux  = -flto
ldflags_fast   =
ldflags_clang  = -lboost_stacktrace_backtrace -lbacktrace
ldflags_debug  = -lboost_stacktrace_backtrace -lbacktrace
ldflags_wasm   = -flto -sASYNCIFY -sTOTAL_MEMORY=1024MB # -sSTACK_SIZE=1024KB -sASYNCIFY_STACK_SIZE=1048576
ldflags_wdebug =       -sASYNCIFY -sTOTAL_MEMORY=1024MB # -sSTACK_SIZE=1024KB -sASYNCIFY_STACK_SIZE=1048576

# NOTE: stack size args seem to have no effect for wasm


native_libs = -lsqlite3 -lboost_context -lzstd \
	      -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 \
	      -lharfbuzz -lcairo \
	      -lxcb -lX11 -lGL -lX11-xcb
libs_linux  = $(native_libs)
libs_fast   = $(native_libs)
libs_clang  = $(native_libs)
libs_debug  = $(native_libs)
libs_wasm   =
libs_wdebug =


# Define the source and object files
tau_cs   = $(wildcard tau/*.cc)
sigma_cs = $(wildcard sigma/*.cc)
try_cs   = $(wildcard try/*.cc)


try_bins_linux  = $(patsubst try/%.cc,        bin/%, $(try_cs)) \
	          $(patsubst try/linux/%.cc,  bin/%, $(try_cs_linux))
try_bins_fast   = $(patsubst try/%.cc,        bin/%-fast, $(try_cs)) \
	          $(patsubst try/fast/%.cc,   bin/%-fast, $(try_cs_fast))
try_bins_clang  = $(patsubst try/%.cc,        bin/%-clang, $(try_cs)) \
	          $(patsubst try/clang/%.cc,  bin/%-clang, $(try_cs_clang))
try_bins_debug  = $(patsubst try/%.cc,        bin/%-debug, $(try_cs)) \
	          $(patsubst try/debug/%.cc,  bin/%-debug, $(try_cs_debug))
try_bins_wasm   = $(patsubst try/%.cc,        bin/%.html, $(try_cs)) \
	          $(patsubst try/wasm/%.cc,   bin/%.html, $(try_cs_wasm))
try_bins_wdebug = $(patsubst try/%.cc,        bin/%-debug.js, $(try_cs)) \
	          $(patsubst try/wdebug/%.cc, bin/%-debug.js, $(try_cs_wdebug))


define target
tau_cs_$1   = $$(wildcard tau/$1/*.cc)
sigma_cs_$1 = $$(wildcard sigma/$1/*.cc)
try_cs_$1   = $$(wildcard try/$1/*.cc)

tau_os_$1   = $$(patsubst tau/%.cc,      bin/$1/tau-%.o,    $$(tau_cs)) \
	      $$(patsubst tau/$1/%.cc,   bin/$1/taup-%.o,   $$(tau_cs_$1))
sigma_os_$1 = $$(patsubst sigma/%.cc,    bin/$1/sigma-%.o,  $$(sigma_cs)) \
	      $$(patsubst sigma/$1/%.cc, bin/$1/sigmap-%.o, $$(sigma_cs_$1))
try_os_$1   = $$(patsubst try/%.cc,      bin/$1-bin/%.o,    $$(try_cs)) \
	      $$(patsubst try/$1/%.cc,   bin/$1-bin/%.o,    $$(try_cs_$1))

$1: $$(try_bins_$1) bin/tau-$1.a bin/sigma-$1.a

bin/tau-$1.a: $$(tau_os_$1)
	$(ar_$1) rcs $$@ $$^
bin/sigma-$1.a: $$(sigma_os_$1)
	$(ar_$1) rcs $$@ $$^

bin/$1/tau-%.o: tau/%.cc bin/$1/tau-%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<
bin/$1/taup-%.o: tau/$1/%.cc bin/$1/taup-%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<

bin/$1/sigma-%.o: sigma/%.cc bin/$1/sigma-%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<
bin/$1/sigmap-%.o: sigma/$1/%.cc bin/$1/sigmap-%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<

bin/$1-bin/%.o: try/%.cc bin/$1-bin/%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<
bin/$1-bin/%.o: try/$1/%.cc bin/$1-bin/%.d | bin
	$(cc_$1) $(cflags_$1) -c -o $$@ $$<
endef


$(foreach x, linux fast clang debug wasm wdebug, $(eval $(call target,$(x))))


top: fast wasm
	./test

all: linux fast wasm debug clang wdebug
	./test


linux-size: bin/sigma
	@echo -n "wc -c bin/sigma = "; wc -c bin/sigma

wasm-size: bin/sigma.js
	@echo -n "gzip -9 bin/sigma.js   = "; cat bin/sigma.js   | gzip -9 | wc -c
	@echo -n "gzip -9 bin/sigma.wasm = "; cat bin/sigma.wasm | gzip -9 | wc -c


bin/%: $(tau_os_linux) $(sigma_os_linux) bin/linux-bin/%.o
	$(cc_linux) -o $@ $^ $(libs_linux) $(ldflags_linux)
bin/%-fast: $(tau_os_fast) $(sigma_os_fast) bin/fast-bin/%.o
	$(cc_fast) -o $@ $^ $(libs_fast) $(ldflags_fast)
bin/%-clang: $(tau_os_clang) $(sigma_os_clang) bin/clang-bin/%.o
	$(cc_clang) -o $@ $^ $(libs_clang) $(ldflags_clang)
bin/%-debug: $(tau_os_debug) $(sigma_os_debug) bin/debug-bin/%.o
	$(cc_debug) -o $@ $^ $(libs_debug) $(ldflags_debug)
bin/%.html: $(tau_os_wasm) $(sigma_os_wasm) bin/wasm-bin/%.o
	$(cc_wasm) -o $@ $^ $(libs_wasm) $(ldflags_wasm)
bin/%-debug.js: $(tau_os_wdebug) $(sigma_os_wdebug) bin/wdebug-bin/%.o
	$(cc_wdebug) -o $@ $^ $(libs_wdebug) $(ldflags_wdebug)


bin:
	mkdir -p bin/linux  bin/linux-bin \
	         bin/fast   bin/fast-bin \
	         bin/clang  bin/clang-bin \
	         bin/debug  bin/debug-bin \
	         bin/wasm   bin/wasm-bin \
	         bin/wdebug bin/wdebug-bin


.PHONY: clean linux-clean fast-clean clang-clean debug-clean wasm-clean wdebug-clean
clean:
	rm -rf bin

linux-clean:
	rm -f bin/linux*/*

fast-clean:
	rm -f bin/fast*/*

clang-clean:
	rm -f bin/clang*/*

debug-clean:
	rm -f bin/debug*/*

wasm-clean:
	rm -f bin/wasm*/*

wdebug-clean:
	rm -f bin/wdebug*/*


# Header file tracing
header-deps: linux-header-deps fast-header-deps clang-header-deps \
             debug-header-deps \
	     wasm-header-deps wdebug-header-deps


define hdeps
$1-header-deps: $$(tau_os_$1:.o=.d) $$(sigma_os_$1:.o=.d) $$(try_os_$1:.o=.d)

-include $$(tau_os_$1:.o=.d)
-include $$(sigma_os_$1:.o=.d)

bin/$1/tau-%.d: tau/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst tau/%.cc, bin/$1/tau-%.o, $$<) -MF $$@ $$<
bin/$1/taup-%.d: tau/$1/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst tau/$1/%.cc, bin/$1/taup-%.o, $$<) -MF $$@ $$<

bin/$1/sigma-%.d: sigma/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst sigma/%.cc, bin/$1/sigma-%.o, $$<) -MF $$@ $$<
bin/$1/sigmap-%.d: sigma/$1/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst sigma/$1/%.cc, bin/$1/sigmap-%.o, $$<) -MF $$@ $$<

bin/$1-bin/%.d: try/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst try/%.cc, bin/$1-bin/%.o, $$<) -MF $$@ $$<
bin/$1-bin/%.d: try/$1/%.cc | bin
	$(cc_$1) $(cflags_$1) -MM -MT $$(patsubst try/$1/%.cc, bin/$1-bin/%.o, $$<) -MF $$@ $$<
endef


$(foreach x, linux fast clang debug wasm wdebug, $(eval $(call hdeps,$(x))))
