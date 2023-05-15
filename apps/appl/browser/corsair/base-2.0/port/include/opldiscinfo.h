/**************************************************************************************/
/**
 * @file opldiscinfo.h
 *
 * Details of storage usage
 *
 **************************************************************************************
 **/

#ifndef __OPLDISCINFO_H__
#define __OPLDISCINFO_H__

#include "bpltype.h"

/******************************************************************
	Define
******************************************************************/

/******************************************************************
	Enum
 ******************************************************************/
typedef enum{

	eDiscInfo_All = 0,
	eDiscInfo_PVR_Only = 1,

}DiscInfo_StorageType_e;
/******************************************************************
	Typedef
******************************************************************/

/******************************************************************
	Interfaces
******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

BPLStatus OPL_Discinfo_GetFree(unsigned int *pulFree);

BPLStatus OPL_Discinfo_GetTotal(unsigned int *pulTotal);

BPLStatus OPL_Discinfo_GetReserved(unsigned int *pulReserved);

#ifdef __cplusplus
};

/******************************************************************
	class OplDiscInfo
******************************************************************/
class OplDiscInfo {
public:
	OplDiscInfo();
	virtual ~OplDiscInfo();

#if defined(CONFIG_OP_JAPAN)
	// JLABs »Æ¿Â
	HUINT32 TotalSize;
	HUINT32 FreeSize;
	HUINT32 ReservedSize;

	HBOOL	IsFormatted;
	HCHAR	*DiscId;
	HCHAR	*Name;
#else
	static unsigned int getFreeSize();
	static unsigned int getTotalSize();
	static unsigned int getReservedSize();
#endif
};

#if defined(CONFIG_OP_JAPAN)
BPLStatus	OPL_Discinfo_GetDiscInfoList(OplDiscInfo **info, HUINT32 *deviceCount,  DiscInfo_StorageType_e eType);
BPLStatus	OPL_Discinfo_GetDefaultDiscInfo(OplDiscInfo *info);
OplDiscInfo*	 OPL_DiscInfo_Clone(OplDiscInfo *info);
BPLStatus 	OPL_DiscInfo_SetStorageName(const char *storageId, const char *storageName);
#endif	//CONFIG_OP_JAPAN

#endif	//__cplusplus
#endif	//__OPLDISCINFO_H__

