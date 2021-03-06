/**
    Provide by Walon Li
    2015/5/27
**/

#ifndef __IO_H__
#define __IO_H__

#define BUILDIO(bwl, bw, type)                                          \
static inline void out##bwl(unsigned type value, int port)              \
{                                                                       \
    __asm__ volatile("out" #bwl " %" #bw "0, %w1"                       \
                 : : "a"(value), "Nd"(port));                           \
    __asm__ volatile("nop ;\n nop ;\n nop ;") ;                             \
}                                                                       \
                                                                        \
static inline unsigned type in##bwl(int port)                           \
{                                                                       \
    unsigned type value;                                                \
    __asm__ volatile("in" #bwl " %w1, %" #bw "0"                         \
                 : "=a"(value) : "Nd"(port));                           \
    __asm__ volatile("nop ;\n nop ;\n nop ;") ;                             \
    return value;                                                       \
}


BUILDIO(b, b, char)
BUILDIO(w, w, short)
BUILDIO(l, , int)

#endif

