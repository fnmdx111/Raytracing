all:
	g++ -DANTIALIASING -DSPECULAR_REFLECTION -O2 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -DANTIALIASING -DSPECULAR_REFLECTION -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11
