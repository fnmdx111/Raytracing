all:
	g++ -O2 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra

debug:
	g++ -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra
