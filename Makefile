
all:
	g++ -O3 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

adv:
	g++ -O3 -DFEAT_DOF -DUSE_TBB -DFEAT_GLOSSY src/*.cc -Iinclude -I/usr/local/Cellar/tbb/4.4-20151115/include -L/usr/local/Cellar/tbb/4.4-20151115/lib -ltbb `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

adv-prof:
	g++ -pg -O3 -DFEAT_DOF -DUSE_TBB -DFEAT_GLOSSY src/*.cc -Iinclude -I/usr/local/Cellar/tbb/4.4-20151115/include -L/usr/local/Cellar/tbb/4.4-20151115/lib -ltbb `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -DPROGRESS -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

profiler:
	g++ -pg -O3 src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

# -DFEAT_ANTIALIASING -DFEAT_SPECULAR_REFLECTION 
