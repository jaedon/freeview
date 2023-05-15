/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   secapi.h\n
	@brief	   파일은 driver/secure_handler/include에 존재하며\n
				수정도 거기서 해주어야 함...octo/include에 존재하는 파일은\n
				단지 copy된 것이므로 수정하지 말것\n

	Description:   \n
	Module: \n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

#ifndef	__CLIENT_SECURE_HANDLER_H__
#define	__CLIENT_SECURE_HANDLER_H__


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <vkernel.h>

#include "di_hdd.h"
#include "di_err.h"
#include "di_usb.h"
#include "di_usb_hid.h"
#include "di_usb_wireless.h"
#include "di_fs.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HINT32 SECAPI_Init(IpcSecMsgType eMsgType);

extern HINT32 SECAPI_Mount(const DI_FS_StorDevIdx_t eDevIdx, const HUINT32 ulPartIdx, const HUINT8 *szDstMountDir, const DI_FileSystemType_t eFsType);
extern HINT32 SECAPI_Umount(const DI_FS_StorDevIdx_t eDevIdx, const HUINT32 ulPartIdx, const DI_FileSystemType_t eFsType);
extern HINT32 SECAPI_Stime(VK_CLOCK_SET_MODE eSetMode, unsigned long ulUnixTime);
extern HINT32 SECAPI_StimeSetTime(unsigned long ulUnixTime);
extern HINT32 SECAPI_StimeSetMode(VK_CLOCK_SET_MODE eSetMode);
extern HINT32 SECAPI_StimeGetMode(VK_CLOCK_SET_MODE *eGetMode);
extern HINT32 SECAPI_Settimeofday(HINT32 iHour, HINT32 iMin);
extern HINT32 SECAPI_Chmod(const HINT8 *pucPath, HUINT32 ulMode);
extern HINT32 SECAPI_Chown(const HINT8 *pucPath);
extern HINT32 SECAPI_Remove(const HINT8 *pucPath);
extern HINT32 SECAPI_Mkdir(const HINT8 *pucPath, HUINT32 ulMode);
extern HINT32 SECAPI_Mknod(const HINT8 *pucPath, HUINT32 ulMode, HUINT32 ulDev);
extern HINT32 SECAPI_SysCmd(HUINT8 ucCmdNum, ...);
extern HINT32 SECAPI_Ioctl(HINT32 nId, HINT32 nRequest, HUINT8 ucCmdNum, ...);
extern HINT32 SECAPI_Stat(const HINT8 *pucPath, struct stat *pstBuf);
extern HINT32 SECAPI_Lstat(const HINT8 *pucPath, struct stat *pstBuf);
extern HINT32 SECAPI_Socket(HINT32 nDomain, HINT32 nType, HINT32 nProtocol);
extern HINT32 SECAPI_Select(HINT32 nFd, fd_set *pstReadFds, fd_set *pstWriteFds, fd_set *pstExceptFds, struct timeval *pstTimeout);
extern HINT32 SECAPI_Open(const HINT8 *pucPath, HINT32 nFlags);
extern HINT32 SECAPI_Open2(const HINT8 *pucPath, HINT32 nFlags, mode_t mode);
extern HINT32 SECAPI_Close(HINT32 nFd);
extern HINT32 SECAPI_Lseek(HINT32 fd, off_t pos, HINT32 origin);
extern HINT32 SECAPI_Read(HINT32 fd, HUINT8 *pucBuf, HUINT32  ulSize);
extern HLONG SECAPI_Write(HINT32 nFd, const void *pvBuf, HLONG lCount);
extern HINT32 SECAPI_Rename(const char *pucOld, const char *pucNew);
extern HINT32 SECAPI_Vfork(HUINT8 *pucProcName, HINT32 *pnPid);
extern HINT32 SECAPI_HddPwrCtrl(DI_HDD_PWR pwr);
extern HINT32 SECAPI_Hdd2PwrCtrl(DI_HDD_PWR pwr); // 2nd internal HDD (detachable HDD)만 power control 할때
extern HINT32 SECAPI_UsbPwrCtrl(DI_USB_PWR_t pwr);
extern HINT32 SECAPI_UsbHostConPwrCtrl(DI_USB_PWR_t pwr);
extern HINT32 SECAPI_UsbStop(void);
#if (1)
extern HINT32 SECAPI_FsGetDeviceFileName(DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
extern HINT32 SECAPI_FsGetDeviceFileFullName(DI_FS_StorDevIdx_t eDevIdx, HUINT8 *szFileName);
extern HINT32 SECAPI_FsGetPartitionFileName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName, HUINT8 nSizeOfFileName);
extern HINT32 SECAPI_FsGetPartitionFileFullName(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartitionIdx, HUINT8 *szFileName);
extern HINT32 SECAPI_FsIsFileExist(HCHAR *pathname);
extern HINT32 SECAPI_FsIsHDDExist(HCHAR *devname);
extern HINT32 SECAPI_FsIsDirExist(HCHAR *dirname);
extern HINT32 SECAPI_FsMkdir(HCHAR *dirname);
extern HINT32 SECAPI_FsGetVolumeUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 bufsize, HUINT32 *uuidsize);
extern HINT32 SECAPI_FsGetVolumeNtfsUuid(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *buf, HUINT32 size);
extern HINT32 SECAPI_FsMountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT8 *szDstMountDir, DI_FileSystemType_t eFsType);
extern HINT32 SECAPI_FsUnmountPartition(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType);
extern HINT32 SECAPI_FsHddDstStart(void);
extern HINT32 SECAPI_FsOpenDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
extern HINT32 SECAPI_FsDeletePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx);
extern HINT32 SECAPI_FsDeleteAllPartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
extern HINT32 SECAPI_FsUpdatePartitionInDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, HUINT32 ulStartUnit, HUINT32 ulEndUnit, DI_FileSystemType_t eFsType);
extern HINT32 SECAPI_FsSyncDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
extern HINT32 SECAPI_FsCloseDevMbrBuf(DI_FS_StorDevIdx_t eDevIdx);
extern HINT32 SECAPI_FsGetPartitionFromDeviceMbr(DI_FS_StorDevIdx_t eDevIdx, DI_FS_PartitionList_t *pstPartList);
extern HINT32 SECAPI_FsEraseAllPartitionInDeviceMbr(DI_FS_StorDevIdx_t eDevIdx);
extern HINT32 SECAPI_FsInitFormatList(DI_FS_FormatList_t *pstList);
extern HINT32 SECAPI_FsAddFormatItemToList(DI_FS_FormatList_t *pstList, DI_FS_FormatItem_t *pstItem);
extern HINT32 SECAPI_FsClearFormatList(DI_FS_FormatList_t *pstList);
extern HINT32 SECAPI_FsFormatLists(DI_FS_FormatList_t *pstFormatList, HUINT32 nonblock);
extern HINT32 SECAPI_FsGetMountedPartitionStatus(DI_FS_StorDevIdx_t eDevIdx, HUINT32 ulPartIdx, DI_FileSystemType_t eFsType, HUINT64 *pullTotalKbSize, HUINT64 *pullUsedKbSize, HBOOL *pbWritable);
extern HINT32 SECAPI_Register_FsHddDstCallback(pfnDI_HDD_DSTNotify pfnDstNotify);
extern HINT32 SECAPI_Register_FsFormatProgressCallback(DI_FS_Event_Callback_Func pfEvtCallback);
#endif
extern HINT32 SECAPI_Regster_HddCallback(pfnDI_HDD_NotifyFUNC pfnNotify);
extern HINT32 SECAPI_Regster_UsbCallback(pfnDI_USB_NotifyFUNC pfnNotify);
extern HINT32 SECAPI_Regster_WlanCallback(pfnDI_USB_NotifyFUNC pfnNotify);
extern HINT32 SECAPI_Regster_HidCallback(const DI_USB_HID_DEVICE eDevices, pfnDI_USB_HID_Notify pfnNotify);


#endif /* !__CLIENT_SECURE_HANDLER_H__ */


