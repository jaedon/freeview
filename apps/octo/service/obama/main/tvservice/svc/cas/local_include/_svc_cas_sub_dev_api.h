/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_cas_sub_dev_api.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__SVC_CAS_SUB_DEV_API_H__
#define	__SVC_CAS_SUB_DEV_API_H__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <octo_common.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___

/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

typedef enum
{
	eCAS_DEV_CI_EXTRACTED = 0,	/* 캠이 제거된 경우 또는 캠 모듈을 power off 한 경우 */
	eCAS_DEV_CI_INSERTED		/* 캠이 삽입된 직후 */
} SvcCas_DevCiState_e;			/* PalCi_State_e 와 매핑됨 */

typedef enum
{
	eCAS_DEV_CI_BYPASS = 0,
	eCAS_DEV_CI_PASSTHROUGH
} SvcCas_DevCiPath_e;			/* PalCi_Path_e 와 매핑됨 */

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
// ePAL_CI_CAM_SOURCE와 같다.
typedef enum
{
	eCAS_DEV_CI_SOURCE_INPUT0,
	eCAS_DEV_CI_SOURCE_INPUT1,
	eCAS_DEV_CI_SOURCE_OTHERMODULE,
	eCAS_DEV_CI_SOURCE_SOURCE_MAX
} SvcCas_DevCiSource_e;
#endif

/* sys */
typedef enum
{
	eCAS_DEV_FLASH_FIELD_SERIAL_NO = 0,
	eCAS_DEV_FLASH_FIELD_DTCP_KEY,
	eCAS_DEV_FLASH_FIELD_CI_PLUS_KEY,
	eCAS_DEV_FLASH_FIELD_MAC_ADDR,
	eCAS_DEV_FLASH_FIELD_HDCP_KEY,
	eCAS_DEV_FLASH_FIELD_PARING_DATA,
	eCAS_DEV_FLASH_FIELD_SYSTEM_ID,
	eCAS_DEV_FLASH_FIELD_LOADER_VER,
	eCAS_DEV_FLASH_FIELD_APP_VER,
	eCAS_DEV_FLASH_FIELD_PRIVATE,
	eCAS_DEV_FLASH_FIELD_CSC_KEY,

	eCAS_DEV_FLASH_FIELD_IRDETO_BBCB,
	eCAS_DEV_FLASH_FIELD_IRDETO_UC_SECURE_ID,
	eCAS_DEV_FLASH_FIELD_IRDETO_UC_PRIVATE_DATA,
	eCAS_DEV_FLASH_FIELD_IRDETO_CPCB,
	eCAS_DEV_FLASH_FIELD_IRDETO_CPCB_ENCRYPTED,
	eCAS_DEV_FLASH_FIELD_IRDETO_CA_PARTITION,
	eCAS_DEV_FLASH_FIELD_IRDETO_LOADER_PARTITION,
	eCAS_DEV_FLASH_FIELD_IRDETO_IR_PARTITION,
	eCAS_DEV_FLASH_FIELD_IRDETO_IR_BACKUP_PARTITION,

	eCAS_DEV_FLASH_FIELD_MAX
} SvcCas_DevFlashField_e;

/* dsc */
typedef enum
{
	eCAS_DSC_TYPE_NONEXDscType_None,
	eCAS_DSC_TYPE_DVB = 1,
	eCAS_DSC_TYPE_AES_CLEAR = 2,
	eCAS_DSC_TYPE_AES_BLOCK = 3,
	eCAS_DSC_TYPE_DES_CLEAR = 4,
	eCAS_DSC_TYPE_DES_BLOCK = 5,
	eCAS_DSC_TYPE_MAX
} SvcCas_DevDscType_e;  // PAL_DscType_t과 mapping 된다.

typedef enum
{
	eCAS_DSC_DATA_TYPE_NONE,
	eCAS_DSC_DATA_TYPE_VIDEO_PES,
	eCAS_DSC_DATA_TYPE_AUDIO_PES,
	eCAS_DSC_DATA_TYPE_PES_PES,
	eCAS_DSC_DATA_TYPE_PSI_PES,
	eCAS_DSC_DATA_TYPE_MAX
} SvcCas_DscDataType_e; // PalDsc_DataType_e 과 mapping된다.

typedef enum
{
	eCAS_DSC_PARITY_ODD,
	eCAS_DSC_PARITY_EVEN
} SvcCas_DevDscKeyParity_e; // PalDsc_KeyParity_e 과 mapping 된다.

typedef enum
{
	eCAS_CAM_TYPE_CI_V1 = 0, /*Legacy CAM*/
	eCAS_CAM_TYPE_CI_PLUS
} SvcCas_DevCiCamType_e;

/*	PalCi_ConnectWay_e 와 같음
*/
typedef enum
{
	eCAS_CI_CONNECT_NORMAL = 0,
	eCAS_CI_CONNECT_DAISYCHAIN,
	eCAS_CI_CONNECT_SPECIAL,
	eCAS_CI_CONNECT_PLAYBACK,
	eCAS_CI_CONNECT_MAX
} SvcCas_DevConnectWay_e;

/*	PalCi_ConnectStatus_e
*/
typedef struct CARM_CI_ConnectStatus
{
	SvcCas_DevCiState_e			eCiConnectState;
	SvcCas_DevConnectWay_e		eCiConnectWay;
} SvcCas_DevCiConnectStatue_t;

/*
typedef enum
{
	ePAL_CI_BYPASS = 0,
	PAL_CI_NORMAL_PASSTHROUGH,
	PAL_CI_DAISYCHAIN_PASSTHROUGH,
	PAL_CI_CUSTOM_PASSTHROUGH,
	ePAL_CI_PATHWAY_MAX
}PalCi_PathWay_e;
*/
typedef enum
{
	eCAS_CI_PATH_BYPASS = 0,
	eCAS_CI_PATH_NORMAL_PASSTHROUGH,
	eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH,
	eCAS_CI_PATH_CUSTOM_PASSTHROUGH,
	eCAS_CI_PATHWAY_MAX
} SvcCas_DevCiPathway_e;

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
/* SC */
typedef enum
{
	eCAS_SCI_SC_EXTRACTED,
	eCAS_SCI_SC_INSERTED
} SvcCas_DevSciEvent_e;

typedef HUINT32 SciHandle_t;
typedef void (* SvcCas_SciCb_t)(SciHandle_t hSci, HUINT8 ucSlotId, SvcCas_DevSciEvent_e eStatus);

typedef enum
{
	eCAS_SCM_RESET_TYPE_NONE,
	eCAS_SCM_RESET_TYPE_COLD,
	eCAS_SCM_RESET_TYPE_WARM
} SvcCas_DevScmResetType_e;


typedef enum
{
	eCAS_SCM_FLAG_DISABLE,
	eCAS_SCM_FLAG_ENABLE
} SvcCas_DevScmFlag_e;


typedef enum // PalSc_Protocol_e와 map.
{
	eCAS_SCI_T0			= 0,
	eCAS_SCI_T1			= 1,

	eCAS_SCI_T_0_IR,
	eCAS_SCI_T_0_CX,
	eCAS_SCI_T_1_NA,
	eCAS_SCI_T_1_ARIB,

	eCAS_SCI_T14			= 14,
	eCAS_SCI_T_END
} SvcCas_DevSciProtocol_e;

typedef enum
{
	eCAS_SCI_SC_NOT_PRESENT,
	eCAS_SCI_SC_PRESENT
} SvcCas_DevSciScStatus_e;

typedef struct
{
	SvcCas_DevSciProtocol_e eProtocol;
	HUINT32 ulClock;
	HUINT32	ulBaudrate;
	HUINT8	ucFI;
	HUINT8	ucDI;
	HUINT8	ucN;
	HUINT8	ucCWI;
	HUINT8	ucBWI;
} SvcCas_DevSciScParam_t;
#endif



#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
typedef struct
{
//	HUINT8		aucFileName[NAME_MAX];
	HUINT32		fileType;
	HUINT64		ulFileSize;
	HUINT32		aTime;
	HUINT32		mTime;
	HUINT32		cTime;
} SvcCas_DevFsFileState_t;
#endif

/* pal_fs.h 참고 */
typedef int					CASDEV_FS_BOOL;
typedef unsigned char		CASDEV_FS_UINT8;
typedef unsigned long		CASDEV_FS_UINT32;
typedef long long			CASDEV_FS_INT64;

#define CASDEV_FS_FILE_DIRECTORY		0x00000001
#define CASDEV_FS_FILE_STREAM			0x00000002
#define CASDEV_FS_FILE_CYCLIC			0x00000004
#define CASDEV_FS_FILE_BOOT			0x00000008
#define CASDEV_FS_MODE_READ			0x00000010
#define CASDEV_FS_MODE_WRITE			0x00000020
#define CASDEV_FS_MODE_CYCLIC			CASDEV_FS_FILE_CYCLIC
#define CASDEV_FS_MODE_PATH			0x00000080
#define CASDEV_FS_MODE_RDWR			0x00000100
#define CASDEV_FS_MODE_APPEND			0x00000200
#define CASDEV_FS_MODE_TRUNC			0x00000400
#define CASDEV_FS_MODE_NONBLOCK		0x00000800
#define CASDEV_FS_MODE_O_DIRECT		0x00001000
#define CASDEV_FS_MODE_O_LARGEFILE		0x00002000

enum
{
	CASDEV_FS_POS_START,
	CASDEV_FS_POS_CURRENT,
	CASDEV_FS_POS_END
};


// DmxTsState_t와 map.
typedef enum
{
	eCAS_DMX_TSSTATE_UNKNOWN = 0,
	eCAS_DMX_TSSTATE_CLEAN,
	eCAS_DMX_TSSTATE_SCRAMBLED
} SvcCas_DevDmxTsState_e;

typedef enum
{
	eCAS_CRYPT_AES,
	eCAS_CRYPT_DES,
	eCAS_CRYPT_TDES,
	eCAS_CRYPT_AES_CTR,
	eCAS_CRYPT_AES_CBC_MAC,
	eCAS_CRYPT_TDES_DTCPIP,
	eCAS_CRYPT_MAX
}SvcCas_DevCryptMode_t;

typedef enum
{
	eCAS_CRYPT_DEFAULT_SYSTEM_KEY,
	eCAS_CRYPT_INTERNAL_CAS_KEY,
	eCAS_CRYPT_USER_KEY,
}SvcCas_DevCryptKeyType_t;

typedef struct
{
	SvcCas_DevCryptMode_t		etCryptMode;
	SvcCas_DevCryptKeyType_t	etCryptKey;
	HUINT8						*pKey;
	HUINT32						ulKeySize;
	const HUINT8				*pSrcBuffer;
	HUINT8						*pDstBuffer;
	HUINT32						ulBuffSize;
}SvcCas_Crypt_t;


/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___

/********************************************************************
* Global Public Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Global_Public_Definitions___

/********************************************************************
* Global Public Function Prototypes
********************************************************************/
#define ___04_Global_Public_Func_Prototypes___

#define _________System_____________

HERROR svc_cas_DevSysGetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
HERROR svc_cas_DevSysSetField(SvcCas_DevFlashField_e eType, HUINT32 ulOffset, void *pData, HUINT32 ulSize);
HERROR svc_cas_DevPortSysGetChipId(HUINT32 *pulChipId);
HERROR svc_cas_DevPortSysGetChipRevision(HUINT32 *pulChipId);

#define _________AV_____________

HERROR svc_cas_DevVideoIsRunning(HUINT32 ulDecoderId, HBOOL *pbState);

#define __________FS___________

CASDEV_FS_BOOL svc_cas_DevFsIsDirExist(const char *dirname);
int svc_cas_DevFsMkdir(const char *dirname);
CASDEV_FS_BOOL svc_cas_DevFsIsFileExist(const char *pathname);

#define __________CI___________

#if defined(CONFIG_MW_CAS_NUM_SC_SLOT)
HERROR svc_cas_DevScInit(void);
HERROR svc_cas_DevScGetSlotId(SciHandle_t hSci, HUINT8 *pulSlotId);
HERROR svc_cas_DevScDeactivate(SciHandle_t hSci);
HERROR svc_cas_DevScReset(	SciHandle_t hSci,
						HUINT8 *pucAtr,
						HUINT32 *pulLen,
						SvcCas_DevScmResetType_e eResetType,
						SvcCas_DevSciScParam_t *pstDefaultParam
						);
HERROR svc_cas_DevScRegisterCbFunc(HUINT32 ulSlotId, DxCAS_GroupId_e eCARMId, SvcCas_SciCb_t pfnCb, SciHandle_t *hSci);
HERROR svc_cas_DevScTransfer(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead, HUINT32 ulEpxectedNumRead);
HERROR svc_cas_DevScRead(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);
HERROR svc_cas_DevScWrite(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT32 ulNumWrite, HUINT8 *pucReadBuf, HUINT32 *pulNumRead);
HERROR svc_cas_DevScSetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam);
HERROR svc_cas_DevScGetParams(SciHandle_t hSci, SvcCas_DevSciScParam_t *pstParam);
HERROR svc_cas_DevScNotifyScValidity(SciHandle_t hSci, SvcCas_DevScmFlag_e eMatched);
HERROR svc_cas_DevScNegotiatePTS(SciHandle_t hSci, HUINT8 *pucWriteBuf, HUINT8 ucNumToWrite, HUINT8 *pucReadBuf, HUINT8 *pucNumRead);
HERROR svc_cas_DevScGetCardStatus(SciHandle_t hSci, SvcCas_DevSciScStatus_e *peStatus);
HERROR svc_cas_DevScGetCardStatusBySlotID(HUINT32 ulSlotId, SvcCas_DevSciScStatus_e *peStatus);
HUINT32 svc_cas_DevScGetNumberOfModule(void);
HERROR svc_cas_DevScActivate(SciHandle_t hSci);
#endif

#if defined(CONFIG_PROD_DEVICE_FILE_SYSTEM)
int		svc_cas_DevFsOpen(int pttype, const char *pathname, CASDEV_FS_UINT32 flags, CASDEV_FS_UINT32 *filedes);
int		svc_cas_DevFsClose(CASDEV_FS_UINT32 filedes);
int		svc_cas_DevFsRead(CASDEV_FS_UINT32 filedes, CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,   CASDEV_FS_UINT32 *sizep);
int		svc_cas_DevFsWrite(CASDEV_FS_UINT32 filedes, const CASDEV_FS_UINT8 *buf, CASDEV_FS_UINT32 size,    CASDEV_FS_UINT32 *sizep);
int		svc_cas_DevFsLSeek(CASDEV_FS_UINT32 filedes, CASDEV_FS_INT64 offset, CASDEV_FS_UINT32 pos,     CASDEV_FS_INT64 *offsetp);
int		svc_cas_DevFsDelete(int pttype, const char *pathname);
#if defined (CONFIG_MW_CAS_VIACCESS) && defined (CONFIG_MW_MM_PVR)
int 	svc_cas_DevFsGetHddSerialNumber(const char *dev, char **hddsn);
#endif
int 	svc_cas_DevFsGetFileInfo(const char *pathname, HUINT32 type, SvcCas_DevFsFileState_t *filestatp);
#endif


#if defined(CONFIG_MW_CI)
HUINT16 svc_cas_DevCiGetNumberOfModule(void);
SvcCas_DevCiState_e CARM_CI_CheckModule (CI_SLOT_ID usSlotId);
HERROR svc_cas_DevCiResetModule (CI_SLOT_ID usSlotId);
HERROR svc_cas_DevCiSetCamType(CI_SLOT_ID usSlotId, SvcCas_DevCiCamType_e eType);
HINT32 svc_cas_DevCiOpen (HUINT8 *device_name);
HUINT8 *svc_cas_DevCiGetDeviceName(void);
HINT32 svc_cas_DevCiClose (HUINT16 usSlotId);
HERROR svc_cas_DevCiPlugInCi (HUINT32 ulActionId, HINT32 nSlotId);
HERROR svc_cas_DevCiSetTsPath (CI_SLOT_ID usSlotId, SvcCas_DevCiPath_e path);

HERROR svc_cas_DevCiRegisterCallback (void (*CiCallbackFunc)(CI_SLOT_ID usSlotId, unsigned short flag));

HERROR svc_cas_DevCiGetStatusCI(unsigned short usSlotId, SvcCas_DevCiConnectStatue_t *pstCIStatus);

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
HERROR svc_cas_DevCiGetCurrentPathWay(HUINT32 unSlotId, SvcCas_DevCiPathway_e *peCiPathWay);
HERROR svc_cas_DevCiSetExtendedTsPath(HUINT32 unTunerId, HUINT32 unSlotId, HUINT32 unDemuxId, SvcCas_DevCiPathway_e eCarmCiPathWay);
#endif

HINT32 svc_cas_DevCiWrite (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 svc_cas_DevCiRead (HUINT16 usSlotId, HUINT16 len, HUINT8 *pbytes);
HINT32 svc_cas_DevCiIoctl (HUINT16 usSlotId, HINT32 command, void *parg);

#endif
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI_PLUS)

HERROR svc_cas_DevDscInit(void);
HERROR svc_cas_DevDscCloseAll(void);
HERROR svc_cas_DevDscResetPid(HUINT32 ulDscIndex, HUINT16 usPid);
HERROR svc_cas_DevDscSetProtectionKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData);
HERROR svc_cas_DevDscClearProtectionKey(HUINT32 ulDscIndex);
HERROR svc_cas_DevDscSetKey(HUINT32 ulDscIndex, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData);
HERROR svc_cas_DevDscSetAesKey(HUINT32 ulDscIndex, HUINT8 *pucKeyData, HUINT32 ulKeySize);
HERROR svc_cas_DevDscOpen(HUINT32 ulDmxId, SvcCas_DevDscType_e eDscType, SvcCas_DscDataType_e eDataType, HUINT32 *pulDscIndex);
HERROR svc_cas_DevDscClose(HUINT32 ulDscIndex);
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_PLUS)
HERROR svc_cas_DevDscSetKeyForCiPlus(HUINT32 ulDscIndex, SvcCas_DevDscType_e eDscType, SvcCas_DevDscKeyParity_e eParity, HUINT8 *pucKeyData, HUINT8 *pucInitVec);
#endif
HERROR svc_cas_DevDscSetPid(HUINT32 ulDscIndex, HUINT16 usPid);
#endif

#if defined(CONFIG_MW_CAS_VERIMATRIX_IPTV)
HERROR svc_cas_DeDscSetVmxIptvDemuxId(HUINT32 ulDemuxId);
HERROR svc_cas_DeDscSetVmxIptvEncryptionAlgorithm(HUINT8 ucSvcIdx, HUINT8 ucEncAlgo);
#endif

HBOOL svc_cas_DevDmxIsSupportScrambleBitChecking(HUINT32 ulDemuxId);
HERROR svc_cas_DevDmxCheckScrambleBitByPid(HUINT32 ulDemuxId, SvcCas_DevDmxTsState_e *peState);

#define __________NVRAM___________
HERROR svc_cas_DevPortNVRAMWriteData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);
HERROR svc_cas_DevPortNVRAMReadData(DxCAS_GroupId_e eCasGroupId, HUINT32 ulOffset, HUINT32 ulSize, HUINT8 *pucData);

#define __________CRYPT___________
HERROR svc_cas_DevPortSysEncrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucEncrypted, HUINT32 ulSize);
HERROR svc_cas_DevPortSysDecrypt(HUINT8 *pucKey, HBOOL bBlock, HUINT8 *pucPlain, HUINT8 *pucDecrypted, HUINT32 ulSize);
HERROR svc_cas_DevPortSysEncryptData(SvcCas_Crypt_t *pstSvcCasCrypt);
HERROR svc_cas_DevPortSysDecryptData(SvcCas_Crypt_t *pstSvcCasCrypt);
HERROR svc_cas_DevPortChiperSha1(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest);
HERROR svc_cas_DevPortChiperSha256(HUINT8* pucMsg, HUINT32 ulMsgSize, HUINT8* pucDigest);
HERROR svc_cas_DevPortChiperHmacSha1(HUINT8 *pucKey, HUINT8 ucKeySize, HUINT8 *pucMsg, HUINT32 ulMsgSize, HUINT8 *pucHmacResult);
HERROR svc_cas_DevPortSysEncryptAesData(HUINT8 *pucData, HUINT8 *pucEncBuf, HUINT32 ulSize);
HERROR svc_cas_DevPortSysDecryptAesData(HUINT8 *pucData, HUINT8 *pucDecBuf, HUINT32 ulSize);
HUINT8* svc_cas_DevPortSysGetEncryptionKey(void);

#define __________PANEL___________
HERROR svc_cas_DevPortSetColdBootPanelCondition (HBOOL bDisplayVideo, HBOOL bViewPanel);

#define __________POWER___________
HERROR svc_cas_DevPortPowerReboot(void);

#define __________FLASH___________
HERROR svc_cas_DevPortNandFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);
HERROR svc_cas_DevPortFlashRead(HUINT32 ulAddress, HUINT8 *pucBuffer, HUINT32 ulSize);

#ifdef __cplusplus
}
#endif


#endif /* !__SVC_CAS_SUB_DEV_API_H__ */
