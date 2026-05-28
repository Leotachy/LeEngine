mkdir bin
glslangValidator.exe -V src/basic.vert -o bin/basic.vert.spv
glslangValidator.exe -V src/basic.frag -o bin/basic.frag.spv
PAUSE