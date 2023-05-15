/********************************************************************
 * $Workfile:   drv_csd.h  $
 * $Modtime:   Jan 12 2007 09:53:44  $
 *
 * Author     :
 * Description:
 *
 *                                 Copyright (c) 2007 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/
#ifndef	_DRV_CSD_H_
#define	_DRV_CSD_H_

#include "htype.h"
#include "drv_err.h"

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

typedef enum
{
	DRV_CSD_BOOT_NOT_SET = 0,
	DRV_CSD_BOOT_FROM_NOR = 0x2,
	DRV_CSD_BOOT_FROM_NAND,
	DRV_CSD_BOOT_FROM_SPI,
	DRV_CSD_BOOT_MODE_MAX
} DRV_CSD_BootMode;

typedef struct
{
	HUINT16	ucIndex;
	HUINT8	ucDataBitLen;
	HUINT32	ulDataBitMask;
	HUINT32	ulMspData;
} DRV_CSD_ChipsetOTP;

/**********************************************************************************************
						 CDS Public Functions
**********************************************************************************************/
DRV_Error DRV_CSDInitialize( void );

DRV_Error DRV_CSDGetChipID(HUINT32 *pulChipID);

DRV_Error DRV_CSDGetChipIDx64(HUINT8 *pszChipIDx64);

DRV_Error DRV_CSDGetSCS(HUINT8 *pucSCS);

DRV_Error DRV_CSDGetJTAG(HUINT8 *pucJTAG);

DRV_Error DRV_CSDGetMarketID(HUINT8 *pucMarketID);

DRV_Error DRV_CSDSetSCS(int lBlockSizeKB, int lKeyIndex);

DRV_Error DRV_CSDSetJTAG(void);

DRV_Error DRV_CSDGetCWE(HUINT8 *pucCWE);

DRV_Error DRV_CSDSetCWE(void);

DRV_Error DRV_CSDGetCheckNumber(HUINT32 *pxCheckNumber);

DRV_Error DRV_CSDGetChipRev(unsigned char *pulChipRev);

DRV_Error DRV_CSDGetCSCCheckNumber(HUINT32 *pxCheckNumber, unsigned char *pCSCData);

DRV_Error DRV_CSDGetOtpDataSection(unsigned char ucIndex, unsigned char *pucDataSection);

DRV_Error DRV_CSDSetOtpDataSection(unsigned char ucIndex, unsigned char *pucDataSection, unsigned char *pucDataSectionCRC);

DRV_Error DRV_CSDGetSTBOwnerID(HUINT16 *pusSTBOwnerID);
DRV_Error DRV_CSDSetSTBOwnerID(HUINT16 usSTBOwnerID);

DRV_Error DRV_CSDGetEncryptionData( unsigned char *pInputData, unsigned char *pOutputData, unsigned int uiLength, unsigned int isEnryption);

DRV_Error DRV_CSDSetMarketID(HUINT32 ulMarketID);

DRV_Error DRV_CSDSetStbCaSn(HUINT8 *pucStbCaSn);

DRV_Error DRV_CSDGetStbCaSn(HUINT8 *pucStbCaSn);

DRV_Error DRV_CSDSetRamScrambling(void);

DRV_Error DRV_CSDGetRamScrambling(HUINT8 *pucRamScr);

DRV_Error DRV_CSDSetEncryptFPK(void);

DRV_Error DRV_CSDGetStbCaSnByPK(HUINT8 *pucStbCaSn);

DRV_Error DRV_CSDSetBootMode(DRV_CSD_BootMode eCsdBootMode);

DRV_Error DRV_CSDGetBootMode(DRV_CSD_BootMode *pCsdBootMode);

DRV_Error DRV_CSDSetSCSVersioning(HUINT16 usPvLength, HUINT8 *pucPvValue);

DRV_Error DRV_CSDGetSCSVersioning(HUINT16 usPvLength, HUINT8 *pucPvValue);

DRV_Error DRV_CSDSetFlashProtection(void);

DRV_Error DRV_CSDGetFlashProtection(HUINT8 *pucFlag);

DRV_Error DRV_CSDSetOTPChipset(void);

DRV_Error DRV_CSDGetOTPChipset(HUINT8 *ucOTPChipset);

DRV_Error DRV_CSDSetHostBootCodeDescryptionSelect(HUINT32 ulMspData);
DRV_Error DRV_CSDGetHostBootCodeDescryptionSelect(HUINT32 *pulMspData);

DRV_Error DRV_CSDGetOTPField(HUINT32* pulOTPField1, HUINT32* pulOTPField2);

DRV_Error DRV_CSDGetChipSetExtension(HUINT8 *pvInfo);

DRV_Error DRV_CSDGetChipSetCut(HUINT8 *pvInfo);

DRV_Error DRV_CSDGetCertCheckNumber(HUINT8 *pCertCheckNumber);

DRV_Error DRV_CSDVerifyCscData(HUINT8 *pCSCData);

#endif
