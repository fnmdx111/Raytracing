LIBPATH += -L"/usr/local/lib"
INCPATH += -Iinclude
INCPATH += -I"/usr/local/include"

LIBRARIES = `pkg-config --libs OpenEXR` -ltbb `pkg-config --libs sdl2`
INCLUDES = `pkg-config --cflags OpenEXR` `pkg-config --cflags sdl2`

AFEATS = -DNDEBUG -DPROGRESS -DFEAT_DOF -DUSE_TBB -DFEAT_GLOSSY
EFEATS = -DNDEBUG -DPROGRESS -DFEAT_DOF -DGCUTTHRU -DUSE_TBB -DVISPROG -DFEAT_GLOSSY -DFEAT_REFRACT

all:
	g++ -O3 -DNDEBUG -DPROGRESS src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

adv:
	g++ -O3 $(AFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

exp:
	g++ -O3 $(EFEATS) src/*.cc $(LIBPATH) $(INCPATH) $(INCLUDES) $(LIBRARIES) -o raytra --std=c++11

dbg-exp:
	g++ -g -DFEAT_DOF -DFEAT_GLOSSY -DFEAT_REFRACT src/*.cc -Iinclude -I/usr/local/tbb/include -L/usr/local/Cellar/tbb/4.4-20151115/lib -ltbb `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

adv-prof:
	g++ -pg -O3 -DNDEBUG -DFEAT_DOF -DUSE_TBB -DFEAT_GLOSSY src/*.cc -Iinclude -I/usr/local/Cellar/tbb/4.4-20151115/include -L/usr/local/Cellar/tbb/4.4-20151115/lib -ltbb `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

debug:
	g++ -DPROGRESS -D__DEBUG__ src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

profiler:
	g++ -pg -O3 -DNDEBUG src/*.cc -Iinclude `pkg-config --cflags OpenEXR` `pkg-config --libs OpenEXR` -o raytra --std=c++11

# -DFEAT_ANTIALIASING -DFEAT_SPECULAR_REFLECTION 
