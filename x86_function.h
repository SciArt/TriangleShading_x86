/* ============================================================================
   [PROJECT]

   Name: Triangle Shading
   Desc: Drawing a triangle based on Gouraud shading
   Arch: x86_64
   Lang: C++, ASM (Intel syntax)
   Tech: AVX, AVX2
   Libs: SFML 2.4

   [FILE]
   
   Name: x86_function.h
   Desc: Declaration of the x86_function

   Auth: Gerard Wypych
==============================================================================*/

#ifndef X86_FUNCTION_H_
#define X86_FUNCTION_H_

extern "C" void x86_function( void* vertices, void* pixels, int width, int height, char* mask );

#endif // X86_FUNCTION_H_
