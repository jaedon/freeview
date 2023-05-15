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

#ifndef __TIR_NVR_H__
#define	__TIR_NVR_H__
#include "htype.h"
#include "tir_platform.h"

#include "ir_loader_v5.h"


#include <pal_sys.h>
#include <pal_param.h>
//#include "_svc_cas_sub_dev_api.h"
#include "_svc_cas_mgr_nvram.h"

//#include <irldvt_type.h>


typedef enum{
	TIRNVR_IRDETO = 1,
	TIRNVR_CA,
	TIRNVR_LOADER,
	TIRNVR_RESET_TO_DEFAULT,
	TIRNVR_ENTIRE_NVRAM
}TIRNVR_Type;

typedef enum
{
	IRLDVT_DEV_FLASH_FIELD_SERIAL_NO = 0,
	IRLDVT_DEV_FLASH_FIELD_DTCP_KEY,
	IRLDVT_DEV_FLASH_FIELD_CI_PLUS_KEY,
	IRLDVT_DEV_FLASH_FIELD_MAC_ADDR,
	IRLDVT_DEV_FLASH_FIELD_HDCP_KEY,
	IRLDVT_DEV_FLASH_FIELD_PARING_DATA,
	IRLDVT_DEV_FLASH_FIELD_SYSTEM_ID,
	IRLDVT_DEV_FLASH_FIELD_LOADER_VER,
	IRLDVT_DEV_FLASH_FIELD_APP_VER,
	IRLDVT_DEV_FLASH_FIELD_PRIVATE,
	IRLDVT_DEV_FLASH_FIELD_CSC_KEY,

	IRLDVT_DEV_FLASH_FIELD_IRDETO_BBCB,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_CA_PARTITION,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_PARTITION,
	IRLDVT_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,

	IRLDVT_DEV_FLASH_FIELD_MAX
} irldvt_DevFlashField_e;



HUINT32 TIR_FLASH_LOADER_Init(void);

HIR_ERR TIR_MOD_Read_BBCB(Boot_Block_Control_Block *pBBCB);


HIR_ERR TIR_FLASH_Read(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize);
HIR_ERR TIR_FLASH_Write(HUINT32 ulOffset, HUINT8 * pData, HUINT32 ulSize);
HIR_ERR	TIR_FLASH_Erase(HUINT32 ulOffset);
HUINT32	TIR_MOD_CountPattern(HUINT16 usModule_id, HUINT32 *pulNumPattern);
HUINT32	TIR_MOD_WritePattern(HUINT16 usModule_id);
HUINT32	TIR_MOD_ModifyLastBit(HUINT16 usModule_id);
HUINT32	TIR_MOD_Erase(HUINT16 usModule_id);
HUINT32	TIR_MOD_CountModuleInPlatform(HUINT16 *pusPartitionCount);
void TIR_MOD_ConvertFlashOffset(HUINT16 usMod_id, HUINT32 * pulOffsetOut );
HINT32 TIR_MEM_Manipulation_Erase( TIRNVR_Type NvrType);
HIR_ERR TIR_E2P_Partition_Write( TIRNVR_Type ePartType, HUINT32 ulPartOffset, void *pucData, HUINT32 ulSize);
HIR_ERR TIR_E2P_Partition_Read( TIRNVR_Type ePartType, HUINT32 ulPartOffset, void *pucData, HUINT32 ulSize );

HERROR	irldvt_svc_cas_SysConvertFlashFieldTypeRmToPalParam(irldvt_DevFlashField_e inType, PAL_PARAM_Item_e *peCnvtType);
HERROR	irldvt_svc_cas_SysConvertFlashFieldTypeRmToPal(irldvt_DevFlashField_e inType, PAL_FLASH_FIELD_t *pCnvtType);
HERROR irldvt_svc_cas_DevSysGetField(irldvt_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
HERROR irldvt_svc_cas_SysSetField(irldvt_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);

#endif

