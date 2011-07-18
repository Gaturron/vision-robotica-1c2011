CXXFLAGS := -g -O3 -fPIC -Wall `pkg-config --cflags opencv` -Ilibelas/src -msse2
LDFLAGS := `pkg-config --libs opencv` -lhighgui -L./libelas/build/ -lelas 

all: libelas/build/libelas.a vision main

vision: vision.o
	g++ -c vision.cpp -o vision.o $(CXXFLAGS)

libelas/build/libelas.a:
	cd libelas; mkdir build; cmake ..; make

clean:
	rm -rf *.o main

main: main.o vision.o
	g++ -o $@ main.o vision.o $(LDFLAGS)

