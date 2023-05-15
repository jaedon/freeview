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

#ifndef __IR_NVR_H__
#define	__IR_NVR_H__

#include <ir_platform.h>
#include <_svc_cas_mgr_nvram.h>

typedef enum{
	IRNVR_PART_IRDETO = 1,
	IRNVR_PART_CA,
	IRNVR_PART_LOADER,

	IRNVR_WHOLE_AREA
}IRNVR_PART_Type;


HIR_ERR CAS_IR_NVR_FlashReadMail(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HIR_ERR CAS_IR_NVR_FlashWriteMail(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HIR_ERR IR_NVR_EepromRead(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HIR_ERR IR_NVR_EepromWrite(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);

HIR_ERR CAS_IR_NVR_FlashEraseMailFile(void);
HIR_ERR CAS_IR_NVR_Partition_Read( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset,HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_Partition_Write( IRNVR_PART_Type ePartType, HUINT32 ulPartOffset, HUINT8 *pucData, HUINT32 ulSize);

// ewlee 20100917 - for ppv
HIR_ERR CAS_IR_NVR_FlashReadPPVList(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HIR_ERR CAS_IR_NVR_FlashWritePPVList(HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);

#endif
