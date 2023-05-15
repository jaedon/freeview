

#ifndef __MXTYPES_H__
#define __MXTYPES_H__

#ifdef __cplusplus
extern "C"{
#endif


typedef unsigned char	MX_BOOL;

typedef char			MX_INT8;
typedef unsigned char	MX_UINT8;
typedef short			MX_INT16;		// 16-bit signed integer
typedef long			MX_INT32;		// 32-bit signed integer

typedef unsigned short	MX_UINT16;		// 16-bit unsigned integer
typedef unsigned long	MX_UINT32;		// 32-bit unsigned integer


#ifndef __GNUC__
	typedef __int64				MX_INT64;		// 64-bit signed integer
	typedef unsigned __int64	MX_UINT64;		// 64-bit unsigned integer
	#define PD64	"%I64d"

#else
	typedef long long			MX_INT64;		// 64-bit signed integer
	typedef unsigned long long	MX_UINT64;		// 64-bit unsigned integer

	#define PD64	"%lld"

#endif


typedef enum _MX_RETURN {
	MX_RETURN_OK = 0,
	MX_RETURN_ERROR = -1,
	MX_RETURN_FAILED = -2,
	MX_RETURN_NOT_EXISTS = -3,
} MX_RETURN;

#define MX_TRUE		1
#define MX_FALSE	0

#ifdef __cplusplus
}
#endif


#endif //__MXTYPES_H__

