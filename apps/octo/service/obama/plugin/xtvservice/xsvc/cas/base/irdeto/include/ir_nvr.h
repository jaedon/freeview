
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

#if defined(IRPLAT_SYS_SAVE_SMARTCARD_NUMBER)
HIR_ERR CAS_IR_NVR_ReadSmartcardNumber (HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_SaveSmartcardNumber (HUINT8 *pucData, HUINT32 ulSize );
#endif
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) && defined (CONFIG_OP_ALMAJD)
HIR_ERR CAS_IR_NVR_ReadHomingChannel (HUINT8 ucSlot, HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_SaveHomingChannel (HUINT8 ucSlot, HUINT8 *pucData, HUINT32 ulSize );
#endif
#if defined (CONFIG_OP_ALMAJD)
HIR_ERR CAS_IR_NVR_ReadRegionCode (HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_SaveRegionCode (HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_ReadFirstBoot (HUINT8 *pucData, HUINT32 ulSize );
HIR_ERR CAS_IR_NVR_SaveFirstBoot (HUINT8 *pucData, HUINT32 ulSize );
#endif
#endif
