/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMINT64_H__
#define __DRMINT64_H__

#include <drmnamespace.h>   /* defining proper namespace (if used) */


#if DRM_NO_NATIVE_64_TYPES

/* DRM_INT64 and DRM_UINT64 keeps 2 32 bit values
** val[0] keeps low 32 bit 
** val[1] keeps high 32 bit
** This is valid for both big and little endian CPUs
*/

typedef struct _DRM_INT64 { 
    unsigned long val[2];
} DRM_INT64;

typedef struct _DRM_UINT64 {   
    unsigned long val[2];
} DRM_UINT64;


extern DRM_API DRM_INT64 DRM_CALL DRM_I64Add(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64Sub(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64Mul(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64Div(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64Mod(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64And(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64ShR(const DRM_INT64 a, const int unsigned b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64ShL(const DRM_INT64 a, const int unsigned b);
extern DRM_API int       DRM_CALL DRM_I64Eql(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API int       DRM_CALL DRM_I64Les(const DRM_INT64 a, const DRM_INT64 b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64(const long int b);
extern DRM_API DRM_INT64 DRM_CALL DRM_I64Asgn(const long int a, const long int b);
extern DRM_API DRM_INT64 DRM_CALL DRM_UI2I64(const DRM_UINT64 b);
extern DRM_API unsigned long int DRM_CALL DRM_I64ToUI32(const DRM_INT64 b);

extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64Add(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64Sub(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64Mul(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64Div(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64Mod(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64And(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64ShR(const DRM_UINT64 a, const unsigned int b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64ShL(const DRM_UINT64 a, const unsigned int b);
extern DRM_API int        DRM_CALL DRM_UI64Eql(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API int        DRM_CALL DRM_UI64Les(const DRM_UINT64 a, const DRM_UINT64 b);
extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64(const unsigned long int b);

extern DRM_API DRM_UINT64 DRM_CALL DRM_UI64HL(const unsigned long int a, const unsigned long int b);
extern DRM_API unsigned long int DRM_CALL DRM_UI64High32( DRM_UINT64 a );
extern DRM_API unsigned long int DRM_CALL DRM_UI64Low32( DRM_UINT64 a );

/* Low 32 bit are stored first in 
** DRM_UINT64 structure.
** Thus we put "b" in first place
*/
#define DRM_UI64LITERAL(a,b) { b, a  }
#define DRM_I64LITERAL(a,b) { b, a  }

#define FILETIME_TO_UI64( ft, ui64 ) {ui64=DRM_UI64HL((ft).dwHighDateTime,(ft).dwLowDateTime);}
#define UI64_TO_FILETIME( ui64, ft ) {(ft).dwLowDateTime=DRM_UI64Low32(ui64);(ft).dwHighDateTime=DRM_UI64High32((ui64));}
#define NATIVE64_TO_NONNATIVE64( ui64 ) {ui64=DRM_UI64HL(DRM_UI64Low32(ui64),DRM_UI64High32(ui64));} /* Reverse the high and low */

#else

#if defined (_MSC_VER)

typedef          __int64 DRM_INT64;
typedef unsigned __int64 DRM_UINT64;

#elif defined (__GNUC__)

typedef int64_t  DRM_INT64;
typedef uint64_t DRM_UINT64;

#endif

#define DRM_I64LITERAL(a,b) (DRM_INT64)(((DRM_INT64)a)<<32|b)
#define DRM_UI64LITERAL(a,b)(DRM_UINT64)(((DRM_UINT64)a)<<32|b)

/* The following macros are not working properly and causes issues when license with expiry dates
 are parses. We could instead compile the SDK with -DDRM_NO_NATIVE_64_TYPES. But when we do, we
 get linking error. DPRODUU() is no longer defined in libbignum. As a workaroiund, use the macros defined
 when DRM_NO_NATIVE_64_TYPES is enable for these conversions. */
/*#define FILETIME_TO_UI64( ft, ui64 ) { (ui64) = *((DRM_UINT64*)&(ft)); } 
#define UI64_TO_FILETIME( ui64, ft ) { (ft)   = *((DRMFILETIME*)&(ui64)); }*/
#define FILETIME_TO_UI64( ft, ui64 ) {ui64=DRM_UI64HL((ft).dwHighDateTime,(ft).dwLowDateTime);}
#define UI64_TO_FILETIME( ui64, ft ) {(ft).dwLowDateTime=DRM_UI64Low32(ui64);(ft).dwHighDateTime=DRM_UI64High32((ui64));}

#define NATIVE64_TO_NONNATIVE64( ui64 )  

#define DRM_I64Add(a, b) ( (a) + (b) )
#define DRM_I64Sub(a, b) ( (a) - (b) )
#define DRM_I64Mul(a, b) ( (a) * (b) )
#define DRM_I64Div(a, b) ( (a) / (b) )
#define DRM_I64Mod(a, b) ( (a) % (b) )
#define DRM_I64And(a, b) ( (a) & (b) )
#define DRM_I64ShR(a, b) ( (a) >> (b) )
#define DRM_I64ShL(a, b) ( (a) << (b) )
#define DRM_I64Eql(a, b) ( (a) == (b) )
#define DRM_I64Les(a, b) ( (a) < (b) )
#define DRM_I64(b) ( (DRM_INT64) (b) )
#define DRM_I64Asgn(a, b) (((DRM_INT64)(a)<<32) | (b))
#define DRM_UI2I64(b) ((DRM_INT64)(b))

#define DRM_I64ToUI32(b) ((DRM_DWORD)(b))



#define DRM_UI64Add(a, b) ( (a) + (b) )
#define DRM_UI64Sub(a, b) ( (a) - (b) )
#define DRM_UI64Mul(a, b) ( (a) * (b) )
#define DRM_UI64Div(a, b) ( (a) / (b) )
#define DRM_UI64Mod(a, b) ( (a) % (b) )
#define DRM_UI64And(a, b) ( (a) & (b) )
#define DRM_UI64ShR(a, b) ( (a) >> (b) )
#define DRM_UI64ShL(a, b) ( (a) << (b) )
#define DRM_UI64Eql(a, b) ( (a) == (b) )
#define DRM_UI64Les(a, b) ( (a) < (b) )
#define DRM_UI64(b) ( (DRM_UINT64) (b) )

#define DRM_UI64HL(a,b) (((DRM_UINT64)(a)<<32) | (b))
#define DRM_UI64High32(a) ((unsigned long int)(a>>32))
#define DRM_UI64Low32(a) ((unsigned long int)(a&DRM_UI64LITERAL(0,0xFFFFFFFF)))

#endif

/* 
** This conversion is common for both native support of 64 bit number and representation as struct
*/
#define DRM_I2UI64(b) (*((DRM_UINT64*)(&(b))))

#define DRM_UI64Add32(a, b) DRM_UI64Add( DRM_UI64(a), DRM_UI64(b) )


#endif /* __DRMINT64_H__ */