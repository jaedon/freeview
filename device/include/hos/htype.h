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

/* network�� ���� ������ ���� packed�� ����� �ʿ䰡 ����. */
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

	HERROR type�� �����ϰ� �� ����. (2008.03.20)

	���� NEO���� API�� return ������ HRESULT�� error code return���� �����Ͽ� ����ϱ�� �Ͽ�����,
	HRESULT �Ϲ� �ܾ� �ǹ̻� ERROR CODE�� �Ϲ����� ��� VALUE, BOOL���� return���� ȥ���� ����������,
	error code return���� ����Ѵٴ� ������ ����� ���ĵ��� �ʾ� �ڵ� ���ݿ� ���� �ǹ̰� ������ ����� ��Ÿ�´�.

	�׷��� Octo������ API�� return ������ HERROR�� ����ϵ��� ����ϸ� �̸� htype.h�� ��������� �����Ѵ�.
	���� API�� return���� error code, value ���� return�� �� ������, error code �̿��� ���� return�� ���
	HERROR�� ����ϴ� ���� �ƴ϶� return �ؾ� �� primitive ���� �Ǵ� enumeration�� �����Ͽ� ��� �Ѵ�.
	�ڵ忡 HRESULT�� ������ ���� �����̴�. Octo���� HRESULT�� ���̻� ������ �ʴ´�.
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
