/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
//#include <octo_common.h>
#include <htype.h>
#include <di_err.h>
#include <di_flash.h>
#include <di_eeprom.h>
#include <di_uart.h>
#include <di_crypt.h>
#include <drv_flash.h>
#include <nvram_map.h>

#include <pal_sys.h>
#include <pal_param.h>

//#include "_svc_cas_mgr_nvram.h"


#include "tir_platform.h"
#if !defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
#include "tir_loader.h"
#endif

/* 아래 함수 정리 하면서 해당사항이 잇는지 확인해라.
#include "pal_eeprom.h"
#include "mw_eep_mgr.h"
*/
#include "tir_nvr.h"

#define _______________ACCESS_NVRAM____________________

#if defined(CONFIG_DEBUG)
#define PrintDebug	printf
#define PrintErr	printf
#define TraceNvr	printf("TRANCE: %s, %d\n", __FUNCTION__, __LINE__ )
#else
#define PrintDebug(...)
#define PrintErr(...)
#endif


Boot_Block_Control_Block g_tIr_bbcb;
HUINT8 g_tIr_cpcb[IR_CPCB_SIZE];
static  unsigned char s_aucBlockData[FLASH_APPL_BLOCK_SIZE];
extern DI_ERR_CODE	DI_NANDFLASH_Write(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);

HERROR	irldvt_svc_cas_SysConvertFlashFieldTypeRmToPalParam(irldvt_DevFlashField_e inType, PAL_PARAM_Item_e *peCnvtType)
{

	if (NULL == peCnvtType)
	{
		return ERR_FAIL;
	}

	switch (inType)
	{
	case IRLDVT_DEV_FLASH_FIELD_SERIAL_NO:
		*peCnvtType = ePAL_PARAM_SERIAL_NO;
		break;

	case IRLDVT_DEV_FLASH_FIELD_DTCP_KEY:
		*peCnvtType = ePAL_PARAM_DTCP_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_CI_PLUS_KEY:
		*peCnvtType = ePAL_PARAM_CI_PLUS_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_MAC_ADDR:
		*peCnvtType = ePAL_PARAM_MAC_ADDR;
		break;

	case IRLDVT_DEV_FLASH_FIELD_HDCP_KEY:
		*peCnvtType = ePAL_PARAM_HDCP_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_PARING_DATA:
		*peCnvtType = ePAL_PARAM_PARING_DATA;
		break;

	case IRLDVT_DEV_FLASH_FIELD_SYSTEM_ID:
		*peCnvtType = ePAL_PARAM_SYSTEM_ID;
		break;

	case IRLDVT_DEV_FLASH_FIELD_LOADER_VER:
		*peCnvtType = ePAL_PARAM_LOADER_VERSION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_APP_VER:
		*peCnvtType = ePAL_PARAM_APP_VERSION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_PRIVATE:
		*peCnvtType = ePAL_PARAM_PRIVATE;
		break;

	case IRLDVT_DEV_FLASH_FIELD_CSC_KEY:
		*peCnvtType = ePAL_PARAM_CSC_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB:
		*peCnvtType = ePAL_PARAM_IRDETO_BBCB;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID:
		*peCnvtType = ePAL_PARAM_IRDETO_UC_SECURE_ID;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA:
		*peCnvtType = ePAL_PARAM_IRDETO_UC_PRIVATE_DATA;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB:
		*peCnvtType = ePAL_PARAM_IRDETO_CPCB;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED:
		*peCnvtType = ePAL_PARAM_IRDETO_CPCB_ENCRYPTED;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_CA_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_LOADER_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_IR_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION:
		*peCnvtType = ePAL_PARAM_IRDETO_IR_BACKUP_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_MAX:
	default:
		*peCnvtType = ePAL_PARAM_MAX;
		break;
	}

	printf("\n[%s],[%d]inType : %d, outTpye : %d++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__,inType, *peCnvtType);
	return ERR_OK;
}

HERROR	irldvt_svc_cas_SysConvertFlashFieldTypeRmToPal(irldvt_DevFlashField_e inType, PAL_FLASH_FIELD_t *pCnvtType)
{
	if (NULL == pCnvtType)
	{
		printf("\n[%s],[%d], pCnvtType is NULL++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	switch (inType)
	{
	case IRLDVT_DEV_FLASH_FIELD_SERIAL_NO:
		printf("\n[%s],[%d]case IRLDVT_DEV_FLASH_FIELD_SERIAL_NO++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		*pCnvtType = PAL_FLASH_FIELD_SERIAL_NO;
		break;

	case IRLDVT_DEV_FLASH_FIELD_DTCP_KEY:
		*pCnvtType = PAL_FLASH_FIELD_DTCP_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_CI_PLUS_KEY:
		*pCnvtType = PAL_FLASH_FIELD_CI_PLUS_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_MAC_ADDR:
		*pCnvtType = PAL_FLASH_FIELD_MAC_ADDR;
		break;

	case IRLDVT_DEV_FLASH_FIELD_HDCP_KEY:
		*pCnvtType = PAL_FLASH_FIELD_HDCP_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_PARING_DATA:
		*pCnvtType = PAL_FLASH_FIELD_PARING_DATA;
		break;

	case IRLDVT_DEV_FLASH_FIELD_SYSTEM_ID:
		*pCnvtType = PAL_FLASH_FIELD_SYSTEM_ID;
		break;

	case IRLDVT_DEV_FLASH_FIELD_LOADER_VER:
		*pCnvtType = PAL_FLASH_FIELD_LOADER_VER;
		break;

	case IRLDVT_DEV_FLASH_FIELD_APP_VER:
		*pCnvtType = PAL_FLASH_FIELD_APP_VER;
		break;

	case IRLDVT_DEV_FLASH_FIELD_PRIVATE:
		*pCnvtType = PAL_FLASH_FIELD_PRIVATE;
		break;

	case IRLDVT_DEV_FLASH_FIELD_CSC_KEY:
		*pCnvtType = PAL_FLASH_FIELD_CSC_KEY;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_BBCB;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_UC_SECURE_ID;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CPCB;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_CA_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_LOADER_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_IR_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION:
		*pCnvtType = PAL_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION;
		break;

	case IRLDVT_DEV_FLASH_FIELD_MAX:
	default:
		*pCnvtType = PAL_FLASH_FIELD_MAX;
		break;
	}
	printf("\n[%s],[%d], input param is %d, return is %d++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__,inType,*pCnvtType);
	return ERR_OK;
}


HERROR irldvt_svc_cas_DevSysGetField(irldvt_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	if (PalParam_Activated())
	{
		PAL_PARAM_Item_e	eCnvtType;
		printf("\n[%s],[%d] in PalParam_Activated()++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);

		if(irldvt_svc_cas_SysConvertFlashFieldTypeRmToPalParam(eType, &eCnvtType) != ERR_OK)
		{			return ERR_FAIL;
		}

		return PAL_PARAM_GetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
	else
	{
		printf("\n[%s],[%d]PalParam_Activated() is not setted++++++++++++++++++++++++++\n",__FUNCTION__,__LINE__);
		PAL_FLASH_FIELD_t	eCnvtType;

		if(irldvt_svc_cas_SysConvertFlashFieldTypeRmToPal(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_SYS_GetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
}

HERROR irldvt_svc_cas_SysSetField(irldvt_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	if (PalParam_Activated())
	{
		PAL_PARAM_Item_e	eCnvtType;

		if(irldvt_svc_cas_SysConvertFlashFieldTypeRmToPalParam(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_PARAM_SetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
	else
	{
		PAL_FLASH_FIELD_t	eCnvtType;

		if(irldvt_svc_cas_SysConvertFlashFieldTypeRmToPal(eType, &eCnvtType) != ERR_OK)
		{
			return ERR_FAIL;
		}

		return PAL_SYS_SetFieldOffset(eCnvtType, ulOffset, pData, ulSize);
	}
}


unsigned short TIR_Get16Bits(unsigned char *base)
{
	unsigned short temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;

	return temp ;
} /* Get16Bits */

unsigned long  TIR_Get24Bits(unsigned char *base)
{
	unsigned long temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;
	temp <<= 8 ;
	temp |= *(base + 2) ;

	return temp ;
} /* Get24Bits */

unsigned long TIR_Get32Bits(unsigned char *base)
{
	unsigned long temp = 0 ;

	temp |= *(base + 0) ;
	temp <<= 8 ;
	temp |= *(base + 1) ;
	temp <<= 8 ;
	temp |= *(base + 2) ;
	temp <<= 8 ;
	temp |= *(base + 3) ;

	return temp ;
} /* Get32Bits */


HIR_ERR TIR_E2P_Partition_Read( TIRNVR_Type ePartType, HUINT32 ulPartOffset,void *pucData, HUINT32 ulSize )
{
	irldvt_DevFlashField_e eType;
	HUINT32 ulMaxOffset;

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	HUINT8				diFlashField[32] = {0,};
	HERROR				Err = ERR_OK;
	HUINT8				aucBuf[512] = {0,};
	HUINT32 			ulFieldSize = 0;
#else
	HUINT32			ulExtZoneOffset;
#endif

	HxLOG_Print("Offset[%d], Size[%d]\n", ulPartOffset, ulSize);

	if(pucData ==NULL)
	{
		HxLOG_Info("null pointer\n" );
		return HIR_NOT_OK;
	}

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	switch(ePartType)
	{
		case TIRNVR_IRDETO :
			//VK_memcpy(diFlashField, IRDETO_IR_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_IRDETO_SIZE;
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION;
			ulMaxOffset = IR_PARTITION_IRDETO_SIZE;
			break;

		case TIRNVR_CA :
			//VK_memcpy(diFlashField, IRDETO_CA_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_CA_SIZE;
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION;
			ulMaxOffset = IR_PARTITION_CA_SIZE;
			break;

		case TIRNVR_LOADER :
			//VK_memcpy(diFlashField, IRDETO_LOADER_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_LOADER_SIZE;
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION;
			ulMaxOffset = IR_PARTITION_LOADER_SIZE;
			break;
		default:
			HxLOG_Info(" %s, Invalid partition(%d)\n", __FUNCTION__, ePartType );
			return HIR_NOT_OK;
	}

	if((ulPartOffset + ulSize)> ulMaxOffset)
	{
		HxLOG_Info("Invalid offset(0x%X)\n", ulPartOffset);
		return HIR_NOT_OK;
	}

	//Err = PAL_NVRAM_Read(ePAL_NVRAM_FS_PARTITION_RW, diFlashField, 0, aucBuf, ulFieldSize);
	if (ERR_OK  != irldvt_svc_cas_DevSysGetField(eType, ulPartOffset, pucData, ulSize))
	{
		HxLOG_Info("svc_cas_DevSysGetField ERROR!!111 %08X", Err);
		return HIR_NOT_OK;
	}
	//VK_memcpy(pucData, &aucBuf[ulPartOffset], ulSize);

	return HIR_OK;


#else
#if 0
	switch(ePartType)
	{
		case TIRNVR_IRDETO :
			ulExtZoneOffset = ulPartOffset;
			break;
		case TIRNVR_CA :
			ulExtZoneOffset = (IR_PARTITION_CA_OFFSET-IR_PARTITION_IRDETO_OFFSET)+ulPartOffset;
			break;
		case TIRNVR_LOADER :
			ulExtZoneOffset = (IR_PARTITION_LOADER_OFFSET- IR_PARTITION_IRDETO_OFFSET)+ulPartOffset;
			break;
		default:
			PrintErr("Invalid partition(%d)\n", ePartType );
			return HIR_NOT_OK;
	}

	if(ulExtZoneOffset >E2P_EXT_ZONE_SIZE-1)
	{
		PrintErr("Invalid offset(0x%X)\n", ulExtZoneOffset) ;
		return HIR_NOT_OK;
	}

	if(ERR_EEPROM_OK != PAL_EEPROM_Read(eE2P_EXT_ZONE_ID,ulExtZoneOffset,pucData, ulSize) )
	{
		PrintErr("PAL_EEPROM_Read error\n " );
		return HIR_NOT_OK;
	}
#endif
#endif

	return HIR_OK;
}

//modified by hmjoo
HIR_ERR TIR_E2P_Partition_Write(TIRNVR_Type ePartType, HUINT32 ulPartOffset, void *pucData, HUINT32 ulSize)
{
	irldvt_DevFlashField_e eType;
	HUINT32	ulMaxOffset;

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	HUINT8				diFlashField[32]={0,};
	HUINT8				aucBuf[512]={0,};
	HUINT32 			ulFieldSize = 0;
	HERROR				Err = ERR_OK ;
#else
	HUINT32 			ulExtZoneOffset;
#endif

	HxLOG_Print("%s, Offset[%d], Size[%d]\n", __FUNCTION__, ulPartOffset, ulSize);

	if(pucData ==NULL)
	{
		HxLOG_Info("%s, null pointer\n", __FUNCTION__ );
		return HIR_NOT_OK;
	}

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	switch(ePartType)
	{
		case TIRNVR_IRDETO :
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION;
			ulMaxOffset = IR_PARTITION_IRDETO_SIZE;
			//VK_memcpy(diFlashField, IRDETO_IR_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_IRDETO_SIZE;
			break;

		case TIRNVR_CA :
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION;
			ulMaxOffset = IR_PARTITION_CA_SIZE;
			//VK_memcpy(diFlashField, IRDETO_CA_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_CA_SIZE;
			break;

		case TIRNVR_LOADER :
			eType = IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION;
			ulMaxOffset = IR_PARTITION_LOADER_SIZE;
			//VK_memcpy(diFlashField, IRDETO_LOADER_PARTITION, sizeof(HUINT8)*32);
			//ulFieldSize  = IR_PARTITION_LOADER_SIZE;
			break;
		default:
			HxLOG_Info(" %s, Invalid partition(%d)\n", __FUNCTION__, ePartType );
			return HIR_NOT_OK;
	}

	//Err = PAL_NVRAM_Read(ePAL_NVRAM_FS_PARTITION_RW, diFlashField, 0, aucBuf, ulFieldSize);
	if((ulPartOffset + ulSize) > ulMaxOffset)
	{
		HxLOG_Info("Invalid offset(0x%X)\n", ulPartOffset);
		return HIR_NOT_OK;
	}

	if (ERR_OK  != irldvt_svc_cas_SysSetField(eType,ulPartOffset,pucData, ulSize))
	{
		HxLOG_Info("CAS_IR_NVR_Partition_Write ERROR!!111 %08X", Err);
		return HIR_NOT_OK;
	}

	//VK_memcpy(&aucBuf[ulPartOffset], pucData, ulSize);
	//Err = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, diFlashField, 0, aucBuf, ulFieldSize);
	/*if (ERR_OK != Err)
	{
		PrintErr("CAS_IR_NVR_Partition_Write ERROR!!222 %08X", Err);
		return HIR_NOT_OK;
	}*/

#else
#if 0
	switch(ePartType)
	{
		case TIRNVR_IRDETO :
			ulExtZoneOffset = ulPartOffset;
			break;
		case TIRNVR_CA :
			ulExtZoneOffset = (IR_PARTITION_CA_OFFSET -IR_PARTITION_IRDETO_OFFSET)+ulPartOffset;
			break;
		case TIRNVR_LOADER :
			ulExtZoneOffset = (IR_PARTITION_LOADER_OFFSET - IR_PARTITION_IRDETO_OFFSET)+ulPartOffset;
			break;

		default:
			PrintErr(" %s, Invalid partition(%d)\n", __FUNCTION__, ePartType );
			return HIR_NOT_OK;
	}

	if(ulExtZoneOffset >E2P_EXT_ZONE_SIZE-1)
	{
		PrintErr("%s, Invalid offset(0x%X)\n", __FUNCTION__, ulExtZoneOffset );
		return HIR_NOT_OK;
	}

	PrintDebug("%s, ulExtZoneOffset = 0x%X\n", __FUNCTION__, ulExtZoneOffset);

	if(ERR_EEPROM_OK != PAL_EEPROM_Write(eE2P_EXT_ZONE_ID,ulExtZoneOffset,pucData, ulSize) )
	{
		PrintErr("%s, PAL_EEPROM_Write error\n ", __FUNCTION__ );
		return HIR_NOT_OK;
	}
#endif
	//VK_TASK_Sleep(1000);
#endif

	return HIR_OK;
}

HUINT32 TIR_FLASH_LOADER_Init(void)
{
	if(HIR_OK !=	TIR_FLASH_Read_BBCB(&g_tIr_bbcb) )
	{
		printf("bbcb read fail\n") ;
		return HIR_NOT_OK;
	}
	if(HIR_OK !=	TIR_FLASH_Read_CPCB(g_tIr_cpcb) )
	{
		printf("cpcb or spcb read fail \n" );
		return HIR_NOT_OK;
	}

	return HIR_OK;
}
HUINT32 TIR_FLASH_Read_CPCB_AfterErase(void)
{
	if(HIR_OK !=	TIR_FLASH_Read_CPCB(g_tIr_cpcb) )
	{
		PrintErr("cpcb read fail" );
		return HIR_NOT_OK;
	}
	return HIR_OK;
}
HIR_ERR TIR_FLASH_Read_BBCB(Boot_Block_Control_Block *pBBCB)
{
	//HUINT32	result=0;
	HUINT32	result=ERR_OK;
	HUINT8 aucBBCB[IR_BBCB_SIZE];
	HxLOG_Print("%s\n", __FUNCTION__ );


#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
		//result = PAL_NVRAM_Read(ePAL_NVRAM_FS_PARTITION_RO, IRDETO_BBCB, 0, (unsigned char *)&aucBBCB, IR_BBCB_SIZE );
		result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB, 0, (unsigned char *)&aucBBCB, IR_BBCB_SIZE);
#else
		result = DI_FLASH_Read( IR_FLASH_BBCB_OFFSET ,  (unsigned char *)&aucBBCB, IR_BBCB_SIZE );
#endif
	if (ERR_OK != result)
	{
		printf("%s, DI_FLASH_Read %08X", __FUNCTION__, result  );
		//HxLOG_CRITICAL("");
		return HIR_NOT_OK;
	}

	/* When you tried to get real value from nvram, you must not use a structure copy method due to fragmentation, jichoi. */
	pBBCB->usCRC16 			= TIR_Get16Bits(aucBBCB + BBCB_CRC16_OFFSET);
	pBBCB->ucManufacturerCode	= aucBBCB[BBCB_MANCODE_OFFSET] ;
	pBBCB->ucHardwareCode		= aucBBCB[BBCB_HWCODE_OFFSET] ;
	pBBCB->ucLoaderMajor		= aucBBCB[BBCB_LOADERMAJOR_OFFSET] ;
	pBBCB->ucLoaderMinor		= aucBBCB[BBCB_LOADERMINOR_OFFSET] ;
	pBBCB->ucLoaderType		= aucBBCB[BBCB_LOADERTYPE_OFFSET] ;
	pBBCB->ulSerialNumber		= TIR_Get32Bits(aucBBCB + BBCB_SERIALNUM_OFFSET);
	pBBCB->ulRandomNumber 	= TIR_Get32Bits(aucBBCB + BBCB_RANDOMNUM_OFFSET);


	printf("\n\n___________________BBCB__DATA_______________________\n");
	printf("usCRC16 = 0x%x\n", pBBCB->usCRC16);
	printf("ucManufacturerCode = 0x%x\n", pBBCB->ucManufacturerCode);
	printf("ucHardwareCode = 0x%x\n", pBBCB->ucHardwareCode);
	printf("ucLoaderMajor = 0x%x\n", pBBCB->ucLoaderMajor);
	printf("ucLoaderMinor = 0x%x\n", pBBCB->ucLoaderMinor);
	printf("ucLoaderType = 0x%x\n", pBBCB->ucLoaderType);
	printf("ulSerialNumber = 0x%lx\n", pBBCB->ulSerialNumber);
	printf("ulRandomNumber = 0x%lx\n", pBBCB->ulRandomNumber);
	printf("\n_____________________________________________________\n");
	return HIR_OK;

}

HIR_ERR TIR_FLASH_Read_CPCB(HUINT8 * pCPCB)
{
	HUINT32	result = ERR_OK;
	HxLOG_Info("%s\n", __FUNCTION__);

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
		//result = PAL_NVRAM_Read(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_CPCB_ENCRYPTED, 0, pCPCB, IR_CPCB_SIZE );
		result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED, 0, pCPCB, IR_CPCB_SIZE);
#else
		//result = PAL_NVRAM_Read(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_CPCB, 0, pCPCB, IR_CPCB_SIZE );
		result = irldvt_svc_cas_DevSysGetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB, 0, pCPCB, IR_CPCB_SIZE);
#endif	// CONFIG_MW_CAS_IRDETO_SECURE

#else
		//result = DI_FLASH_Read( IR_FLASH_CPCB_OFFSET ,  pCPCB, IR_CPCB_SIZE );
#endif	// CONFIG_MW_SYS_NAND_FLASH_ONLY

#if defined(CONFIG_MW_CAS_IRDETO_SECURE) && !defined(WIN32)
	{
		#if 0
		stCrypt_t 		stCrytInfo;
		DI_ERR_CODE		eErr;


	#ifdef DUMP_SECURED_CCB
	{
		int nCCB_Idx;
		unsigned char *pucPosCCB = pCPCB+CCB_Part_Crc_Info_Start;

		DI_UART_Print("*** Encrypted CCB_Partition Data ***\n");
		for(nCCB_Idx=0 ; nCCB_Idx < FLASH_TOTAL_PARTITION_COUNT ; nCCB_Idx++)
		{
			DI_UART_Print("nCCB_Idx[%d]: %02X %02X %02X %02X %02X %02X %02X %02X\n", nCCB_Idx,
				*(pucPosCCB), *(pucPosCCB+1), *(pucPosCCB+2), *(pucPosCCB+3), *(pucPosCCB+4), *(pucPosCCB+5), *(pucPosCCB+6) , *(pucPosCCB+7) );
			pucPosCCB+=CCB_Part_One_Crc_Info_Length;
		}
		DI_UART_Print("\n");
	}
	#endif

		VK_memset32(&stCrytInfo, 0, sizeof(stCrypt_t));

		stCrytInfo.pSrcBuffer = DI_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);

		if(stCrytInfo.pSrcBuffer == NULL)
		{
			PrintErr("[TIR_FLASH_Read_CPCB] pSrcBuffer VK_MEM_Alloc fail\n");
		}

		stCrytInfo.pDstBuffer = DI_CRYPT_MemAlloc(CCB_Crc32 - CCB_Part_Crc_Info_Start);

		if(stCrytInfo.pDstBuffer == NULL)
		{
			PrintErr("[TIR_FLASH_Read_CPCB] pDstBuffer VK_MEM_Alloc fail\n");
		}

		stCrytInfo.etCryptMode 	= DI_CRYPT_TDES;
		stCrytInfo.etCryptKey	= DI_CRYPT_DEFAULT_SYSTEM_KEY;
		stCrytInfo.etCryptData 	= DI_CRYPT_BLOCK;
		stCrytInfo.ulBuffSize 	= CCB_Crc32 - CCB_Part_Crc_Info_Start;
		stCrytInfo.pKey		= NULL;
		stCrytInfo.pIvKey		= NULL;

		VK_MEM_Memcpy(stCrytInfo.pSrcBuffer, pCPCB+CCB_Part_Crc_Info_Start, CCB_Crc32-CCB_Part_Crc_Info_Start);

		eErr = DI_CRYPT_Decrypt(&stCrytInfo);

		if(eErr != DI_ERR_OK)
		{
			PrintErr("[TIR_FLASH_Read_CPCB] error DI_CRYPT_Decrypt = 0x%x\n", eErr );
		}

		VK_MEM_Memcpy(pCPCB+CCB_Part_Crc_Info_Start, stCrytInfo.pDstBuffer, CCB_Crc32-CCB_Part_Crc_Info_Start);

		DI_CRYPT_MemFree(stCrytInfo.pSrcBuffer);
		DI_CRYPT_MemFree(stCrytInfo.pDstBuffer);

	#ifdef DUMP_SECURED_CCB
		{
			int nCCB_Idx;
			unsigned char *pucPosCCB = pCPCB+CCB_Part_Crc_Info_Start;

			DI_UART_Print("*** Original CCB_Partition Data ***\n");
			for(nCCB_Idx=0 ; nCCB_Idx <FLASH_TOTAL_PARTITION_COUNT; nCCB_Idx++)
			{
				DI_UART_Print("nCCB_Idx[%d]: %02X %02X %02X %02X %02X %02X %02X %02X\n", nCCB_Idx,
					*(pucPosCCB), *(pucPosCCB+1), *(pucPosCCB+2), *(pucPosCCB+3), *(pucPosCCB+4), *(pucPosCCB+5), *(pucPosCCB+6) , *(pucPosCCB+7) );
				pucPosCCB+=CCB_Part_One_Crc_Info_Length;
			}
			DI_UART_Print("\n");
		}
	#endif
	#endif

	}
#endif
	if (ERR_OK != result)
	{
		HxLOG_Print("svc_cas_DevSysGetField", result );
		return HIR_NOT_OK;
	}

	return HIR_OK;
}


HIR_ERR TIR_FLASH_Erase_CPCB(void)
{
	HINT32 ret_result = 0;
	unsigned char *pucData;

	pucData = VK_MEM_Alloc(IR_CPCB_SIZE);
	memset( pucData , 0xff , IR_CPCB_SIZE);

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
//edited by hmjoo
	ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED, 0, pucData, IR_CPCB_SIZE );
#else
	ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB, 0, pucData, IR_CPCB_SIZE);
#endif
	if (ERR_OK != ret_result)
	{
		PrintErr("[TIR_FLASH_Erase_CPCB LDR ERROR : %d]\n", ret_result);
		return HIR_NOT_OK;
	}
#else
	ret_result = DI_EEPROM_Write( IR_PARTITION_IRDETO_SPCB_OFFSET - DI_EEP_SYS_RESERVED_SIZE, \
								pucData, \
								IR_PARTITION_IRDETO_SPCB_SIZE )
	if (ERR_OK != ret_result)
	{
		PrintErr("[TIR_FLASH_Erase_CPCB EEPROM LDR ERROR : %d]\n", ret_result);
		return HIR_NOT_OK;
	}
#endif

	VK_MEM_Free(pucData);

	return HIR_OK;
}

extern DI_ERR_CODE	DI_NANDFLASH_Read(HUINT32 ulAddress, HUINT8* pucBuffer, HUINT32 ulSize);
HIR_ERR TIR_FLASH_Read(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize)
{
	HUINT32	result;
	PrintDebug("%s\n", __FUNCTION__);

#if defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)
	result = DI_NANDFLASH_Read_ForAllArea( ulOffset , pData, ulSize );
#elif defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	result = DI_NANDFLASH_Read( ulOffset , pData, ulSize );
#else
	result = DI_FLASH_Read( ulOffset , pData, ulSize );
#endif
	if (ERR_OK != result)
	{
		PrintErr("%s,  DI_FLASH_Read %08X \n",__FUNCTION__,  result );

		return HIR_NOT_OK;
	}

	PrintDebug("%s,  DI_FLASH_Read %08X \n",__FUNCTION__,  *(pData+FLASH_APPL_BLOCK_SIZE-1) );

	return HIR_OK;
}

HIR_ERR TIR_FLASH_Write(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize)
{
	HUINT32	result = 0;
	PrintDebug("%s\n", __FUNCTION__);

#if defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)
	result = DI_NANDFLASH_Write_ForAllArea( ulOffset , pData, ulSize );
#elif defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	result = DI_NANDFLASH_Write( ulOffset , pData, ulSize );
#else
	result = DI_FLASH_Write( ulOffset , pData, ulSize );
#endif
	if (ERR_OK != result)
	{
		PrintErr("%s,  DI_FLASH_Write %08X \n", __FUNCTION__, result );

		return HIR_NOT_OK;
	}

	PrintDebug("%s,  DI_FLASH_Write %08X \n", __FUNCTION__, *(pData+FLASH_APPL_BLOCK_SIZE-1)  );

	return HIR_OK;
}

HIR_ERR	TIR_FLASH_EraseOffset(HUINT32 ulOffset)
{
#if defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)
	HUINT32	result;
	unsigned char	*pucPatternBuf;

	PrintDebug("%s\n", __FUNCTION__);

	pucPatternBuf = VK_MEM_Alloc(FLASH_APPL_BLOCK_SIZE);
	memset(pucPatternBuf, 0x00, FLASH_APPL_BLOCK_SIZE );

	result = DI_NANDFLASH_Write_ForAllArea(ulOffset, pucPatternBuf, FLASH_APPL_BLOCK_SIZE);

	if (ERR_OK != result)
	{
		PrintErr("%s,  DI_FLASH_Write %08X", __FUNCTION__, result );
		VK_MEM_Free(pucPatternBuf);
		return HIR_NOT_OK;
	}

	VK_MEM_Free(pucPatternBuf);
	return HIR_OK;
#elif defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)

	return HIR_OK;

#else
	HUINT32	result;

	PrintDebug("%s\n", __FUNCTION__);

	result = DI_FLASH_EraseBlock(ulOffset  / FLASH_APPL_BLOCK_SIZE);
	if (ERR_OK != result)
	{
		PrintErr("%s,  DI_FLASH_Write %08X", __FUNCTION__, result );

		return HIR_NOT_OK;
	}
	return HIR_OK;
#endif
}

HUINT32	TIR_FLASH_CountPattern(HUINT8 ucPartIdx, HUINT16 *pusNumPattern)
{

	DI_ERR_CODE di_err;
	unsigned char	pattern;
	HUINT32	pattern_cnt, sub_pat_cnt, ulFlashOffset;
	unsigned long	i;
	int			nRet;
	unsigned char* pucPatternBuf;

	PrintDebug("%s, ucPartIdx = %d\n", __FUNCTION__, ucPartIdx);

	pattern_cnt = sub_pat_cnt = nRet = 0;

	if(ucPartIdx <= FLASH_TOTAL_PARTITION_COUNT)
	{
		// change index to real flash block num
		ucPartIdx = ucPartIdx - 1;  //ucPartIdx start from 1.

		pucPatternBuf = VK_MEM_Alloc(FLASH_APPL_BLOCK_SIZE);

		TIR_FLASH_ConvertFlashOffset(ucPartIdx, &ulFlashOffset);

		di_err = TIR_FLASH_Read( ulFlashOffset , pucPatternBuf , FLASH_APPL_BLOCK_SIZE );
		if( di_err )
		{
			PrintErr("[dl_FLASH_Read ERROR : %d]\n", di_err);
			nRet = di_err;
			VK_MEM_Free(pucPatternBuf);
			return nRet;
		}
		for( i = 0; i < FLASH_APPL_BLOCK_SIZE; i++ )
		{
			pattern = pucPatternBuf[i];
			if(pattern == 0)
			{
				sub_pat_cnt=1;
			}
			else if(pattern == sub_pat_cnt)
			{
				sub_pat_cnt++;
				if(sub_pat_cnt == 0x100)
				{
					pattern_cnt++;
					sub_pat_cnt=0;
				}
			}
			else
				sub_pat_cnt=0;
		}

		VK_MEM_Free(pucPatternBuf);

	}
	else
	{
		PrintErr("[INVALID PARTITION INDEX]\n");
		nRet = -1;
	}

	PrintDebug("%s, pattern_cnt = %d\n", __FUNCTION__, pattern_cnt);

	if( nRet )
	{
		return 1;
	}
	else
	{
		*pusNumPattern = (signed short)pattern_cnt;
		return 0;
	}

}

HUINT32	TIR_FLASH_WritePattern(HUINT8 ucPartIdx)
{

	unsigned char 	*pucPatternBuf;
	int				i, nRet=0;

	DI_ERR_CODE	nErrCode;
	HUINT32	ulFlashOffset;

	nRet = 0;

	PrintDebug("%s, ucPartIdx = %d\n", __FUNCTION__, ucPartIdx);

	// make pattern data
	pucPatternBuf = VK_MEM_Alloc(FLASH_APPL_BLOCK_SIZE);

	for( i = 0; i < FLASH_APPL_BLOCK_SIZE; i++ )
	{
		pucPatternBuf[i] = (unsigned char)(i& 0xff);
	}

	// change index to real flash block num
	ucPartIdx = ucPartIdx - 1;	//ucPartIdx start from 1.

	TIR_FLASH_ConvertFlashOffset(ucPartIdx, &ulFlashOffset);

//octo di api, not nesecsary erase flash,	nErrCode = DDI_FLASH_Erase_ALL(ucPartIdx);
	nErrCode = TIR_FLASH_EraseOffset(ulFlashOffset);
	if (nErrCode == 0)
	{
		nErrCode = TIR_FLASH_Write( ulFlashOffset , pucPatternBuf, FLASH_APPL_BLOCK_SIZE );
		if (nErrCode)
		{
			PrintErr("[TIR_FLASH_Write, ERROR : %d]\n", nErrCode);
			nRet = nErrCode;
		}
	} else
	{
		PrintErr("[TIR_FLASH_Write, ERROR : %d]\n", nErrCode);
		nRet = nErrCode;
	}


	VK_MEM_Free(pucPatternBuf);

	if( nRet )
		return 1;
	else
		return 0;

}

HUINT32	TIR_FLASH_ModifyPattern(HUINT8 ucPartIdx)
{

	HUINT32	ulFlashOffset;
	DI_ERR_CODE	nErrCode;

	PrintDebug("%s, ucPartIdx = %d\n", __FUNCTION__, ucPartIdx);

	// change index to real flash block num
	ucPartIdx = ucPartIdx - 1;	//ucPartIdx start from 1.

	TIR_FLASH_ConvertFlashOffset(ucPartIdx, &ulFlashOffset);

	TIR_FLASH_Read( ulFlashOffset , s_aucBlockData, FLASH_APPL_BLOCK_SIZE);

	PrintDebug("\n\t Read LastByte [0x%x] \n", s_aucBlockData[FLASH_APPL_BLOCK_SIZE-1]);

	/* TOGGLE */
	s_aucBlockData[FLASH_APPL_BLOCK_SIZE-1] ^= 0x01;

	PrintDebug("\t Write LastByte [0x%x] \n\n", s_aucBlockData[FLASH_APPL_BLOCK_SIZE-1]);

	nErrCode = TIR_FLASH_EraseOffset(ulFlashOffset);
	if (nErrCode == 0)
	{
		nErrCode = TIR_FLASH_Write(ulFlashOffset, s_aucBlockData, FLASH_APPL_BLOCK_SIZE );
		if( nErrCode )
		{
			PrintErr("[TIR_FLASH_Write ERROR : %d]\n", nErrCode);
			return 1;
		}
	}
	else
	{
		PrintErr("[TIR_FLASH_EraseOffset ERROR : %d]\n", nErrCode);
		return 1;
	}

	return 0;
}

HUINT32	TIR_FLASH_ErasePattern(HUINT8 ucPartIdx)
{
	DI_ERR_CODE	nErrCode;
	HUINT32	ulFlashOffset;

	PrintDebug("%s, ucPartIdx = %d\n", __FUNCTION__, ucPartIdx);
	// change index to real flash block num
	ucPartIdx = ucPartIdx - 1;	//ucPartIdx start from 1.

	TIR_FLASH_ConvertFlashOffset(ucPartIdx, &ulFlashOffset);

	memset(s_aucBlockData, 0x00, FLASH_APPL_BLOCK_SIZE );

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
	nErrCode = TIR_FLASH_Write( ulFlashOffset, s_aucBlockData  , FLASH_APPL_BLOCK_SIZE);
#else
	nErrCode = TIR_FLASH_EraseOffset(ulFlashOffset);
#endif
	if (nErrCode)
	{
		PrintErr("[TIR_FLASH_Write ERROR : %d]\n", nErrCode);
		return 1;
	}
	return 0;
}

HUINT32	TIR_FLASH_CountNCPartition(HUINT16 *pusPartitionCount)
{

	int				i;
	unsigned long		offcrc, p_crc_32;
	unsigned char		tmp_p_cnt, tmp_nc_cnt, non_crc_mask;
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
	unsigned long		p_crc_32_1,p_crc_32_2, p_crc_32_3;
#endif
	tmp_p_cnt = g_tIr_cpcb[CCB_Partition_Count];

	tmp_nc_cnt = 0;

	for( i = 0; i < tmp_p_cnt; i++)
	{
		offcrc = (unsigned long)i * CCB_Part_One_Crc_Info_Length;

		non_crc_mask = g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc];

#if defined(CONFIG_PROD_IR2000SD)
		p_crc_32 = *((unsigned short *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32]));
		p_crc_32 = (p_crc_32 <<16) | (*((unsigned short *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32 + 2])));
#else
		p_crc_32 = *((unsigned long *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32]));
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
		p_crc_32_1= *((unsigned long *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32+4]));
		p_crc_32_2= *((unsigned long *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32+8]));
		p_crc_32_3 = *((unsigned long *)(&g_tIr_cpcb[CCB_Part_Crc_Info_Start + offcrc + CCB_Part_Crc_Info_P_CRC_32+12]));
#endif

#endif

		if(non_crc_mask == 0xA5 && p_crc_32 == 0xA5A5A5A5
#if  defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V32)
								&& p_crc_32_1 == 0xA5A5A5A5
								&& p_crc_32_2== 0xA5A5A5A5
								&& p_crc_32_3== 0xA5A5A5A5
#endif
		)
		{
				tmp_nc_cnt++;
	}

	}

	*pusPartitionCount = tmp_nc_cnt;

	return 0;

}

void TIR_FLASH_ConvertFlashOffset(HUINT8 ucPartIdx, HUINT32 * pulOffsetOut )
{
	HUINT32 ulFlashOffset;

	PrintDebug("%s, ucPartIdx = %d\n", __FUNCTION__, ucPartIdx);

	ulFlashOffset = ucPartIdx*FLASH_APPL_BLOCK_SIZE;

#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
#if defined(CONFIG_PROD_VTCHD06)
	// NAND 128M MAP
	if(ucPartIdx < 74 /* rootfs + kernel */ )
	{
		ulFlashOffset += 0x00400000; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else if(ucPartIdx < (74+8) /* rootfs + kernel + DB Backup */ )
	{
		ulFlashOffset = 0x05600000 /*IR_FLASH_DBBACKUP_START_ADDR*/ + ((ucPartIdx-74)*FLASH_APPL_BLOCK_SIZE);
	}
	else //if(ucPartIdx >= (74+8) /* rootfs + kernel + DB Backup */ )
	{
		ulFlashOffset = 0x05B00000 /*IR_FLASH_DB_START_ADDR*/ + ((ucPartIdx-74-8)*FLASH_APPL_BLOCK_SIZE);
	}

#elif defined(CONFIG_PROD_IR3000HD) || defined(CONFIG_PROD_IR3020HD) || defined(CONFIG_PROD_IR3100HD) || defined(CONFIG_PROD_IR3030HD) || defined(CONFIG_PROD_HM9504HD) || defined(CONFIG_PROD_HDR1003S)
	// NAND 128M MAP
	if(ucPartIdx < 74 /* rootfs + kernel */ ) // CRC (Application)
	{
		ulFlashOffset += 0x00400000; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else // if(ucPartIdx < (74+8) /* rootfs + kernel + DB */ )	// NON CRC2: DB
	{
		ulFlashOffset = 0x05B00000 /*IR_FLASH_DBBACKUP_START_ADDR*/ + ((ucPartIdx-74)*FLASH_APPL_BLOCK_SIZE);
	}
#elif defined(CONFIG_PROD_IR4000HD)
	// NAND 256M MAP
	if(ucPartIdx < 136 /* rootfs + kernel */ ) // CRC (Application)
	{
		ulFlashOffset += 0x00400000; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else // if(ucPartIdx < (136+32) /* rootfs + kernel + DB */ )	// NON CRC2: DB
	{
		ulFlashOffset = 0x0AC00000 /*IR_FLASH_DBBACKUP_START_ADDR*/ + ((ucPartIdx-136)*FLASH_APPL_BLOCK_SIZE);
	}

#else	// for UD-FOX/HD and others
	// NAND 128M MAP
	if(ucPartIdx < 74 /* rootfs + kernel */ ) // CRC (Application)
	{
		ulFlashOffset += 0x00400000; /*IR_FLASH_ROOTFS_START_ADDR*/
	}
	else if(ucPartIdx < (74+8) /* rootfs + kernel + DB Backup */ )	// NON CRC1 : DB Backup
	{
		ulFlashOffset = 0x05600000 /*IR_FLASH_DBBACKUP_START_ADDR*/ + ((ucPartIdx-74)*FLASH_APPL_BLOCK_SIZE);
	}
	else if(ucPartIdx < (74+8+8) /* rootfs + kernel + DB  */ )	// NON CRC2 : DB
	{
		ulFlashOffset = 0x05B00000 /*IR_FLASH_DB_START_ADDR*/ + ((ucPartIdx-74-8)*FLASH_APPL_BLOCK_SIZE);
	}
	else //if(ucPartIdx >= (74+8+8) /* rootfs + kernel + DB Backup + DB*/ )	// NON CRC3 : DB User
	{
		ulFlashOffset = 0x06000000 /*IR_FLASH_DB_START_ADDR*/ + ((ucPartIdx-74-8-8)*FLASH_APPL_BLOCK_SIZE);
	}

#endif
#else
#if defined(SUPPORT_IRDETO_LOADER_FLASH_32MB)
	//  It's conversion for 32 mb flash.
	if(ucPartIdx > 223)
	{
		ulFlashOffset = ulFlashOffset+ (FLASH_APPL_BLOCK_SIZE *16);
	}

#elif defined(SUPPORT_IRDETO_LOADER_FLASH_32MB_2ND)

	if(ucPartIdx < IR_FLASH_KERNEL_BLOCK_COUNT)
	{
		ulFlashOffset += IR_FLASH_KERNEL_START_ADDR;
	}
	else
	{
		ulFlashOffset -= IR_FLASH_KERNEL_SIZE;
	}

#elif defined(SUPPORT_IRDETO_LOADER_FLASH_8MB)

		ulFlashOffset += IR_FLASH_APPLICATION_OFFSET;

#elif defined(SUPPORT_IRDETO_LOADER_NOR_FLASH_4MB)

	if(ucPartIdx >= (IR_FLASH_KERNEL_BLOCK_COUNT + IR_FLASH_ROOTFS_BLOCK_COUNT))
	{
		ulFlashOffset += (IR_FLASH_KERNEL_START_ADDR+ IR_FLASH_BG_OTA_SIZE + IR_FLASH_RESEVED_OTA_SIZE);
	}
	else
	{
		ulFlashOffset += IR_FLASH_KERNEL_START_ADDR;
	}

#endif
#endif
	* pulOffsetOut = ulFlashOffset;

	PrintDebug("%s, part idx=%d, ulFlashOffset = 0x%X\n", __FUNCTION__, ucPartIdx, ulFlashOffset );

}


HINT32 TIR_E2P_Partition_Erase( TIRNVR_Type NvrType)
{
	unsigned char *pucData;
	HINT32 ret_result = 0;

	switch(NvrType)
	{
		case TIRNVR_LOADER:
			pucData = VK_MEM_Alloc( IR_PARTITION_LOADER_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_LOADER_SIZE );
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_LOADER_PARTITION, 0, pucData, IR_PARTITION_LOADER_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION, 0, pucData, IR_PARTITION_LOADER_SIZE);
			{
				PrintErr("[TIR_E2P_Partition_Erase LDR ERROR : %d]\n", ret_result);
			}
#else
			ret_result = DI_EEPROM_Write( IR_PARTITION_LOADER_OFFSET - DI_EEP_SYS_RESERVED_SIZE, \
										pucData, \
										IR_PARTITION_LOADER_SIZE );
#endif
			VK_MEM_Free(pucData);
			break;

		case TIRNVR_IRDETO:
			pucData = VK_MEM_Alloc( IR_PARTITION_IRDETO_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_IRDETO_SIZE );
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_IR_PARTITION, 0, pucData, IR_PARTITION_IRDETO_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION, 0, pucData, IR_PARTITION_LOADER_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase IRDETO ERROR : %d]\n", ret_result);
			}

			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IR_BACKUP_PARTITION, 0, pucData, IR_PARTITION_IRDETO_BACKUP_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION, 0, pucData, IR_PARTITION_IRDETO_BACKUP_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase IRDETO BACKUP ERROR : %d]\n", ret_result);
			}
#else
			ret_result = DI_EEPROM_Write( IR_PARTITION_IRDETO_OFFSET- DI_EEP_SYS_RESERVED_SIZE, \
										pucData, \
										IR_PARTITION_IRDETO_SIZE );
#endif
			VK_MEM_Free(pucData);

			break;

		case TIRNVR_CA:
			pucData = VK_MEM_Alloc( IR_PARTITION_CA_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_CA_SIZE );
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_CA_PARTITION, 0, pucData, IR_PARTITION_CA_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION, 0, pucData, IR_PARTITION_CA_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase CA ERROR : %d]\n", ret_result);
			}
#else
			ret_result = DI_EEPROM_Write( IR_PARTITION_CA_OFFSET - DI_EEP_SYS_RESERVED_SIZE, \
										pucData,\
										IR_PARTITION_CA_SIZE );
#endif
			VK_MEM_Free(pucData);
			break;

		case TIRNVR_NO_CA_EXT_ZONE:
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
// encrpyted spcb or cpcb를  지운다.
			if(TIR_FLASH_Erase_CPCB() != HIR_OK) PrintDebug(" ++++ TIR_FLASH_Erase_CPCB Fail ++++ \n");
			else 								 PrintDebug(" ++++ TIR_FLASH_Erase_CPCB OK ++++ \n");

#else
			pucData = VK_MEM_Alloc( IR_PARTITION_NO_CA_EXTZONE_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_NO_CA_EXTZONE_SIZE );
			ret_result = DI_EEPROM_Write( IR_PARTITION_NO_CA_EXTZONE_OFFSET -DI_EEP_SYS_RESERVED_SIZE, \
										pucData,\
										IR_PARTITION_NO_CA_EXTZONE_SIZE );
			VK_MEM_Free(pucData);
#endif
			break;

		case TIRNVR_ALL_EXT_ZONE: /* No all area eeprom due to prvent to effect system boot and.. special thing....... */
#if defined(CONFIG_MW_SYS_NAND_FLASH_ONLY)
			/* IRDETO / CA / LOADER Field Erase */
			pucData = VK_MEM_Alloc( IR_PARTITION_CA_SIZE );
			memset( pucData , 0x00 , IR_PARTITION_CA_SIZE );

			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_LOADER_PARTITION, 0, pucData, IR_PARTITION_LOADER_SIZE);

			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION,0,pucData,IR_PARTITION_LOADER_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase LOADER ERROR : %d]\n", ret_result);
			}

			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_IR_PARTITION, 0, pucData, IR_PARTITION_LOADER_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION,0,pucData,IR_PARTITION_LOADER_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase IRDETO IR ERROR : %d]\n", ret_result);
			}

			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IRDETO_CA_PARTITION, 0, pucData, IR_PARTITION_CA_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION,0,pucData,IR_PARTITION_CA_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase CA ERROR : %d]\n", ret_result);
			}

			//ret_result = PAL_NVRAM_Write(ePAL_NVRAM_FS_PARTITION_RW, IR_BACKUP_PARTITION, 0, pucData, IR_PARTITION_IRDETO_BACKUP_SIZE);
			ret_result = irldvt_svc_cas_SysSetField(IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,0,pucData,IR_PARTITION_IRDETO_BACKUP_SIZE);
			if (ERR_OK != ret_result)
			{
				PrintErr("[TIR_E2P_Partition_Erase IRETO BACKUP ERROR : %d]\n", ret_result);
			}
// encrpyted spcb or cpcb를  지운다.
			if(TIR_FLASH_Erase_CPCB() != HIR_OK) PrintDebug(" ++++ TIR_FLASH_Erase_CPCB Fail ++++ \n");
			else 								 PrintDebug(" ++++ TIR_FLASH_Erase_CPCB OK ++++ \n");

			VK_MEM_Free(pucData);
#else
			pucData = VK_MEM_Alloc( IR_PARTITION_ALL_EXTZONE_SIZE  );
			memset( pucData , 0x00 , IR_PARTITION_ALL_EXTZONE_SIZE );
			ret_result = DI_EEPROM_Write( IR_PARTITION_ALL_EXTZONE_OFFSET - DI_EEP_SYS_RESERVED_SIZE, \
										pucData,\
										IR_PARTITION_ALL_EXTZONE_SIZE );
			VK_MEM_Free(pucData);
#endif
			break;

		default:
			PrintDebug("%s, %d, what??? %d\n", __FILE__, __LINE__, NvrType);
			break;
	}

	return (DI_ERR_OK != ret_result )? 1: 0;

}

