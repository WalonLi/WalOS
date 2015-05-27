/**
    Provide by Walon Li
    2015/5/27
**/

#ifndef WALOS_IO_H
#define WALOS_IO_H

#define BUILDIO(bwl, bw, type)                                          \
static inline void out##bwl(unsigned type value, int port)              \
{                                                                       \
    asm volatile("out" #bwl " %" #bw "0, %w1"                           \
                 : : "a"(value), "Nd"(port));                           \
    asm volatile("nop ; nop ; nop ;") ;                                 \
}                                                                       \
                                                                        \
static inline unsigned type in##bwl(int port)                           \
{                                                                       \
    unsigned type value;                                                \
    asm volatile("in" #bwl " %w1, %" #bw ""                             \
                 : "=a"(value) : "Nd"(port));                           \
    asm volatile("nop ; nop ; nop ;") ;                                 \
    return value;                                                       \
}


BUILDIO(b, b, char)
BUILDIO(w, w, short)
BUILDIO(l, , int)

#endif

