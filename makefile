t/test.o: t/test.cc
	g++ -std=c++20 -O3 -g -o $@ -c $< -lboost_context
