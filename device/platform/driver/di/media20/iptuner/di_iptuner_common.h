#ifndef __DI_IPTUNER_COMMON_H__
#define __DI_IPTUNER_COMMON_H__

/*****************************************************************************
 * Required system headers
 *****************************************************************************/
#include <stdarg.h>
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif

#include "htype.h"
#include "vkernel.h"

#ifndef __cplusplus
# include <stdbool.h>
#endif

/*****************************************************************************
 * Error values (shouldn't be exposed)
 *****************************************************************************/
#define DI_IPT_SUCCESS         -0                                   /* No error */
#define DI_IPT_ENOMEM          -1                          /* Not enough memory */
#define DI_IPT_ETHREAD         -2                               /* Thread error */
#define DI_IPT_ETIMEOUT        -3                                    /* Timeout */
#define DI_IPT_ENOMOD         -10                           /* Module not found */
#define DI_IPT_ENOOBJ         -20                           /* Object not found */
#define DI_IPT_ENOVAR         -30                         /* Variable not found */
#define DI_IPT_EBADVAR        -31                         /* Bad variable value */
#define DI_IPT_ENOITEM        -40                           /**< Item not found */
#define DI_IPT_EEXIT         -255                             /* Program exited */
#define DI_IPT_EEXITSUCCESS  -999                /* Program exited successfully */
#define DI_IPT_EGENERIC      -666                              /* Generic error */

/*****************************************************************************
 * Protocol String
 *****************************************************************************/
//#define DI_IPTUNER_USER_AGENT_STRING "HbbTV/0.5.0 (;;;;) CE-HTML/0.5.0 (; Humax; iCord; 1.00.03; 0.3) ANT/3.0.4"
#if 0
#define DI_IPTUNER_USER_AGENT_STRING "ANTGalio/3.0.6.22.46 CE-HTML/1.0 (; Humax; iCord; 1.00.11; 0.3) HbbTV/1.1.1 (;;;;;)"
#else
#define DI_IPTUNER_USER_AGENT_STRING "Linux 7335b0-smp; U; HbbTV/1.1.1 (+PVR;Humax;HMS;1.00.32;1.0;)ce-html/1.0 hdplusSmartTV/1.0 (NETRANGEMMH; );"
#endif

#define FREENULL(a) do { DD_MEM_Free ( a ); a = NULL; } while(0)

#define EMPTY_STR(str) (!str || !*str)

#ifndef __MAX
#   define __MAX(a, b)   ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef __MIN
#   define __MIN(a, b)   ( ((a) < (b)) ? (a) : (b) )
#endif


/* MSB (big endian)/LSB (little endian) conversions - network order is always
 * MSB, and should be used for both network communications and files. */
static inline HUINT16 U16_AT( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT16)p[0] << 8) | p[1] );
}
static inline HUINT32 U32_AT( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT32)p[0] << 24) | ((HUINT32)p[1] << 16)
              | ((HUINT32)p[2] << 8) | p[3] );
}
static inline HUINT64 U64_AT( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT64)p[0] << 56) | ((HUINT64)p[1] << 48)
              | ((HUINT64)p[2] << 40) | ((HUINT64)p[3] << 32)
              | ((HUINT64)p[4] << 24) | ((HUINT64)p[5] << 16)
              | ((HUINT64)p[6] << 8) | p[7] );
}

static inline HUINT16 GetWLE( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT16)p[1] << 8) | p[0] );
}
static inline HUINT32 GetDWLE( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT32)p[3] << 24) | ((HUINT32)p[2] << 16)
              | ((HUINT32)p[1] << 8) | p[0] );
}
static inline HUINT64 GetQWLE( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT64)p[7] << 56) | ((HUINT64)p[6] << 48)
              | ((HUINT64)p[5] << 40) | ((HUINT64)p[4] << 32)
              | ((HUINT64)p[3] << 24) | ((HUINT64)p[2] << 16)
              | ((HUINT64)p[1] << 8) | p[0] );
}

#define GetWBE( p )     U16_AT( p )
#define GetDWBE( p )    U32_AT( p )
#define GetQWBE( p )    U64_AT( p )

/* Helper writer functions */
#define SetWLE( p, v ) _SetWLE( (HUINT8*)(p), v)
static inline void _SetWLE( HUINT8 *p, HUINT16 i_dw )
{
    p[1] = ( i_dw >>  8 )&0xff;
    p[0] = ( i_dw       )&0xff;
}

#define SetDWLE( p, v ) _SetDWLE( (HUINT8*)(p), v)
static inline void _SetDWLE( HUINT8 *p, HUINT32 i_dw )
{
    p[3] = ( i_dw >> 24 )&0xff;
    p[2] = ( i_dw >> 16 )&0xff;
    p[1] = ( i_dw >>  8 )&0xff;
    p[0] = ( i_dw       )&0xff;
}
#define SetQWLE( p, v ) _SetQWLE( (HUINT8*)(p), v)
static inline void _SetQWLE( HUINT8 *p, HUINT64 i_qw )
{
    SetDWLE( p,   i_qw&0xffffffff );
    SetDWLE( p+4, ( i_qw >> 32)&0xffffffff );
}
#define SetWBE( p, v ) _SetWBE( (HUINT8*)(p), v)
static inline void _SetWBE( HUINT8 *p, HUINT16 i_dw )
{
    p[0] = ( i_dw >>  8 )&0xff;
    p[1] = ( i_dw       )&0xff;
}

#define SetDWBE( p, v ) _SetDWBE( (HUINT8*)(p), v)
static inline void _SetDWBE( HUINT8 *p, HUINT32 i_dw )
{
    p[0] = ( i_dw >> 24 )&0xff;
    p[1] = ( i_dw >> 16 )&0xff;
    p[2] = ( i_dw >>  8 )&0xff;
    p[3] = ( i_dw       )&0xff;
}
#define SetQWBE( p, v ) _SetQWBE( (HUINT8*)(p), v)
static inline void _SetQWBE( HUINT8 *p, HUINT64 i_qw )
{
    SetDWBE( p+4,   i_qw&0xffffffff );
    SetDWBE( p, ( i_qw >> 32)&0xffffffff );
}

#endif /* !__DI_IPTUNER_COMMON_H__ */
