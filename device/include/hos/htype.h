/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
/* File Name: htype.h
 * Original Author: Jacob Oh
 * File Description:
 * Module:
 * Remarks:
 */

/*******************************************************************/
/* Copyright (c) 2006 HUMAX Co., Ltd. 							   */
/* All rights reserved.											   */
/*******************************************************************/
#ifndef _HTYPE_H_
#define _HTYPE_H_

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#ifdef WIN32
#define inline
#define INLINE	inline
#elif defined(_ICC)
#define INLINE __inline
#define __FUNCTION__		"[F]"
#define __func__			"[F]"
#elif defined(__GNUC__)
#define INLINE	inline
#else
#define INLINE
#endif

#define STATIC	static

/* network을 통한 전송을 위해 packed를 사용할 필요가 있음. */
#if defined(__GNUC__)
	#define	PACKED		__attribute__((packed))
	#define	CONSTRUCTOR	__attribute__((constructor))
	#define	DESTRUCTOR	__attribute__((destructor))
#else
	#define	PACKED
	#define	CONSTRUCTOR
	#define	DESTRUCTOR
#endif


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
/* */
typedef char				HCHAR;

/* 8 bit integer types */
typedef	signed char			HINT8;
typedef	unsigned char			HUINT8;

/* 16 bit integer types */
typedef	signed short			HINT16;
typedef	unsigned short			HUINT16;

/* 32 bit integer types */
typedef	int						HINT32;
typedef	unsigned int			HUINT32;

#if !defined(CONFIG_NOT_SUPPORT_BIGINT)
/* 64 bit integer types */
typedef signed long long		HINT64;
typedef unsigned long long		HUINT64;
#endif

/* boolean type */
typedef	int						HBOOL;			/* note: it should be 32 bits on 32 bits machine. in case of 8 <-> 32 bits parameter passing of HBOOL, it may cause a problem - stack irruption. */

/* long data type */
typedef	signed long				HLONG;
typedef	unsigned long			HULONG;

/* float data type */
typedef float					HFLOAT32;

/* double data type */
typedef double					HFLOAT64;

#ifndef NULL
#define NULL					0
#endif

#ifndef FALSE
#define FALSE					0
#endif

#ifndef TRUE
#define TRUE					1
#endif

#define	ERR_OK					0
#define	ERR_FAIL				1

typedef unsigned				HBIT;

/** Core Arcitecture Team comment (Recommendation)

	HERROR type을 규정하게 된 이유. (2008.03.20)

	기존 NEO에서 API의 return 값으로 HRESULT를 error code return으로 규정하여 사용하기로 하였으나,
	HRESULT 일반 단어 의미상 ERROR CODE와 일반적인 계산 VALUE, BOOL등의 return값과 혼동을 일으켰으며,
	error code return으로 사용한다는 규정이 제대로 전파되지 않아 코드 전반에 걸쳐 의미가 약해진 결과를 나타냈다.

	그래서 Octo에서는 API의 return 값으로 HERROR를 사용하도록 명시하며 이를 htype.h에 명시적으로 지정한다.
	또한 API의 return값은 error code, value 등을 return할 수 있으며, error code 이외의 값을 return할 경우
	HERROR를 사용하는 것이 아니라 return 해야 할 primitive 형식 또는 enumeration을 선언하여 써야 한다.
	코드에 HRESULT가 나오는 것은 오류이다. Octo에서 HRESULT는 더이상 쓰이지 않는다.
*/
typedef HUINT32					HERROR;

typedef HUINT32					UNIXTIME;

typedef    struct
{
	HUINT16		usMsgClass;
	HUINT16		usParam;		/* sub type or other purpose */
	HUINT32		ulParam;		/* it can be used as pointer */
	union
	{
		HUINT8		aucC [8];
		HUINT16		ausS [4];
		HUINT32		aulL [2];
		void		*pavV [2];
	} unInfo;
} HMSG;		/* Humax Message structure */


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ Extern functions *************************/
/*******************************************************************/


/*******************************************************************/
/******************** Global function prototype ********************/
/*******************************************************************/


#endif /* _HTYPE_H_ */
