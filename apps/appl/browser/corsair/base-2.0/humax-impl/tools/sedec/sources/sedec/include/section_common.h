#ifndef __COMMON__THKANG2___
#define __COMMON__THKANG2___

#include <stdio.h>
#if defined(CONFIG_DEBUG)
#include "hlib.h"
#endif
//#define SECTION_DEBUG	printf

#if defined(CONFIG_DEBUG)
#define LOG_PREFIX  "sedec: "
#define SECTION_DEBUG(fmt, ...)\
        do { \
        char *buf = NULL;\
        int size = sizeof(fmt)+sizeof(LOG_PREFIX);\
        buf = (char *)calloc(size,sizeof(char));\
        snprintf(buf,size,"%s%s",LOG_PREFIX,fmt);\
        HxLOG_Print(buf,##__VA_ARGS__);\
        if ( buf ){ free(buf); }\
    } while (0)
#else
#define SECTION_DEBUG(fmt, ...)
#endif

#endif

