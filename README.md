RemoteGL
========

An attempt at making an interprocess OpenGL ES 2 wrapper which uses C++ template metaprogramming to autogenerate the IPC calls. Why interprocess OpenGL ES 2? So that if the client's logic crashes, it can quicklly resume the GPU session without have to spend a bunch of time reloading the assets. Why C++ template metaprogramming? I don't want to write each call's IPC. I may have spent more time figuring out how to do the metaprogramming stuff than it would have taken to manually do the IPC. But hey, I learned a lot.

Instructions
------------
<pre>
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build .
cmake --build build
build/examples/server/server build/examples/cube/cube
</pre>
