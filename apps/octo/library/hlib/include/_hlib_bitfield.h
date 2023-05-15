
#define	____HxBITFIELD_README___________________

/******************************************************************************
 [Example Code]
	void test(void)
	{
		HINT32	i;
		HUINT8	bitfield[4];	// 32bit

		memset(bitfield, 0, 4);

		HxBITFIELD_SET(bitfield, 23);
		HxBITFIELD_SET(bitfield, 4);
		HxBITFIELD_SET(bitfield, 12);
		HxBITFIELD_FLP(bitfield, 7);

		for (i = 0 ; i < 4 ; i++)
		{
			printf("%02X ", bitfield[i]);
		}

 [Expected Result]
	90 10 80 00

 ************************************************************************************/

#ifndef	HxBITFIELD_H__
#define HxBITFIELD_H__

#include "htype.h"


#ifdef	__cplusplus

#include <cstdio>
#include <cstdarg>
extern	"C" {
#else
#include <stdio.h>
#include <stdarg.h>
#endif

#define	HxBITFIELD_SIZE 8

#if	HxBITFIELD_SIZE == 8
typedef HUINT8	HxBitField_t;
#elif HxBITFIELD_SIZE == 16
typedef HUINT16	HxBitField_t;
#elif HxBITFIELD_SIZE == 32
typedef	HUINT32	HxBitField_t;
#elif HxBITFIELD_SIZE == 64
typedef	HUINT64	HxBitField_t;
#else
#error	"OTL"
#endif

#define HxBITFIELD_GET(field,bitpos)	\
		(((HxBitField_t *)(field))[(bitpos)/HxBITFIELD_SIZE] & (1 << ((bitpos)%HxBITFIELD_SIZE)))

#define	HxBITFIELD_SET(field,bitpos)	\
	do{\
		((HxBitField_t *)(field))[(bitpos)/HxBITFIELD_SIZE] |= (1 << ((bitpos)%HxBITFIELD_SIZE));\
	}while(0)

#define	HxBITFIELD_CLR(field,bitpos)	\
	do{\
		((HxBitField_t *)(field))[(bitpos)/HxBITFIELD_SIZE] &= ~(1 << ((bitpos)%HxBITFIELD_SIZE));\
	}while(0)

#define	HxBITFIELD_FLP(field,bitpos)	\
	do{\
		((HxBitField_t *)(field))[(bitpos)/HxBITFIELD_SIZE] ^= (1 << ((bitpos)%HxBITFIELD_SIZE));\
	}while(0)

#ifdef	__cplusplus
}
#endif

#endif /* HxBITFIELD_H__ */

