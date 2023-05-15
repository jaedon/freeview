/*************************************************************
 * @file		hxmacro.h
 * @date		2012/03/06
 * @author		someone
 * @brief		HLIB Macro Definitions

 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXMACRO_H_
#define	_HXMACRO_H_

/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */


/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */
#ifndef UNUSED
#define UNUSED(_x)		((void)(_x))
#endif
#ifndef	PARAM_UNUSED
#define	PARAM_UNUSED(_x)	(void)_x
#endif

#define	FUNCTIONIZE(a,b)	a(b)
#define	STRINGIZE(a)		#a
#define	INT2STRING(i)		FUNCTIONIZE(STRINGIZE,i)

#define	__FILE_LINE__		__FILE__ ":" INT2STRING(__LINE__)

#define	HxMACRO_KILOBYTE	(1024)
#define	HxMACRO_MEGABYTE	(HxMACRO_KILOBYTE*HxMACRO_KILOBYTE)
#define	HxMACRO_GIGABYTE	(HxMACRO_KILOBYTE*HxMACRO_MEGABYTE)


#define	HxMACRO_MAX(a,b)	(((a) > (b)) ? (a) : (b))
#define	HxMACRO_MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define	HxMACRO_TRIM(value,min,max)	((value) < (min) ? (min) : (value) > (max) ? (max) : (value))

#define	HxMACRO_BITSET(byte,bit)	((byte) | (bit))
#define	HxMACRO_BITCLR(byte,bit)	((byte) & ~(bit))
#define	HxMACRO_BITFLP(byte,bit)	((byte) ^ (bit))
#define	HxMACRO_BITMSK(byte,bit)	((byte) & (bit))

#define	HxMACRO_ABS(a)				(((a) < 0) ? -(a) : (a))
#define HxMACRO_AVERAGE(a, b)   	( ((a) + (b)) >> 1 )
#define	HxMACRO_SGN(x)				((x) > 0 ? 1 : -1)

#define HxMACRO_DIV15(x)			((x + (((x + 16) >> 4) + 16)) >> 4)
#define HxMACRO_DIV255(x)			((x + (((x + 128) >> 8) + 128)) >> 8)

#define HxMACRO_MULDIV255(a,b) 		((((HUINT8)(a))*((((HUINT8)(b))+1)))>>8)                /* very fast, 92% accurate*/

#define HxMACRO_PI 					3.1415926535897932384626433832795
#define HxMACRO_PItoINT 			3141592

#define	HxMACRO_SWAP32(l)			( ((((l) & 0xFF000000) >> 24) | \
								   	(((l) & 0x00FF0000) >> 8)  | \
								   	(((l) & 0x0000FF00) << 8)  | \
								   	(((l) & 0x000000FF) << 24)))
#define	HxMACRO_SWAP16(s)			(((((s) & 0xFF) << 8) | (((s) >> 8) & 0xFF)))

#define	HxMACRO_Get8Bit(a)		(*((HUINT8*)(a)) & 0xFF)
#define HxMACRO_Get10Bit(a) 	(HUINT16)((((*((HUINT8 *)(a))) << 8) | (*((HUINT8 *)(a)+1))) & 0x03FF)/* 0000 0011 1111 1111 */
#define HxMACRO_Get12Bit(a) 	(HUINT16)((((*((HUINT8 *)(a))) << 8) | (*((HUINT8 *)(a)+1))) & 0x0FFF)/* 0000 1111 1111 1111 */
#define HxMACRO_Get13Bit(a) 	(HUINT16)((((*((HUINT8 *)(a))) << 8) | (*((HUINT8 *)(a)+1))) & 0x1FFF)/* 0001 1111 1111 1111 */
#define HxMACRO_Get14Bit(a) 	(HUINT16)((((*((HUINT8 *)(a))) << 8) | (*((HUINT8 *)(a)+1))) & 0x3FFF)/* 0011 1111 1111 1111 */
#define HxMACRO_Get15Bit(a) 	(HUINT16)((((*((HUINT8 *)(a))) << 8) | (*((HUINT8 *)(a)+1))) & 0x7FFF)/* 0111 1111 1111 1111 */
#define	HxMACRO_Get16Bit(a)		(HUINT16)((((*((HUINT8 *)(a)) << 8) | (*((HUINT8*)(a)+1)))))
#define	HxMACRO_Get24Bit(a)		(HUINT32)((((*((HUINT8 *)(a)) << 16) | (*((HUINT8*)(a)+1) << 8) |(*((HUINT8*)(a)+2)))))
#define	HxMACRO_Get32Bit(a)		(HUINT32)((((*((HUINT8 *)(a)) << 24) | (*((HUINT8*)(a)+1) << 16) |(*((HUINT8*)(a)+2) << 8) |(*((HUINT8*)(a)+3)))))
#define	HxMACRO_Get40Bit(a)		(HUINT32)((((*((HUINT8 *)(a)) << 32) | (*((HUINT8 *)(a)+1) << 24) | (*((HUINT8*)(a)+2) << 16) |(*((HUINT8*)(a)+3) << 8) |(*((HUINT8*)(a)+4)))))

#define	HxMACRO_Put16Bit(p, data)	{*((HUINT8 *)(p)) = (HUINT8)((data) >> 8);*((HUINT8 *)((p) + 1)) = (HUINT8)(data);}
#define	HxMACRO_Put32Bit(p, data)	{*((HUINT8 *)(p)) = (HUINT8)((data) >> 24);*((HUINT8 *)((p) + 1)) = (HUINT8)((data) >> 16);*((HUINT8 *)((p) + 2)) = (HUINT8)((data) >> 8);*((HUINT8*)((p) + 3)) = (HUINT8)(data);}

#define __VA_COUNT( \
		__1, __2, __3, __4, __5, __6, __7, __8, __9, _10, _11, _12, _13, _14, _15, _16,	\
		_17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,	\
		_33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,	\
		_49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...)	N

#define HxMACRO_VA_COUNT(...)	(	\
	sizeof(#__VA_ARGS__) - 1		\
	? __VA_COUNT(__VA_ARGS__, 		\
		63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50,	49, 48,		\
		47, 46, 45, 44, 43, 42, 41, 40,	39, 38, 37, 36, 35, 34, 33, 32,		\
		31, 30,	29, 28, 27, 26, 25, 24, 23, 22, 21, 20,	19, 18, 17, 16,		\
		15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0 )	\
	: 0\
)

#define	HxMACRO_TIMEIT(expr)	\
	do {\
		HUINT32 __tick__ = HLIB_STD_GetSystemTick();\
		{expr;}\
		Hx_Print("[01;33m \tTIMEIT[%s:%d]: %u msec:%s [00;39;49m \n", __FUNCTION__, __LINE__, HLIB_STD_GetSystemTick() - __tick__, #expr);\
	} while (0)

/* *************************************************************************************************** */
#define ___MACRO_FUCTION_Definition______________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */


#endif /* _HXMACRO_H_ */

