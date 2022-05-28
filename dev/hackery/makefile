ALL: fabric1 fabric2 continuation-perf

fabric1: fabric1.cc
	g++ -o fabric1 fabric1.cc -lboost_fiber -lboost_context

fabric2: fabric2.cc
	g++ -o fabric2 fabric2.cc -lboost_fiber -lboost_context

continuation-perf: continuation-perf.cc
	g++ -o continuation-perf continuation-perf.cc -lboost_context

.PHONY: clean
clean:
	rm fabric1 fabric2 continuation-perf
