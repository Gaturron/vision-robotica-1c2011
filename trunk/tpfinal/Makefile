CXXFLAGS := -g -O3 -fPIC -Wall `pkg-config --cflags opencv` -Ilibelas/src -msse2
LDFLAGS := `pkg-config --libs opencv` -lhighgui -L./libelas/build/ -lelas

all: libelas/build/libelas.a main

libelas/build/libelas.a:
	cd libelas; mkdir build; cmake ..; make

clean:
	rm -rf *.o main

main: main.o
	g++ -o $@ main.o $(LDFLAGS)


