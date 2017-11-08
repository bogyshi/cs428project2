node: node.o
	g++ -g -o node node.o -pthread
node.o: node.hpp node.cpp
	g++ -c -g node.cpp -pthread
control: control.o
	g++ -g -o control control.o
control.o: control.cpp control.hpp
	g++ -c -g control.cpp
all: node control
clean:
	rm *~ node.o node control control.o
