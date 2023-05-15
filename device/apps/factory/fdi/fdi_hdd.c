/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_hdd.h"

#include "drv_err.h"
#include "drv_smart.h"
#include "di_hdd.h"
#include "di_fs.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define HDD_DEVICE_FILE "/dev/sda1"
#define FDI_SMART_BUF_SIZE	516
#define FDI_SMART_CMD			(0x031F)

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
BOOL g_bHddInit;
int g_nHddFd;
char device_name[10];

static FDI_HDD_CALLBACK	s_pfFdiFormat_NotifyCb[FDI_HDD_EVENT_MAX];

static void INTRN_FDI_HDD_DeleteSpace(char *str)
{
	int len;

	if(str == NULL)
	{
		return;
	}

	len = strlen(str);
	while(len--)
	{
		if(str[len] != ' ')
		{
			str[len+1] = '\0';
			break;
		}
	}
	return;
}

static FDI_ERR_CODE INTRN_FDI_HDD_EventCallBack(DI_HDD_Event_t etHddEventType, void *pucHddInfo)
{
	hdd_info_t	*info = pucHddInfo;

	memset(device_name, 0, sizeof(device_name));
	if( info == NULL )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[INTRN_FDI_HDD_EventCallBack] : parm is NULL!!\n"));
		return FDI_ERR;
	}

	strcpy(device_name, "/dev/");
	memcpy(&device_name[5], info->devName, 3);
	FDI_PRINT(FDI_PRT_DBGINFO, ("[INTRN_FDI_HDD_EventCallBack] : event %d, info %s \n", etHddEventType, device_name));

	g_bHddInit = TRUE;

	return FDI_NO_ERR;
}

static void INTRN_FDI_HDD_FormatProcessCallBack(HUINT32 ulEvent, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	FDI_HDD_EVENT_e eEvent;

	switch(ulEvent)
	{
		case DI_FS_EVENT_FORMAT_DONE:
			eEvent = FDI_HDD_EVENT_DONE;
			break;
		case DI_FS_EVENT_FORMAT_FAIL:
			eEvent = FDI_HDD_EVENT_FAIL;
			break;
		default:
			return;

	}

	if(s_pfFdiFormat_NotifyCb[eEvent] != NULL)
	{
		(*s_pfFdiFormat_NotifyCb[eEvent])();
	}
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_GetName(int portnum, char *pDevName)
{
	unsigned int len = 0;

	len = strlen(&device_name[0]);

	if((len == 0)||(pDevName == NULL))
		return FDI_ERR;

	strncpy(pDevName, &device_name[0], 10);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Open()
{
#if 0
	g_nHddFd = open(HDD_DEVICE_FILE, O_RDWR | O_NONBLOCK);
	if(g_nHddFd < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Init] : open Error!!\n"));
		return FDI_ERR;
	}
	g_bHddInit = TRUE;
#endif

	DI_HDD_RegisterCallback( (pfnDI_HDD_NotifyFUNC)INTRN_FDI_HDD_EventCallBack );

	DI_HDD_PWR_Ctrl(DI_HDD_PWR_ON);

	DI_FS_RegisterFormatProgressCallback((DI_FS_Event_Callback_Func)INTRN_FDI_HDD_FormatProcessCallBack);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_GetInfo(FDI_HDD_INFO_t *ptHddInfo)
{
	DI_ERR_CODE	diErr = DI_ERR_OK;
	int iModelNameSize=0;
	DI_HDD_INFO_t	tHddInfo;

	if (g_bHddInit  != TRUE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : HDD is not initialized yet!!\n"));
		return FDI_ERR_WAIT;
	}

	diErr = DI_HDD_GetHddInfo(ATA_FIRST_HDD_PORT_NUM, &tHddInfo);
	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : DI_HDD_GetHddInfo error (%d)!!\n",diErr));
		return FDI_ERR;
	}
#if 0
//	INTRN_FDI_HDD_DeleteSpace(tHddInfo.ucModelName);
//	strncpy(ptHddInfo->aucModelName, tHddInfo.ucModelName, FDI_HDD_MODEL_NAME_SIZE);

	/* ASCII 0~9, a~z, A~Z */
	for(iModelNameSize=0; iModelNameSize<FDI_HDD_MODEL_NAME_SIZE; iModelNameSize++)
	{
		if(0x30 <= tHddInfo.ucModelName[iModelNameSize] && tHddInfo.ucModelName[iModelNameSize] <= 0x39)
		{
			continue;
		}
		else if(0x41 <= tHddInfo.ucModelName[iModelNameSize] && tHddInfo.ucModelName[iModelNameSize] <= 0x5A)
		{
			continue;
		}
		else if(0x61 <= tHddInfo.ucModelName[iModelNameSize] && tHddInfo.ucModelName[iModelNameSize] <= 0x7A)
		{
			continue;
		}
		else
		{
			break;
		}
	}
	strncpy(ptHddInfo->aucModelName, tHddInfo.ucModelName, iModelNameSize);
#endif
	strncpy(ptHddInfo->aucModelName, tHddInfo.aucModelName, FDI_HDD_MODEL_NAME_SIZE);
	strncpy(ptHddInfo->aucSerialNo, tHddInfo.aucSerialNo, FDI_HDD_SERIAL_NO_SIZE);
	strncpy(ptHddInfo->aucFirmwareVer, tHddInfo.aucFirmwareVer, FDI_HDD_FIRMWARE_VER_SIZE);
	ptHddInfo->ulSizeMB = tHddInfo.ulSizeMB;
	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Write(unsigned long unOffset, unsigned char *pucData, unsigned long *pulSize)
{
	int nWriteSize;

	if( g_bHddInit == FALSE )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : HDD is not initialized!!\n"));
		return FDI_ERR;
	}

	if( lseek(g_nHddFd, unOffset, SEEK_SET) < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : lseek Error!!\n"));
		return FDI_ERR;
	}

	nWriteSize = write(g_nHddFd, pucData, *pulSize);
	if( nWriteSize < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : write Error!!\n"));
		return FDI_ERR;
	}
	*pulSize = nWriteSize;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Read(unsigned long unOffset, unsigned char *pucData, unsigned long *pulSize)
{
	int nReadSize;

	if( g_bHddInit == FALSE )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : HDD is not initialized!!\n"));
		return FDI_ERR;
	}

	if( lseek(g_nHddFd, unOffset, SEEK_SET) < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Read] : lseek Error!!\n"));
		return FDI_ERR;
	}

	nReadSize = read(g_nHddFd, pucData, *pulSize);
	if( nReadSize < 0 )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Read] : read Error!!\n"));
		return FDI_ERR;
	}
	*pulSize = nReadSize;

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Close()
{
	close(g_nHddFd);
	g_bHddInit = FALSE;

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_GetStatus(BOOL *bConnect)
{
	*bConnect = g_bHddInit;
	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_GetDeviceName(unsigned char *pucName)
{
	if( g_bHddInit == FALSE )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : HDD is not initialized!!\n"));
		return FDI_ERR;
	}

	sprintf(pucName,"%s",device_name);
	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_DSTOpen(unsigned char *pucName, int *pHddFd)
{
	*pHddFd=open(pucName, O_RDONLY | O_NONBLOCK);
	if(*pHddFd < 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_Write] : HDD is not initialized!!\n"));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_DSTClose(unsigned char *pucName)
{
	close(pucName);
	return FDI_NO_ERR;
}

int FDI_HDD_AtaCmd(int fd, unsigned char *buf, FDI_ATA_CMD eCmd)
{
    unsigned char *pbuf = buf;
    /* Read Identify Device info from HDD as used by S.M.A.R.T
       Check 82th-byte whether S.M.A.R.T Set Support or not */

    if(eCmd >= FDI_ATA_CMD_MAX)
    {
         FDI_PRINT(FDI_PRT_ERROR, ("[%d]%s No Command \n",__LINE__,__func__));
        return 1;
    }

    /* 0 = ATA Command
    * 1 = Sector Count
    * 2 = Features
    * 3 = Sector Number
    * 4 = CylinderLow
    * 5 = CylinderHigh
    * 6 = Device/Head */

    switch (eCmd)
    {
        case FDI_ATA_IDENTIFY_DEVICE:
             pbuf[0] = 0xec;
             pbuf[1] = 0;
             pbuf[2] = 0;
             pbuf[3] = 1;
             pbuf[4] = 0;
             pbuf[5] = 0;
             break;
        case FDI_ATA_SMART_READ_ATTR_VALUE:
             pbuf[0] = 0xb0;
             pbuf[1] = pbuf[3] = 1;
             pbuf[2] = 0xd0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case FDI_ATA_SMART_READ_ATTR_THRE_VALUE:
             pbuf[0] = 0xb0;
             pbuf[1] = pbuf[3] = 1;
             pbuf[2] = 0xd1;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case FDI_ATA_SMART_ENABLE:
             pbuf[0] = 0xb0;
             pbuf[1] = 0;
             pbuf[2] = 0xd8;
             pbuf[3] = 0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        case FDI_ATA_SMART_EXECUTE_OFFLINE_IMMEDIATE:
             pbuf[0] = 0xb0;
             pbuf[2] = 0xd4;
             pbuf[3] = 0;
             pbuf[4] = 0x4f;
             pbuf[5] = 0xc2;
             break;
        default:
             break;
    }

    if(ioctl(fd,FDI_SMART_CMD,pbuf)!= 0)
    {
         FDI_PRINT(FDI_PRT_ERROR, ("[%d]%s ioctl error!!! \n",__LINE__,__func__));
        return 1;
    }

    if((pbuf[4] == 0xf4) && (pbuf[5] == 0x2c))
    {
         FDI_PRINT(FDI_PRT_ERROR, ("[%d]%s read attribute data failed\n",__LINE__,__func__));
        return 1;
    }

    return 0;
}

int FDI_HDD_DstStatus(int fd)
{
    int ret=0;
    unsigned char buf[FDI_SMART_BUF_SIZE];
    unsigned char status=0;

    memset(buf,0x00,FDI_SMART_BUF_SIZE);
    ret=FDI_HDD_AtaCmd(fd, buf, FDI_ATA_SMART_READ_ATTR_VALUE);
    if(ret < 0)
    {
		FDI_PRINT(FDI_PRT_ERROR, ("[%d]%s Error to read Attr values \n",__LINE__,__func__));
		return -1;
    }
    status = ((buf[367] >> 4) & 0x0F);
    switch(status) {
        case 15:
       	    break;
        case 0:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Complete No Error\n"));
		    break;
        case 3:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Fatal error\n"));
		    break;
        case 4:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Test element failed\n"));
		    break;
        case 5:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Electrical element failed\n"));
		    break;
        case 6:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Servo(seek) element failed\n"));
		    break;
        case 7:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Read element failed\n"));
		    break;
        case 8:
		    FDI_PRINT(FDI_PRT_ERROR, ("Diagnostic Test : Handling Damage\n"));
		    break;
        default:
		    status = -1;
		    break;
    }
    return status;
}



/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_GetPartitonInfo(int portnum, FDI_HDD_INFO_t *ptHddInfo)
{
	DRV_Error eDrvErr;
	int iModelNameSize = 0;

	DI_ERR_CODE	diErr = 0;
	DRV_Error drvErr;
	DI_FS_PartitionList_t tmpPartList;
	DI_FileSystemType_t eFsType;
	unsigned char ucmntDir[10];

	unsigned int index = 0;
	HUINT64 totalKbSize = 0, usedKbSize = 0;
	HBOOL writable;
	int cnt;
	HUINT64 moduKbSize = 0;

	DI_FS_StorDevIdx_t	selectDevID = 0;
	DI_HDD_INFO_t	tHddInfo;

	if (g_bHddInit  != TRUE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : HDD is not initialized yet!!\n"));
		return FDI_ERR_WAIT;
	}

	diErr = DI_HDD_GetHddInfo(portnum, &tHddInfo);
	if(diErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : DI_HDD_GetHddInfo error (%d)!!\n",diErr));
		return FDI_ERR;
	}

	/* ASCII 0~9, a~z, A~Z */
	for(iModelNameSize = 0; iModelNameSize<FDI_HDD_MODEL_NAME_SIZE; iModelNameSize++)
	{
		if(0x30 <= tHddInfo.aucModelName[iModelNameSize] && tHddInfo.aucModelName[iModelNameSize] <= 0x39)
		{
			continue;
		}
		else if(0x41 <= tHddInfo.aucModelName[iModelNameSize] && tHddInfo.aucModelName[iModelNameSize] <= 0x5A)
		{
			continue;
		}
		else if(0x61 <= tHddInfo.aucModelName[iModelNameSize] && tHddInfo.aucModelName[iModelNameSize] <= 0x7A)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	memset(&tmpPartList,0,sizeof(DI_FS_PartitionList_t));

	if(0x61 <= device_name [7] <= 0x69)
	{
		selectDevID = (device_name [7] - 0x61);
	}


	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : selectDevID(%d)\n", selectDevID));

	diErr = DI_FS_GetPartitionFromDeviceMbr (selectDevID,&tmpPartList);
	if( diErr != DI_ERR_OK )
	{

		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : DI_FS_GetPartitionFromDeviceMbr fail (%d)\n", diErr));
		return FDI_ERR;
	}

	ptHddInfo->ulNumPartitions = tmpPartList.ulNumPartition;
	for(cnt=0; cnt < ptHddInfo->ulNumPartitions; cnt++)
	{
		diErr = DI_FS_GetFsType(selectDevID, cnt, &eFsType);
		if(diErr == DI_ERR_OK
			&& ((eFsType == DI_FS_EXT3_FILESYSTEM) || (eFsType == DI_FS_EXT4_FILESYSTEM))
			)
		{
			diErr = DI_FS_GetMountedPartitionStatus(selectDevID,cnt,eFsType,NULL,NULL,NULL);
			if(diErr == DI_ERR_ERROR)
			{
				memset(ucmntDir, 0x0, 10);
				snprintf(ucmntDir,10,"/mnt/hd%d", cnt+1);
				diErr = DI_FS_MountPartition(selectDevID,cnt, ucmntDir, eFsType);
				if(diErr != 0)
				{
					FDI_PRINT(FDI_PRT_ERROR, ("[FDI_HDD_GetInfo] : mount fail (%d:%d)\n", portnum-1, cnt));
				}
			}
		}
	}
	for(index = 0; index < tmpPartList.ulNumPartition; index++)
	{
		if((tmpPartList.astPartition[index].eFsType == DI_FS_EXT3_FILESYSTEM) || (tmpPartList.astPartition[index].eFsType == DI_FS_EXT4_FILESYSTEM))
		{
			diErr = DI_FS_GetMountedPartitionStatus(selectDevID, index, tmpPartList.astPartition[index].eFsType, &totalKbSize, &usedKbSize, &writable);
			if(index < FDI_HDD_MAX_PARTITION_NUMBER)
			{
				ptHddInfo->ullPartitionSize[index] = totalKbSize;
			}
			if( diErr != DI_ERR_OK )
			{
				break;
			}

			if(totalKbSize > 0)
			{
				moduKbSize += totalKbSize;
			}
		}
	}

	strncpy(ptHddInfo->aucModelName, tHddInfo.aucModelName, FDI_HDD_MODEL_NAME_SIZE);
	strncpy(ptHddInfo->aucSerialNo, tHddInfo.aucSerialNo, FDI_HDD_SERIAL_NO_SIZE);
	strncpy(ptHddInfo->aucFirmwareVer, tHddInfo.aucFirmwareVer, FDI_HDD_FIRMWARE_VER_SIZE);

	ptHddInfo->ulSizeMB = tHddInfo.ulSizeMB;

	return FDI_NO_ERR;
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_SetParking(int portnum, unsigned char ParkingOn)
{
	DRV_Error eDrvErr = 0;

	if (ParkingOn)
		eDrvErr = DRV_SMART_ATA_SpinDown(portnum);
	else
		eDrvErr = DRV_SMART_ATA_SpinUp(portnum);

	if( eDrvErr != DRV_OK )
	{
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Encrypted_Open(int portnum, int partIdx)
{
#if defined(CONFIG_CRYPTSETUP)
	DI_ERR_CODE errCode;
	char password[32] = { 0, };
	char input[256] = { 0, };
	int devIdx = 0;

	if (g_bHddInit != TRUE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] HDD is not initialized yet!!\n", __func__));

		return FDI_ERR_WAIT;
	}

	errCode = DI_HDD_GetEncryptionKey(portnum, password);
	if (errCode != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_HDD_GetEncryptionKey error(0x%x)!\n", __func__, errCode));

		return FDI_ERR;
	}

	snprintf(input, sizeof(input) - 1, "encrypted-hd%.2d", partIdx);

	/* Get device index */
	if ((device_name[7] >= 0x61) && (device_name[7] <= 0x7A))
	{
		devIdx = device_name[7] - 0x61;
	}
	else if ((device_name[7] >= 0x41) && (device_name[7] <= 0x5A))
	{
		devIdx = device_name[7] - 0x41;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Device name error(%s)!\n", __func__, &device_name[0]));

		return FDI_ERR;
	}

	errCode = DI_FS_ENCR_Open(devIdx, partIdx, input, password);
	if (errCode != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_ENCR_Open error(0x%x)!\n", __func__, errCode));

		return FDI_ERR;
	}
#endif

	return FDI_NO_ERR;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_HDD_Encrypted_Close(int portnum, int partIdx)
{
#if defined(CONFIG_CRYPTSETUP)
	DI_ERR_CODE errCode;
	int devIdx = 0;

	if (g_bHddInit != TRUE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] HDD is not initialized yet!!\n", __func__));

		return FDI_ERR_WAIT;
	}

	/* Get device index */
	if ((device_name[7] >= 0x61) && (device_name[7] <= 0x7A))
	{
		devIdx = device_name[7] - 0x61;
	}
	else if ((device_name[7] >= 0x41) && (device_name[7] <= 0x5A))
	{
		devIdx = device_name[7] - 0x41;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Device name error(%s)!\n", __func__, &device_name[0]));

		return FDI_ERR;
	}

	errCode = DI_FS_ENCR_Close(devIdx, partIdx);
	if (errCode != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_ENCR_Close error(0x%x)!\n", __func__, errCode));

		return FDI_ERR;
	}
#endif

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_Format(HUINT32 portnum, HUINT32 partNum, HUINT64 *partSize,FDI_FS type )
{
	DI_FS_StorDevIdx_t eDevIdx;
	DI_FS_PartitionList_t PartitionList;
	DI_FileSystemType_t eFsType;
	HUINT32 ucIdx;
	DI_ERR_CODE errorCode;
	DI_FS_FormatList_t FormatList;
	DI_FS_FormatItem_t stFsPartitionFormatItem;

	if(g_bHddInit != TRUE)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] HDD is not initialized yet!!\n", __func__));
		return FDI_ERR_WAIT;
	}
	if(partNum == 0)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] partNum is 0!!\n", __func__));
		return FDI_ERR_WAIT;
	}

	if (strcmp(device_name, "/dev/sda") == 0)
	{
		eDevIdx = eDI_FS_DEV_sda;
	}
	else if (strcmp(device_name, "/dev/sdb") == 0)
	{
		eDevIdx = eDI_FS_DEV_sdb;
	}
	else if (strcmp(device_name, "/dev/sdc") == 0)
	{
		eDevIdx = eDI_FS_DEV_sdc;
	}
	else if (strcmp(device_name, "/dev/sdd") == 0)
	{
		eDevIdx = eDI_FS_DEV_sdd;
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Please Check HDD [%s]!\n", __func__,device_name));
		return FDI_ERR;;
	}
	switch(type)
	{
		case FDI_FS_EXT2_FILESYSTEM:
			eFsType = DI_FS_EXT2_FILESYSTEM;
			break;
		case FDI_FS_EXT3_FILESYSTEM:
			eFsType = DI_FS_EXT3_FILESYSTEM;
			break;
		case FDI_FS_EXT4_FILESYSTEM:
			eFsType = DI_FS_EXT4_FILESYSTEM;
			break;
		case FDI_FS_XFS_FILESYSTEM:
			eFsType = DI_FS_XFS_FILESYSTEM;
			break;
		case FDI_FS_FAT16_FILESYSTEM:
			eFsType = DI_FS_FAT16_FILESYSTEM;
			break;
		case FDI_FS_FAT32_FILESYSTEM:
			eFsType = DI_FS_FAT32_FILESYSTEM;
			break;
		case FDI_FS_NTFS_FILESYSTEM:
			eFsType = DI_FS_NTFS_FILESYSTEM;
			break;
		case FDI_FS_MSDOS_FILESYSTEM:
			eFsType = DI_FS_MSDOS_FILESYSTEM;
			break;
		case FDI_FS_EXFAT_FILESYSTEM:
			eFsType = DI_FS_EXFAT_FILESYSTEM;
			break;
		default:
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] Please Check HDD type [%d]!\n", __func__,type));
			return FDI_ERR;
	}

	DI_FS_GetPartitionFromDeviceMbr(eDevIdx, &PartitionList);

	/* Umount */
	for (ucIdx=0; ucIdx< PartitionList.ulNumPartition; ucIdx++)
	{
		DI_FS_UnmountPartition(eDevIdx, ucIdx, eFsType);
	}

	/* Delete Partition */
	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] 1. Deleting Partitions......!!\n", __func__));
	errorCode = DI_FS_OpenDevMbrBuf(eDevIdx);
	errorCode = DI_FS_DeleteAllPartitionInDevMbrBuf(eDevIdx);


	/* Make Parttion */
	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] 2. Making New Partitions......!!\n", __func__));
	PartitionList.ulNumPartition = partNum;

	for (ucIdx=0; ucIdx< partNum; ucIdx++)
	{
		errorCode = DI_FS_SetPartitionInDevMbrBuf(eDevIdx,DI_FS_PRIMARY_PARTITION_1+ucIdx, partSize[ucIdx] ,eFsType );
		if(errorCode)
		{
			FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_SetPartitionInDevMbrBuf Error [%d]!\n", __func__,errorCode));
			return FDI_ERR;
		}

	}
	errorCode = DI_FS_SyncDevMbrBuf(eDevIdx);
	if(errorCode)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_SyncDevMbrBuf Error [%d]!\n", __func__,errorCode));
		return FDI_ERR;
	}
	errorCode = DI_FS_CloseDevMbrBuf(eDevIdx);
	if(errorCode)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_CloseDevMbrBuf Error [%d]!\n", __func__,errorCode));
		return FDI_ERR;
	}

	/* Make FS */
	FDI_PRINT(FDI_PRT_DBGINFO, ("[%s] 3. Making Filesystem.....!!\n", __func__));
	errorCode = DI_FS_InitFormatList(&FormatList);
	if(errorCode)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_InitFormatList Error [%d]!\n", __func__,errorCode));
		return FDI_ERR;
	}

	for (ucIdx=0; ucIdx<partNum; ucIdx++)
	{
		stFsPartitionFormatItem.ePartFsType = eFsType;
		stFsPartitionFormatItem.ulOption = DI_FS_EXT_OPTION_DEFAULT;
		stFsPartitionFormatItem.ulPartitionNo = ucIdx;
		stFsPartitionFormatItem.eDevIdx = eDevIdx;
		DI_FS_AddFormatItemToList(&FormatList, &stFsPartitionFormatItem);
	}

	errorCode = DI_FS_FormatLists(&FormatList, 0);
	if(errorCode)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_FormatLists Error [%d]!\n", __func__,errorCode));
		return FDI_ERR;
	}

	errorCode = DI_FS_ClearFormatList(&FormatList);
	if(errorCode)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] DI_FS_ClearFormatList Error [%d]!\n", __func__,errorCode));
		return FDI_ERR;
	}

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_HDD_RegisterCallback(FDI_HDD_EVENT_e eEvent, FDI_HDD_CALLBACK callback)
{
	if(eEvent >=FDI_HDD_EVENT_MAX)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] Event error [%d]!\n", __func__,eEvent));
		return FDI_ERR;
	}

	if(callback == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[%s] callback NULL\n", __func__));
		return FDI_ERR;
	}

	s_pfFdiFormat_NotifyCb[eEvent] = callback;


	return FDI_NO_ERR;
}



