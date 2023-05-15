/**
	@file     _svc_cas_sub_dev_api.c
	@brief    _svc_cas_sub_dev_api.c (CAS Service)

	Description: \n
	Module: MW/CAS \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>

#include <svc_cas.h>
#include <_svc_cas_sub_dev_api.h>
#include <_svc_cas_mgr_pal_port.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___

/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

/********************************************************************
* Static Functions
* Naming rule : "local"_"Module Prefix" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

/********************************************************************
* Module Internal Functions
* Naming rule : "" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

/********************************************************************
* Global Public Functions
* Naming rule : "Module Prefix" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

#define _________System_____________

HERROR svc_cas_DevSysGetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	return svc_cas_PalPortSysGetField(eType, ulOffset, (void*)pData, ulSize);
}

HERROR svc_cas_DevSysSetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize)
{
	return svc_cas_PalPortSysSetField(eType, ulOffset, (void*)pData, ulSize);
}

HERROR svc_cas_DevPortSysGetChipId(HUINT32 *pulChipId)
{
	return svc_cas_PalPortSysGetChipId(pulChipId);
}

HERROR svc_cas_DevPortSysGetChipRevision(HUINT32 *pulChipId)
{
	return svc_cas_PalPortSysGetChipRevision(pulChipId);
}

#define _________AV_____________

HERROR svc_cas_DevVideoIsRunning(HUINT32 ulDecoderId, HBOOL *pbState)
{
	*pbState = svc_cas_PalPortIsVideoRunning(ulDecoderId);

	return ERR_OK;
}

#define __________FS___________

CASDEV_FS_BOOL svc_cas_DevFsIsDirExist(const char *dirname)
{
	return svc_cas_PalPortIsDirExist(dirname);
}

int svc_cas_DevFsMkdir(const char *dirname)
{
	return svc_cas_PalPortMkDir(dirname);
}

CASDEV_FS_BOOL svc_cas_DevFsIsFileExist(const char *pathname)
{
	return svc_cas_PalPortIsFileExist(pathname);
}

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
HERROR svc_cas_DevScInit(void)
{
	return svc_cas_PalPortScInit();
}
HERROR svc_cas_DevScGetSlotId(SciHandle_t hSci, HUINT8 *pulSlotId)
{
	return svc_cas_PalPortScGetSlotId(hSci, pulSlotId);
}
HERROR svc_cas_DevScDeactivate(SciHandle_t hSci)
{
	return svc_cas_PalPortScDeactivate(hSci);
}
HERROR svc_cas_DevScReset(	SciHandle_t hSci,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						SvcCas_DevScmResetType_e eResetType,
						SvcCas_DevSciScParam_t *pstDefaultParam
						)
{
	return svc_cas_PalPortScReset(hSci, pucAtr, pulLen, eResetType, pstDefaultParam);
}
HERROR svc_cas_DevScRegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCARMId, SvcCas_SciCb_t pfnCb, SciHandle_t *hSci)
{
	return svc_cas_PalPortScRegisterCbFunc(ulSlotId, eCARMId, pfnCb, hSci);
}
HERROR svc_cas_DevScTransfer(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulEpxectedNumRead)
{
	return svc_cas_PalPortScTransfer(hSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead, ulEpxectedNumRead);
}
HERROR svc_cas_DevScRead(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	return svc_cas_PalPortScRead(hSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead);
}
HERROR svc_cas_DevScWrite(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead)
{
	return svc_cas_PalPortScWrite(hSci, pucWriteBuf, ulNumWrite, pucReadBuf, pulNumRead);
}
HERROR svc_cas_DevScSetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam)
{
	return svc_cas_PalPortScSetParams(hSci, pstParam);
}
HERROR svc_cas_DevScGetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam)
{
	return svc_cas_PalPortScGetParams(hSci, pstParam);
}
HERROR svc_cas_DevScNotifyScValidity(SciHandle_t hSci, SvcCas_DevScmFlag_e eMatched)
{
	return svc_cas_PalPortScNotifyScValidity(hSci, eMatched);
}
HERROR svc_cas_DevScNegotiatePTS(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead)
{
	return svc_cas_PalPortScNegotiatePTS(hSci, pucWriteBuf, ucNumToWrite, pucReadBuf, pucNumRead);

}
HERROR svc_cas_DevScGetCardStatus(SciHandle_t hSci, SvcCas_DevSciScStatus_e *peStatus)
{
	return svc_cas_PalPortScGetCardStatus(hSci, peStatus);

}

HERROR svc_cas_DevScGetCardStatusBySlotID(HUINT32 ulSlotId, SvcCas_DevSciScStatus_e *peStatus)
{
	return svc_cas_PalPortScGetCardStatusBySlotID(ulSlotId, peStatus);
}

HUINT32 svc_cas_DevScGetNumberOfModule(void)
{
	return svc_cas_PalPortScGetNumberOfModule();
}
HERROR svc_cas_DevScActivate(SciHandle_t hSci)
{
	return svc_cas_PalPortScActivate(hSci);
}
#endif

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
#define __________FileSystem____________
/* FS */
int		svc_cas_DevFsOpen(int pttype, const char *pathname, CASDEV_FS_UINT32 flags, CASDEV_FS_UINT32 *filedes)
{
	return svc_cas_PalPortFsOpen(pttype, pathname, flags, filedes);
}

int		svc_cas_DevFsClose(CASDEV_FS_UINT32 filedes)
{
	return svc_cas_PalPortFsClose(filedes);
}


int		svc_cas_DevFsRead(CASDEV_FS_UINT32 filedes, CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,   CASDEV_FS_UINT32 *sizep)
{
	return svc_cas_PalPortFsRead(filedes, buf, size, sizep);
}


int		svc_cas_DevFsWrite(CASDEV_FS_UINT32 filedes, const CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,    CASDEV_FS_UINT32 *sizep)
{
	return svc_cas_PalPortFsWrite(filedes, buf,  size, sizep);
}

int		svc_cas_DevFsLSeek(CASDEV_FS_UINT32 filedes, CASDEV_FS_INT64 offset, CASDEV_FS_UINT32 pos,     CASDEV_FS_INT64 *offsetp)
{
	return svc_cas_PalPortFsLSeek(filedes, offset, pos, offsetp);
}


int		svc_cas_DevFsDelete(int pttype, const char *pathname)
{
	return svc_cas_PalPortFsDelete(pttype, pathname);
}





#if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)
int 	svc_cas_DevFsGetHddSerialNumber(const char *dev, char **hddsn)
{
	return svc_cas_PalPortFsGetHddSerialNumber(dev, hddsn);
}
#endif // #if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)



int 	svc_cas_DevFsGetFileInfo(const char *pathname, HUINT32 type, SvcCas_DevFsFileState_t *filestatp)
{
	return svc_cas_PalPortFsGetFileInfo(pathname, type, filestatp);
}
#endif


#define __________CI___________

#if defined(CONFIG_MW_CI)
HUINT16 svc_cas_DevCiGetNumberOfModule(void)
{
	return svc_cas_PalPortCiGetNumberOfModule();
}

SvcCas_DevCiState_e CARM_CI_CheckModule (CI_SLOT_ID usSlotId) /* 캠 삽입 유무를 리턴함 */
{
	return svc_cas_PalPortCiCheckModule(usSlotId);
}

HERROR svc_cas_DevCiResetModule (CI_SLOT_ID usSlotId) /* 캠 모듈을 리셋함 */
{
	return svc_cas_PalPortCiResetModule(usSlotId);
}

HERROR svc_cas_DevCiSetCamType(CI_SLOT_ID usSlotId, SvcCas_DevCiCamType_e eType)
{
	return svc_cas_PalPortCiSetCamType(usSlotId, eType);
}

HINT32 svc_cas_DevCiOpen (HUINT8 *device_name)
{
	return svc_cas_PalPortCiOpen(device_name);
}

HUINT8 *svc_cas_DevCiGetDeviceName(void)
{
	return svc_cas_PalPortCiGetDeviceName();
}

HINT32 svc_cas_DevCiClose (HUINT16 usSlotId)
{
	return svc_cas_PalPortCiClose(usSlotId);
}

HERROR	svc_cas_DevCiPlugInCi (HUINT32 ulActionId, HINT32 nSlotId)
{
	return svc_cas_PalPortCiPlugInCi (ulActionId, nSlotId);
}

HERROR svc_cas_DevCiSetTsPath (CI_SLOT_ID usSlotId, SvcCas_DevCiPath_e path) /* bypass 또는 pass-thru를 지정하기 위한 함수 */
{
	return svc_cas_PalPortCiSetTsPath (usSlotId, path);
}

HERROR svc_cas_DevCiRegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag)) /* 캠의 상태에 따라 CI 드라이버로부터 호출되기를 원하는 콜백을 등록하기 위한 함수 */
{
	return svc_cas_PalPortCiRegisterCallback(CiCallbackFunc);
}

HERROR svc_cas_DevCiGetStatusCI(unsigned short usSlotId, SvcCas_DevCiConnectStatue_t *pstCIStatus)
{
	return svc_cas_PalPortCiGetStatusCI(usSlotId, pstCIStatus);
}

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR svc_cas_DevCiGetCurrentPathWay(HUINT32 unSlotId, SvcCas_DevCiPathway_e *peCiPathWay)
{
	return svc_cas_PalPortCiGetCurrentPathWay(unSlotId, peCiPathWay);
}

HERROR svc_cas_DevCiSetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, SvcCas_DevCiPathway_e eCarmCiPathWay)
{
	return svc_cas_PalPortCiSetExtendedTsPath(unTunerId, unSlotId, unDemuxId, eCarmCiPathWay);
}
#endif

HINT32 svc_cas_DevCiWrite (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
	return svc_cas_PalPortCiWrite (usSlotId, len, pbytes);
}

HINT32 svc_cas_DevCiRead (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes)
{
	return svc_cas_PalPortCiRead (usSlotId, len, pbytes);
}

HINT32 svc_cas_DevCiIoctl (HUINT16 usSlotId, HINT32 command, void *parg)
{
	return svc_cas_PalPortCiIoctl (usSlotId, command, parg);
}

#endif


#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)

HERROR svc_cas_DevDscInit(void)
{
	return svc_cas_PalPortDscInit();
}


HERROR svc_cas_DevDscCloseAll(void)
{
	return svc_cas_PalPortDscCloseAll();
}


HERROR svc_cas_DevDscResetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	return svc_cas_PalPortDscResetPid(ulDscIndex, usPid);
}

HERROR svc_cas_DevDscSetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData)
{
	return svc_cas_PalPortDscSetProtectionKey(ulDscIndex, pucKeyData);
}

HERROR svc_cas_DevDscClearProtectionKey(HUINT32 ulDscIndex)
{
	return svc_cas_PalPortDscClearProtectionKey(ulDscIndex);
}

HERROR svc_cas_DevDscSetKey(HUINT32 ulDscIndex, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData)
{

	return svc_cas_PalPortDscSetKey(ulDscIndex, eParity, pucKeyData);
}


HERROR svc_cas_DevDscSetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize)
{
	return svc_cas_PalPortDscSetAesKey(ulDscIndex, pucKeyData, ulKeySize);
}

HERROR svc_cas_DevDscOpen(HUINT32 ulDmxId, SvcCas_DevDscType_e eDscType, SvcCas_DscDataType_e eDataType, HUINT32 *pulDscIndex)
{
	return svc_cas_PalPortDscOpen(ulDmxId, eDscType, eDataType, pulDscIndex);
}

HERROR svc_cas_DevDscClose(HUINT32 ulDscIndex)
{
	return svc_cas_PalPortDscClose(ulDscIndex);
}

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_DevDscSetKeyForCiPlus(HUINT32 ulDscIndex, SvcCas_DevDscType_e eDscType, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec)
{
	return svc_cas_PalPortDscSetKeyForCiPlus(ulDscIndex, eDscType, eParity, pucKeyData, pucInitVec);
}
#endif

HERROR svc_cas_DevDscSetPid(HUINT32 ulDscIndex, HUINT16 usPid)
{
	return svc_cas_PalPortDscSetPid(ulDscIndex, usPid);
}
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR svc_cas_DeDscSetVmxIptvDemuxId(HUINT32 ulDemuxId)
{
	return svc_cas_PalPortDscSetVmxIptvDemuxId(ulDemuxId);
}

HERROR svc_cas_DeDscSetVmxIptvEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo)
{
	return svc_cas_PalPortDscSetVmxIptvEncryptionAlgorithm(ucSvcIdx, ucEncAlgo);
}
#endif

HBOOL svc_cas_DevDmxIsSupportScrambleBitChecking(HUINT32 ulDemuxId)
{
	return svc_cas_PalPortDmxIsSupportScrambleBitChecking(ulDemuxId);
}
HERROR svc_cas_DevDmxCheckScrambleBitByPid(HUINT32 ulDemuxId, SvcCas_DevDmxTsState_e *peState)
{
	return svc_cas_PalPortDmxCheckScrambleBitByPid(ulDemuxId, peState);
}


#define __________NVRAM___________
HERROR svc_cas_DevPortNVRAMWriteData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	return svc_cas_PalPortNVRAMWriteData(eCasGroupId, ulOffset, ulSize, pucData);
}

HERROR svc_cas_DevPortNVRAMReadData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData)
{
	return svc_cas_PalPortNVRAMReadData(eCasGroupId, ulOffset, ulSize, pucData);
}

#define __________CRYPT___________
HERROR svc_cas_DevPortSysEncrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize)
{
	return svc_cas_PalPortSysEncrypt(pucKey, bBlock, pucPlain, pucEncrypted, ulSize);
}
HERROR svc_cas_DevPortSysDecrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize)
{
	return svc_cas_PalPortSysDecrypt(pucKey, bBlock, pucPlain, pucDecrypted, ulSize);
}

HERROR svc_cas_DevPortSysEncryptData(SvcCas_Crypt_t *pstSvcCasCrypt)
{
	return svc_cas_PalPortSysEncryptData(pstSvcCasCrypt);
}

HERROR svc_cas_DevPortSysDecryptData(SvcCas_Crypt_t *pstSvcCasCrypt)
{
	return svc_cas_PalPortSysDecryptData(pstSvcCasCrypt);
}

HERROR svc_cas_DevPortChiperSha1(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest)
{
	return svc_cas_PalPortChiperSha1(pucMsg, ulMsgSize, pucDigest);
}

HERROR svc_cas_DevPortChiperSha256(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest)
{
	return svc_cas_PalPortChiperSha256(pucMsg, ulMsgSize, pucDigest);
}

HERROR svc_cas_DevPortChiperHmacSha1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult)
{
	return svc_cas_PalPortChiperHmacSha1(pucKey, ucKeySize, pucMsg, ulMsgSize, pucHmacResult);
}

HERROR svc_cas_DevPortSysEncryptAesData(HUINT8 *pucData, HUINT8 *pucEncBuf, HUINT32 ulSize)
{
	return svc_cas_PalPortSysEncryptAesData(pucData, pucEncBuf, ulSize);
}

HERROR svc_cas_DevPortSysDecryptAesData(HUINT8 *pucData, HUINT8 *pucDecBuf, HUINT32 ulSize)
{
	return svc_cas_PalPortSysDecryptAesData(pucData, pucDecBuf, ulSize);
}

HUINT8* svc_cas_DevPortSysGetEncryptionKey(void)
{
	return svc_cas_PalPortSysGetEncryptionKey();
}
#define __________PANEL___________
HERROR svc_cas_DevPortSetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel)
{
	return svc_cas_PalPortSetColdBootPanelCondition(bDisplayVideo, bViewPanel);
}

#define __________POWER___________
HERROR svc_cas_DevPortPowerReboot(void)
{
	return svc_cas_PalPortPowerReboot();
}

#define __________FLASH___________
HERROR svc_cas_DevPortNandFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	return svc_cas_PalPortNandFlashRead(ulAddress, pucBuffer, ulSize);
}

HERROR svc_cas_DevPortFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize)
{
	return svc_cas_PalPortFlashRead(ulAddress, pucBuffer, ulSize);
}


/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___


/*********************** End of File ******************************/

