/**
	@file	  pal_dsc.h
	@brief	  																					\n

	Description: 																				\n
	Module: PAL/DSC(Descramble) 																\n

	Copyright (c) 2008 HUMAX Co., Ltd.															\n
	All rights reserved.																		\n
*/


#ifndef	__PAL_DSC_H__
#define	__PAL_DSC_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef enum
{
	ePAL_DSC_DSCTYPE_NONE,
	ePAL_DSC_DSCTYPE_DVB,
	ePAL_DSC_DSCTYPE_AES_CLEAR,
	ePAL_DSC_DSCTYPE_AES_BLOCK,
	ePAL_DSC_DSCTYPE_DES_CLEAR,
	ePAL_DSC_DSCTYPE_DES_BLOCK,
	ePAL_DSC_DSCTYPE_MAX
} PalDsc_DscType_e;

typedef enum
{
	ePAL_DSC_KEYPARITY_ODD,
	ePAL_DSC_KEYPARITY_EVEN
} PalDsc_KeyParity_e; // TSC를 생각해도 상식적으로 EVEN = 0, ODD = 1 로 정의가 되어야 하는데 참내... 그러나 7103 에서도 이렇게 정의되어 사용하고 있으므로 임의로 고치면 안된다 -,.-

typedef enum
{
	ePAL_DSC_DATATYPE_NONE,
	ePAL_DSC_DATATYPE_VIDEOPES,
	ePAL_DSC_DATATYPE_AUDIOPES,
	ePAL_DSC_DATATYPE_PES,
	ePAL_DSC_DATATYPE_PSIPES,
	ePAL_DSC_DATATYPE_MAX
} PalDsc_DataType_e;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


extern HERROR PAL_DSC_Init(void);
extern HERROR PAL_DSC_Open(HUINT32 ulDemuxId, PalDsc_DscType_e eDscType, PalDsc_DataType_e eDataType, HUINT32 *pulDscIndex);
extern HERROR PAL_DSC_CloseAll(void);
extern HERROR PAL_DSC_Close(HUINT32 ulDscIndex);
extern HERROR PAL_DSC_SetPid(HUINT32 ulDscIndex, HUINT16 usPid);
extern HERROR PAL_DSC_ResetPid(HUINT32 ulDscIndex, HUINT16 usPid);
extern HERROR PAL_DSC_SetKey(HUINT32 ulDscIndex, PalDsc_KeyParity_e eParity, HUINT8 *pucKeyData);
extern HERROR PAL_DSC_SetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData);
extern HERROR PAL_DSC_ClearProtectionKey(HUINT32 ulDscIndex);
extern HERROR PAL_DSC_SetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize);

#ifdef CONFIG_MW_CI_PLUS
extern HERROR PAL_DSC_SetKeyForCiPlus(HUINT32 ulDscIndex, PalDsc_DscType_e eDscType, PalDsc_KeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec);
#endif

extern HERROR PAL_DSC_PrintCurrentKey(HUINT32 ulDscIndex);
extern HERROR PAL_DSC_PrintDscStatus(HUINT32 ulDscIndex);
extern HERROR PAL_DSC_PrintAllDscStatus(void);

#endif	//__PAL_DSC_H__

