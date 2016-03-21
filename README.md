Course project for Computer Graphics
====

This project is a raytracing renderer.

Main features
----

* Limited scene primitives (sphere, triangle, and plane)
* Bounding Volume Hierarchy intersection acceleration
* Wavefront obj support
* Soft shadow
* Reflections
* Renderer's progress report (enabled via compiler flag `-DTPROGRESS`)


Experimental features
----

* Glossy effect (enabled via compiler flag `-DFEAT_GLOSSY`)
* Refractions (enabled via compiler flag `-DFEAT_REFRACT`)
* Depth of field effect (simple camera model) (enabled via compiler flag `-DFEAT_DOF`)
* Multicored rendering (enabled via compiler flag `-DMULTITHD`, need Intel Threading Building Blocks)
* Glossy ray tracing pruning (see all Cut-through in unstable features, enabled via compiler flag `-DGPRUNING`). This feature will speed up the performance for glossy effects by about 5%.
* Visualized rendering progress (enabled via compiler flag `-DVPROGRESS`, libsdl2 and TBB are required); Tiled rendering is unstable, so this feature won't support visualizing it. This feature will add about 8% performance overhead.


Unstable features
----

* Tiled rendering (enabled via compiler flag `-DUSE_TILING`) (Warning: it actually makes rendering much slower, I guess it's because of all the overheads, but I'm not sure.)
* Pruning, i.e. in distribution ray tracing, when the difference between this sample and the last sample is small enough, no more samples are taken. It could accelerate the sampling process in ray tracing. (available with compiler flag `-D(D|G|S)PRUNING`) (It actually doesn't improve the rendering speed much, so it's off by default.)


Makefile
----

Do edit the makefile to fit your needs.

