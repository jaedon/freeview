/**************************************************************************************************/
#define ________CA_SEC_Private_Include_____________________________________________________
/**************************************************************************************************/
#include "na_glob.h"

//#include "pal_sys.h"
#include "util.h"
#include <_svc_cas_mgr_common.h>
#if defined(MULTI_DIRECT_TSID)
#include "na_ca_session_mgr.h"
#endif

/**************************************************************************************************/
#define ________CA_SEC_Golbal_Value________________________________________________________
/**************************************************************************************************/
#if !defined(CONFIG_MW_CAS_NAGRA_OP_AMX)
#define _NA_SEC_USE_LAGERCYKEY_FUNC
#define _NA_SEC_USE_SET0LEVEL_PROTECTEDKEY_FUNC
#endif

/**************************************************************************************************/
#define ________CA_SEC_Private_Define_______________________________________________________
/**************************************************************************************************/






/**************************************************************************************************/
#define ________CA_SEC_Private_Type_Define_________________________________________________
/**************************************************************************************************/
#if 0//defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define SECLogTrace(message) 	(HLIB_CMD_Printf(C_CODE_F_PURPLE"[CA SEC](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define SECLogError(message) 	(HLIB_CMD_Printf(C_CODE_F_RED"[CA SEC](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))
#define SECLogWarning(message) 	(HLIB_CMD_Printf(C_CODE_F_RED_B_BLACK"[CA SEC](%s:%d)"C_CODE_RESET, __FUNCTION__, __LINE__), (HLIB_CMD_Printf message))


#if defined (HxLOG_Print)
#undef	HxLOG_Print
#define HxLOG_Print(...)	SECLogTrace((__VA_ARGS__))
#endif

#if defined (HxLOG_Error)
#undef	HxLOG_Error
#define HxLOG_Error(...)	SECLogError((__VA_ARGS__))
#endif

#if defined (HxLOG_Warning)
#undef	HxLOG_Warning
#define HxLOG_Warning(...)	SECLogWarning((__VA_ARGS__))
#endif

#endif

typedef enum
{
	eNotthing = 0x0,
	eInitialized,
	eTerminated,

	SecStatus_Max
} SecStatus_t;

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
typedef enum
{
	eHashSem_Sha1,
	eHashSem_Sha256,
	eHashSem_Sha384,
	eHashSem_Hmac256,
	eHashSem_MAX
}eSecHashSemaphore_t;
#endif

#define CSC_FLASH_SIZE			(256)
#define CSC_KEY_SIZE 			(16)
#define CHIPREVISION_SIZE		(3)

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
#define SEC_SESSION_MAX_NUM_FOR_DAL_DAL		10
typedef struct tagNaSecSessionInfo
{
	HBOOL					bRegistered;
	TSecCipherSession		xSession;
	TTransportSessionId		xTransportSessionId;
	TUnsignedInt16			xEmi;
} NaSecSessionInfo_t;
#endif
#endif

/**************************************************************************************************/
#define ________CA_SEC_Private_Static_Value_________________________________________________
/**************************************************************************************************/
#if !defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
static SecStatus_t 		s_eSecStatus = eNotthing;
static TUnsignedInt8	s_ucChipRevision[CHIPREVISION_SIZE] = {0, };
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if !defined(_NA_NOCS_OVERRIDE_FUNCTION)
static TSecFunctionTable s_stSecFunctionTable;
#endif
static HUINT32			s_ulSecSemId;
static HUINT32			s_aulHashSemId[eHashSem_MAX];
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) && !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
STATIC NaPrmKeyInfoCallback_t s_stSecKeyDataCallback;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
NaSecSessionInfo_t s_astSecSessionInfo[SEC_SESSION_MAX_NUM_FOR_DAL_DAL];
#endif
#endif

/**************************************************************************************************/
#define ________CA_SEC_Private_Static_Prototype______________________________________________
/**************************************************************************************************/
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE) && !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
static void local_na_sec_CallKeyDataInfoCallback(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, NaPrmKeyType_e eKeyType, size_t xSize, const TUnsignedInt8* pxKey);
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
STATIC NaSecSessionInfo_t	*_na_sec_GetSessionInfoTable(void);
STATIC NaSecSessionInfo_t	*_na_sec_GetSessionInfoByHandle(TSecCipherSession xSession);
STATIC void					_na_sec_InitSecSessionInfo(void);
STATIC HERROR				_na_sec_RegisterSessionInfo(TSecCipherSession xSession, TTransportSessionId xTsId);
STATIC HERROR				_na_sec_UnregisterSessionInfo(TSecCipherSession xSession);
STATIC HERROR				_na_sec_GetTsIdByHandle(TSecCipherSession xSession, TTransportSessionId *pxTsId);
#endif
#endif

/**************************************************************************************************/
#define ________CA_SEC_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_SEC_Externel_Functions_Prototype_______________________________________________
/**************************************************************************************************/
#if defined(MULTI_DIRECT_TSID)
#else
HBOOL CA_NA_CaSessionMgrIsTsIdSetDirect(void);
#endif

extern TSecStatus secGetNuid64(TSecNuid64 *pxNuid);
extern TSecStatus secGetChipsetExtension(const TChar** ppxChipsetExtension);
extern TSecStatus secGenerateRandomBytes(size_t xNumOfBytes, TUnsignedInt8* pxRandomBytes);
extern TSecStatus secRsaGenerateKey(size_t xKeySize, TUnsignedInt32 xE, TUnsignedInt8* pxN, TUnsignedInt8* pxP, TUnsignedInt8* pxQ, TUnsignedInt8* pxDP, TUnsignedInt8* pxDQ, TUnsignedInt8* pxQInv);
extern TSecStatus secRsaPublicEncrypt(const TUnsignedInt8* pxInput, size_t xInputSize, TUnsignedInt8* pxOutput, TUnsignedInt32 xE, const TUnsignedInt8* pxN, size_t xKeySize, TSecRsaPadding xPadding);
extern TSecStatus secRsaPrivateEncrypt(const TUnsignedInt8* pxInput, size_t xInputSize, TUnsignedInt8* pxOutput, TUnsignedInt32 xE, const TUnsignedInt8* pxN, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, const TUnsignedInt8* pxDP, const TUnsignedInt8* pxDQ, const TUnsignedInt8* pxQInv, size_t xKeySize, TSecRsaPadding xPadding);
extern TSecStatus secRsaPublicDecrypt(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t* pxOutputSize, TUnsignedInt32 xE, const TUnsignedInt8* pxN, size_t xKeySize, TSecRsaPadding xPadding);
extern TSecStatus secRsaPrivateDecrypt(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t* pxOutputSize, TUnsignedInt32 xE, const TUnsignedInt8* pxN, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, const TUnsignedInt8* pxDP, const TUnsignedInt8* pxDQ, const TUnsignedInt8* pxQInv, size_t xKeySize, TSecRsaPadding xPadding);
extern TSecStatus secDhGenerateKey(const TUnsignedInt8* pxG, const TUnsignedInt8* pxP, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKey, size_t xKeySize);
extern TSecStatus secDhComputeKey(const TUnsignedInt8* pxP, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxOtherPubKey, TUnsignedInt8* pxSharedSecret, size_t xKeySize);
extern TSecStatus secSha1Init(TSecHashContext* pxContext);
extern TSecStatus secSha1Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize);
extern TSecStatus secSha1Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest);
extern TSecStatus secSha256Init(TSecHashContext* pxContext);
extern TSecStatus secSha256Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize);
extern TSecStatus secSha256Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest);
extern TSecStatus secSha384Init(TSecHashContext* pxContext);
extern TSecStatus secSha384Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize);
extern TSecStatus secSha384Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest);
extern TSecStatus secHmacSha256Init(const TUnsignedInt8* pxKey, size_t xKeySize, TSecHashContext* pxContext);
extern TSecStatus secHmacSha256Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize);
extern TSecStatus secHmacSha256Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest);
extern TSecStatus secEcdsaGenerateKey(TSecEcParams xParams, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKeyX, TUnsignedInt8* pxPubKeyY);
extern TSecStatus secEcdsaSign(TSecEcParams xParams, TSecHashType xHashType, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxMessage, size_t xMessageSize, TUnsignedInt8* pxSigR, TUnsignedInt8* pxSigS);
extern TSecStatus secEcdsaVerify(TSecEcParams xParams, TSecHashType xHashType, const TUnsignedInt8* pxPubKeyX, const TUnsignedInt8* pxPubKeyY, const TUnsignedInt8* pxMessage, size_t xMessageSize, const TUnsignedInt8* pxSigR, const TUnsignedInt8* pxSigS);
extern TSecStatus secEcdhGenerateKey(TSecEcParams xParams, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKeyX, TUnsignedInt8* pxPubKeyY);
extern TSecStatus secEcdhComputeKey(TSecEcParams xParams, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxOtherPubKeyX, const TUnsignedInt8* pxOtherPubKeyY, TUnsignedInt8* pxSharedSecret);
extern TSecStatus secRsaComputeCrtParams(size_t xKeySize, TUnsignedInt32 xE, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, TUnsignedInt8* pxDP, TUnsignedInt8* pxDQ, TUnsignedInt8* pxQInv);
extern TSecStatus secEncryptFlashProtKey(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xSize);
extern TSecStatus secUseFlashProtKey(TSecCipherSession xSession, TUnsignedInt16 xEmi);
extern TSecStatus secUseLegacyKey(TSecCipherSession xSession);
extern TSecStatus secOpenRam2RamEncryptSession(TSecCipherSession* pxSession);
extern TSecStatus secOpenRam2RamDecryptSession(TSecCipherSession* pxSession);
extern TSecStatus secCloseSession(TSecCipherSession xSession);
extern TSecStatus secSetClearTextKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xClearTextKeySize, const TUnsignedInt8* pxClearTextKey);
extern TSecStatus secUseCertKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId);
extern TSecStatus secSessionEncrypt(TSecCipherSession xSession, const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xMessageSize, const TUnsignedInt8* pxInitVector, size_t xInitVectorSize);
extern TSecStatus secSessionDecrypt(TSecCipherSession xSession, const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xMessageSize, const TUnsignedInt8* pxInitVector, size_t xInitVectorSize);
extern TSecStatus secOpenStreamEncryptSession(TSecCipherSession* pxSession, TTransportSessionId xTransportSessionId);
extern TSecStatus secOpenStreamDecryptSession(TSecCipherSession* pxSession, TTransportSessionId xTransportSessionId);
extern TSecStatus secSet0LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey);
extern TSecStatus secSet1LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey, size_t xCipheredProtectingKeySize, const TUnsignedInt8* pxL1CipheredProtectingKey);
extern TSecStatus secSet2LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey, size_t xCipheredProtectingKeySize, const TUnsignedInt8* pxL1CipheredProtectingKey, const TUnsignedInt8* pxL2CipheredProtectingKey);
extern TSecStatus secSetMetadata(TSecCipherSession xSession, size_t xMetadataSize, const TUnsignedInt8* pxMetadata);
extern TUnsignedInt8* secAllocateBuffer(size_t xBufferSize);
extern TSecStatus secFreeBuffer(TUnsignedInt8 *pxBuffer);
extern TSessionManagerStatus brcmCaGetM2MKeyslotHandle(TTransportSessionId xTSid, NEXUS_KeySlotHandle *pM2mKeyHandle);

/**************************************************************************************************/
#define ________CA_SEC_Public_Functions_Body_______________________________________________
/**************************************************************************************************/
#if !defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
TSecStatus secInitialize(void)
{
	if (s_eSecStatus == eInitialized)
	{
		HxLOG_Info("Already Initialized....\n");
		return SEC_ERROR;
	}

	HxLOG_Print("SEC Initialized....\n");
	s_eSecStatus = eInitialized;
	return SEC_NO_ERROR;
}


TSecStatus secTerminate(void)
{
	if (s_eSecStatus == eTerminated)
	{
		HxLOG_Info("Already Terminated....\n");
		return SEC_ERROR;
	}

	HxLOG_Print("SEC Terminated....\n");
	s_eSecStatus = eTerminated;
	return SEC_NO_ERROR;
}


TSecStatus secGetNuid(TSecNuid *pxNuid)
{
	TUnsignedInt32 ulChipId = 0;
	TUnsignedInt8 aucNuid[4], aucBigNuid[4];

	if (pxNuid == NULL)
	{
	    HxLOG_Error("error pxNuid is NULL...\n");
		return SEC_ERROR;
	}

   	if (svc_cas_DevPortSysGetChipId(&ulChipId) != ERR_OK)
   	{
	   	HxLOG_Error("error in svc_cas_DevPortSysGetChipId...\n");
	   	return SEC_ERROR;
	}

	HxSTD_memset(aucNuid, 0, 4);
	HxSTD_memcpy(aucNuid, &ulChipId, sizeof(TUnsignedInt32));

	// convert little endian to big endian
	aucBigNuid[0] = aucNuid[3];
	aucBigNuid[1] = aucNuid[2];
	aucBigNuid[2] = aucNuid[1];
	aucBigNuid[3] = aucNuid[0];
	HxSTD_memcpy( pxNuid, aucBigNuid, 4 );

	HxLOG_Print("NUID (0x%X)\n", ulChipId);
	HxLOG_Print("Target (%X, %X, %X, %X)\n", aucBigNuid[0], aucBigNuid[1], aucBigNuid[2], aucBigNuid[3]);
	return SEC_NO_ERROR;
}


TSecStatus secEncryptData(TUnsignedInt8 *pxDst, const TUnsignedInt8 *pxSrc, TSize xDataLength)
{
	TUnsignedInt32 	ulRemain = 0;
	TSignedInt32	nErrCode = ERR_OK;
	SvcCas_Crypt_t	stSvcCasCrypt;

	TUnsignedInt8 	ucCsc[CSC_FLASH_SIZE];
	TUnsignedInt8  	aucKey[] = {0x53, 0xF4, 0x63, 0x14 ,0xAC, 0x59, 0x3C, 0xC5, 0x73, 0x7A, 0xF8, 0x3A, 0x5B, 0x15, 0xB9, 0xFE};

	if (pxDst == NULL || pxSrc == NULL)
	{
	    HxLOG_Error("error pxDst or pxSrc is NULL...\n");
		return SEC_ERROR;
	}

	ulRemain = xDataLength % 8;
	if (ulRemain != 0)
	{
	    HxLOG_Error("xDataLength (%d) is not multile of 8 bytes\n", xDataLength);
		return SEC_ERROR;
	}

	nErrCode = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)ucCsc, CSC_FLASH_SIZE);
	if (nErrCode != ERR_OK)
	{
	    HxLOG_Error("error svc_cas_DevSysGetField, nErrCode:0x%X\n", nErrCode);
		return SEC_ERROR;
	}

	HxSTD_memcpy(aucKey, &ucCsc[17], CSC_KEY_SIZE);
	HxSTD_memset(&stSvcCasCrypt, 0x00, sizeof(SvcCas_Crypt_t));

	stSvcCasCrypt.etCryptMode = eCAS_CRYPT_TDES;
	stSvcCasCrypt.etCryptKey = eCAS_CRYPT_INTERNAL_CAS_KEY;
	stSvcCasCrypt.ulKeySize = CSC_KEY_SIZE;
	stSvcCasCrypt.ulBuffSize = xDataLength;
	stSvcCasCrypt.pKey = aucKey;

	stSvcCasCrypt.pSrcBuffer = pxSrc;
	stSvcCasCrypt.pDstBuffer = pxDst;

	if(svc_cas_DevPortSysEncryptData(&stSvcCasCrypt) != ERR_OK)
	{
		HxLOG_Error("error svc_cas_DevPortSysEncryptData\n");
		return SEC_ERROR;
	}

	HxLOG_Print("secEncryptData (-)\n");
	return SEC_NO_ERROR;
}


TSecStatus secDecryptData(  TUnsignedInt8 *pxDst, const TUnsignedInt8 *pxSrc, TSize xDataLength)
{
	TUnsignedInt32 	ulRemain = 0;
	TSignedInt32 	nErrCode = ERR_OK;
	SvcCas_Crypt_t	stSvcCasCrypt;

	TUnsignedInt8 	ucCsc[CSC_FLASH_SIZE];
	TUnsignedInt8  	aucKey[] = {0x53, 0xF4, 0x63, 0x14 ,0xAC, 0x59, 0x3C, 0xC5, 0x73, 0x7A, 0xF8, 0x3A, 0x5B, 0x15, 0xB9, 0xFE};

	if (pxDst == NULL || pxSrc == NULL)
	{
	    HxLOG_Error("error pxDst or pxSrc is NULL...\n");
		return SEC_ERROR;
	}

	ulRemain = xDataLength % 8;
	if (ulRemain != 0)
	{
	    HxLOG_Error("xDataLength (%d) is not multile of 8 bytes\n", xDataLength);
		return SEC_ERROR;
	}

	nErrCode = svc_cas_DevSysGetField(eCAS_DEV_FLASH_FIELD_CSC_KEY, 0, (void*)ucCsc, CSC_FLASH_SIZE);
	if (nErrCode != ERR_OK)
	{
	    HxLOG_Error("error svc_cas_DevSysGetField, nErrCode:0x%X\n", nErrCode);
		return SEC_ERROR;
	}
	HxSTD_memcpy(aucKey, &ucCsc[17], CSC_KEY_SIZE);
	HxSTD_memset(&stSvcCasCrypt, 0x00, sizeof(SvcCas_Crypt_t));

	stSvcCasCrypt.etCryptMode = eCAS_CRYPT_TDES;
	stSvcCasCrypt.etCryptKey = eCAS_CRYPT_INTERNAL_CAS_KEY;
	stSvcCasCrypt.ulKeySize = CSC_KEY_SIZE;
	stSvcCasCrypt.ulBuffSize = xDataLength;
	stSvcCasCrypt.pKey = aucKey;
	stSvcCasCrypt.pSrcBuffer = pxSrc;
	stSvcCasCrypt.pDstBuffer = pxDst;

    if(svc_cas_DevPortSysDecryptData(&stSvcCasCrypt) != ERR_OK)
	{
		HxLOG_Error("error svc_cas_DevPortSysDecryptData\n");
		return SEC_ERROR;
	}

	HxLOG_Print("secDecryptData (-)\n");
	return SEC_NO_ERROR;
}


TSecStatus secGetChipsetRevision(const TChar** ppxChipsetRevision)
{
	HxLOG_Print("secGetChipsetRevision (+)\n");

	if (ppxChipsetRevision == NULL)
		return SEC_ERROR;

	HxSTD_memset(s_ucChipRevision, 0, sizeof(s_ucChipRevision));

	svc_cas_DevPortSysGetChipRevision(s_ucChipRevision);
	HxLOG_Print("secGetChipsetRevision ucChipRevision= %s\n", s_ucChipRevision);

	*ppxChipsetRevision = s_ucChipRevision;

	//*ppxChipsetRevision = (TChar*)"B1";
	HxLOG_Print("secGetChipsetRevision (-)\n");
	return SEC_NO_ERROR;
}
#else

void secChipReset(void)
{
	HxLOG_Print("secChipReset (+)\n");

	system("reboot");
}

static TSecStatus _caSec_GetNuid(TSecNuid *pxNuid)
{
	TSecStatus secStatus;

	secStatus = secGetNuid(pxNuid);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_GetNuid64(TSecNuid64 *pxNuid)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secGetNuid64(pxNuid);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_GetChipsetRevision(const TChar** ppxChipsetRevision)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secGetChipsetRevision(ppxChipsetRevision);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_GetChipsetExtension(const TChar** ppxChipsetExtension)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secGetChipsetExtension(ppxChipsetExtension);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EncryptData(TUnsignedInt8* pxOutput, const TUnsignedInt8* pxInput, TSize xDataSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEncryptData(pxOutput, pxInput, xDataSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_DecryptData(TUnsignedInt8* pxOutput, const TUnsignedInt8* pxInput, TSize xDataSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secDecryptData(pxOutput, pxInput, xDataSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_GenerateRandomBytes(size_t xNumOfBytes, TUnsignedInt8* pxRandomBytes)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secGenerateRandomBytes(xNumOfBytes, pxRandomBytes);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaGenerateKey(size_t xKeySize, TUnsignedInt32 xE, TUnsignedInt8* pxN, TUnsignedInt8* pxP, TUnsignedInt8* pxQ, TUnsignedInt8* pxDP, TUnsignedInt8* pxDQ, TUnsignedInt8* pxQInv)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaGenerateKey(xKeySize, xE, pxN, pxP, pxQ, pxDP, pxDQ, pxQInv);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaPublicEncrypt(const TUnsignedInt8* pxInput, size_t xInputSize, TUnsignedInt8* pxOutput, TUnsignedInt32 xE, const TUnsignedInt8* pxN, size_t xKeySize, TSecRsaPadding xPadding)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaPublicEncrypt(pxInput, xInputSize, pxOutput, xE, pxN, xKeySize, xPadding);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaPrivateEncrypt(const TUnsignedInt8* pxInput, size_t xInputSize, TUnsignedInt8* pxOutput, TUnsignedInt32 xE, const TUnsignedInt8* pxN, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, const TUnsignedInt8* pxDP, const TUnsignedInt8* pxDQ, const TUnsignedInt8* pxQInv, size_t xKeySize, TSecRsaPadding xPadding)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaPrivateEncrypt(pxInput, xInputSize, pxOutput, xE, pxN, pxP, pxQ, pxDP, pxDQ, pxQInv, xKeySize, xPadding);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaPublicDecrypt(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t* pxOutputSize, TUnsignedInt32 xE, const TUnsignedInt8* pxN, size_t xKeySize, TSecRsaPadding xPadding)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaPublicDecrypt(pxInput, pxOutput, pxOutputSize, xE, pxN, xKeySize, xPadding);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaPrivateDecrypt(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t* pxOutputSize, TUnsignedInt32 xE, const TUnsignedInt8* pxN, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, const TUnsignedInt8* pxDP, const TUnsignedInt8* pxDQ, const TUnsignedInt8* pxQInv, size_t xKeySize, TSecRsaPadding xPadding)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaPrivateDecrypt(pxInput, pxOutput, pxOutputSize, xE, pxN, pxP, pxQ, pxDP, pxDQ, pxQInv, xKeySize, xPadding);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_DhGenerateKey(const TUnsignedInt8* pxG, const TUnsignedInt8* pxP, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKey, size_t xKeySize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secDhGenerateKey(pxG, pxP, pxInputPrivKey, pxOutputPrivKey, pxPubKey, xKeySize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_DhComputeKey(const TUnsignedInt8* pxP, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxOtherPubKey, TUnsignedInt8* pxSharedSecret, size_t xKeySize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secDhComputeKey(pxP, pxPrivKey, pxOtherPubKey, pxSharedSecret, xKeySize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha1Init(TSecHashContext* pxContext)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	VK_SEM_Get(s_aulHashSemId[eHashSem_Sha1]);
	secStatus = secSha1Init(pxContext);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha1]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha1Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha1Update(xContext, pxMessageChunk, xChunkSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha1]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha1Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha1Final(xContext, pxMessageDigest);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
	VK_SEM_Release(s_aulHashSemId[eHashSem_Sha1]);

	return secStatus;
}

static TSecStatus _caSec_Sha256Init(TSecHashContext* pxContext)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	VK_SEM_Get(s_aulHashSemId[eHashSem_Sha256]);
	secStatus = secSha256Init(pxContext);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha256]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha256Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha256Update(xContext, pxMessageChunk, xChunkSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha256]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha256Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha256Final(xContext, pxMessageDigest);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
	VK_SEM_Release(s_aulHashSemId[eHashSem_Sha256]);

	return secStatus;
}

static TSecStatus _caSec_Sha384Init(TSecHashContext* pxContext)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	VK_SEM_Get(s_aulHashSemId[eHashSem_Sha384]);
	secStatus = secSha384Init(pxContext);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha384]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha384Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha384Update(xContext, pxMessageChunk, xChunkSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Sha384]);
	}

	return secStatus;
}

static TSecStatus _caSec_Sha384Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSha384Final(xContext, pxMessageDigest);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
	VK_SEM_Release(s_aulHashSemId[eHashSem_Sha384]);

	return secStatus;
}

static TSecStatus _caSec_HmacSha256Init(const TUnsignedInt8* pxKey, size_t xKeySize, TSecHashContext* pxContext)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	VK_SEM_Get(s_aulHashSemId[eHashSem_Hmac256]);
	secStatus = secHmacSha256Init(pxKey, xKeySize, pxContext);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Hmac256]);
	}

	return secStatus;
}

static TSecStatus _caSec_HmacSha256Update(TSecHashContext xContext, const TUnsignedInt8* pxMessageChunk, size_t xChunkSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secHmacSha256Update(xContext, pxMessageChunk, xChunkSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
		VK_SEM_Release(s_aulHashSemId[eHashSem_Hmac256]);
	}

	return secStatus;
}

static TSecStatus _caSec_HmacSha256Final(TSecHashContext xContext, TUnsignedInt8* pxMessageDigest)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secHmacSha256Final(xContext, pxMessageDigest);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
	VK_SEM_Release(s_aulHashSemId[eHashSem_Hmac256]);

	return secStatus;
}

static TSecStatus _caSec_EcdsaGenerateKey(TSecEcParams xParams, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKeyX, TUnsignedInt8* pxPubKeyY)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEcdsaGenerateKey(xParams, pxInputPrivKey, pxOutputPrivKey, pxPubKeyX, pxPubKeyY);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EcdsaSign(TSecEcParams xParams, TSecHashType xHashType, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxMessage, size_t xMessageSize, TUnsignedInt8* pxSigR, TUnsignedInt8* pxSigS)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEcdsaSign(xParams, xHashType, pxPrivKey, pxMessage, xMessageSize, pxSigR, pxSigS);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EcdsaVerify(TSecEcParams xParams, TSecHashType xHashType, const TUnsignedInt8* pxPubKeyX, const TUnsignedInt8* pxPubKeyY, const TUnsignedInt8* pxMessage, size_t xMessageSize, const TUnsignedInt8* pxSigR, const TUnsignedInt8* pxSigS)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEcdsaVerify(xParams, xHashType, pxPubKeyX, pxPubKeyY, pxMessage, xMessageSize, pxSigR, pxSigS);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EcdhGenerateKey(TSecEcParams xParams, const TUnsignedInt8* pxInputPrivKey, TUnsignedInt8* pxOutputPrivKey, TUnsignedInt8* pxPubKeyX, TUnsignedInt8* pxPubKeyY)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEcdhGenerateKey(xParams, pxInputPrivKey, pxOutputPrivKey, pxPubKeyX, pxPubKeyY);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EcdhComputeKey(TSecEcParams xParams, const TUnsignedInt8* pxPrivKey, const TUnsignedInt8* pxOtherPubKeyX, const TUnsignedInt8* pxOtherPubKeyY, TUnsignedInt8* pxSharedSecret)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEcdhComputeKey(xParams, pxPrivKey, pxOtherPubKeyX, pxOtherPubKeyY, pxSharedSecret);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_RsaComputeCrtParams(size_t xKeySize, TUnsignedInt32 xE, const TUnsignedInt8* pxP, const TUnsignedInt8* pxQ, TUnsignedInt8* pxDP, TUnsignedInt8* pxDQ, TUnsignedInt8* pxQInv)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secRsaComputeCrtParams(xKeySize, xE, pxP, pxQ, pxDP, pxDQ, pxQInv);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_EncryptFlashProtKey(const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secEncryptFlashProtKey(pxInput, pxOutput, xSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_UseFlashProtKey(TSecCipherSession xSession, TUnsignedInt16 xEmi)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secUseFlashProtKey(xSession, xEmi);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

#if defined(_NA_SEC_USE_LAGERCYKEY_FUNC)
static TSecStatus _caSec_UseLegacyKey(TSecCipherSession xSession)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secUseLegacyKey(xSession);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}
#endif

static TSecStatus _caSec_OpenRam2RamEncryptSession(TSecCipherSession* pxSession)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secOpenRam2RamEncryptSession(pxSession);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_OpenRam2RamDecryptSession(TSecCipherSession* pxSession)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secOpenRam2RamDecryptSession(pxSession);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_CloseSession(TSecCipherSession xSession)
{
	TSecStatus secStatus;
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	TTransportSessionId xTsId;
#endif
#if defined(CAS_ZAP_LOG)
{
	static int cntClose  = 0;
	HxLOG_Print("**************************************************************************** \n");
	HxLOG_Print("*\n");
	HxLOG_Print("\t CAS_ZAP : CA_SEC [%s:%d] ----- (%d) ----- Close  pxSession(0x%08x) ----------\n", __FUNCTION__, __LINE__, cntClose++, xSession);
	HxLOG_Print("*\n");
	HxLOG_Print("**************************************************************************** \n");
}
#endif

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secCloseSession(xSession);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if(_na_sec_GetTsIdByHandle(xSession, &xTsId) == ERR_OK)
	{
		if(brcmCaRemoveTransportSessionContext(xTsId) != 0)
		{
			HxLOG_Error("ERROR In [brcmCaRemoveTransportSessionContext]\n");
		}

		if(_na_sec_UnregisterSessionInfo(xSession) != ERR_OK)
		{
			HxLOG_Error("ERROR In [_na_sec_UnregisterSessionInfo]\n");
		}
	}
#endif

	return secStatus;
}

static TSecStatus _caSec_SetClearTextKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xClearTextKeySize, const TUnsignedInt8* pxClearTextKey)
{
	TSecStatus secStatus;

#if defined(CAS_ZAP_LOG)
{
	static int cntKey  = 0;
	HxLOG_Print("**************************************************************************** \n");
	HxLOG_Print("*\n");
	HxLOG_Print("\t CAS_ZAP : CA SEC[%s:%d] ----- (%d) ----- Key  pxSession(0x%08x) ----------\n", __FUNCTION__, __LINE__, cntKey++, xSession);
	HxLOG_Print("*\n");
	HxLOG_Print("**************************************************************************** \n");
}
#endif


	HxLOG_Print("[%s:%d] called : xSession(0x%08x)\n", __FUNCTION__, __HxLINE__, xSession);

	HxLOG_Print("xSession[%p], EMI:[0x%x], KeyIdSize [%d], KeyId[%p], KeySize[%d], Keys[%p]\n", xSession, xEmi, xKeyIdSize, pxKeyId, xClearTextKeySize, pxClearTextKey);
	//HLIB_LOG_Dump(pxClearTextKey, xClearTextKeySize, 0, FALSE);

	secStatus = secSetClearTextKey(xSession, xEmi, xKeyIdSize, pxKeyId, xClearTextKeySize, pxClearTextKey);

	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	else
	{
		local_na_sec_CallKeyDataInfoCallback(xSession, xEmi, xKeyIdSize, pxKeyId, ePRM_KEYTYPE_CLEARTEXTKEY, xClearTextKeySize, pxClearTextKey);
	}
#endif

	return secStatus;
}

static TSecStatus _caSec_UseCertKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called : xSession(0x%08x)\n", __FUNCTION__, __HxLINE__, xSession);

	secStatus = secUseCertKey(xSession, xEmi, xKeyIdSize, pxKeyId);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	else
	{
		local_na_sec_CallKeyDataInfoCallback(xSession, xEmi, xKeyIdSize, pxKeyId, ePRM_KEYTYPE_USECERTKEY, 0, NULL);
	}
#endif

	return secStatus;
}

static TSecStatus _caSec_SessionEncrypt(TSecCipherSession xSession, const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xMessageSize, const TUnsignedInt8* pxInitVector, size_t xInitVectorSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSessionEncrypt(xSession, pxInput, pxOutput, xMessageSize, pxInitVector, xInitVectorSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_SessionDecrypt(TSecCipherSession xSession, const TUnsignedInt8* pxInput, TUnsignedInt8* pxOutput, size_t xMessageSize, const TUnsignedInt8* pxInitVector, size_t xInitVectorSize)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSessionDecrypt(xSession, pxInput, pxOutput, xMessageSize, pxInitVector, xInitVectorSize);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TSecStatus _caSec_OpenStreamEncryptSession(TSecCipherSession* pxSession, TTransportSessionId xTransportSessionId)
{
	TSecStatus	secStatus;
#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	Handle_t	hAction;
#endif
	TTransportSessionId xTsId = 0;

	HxLOG_Print("[%s:%d] called \n", __FUNCTION__, __HxLINE__);

	if(xTransportSessionId == TRANSPORT_SESSION_ID_INVALID)
	{
		HxLOG_Error("xTransportSessionId is TRANSPORT_SESSION_ID_INVALID\n");
		return SEC_ERROR_BAD_PARAMETER;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if(pxSession != NULL)
	{
		if(brcmCaAddTransportSessionContext(xTransportSessionId, NULL, NULL, NULL, NULL) != 0 /* SESSION_MANAGER_NO_ERROR */)
		{
			HxLOG_Error("brcmCaAddTransportSessionContext() error(%d), xTsId(%x)\n", xTsId);
			return ERR_FAIL;
		}
	}
#else
#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] open encryption \n", __FUNCTION__, __LINE__);
	if(CA_NA_CaSessionMgrIsTsIdSetDirect(xTransportSessionId) != TRUE)
#else
	if(CA_NA_CaSessionMgrIsTsIdSetDirect() != TRUE)
#endif
	{
		hAction = (Handle_t)xTransportSessionId;
		if(svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, (HUINT32 *)&xTsId) != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcRmGetResourceId() error\n");
		}
		HxLOG_Print("[%s:%d] No Direct \n", __FUNCTION__, __LINE__);
	}
	else
#endif
	{
		xTsId = xTransportSessionId;
		HxLOG_Print("[%s:%d] Direct \n", __FUNCTION__, __LINE__);
	}

	secStatus = secOpenStreamEncryptSession(pxSession, xTsId);

	HxLOG_Print("[%s:%d] ----- pxSession(0x%08x), xTransportSessionId(%d)\n", __FUNCTION__, __HxLINE__,  *pxSession, xTransportSessionId);

	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if(pxSession != NULL)
		_na_sec_RegisterSessionInfo(*pxSession, xTsId);
#endif

	return secStatus;
}

static TSecStatus _caSec_OpenStreamDecryptSession(TSecCipherSession* pxSession, TTransportSessionId xTransportSessionId)
{
	TSecStatus secStatus;
#if !defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	Handle_t	hAction;
#endif
	TTransportSessionId xTsId = 0;

	HxLOG_Print("[%s:%d] called *pxSession(0x%08x), xTransportSessionId(%d)\n", __FUNCTION__, __HxLINE__, *pxSession, xTransportSessionId);

	if(xTransportSessionId == TRANSPORT_SESSION_ID_INVALID)
	{
		HxLOG_Error("xTransportSessionId is TRANSPORT_SESSION_ID_INVALID\n");
		return SEC_ERROR_BAD_PARAMETER;
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if(pxSession != NULL)
	{
		if(brcmCaAddTransportSessionContext(xTransportSessionId, NULL, NULL, NULL, NULL) != 0 /* SESSION_MANAGER_NO_ERROR */)
		{
			HxLOG_Error("brcmCaAddTransportSessionContext() error(%d), xTsId(%x)\n", xTsId);
			return ERR_FAIL;
		}
	}
#else
#if defined(MULTI_DIRECT_TSID)
	HxLOG_Print("[%s:%d] open decryption\n", __FUNCTION__, __LINE__);
	if(CA_NA_CaSessionMgrIsTsIdSetDirect(xTransportSessionId) != TRUE)
#else
	if(CA_NA_CaSessionMgrIsTsIdSetDirect() != TRUE)
#endif
	{
		hAction = (Handle_t)xTransportSessionId;
		if(svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, (HUINT32 *)&xTsId) != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcRmGetResourceId() error\n");
		}
		HxLOG_Print("[%s:%d] No Direct\n", __FUNCTION__, __LINE__);
	}
	else
#endif
	{
		xTsId = xTransportSessionId;
		HxLOG_Print("[%s:%d] Direct\n", __FUNCTION__, __LINE__);
	}

	secStatus = secOpenStreamDecryptSession(pxSession, xTsId);


#if defined(CAS_ZAP_LOG)
{
	static int open  = 0;
	HxLOG_Print("**************************************************************************** \n");
	HxLOG_Print("*\n");
	HxLOG_Print("\t CAS_ZAP : CA SEC [%s:%d] ----- (%d) ----- Open tsId[%d] pxSession(0x%08x) ----------\n", __FUNCTION__, __LINE__, open++, xTsId, *pxSession);
	HxLOG_Print("*\n");
	HxLOG_Print("**************************************************************************** \n");
}
#endif


	HxLOG_Print("[%s:%d] pxSession(0x%08x), secStatus(%d)\n", __FUNCTION__, __LINE__, pxSession, secStatus);
	
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if(pxSession != NULL)
		_na_sec_RegisterSessionInfo(*pxSession, xTsId);
#endif

	return secStatus;
}

#if defined(_NA_SEC_USE_SET0LEVEL_PROTECTEDKEY_FUNC)
static TSecStatus _caSec_Set0LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSet0LevelProtectedKey(xSession, xEmi, xKeyIdSize, pxKeyId, xCipheredContentKeySize, pxCipheredContentKey);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	else
	{
		local_na_sec_CallKeyDataInfoCallback(xSession, xEmi, xKeyIdSize, pxKeyId, ePRM_KEYTYPE_SET0LEVELPROTECTEDKEY, 0, NULL);
	}
#endif

	return secStatus;
}
#endif

static TSecStatus _caSec_Set1LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey, size_t xCipheredProtectingKeySize, const TUnsignedInt8* pxL1CipheredProtectingKey)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	HxLOG_Print("xSession[%p], EMI:[0x%x], KeyIdSize [%d], KeyId[%p]\n", xSession, xEmi, xKeyIdSize, pxKeyId);

	secStatus = secSet1LevelProtectedKey(xSession, xEmi, xKeyIdSize, pxKeyId, xCipheredContentKeySize, pxCipheredContentKey, xCipheredProtectingKeySize, pxL1CipheredProtectingKey);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	else
	{
		local_na_sec_CallKeyDataInfoCallback(xSession, xEmi, xKeyIdSize, pxKeyId, ePRM_KEYTYPE_SET1LEVELPROTECTEDKEY, 0, NULL);
	}
#endif

	return secStatus;
}

static TSecStatus _caSec_Set2LevelProtectedKey(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, size_t xCipheredContentKeySize, const TUnsignedInt8* pxCipheredContentKey, size_t xCipheredProtectingKeySize, const TUnsignedInt8* pxL1CipheredProtectingKey, const TUnsignedInt8* pxL2CipheredProtectingKey)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secSet2LevelProtectedKey(xSession, xEmi, xKeyIdSize, pxKeyId, xCipheredContentKeySize, pxCipheredContentKey, xCipheredProtectingKeySize, pxL1CipheredProtectingKey, pxL2CipheredProtectingKey);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}
#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	else
	{
		local_na_sec_CallKeyDataInfoCallback(xSession, xEmi, xKeyIdSize, pxKeyId, ePRM_KEYTYPE_SET2LEVELPROTECTEDKEY, 0, NULL);
	}
#endif

	return secStatus;
}

static TSecStatus _caSec_SetMetadata(TSecCipherSession xSession, size_t xMetadataSize, const TUnsignedInt8* pxMetadata)
{
	TSecStatus secStatus;

#if defined(CAS_ZAP_LOG)
{
	static int cntClose  = 0;
	HxLOG_Print("**************************************************************************** \n");
	HxLOG_Print("*\n");
	HxLOG_Print("\t CAS_ZAP : CA SEC [%s:%d] ----- (%d) ----- meta  pxSession(0x%08x), xMetadataSize(%d) ----------\n", __FUNCTION__, __LINE__, cntClose++, xSession, xMetadataSize);
	HxLOG_Print("*\n");
	HxLOG_Print("**************************************************************************** \n");
}
#endif

	HxLOG_Print("[%s:%d] called : xSession(0x%08x)\n", __FUNCTION__, __HxLINE__, xSession);

	secStatus = secSetMetadata(xSession, xMetadataSize, pxMetadata);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

static TUnsignedInt8* _caSec_AllocateBuffer(size_t xBufferSize)
{
	TUnsignedInt8 *pxBuffer = NULL;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	pxBuffer = secAllocateBuffer(xBufferSize);
	if(pxBuffer == NULL)
	{
		HxLOG_Error("pxBuffer is NULL\n");
	}

	return pxBuffer;
}

static TSecStatus _caSec_FreeBuffer(TUnsignedInt8 *pxBuffer)
{
	TSecStatus secStatus;

	HxLOG_Print("[%s:%d] called\n", __FUNCTION__, __HxLINE__);

	secStatus = secFreeBuffer(pxBuffer);
	if(secStatus != SEC_NO_ERROR)
	{
		HxLOG_Error("ERROR (%d)\n", secStatus);
	}

	return secStatus;
}

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
STATIC NaSecSessionInfo_t *_na_sec_GetSessionInfoTable(void)
{
	return s_astSecSessionInfo;
}

STATIC NaSecSessionInfo_t *_na_sec_GetSessionInfoByHandle(TSecCipherSession xSession)
{
	HUINT32 i;
	NaSecSessionInfo_t *pstSessionInfo, *pstSessionInfoTable = _na_sec_GetSessionInfoTable();

	for(i = 0; i < SEC_SESSION_MAX_NUM_FOR_DAL_DAL; i ++)
	{
		pstSessionInfo = &pstSessionInfoTable[i];

		if(pstSessionInfo->bRegistered == TRUE && pstSessionInfo->xSession == xSession)
		{
			return pstSessionInfo;
		}
	}

	return NULL;
}

STATIC void _na_sec_InitSecSessionInfo(void)
{
	NaSecSessionInfo_t *pstSessionInfoTable = _na_sec_GetSessionInfoTable();

	HxSTD_memset(pstSessionInfoTable, 0, sizeof(NaSecSessionInfo_t) * SEC_SESSION_MAX_NUM_FOR_DAL_DAL);
}

STATIC HERROR _na_sec_RegisterSessionInfo(TSecCipherSession xSession, TTransportSessionId xTsId)
{
	HUINT32 i;
	NaSecSessionInfo_t *pstSessionInfo, *pstSessionInfoTable = _na_sec_GetSessionInfoTable();

	HxLOG_Print("[%s:%d] called, xSession[0x%x], xTsId[%d]\n", __FUNCTION__, __HxLINE__, xSession, xTsId);

	for(i = 0; i < SEC_SESSION_MAX_NUM_FOR_DAL_DAL; i ++)
	{
		pstSessionInfo = &pstSessionInfoTable[i];

		if(pstSessionInfo->bRegistered != TRUE)
		{
			pstSessionInfo->xSession = xSession;
			pstSessionInfo->xTransportSessionId = xTsId;
			pstSessionInfo->bRegistered = TRUE;

			return ERR_OK;
		}
	}

	HxLOG_Error("ERROR Not enough Session buffer\n");
	return ERR_FAIL;
}

STATIC HERROR _na_sec_UnregisterSessionInfo(TSecCipherSession xSession)
{
	NaSecSessionInfo_t *pstSessionInfo;

	HxLOG_Print("[%s:%d] called, xSession[0x%x]\n", __FUNCTION__, __HxLINE__, xSession);

	pstSessionInfo = _na_sec_GetSessionInfoByHandle(xSession);

	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("ERROR There is no Session info mached session[0x%x]\n", xSession);
		return ERR_FAIL;
	}

	HxSTD_memset(pstSessionInfo, 0, sizeof(NaSecSessionInfo_t));

	return ERR_OK;
}

STATIC  HERROR _na_sec_GetTsIdByHandle(TSecCipherSession xSession, TTransportSessionId *pxTsId)
{
	NaSecSessionInfo_t *pstSessionInfo;

	HxLOG_Print("[%s:%d] called, xSession[0x%x]\n", __FUNCTION__, __HxLINE__, xSession);

	pstSessionInfo = _na_sec_GetSessionInfoByHandle(xSession);

	if(pstSessionInfo == NULL)
	{
		HxLOG_Error("ERROR There is no Session info mached session[0x%x]\n", xSession);
		return ERR_FAIL;
	}

	*pxTsId = pstSessionInfo->xTransportSessionId;

	return ERR_OK;
}
#endif

#if defined(_NA_NOCS_OVERRIDE_FUNCTION)
STATIC HERROR _na_sec_InitFunctionTable(void)
{
	TSecFunctionTable* secFunctionTable = secGetFunctionTable();

	secFunctionTable->secApiVersion = SECAPI_VERSION_INT;
	/**< Version of the SEC API. Use the macro SECAPI_VERSION_INT to assign the right value.
	 */
	secFunctionTable->secGetNuid = _caSec_GetNuid;
	/**< Get chipset NUID (32 bits)
	  */
	secFunctionTable->secGetChipsetRevision = _caSec_GetChipsetRevision;
	/**< Get chipset revision
	  */
	secFunctionTable->secEncryptData = _caSec_EncryptData;
	/**< RAM2RAM encryption
	  */
	secFunctionTable->secDecryptData = _caSec_DecryptData;
	/**< RAM2RAM decryption
	  */
	secFunctionTable->secGenerateRandomBytes = _caSec_GenerateRandomBytes;
	/**< Generate a sequence of random bytes
	  */
	secFunctionTable->secRsaGenerateKey = _caSec_RsaGenerateKey;
	/**< Generate RSA private key
	  */
	secFunctionTable->secRsaPublicEncrypt = _caSec_RsaPublicEncrypt;
	/**< RSA encryption using public key
	  */
	secFunctionTable->secRsaPrivateEncrypt = _caSec_RsaPrivateEncrypt;
	/**< RSA encryption using private key
	  */
	secFunctionTable->secRsaPublicDecrypt = _caSec_RsaPublicDecrypt;
	/**< RSA decryption using public key
	  */
	secFunctionTable->secRsaPrivateDecrypt = _caSec_RsaPrivateDecrypt;
	/**< RSA decryption using private key
	  */
	secFunctionTable->secDhGenerateKey = _caSec_DhGenerateKey;
	/**< Generate Diffie-Hellman keys
	  */
	secFunctionTable->secDhComputeKey = _caSec_DhComputeKey;
	/**< Compute Diffie-Hellman shared secret
	  */
	secFunctionTable->secSha1Init = _caSec_Sha1Init;
	/**< Initialize SHA-1 message digest
	  */
	secFunctionTable->secSha1Update = _caSec_Sha1Update;
	/**< Update SHA-1 message digest
	  */
	secFunctionTable->secSha1Final = _caSec_Sha1Final;
	/**< Finalize SHA-1 message digest
	  */
	secFunctionTable->secSha256Init = _caSec_Sha256Init;
	/**< Initialize SHA-256 message digest
	  */
	secFunctionTable->secSha256Update = _caSec_Sha256Update;
	/**< Update SHA-256 message digest
	  */
	secFunctionTable->secSha256Final = _caSec_Sha256Final;
	/**< Finalize SHA-256 message digest
	  */
	secFunctionTable->secHmacSha256Init = _caSec_HmacSha256Init;
	/**< Initialize HMAC-SHA-256 message digest
	  */
	secFunctionTable->secHmacSha256Update = _caSec_HmacSha256Update;
	/**< Update HMAC-SHA-256 message digest
	  */
	secFunctionTable->secHmacSha256Final = _caSec_HmacSha256Final;
	/**< Finalize HMAC-SHA-256 message digest
	  */
	secFunctionTable->secEcdsaGenerateKey = _caSec_EcdsaGenerateKey;
	/**< Generate an ECDSA key pair
	  */
	secFunctionTable->secEcdsaSign = _caSec_EcdsaSign;
	/**< Sign a message with ECDSA
	  */
	secFunctionTable->secEcdsaVerify = _caSec_EcdsaVerify;
	/**< Verify an ECDSA signature
	  */
	secFunctionTable->secEcdhGenerateKey = _caSec_EcdhGenerateKey;
	/**< Generate ECDH public/private key pair
	  */
	secFunctionTable->secEcdhComputeKey = _caSec_EcdhComputeKey;
	/**< Compute ECDH shared secret key
	  */
	secFunctionTable->secOpenRam2RamEncryptSession = _caSec_OpenRam2RamEncryptSession;
	/**< Open a RAM2RAM encryption session
	  */
	secFunctionTable->secOpenRam2RamDecryptSession = _caSec_OpenRam2RamDecryptSession;
	/**< Open a RAM2RAM decryption session
	  */
	secFunctionTable->secCloseSession = _caSec_CloseSession;
	/**< Close a block cipher encryption or decryption session
	  */
	secFunctionTable->secSetClearTextKey = _caSec_SetClearTextKey;
	/**< Set a clear-text key in a RAM2RAM key table
	  */
	secFunctionTable->secSet2LevelProtectedKey = _caSec_Set2LevelProtectedKey;
	/**< Set protected keys in a 2-level key ladder crypto-engine
	  */
	secFunctionTable->secUseCertKey = _caSec_UseCertKey;
	/**< Use the CERT key for RAM2RAM operations
	  */
	secFunctionTable->secSessionEncrypt = _caSec_SessionEncrypt;
	/**< Perform RAM2RAM encryption operations
	  */
	secFunctionTable->secSessionDecrypt = _caSec_SessionDecrypt;
	/**< Perform RAM2RAM decryption operations
	  */
	secFunctionTable->secGetNuid64 = _caSec_GetNuid64;
	/**< Get chipset NUID (64 bits)
	  */
	secFunctionTable->secGetChipsetExtension = _caSec_GetChipsetExtension;
	/**< Get chipset extension
	  */
	secFunctionTable->secSha384Init = _caSec_Sha384Init;
	/**< Initialize SHA-384 message digest
	  */
	secFunctionTable->secSha384Update = _caSec_Sha384Update;
	/**< Update SHA-384 message digest
	  */
	secFunctionTable->secSha384Final = _caSec_Sha384Final;
	/**< Finalize SHA-384 message digest
	  */
	secFunctionTable->secRsaComputeCrtParams = _caSec_RsaComputeCrtParams;
	/**< Computes RSA CRT parameters dP, dQ and qInv.
	  */
	secFunctionTable->secEncryptFlashProtKey = _caSec_EncryptFlashProtKey;
	/**< Encrypt the flash protection key with the flash protection root key
	  */
	secFunctionTable->secUseFlashProtKey = _caSec_UseFlashProtKey;
	/**< Use the flash protection key for RAM2RAM operations
	  */
	secFunctionTable->secOpenStreamEncryptSession = _caSec_OpenStreamEncryptSession;
	/**< Open a stream encryption session
	  */
	secFunctionTable->secOpenStreamDecryptSession = _caSec_OpenStreamDecryptSession;
	/**< Open a stream decryption session
	  */
#if defined(_NA_SEC_USE_SET0LEVEL_PROTECTEDKEY_FUNC)
	secFunctionTable->secSet0LevelProtectedKey = _caSec_Set0LevelProtectedKey;
#endif
	/**< Set protected keys in a 0-level key ladder crypto-engine
	  */
	secFunctionTable->secSet1LevelProtectedKey = _caSec_Set1LevelProtectedKey;
	/**< Set protected keys in a 1-level key ladder crypto-engine
	  */
	secFunctionTable->secSetMetadata = _caSec_SetMetadata;
	/**< Associate metadata to a stream encryption/decryption session
	  */
	secFunctionTable->secAllocateBuffer = _caSec_AllocateBuffer;
	/**< Allocate a cryptoprocessor compliant buffer
	  */
	secFunctionTable->secFreeBuffer = _caSec_FreeBuffer;
	/**< Free a cryptoprocessor compliant buffer
	  */
#if defined(_NA_SEC_USE_LAGERCYKEY_FUNC)
	secFunctionTable->secUseLegacyKey = _caSec_UseLegacyKey;
#endif
	/**< Use the legacy secret key
	  */
	return ERR_OK;
}
#else
STATIC HERROR _na_sec_InitFunctionTable(void)
{
	HxSTD_memset(&s_stSecFunctionTable, 0, sizeof(TSecFunctionTable));

	s_stSecFunctionTable.secApiVersion = SECAPI_VERSION_INT;
	/**< Version of the SEC API. Use the macro SECAPI_VERSION_INT to assign the right value.
	 */
	s_stSecFunctionTable.secGetNuid = _caSec_GetNuid;
	/**< Get chipset NUID (32 bits)
	  */
	s_stSecFunctionTable.secGetChipsetRevision = _caSec_GetChipsetRevision;
	/**< Get chipset revision
	  */
	s_stSecFunctionTable.secEncryptData = _caSec_EncryptData;
	/**< RAM2RAM encryption
	  */
	s_stSecFunctionTable.secDecryptData = _caSec_DecryptData;
	/**< RAM2RAM decryption
	  */
	s_stSecFunctionTable.secGenerateRandomBytes = _caSec_GenerateRandomBytes;
	/**< Generate a sequence of random bytes
	  */
	s_stSecFunctionTable.secRsaGenerateKey = _caSec_RsaGenerateKey;
	/**< Generate RSA private key
	  */
	s_stSecFunctionTable.secRsaPublicEncrypt = _caSec_RsaPublicEncrypt;
	/**< RSA encryption using public key
	  */
	s_stSecFunctionTable.secRsaPrivateEncrypt = _caSec_RsaPrivateEncrypt;
	/**< RSA encryption using private key
	  */
	s_stSecFunctionTable.secRsaPublicDecrypt = _caSec_RsaPublicDecrypt;
	/**< RSA decryption using public key
	  */
	s_stSecFunctionTable.secRsaPrivateDecrypt = _caSec_RsaPrivateDecrypt;
	/**< RSA decryption using private key
	  */
	s_stSecFunctionTable.secDhGenerateKey = _caSec_DhGenerateKey;
	/**< Generate Diffie-Hellman keys
	  */
	s_stSecFunctionTable.secDhComputeKey = _caSec_DhComputeKey;
	/**< Compute Diffie-Hellman shared secret
	  */
	s_stSecFunctionTable.secSha1Init = _caSec_Sha1Init;
	/**< Initialize SHA-1 message digest
	  */
	s_stSecFunctionTable.secSha1Update = _caSec_Sha1Update;
	/**< Update SHA-1 message digest
	  */
	s_stSecFunctionTable.secSha1Final = _caSec_Sha1Final;
	/**< Finalize SHA-1 message digest
	  */
	s_stSecFunctionTable.secSha256Init = _caSec_Sha256Init;
	/**< Initialize SHA-256 message digest
	  */
	s_stSecFunctionTable.secSha256Update = _caSec_Sha256Update;
	/**< Update SHA-256 message digest
	  */
	s_stSecFunctionTable.secSha256Final = _caSec_Sha256Final;
	/**< Finalize SHA-256 message digest
	  */
	s_stSecFunctionTable.secHmacSha256Init = _caSec_HmacSha256Init;
	/**< Initialize HMAC-SHA-256 message digest
	  */
	s_stSecFunctionTable.secHmacSha256Update = _caSec_HmacSha256Update;
	/**< Update HMAC-SHA-256 message digest
	  */
	s_stSecFunctionTable.secHmacSha256Final = _caSec_HmacSha256Final;
	/**< Finalize HMAC-SHA-256 message digest
	  */
	s_stSecFunctionTable.secEcdsaGenerateKey = _caSec_EcdsaGenerateKey;
	/**< Generate an ECDSA key pair
	  */
	s_stSecFunctionTable.secEcdsaSign = _caSec_EcdsaSign;
	/**< Sign a message with ECDSA
	  */
	s_stSecFunctionTable.secEcdsaVerify = _caSec_EcdsaVerify;
	/**< Verify an ECDSA signature
	  */
	s_stSecFunctionTable.secEcdhGenerateKey = _caSec_EcdhGenerateKey;
	/**< Generate ECDH public/private key pair
	  */
	s_stSecFunctionTable.secEcdhComputeKey = _caSec_EcdhComputeKey;
	/**< Compute ECDH shared secret key
	  */
	s_stSecFunctionTable.secOpenRam2RamEncryptSession = _caSec_OpenRam2RamEncryptSession;
	/**< Open a RAM2RAM encryption session
	  */
	s_stSecFunctionTable.secOpenRam2RamDecryptSession = _caSec_OpenRam2RamDecryptSession;
	/**< Open a RAM2RAM decryption session
	  */
	s_stSecFunctionTable.secCloseSession = _caSec_CloseSession;
	/**< Close a block cipher encryption or decryption session
	  */
	s_stSecFunctionTable.secSetClearTextKey = _caSec_SetClearTextKey;
	/**< Set a clear-text key in a RAM2RAM key table
	  */
	s_stSecFunctionTable.secSet2LevelProtectedKey = _caSec_Set2LevelProtectedKey;
	/**< Set protected keys in a 2-level key ladder crypto-engine
	  */
	s_stSecFunctionTable.secUseCertKey = _caSec_UseCertKey;
	/**< Use the CERT key for RAM2RAM operations
	  */
	s_stSecFunctionTable.secSessionEncrypt = _caSec_SessionEncrypt;
	/**< Perform RAM2RAM encryption operations
	  */
	s_stSecFunctionTable.secSessionDecrypt = _caSec_SessionDecrypt;
	/**< Perform RAM2RAM decryption operations
	  */
	s_stSecFunctionTable.secGetNuid64 = _caSec_GetNuid64;
	/**< Get chipset NUID (64 bits)
	  */
	s_stSecFunctionTable.secGetChipsetExtension = _caSec_GetChipsetExtension;
	/**< Get chipset extension
	  */
	s_stSecFunctionTable.secSha384Init = _caSec_Sha384Init;
	/**< Initialize SHA-384 message digest
	  */
	s_stSecFunctionTable.secSha384Update = _caSec_Sha384Update;
	/**< Update SHA-384 message digest
	  */
	s_stSecFunctionTable.secSha384Final = _caSec_Sha384Final;
	/**< Finalize SHA-384 message digest
	  */
	s_stSecFunctionTable.secRsaComputeCrtParams = _caSec_RsaComputeCrtParams;
	/**< Computes RSA CRT parameters dP, dQ and qInv.
	  */
	s_stSecFunctionTable.secEncryptFlashProtKey = _caSec_EncryptFlashProtKey;
	/**< Encrypt the flash protection key with the flash protection root key
	  */
	s_stSecFunctionTable.secUseFlashProtKey = _caSec_UseFlashProtKey;
	/**< Use the flash protection key for RAM2RAM operations
	  */
	s_stSecFunctionTable.secOpenStreamEncryptSession = _caSec_OpenStreamEncryptSession;
	/**< Open a stream encryption session
	  */
	s_stSecFunctionTable.secOpenStreamDecryptSession = _caSec_OpenStreamDecryptSession;
	/**< Open a stream decryption session
	  */
	s_stSecFunctionTable.secSet0LevelProtectedKey = _caSec_Set0LevelProtectedKey;
	/**< Set protected keys in a 0-level key ladder crypto-engine
	  */
	s_stSecFunctionTable.secSet1LevelProtectedKey = _caSec_Set1LevelProtectedKey;
	/**< Set protected keys in a 1-level key ladder crypto-engine
	  */
	s_stSecFunctionTable.secSetMetadata = _caSec_SetMetadata;
	/**< Associate metadata to a stream encryption/decryption session
	  */
	s_stSecFunctionTable.secAllocateBuffer = _caSec_AllocateBuffer;
	/**< Allocate a cryptoprocessor compliant buffer
	  */
	s_stSecFunctionTable.secFreeBuffer = _caSec_FreeBuffer;
	/**< Free a cryptoprocessor compliant buffer
	  */
	s_stSecFunctionTable.secUseLegacyKey = _caSec_UseLegacyKey;
	/**< Use the legacy secret key
	  */
	return ERR_OK;
}
#endif

#if !defined (_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
static void local_na_sec_InitCallback(void)
{
	HxSTD_memset(&s_stSecKeyDataCallback, 0, sizeof(NaPrmKeyInfoCallback_t));
}

static void local_na_sec_CallKeyDataInfoCallback(TSecCipherSession xSession, TUnsignedInt16 xEmi, size_t xKeyIdSize, TUnsignedInt8* pxKeyId, NaPrmKeyType_e eKeyType, size_t xSize, const TUnsignedInt8* pxKey)
{
	TUnsignedInt32		nReturn;
	NaPrmKeyInfo_t		stKeyInfo;
	TTransportSessionId	xTSid;

	/* VOD Service 일 경우 imma에서 decryption 동작을 하므로 key 정보를 TV magager 단으로 올려 보내 줌, 일단 PVR일경우에 대해서는 고려치 않음. */
	if(s_stSecKeyDataCallback.pfnKeyDataCallback != NULL)
	{
		stKeyInfo.usEmi = xEmi;
		stKeyInfo.xTSid = s_stSecKeyDataCallback.xTSid;
		stKeyInfo.ulPlaybackHandle = s_stSecKeyDataCallback.ulPlaybackHandle;
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
		if(xEmi != 0x4023) /* 현재는 HLS에 대한 것만 고려하고 있다. 나중에 필요 시 더 추가 */
		{
			HxLOG_Debug("No need to notify the key info (%x)\n", xEmi);
			return;
		}

		if(_na_sec_GetTsIdByHandle(xSession, &xTSid) != ERR_OK)
		{
			HxLOG_Error("_na_sec_GetTsIdByHandle() xSession(%x)\n", xSession);
		}
#else
		xTSid = s_stSecKeyDataCallback.xTSid;
#endif

		nReturn = brcmCaGetM2MKeyslotHandle(xTSid, &stKeyInfo.ulKeyHandler);
		if(nReturn)
		{
			HxLOG_Error("brcmCaGetM2MKeyslotHandle() error(%d), xTsId(%x)\n", nReturn, xTSid);
			return;
		}
		stKeyInfo.ulKeyIdSize = xKeyIdSize;
		stKeyInfo.pucKeyId = pxKeyId;
		stKeyInfo.eKeyType = eKeyType;

		stKeyInfo.ulKeyLength = 0;
		HxSTD_memset(&stKeyInfo.ucKey[0], 0, sizeof(stKeyInfo.ucKey));

		if(pxKey != NULL && xSize > 0)
		{
			stKeyInfo.ulKeyLength = xSize;
			HxSTD_memcpy(&stKeyInfo.ucKey[0], pxKey, xSize);
		}

		s_stSecKeyDataCallback.pfnKeyDataCallback((void *)&stKeyInfo);
	}

}
#endif

#if !defined(_NA_NOCS_OVERRIDE_FUNCTION)
TSecFunctionTable* secGetFunctionTable(void)
{
 	return &s_stSecFunctionTable;
}
#endif

HERROR CAS_NA_SecInitialize(void)
{
	HINT32 i;
	HCHAR szSemName[32];

	if(_na_sec_InitFunctionTable() != ERR_OK)
	{
		HxLOG_Error("Error in _na_sec_InitFunctionTable()\n");
		return ERR_FAIL;
	}

	if(VK_SEM_Create((unsigned long *)&s_ulSecSemId, "CaSecSem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Error("Error in VK_SEM_Create()\n");
		return ERR_FAIL;
	}

	for(i = 0 ; i < eHashSem_MAX ; i++)
	{
		snprintf(szSemName, 32, "CaSecHash%02d", i);
		if(VK_SEM_Create((unsigned long *)&s_aulHashSemId[i], szSemName, VK_SUSPENDTYPE_FIFO) != VK_OK)
		{
			HxLOG_Error("Error in VK_SEM_Create() - %d\n", i);
			return ERR_FAIL;
		}
	}

#if !defined (_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
	local_na_sec_InitCallback();
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	_na_sec_InitSecSessionInfo();
#endif

	return ERR_OK;
}

#if !defined(_NA_PRM_KEY_NOTIFY_FROM_NEXUS_)
HERROR CAS_NA_SecRegisterKeyDataCallback(TTransportSessionId xTSid, HUINT32 ulPlaybackHandle, PrmKeyInfo_Calback_Function fnKeyDataCallback)
{
	s_stSecKeyDataCallback.xTSid = xTSid;
	s_stSecKeyDataCallback.ulPlaybackHandle = ulPlaybackHandle;
	s_stSecKeyDataCallback.pfnKeyDataCallback = fnKeyDataCallback;

	return ERR_OK;
}
#endif

#endif

