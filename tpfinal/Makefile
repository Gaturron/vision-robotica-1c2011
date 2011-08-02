CXXFLAGS := -g -O3 -fPIC -Wall `pkg-config --cflags opencv` -Ilibelas/src -msse2
LDFLAGS := `pkg-config --libs opencv` -lhighgui -L./libexabot-remote/ -L./libelas/build/ -lelas

all: libelas/build/libelas.a libcam libexabot-remote/libexabot-remote.so vision main

vision: vision.o
	g++ -c vision.cpp -o vision.o $(CXXFLAGS)

libcam:	
	cd libcam; make

libexabot-remote/libexabot-remote.so:
	cd libexabot-remote; mkdir build; cmake ..; make

libelas/build/libelas.a:
	cd libelas; mkdir build; cmake ..; make

clean:
	rm -rf *.o main;
	
main: main.o vision.o 
	g++ -o $@ main.o vision.o libcam/libcam.o libexabot-remote/libexabot-remote.so $(LDFLAGS)

