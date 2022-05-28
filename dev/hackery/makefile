ALL: fabric1 fabric2

fabric1: fabric1.cc
	g++ -o fabric1 fabric1.cc -lboost_fiber -lboost_context

fabric2: fabric2.cc
	g++ -o fabric2 fabric2.cc -lboost_fiber -lboost_context

.PHONY: clean
clean:
	rm fabric1 fabric2
