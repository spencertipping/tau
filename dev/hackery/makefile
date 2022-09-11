programs := fabric1 fabric1-static fabric2 continuation-perf msgpack xcb xlib-transparent xlib-glx

ALL: $(programs)

.PHONY: clean
clean:
	rm -f $(programs)

x%: x%.cc
	g++ -O3 -o $@ $< -lxcb -lX11 -lGL -lX11-xcb -lpthread \
	    -pthread -I/usr/include/pango-1.0 -I/usr/include/harfbuzz \
	    -I/usr/include/pango-1.0 -I/usr/include/libmount \
	    -I/usr/include/blkid -I/usr/include/fribidi \
	    -I/usr/include/harfbuzz -I/usr/include/cairo \
	    -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
	    -I/usr/include/pixman-1 -I/usr/include/uuid \
	    -I/usr/include/freetype2 -I/usr/include/libpng16 \
            -lpangocairo-1.0 -lpango-1.0 -lgobject-2.0 \
	    -lglib-2.0 -lharfbuzz -lcairo

epoll: epoll.cc
	g++ -O3 -g $@ $<

msgpack: msgpack.cc
	g++ -O3 -o msgpack msgpack.cc

fabric1: fabric1.cc
	g++ -O3 -o fabric1 fabric1.cc -lboost_fiber -lboost_context

fabric2: fabric2.cc
	g++ -O3 -o fabric2 fabric2.cc -lboost_fiber -lboost_context

callcc-nested: callcc-nested.cc
	g++ -O3 -o callcc-nested callcc-nested.cc -lboost_fiber -lboost_context

fabric1-static: fabric1.cc
	g++ -O3 -o fabric1-static fabric1.cc \
	          /usr/lib/x86_64-linux-gnu/libboost_context.a \
	          /usr/lib/x86_64-linux-gnu/libboost_fiber.a

continuation-perf: continuation-perf.cc
	g++ -O3 -o continuation-perf continuation-perf.cc -lboost_context
