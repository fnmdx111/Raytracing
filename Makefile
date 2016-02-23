all:
	g++ -O2 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11
