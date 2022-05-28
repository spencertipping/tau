ALL: fabric1 fabric1-static fabric2 continuation-perf

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

.PHONY: clean
clean:
	rm fabric1 fabric1-static fabric2 continuation-perf
