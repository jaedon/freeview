/* $Header: $ */

#ifndef DLIB_NETWORK_H__
#define DLIB_NETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <hlib.h>

#ifdef	CONFIG_OP_FREESAT
typedef struct
{
	HCHAR	defaultAuth[DxDEFAULT_AUTHORITY_LEN];

} DxFreesatNet_t;
#endif



typedef struct
{
	HUINT8				szCountryCode[4];
	HUINT8				ucRegionDepth; // 0xFF is Invalid mark
	HUINT32				ulRegionCode; // Primari(8) + Second(8) + Teritary(16)
} DxTvAnyTime_RegionInfoNet_t;

typedef struct
{
	HCHAR	                        defaultAuth[DxDEFAULT_AUTHORITY_LEN];
	DxTvAnyTime_RegionInfoNet_t		stRegionInfo[DxMAX_T_REGION_NUM];
	HCHAR	                        UriPrefix[DxMAX_URI_PREFIX_INFO_LEN];

} DxTvAnyTimeNet_t;


typedef struct
{
	HINT32				uid;
	const HCHAR			*typeOf;
	HINT32				version;
	HINT32				onid;
	HCHAR				name[DxNAME_LEN];
	DxDeliveryType_e	deliType;

	union {
#if defined(CONFIG_OP_FREESAT)
		DxFreesatNet_t	    freesat;
#elif	defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		DxTvAnyTimeNet_t 	tva;
#else
		void *	__;
#endif
	} ex;
       HUINT32                 __CRC__;  //CHECK CRC
} DxNetwork_t;


#ifdef __cplusplus
}
#endif

#endif // DLIB_NETWORK_H__


