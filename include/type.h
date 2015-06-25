/**
    Provide by Walon Li
    2015/5/22
**/

#ifndef __TYPE_H__
#define __TYPE_H__

typedef signed      char    int8_t ;
typedef unsigned    char    uint8_t ;
typedef signed      short   int16_t ;
typedef unsigned    short   uint16_t ;
typedef signed      int     int32_t ;
typedef unsigned    int     uint32_t ;
typedef signed      long long int64_t ;
typedef unsigned    long long uint64_t ;

typedef	char *va_list;

#define bool _Bool
#define true 1
#define false 0

/**
 * MESSAGE mechanism is borrowed from MINIX
 */
struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	u64	m3l1;
	u64	m3l2;
	void*	m3p1;
	void*	m3p2;
};

typedef struct _MESSAGE{
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} MESSAGE;

#endif
