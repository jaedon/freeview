/* $Header: $ */

#ifndef DLIB_TRANSPONDER_H__
#define DLIB_TRANSPONDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#if	defined(CONFIG_OP_FREESAT)
typedef struct
{
	HCHAR	defaultAuth[DxDEFAULT_AUTHORITY_LEN];
} DxFreesatTs_t;
#endif


typedef struct
{
	HUINT8				szCountryCode[4];
	HUINT8				ucRegionDepth; // 0xFF is Invalid mark
	HUINT32				ulRegionCode; // Primari(8) + Second(8) + Teritary(16)
} DxTvAnyTime_RegionInfoTs_t;

typedef struct
{
	HBOOL 	                        bFreqListed;
	HCHAR	                        defaultAuth[DxDEFAULT_AUTHORITY_LEN];
	DxTvAnyTime_RegionInfoTs_t		stRegionInfo[DxMAX_T_REGION_NUM];
} DxTvAnyTimeTs_t;


typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HINT32				netuid;
	HINT32				tsid;
	HINT32				onid;
	HINT32				tunerid;

	DxDeliveryType_e	deliType;
	DxTuneParam_t		tuningParam;

	union {
#if defined(CONFIG_OP_FREESAT)
		DxFreesatTs_t	    freesat;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		DxTvAnyTimeTs_t 	tva;
#else
		void *	__;
#endif
	} ex;
    HUINT32                 __CRC__;  //CHECK CRC
} DxTransponder_t;



#ifdef __cplusplus
}
#endif

#endif // DLIB_TRANSPONDER_H__


