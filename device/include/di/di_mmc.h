#ifndef __DI_MMC_H__
#define __DI_MMC_H__

/*----------------------------------------------------------------------------------
** Headers
**--------------------------------------------------------------------------------*/
#include "di_err.h"
#include "htype.h"

/*----------------------------------------------------------------------------------
** defines
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** typedef
**--------------------------------------------------------------------------------*/
typedef enum {
	DI_MMC_PWR_ON = 0,
	DI_MMC_PWR_OFF,
	DI_MMC_PWR_MAX
} DI_MMC_PWR_E;

typedef enum {
	DI_MMC_PORT_1 = 1,
	DI_MMC_PORT_MAX
} DI_MMC_PORT_E;

typedef enum {
	DI_MMC_EVT_CONNECTED = 0,
	DI_MMC_EVT_DISCONNECTED,
	DI_MMC_EVT_MAX
} DI_MMC_EVT_E;

typedef enum {
	DI_MMC_SVC_EVT_FORMAT_DONE = 0,
	DI_MMC_SVC_EVT_FORMAT_FAIL,
	DI_MMC_SVC_EVT_MOUNT_DONE,
	DI_MMC_SVC_EVT_MOUNT_FAIL,
	DI_MMC_SVC_EVT_UMOUNT_DONE,
	DI_MMC_SVC_EVT_UMOUNT_FAIL,
	DI_MMC_SVC_EVT_MAX
} DI_MMC_SVC_EVT_E;

typedef enum
{
	DI_MMC_MNTOPT_RDONLY      = 0x0001,
	DI_MMC_MNTOPT_NOSUID      = 0x0002,
	DI_MMC_MNTOPT_NODEV       = 0x0004,
	DI_MMC_MNTOPT_NOEXEC      = 0x0008,
	DI_MMC_MNTOPT_SYNCHRONOUS = 0x0010,
	DI_MMC_MNTOPT_REMOUNT     = 0x0020,
	DI_MMC_MNTOPT_MANDLOCK    = 0x0040,
	DI_MMC_MNTOPT_NOATIME     = 0x0080,
	DI_MMC_MNTOPT_NODIRATIME  = 0x0100,
	DI_MMC_MNTOPT_BIND        = 0x0200,
	DI_MMC_MNTOPT_MOVE        = 0x0400
} DI_MMC_MNTOPT_E;

typedef struct {
	HINT8 		dev_name[16];
	HUINT32		portnum;
} DI_MMC_HP_EVT_T;

typedef struct {
	HUINT32 nPort;
	HUINT32 nMounted;
	HUINT32 uiDiskSize;
	HUINT32 uiAvailableSize;
	HUINT32 uiUsedSize;
	HBOOL   bCardLock;
	HUINT8  ucBusWidths;
	HINT8   szUuid[32];	
	HUINT8	ucPSN[4];	/*Product Serial Number*/
} DI_MMC_INFO_T;

typedef DI_ERR_CODE	(*pfnDI_MMC_CallbackFunc)(DI_MMC_EVT_E evt, void *mmc_info);
typedef DI_ERR_CODE	(*pfnDI_MMC_SvcCallbackFunc)(DI_MMC_SVC_EVT_E evt);


/*----------------------------------------------------------------------------------
** Variables
**--------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
** Function Declaration
**--------------------------------------------------------------------------------*/
DI_ERR_CODE DI_MMC_PowerControl(DI_MMC_PORT_E port, DI_MMC_PWR_E on);
DI_ERR_CODE DI_MMC_Format(DI_MMC_PORT_E port);
DI_ERR_CODE DI_MMC_MountExt(DI_MMC_PORT_E port, HUINT8 *mount_dir, DI_MMC_MNTOPT_E eMntFlags, char *pMntOpts);
DI_ERR_CODE DI_MMC_Umount(DI_MMC_PORT_E port);
DI_ERR_CODE DI_MMC_GetMMCInfo(DI_MMC_PORT_E port, DI_MMC_INFO_T *tMmcInfo);
DI_ERR_CODE DI_MMC_SetVolumeName(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize);
DI_ERR_CODE DI_MMC_GetVolumeName(DI_MMC_PORT_E port, HUINT8 *buf, HUINT32 bufsize);
DI_ERR_CODE DI_MMC_RegisterCallbackFunc(pfnDI_MMC_CallbackFunc pfnMmcCallback);
DI_ERR_CODE DI_MMC_RegisterSvcCallbackFunc(pfnDI_MMC_SvcCallbackFunc pfnMmcSvcCallback);
DI_ERR_CODE DI_MMC_Init(void);

#define DI_MMC_Mount(port, mnt_dir) DI_MMC_MountExt(port, mnt_dir, 0, NULL);

#endif
