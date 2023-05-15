#ifndef __DI_IPTUNER_COMMON_H__
#define __DI_IPTUNER_COMMON_H__ 

/*****************************************************************************
 * Required system headers
 *****************************************************************************/
#include <stdarg.h>
#include <stdio.h>

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
#if defined(HDR_FOX_T2) || defined(HD_FOX_T2) || defined(HD5500T_PATCH)
#define DI_IPTUNER_USER_AGENT_STRING "HbbTV/1.1.1 (+PVR;;;;) ANTGalio/3.1.5.24.04"
#else
#define DI_IPTUNER_USER_AGENT_STRING "ANTGalio/3.0.6.22.46 CE-HTML/1.0 (; Humax; iCord; 1.00.11; 0.3) HbbTV/1.1.1 (;;;;;)"
#endif

#define FREENULL(a) do { DD_MEM_Free ( a ); a = NULL; } while(0)

#define EMPTY_STR(str) (!str || !*str)

#ifndef __MAX
#define __MAX(a, b)   ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef __MIN
#define __MIN(a, b)   ( ((a) < (b)) ? (a) : (b) )
#endif

/* MSB (big endian)/LSB (little endian) conversions - network order is always
 * MSB, and should be used for both network communications and files. */
static inline HUINT16 Convert_U16( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT16)p[0] << 8) | p[1] );
}
static inline HUINT32 Convert_U32( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT32)p[0] << 24) | ((HUINT32)p[1] << 16)
              | ((HUINT32)p[2] << 8) | p[3] );
}
static inline HUINT64 Convert_U64( const void * _p )
{
    const HUINT8 * p = (const HUINT8 *)_p;
    return ( ((HUINT64)p[0] << 56) | ((HUINT64)p[1] << 48)
              | ((HUINT64)p[2] << 40) | ((HUINT64)p[3] << 32)
              | ((HUINT64)p[4] << 24) | ((HUINT64)p[5] << 16)
              | ((HUINT64)p[6] << 8) | p[7] );
}

static inline HUINT16 GetWordLE( const void * _data )
{
    const HUINT8 * data = (const HUINT8 *)_data;
    return ( ((HUINT16)data[1] << 8) | data[0] );
}

static inline HUINT32 GetDWordLE( const void * _data )
{
    const HUINT8 * data = (const HUINT8 *)_data;
    return ( ((HUINT32)data[3] << 24) | ((HUINT32)data[2] << 16)
              | ((HUINT32)data[1] << 8) | data[0] );
}
static inline HUINT64 GetQWordLE( const void * _data )
{
    const HUINT8 * data = (const HUINT8 *)_data;
    return ( ((HUINT64)data[7] << 56) | ((HUINT64)data[6] << 48)
              | ((HUINT64)data[5] << 40) | ((HUINT64)data[4] << 32)
              | ((HUINT64)data[3] << 24) | ((HUINT64)data[2] << 16)
              | ((HUINT64)data[1] << 8) | data[0] );
}

#define GetWordBE( p ) Convert_U16( p )
#define GetDWordBE( p ) Convert_U32( p )
#define GetQWordBE( p ) Convert_U64( p )

/* Helper writer functions */

#define SetWordLE( p, v ) _SetWordLE( (HUINT8*)(p), v)
static inline void _SetWordLE( HUINT8 *data, HUINT16 i_dw )
{
    data[1] = ( i_dw >>  8 )&0xff;
    data[0] = ( i_dw )&0xff;
}


#define SetDWordLE( p, v ) _SetDWordLE( (HUINT8*)(p), v)
static inline void _SetDWordLE( HUINT8 *data, HUINT32 i_dw )
{
    data[3] = ( i_dw >> 24 )&0xff;
    data[2] = ( i_dw >> 16 )&0xff;
    data[1] = ( i_dw >>  8 )&0xff;
    data[0] = ( i_dw       )&0xff;
}

#define SetQWordLE( p, v ) _SetQWordLE( (HUINT8*)(p), v)
static inline void _SetQWordLE( HUINT8 *data, HUINT64 i_qw )
{
    SetDWordLE( data,   i_qw&0xffffffff );
    SetDWordLE( data+4, ( i_qw >> 32)&0xffffffff );
}

#define SetWordBE( p, v ) _SetWordBE( (HUINT8*)(p), v)
static inline void _SetWordBE( HUINT8 *data, HUINT16 i_dw )
{
    data[0] = ( i_dw >>  8 )&0xff;
    data[1] = ( i_dw       )&0xff;
}

#define SetDWordBE( p, v ) _SetDWordBE( (HUINT8*)(p), v)
static inline void _SetDWordBE( HUINT8 *data, HUINT32 i_dw )
{
    data[0] = ( i_dw >> 24 )&0xff;
    data[1] = ( i_dw >> 16 )&0xff;
    data[2] = ( i_dw >>  8 )&0xff;
    data[3] = ( i_dw       )&0xff;
}

#define SetQWordBE( p, v ) _SetQWordBE( (HUINT8*)(p), v)
static inline void _SetQWordBE( HUINT8 *data, HUINT64 i_qw )
{
    SetDWordBE( data+4,   i_qw&0xffffffff );
    SetDWordBE( data, ( i_qw >> 32)&0xffffffff );
}

#endif /* !__DI_IPTUNER_COMMON_H__ */
