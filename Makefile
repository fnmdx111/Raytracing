
all:
	g++ -pg -DFEAT_SHADOW -DFEAT_ANTIALIASING -DPROGRESS -O3 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -DFEAT_SHADOW -DPROGRESS -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

# -DFEAT_ANTIALIASING -DFEAT_SPECULAR_REFLECTION 
