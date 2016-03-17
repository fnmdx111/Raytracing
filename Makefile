
all:
	g++ -O3 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

dof:
	g++ -O3 src/*.cc -DFEAT_DOF -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -DPROGRESS -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

profiler:
	g++ -pg -O3 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11


# -DFEAT_ANTIALIASING -DFEAT_SPECULAR_REFLECTION 
