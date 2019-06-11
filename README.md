# Shading Triangle - x86_64, AVX, AVX2, C++

Drawing shaded triangle (Gouraud shading). Project is written in two languages: C++ and x86_64 assembler (Intel syntax).

### C++
- window managment, interaction with user, drawing grid (background)...

### Assembler (x86_64, Intel syntax)
- function that is used for drawing shaded triangle
- floating point numbers were used for calculations 
- in order to optimize the program, coordinates and colors are calculated simultaneously (AVX, AVX2)

## Build instructions

- SFML
`sudo apt-get install libsfml-dev`
- NASM
`sudo apt-get install nasm`

To build project:
`make`

## Contact
If you want to ask me something about this project, feel free to contact me via e-mail: contact@sci-art.net

## Video
https://youtu.be/qqHOqkGMuxY

[![YouTube Video](https://img.youtube.com/vi/qqHOqkGMuxY/0.jpg)](https://youtu.be/qqHOqkGMuxY)
