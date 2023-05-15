/*
 * Copyright (C) 2008 HUMAX Co., Ltd. All rights reserved.
 */

#ifndef __DRV_SMART_H__
#define __DRV_SMART_H__

#include "htype.h"
#include "drv_err.h"

#define ATA_SERIAL_NUM_LEN			(20)
#define ATA_FIRMWARE_NUM_LEN		(8)
#define ATA_MODEL_NUM_LEN			(40)

typedef struct
{
	HUINT8	ucModelName[ATA_MODEL_NUM_LEN+1];
	HUINT8	ucFirmwareVer[ATA_FIRMWARE_NUM_LEN+1];
	HUINT8	ucSerialNo[ATA_SERIAL_NUM_LEN+1];
	HUINT32	ulSizeMB;
}DRV_SMART_SATA_INFO_t;

typedef enum {
	ATA_FIRST_HDD_PORT_NUM = 1,
#if defined(CONFIG_HDD_SATA_2)		
	ATA_SECOND_HDD_PORT_NUM,
#endif	
	ATA_HDD_PORT_NUM_MAX
} ATA_HDD_PORT_NUM_E;

typedef enum {
	ATA_IDENTIFY_DEVICE=0,
	ATA_SMART_READ_ATTR_VALUE,
	ATA_SMART_READ_ATTR_THRE_VALUE,
	ATA_SMART_ENABLE,
	ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE,
	ATA_SMART_STANDBY_IMMEDIATE,
	ATA_CMD_MAX=255
} AtaCmd;

struct SmartAttrEntry {
    unsigned char id;
    unsigned short flag;
    unsigned char value;
    /* there is the worst value. but we didn't use it */
    unsigned char vendor[8];
} __attribute__ ((packed)); /* specify the member's type at struct or union */

struct SmartAttrThreEntry {
    unsigned char id;
    unsigned char thres;
    unsigned char vendor[10];
} __attribute__ ((packed)); /* specify the member's type at struct or union */

struct HddInfo {
    char SerialNum[21];
    char FWVersion[9];
    char ModelName[41];
    unsigned int NumMaxLba; /** Total Number of user addressable sectors **/
};

typedef enum di_dst_test_msg
{
	DI_SMART_MSG_DST_START = 100,
	DI_SMART_MSG_CHECK_STATUS,
	DI_SMART_MSG_COMPLETE_CALLBACK,
	DI_SMART_MSG_ERROR,	
} DI_DST_TEST_MSG;

typedef struct {
	int				port_num;
	DI_DST_TEST_MSG dstMsg;
} SMART_DST_TEST_MSG_T;

typedef struct
{
	HINT8	ucCurrentTemp;
	HINT8	ucMaxTemp;
}DRV_SMART_READ_ATTR_t;

typedef void	(*pfnDRV_HDD_DSTNotify)(HUINT32 dstResult, int progressStatus);

DRV_Error DRV_SMART_Init(int portnum, char *devname);
DRV_Error DRV_SMART_Uninit(int portnum);
DRV_Error DRV_SMART_Read_Attr_Temp(int portnum, char *Temp);
DRV_Error DRV_SMART_Read_Attr(int portnum, DRV_SMART_READ_ATTR_t *pstAttrInfo);
DRV_Error DRV_SMART_IdentifyDevice(int portnum, DRV_SMART_SATA_INFO_t *pSTAInfo);
DRV_Error DRV_SMART_ATA_OP_FlushCache(int portnum);
DRV_Error DRV_SMART_ATA_OP_FlushBufferCache(int portnum);
DRV_Error DRV_SMART_ATA_OP_StandbyImmediate(int portnum);
DRV_Error DRV_SMART_ATA_OP_Reset(int portnum);
DRV_Error DRV_SMART_ATA_SpinUp(int portnum);
DRV_Error DRV_SMART_ATA_SpinDown(int portnum);
DRV_Error DRV_SMART_DstInit(void);
void DRV_SMART_DstStart(int portnum);
void DRV_SMART_RegisterDstCallback(pfnDRV_HDD_DSTNotify pfnNotify);

#endif
