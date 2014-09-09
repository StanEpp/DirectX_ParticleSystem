This project requires DirectX 11 and thus a graphics card which supports this version. It is not possible to run this demo 
on older hardware because I haven't implemented an alternative way of computing the particle's position.

[KEYS]
TAB = Show FPS and mouse coordinates
Press and hold right mouse key = Camera won't move anymore and by pressing the left mouse key the particles will be attracted by the cursor
ESC = exit demo

[COMMENT]
The algorithm for the computation of the particle's position can be found in "cs.hlsl". This is the code for the compute shader which is
getting compiled during runtime.
