/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_mgr_pal_port.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MGR_PAL_PORT_H__
#define	__CAS_MGR_PAL_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___

#include <pal_sef.h>
#include <pal_video.h>
#include <pal_audio.h>
#include <pal_pipe.h>
#include <_svc_cas_sub_dev_api.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___


/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

/* VIDEO_DeviceEvent_t와 같아야함. */
typedef enum
{
	eCasMgrVideoDevEvent_NONE,
	eCasMgrVideoDevEvent_PARAM_CHANGED,
	eCasMgrVideoDevEvent_FRAME_START,
	eCasMgrVideoDevEvent_UNDERRUN,
	eCasMgrVideoDevEvent_MAX_NUM
} SvcCas_VideoDevEvent_e;

/* AUDIO_DeviceEvent_t와 같아야함. */
typedef enum
{
	eCasMgrAudioDevEvent_NONE,
	eCasMgrAudioDevEvent_UNDERRUN,
	eCasMgrAudioDevEvent_NEWFRAME,
	eCasMgrAudioDevEvent_NEWPARAM,
	eCasMgrAudioDevEvent_CLIP_COMPLETE,	// Audio clip play가 stop 된 경우에 ...
	eCasMgrAudioDevEvent_STARTED,		// Audio clip play가 start 된 경우에 ...
	eCasMgrAudioDevEvent_MAX
} SvcCas_AudioDevEvent_e;

typedef PalSef_RawData_t SvcCas_SiRawData_t;

typedef void (* SvcCas_MgrVideoCb_t)(HUINT32 ulDecoderId, SvcCas_VideoDevEvent_e eVideoEvent);
typedef void (* SvcCas_MgrAudioCb_t)(SvcCas_AudioDevEvent_e eAudioEvent);
typedef void (* SvcCas_PalVideoCb_t)(HUINT32 ulDecoderId, PalVideo_Event_e eVideoEvent);
typedef void (* SvcCas_PalAudioCb_t)(PalAudio_Event_e eAudioEvent);

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
HERROR svc_cas_PalPortScInit(void);
HERROR svc_cas_PalPortScGetSlotId(SciHandle_t hSci, HUINT8 *pulSlotId);
HERROR svc_cas_PalPortScDeactivate(SciHandle_t hSci);
HERROR svc_cas_PalPortScReset(	SciHandle_t hSci,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						SvcCas_DevScmResetType_e eResetType,
						SvcCas_DevSciScParam_t *pstDefaultParam
						);
HERROR svc_cas_PalPortScRegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCARMId, SvcCas_SciCb_t pfnCb, SciHandle_t *hSci);
HERROR svc_cas_PalPortScTransfer(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulEpxectedNumRead);
HERROR svc_cas_PalPortScRead(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);
HERROR svc_cas_PalPortScWrite(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);
HERROR svc_cas_PalPortScSetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam);
HERROR svc_cas_PalPortScGetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam);
HERROR svc_cas_PalPortScNotifyScValidity(SciHandle_t hSci, SvcCas_DevScmFlag_e eMatched);
HERROR svc_cas_PalPortScNegotiatePTS(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead);
HERROR svc_cas_PalPortScGetCardStatus(SciHandle_t hSci, SvcCas_DevSciScStatus_e *peStatus);
HERROR svc_cas_PalPortScGetCardStatusBySlotID(HUINT32 ulSlotId, SvcCas_DevSciScStatus_e *peStatus);
HUINT32 svc_cas_PalPortScGetNumberOfModule(void);
HERROR svc_cas_PalPortScActivate(SciHandle_t hSci);
#endif

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___

#define _________System_____________

HERROR svc_cas_PalPortSysGetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
HERROR svc_cas_PalPortSysSetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
HERROR svc_cas_PalPortSysGetFusingInfo(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE);
HERROR svc_cas_PalPortSysGetFusingInfoExt(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT32* pulOTPField1, HUINT32* pulOTPField2);
#if defined(CONFIG_MW_CAS_CONAX_CPM)
HERROR svc_cas_PalPortSysGetFusingInfoForConax(HUINT8 *pucSCS, HUINT8 *pucJTAG, HUINT8 *pucOTP, HUINT8 *pucCWE, HUINT8 *pucRemux, HUINT8 *pucMarketID);
#endif
HERROR svc_cas_PalPortSysGetChipId(HUINT32 *pulChipId);
HERROR svc_cas_PalPortSysGetChipRevision(HUINT32 *pulChipId);
HERROR svc_cas_PalPortSysGetSystemSerialNumber(HUINT8 *szSerialNoStr, HUINT32 ulSize);

#define _________AV_____________

HERROR svc_cas_PalPortVideoRegisterEventCallback(SvcCas_MgrVideoCb_t pfnCasMgrVideoCb);
HERROR svc_cas_PalPortAudioRegisterEventCallback(SvcCas_MgrAudioCb_t pfnCasMgrAudioCb);
HBOOL svc_cas_PalPortIsVideoRunning(HUINT32 ulDecoderId);

#define _________FS_____________

CASDEV_FS_BOOL svc_cas_PalPortIsDirExist(const char *dirname);
int svc_cas_PalPortMkDir(const char *dirname);
CASDEV_FS_BOOL svc_cas_PalPortIsFileExist(const char *pathname);

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
int		svc_cas_PalPortFsOpen(int pttype, const char *pathname, CASDEV_FS_UINT32 flags, CASDEV_FS_UINT32 *filedes);
int		svc_cas_PalPortFsClose(CASDEV_FS_UINT32 filedes);
int		svc_cas_PalPortFsRead(CASDEV_FS_UINT32 filedes, CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size, CASDEV_FS_UINT32 *sizep);
int		svc_cas_PalPortFsWrite(CASDEV_FS_UINT32 filedes, const CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size, CASDEV_FS_UINT32 *sizep);
int		svc_cas_PalPortFsLSeek(CASDEV_FS_UINT32 filedes, CASDEV_FS_INT64 offset, CASDEV_FS_UINT32 pos, CASDEV_FS_INT64 *offsetp);
int		svc_cas_PalPortFsDelete(int pttype, const char *pathname);
#if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)
int 	svc_cas_PalPortFsGetHddSerialNumber(const char *dev, char **hddsn);
#endif
int 	svc_cas_PalPortFsGetFileInfo(const char *pathname, HUINT32 type, SvcCas_DevFsFileState_t *filestatp);
#endif

#define __________CI___________

HUINT16 svc_cas_PalPortCiGetNumberOfModule(void);
HUINT16 svc_cas_PalPortCiGetMaxNumOfCISlot(void);
#if defined(CONFIG_MW_CI)
SvcCas_DevCiState_e svc_cas_PalPortCiCheckModule (CI_SLOT_ID usSlotId);
HERROR svc_cas_PalPortCiResetModule (CI_SLOT_ID usSlotId);
HERROR svc_cas_PalPortCiSetCamType(CI_SLOT_ID usSlotId, SvcCas_DevCiCamType_e eType);
HINT32 svc_cas_PalPortCiOpen (HUINT8 *device_name);
HUINT8 *svc_cas_PalPortCiGetDeviceName(void);
HINT32 svc_cas_PalPortCiClose (HUINT16 usSlotId);
HERROR	svc_cas_PalPortCiPlugInCi (HUINT32 ulActionId, HINT32 nSlotId);
HERROR svc_cas_PalPortCiGetPath(HUINT32 ulDemuxId, HINT32 *pnCI_Id);
HERROR svc_cas_PalPortCiSetTsPath (CI_SLOT_ID usSlotId, SvcCas_DevCiPath_e path);
HERROR svc_cas_PalPortCiRegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag));

HERROR svc_cas_PalPortCiGetStatusCI(unsigned short usSlotId, SvcCas_DevCiConnectStatue_t *pstCIStatus);

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR svc_cas_PalPortCiGetCurrentPathWay(HUINT32 unSlotId, SvcCas_DevCiPathway_e *peCiPathWay);
HERROR svc_cas_PalPortCiSetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, SvcCas_DevCiPathway_e eCarmCiPathWay);
#endif

HINT32 svc_cas_PalPortCiWrite (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 svc_cas_PalPortCiRead (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 svc_cas_PalPortCiIoctl (HUINT16 usSlotId, HINT32 command, void *parg);
#endif

#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)

HERROR svc_cas_PalPortDscInit(void);
HERROR svc_cas_PalPortDscCloseAll(void);
HERROR svc_cas_PalPortDscResetPid(HUINT32 ulDscIndex, HUINT16 usPid);
HERROR svc_cas_PalPortDscSetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData);
HERROR svc_cas_PalPortDscClearProtectionKey(HUINT32 ulDscIndex);
HERROR svc_cas_PalPortDscSetKey(HUINT32 ulDscIndex, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData);
HERROR svc_cas_PalPortDscSetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize);


HERROR svc_cas_PalPortDscOpen(HUINT32 ulDemuxId, SvcCas_DevDscType_e eDscType, SvcCas_DscDataType_e eDataType, HUINT32 *pulDscIndex);
HERROR svc_cas_PalPortDscClose(HUINT32 ulDscIndex);
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_PalPortDscSetKeyForCiPlus(HUINT32 ulDscIndex, SvcCas_DevDscType_e eDscType, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec);
#endif
HERROR svc_cas_PalPortDscSetPid(HUINT32 ulDscIndex, HUINT16 usPid);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR svc_cas_PalPortDscSetVmxIptvDemuxId(HUINT32 ulDemuxId);
HERROR svc_cas_PalPortDscSetVmxIptvEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo);
#endif

HBOOL svc_cas_PalPortDmxIsSupportScrambleBitChecking(HUINT32 ulDemuxId);
HERROR svc_cas_PalPortDmxCheckScrambleBitByPid(HUINT32 ulDemuxId, SvcCas_DevDmxTsState_e *peState);

#define __________NVRAM___________
HERROR svc_cas_PalPortNVRAMWriteData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HERROR svc_cas_PalPortNVRAMReadData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);


#define __________CRYPT___________
HERROR svc_cas_PalPortSysEncrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize);
HERROR svc_cas_PalPortSysDecrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize);
HERROR svc_cas_PalPortSysEncryptData(SvcCas_Crypt_t *pstSvcCasCrypt);
HERROR svc_cas_PalPortSysDecryptData(SvcCas_Crypt_t *pstSvcCasCrypt);
HERROR svc_cas_PalPortChiperSha1(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest);
HERROR svc_cas_PalPortChiperSha256(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest);
HERROR svc_cas_PalPortChiperHmacSha1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult);
HERROR svc_cas_PalPortSysEncryptAesData(HUINT8 *pucData, HUINT8 *pucEncBuf, HUINT32 ulSize);
HERROR svc_cas_PalPortSysDecryptAesData(HUINT8 *pucData, HUINT8 *pucDecBuf, HUINT32 ulSize);
HUINT8* svc_cas_PalPortSysGetEncryptionKey(void);
#define __________PANEL___________
HERROR svc_cas_PalPortSetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel);

#define __________POWER___________
HERROR svc_cas_PalPortPowerReboot(void);

#define __________FLASH___________
HERROR svc_cas_PalPortNandFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);
HERROR svc_cas_PalPortFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);

#ifdef __cplusplus
}
#endif



#endif /* !__CAS_MGR_PAL_PORT_H__ */
