node: node.o
	g++ -g -o node node.o -pthread
node.o: node.hpp node.cpp
	g++ -c -g node.cpp -pthread
clean:
	rm *~ node.o
