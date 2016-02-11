all:
	g++ -O2 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra
