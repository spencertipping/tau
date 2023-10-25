cc_server = g++
cc_sfast  = g++
cc_sprof  = g++
cc_sdebug = g++
cc_linux  = g++
cc_fast   = g++
cc_clang  = clang++
cc_debug  = g++
cc_wasm   = dev/emsdk em++
cc_wdebug = dev/emsdk em++

ar_server = ar
ar_sfast  = ar
ar_sprof  = ar
ar_sdebug = ar
ar_linux  = ar
ar_fast   = ar
ar_clang  = ar
ar_debug  = ar
ar_wasm   = dev/emsdk emar
ar_wdebug = dev/emsdk emar

cflags = $(shell cat compile_flags.txt)

# NOTE: -flto crashes the linker when building asqi
cflags_server = $(cflags) -O3 -DBOOST_STACKTRACE_USE_NOOP -Dτdebug=0 -Dτallow_todo=1
cflags_sfast  = $(cflags) -O1 -DBOOST_STACKTRACE_USE_NOOP
cflags_sprof  = $(cflags) -O2 -DBOOST_STACKTRACE_USE_NOOP -pg
cflags_sdebug = $(cflags) -O0 -DBOOST_STACKTRACE_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -g -DDEBUG
cflags_linux  = $(cflags) -O3 -DBOOST_STACKTRACE_USE_NOOP -Dτgl=1 -Dτdebug=0 -Dτallow_todo=1
cflags_fast   = $(cflags) -O1 -DBOOST_STACKTRACE_USE_NOOP -Dτgl=1
cflags_clang  = $(cflags) -O0 -DBOOST_STACKTRACE_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -gdwarf-4 -DDEBUG -Dτgl=1
cflags_debug  = $(cflags) -O0 -DBOOST_STACKTRACE_LINK -DBOOST_STACKTRACE_USE_BACKTRACE -g -DDEBUG -Dτgl=1
cflags_wasm   = $(cflags) -O3 -DBOOST_STACKTRACE_USE_NOOP -Dτgl=1 -flto -fexceptions \
                -Wno-mathematical-notation-identifier-extension
cflags_wdebug = $(cflags) -O1 -DBOOST_STACKTRACE_USE_NOOP -Dτgl=1 -g -fexceptions \
                -Wno-mathematical-notation-identifier-extension

ldflags_server =
ldflags_sfast  =
ldflags_sprof  = -pg
ldflags_sdebug = -lboost_stacktrace_backtrace -lbacktrace
ldflags_linux  =
ldflags_fast   =
ldflags_clang  = -lboost_stacktrace_backtrace -lbacktrace
ldflags_debug  = -lboost_stacktrace_backtrace -lbacktrace
ldflags_wasm   = -flto -sASYNCIFY -sTOTAL_MEMORY=1024MB # -sSTACK_SIZE=1024KB -sASYNCIFY_STACK_SIZE=1048576
ldflags_wdebug =       -sASYNCIFY -sTOTAL_MEMORY=1024MB # -sSTACK_SIZE=1024KB -sASYNCIFY_STACK_SIZE=1048576

# NOTE: stack size args seem to have no effect for wasm


server_libs = -lsqlite3 -lboost_context -lzstd -llmdb
native_libs = $(server_libs) \
	      -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 -lglib-2.0 \
	      -lharfbuzz -lcairo \
	      -lxcb -lxcb-keysyms -lX11 -lGL -lX11-xcb
libs_server = $(server_libs)
libs_sfast  = $(server_libs)
libs_sprof  = $(server_libs)
libs_sdebug = $(server_libs)
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


try_bins_server = $(patsubst try/%.cc,        bin/%-server, $(try_cs)) \
	          $(patsubst try/server/%.cc, bin/%-server, $(try_cs_server))
try_bins_sfast  = $(patsubst try/%.cc,        bin/%-sfast, $(try_cs)) \
	          $(patsubst try/sfast/%.cc,  bin/%-sfast, $(try_cs_sfast))
try_bins_sprof  = $(patsubst try/%.cc,        bin/%-sprof, $(try_cs)) \
	          $(patsubst try/sprof/%.cc,  bin/%-sprof, $(try_cs_sprof))
try_bins_sdebug = $(patsubst try/%.cc,        bin/%-sdebug, $(try_cs)) \
	          $(patsubst try/sdebug/%.cc, bin/%-sdebug, $(try_cs_sdebug))
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


$(foreach x, server sfast sprof sdebug linux fast clang debug wasm wdebug, $(eval $(call target,$(x))))


t: fast
	./test

top: fast wasm
	./test

all: server sfast sprof sdebug linux fast wasm debug clang wdebug
	./test

bench: linux-bench wasm-bench


.PHONY: linux-size wasm-size
linux-size: bin/sigma
	@echo -n "wc -c bin/sigma = "; wc -c bin/sigma

wasm-size: bin/sigma.js
	@echo -n "gzip -9 bin/sigma.js   = "; cat bin/sigma.js   | gzip -9 | wc -c
	@echo -n "gzip -9 bin/sigma.wasm = "; cat bin/sigma.wasm | gzip -9 | wc -c


linux-bench: linux
	bin/xi-bench

wasm-bench: wasm
	node bin/xi-bench.js


bin/%-server: $(tau_os_server) $(sigma_os_server) bin/server-bin/%.o
	$(cc_server) -o $@ $^ $(libs_server) $(ldflags_server)
bin/%-sfast: $(tau_os_sfast) $(sigma_os_sfast) bin/sfast-bin/%.o
	$(cc_sfast) -o $@ $^ $(libs_sfast) $(ldflags_sfast)
bin/%-sprof: $(tau_os_sprof) $(sigma_os_sprof) bin/sprof-bin/%.o
	$(cc_sprof) -o $@ $^ $(libs_sprof) $(ldflags_sprof)
bin/%-sdebug: $(tau_os_sdebug) $(sigma_os_sdebug) bin/sdebug-bin/%.o
	$(cc_sdebug) -o $@ $^ $(libs_sdebug) $(ldflags_sdebug)
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
	mkdir -p bin/server bin/server-bin \
	         bin/sfast  bin/sfast-bin \
	         bin/sprof  bin/sprof-bin \
	         bin/sdebug bin/sdebug-bin \
		 bin/linux  bin/linux-bin \
	         bin/fast   bin/fast-bin \
	         bin/clang  bin/clang-bin \
	         bin/debug  bin/debug-bin \
	         bin/wasm   bin/wasm-bin \
	         bin/wdebug bin/wdebug-bin


.PHONY: clean server-clean sfast-clean sprof-clean sdebug-clean linux-clean fast-clean clang-clean debug-clean wasm-clean wdebug-clean
clean:
	rm -rf bin

server-clean:
	rm -f bin/server*/*

sfast-clean:
	rm -f bin/sfast*/*

sprof-clean:
	rm -f bin/sprof*/*

sdebug-clean:
	rm -f bin/sdebug*/*

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
header-deps: server-header-deps sfast-header-deps sprof-header-deps sdebug-header-deps \
             linux-header-deps fast-header-deps clang-header-deps \
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


$(foreach x, server sfast sprof sdebug linux fast clang debug wasm wdebug, $(eval $(call hdeps,$(x))))
