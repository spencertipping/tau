programs := fabric1 fabric1-static fabric2 continuation-perf msgpack xcb xlib-transparent xlib-glx

ALL: $(programs)

.PHONY: clean
clean:
	rm -f $(programs)

x%: x%.cc
	g++ -O3 -o $@ $< -lxcb -lX11 -lGL -lX11-xcb

msgpack: msgpack.cc
	g++ -O3 -o msgpack msgpack.cc

fabric1: fabric1.cc
	g++ -O3 -o fabric1 fabric1.cc -lboost_fiber -lboost_context

fabric2: fabric2.cc
	g++ -O3 -o fabric2 fabric2.cc -lboost_fiber -lboost_context

fabric1-static: fabric1.cc
	g++ -O3 -o fabric1-static fabric1.cc \
	          /usr/lib/x86_64-linux-gnu/libboost_context.a \
	          /usr/lib/x86_64-linux-gnu/libboost_fiber.a

continuation-perf: continuation-perf.cc
	g++ -O3 -o continuation-perf continuation-perf.cc -lboost_context
