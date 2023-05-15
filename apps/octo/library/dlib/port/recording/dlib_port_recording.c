
#include <hlib.h>
#include "_dlib_port_recording.h"

#define	MACRO_RECORDING_STRLEN(s)				(sizeof(s)/sizeof(s[0])-1)

typedef	struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxRecording_Field_t;


static	DxEnumStringTable_t	_stRecStatusTable =
{
	(HCHAR*)DxNAMEOF(DxRecStatus_e),
	{
		{	"start",		eDxREC_STS_RECORD_START	},
		{	"recording",	eDxREC_STS_RECORDING	},
		{	"recorded",		eDxREC_STS_RECORDED		},
		{	"incompleted",	eDxREC_STS_INCOMPLETED	},
		{	NULL,								-1}
	}
};

static	DxEnumStringTable_t	_stRecTimestampTable =
{
	(HCHAR*)DxNAMEOF(DxRecTimestampType_e),
	{
		{	"pmt",		eDxREC_TIMESTAMP_PMT	},
		{	"eit",		eDxREC_TIMESTAMP_EIT	},
		{	"drm",		eDxREC_TIMESTAMP_DRM	},
		{	"license",	eDxREC_TIMESTAMP_LICENSE	},
		{	NULL,								-1}
	}
};


static	DxEnumStringTable_t	_stRecEndReasonTable =
{
	(HCHAR*)DxNAMEOF(DxRecEndReasonType_e),
	{
		{	"f-nofail",				eDxREC_FAILED_NOFAIL	},
		{	"f-hddfull",			eDxREC_FAILED_HDDFULL	},
		{	"f-maxnum",				eDxREC_FAILED_MAXNUM	},
		{	"f-maxrecnum", 			eDxREC_FAILED_MAXRECORDLISTNUM	},
		{	"f-less30", 			eDxREC_FAILED_LESS30SEC	},
		{	"f-nosignal",			eDxREC_FAILED_NOSIGNAL	},
		{	"f-nonservice",			eDxREC_FAILED_NONSERVICE},
		{	"f-search",				eDxREC_FAILED_SEARCHING	},

		{	"f-tpconf",				eDxREC_FAILED_CONFLICT_TP},
		{	"f-drm",				eDxREC_FAILED_DRM	},
		{	"f-noscscr",			eDxREC_FAILED_NOSC_SCR},
		{	"f-norightscr",			eDxREC_FAILED_NORIGHT_SCR	},

		{	"f-nohdd",				eDxREC_FAILED_NOHDD},
		{	"i-hddfull",			eDxREC_INCOMPLETE_HDDFULL	},
		{	"f-unknown",			eDxREC_FAILED_UNKNOWN},
		{	"f-nousbhdd",			eDxREC_FAILED_NOUSBHDD	},

		{	"f-discon-hdd",			eDxREC_FAILED_DISCONNECTUSBHDD},
		{	"f-extusb-copydel",		eDxREC_FAILED_EXTUSB_CopyDel	},
		{	"f-samesvc",			eDxREC_FAILED_SAME_SVC_RECORDING},
		{	"f-highprio",			eDxREC_FAILED_HIGHER_PRIORITY	},
		{	"f-unabletracking",		eDxREC_FAILED_UNABLE_TRACKING	},
		{	"f-noav",				eDxREC_FAILED_NO_VIDEO_AUDIO	},
		{	"f-sitimeout",			eDxREC_FAILED_SI_TIMEOUT	},
		{	"f-poweroff",			eDxREC_FAILED_POWER_OFF	},
		{	NULL,								-1}
	}
};

static	DxEnumStringTable_t	_stAudioTypeTable =
{
	(HCHAR*)DxNAMEOF(DxAUDIO_Type_e),
	{
		{	"undefined",			eDxAUDIO_TYPE_UNDEFINED	},
		{	"clean-effect",			eDxAUDIO_TYPE_CLEAN_EFFECTS	},
		{	"hear-impaired",		eDxAUDIO_TYPE_HEARING_IMPAIRED	},
		{	"visual-impaired",		eDxAUDIO_TYPE_VISUAL_IMPAIRED	},
		{	"reserved", 			eDxAUDIO_TYPE_RESERVED	},
		{	NULL,								-1}
	}
};

#if defined(CONFIG_OP_FREESAT)
static	DxEnumStringTable_t	_stFreesat_PVR_GuidanceTypeTable =
{
	(HCHAR*)DxNAMEOF(DxFreesat_PVR_GuidanceType_e),
	{
		{	"none",		eDxFSat_GUIDANCE_TYPE_NONE	},
		{	"max",		eDxFSat_GUIDANCE_TYPE_MAX	},
		{	NULL,								-1}
	}
};
#endif

#if defined(CONFIG_OP_UK_DTT)
static	DxEnumStringTable_t	_stUkdtt_PVR_GuidanceTypeTable =
{
	(HCHAR*)DxNAMEOF(DxUkdtt_PVR_GuidanceType_e),
	{
		{	"langcode",	eDxUkdtt_GUIDANCE_TYPE_LANGCODE	},
		{	"mode",		eDxUkdtt_GUIDANCE_TYPE_MODE_SPECIFIED	},
		{	"data",		eDxUkdtt_GUIDANCE_TYPE_DATA	},
		{	"max",		eDxUkdtt_GUIDANCE_TYPE_MAX	},
		{	NULL,								-1}
	}
};
#endif

/*   Encode functions for the store on the File System
	The format is based on OCTO META PVR (for the interopability ... HDR-1000S and ...) */
// json string decoding 안 되는 txt 들어올 경우를 대비 URL Encoding
static HERROR dlib_recording_encodeStrAsUrlEncoding(HINT32	sd, HCHAR	*pszFieldName, HCHAR	*pszStr)
{
	HUINT32		len = 0;
	HCHAR		*buf= NULL;

	len = HLIB_STD_URLStrLen(pszStr) + 1;
    buf = DLIB_Calloc(len+1);
    HLIB_STD_URLEncode(buf, pszStr, len);

	HLIB_RWSTREAM_Print(sd, "\"%s\":\"%s\"", 	pszFieldName, buf);

	DLIB_Free(buf);

	return ERR_OK;
}

HERROR		dlib_recording_EncodeRecData_RecInfo (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	HUINT32	i;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	// should extend the DxRecListData_t minimally
	HLIB_RWSTREAM_Print(sd, "{");				// start body
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"szUrl", 			pstRecInfo->szUrl);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucPath", 			pstRecInfo->stMeta.szPath);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucFileName", 			pstRecInfo->stMeta.szFileName);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulStartTime", 		pstRecInfo->ulRecStartTime);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulEndTime", 		pstRecInfo->stMeta.ulEndTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulRecTime", 		pstRecInfo->ulRecDuration);
#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulOrgDuration",	pstRecInfo->ulRecOrgDuration);
#endif

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucRecordStatus",	pstRecInfo->eRecStatus);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u2cEventBase", 	pstRecInfo->bEventBased);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"nEventId", 		pstRecInfo->nEventId);

#if defined (CONFIG_OP_SES)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucSCPinCodeFlag",		pstRecInfo->ucas.ucSCPinCodeFlag);
#endif

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1cLocked",		pstRecInfo->bBlocked);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1cWatched",		pstRecInfo->bPlayed);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1cScrambled", 	pstRecInfo->bScrambled);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1cEncryption",	pstRecInfo->bEncrypted);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1cAutoDeleted",	pstRecInfo->bAudoDeleted);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucEndCondition",	pstRecInfo->eRecEndReason);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulLastWatchtime",	pstRecInfo->ulLastViewTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bUserLocked",		pstRecInfo->bUserLock);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bWillBeDeleted",	pstRecInfo->bWillBeDeleted);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulChipId",	pstRecInfo->ulChipId);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bDoNotDelete",		pstRecInfo->bDoNotDelete);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulDoNotDeleteSetTime",	pstRecInfo->ulDoNotDeleteSetTime);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"astBookmarks");
	for (i = 0; i < pstRecInfo->usNumOfBookmark ; i++)
	{
		DxRecList_Bookmark_t	*pstBookmark = &pstRecInfo->astBookmarks[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",			"ulPos",		pstBookmark->ulPos);
		HLIB_RWSTREAM_Print(sd, ", ");
		dlib_recording_encodeStrAsUrlEncoding(sd, 	"szName", 		pstBookmark->szName);
		HLIB_RWSTREAM_Print(sd, "}");	// pstBookmark
	}
	HLIB_RWSTREAM_Print(sd, "]"); // astBokmark
	HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"astPausedPoints");
	for (i = 0; i < pstRecInfo->usNumOfPausedPoints ; i++)
	{
		DxRecList_Bookmark_t	*pstPausedPoint = &pstRecInfo->astPausedPoints[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",			"ulPos",		pstPausedPoint->ulPos);
		HLIB_RWSTREAM_Print(sd, ", ");
		dlib_recording_encodeStrAsUrlEncoding(sd, 	"szName", 		pstPausedPoint->szName);
		HLIB_RWSTREAM_Print(sd, "}");	// astPausedPoints
	}
	HLIB_RWSTREAM_Print(sd, "]"); // astPausedPoints
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucDisplayEvtName",pstRecInfo->stMeta.aucDisplayEvtName);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bSeriesRec",	pstRecInfo->bSeriesRec);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"szSeriesId",	pstRecInfo->stEvt.aucSeriesId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulEpisodeNum",	pstRecInfo->stEvt.ulEpisodeNum);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulSeason",		pstRecInfo->stEvt.ulSeason);

#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulFtaCmVal",		pstRecInfo->extension.freesat.ulFtaCmVal);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucCopyTime",		pstRecInfo->extension.freesat.ucCopyTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulTimeOffsetAtRec",pstRecInfo->extension.freesat.ulTimeOffsetAtRec);
#endif // CONFIG_OP_FREESAT
#if defined(CONFIG_OP_UK_DTT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",		"ulFtaCmVal",		pstRecInfo->extension.ukdtt.ulFtaCmVal);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",		"ucCopyTime",		pstRecInfo->extension.ukdtt.ucCopyTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",		"ulTimeOffsetAtRec",pstRecInfo->extension.ukdtt.ulTimeOffsetAtRec);
#endif // CONFIG_OP_UK_DTT

#if defined(CONFIG_MW_CI)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulInstType",	pstRecInfo->ucas.cas.ulInstType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulSlotId",		pstRecInfo->ucas.cas.ulSlotId);
  #if defined(CONFIG_MW_CI_PLUS_V_1_3)	// 해당 모델은 CI+ v1.3 지원.
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bCamNeedContent",	pstRecInfo->ucas.cas.ciplus.bCamNeedContent);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucBindedCamId",	pstRecInfo->ucas.cas.ciplus.aucBindedCamId);
  #endif
#endif // CONFIG_MW_CI_PLUS_V_1_3

#if 0 //defined(CONFIG_MW_CAS_NAGRA_OP_SES) // defined(CONFIG_APS_HD)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",		"ucSCPinCodeFlag",	pstRecInfo->ucas.ucSCPinCodeFlag);
#endif

#if defined (CONFIG_MW_CAS_CONAX)
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"szConaxMetaName", 	pstRecInfo->ucas.conax.szConaxMetaName);
#endif

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bTranscoded",		pstRecInfo->bTranscoded);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulRecMode",		pstRecInfo->ulRecMode);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"usTrRecState",		pstRecInfo->stMeta.usTrRecState);

	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}


#define DLIB_REC_ENCRYPT_DES
#define DLIB_REC_CRYPT_BLOCK_SIZE	8
static HUINT8 s_aucDes_Key[DLIB_REC_CRYPT_BLOCK_SIZE] = {0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6};


static const HCHAR* dlib_recording_EncryptAsBase64 (HxJSONBIN_t	encoder, HUINT8 *pucData, HUINT32 ulDataSize, const HxCryptKey_t *key)
{
	HUINT8	*pszEncryptBuf;
	HBOOL	bEncrypted = FALSE;

	// encrypt
	pszEncryptBuf = DLIB_Calloc(ulDataSize);
	if (key)
	{
		switch (key->type)
		{
		case HxCRYPT_DES:
			bEncrypted = TRUE;
			HLIB_CRYPT_DES_EncryptData((char *)key->u.DES, pucData, pszEncryptBuf, ulDataSize);
			break;

		case HxCRYPT_3DES:
			bEncrypted = TRUE;
			HLIB_CRYPT_DES3_EncryptData((char *)key->u.DES3, pucData, pszEncryptBuf, ulDataSize);
			break;

		default:
			break;
		}
	}

	if (!bEncrypted)
	{
		HLIB_CRYPT_DES_EncryptData(s_aucDes_Key, pucData, pszEncryptBuf, ulDataSize);
	}

	// encode as base 64
	HLIB_JSONBIN_ENCODER_Clear(encoder);
	if (FALSE == HLIB_JSONBIN_ENCODER_SetBinary(encoder, pszEncryptBuf, ulDataSize))
	{	// fail
		HxLOG_Error("HLIB_JSONBIN_ENCODER_SetBinary() Fail! \n");
		if (pszEncryptBuf)
			DLIB_Free(pszEncryptBuf);
		return (HCHAR *)NULL;
	}

	if (pszEncryptBuf)
		DLIB_Free(pszEncryptBuf);
	return HLIB_JSONBIN_ENCODER_Encode(encoder);
}

#if 0 // 기존 hms ph1 의 잔재
HERROR		dlib_recording_EncodeRecData_DrmInfo (HINT32 sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname, const HxCryptKey_t *key)
{
	HxJSONBIN_t	encoder = HLIB_JSONBIN_ENCODER_Open();
	const	HCHAR	*pszDrmEncodedStr;

	pszDrmEncodedStr = dlib_recording_EncryptAsBase64(encoder, (HUINT8*)&pstRecInfo->stMeta.stDrm, sizeof(DxDrmInfo_t), key);
	if (NULL == pszDrmEncodedStr)
	{
		HxLOG_Error("local_Meta_EncryptAsBase64() Fail! \n");
		HLIB_JSONBIN_ENCODER_Close(encoder);
		return ERR_FAIL;
	}

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body
	HLIB_RWSTREAM_Print(sd, "\"%s\":", 			"stDrmInfo");
	HLIB_RWSTREAM_PutString(sd, pszDrmEncodedStr);
	HLIB_JSONBIN_ENCODER_Close(encoder);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ulCrc", pstRecInfo->stMeta.ulDrmCrc);
	HLIB_RWSTREAM_Print(sd, "}");				// end body

	return ERR_OK;
}
#else
HERROR		dlib_recording_EncodeRecData_CcInfo (HINT32 sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname, const HxCryptKey_t *key)
{
	HxJSONBIN_t	encoder = HLIB_JSONBIN_ENCODER_Open();
	const	HCHAR	*pszDrmEncodedStr;

	pszDrmEncodedStr = dlib_recording_EncryptAsBase64(encoder, (HUINT8*)&pstRecInfo->stMeta.stCcInfo, sizeof(DxCopyrightControl_t), key);
	if (NULL == pszDrmEncodedStr)
	{
		HxLOG_Error("local_Meta_EncryptAsBase64() Fail! \n");
		HLIB_JSONBIN_ENCODER_Close(encoder);
		return ERR_FAIL;
	}

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body
	HLIB_RWSTREAM_Print(sd, "\"%s\":", 			"stCcInfo");
	HLIB_RWSTREAM_PutString(sd, pszDrmEncodedStr);
	HLIB_JSONBIN_ENCODER_Close(encoder);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ulCcInfoCrc", pstRecInfo->stMeta.ulCcInfoCrc);
	HLIB_RWSTREAM_Print(sd, "}");				// end body

	return ERR_OK;
}


#endif


HERROR		dlib_recording_EncodeRecData_ServiceAudio (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	HUINT32			i, j;
	DxMETA_PvrAudioInfoList_t	*pstAudioInfoList = &pstRecInfo->stAudioInfoList;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);
	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":[", 			"astAudioInfo");
	for (i = 0; i < pstAudioInfoList->ulItemNum ; i++)
	{
		HUINT32	ulLangCount = 0;
		DxMETA_PvrAudioInfo_t	*pstAudioInfo = &pstAudioInfoList->astAudioInfo[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ulLangNum", pstAudioInfo->ulLangNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"stLangInfo");
		for (j = 0; j < pstAudioInfo->ulLangNum; j++)
		{
			DxMwPvrIso639LangInfo_t	*pstLangInfo = &pstAudioInfo->stLangInfo[j];

			if (0 < ulLangCount)
				HLIB_RWSTREAM_Print(sd, ",");

			if (HxSTD_StrLen(pstLangInfo->szIso639LangCode) <= 0)	continue;

			HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"eAudType",			pstLangInfo->eAudType);
			HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"",		"szIso639LangCode",	pstLangInfo->szIso639LangCode);
			HLIB_RWSTREAM_Print(sd, "}");	// pstIsoLangInfo

			ulLangCount++;
		}
		HLIB_RWSTREAM_Print(sd, "]");	// stLangInfo
		HLIB_RWSTREAM_Print(sd, "}");	// pstAudioInfo
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astAudioInfo
	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}



HERROR		dlib_recording_EncodeRecData_ServiceSbtl (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	HUINT32 		i;
	DxMETA_PvrSbtlInfoList_t *pstSbtlInfoList = &pstRecInfo->stSbtlInfoList;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);
	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":[", 			"astSbtlInfo");
	for (i = 0; i < pstSbtlInfoList->ulItemNum ; i++)
	{
		DxMETA_PvrSbtlInfo_t	*pstSbtlInfo = &pstSbtlInfoList->astSbtlInfo[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucComponentType",	pstSbtlInfo->ucComponentType);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"aucLangCode", 		pstSbtlInfo->aucLangCode);
		HLIB_RWSTREAM_Print(sd, "}");	// pstSbtlInfo
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astSbtlInfo
	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}


HERROR		dlib_recording_EncodeRecData_ServiceInfo (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body
	HLIB_RWSTREAM_Print(sd, "\"%s\":%d", 			"nChNumber", 		pstRecInfo->nSvcNo);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucServiceName",	pstRecInfo->szSvcName);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usSvcId",			pstRecInfo->stMeta.usSvcId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usTsId",			pstRecInfo->stMeta.usTsId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usOnId",			pstRecInfo->stMeta.usOnId);
#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ",\"%s\":%d", 			"nMasterChNumber", 		pstRecInfo->extension.freesat.nMasterChNumber);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucMasterServiceName",	pstRecInfo->extension.freesat.aucMasterServiceName);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterSvcId",		pstRecInfo->extension.freesat.usMasterSvcId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterTsId",			pstRecInfo->extension.freesat.usMasterTsId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usMasterOnId",			pstRecInfo->extension.freesat.usMasterOnId);
#endif
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usPmtPid",			pstRecInfo->stMeta.usPmtPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usPcrPid",			pstRecInfo->stMeta.usPcrPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usTtxPid",			pstRecInfo->stMeta.usTtxPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usCaptionPid",		pstRecInfo->stMeta.usCaptionPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usVideoPid",		pstRecInfo->stMeta.usVideoPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eVideoCodec",		pstRecInfo->stMeta.eVideoCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usAudioPid",		pstRecInfo->stMeta.usAudioPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eAudioCodec",		pstRecInfo->stMeta.eAudioCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usAudioAuxPid",	pstRecInfo->stMeta.usAudioAuxPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eAudioAuxCodec",	pstRecInfo->stMeta.eAudioAuxCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"usDolbyPid",		pstRecInfo->stMeta.usDolbyPid);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eDolbyCodec",		pstRecInfo->stMeta.eDolbyCodec);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bDolbyFlag",		pstRecInfo->stMeta.bDolbyFlag);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eSvcType",			pstRecInfo->eSvcType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eVideoType",		pstRecInfo->eVideoType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eCasType",			pstRecInfo->eCasType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"ucSubttlIdx",		pstRecInfo->stMeta.ucSubttlIdx);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bSubttlUserFlag",	pstRecInfo->stMeta.bSubttlUserFlag);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eSoundMode",		pstRecInfo->stMeta.eSoundMode);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"eDualMono",		pstRecInfo->stMeta.eDualMono);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"bAudUserSetFlag",	pstRecInfo->stMeta.bAudUserSetFlag);

	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ServiceAudio(sd, pstRecInfo, 	"stAudioInfoList");
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ServiceSbtl(sd, pstRecInfo, 	"stSbtlInfoList");

	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}



HERROR		dlib_recording_EncodeRecData_EventInfo (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	HUINT32	i;

	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "{");				// start body

	HLIB_RWSTREAM_Print(sd, "\"%s\":%d", 			"ulStartTime", 		pstRecInfo->stEvt.ulStartTime);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulDuration", 		pstRecInfo->stEvt.ulDuration);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u3RunnigStatus", 	pstRecInfo->stEvt.ucRunStatus);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"u1FreeCaMode", 	pstRecInfo->stEvt.ucFreeCaMode);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 			"astParentalRating");
	for (i = 0; i < pstRecInfo->stEvt.ucPRNum; i++)
	{
		DxPvr_ParentalRating_t	*pstParentalRating = &pstRecInfo->stEvt.astParentalRating[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":\"%s\"", 		"aucIsoCountryCode",pstParentalRating->aucIsoCountryCode);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucParentalRating", pstParentalRating->ucParentalRating);
		HLIB_RWSTREAM_Print(sd, "}");	// pstParentalRating
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astParentalRating

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucContentNibble", 	pstRecInfo->stEvt.ucContentNibble);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucUserNibble", 	pstRecInfo->stEvt.ucUserNibble);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucEventName",		pstRecInfo->stEvt.aucEventName);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucEventText",		pstRecInfo->stEvt.aucText);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"bSeriesRec",	pstRecInfo->bSeriesRec);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd,	"szSeriesId",	pstRecInfo->stEvt.aucSeriesId);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulEpisodeNum", pstRecInfo->stEvt.ulEpisodeNum);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ulSeason", 	pstRecInfo->stEvt.ulSeason);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"nEvtId", 	pstRecInfo->nEventId);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":[",			"astComponent");
	for (i = 0; i < DxEPG_COMPONENT_DESC_MAX; i++)
	{
		DxPvr_EvtComponent_t		*pstEvtComp		= &pstRecInfo->stEvt.stComponent[i];

		if (0 == pstEvtComp->ucStreamContent)
		{
			HxLOG_Print("[%s:%d] (%d) component encoded \n", __FUNCTION__, __LINE__, i);
			break;
		}

        if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucStreamContent", 	pstEvtComp->ucStreamContent);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucComponentType", 	pstEvtComp->ucComponentType);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"",		"aucIsoCountryCode",pstEvtComp->aucIsoCountryCode);
		HLIB_RWSTREAM_Print(sd, "}");	// pstEvtComp
	}
	HLIB_RWSTREAM_Print(sd, "]");	// astComponent


#if defined(CONFIG_OP_FREESAT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":", 			"ftaCntMgmt");
	HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 				"ucDoNotScramble", 		pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucDoNotScramble);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ucCtrlRAccOverINet", 	pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucCtrlRAccOverINet);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d}", 			"ucDoNotApplyRevocation", 	pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucDoNotApplyRevocation);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":",			"guidance");
	HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",				"eType",		pstRecInfo->extension.freesat.stEvt.astGuidance.eType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 		"stLangCode",	pstRecInfo->extension.freesat.stEvt.astGuidance.stLangCode);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 		"stData",		pstRecInfo->extension.freesat.stEvt.astGuidance.stData);
	HLIB_RWSTREAM_Print(sd, "}");
#endif

#if defined(CONFIG_OP_UK_DTT)
	HLIB_RWSTREAM_Print(sd, ", \"%s\":", 			"ftaCntMgmt");
	HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 				"ucDoNotScramble", 		pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucDoNotScramble);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 			"ucCtrlRAccOverINet", 	pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucCtrlRAccOverINet);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d}", 			"ucDoNotApplyRevocation", 	pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucDoNotApplyRevocation);

	HLIB_RWSTREAM_Print(sd, ", \"%s\":",			"guidance");
	HLIB_RWSTREAM_Print(sd, "{");
	HLIB_RWSTREAM_Print(sd, "  \"%s\":%d",			"eType",		pstRecInfo->extension.ukdtt.stEvt.stGuidance.eType);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",	 		"ucMode",		pstRecInfo->extension.ukdtt.stEvt.stGuidance.ucMode);
	HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 		"stLangCode",	pstRecInfo->extension.ukdtt.stEvt.stGuidance.stLangCode);
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_encodeStrAsUrlEncoding(sd, 		"stData",		pstRecInfo->extension.ukdtt.stEvt.stGuidance.stData);
	HLIB_RWSTREAM_Print(sd, "}");
#endif

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	if(pstRecInfo->stMeta.astTvaCrids[i].ucType > 0)
	{
		HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 		"crids");
		for (i = 0; i < DxMAX_CRID_DATA_NUM; i++)
		{
			DxPVR_TvaCrid_t	*pstCrid = &pstRecInfo->stMeta.astTvaCrids[i];

			if (0 < i)
				HLIB_RWSTREAM_Print(sd, ",");

			HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",		"ucType",	pstCrid->ucType);
			HLIB_RWSTREAM_Print(sd, ", ");
			dlib_recording_encodeStrAsUrlEncoding(sd, 	"szCrid",		pstCrid->szCrid);
			HLIB_RWSTREAM_Print(sd, "}");	// pstCrid
		}
		HLIB_RWSTREAM_Print(sd, "]"); // crids
	}

#if defined(CONFIG_OP_TDC)
	if(HxSTD_StrLen(pstRecInfo->stMeta.szSeriesTitle) > 0)
	{
		HLIB_RWSTREAM_Print(sd, ", ");
		dlib_recording_encodeStrAsUrlEncoding(sd, 	"seriesTitle",		pstRecInfo->stMeta.szSeriesTitle);
	}
#endif
	
#endif // CONFIG_OP_PVR_BASED_ON_TVANYTIME

	HLIB_RWSTREAM_Print(sd, "}");	// end body

	return ERR_OK;
}



HERROR		dlib_recording_EncodeRecData_ExtEventInfo (HINT32		sd, DxRecListData_t	*pstRecInfo, const HCHAR *objname)
{
	HUINT32	i, k;


	if (objname)
		HLIB_RWSTREAM_Print(sd, "\"%s\":", objname);

	HLIB_RWSTREAM_Print(sd, "[");		// array

	for (i = 0; i < pstRecInfo->ulExtEvtCnt; ++i)
	{
		DxRecList_ExtEvt_t	*pstExtEvt = &pstRecInfo->pstExtEvts[i];

		if (0 < i)
			HLIB_RWSTREAM_Print(sd, ",");

		HLIB_RWSTREAM_Print(sd, "{\"%s\":%d", 			"ucDesNum", 		pstExtEvt->ucDesNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucLastDesNum", 	pstExtEvt->ucLastDesNum);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":\"%s\"", 	"szIso639LangCode",	pstExtEvt->szIso639LangCode);
		HLIB_RWSTREAM_Print(sd, ", \"%s\":[", 		"astItem");

		for (k = 0; k < pstExtEvt->ucItemNum; ++k)
		{
			DxRecList_Ext_ItemData_t	*pstItem = &pstExtEvt->astItem[k];

			if (0 < k)
				HLIB_RWSTREAM_Print(sd, ",");

			HLIB_RWSTREAM_Print(sd, "{\"%s\":%d",			"ucDescriptionLength",	HxSTD_StrLen(pstItem->aucDescription));
			HLIB_RWSTREAM_Print(sd, ", ");
			dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucDescription",		pstItem->aucDescription);
			HLIB_RWSTREAM_Print(sd, ", \"%s\":%d",			"ucItemLength",	HxSTD_StrLen(pstItem->aucItem));
			HLIB_RWSTREAM_Print(sd, ", ");
			dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucItem",				pstItem->aucItem);
			HLIB_RWSTREAM_Print(sd, "}");
		}
		HLIB_RWSTREAM_Print(sd, "]"); // astItem

		HLIB_RWSTREAM_Print(sd, ", \"%s\":%d", 		"ucCharLength", 	HxSTD_StrLen(pstRecInfo->aucExtText));
		HLIB_RWSTREAM_Print(sd, ", ");
		dlib_recording_encodeStrAsUrlEncoding(sd, 	"aucChar",			pstRecInfo->aucExtText);
		HLIB_RWSTREAM_Print(sd, "}");
	}
	HLIB_RWSTREAM_Print(sd, "]");	// array

	return ERR_OK;
}



HCHAR *		dlib_recording_EncodeRecData (DxRecListData_t	*pstRecInfo, HUINT32 *dstSize, const HxCryptKey_t *key)
{
#ifdef	DxREC_ENCRYPT_HJM
	HINT32	hjm;
	HINT32	sd;
	HCHAR	*encrypted, *json;
#ifdef DxREC_ENCRYPT_HJM_WITH_CRC
	HUINT32	crc;
#endif
	sd  = HLIB_RWSTREAM_Open(NULL, 0);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	HLIB_RWSTREAM_Print(sd, "{\"%s\":{", "PVR_File_MetaContents_t");
	dlib_recording_EncodeRecData_RecInfo(sd, pstRecInfo, "stRecordInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
#if 0 // 기존 hms ph1 의 잔재
	dlib_recording_EncodeRecData_DrmInfo(sd, pstRecInfo, "stFileDrm", key);
#else
	dlib_recording_EncodeRecData_CcInfo(sd, pstRecInfo, "stCcInfo", key);
#endif

	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ServiceInfo(sd, pstRecInfo, "stServiceInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_EventInfo(sd, pstRecInfo, "stEventInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ExtEventInfo(sd, pstRecInfo, "stExtEventInfo");
	HLIB_RWSTREAM_Print(sd, "}}");
#ifdef DxREC_ENCRYPT_HJM_WITH_CRC
	crc = HLIB_MATH_GetCrc32(HLIB_RWSTREAM_GetBuf(sd), HLIB_RWSTREAM_GetWritten(sd));
#endif
	encrypted = DxRECORDING_Encrypt(HLIB_RWSTREAM_GetBuf(sd), DxRECLISTDATA_GetCryptKey());
	HLIB_RWSTREAM_Close(sd);
	if (encrypted == NULL)
	{
		HxLOG_Error("encrypt fail!\n");
		return NULL;
	}

	hjm = HLIB_JSONWRITER_OpenEx(DLIB_Malloc_CB, DLIB_Free_CB);
	if (!hjm)
	{
		DxRECORDING_FreeData(encrypted);
		HxLOG_Error("[%s:%d] json writer open fail!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}
	HxJSONWRITER_ObjectBegin(hjm);
		HxJSONWRITER_String(hjm, "hjm", encrypted);
#ifdef DxREC_ENCRYPT_HJM_WITH_CRC
		HxJSONWRITER_Integer(hjm, "crc", crc);
#endif
	HxJSONWRITER_ObjectEnd(hjm);
	DxRECORDING_FreeData(encrypted);

	if (HLIB_JSONWRITER_Error(hjm))
	{
		HxLOG_Error("jsonwriter error:%s\n", HLIB_JSONWRITER_Error(hjm));
		HLIB_JSONWRITER_Close(hjm);
		return NULL;
	}
	if (dstSize)
		*dstSize = HLIB_JSONWRITER_GetLength(hjm);

	json = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(hjm));
	HLIB_JSONWRITER_Close(hjm);

	return json;
#else
	HINT32		sd;

	sd = HLIB_RWSTREAM_OpenEx(NULL, 0, DLIB_Malloc_CB, DLIB_Free_CB);
	if (!sd)
	{
		HxLOG_Warning("[%s:%d] R/W Stream open fail!\n", __HxFILE__, __HxLINE__);
		return NULL;
	}

	HLIB_RWSTREAM_Print(sd, "{\"%s\":{", "PVR_File_MetaContents_t");

	dlib_recording_EncodeRecData_RecInfo(sd, pstRecInfo, "stRecordInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
#if 0// 기존 hms ph1 의 잔재
	dlib_recording_EncodeRecData_DrmInfo(sd, pstRecInfo, "stFileDrm", key);
#else
	dlib_recording_EncodeRecData_CcInfo(sd, pstRecInfo, "stCcInfo", key);
#endif
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ServiceInfo(sd, pstRecInfo, "stServiceInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_EventInfo(sd, pstRecInfo, "stEventInfo");
	HLIB_RWSTREAM_Print(sd, ", ");
	dlib_recording_EncodeRecData_ExtEventInfo(sd, pstRecInfo, "stExtEventInfo");

	HLIB_RWSTREAM_Print(sd, "}}");

	*dstSize = HLIB_RWSTREAM_GetWritten(sd);

	return HLIB_RWSTREAM_CloseWithoutBuf(sd);
#endif
}



static HERROR	dlib_recording_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{	// encode as HJM for the compatability
	DxRecListData_t	*pstRecInfo = (DxRecListData_t*)src;

	// more than DxRecListData_t (if it has extended event info)
	HxLOG_Assert(sizeof(DxRecListData_t) <= srcSize);

	if (pstRecInfo->ulExtEvtCnt)
	{	// has extEvt
		HUINT8 *pSrc = (HUINT8*)src;
		pstRecInfo->pstExtEvts = (DxRecList_ExtEvt_t*)&pSrc[sizeof(DxRecListData_t)];
	}

	// encode the RecData content
	*dst		= dlib_recording_EncodeRecData(pstRecInfo, dstSize, NULL);
	if (0 < *dstSize)
		return ERR_OK;

	return ERR_FAIL;
}



/* Decode Functions */
static HERROR dlib_recording_Decode_RecInfo(DxRecListData_t *pstRecInfo, HxJSON_t jsonRecordInfo)
{
	const HCHAR	*pszStr;

	if (NULL == jsonRecordInfo || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "szUrl");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->szUrl, pszStr, DxRECLIST_URL);
	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "aucPath");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->stMeta.szPath, pszStr, DxRECLIST_PATH);
	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "aucFileName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->stMeta.szFileName, pszStr, DxRECLIST_PATH);

	HxLOG_Print("[%s:%d] path(%s) name(%s)\n", __FUNCTION__, __LINE__, pstRecInfo->stMeta.szPath, pstRecInfo->stMeta.szFileName);

	pstRecInfo->ulRecStartTime		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulStartTime");
	pstRecInfo->stMeta.ulEndTime	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulEndTime");
	pstRecInfo->ulRecDuration		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulRecTime");
	HxLOG_Print("[%s:%d] startTime(%d) endTime(%d) recTime(%d)\n", __FUNCTION__, __LINE__
		, pstRecInfo->ulRecStartTime, pstRecInfo->stMeta.ulEndTime, pstRecInfo->ulRecDuration);
	pstRecInfo->ulRecOrgDuration	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulOrgDuration");
	HxLOG_Print("[%s:%d] ulOrgDuration(%d) \n", __FUNCTION__, __LINE__, pstRecInfo->ulRecOrgDuration);
	if (pstRecInfo->ulRecOrgDuration == 0)
		pstRecInfo->ulRecOrgDuration = pstRecInfo->ulRecDuration;

	pstRecInfo->eRecStatus		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucRecordStatus");
	pstRecInfo->bEventBased		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u2cEventBase");
	pstRecInfo->nEventId		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "nEventId");
	pstRecInfo->bBlocked		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u1cLocked");
	pstRecInfo->bPlayed			= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u1cWatched");
#if defined (CONFIG_OP_SES)
	pstRecInfo->ucas.ucSCPinCodeFlag		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucSCPinCodeFlag");

	HxLOG_Print("[%s:%d] ucRecordStatus(%d) u2cEventBase(%d), u1cLocked(%d), u1cWatched(%d), ucSCPinCodeFlag(%d)\n", __FUNCTION__, __LINE__
			, pstRecInfo->eRecStatus, pstRecInfo->bEventBased, pstRecInfo->bBlocked, pstRecInfo->bPlayed, pstRecInfo->ucas.ucSCPinCodeFlag);
#else
	HxLOG_Print("[%s:%d] ucRecordStatus(%d) u2cEventBase(%d), u1cLocked(%d), u1cWatched(%d)\n", __FUNCTION__, __LINE__
			, pstRecInfo->eRecStatus, pstRecInfo->bEventBased, pstRecInfo->bBlocked, pstRecInfo->bPlayed);
#endif

	pstRecInfo->bScrambled		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u1cScrambled");
	HxLOG_Print("[%s:%d] u1cScrambled(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->bScrambled);

	pstRecInfo->bEncrypted	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u1cEncryption");
	pstRecInfo->bAudoDeleted	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "u1cAutoDeleted");
	HxLOG_Print("[%s:%d] u1cEncryption(%d) u1cAutoDeleted(%d)\n", __FUNCTION__, __LINE__
		, pstRecInfo->bEncrypted, pstRecInfo->bAudoDeleted);

	pstRecInfo->eRecEndReason	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucEndCondition");
	pstRecInfo->ulLastViewTime	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulLastWatchtime");
	pstRecInfo->bUserLock		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bUserLocked");
	pstRecInfo->bWillBeDeleted	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bWillBeDeleted");
	pstRecInfo->ulChipId		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulChipId");

	pstRecInfo->bDoNotDelete		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bDoNotDelete");
	pstRecInfo->ulDoNotDeleteSetTime		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulDoNotDeleteSetTime");

	HxLOG_Print("[%s:%d] ucEndCondition(%d) ulLastWatchtime(%d), bUserLocked(%d), bWillBeDeleted(%d)\n"
		, __FUNCTION__, __LINE__, pstRecInfo->eRecEndReason, pstRecInfo->ulLastViewTime
		, pstRecInfo->bUserLock, pstRecInfo->bWillBeDeleted);

	{
		HxJSON_t	jBookmarkArray = HxJSON_Object_Get (jsonRecordInfo, "astBookmarks");

		if (jBookmarkArray)
		{
			HxJSON_t	jBookmark;
			HINT32		i;

			pstRecInfo->usNumOfBookmark = HLIB_JSON_GetSize(jBookmarkArray);
			if (pstRecInfo->usNumOfBookmark)
			{
				for (i = 0; i < pstRecInfo->usNumOfBookmark; ++i)
				{
					jBookmark = HxJSON_Array_Get (jBookmarkArray, i);
					if (jBookmark)
					{
						pstRecInfo->astBookmarks[i].ulPos	= HLIB_JSON_Object_GetInteger(jBookmark, "ulPos");
						pszStr	= HLIB_JSON_Object_GetString(jBookmark, "szName");
						if (pszStr)
							HxSTD_StrNCpy(pstRecInfo->astBookmarks[i].szName, pszStr, DxRECLIST_BOOKMARK_NAME_LEN);

						HxLOG_Print("[%s:%d] (%d) idx bookmark pos(%d), name(%s)\n", __FUNCTION__, __LINE__
							, i, pstRecInfo->astBookmarks[i].ulPos, pstRecInfo->astBookmarks[i].szName);
					}
				}	// end for
			}
		}
	}
	{
		HxJSON_t	jBookmarkArray = HxJSON_Object_Get (jsonRecordInfo, "astPausedPoints");

		if (jBookmarkArray)
		{
			HxJSON_t	jBookmark;
			HINT32		i;

			pstRecInfo->usNumOfPausedPoints = HLIB_JSON_GetSize(jBookmarkArray);
			if (pstRecInfo->usNumOfPausedPoints)
			{
				for (i = 0; i < pstRecInfo->usNumOfPausedPoints; ++i)
				{
					jBookmark = HxJSON_Array_Get (jBookmarkArray, i);
					if (jBookmark)
					{
						pstRecInfo->astPausedPoints[i].ulPos	= HLIB_JSON_Object_GetInteger(jBookmark, "ulPos");
						pszStr	= HLIB_JSON_Object_GetString(jBookmark, "szName");
						if (pszStr)
							HxSTD_StrNCpy(pstRecInfo->astPausedPoints[i].szName, pszStr, DxRECLIST_BOOKMARK_NAME_LEN);

						HxLOG_Print("[%s:%d] (%d) idx astPausedPoint pos(%d), name(%s)\n", __FUNCTION__, __LINE__
							, i, pstRecInfo->astPausedPoints[i].ulPos, pstRecInfo->astPausedPoints[i].szName);
					}
				}	// end for
			}
		}
	}

	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "aucDisplayEvtName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->stMeta.aucDisplayEvtName, pszStr, DxRECLIST_NAME_LEN);
	HxLOG_Print("[%s:%d] aucDisplayEvtName(%s)\n", __FUNCTION__, __LINE__, pstRecInfo->stMeta.aucDisplayEvtName);

	pstRecInfo->bSeriesRec	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bSeriesRec");
	HxLOG_Print("[%s:%d] bSeriesRec(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->bSeriesRec);
	pszStr = HLIB_JSON_Object_GetString(jsonRecordInfo, "szSeriesId");
	if (pszStr)
		HLIB_STD_StrNCpySafe(pstRecInfo->stEvt.aucSeriesId, pszStr, DxPVR_TVANYTIME_CRID_TOTAL_LEN);
	pstRecInfo->stEvt.ulEpisodeNum = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulEpisodeNum");
	pstRecInfo->stEvt.ulSeason = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulSeason");

#if defined(CONFIG_OP_FREESAT)
	pstRecInfo->extension.freesat.ulFtaCmVal					= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulFtaCmVal");
	pstRecInfo->extension.freesat.stEvt.astGuidance.eType	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "guidanceType");
	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "stGuidanceData");
	if (pszStr)
		HLIB_STD_StrNCpySafe(pstRecInfo->extension.freesat.stEvt.astGuidance.stData, pszStr, DxFREESAT_PVR_GUIDANCE_DATA_MAX);
	pstRecInfo->extension.freesat.ucCopyTime			= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucCopyTime");
	pstRecInfo->extension.freesat.ulTimeOffsetAtRec	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulTimeOffsetAtRec");
#endif
#if defined(CONFIG_OP_UK_DTT)
	pstRecInfo->extension.ukdtt.ulFtaCmVal					= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulFtaCmVal");
	pstRecInfo->extension.ukdtt.ucCopyTime			= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucCopyTime");
	pstRecInfo->extension.ukdtt.ulTimeOffsetAtRec	= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulTimeOffsetAtRec");
#endif

#if defined(CONFIG_MW_CI)
	pstRecInfo->ucas.cas.ulInstType = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulInstType");
	pstRecInfo->ucas.cas.ulSlotId = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulSlotId");
  #if defined(CONFIG_MW_CI_PLUS_V_1_3)	// 해당 모델은 CI+ v1.3 지원.
	pstRecInfo->ucas.cas.ciplus.bCamNeedContent = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bCamNeedContent");
	HxLOG_Print("[%s:%d] bCamNeedContent(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->ucas.cas.ciplus.bCamNeedContent);
	pszStr = HLIB_JSON_Object_GetString(jsonRecordInfo, "aucBindedCamId");
	if (pszStr)
		HLIB_STD_StrNCpySafe(pstRecInfo->ucas.cas.ciplus.aucBindedCamId, pszStr, DxRECLIST_CAMID_LEN);
  #endif	//CONFIG_MW_CI_PLUS_V_1_3
#endif

#if 0 //defined(CONFIG_MW_CAS_NAGRA_OP_SES) // defined(CONFIG_APS_HD)
	pstRecInfo->ucas.ucSCPinCodeFlag = HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ucSCPinCodeFlag");
#endif

#if defined(CONFIG_MW_CAS_CONAX)
	pszStr	= HLIB_JSON_Object_GetString(jsonRecordInfo, "szConaxMetaName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->ucas.conax.szConaxMetaName, pszStr, DxRECLIST_PATH);
#endif

	pstRecInfo->bTranscoded		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "bTranscoded");
	pstRecInfo->ulRecMode		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "ulRecMode");
	pstRecInfo->stMeta.usTrRecState		= HLIB_JSON_Object_GetInteger(jsonRecordInfo, "usTrRecState");

	return ERR_OK;
}


static HERROR dlib_recording_DecryptFromBase64Json (HxJSON_t json, HUINT8 *pucData, HUINT32 ulDataSize, const HxCryptKey_t *key)
{
	// decode base 64 json
	HERROR		hErr 			= ERR_FAIL;
	HUINT8		*pszBase64DecodeBuf;
	HxJSONBIN_t	decoder			= HLIB_JSONBIN_DECODER_Open();
	HBOOL		bDecrypted		= FALSE;

	pszBase64DecodeBuf = HLIB_JSONBIN_DECODER_Decode(decoder, NULL, ulDataSize, json, NULL, NULL, NULL);
	HLIB_JSONBIN_DECODER_Close(decoder);

	if (NULL == pszBase64DecodeBuf)
	{
		HxLOG_Error("HLIB_JSONBIN_DECODER_Decode() Fail! \n");
		goto ERROR;
	}

	if (key)
	{
		switch (key->type)
		{
		case HxCRYPT_DES:
			bDecrypted = TRUE;
			HLIB_CRYPT_DES_DecryptData((char *)key->u.DES, pszBase64DecodeBuf, pucData, ulDataSize);
			break;

		case HxCRYPT_3DES:
			bDecrypted = TRUE;
			HLIB_CRYPT_DES3_DecryptData((char *)key->u.DES3, pszBase64DecodeBuf, pucData, ulDataSize);
			break;
		default:
			break;
		}
	}

	if (!bDecrypted)
		HLIB_CRYPT_DES_DecryptData(s_aucDes_Key, pszBase64DecodeBuf, pucData, ulDataSize);
	hErr = ERR_OK;

ERROR:
	if (pszBase64DecodeBuf)
			HLIB_JSONBIN_BufFree(pszBase64DecodeBuf);

	return hErr;
}

#if defined(CONFIG_PROD_HMS1000S) // 기존 hms ph1 의 잔재
static HERROR dlib_recording_Decode_DrmInfo(DxRecListData_t *pstRecInfo, HxJSON_t jsonFileDrmInfo, const HxCryptKey_t *key)
{
	HxJSON_t	jsonDrmInfo;

	if (NULL == jsonFileDrmInfo || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	jsonDrmInfo = HxJSON_Object_Get(jsonFileDrmInfo, "stDrmInfo");
	if (NULL == jsonDrmInfo)
	{
		HxLOG_Error(": can't get the stDrmInfo json \n");
		return ERR_FAIL;
	}

	if (ERR_FAIL == dlib_recording_DecryptFromBase64Json(jsonDrmInfo, (HUINT8*)&pstRecInfo->stMeta.stDrm, sizeof(DxDrmInfo_t), key))
	{
		HxLOG_Error(": Fail in local_Meta_DecryptFromBase64Json() \n");
		return ERR_FAIL;
	}

	pstRecInfo->stMeta.ulDrmCrc	= HLIB_JSON_Object_GetInteger(jsonFileDrmInfo, "ulCrc");

	HxLOG_Print("[%s:%d] meta Crc=(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->stMeta.ulDrmCrc);

	return ERR_OK;
}
#endif


static HERROR dlib_recording_Decode_CcInfo(DxRecListData_t *pstRecInfo, HxJSON_t jsonFileDrmInfo, const HxCryptKey_t *key)
{
	HxJSON_t	jsonCcInfo;

	if (NULL == jsonFileDrmInfo || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	jsonCcInfo = HxJSON_Object_Get(jsonFileDrmInfo, "stCcInfo");
	if (NULL == jsonCcInfo)
	{
		HxLOG_Error(": can't get the stDrmInfo json \n");
		return ERR_FAIL;
	}

	if (ERR_FAIL == dlib_recording_DecryptFromBase64Json(jsonCcInfo, (HUINT8*)&pstRecInfo->stMeta.stCcInfo, sizeof(DxCopyrightControl_t), key))
	{
		HxLOG_Error(": Fail in local_Meta_DecryptFromBase64Json() \n");
		return ERR_FAIL;
	}

	pstRecInfo->stMeta.ulCcInfoCrc	= HLIB_JSON_Object_GetInteger(jsonFileDrmInfo, "ulCcInfoCrc");

	HxLOG_Print("[%s:%d] meta Crc=(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->stMeta.ulCcInfoCrc);

	return ERR_OK;
}


static HERROR dlib_recording_Decode_ServiceAudio(DxRecListData_t *pstRecInfo, HxJSON_t jAudioInfoList)
{
	const HCHAR					*pszStr;
	DxMETA_PvrAudioInfoList_t	*pstAudioInfoList;
	HxJSON_t		array, item;
	HINT32			i, j;

	if (NULL == jAudioInfoList || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pstAudioInfoList = &pstRecInfo->stAudioInfoList;

	array		= HxJSON_Object_Get (jAudioInfoList, "astAudioInfo");
	if (array)
	{
		pstAudioInfoList->ulItemNum = HLIB_JSON_GetSize(array);
		for (i = 0; i < pstAudioInfoList->ulItemNum; ++i)
		{
			item = HxJSON_Array_Get (array, i);
			if (item)
			{
				DxMETA_PvrAudioInfo_t	*pstAudioInfo = &pstAudioInfoList->astAudioInfo[i];
				HxJSON_t				subArray, subItem;

				subArray = HxJSON_Object_Get(item, "stLangInfo");
				if (NULL == subArray)		continue;
				pstAudioInfo->ulLangNum = HLIB_JSON_GetSize(subArray);
				for (j = 0; j < pstAudioInfo->ulLangNum; ++j)
				{
					DxMwPvrIso639LangInfo_t	*pstLangInfo = &pstAudioInfo->stLangInfo[j];
					subItem = HxJSON_Array_Get (subArray, j);

					pstLangInfo->eAudType	= HLIB_JSON_Object_GetInteger(subItem, "eAudType");
					pszStr					= HLIB_JSON_Object_GetString(subItem, 	"szIso639LangCode");
					if (pszStr)
						HxSTD_StrNCpy(pstLangInfo->szIso639LangCode, pszStr, DxPVR_LANG_ISO_639_LEN);
				}
			}
		}	// end for
	}
	return ERR_OK;
}



static HERROR dlib_recording_Decode_ServiceSbtl(DxRecListData_t *pstRecInfo, HxJSON_t jSbtlInfoList)
{
	const HCHAR					*pszStr;
	DxMETA_PvrSbtlInfoList_t	*pstSbtlInfoList;
	HxJSON_t		array, item;
	HINT32			i;

	if (NULL == jSbtlInfoList || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pstSbtlInfoList = &pstRecInfo->stSbtlInfoList;
	array			= HxJSON_Object_Get (jSbtlInfoList, "astSbtlInfo");
	if (array)
	{
		pstSbtlInfoList->ulItemNum = HLIB_JSON_GetSize(array);
		for (i = 0; i < pstSbtlInfoList->ulItemNum; ++i)
		{
			item = HxJSON_Array_Get (array, i);
			if (item)
			{
				DxMETA_PvrSbtlInfo_t	*pstSbtlInfo = &pstSbtlInfoList->astSbtlInfo[i];

				pstSbtlInfo->ucComponentType= HLIB_JSON_Object_GetInteger(item, "ucComponentType");
				pszStr						= HLIB_JSON_Object_GetString(item, "aucLangCode");
				if (pszStr)
					HxSTD_StrNCpy(pstSbtlInfo->aucLangCode, pszStr, DxPVR_LANG_ISO_639_LEN);
			}
		}	// end for
	}
	return ERR_OK;
}



static HERROR dlib_recording_Decode_ServiceInfo(DxRecListData_t *pstRecInfo, HxJSON_t jsonServiceInfo)
{
	const HCHAR		*pszStr;

	if (NULL == jsonServiceInfo || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}

	pstRecInfo->nSvcNo	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "nChNumber");
	pszStr	= HLIB_JSON_Object_GetString(jsonServiceInfo, "aucServiceName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->szSvcName, pszStr, DxRECLIST_NAME_LEN);

	pstRecInfo->stMeta.usSvcId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usSvcId");
	pstRecInfo->stMeta.usTsId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usTsId");
	pstRecInfo->stMeta.usOnId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usOnId");
#if defined(CONFIG_OP_FREESAT)
	pstRecInfo->extension.freesat.nMasterChNumber	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "nMasterChNumber");
	pszStr	= HLIB_JSON_Object_GetString(jsonServiceInfo, "aucMasterServiceName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->extension.freesat.aucMasterServiceName, pszStr, DxPVR_MAX_SERVICE_NAME_LEN);

	pstRecInfo->extension.freesat.usMasterSvcId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterSvcId");
	pstRecInfo->extension.freesat.usMasterTsId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterTsId");
	pstRecInfo->extension.freesat.usMasterOnId		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usMasterOnId");
#endif
	pstRecInfo->stMeta.usPmtPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usPmtPid");
	pstRecInfo->stMeta.usPcrPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usPcrPid");
	pstRecInfo->stMeta.usTtxPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usTtxPid");
	pstRecInfo->stMeta.usCaptionPid= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usCaptionPid");
	pstRecInfo->stMeta.usVideoPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usVideoPid");
	pstRecInfo->stMeta.eVideoCodec	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eVideoCodec");

	pstRecInfo->stMeta.usAudioPid		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usAudioPid");
	pstRecInfo->stMeta.eAudioCodec		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eAudioCodec");
	pstRecInfo->stMeta.usAudioAuxPid	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usAudioAuxPid");
	pstRecInfo->stMeta.eAudioAuxCodec	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eAudioAuxCodec");
	pstRecInfo->stMeta.usDolbyPid		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "usDolbyPid");
	pstRecInfo->stMeta.eDolbyCodec		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eDolbyCodec");
	pstRecInfo->stMeta.bDolbyFlag		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bDolbyFlag");

	pstRecInfo->eSvcType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eSvcType");
	pstRecInfo->eVideoType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eVideoType");
	pstRecInfo->eCasType		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eCasType");
	pstRecInfo->stMeta.ucSubttlIdx		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "ucSubttlIdx");
	pstRecInfo->stMeta.bSubttlUserFlag	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bSubttlUserFlag");
	pstRecInfo->stMeta.eSoundMode		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eSoundMode");
	pstRecInfo->stMeta.eDualMono		= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "eDualMono");
	pstRecInfo->stMeta.bAudUserSetFlag	= HLIB_JSON_Object_GetInteger(jsonServiceInfo, "bAudUserSetFlag");

	{
		HxJSON_t	jAudioInfoList 	= HxJSON_Object_Get(jsonServiceInfo, "stAudioInfoList");
		HxJSON_t	jSbtlInfoList 	= HxJSON_Object_Get(jsonServiceInfo, "stSbtlInfoList");
		dlib_recording_Decode_ServiceAudio(pstRecInfo, jAudioInfoList);
		dlib_recording_Decode_ServiceSbtl(pstRecInfo, jSbtlInfoList);
	}


	HxLOG_Print("[%s:%d] meta ch num(%d), svcName(%s) tripleid(%d/%d/%d)\n", __FUNCTION__, __LINE__,
		pstRecInfo->nSvcNo, pstRecInfo->szSvcName, pstRecInfo->stMeta.usOnId, pstRecInfo->stMeta.usTsId
		, pstRecInfo->stMeta.usSvcId);

	return ERR_OK;
}


static HERROR dlib_recording_Decode_EventInfo(DxRecListData_t *pstRecInfo, HxJSON_t jEvt)
{
	const HCHAR		*pszStr;
	HxJSON_t		jPRArray, jCompArray;

	if (NULL == jEvt || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}


	pstRecInfo->stEvt.ulStartTime	= HLIB_JSON_Object_GetInteger(jEvt, "ulStartTime");
	pstRecInfo->stEvt.ulDuration	= HLIB_JSON_Object_GetInteger(jEvt, "ulDuration");
	pstRecInfo->stEvt.ucRunStatus	= HLIB_JSON_Object_GetInteger(jEvt, "u3RunnigStatus");
	pstRecInfo->stEvt.ucFreeCaMode	= HLIB_JSON_Object_GetInteger(jEvt, "u1FreeCaMode");

	HxLOG_Print("[%s:%d] ulStartTime (%d) ulDuration (%d) u3RunnigStatus(%d) u1FreeCaMode(%d) \n"
		, __FUNCTION__, __LINE__, pstRecInfo->stEvt.ulStartTime, pstRecInfo->stEvt.ulDuration
		, pstRecInfo->stEvt.ucRunStatus, pstRecInfo->stEvt.ucFreeCaMode);

	jPRArray	= HxJSON_Object_Get (jEvt, "astParentalRating");
	if (jPRArray)
	{
		HINT32	i;

		pstRecInfo->stEvt.ucPRNum	= HLIB_JSON_GetSize(jPRArray);
		for (i = 0; i < pstRecInfo->stEvt.ucPRNum; ++i)
		{
			HxJSON_t	jPR = HxJSON_Array_Get (jPRArray, i);
			if (jPR)
			{
				pszStr	= HLIB_JSON_Object_GetString(jPR, "aucIsoCountryCode");
				if (pszStr)
					HxSTD_StrNCpy(pstRecInfo->stEvt.astParentalRating[i].aucIsoCountryCode, pszStr, DxPVR_LANG_ISO_639_LEN);
				pstRecInfo->stEvt.astParentalRating[i].ucParentalRating	= HLIB_JSON_Object_GetInteger(jPR, "ucParentalRating");

				HxLOG_Print("[%s:%d] (%d) idx aucIsoCountryCode (%s) ucParentalRating(%d) \n"
					, __FUNCTION__, __LINE__, i, pstRecInfo->stEvt.astParentalRating[i].aucIsoCountryCode
					, pstRecInfo->stEvt.astParentalRating[i].ucParentalRating);
			}
		}	// end for
	}

	pstRecInfo->stEvt.ucContentNibble	= HLIB_JSON_Object_GetInteger(jEvt, "ucContentNibble");
	pstRecInfo->stEvt.ucUserNibble		= HLIB_JSON_Object_GetInteger(jEvt, "ucUserNibble");
	pszStr	= HLIB_JSON_Object_GetString(jEvt, "aucEventName");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->stEvt.aucEventName, pszStr, DxPVR_SHORT_EVENT_NAME_LEN);

	pszStr	= HLIB_JSON_Object_GetString(jEvt, "aucEventText");
	if (pszStr)
		HxSTD_StrNCpy(pstRecInfo->stEvt.aucText, pszStr, DxPVR_EVENT_TEXT);
	pstRecInfo->nEventId			= HLIB_JSON_Object_GetInteger(jEvt, "nEvtId");

	HxLOG_Print("[%s:%d] ucContentNibble(%d) ucUserNibble(%d) evtName (%s) txt (%s) id(%d)\n"
		, __FUNCTION__, __LINE__, pstRecInfo->stEvt.ucContentNibble, pstRecInfo->stEvt.ucUserNibble
		, pstRecInfo->stEvt.aucEventName, pstRecInfo->stEvt.aucText, pstRecInfo->nEventId);


	jCompArray	= HxJSON_Object_Get (jEvt, "astComponent");
	if (jCompArray)
	{
		HUINT32	i;
		HUINT32	ulCompCnt = HLIB_JSON_GetSize(jCompArray);

		for (i = 0; i < ulCompCnt; ++i)
		{
			HxJSON_t	jComp = HxJSON_Array_Get (jCompArray, i);
			if (jComp)
			{
				pstRecInfo->stEvt.stComponent[i].ucStreamContent	= HLIB_JSON_Object_GetInteger(jComp, "ucStreamContent");
				pstRecInfo->stEvt.stComponent[i].ucComponentType	= HLIB_JSON_Object_GetInteger(jComp, "ucComponentType");
				pszStr	= HLIB_JSON_Object_GetString(jComp, "aucIsoCountryCode");
				if (pszStr)
					HxSTD_StrNCpy(pstRecInfo->stEvt.stComponent[i].aucIsoCountryCode, pszStr, DxPVR_LANG_ISO_639_LEN);

				HxLOG_Print("[%s:%d] (%d) idx aucIsoCountryCode (%s) content (%d) type (%d)\n"
					, __FUNCTION__, __LINE__, i, pstRecInfo->stEvt.stComponent[i].aucIsoCountryCode
					, pstRecInfo->stEvt.stComponent[i].ucStreamContent, pstRecInfo->stEvt.stComponent[i].ucComponentType);
			}
		}	// end for
	}
		pstRecInfo->bSeriesRec	= HLIB_JSON_Object_GetInteger(jEvt, "bSeriesRec");
		HxLOG_Print("[%s:%d] bSeriesRec(%d)\n", __FUNCTION__, __LINE__, pstRecInfo->bSeriesRec);
		pszStr = HLIB_JSON_Object_GetString(jEvt, "szSeriesId");
		if (pszStr)
			HxSTD_StrNCpy(pstRecInfo->stEvt.aucSeriesId, pszStr, DxPVR_TVANYTIME_CRID_TOTAL_LEN-1);
		pstRecInfo->stEvt.ulEpisodeNum = HLIB_JSON_Object_GetInteger(jEvt, "ulEpisodeNum");
		pstRecInfo->stEvt.ulSeason = HLIB_JSON_Object_GetInteger(jEvt, "ulSeason");

#if defined(CONFIG_OP_FREESAT)
{
	HxJSON_t	jFtaCntMgm, jGuidance;

	jFtaCntMgm = HxJSON_Object_Get(jEvt, "ftaCntMgmt");
	if (jFtaCntMgm)
	{
		pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucDoNotScramble 		= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotScramble");
		pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucCtrlRAccOverINet 	= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucCtrlRAccOverINet");
		pstRecInfo->extension.freesat.stEvt.stFtaCntMgmt.ucDoNotApplyRevocation = HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotApplyRevocation");
	}

	jGuidance = HxJSON_Object_Get(jEvt, "guidance");
	if (jGuidance)
	{
		pstRecInfo->extension.freesat.stEvt.astGuidance.eType	= HLIB_JSON_Object_GetInteger(jGuidance, "eType");
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stLangCode");
		if (pszStr)
			HxSTD_StrNCpy(pstRecInfo->extension.freesat.stEvt.astGuidance.stLangCode, pszStr, DxPVR_LANG_ISO_639_LEN );
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stData");
		if (pszStr)
			HxSTD_StrNCpy(pstRecInfo->extension.freesat.stEvt.astGuidance.stData, pszStr, DxFREESAT_PVR_GUIDANCE_DATA_MAX );
	}
}
#endif // CONFIG_OP_FREESAT
#if defined(CONFIG_OP_UK_DTT)
{
	HxJSON_t	jFtaCntMgm, jGuidance;

	jFtaCntMgm = HxJSON_Object_Get(jEvt, "ftaCntMgmt");
	if (jFtaCntMgm)
	{
		pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucDoNotScramble 		= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotScramble");
		pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucCtrlRAccOverINet 	= HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucCtrlRAccOverINet");
		pstRecInfo->extension.ukdtt.stEvt.stFtaCntMgmt.ucDoNotApplyRevocation = HLIB_JSON_Object_GetInteger(jFtaCntMgm, "ucDoNotApplyRevocation");
	}

	jGuidance = HxJSON_Object_Get(jEvt, "guidance");
	if (jGuidance)
	{
		pstRecInfo->extension.ukdtt.stEvt.stGuidance.eType	= HLIB_JSON_Object_GetInteger(jGuidance, "eType");
		pstRecInfo->extension.ukdtt.stEvt.stGuidance.ucMode	= HLIB_JSON_Object_GetInteger(jGuidance, "ucMode");
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stLangCode");
		if (pszStr)
			HxSTD_StrNCpy(pstRecInfo->extension.ukdtt.stEvt.stGuidance.stLangCode, pszStr, DxPVR_LANG_ISO_639_LEN );
		pszStr	= HLIB_JSON_Object_GetString(jGuidance, "stData");
		if (pszStr)
			HxSTD_StrNCpy(pstRecInfo->extension.ukdtt.stEvt.stGuidance.stData, pszStr, DxMAX_GUIDANCE_DATA_LEN );
	}
}
#endif // CONFIG_OP_UK_DTT

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
{
	HxJSON_t	jCridArray;

	jCridArray	= HxJSON_Object_Get (jEvt, "crids");
	if (jCridArray)
	{
		HINT32	i;
		HUINT32	ulCridCnt = HLIB_JSON_GetSize(jCridArray);

		HxLOG_Print("[%s:%d] ulCridCnt (%d)\n", __FUNCTION__, __LINE__, ulCridCnt);

		for (i = 0; i < ulCridCnt; ++i)
		{
			HxJSON_t	jCrid = HxJSON_Array_Get (jCridArray, i);
			if (jCrid)
			{
				pstRecInfo->stMeta.astTvaCrids[i].ucType	= HLIB_JSON_Object_GetInteger(jCrid, "ucType");
				pszStr	= HLIB_JSON_Object_GetString(jCrid, "szCrid");
				if (pszStr)
					HxSTD_snprintf(pstRecInfo->stMeta.astTvaCrids[i].szCrid, DxPVR_TVANYTIME_CRID_TOTAL_LEN, "%s", pszStr);

				HxLOG_Print("[%s:%d] (%d) idx, ucType (%d) szCrid(%s)\n", __FUNCTION__, __LINE__
					, i, pstRecInfo->stMeta.astTvaCrids[i].ucType, pstRecInfo->stMeta.astTvaCrids[i].szCrid);
			}
		}	// end for
	}
}

#if defined(CONFIG_OP_TDC)
	pszStr	= HLIB_JSON_Object_GetString(jEvt, "seriesTitle");
	if (pszStr)
	{
		HLIB_STD_StrUtf8NCpy(pstRecInfo->stMeta.szSeriesTitle, pszStr, DxMAX_SERIES_TITLE_LEN);
	}
#endif

#endif // CONFIG_OP_PVR_BASED_ON_TVANYTIME

	return ERR_OK;
}



static HERROR dlib_recording_Decode_ExtEventInfo(DxRecListData_t *pstRecInfo, HxJSON_t jExtEvtList)
{
	const HCHAR		*pszStr;
	HUINT32	ulExtEvtCnt, i;

	if (NULL == jExtEvtList || NULL == pstRecInfo)
	{
		HxLOG_Error(": invalid parameter\n");
		return ERR_FAIL;
	}


	ulExtEvtCnt = HLIB_JSON_GetSize(jExtEvtList);

	HxLOG_Print("[%s:%d] ext evt cnt (%d)\n", __FUNCTION__, __LINE__, ulExtEvtCnt);
	for (i = 0; i < ulExtEvtCnt; ++i)
	{
		DxRecList_ExtEvt_t *pstExtEvtUnit	= &pstRecInfo->pstExtEvts[i];
		HxJSON_t			jExtEvt 		= HxJSON_Array_Get (jExtEvtList, i);

		if (jExtEvt)
		{
			HxSTD_MemSet(pstExtEvtUnit, 0, sizeof(DxRecList_ExtEvt_t));

			pstExtEvtUnit->ucDesNum = HLIB_JSON_Object_GetInteger(jExtEvt, "ucDesNum");
			pstExtEvtUnit->ucLastDesNum = HLIB_JSON_Object_GetInteger(jExtEvt, "ucLastDesNum");
			pszStr	= HLIB_JSON_Object_GetString(jExtEvt, "szIso639LangCode");
			if (pszStr)
				HxSTD_StrNCpy(pstExtEvtUnit->szIso639LangCode, pszStr, DxPVR_LANG_ISO_639_LEN);

			HxLOG_Print("[%s:%d]	(%d) idx ext evt, des num(%d/%d), lngcode(%s)\n", __FUNCTION__, __LINE__
				, i, pstExtEvtUnit->ucDesNum, pstExtEvtUnit->ucLastDesNum, pstExtEvtUnit->szIso639LangCode);

			pstExtEvtUnit->ucItemNum = 0;
			{
				HINT32		k;
				HxJSON_t	jExtItemArray 	= HxJSON_Object_Get (jExtEvt, "astItem");
				if (jExtItemArray)
				{
					pstExtEvtUnit->ucItemNum	= HLIB_JSON_GetSize(jExtItemArray);


					for (k = 0; k < pstExtEvtUnit->ucItemNum; ++k)
					{
						HxJSON_t	jExtEvtItem 	= HxJSON_Array_Get (jExtItemArray, k);
						pszStr	= HLIB_JSON_Object_GetString(jExtEvtItem, "aucDescription");
						if (pszStr)
						{
							HxSTD_StrNCpy(pstExtEvtUnit->astItem[k].aucDescription, pszStr, DxPVR_EVENT_TEXT);
						}
						pszStr	= HLIB_JSON_Object_GetString(jExtEvtItem, "aucItem");
						if (pszStr)
						{
							HxSTD_StrNCpy(pstExtEvtUnit->astItem[k].aucItem, pszStr, DxPVR_EVENT_TEXT);
						}


						HxLOG_Print("[%s:%d] 	(%d) item idx desc (%s), item (%s)\n", __FUNCTION__, __LINE__
							, k, pstExtEvtUnit->astItem[k].aucDescription, pstExtEvtUnit->astItem[k].aucItem);
					}	// end for k
				}
			}	// end if (pstExtEvtUnit->ucItemNum)

			pszStr	= HLIB_JSON_Object_GetString(jExtEvt, "aucChar");
			if (pszStr)
			{
				HxSTD_StrNCpy(pstRecInfo->aucExtText, pszStr, DxRECLIST_EXT_TEXT_BUFSIZE);
			}

			HxLOG_Print("[%s:%d] (%d) idx char (%s)\n", __FUNCTION__, __LINE__, i, pstRecInfo->aucExtText);
		}	// if (jExtEvt)
	}	// end for (i = 0; i < ulExtEvtCnt; ++i)


	return ERR_OK;
}


static DxRecListData_t *	dlib_recording_Decode_RecData_Nested (HxJSON_t json, const HxCryptKey_t *key)
{
	HxJSON_t	jsonPvrFileMeta, jRecordInfo, jCcInfo, jServiceInfo, jEventInfo, jExtEventInfo;
	DxRecListData_t	*pstRecData	= NULL;
	HUINT32		ulExtEvtCnt 	= 0;
	HUINT32		ulRecDataSize 	= 0;
#if defined(CONFIG_PROD_HMS1000S) // 기존 hms ph1 의 잔재
	HxJSON_t	jFileDrm;
#endif

	HxLOG_Assert(json);

// 2. load the data - encrypted -> read & decrypt, normal text -> decode directly : TODO : use HxLib
	jsonPvrFileMeta = HxJSON_Object_Get(json, "PVR_File_MetaContents_t");
	if (NULL == jsonPvrFileMeta)
	{
		HxLOG_Critical("can't get the PVR_File_MetaContents_t object !!!!!!\n");
		goto DLIB_REC_DECODE_FINISH;
	}

	HxLOG_Print("[%s:%d] get the PVR_File_MetaContents_t \n", __FUNCTION__, __LINE__);

	jExtEventInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stExtEventInfo");
	if (jExtEventInfo)
	{
		ulExtEvtCnt	= HLIB_JSON_GetSize(jExtEventInfo);
	}
	ulRecDataSize	= sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * ulExtEvtCnt;

	pstRecData	= (DxRecListData_t*)DLIB_Calloc(ulRecDataSize);
	if (NULL == pstRecData)
	{
		HxLOG_Critical("can't allloc the DxRecListData_t !\n");
		goto DLIB_REC_DECODE_FINISH;
	}


	jRecordInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stRecordInfo");
	dlib_recording_Decode_RecInfo(pstRecData, jRecordInfo);

#if defined(CONFIG_PROD_HMS1000S) // 기존 hms ph1 의 잔재
	jCcInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stCcInfo");

	if(jCcInfo == NULL) // 즉 stCcInfo가 없으면, stFileDrm으로 찾는다. (ph1방식)
	{
		jFileDrm = HxJSON_Object_Get(jsonPvrFileMeta, "stFileDrm");
		dlib_recording_Decode_DrmInfo(pstRecData, jFileDrm, key);

		// TODO: 중요.
		//기존 hms ph1 에서 DxDrmInfo_t 에 아래와 같은 data를 집어넣고 있었다.
		//hms ph1 은 Ci Plust만 있고, CiPlusDrmInfo_t -> DxCopyrightControl_CiPlus_t 변경되었으나 size는 같다. 즉 복사해주면 ok.
		// ph1에서 drm model type 이 eDrmModelType_None(0) 이 아닐 때만 복사.
		#if 0
		typedef struct
		{
			DrmModelType_t	 			eDrmType;

			union
			{
				HUINT8					aucReserved[60];  // 정보 저장이 필요한 모듈과의 호환을 위해서 해당 자료구조의  최대값(60 byte)을 지정함
				ApsDrmInfo_t			stApsDrmInfo;
				NordigDrmInfo_t			stNordigDrmInfo;
				CiPlusDrmInfo_t			stCiPlusDrmInfo;
				SkyDDrmInfo_t			stSkyDDrmInfo;
				UpcDrmInfo_t			stUpcDrmInfo;
				IrdetoCopyControlInfo_t stIrdetoDrmInfo;
				AribCpInfo_t			stAribCpInfo;
				ViaccessDrmInfo_t		stViaccessDrmInfo;
				// 향이 추가될 때마다 위에 추가한다.
			} info;

		} DrmInfo_t;

		typedef struct
		{
			CiPlusDrmType_t 		eCiPlusDrmType;
			HUINT8					ucCiPlusDrmInfoVer;

			union{
				CiPlusUriInfo_t				stCiPlusUriInfo;
				CiPlusRecCtrl_t				stCiPlusRecCtrl;
				CiPlusLicenseInfo_t			stCiPlusLicenseInfo;
				CiPlusParentalCtrlInfo_t	stCiPlusParentalCtrlInfo;
			} ciplusDrmInfo;
		} CiPlusDrmInfo_t;
		#endif
		if(pstRecData->stMeta.stDrm.ulDrmType != 0)
		{
			// 뒤의 60byte 영역을 복사하는 것임.
			HxSTD_MemCopy(&pstRecData->stMeta.stCcInfo.stCasCcInfo, &pstRecData->stMeta.stDrm, sizeof(DxDrmInfo_t));
			// type enum 값이 바뀌었으니, ci plus 로 세팅
			pstRecData->stMeta.stCcInfo.stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_CiPlus;
		}
		else
		{
			HxSTD_MemSet(&pstRecData->stMeta.stCcInfo, 0x00, sizeof(DxCopyrightControl_t));
			pstRecData->stMeta.ulCcInfoCrc = 0;
		}

	}
	else
	{
		dlib_recording_Decode_CcInfo(pstRecData, jCcInfo, key);
	}
#else
	jCcInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stCcInfo");
	dlib_recording_Decode_CcInfo(pstRecData, jCcInfo, key);
#endif

	jServiceInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stServiceInfo");
	dlib_recording_Decode_ServiceInfo(pstRecData, jServiceInfo);

	jEventInfo = HxJSON_Object_Get(jsonPvrFileMeta, "stEventInfo");
	if (ERR_FAIL == dlib_recording_Decode_EventInfo(pstRecData, jEventInfo))
	{
		HxLOG_Critical("local_Meta_loadJMeta_EventInfo() fail\n");
		goto DLIB_REC_DECODE_FINISH;
	}

	pstRecData->pstExtEvts = NULL;
	if (ulExtEvtCnt)
	{
		HUINT8 *pSrc = (HUINT8*)pstRecData;

		pstRecData->pstExtEvts = (DxRecList_ExtEvt_t*)&pSrc[sizeof(DxRecListData_t)];
		pstRecData->ulExtEvtCnt	= ulExtEvtCnt;
		HxLOG_Print("[%s:%d]	pstFileMeta->stEventInfo.ulExtEvtNum =(%d) \n"
			, __FUNCTION__, __LINE__, pstRecData->ulExtEvtCnt);

		if (ERR_FAIL == dlib_recording_Decode_ExtEventInfo(pstRecData, jExtEventInfo))
		{
			HxLOG_Critical("dlib_recording_Decode_ExtEventInfo() fail\n");
			goto DLIB_REC_DECODE_FINISH;
		}
	}	// end if (jExtEventInfo)


	// set the additional information (ex - szName - aucDisplayEvtName or aucEventName)
	DxRECLISTDATA_ValidateName (pstRecData);
	//HxSTD_PrintToStrN(pstRecData->szUrl, DxRECLIST_URL, "HPVR:/%s%s%s"
	//	, pstRecData->stMeta.szPath, pstRecData->stMeta.szFileName, DxEXT_META_FILE);

	pstRecData->ulType				= eDxSCHED_ACTION_RECORD;
	return pstRecData;

DLIB_REC_DECODE_FINISH:
	if (pstRecData)
		DLIB_Free(pstRecData);
	return NULL;
}

#ifdef	DxREC_ENCRYPT_HJM
static HCHAR *				dlib_recording_DecryptHJM (HxJSON_t json, const HCHAR *type)
{
	const HCHAR *txt;

	txt = HLIB_JSON_Object_GetString(json, type);
	if (txt)
	{
		HCHAR * decrypted = DxRECORDING_Decrypt(txt, DxRECLISTDATA_GetCryptKey());
		if (decrypted == NULL)
		{
			HxLOG_Error("decrypt fail!\n");
			return NULL;
		}
#ifdef DxREC_ENCRYPT_HJM_WITH_CRC
		if (strcmp(type, "hjm") == 0)
		{
			HUINT32 crc = HLIB_MATH_GetCrc32(decrypted, strlen(decrypted));
			if (crc != HLIB_JSON_Object_GetInteger(json, "crc"))
			{
				HxLOG_Error("crc does NOT matched(%u != %u)!\n", crc, HLIB_JSON_Object_GetInteger(json, "crc"));
				DLIB_Free(decrypted);
				return NULL;
			}
		}
#endif
		return decrypted;
	}
	return NULL;
}
#endif

static DxRecListData_t *	dlib_recording_Decode_RecData (HxJSON_t json, const HxCryptKey_t *key)
{
#ifdef	DxREC_ENCRYPT_HJM
	const HCHAR *txt;
	HxJSON_t	hjm = NULL;
	DxRecListData_t *pstRecData = NULL;

	txt = HLIB_JSON_Object_GetString(json, "hjm");
	if (txt)
	{
		HCHAR * decrypted = dlib_recording_DecryptHJM(json, "hjm");
		if (decrypted == NULL)
		{
			HxLOG_Error("decrypt fail!\n");
			return NULL;
		}
		hjm = HLIB_JSON_Decode(decrypted);
		if (!hjm)
		{
			HxLOG_Error("json decode fail:\"%s\"\n", decrypted);
			DLIB_Free(decrypted);
			return NULL;
		}
		DLIB_Free(decrypted);
		json = hjm;
	}
	pstRecData = dlib_recording_Decode_RecData_Nested(json, key);
	if (hjm)
		HLIB_JSON_Delete(hjm);
	return pstRecData;
#else
	return dlib_recording_Decode_RecData_Nested(json, key);
#endif
}


static HERROR	dlib_recording_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	DxRecListData_t	*pstRecData	= NULL;
	HxJSON_t		json;
	HxLOG_Assert(srcSize && src);

	json = HLIB_JSON_Decode(src);
	if (NULL == json)
	{
		*dst     = NULL;
		*dstSize = 0;
		HxLOG_Critical("HLIB_JSON_Decode Fail!!!!!!\n");
		return ERR_FAIL;
	}

	pstRecData = dlib_recording_Decode_RecData(json, NULL);
	HLIB_JSON_Delete(json);

	if (pstRecData == NULL)
	{
		*dst     = NULL;
		*dstSize = 0;
		return ERR_FAIL;
	}

	*dst     = pstRecData;
	*dstSize = sizeof(DxRecListData_t) + sizeof(DxRecList_ExtEvt_t) * pstRecData->ulExtEvtCnt;
	return ERR_OK;
}


#define __DLIB_RECORDING_FIELDS__
/**************************************************************************************
	Get Field Functions
**************************************************************************************/
static HBOOL	dlib_recording_GetUrl(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.string =	DLIB_StrDup(recording->szUrl);

	return TRUE;
}

static HBOOL	dlib_recording_GetName(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.string =	DLIB_StrDup(recording->szName);

	return TRUE;
}

static HBOOL	dlib_recording_GetPath(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;
	HCHAR	*path;
	HUINT32	len;

	HxLOG_Assert(src && size);

	len  = HxSTD_StrLen(recording->stMeta.szPath) + HxSTD_StrLen(recording->stMeta.szFileName) + 10;
	path = DLIB_Malloc(len);
	if (path)
	{
		HLIB_PATH_Join(recording->stMeta.szPath, recording->stMeta.szFileName, path, len);
		field->u.string = path;
	}

	return TRUE;
}


static HBOOL	dlib_recording_GetStatus(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->eRecStatus;

	return TRUE;
}

static HBOOL	dlib_recording_GetDuration(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->ulRecDuration;

	return TRUE;
}

static HBOOL	dlib_recording_GetEpisode(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->stEvt.ulEpisodeNum;
	return TRUE;
}

static HBOOL	dlib_recording_GetSeriesID(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.string =	DLIB_StrDup(recording->stEvt.aucSeriesId);
	return TRUE;
}

static HBOOL	dlib_recording_GetSeason(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->stEvt.ulSeason;
	return TRUE;
}

static HBOOL	dlib_recording_GetIsLock(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->bBlocked;
	return TRUE;
}

static HBOOL	dlib_recording_GetIStartTime(DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = recording->ulRecStartTime;
	return TRUE;
}

static HCHAR * dlib_recording_GetGroupCRIDs(const DxPVR_TvaCrid_t *pstCRIDList)
{
#define KEYWORD_CRID_PROGRAM 		"pCRID:"
#define KEYWORD_CRID_SERIES 		"sCRID:"
#define KEYWORD_CRID_RECOMMEND 		"rCRID:"
#define KEYWORD_CRID_DLIMITER		","

	HUINT32 		i;
	HCHAR			szKeyword[1024] = {0,};
	HCHAR			*pRet;


	if(pstCRIDList == NULL)
		return NULL;

	for(i=0; i< DxMAX_CRID_DATA_NUM; i++)
	{
		if(i>0 && (pstCRIDList[i].ucType ==DxCRID_TYPE_SERIES || pstCRIDList[i].ucType ==DxCRID_TYPE_PROG || pstCRIDList[i].ucType ==DxCRID_TYPE_RECOMMEND ))
		{
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_DLIMITER,MACRO_RECORDING_STRLEN(KEYWORD_CRID_DLIMITER));
		}


		switch(pstCRIDList[i].ucType)
		{
		case DxCRID_TYPE_SERIES:
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_SERIES,MACRO_RECORDING_STRLEN(KEYWORD_CRID_SERIES));
			break;
		case DxCRID_TYPE_PROG:
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_PROGRAM,MACRO_RECORDING_STRLEN(KEYWORD_CRID_PROGRAM));
			break;
		case DxCRID_TYPE_RECOMMEND:
			HxSTD_StrNCat(szKeyword ,KEYWORD_CRID_RECOMMEND,MACRO_RECORDING_STRLEN(KEYWORD_CRID_RECOMMEND));
			break;
		default:
			continue;
		}

		HxSTD_StrNCat(szKeyword ,pstCRIDList[i].szCrid,HLIB_STD_StrNLen(pstCRIDList[i].szCrid,DxCRID_LEN));
	}

	HxLOG_Print("[%s][%d] CRID Keyword: %s\n", __FUNCTION__,__LINE__,szKeyword);
	pRet = DLIB_StrDup(szKeyword);
	return pRet;
}

#ifdef CONFIG_FUNC_RFEPG_USE_KEYWORDSEARCH
static HBOOL	dlib_recording_GetKeyword (DxField_t *field, const void *src, HUINT32 size)
{
	const DxRecListData_t	*recording = (DxRecListData_t*)src;
	HCHAR 					*pKeyword = NULL;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME) || defined(CONFIG_OP_AUS_DTT)
	pKeyword = dlib_recording_GetGroupCRIDs(recording->stMeta.astTvaCrids);
#endif
	if(pKeyword)
		field->u.string = pKeyword;
	else
		field->u.string = DLIB_StrDup("NULL");

	return TRUE;
}
#endif

static const DxRecording_Field_t	s_Recording_FIELDS[] =
{
	  {{eDxTYPE_STRING , "url"    	, FALSE, {0}}, dlib_recording_GetUrl}
	, {{eDxTYPE_STRING,  "name"  	, TRUE , {0}}, dlib_recording_GetName}
	, {{eDxTYPE_STRING,  "path"  	, TRUE , {0}}, dlib_recording_GetPath}
	, {{eDxTYPE_INTEGER, "status"   , FALSE, {0}}, dlib_recording_GetStatus}
	, {{eDxTYPE_INTEGER, "duration"	, FALSE, {0}}, dlib_recording_GetDuration}
	, {{eDxTYPE_INTEGER, "episode"	, FALSE, {0}}, dlib_recording_GetEpisode}
	, {{eDxTYPE_INTEGER, "season"	, FALSE, {0}}, dlib_recording_GetSeason}
	, {{eDxTYPE_STRING,	"seriesID"	, FALSE, {0}}, dlib_recording_GetSeriesID}
	, {{eDxTYPE_INTEGER, "isLocked"	, FALSE, {0}}, dlib_recording_GetIsLock}
	, {{eDxTYPE_INTEGER, "starttime",	FALSE, {0}}, dlib_recording_GetIStartTime}
#ifdef CONFIG_FUNC_RFEPG_USE_KEYWORDSEARCH
	, {{eDxTYPE_STRING,  "keyword"	 , TRUE , {0}}, dlib_recording_GetKeyword}
#endif
};


void			DLIB_RECORDING_Init(void)
{
	(void)DxRECLISTDATA_GetCryptKey();

	dlib_RegisterEnumTable(&_stRecStatusTable);
	dlib_RegisterEnumTable(&_stRecTimestampTable);
	dlib_RegisterEnumTable(&_stRecEndReasonTable);
	dlib_RegisterEnumTable(&_stAudioTypeTable);
#if defined(CONFIG_OP_FREESAT)
	dlib_RegisterEnumTable(&_stFreesat_PVR_GuidanceTypeTable);
#endif // CONFIG_OP_FREESAT
#if defined(CONFIG_OP_UK_DTT)
	dlib_RegisterEnumTable(&_stUkdtt_PVR_GuidanceTypeTable);
#endif // CONFIG_OP_UK_DTT

	dlib_RegisterDECModule(DxNAMEOF(DxRecListData_t), dlib_recording_Encoder, dlib_recording_Decoder);
}


const HCHAR *	DLIB_RECORDING_GetDBFullName (HINT32 dupIndex)
{
	if (dupIndex == 0)
		return ":memory:";
	return NULL;
}



HINT32			DLIB_RECORDING_GetFieldCount (void)
{
	return sizeof(s_Recording_FIELDS) / sizeof(s_Recording_FIELDS[0]);
}

DxField_t *		DLIB_RECORDING_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_RECORDING_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_Recording_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_Recording_FIELDS[index].getData)
	{
		if (!s_Recording_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_Recording_FIELDS[index].field.name);
			return NULL;
		}
	}

	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_RECORDING_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_recording_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_RECORDING_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_recording_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_RECORDING_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_RECORDING_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

void*			DLIB_RECORDING_CloneType(void *org, HINT32 nSize)
{
	DxRecListData_t	*clone, *src;

	if (org == NULL)
		return NULL;

	src = (DxRecListData_t*)org;
	clone	= (DxRecListData_t *)DLIB_MemDup(src, sizeof(DxRecListData_t));
	if (clone)
	{
		if (src->pstExtEvts)
			clone->pstExtEvts = DLIB_MemDup((const HCHAR*)src->pstExtEvts
											, sizeof(DxRecList_ExtEvt_t) * clone->ulExtEvtCnt);
	}

	(void)(nSize);

	return clone;
}

HERROR			DLIB_RECORDING_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxRecListData_t	*pstRecData = NULL;
	DxRecListData_t	*pstOrgRecData = NULL;

	if (NULL == src || 0 == srcSize)
		return ERR_FAIL;

	pstRecData = (DxRecListData_t *)DLIB_Calloc(sizeof(DxRecListData_t));
	if (NULL == pstRecData)
	{
		HxLOG_Error("%s() Out of memory!!\n", __FUNCTION__);
		return ERR_FAIL;
	}

	pstOrgRecData = (DxRecListData_t*)src;
	HxSTD_MemCopy(pstRecData, pstOrgRecData, sizeof(DxRecListData_t));


	if (pstRecData->ulExtEvtCnt)
	{	// memory copy
		HCHAR	*pszCHARSrc = (HCHAR*)src;
		pstRecData->pstExtEvts = (DxRecList_ExtEvt_t *)DLIB_Calloc(sizeof(DxRecList_ExtEvt_t)
			* pstOrgRecData->ulExtEvtCnt);
		HxSTD_MemCopy(pstRecData->pstExtEvts, &pszCHARSrc[sizeof(DxRecListData_t)]
			, sizeof(DxRecList_ExtEvt_t) * pstOrgRecData->ulExtEvtCnt);
	}

	*type = pstRecData;
	return ERR_OK;
}

void			DLIB_RECORDING_FreeType (void *type)
{
	DxRecListData_t	*rec;

	rec = (DxRecListData_t *)type;

	if (rec->pstExtEvts && (HUINT32)(rec + 1) != (HUINT32)(rec->pstExtEvts))
	{
		DLIB_Free(rec->pstExtEvts);
	}
	DLIB_Free(rec);
}

void			DLIB_RECORDING_Print (FILE *fp, const DxRecListData_t *t)
{
	fprintf(fp, "%s[%d] %s|%s\n"
			, DxNAMEOF(DxRecListData_t)
			, t->ulContentId
			, t->szUrl
			, t->szName
		);
	DxLIB_PRINT_STR(t->szThumbnailPath);
	DxLIB_PRINT_INT(t->ulRecStartTime);
	DxLIB_PRINT_INT(t->ulRecDuration);
	DxLIB_PRINT_INT(t->ulRecOrgDuration);
	DxLIB_PRINT_ENUM(t->eRecStatus);
	DxLIB_PRINT_ENUM(t->eRecEndReason);
	DxLIB_PRINT_BOOL(t->bScrambled);
	DxLIB_PRINT_BOOL(t->bEncrypted);
	DxLIB_PRINT_BOOL(t->bEventBased);
	DxLIB_PRINT_INT(t->nEventId);
	DxLIB_PRINT_BOOL(t->bSeriesRec);
	DxLIB_PRINT_INT(t->ulLastViewTime);
	DxLIB_PRINT_BOOL(t->bUserLock);
	DxLIB_PRINT_BOOL(t->bWillBeDeleted);
	DxLIB_PRINT_HEX(t->ulChipId);
	DxLIB_PRINT_BOOL(t->bDoNotDelete);
	DxLIB_PRINT_INT(t->ulDoNotDeleteSetTime);
	DxLIB_PRINT_BOOL(t->bAudoDeleted);
	DxLIB_PRINT_BOOL(t->bPlayed);
	DxLIB_PRINT_BOOL(t->bBlocked);
#if defined (CONFIG_OP_SES)
	DxLIB_PRINT_BOOL(t->ucas.ucSCPinCodeFlag);
#endif
#if defined(CONFIG_MW_CAS_CONAX)
	DxLIB_PRINT_STR(t->ucas.conax.szConaxMetaName);
#endif
	DxLIB_PRINT_STR(t->szStorageId);
	DxLIB_PRINT_INT(t->usNumOfBookmark);
	DxLIB_PRINT_INT(t->usNumOfPausedPoints);
	DxLIB_PRINT_STR(t->szSvcName);
	DxLIB_PRINT_INT(t->nSvcNo);
	DxLIB_PRINT_INT(t->svcUId);
	DxLIB_PRINT_ENUM(t->eSvcType);
	DxLIB_PRINT_ENUM(t->eVideoType);
	DxLIB_PRINT_ENUM(t->eCasType);
	DxLIB_PRINT_BOOL(t->bTranscoded);
	DxLIB_PRINT_INT(t->ulRecMode);
	DxLIB_PRINT_INT(t->stAudioInfoList.ulItemNum);
	DxLIB_PRINT_INT(t->stSbtlInfoList.ulItemNum);
	DxLIB_PRINT_STR(t->aucExtText);
	DxLIB_PRINT_INT(t->ulExtEvtCnt);
	DxLIB_PRINT_STR(t->stMeta.szPath);
	DxLIB_PRINT_STR(t->stMeta.szFileName);
	DxLIB_PRINT_INT(t->stMeta.ulEndTime);
	DxLIB_PRINT_STR(t->stMeta.aucDisplayEvtName);
	DxLIB_PRINT_INT(t->stMeta.usSvcId);
	DxLIB_PRINT_INT(t->stMeta.usTsId);
	DxLIB_PRINT_INT(t->stMeta.usOnId);
	DxLIB_PRINT_INT(t->stMeta.usPmtPid);
	DxLIB_PRINT_INT(t->stMeta.usPcrPid);
	DxLIB_PRINT_INT(t->stMeta.usTtxPid);
	DxLIB_PRINT_INT(t->stMeta.usCaptionPid);
	DxLIB_PRINT_INT(t->stMeta.usVideoPid);
	DxLIB_PRINT_INT(t->stMeta.eVideoCodec);
	DxLIB_PRINT_INT(t->stMeta.usAudioPid);
	DxLIB_PRINT_INT(t->stMeta.eAudioCodec);
	DxLIB_PRINT_INT(t->stMeta.usAudioAuxPid);
	DxLIB_PRINT_INT(t->stMeta.eAudioAuxCodec);
	DxLIB_PRINT_INT(t->stMeta.usDolbyPid);
	DxLIB_PRINT_INT(t->stMeta.eDolbyCodec);
	DxLIB_PRINT_BOOL(t->stMeta.bDolbyFlag);
	DxLIB_PRINT_INT(t->stMeta.ucSubttlIdx);
	DxLIB_PRINT_BOOL(t->stMeta.bSubttlUserFlag);
	DxLIB_PRINT_INT(t->stMeta.eSoundMode);
	DxLIB_PRINT_INT(t->stMeta.eDualMono);
	DxLIB_PRINT_BOOL(t->stMeta.bAudUserSetFlag);
	DxLIB_PRINT_INT(t->stMeta.usTrRecState);
	DxLIB_PRINT_INT(t->stEvt.ulStartTime);
	DxLIB_PRINT_INT(t->stEvt.ulDuration);
	DxLIB_PRINT_INT(t->stEvt.ucPRNum);
	DxLIB_PRINT_INT(t->stEvt.ucContentNibble);
	DxLIB_PRINT_INT(t->stEvt.ucUserNibble);
	DxLIB_PRINT_INT(t->stEvt.ucRunStatus);
	DxLIB_PRINT_INT(t->stEvt.ucFreeCaMode);
	DxLIB_PRINT_STR(t->stEvt.aucEventName);
	DxLIB_PRINT_STR(t->stEvt.aucText);
	DxLIB_PRINT_STR(t->stEvt.aucSeriesId);
	DxLIB_PRINT_INT(t->stEvt.ulEpisodeNum);
	DxLIB_PRINT_INT(t->stEvt.ulSeason);

	{
		HUINT16	i;
		for (i = 0 ; i < t->usNumOfBookmark ; i++)
		{
			DxLIB_PRINT_INT(t->astBookmarks[i].ulPos);
			DxLIB_PRINT_STR(t->astBookmarks[i].szName);
		}
	}
	{
		HUINT16	i;
		for (i = 0 ; i < t->usNumOfPausedPoints ; i++)
		{
			DxLIB_PRINT_INT(t->astPausedPoints[i].ulPos);
			DxLIB_PRINT_STR(t->astPausedPoints[i].szName);
		}
	}
}

HCHAR *			DxRECORDING_MakeURL (
								HCHAR *url
								, HUINT32 n
								, const HCHAR *uuid
								, HINT32 onid
								, HINT32 tsid
								, HINT32 sid
								, HINT32 svcuid
								, HUINT32 time
							)
{
	HINT32 ret = HxSTD_PrintToStrN(url, n
					, "HPVR://%s/%x.%x.%x;%x;%x"
					, uuid ? uuid : ""
					, onid, tsid, sid
					, svcuid
					, time
				);
	if (ret < 0 || (HINT32)n <= ret)
		return NULL;
	return url;
}

void				DxRECLISTDATA_Delete (DxRecListData_t *rec)
{
	if (rec)
		DLIB_RECORDING_FreeType(rec);
}

DxRecListData_t *	DxRECLISTDATA_Clone (const DxRecListData_t *org)
{
	DxRecListData_t	*clone;

	if (org == NULL)
		return NULL;

	clone	= (DxRecListData_t *)DLIB_MemDup(org, sizeof(DxRecListData_t));
	if (clone)
	{
		if (org->pstExtEvts)
		{
			clone->pstExtEvts = DLIB_MemDup((const HCHAR*)org->pstExtEvts
							, sizeof(DxRecList_ExtEvt_t) * org->ulExtEvtCnt);
		}
	}
	return clone;
}

HCHAR *				DxRECLISTDATA_Encode (const DxRecListData_t *rec, const HxCryptKey_t *key)
{
	HUINT32	size;

	return dlib_recording_EncodeRecData((DxRecListData_t *)rec, &size, key);
}

DxRecListData_t *	DxRECLISTDATA_Decode (HxJSON_t json, const HxCryptKey_t *key)
{
	return dlib_recording_Decode_RecData(json, key);
}

HINT32				DxRECLISTDATA_Compare (const DxRecListData_t *lhs, const DxRecListData_t *rhs)
{
	HINT32	diff;

	diff = memcmp(lhs, rhs, offsetof(DxRecListData_t, pstExtEvts));
	if (diff != 0)
		return diff;

	diff = memcmp(lhs->aucExtText, rhs->aucExtText, DxRECLIST_EXT_TEXT_BUFSIZE+1);
	if (diff != 0)
		return diff;

	diff = memcmp(&lhs->ucas, &rhs->ucas, sizeof(lhs->ucas));
	if (diff != 0)
		return diff;

	diff = memcmp(&lhs->extension, &rhs->extension, sizeof(lhs->extension));
	if (diff != 0)
		return diff;

	if (lhs->pstExtEvts == rhs->pstExtEvts)
		return 0;
	if (lhs->pstExtEvts == NULL)
		return -1;
	if (rhs->pstExtEvts == NULL)
		return +1;
	return memcmp(lhs->pstExtEvts, rhs->pstExtEvts, sizeof(DxRecList_ExtEvt_t) * lhs->ulExtEvtCnt);
}

HCHAR *				DxRECLISTDATA_Decrypt (HxJSON_t json, const HCHAR *fileExt)
{
#ifdef	DxREC_ENCRYPT_HJM
	return dlib_recording_DecryptHJM(json, fileExt);
#else
	(void)json;
	return NULL;
#endif
}

const HxCryptKey_t *	DxRECLISTDATA_GetCryptKey (void)
{
	static HxCryptKey_t		s_cryptKey;

	if (s_cryptKey.type != HxCRYPT_3DES)
	{
		s_cryptKey.type = HxCRYPT_3DES;
		HLIB_STD_StrNCpySafe((HCHAR *)(s_cryptKey.u.DES3), "may force be with you", 24);
	}
	return &s_cryptKey;
}

void		DxRECLISTDATA_ValidateName (DxRecListData_t *rec)
{
	if (rec->stMeta.aucDisplayEvtName[0] != '\0')
		HLIB_STD_StrUtf8NCpy(rec->szName, rec->stMeta.aucDisplayEvtName, DxRECLIST_NAME_LEN);
	else if (rec->stEvt.aucEventName[0] != '\0')
		HLIB_STD_StrUtf8NCpy(rec->szName, rec->stEvt.aucEventName, DxRECLIST_NAME_LEN);
	else if (rec->szSvcName[0] != '\0')
		HLIB_STD_StrUtf8NCpy(rec->szName, rec->szSvcName, DxRECLIST_NAME_LEN);
	else
		HLIB_STD_StrUtf8NCpy(rec->szName, rec->stMeta.szFileName, DxRECLIST_NAME_LEN);
}

static HCHAR *		dlib_recording_EncodeTimestampEvent (HINT32 writer, const DxEvent_t *event)
{
	//1 asjkdfl;aksld;fjakl;sdjfkl;ajksl;djfkl;jkl;jkl;ajskl;djfklaj;sdfjk
	/*
		이벤트는 따로 인코딩/디코딩 함수를 갖고 있으나, 망할 hjtsm 포멧 때문에 하위 호환성 맞춰 줘야함 ㅠㅠ
	 */
#if 0	// 추후 사용 시 open 예정.
	HxJSONWRITER_Object(writer, "stEventInfo");
	HxJSONWRITER_Integer(writer, "ulStartTime", event->startTime);
	HxJSONWRITER_Integer(writer, "ulDuration", event->duration);
	HxJSONWRITER_Integer(writer, "u3RunnigStatus", event->runStatus);
	HxJSONWRITER_Integer(writer, "u1FreeCaMode", event->freeCaMode);

	HxJSONWRITER_Array(writer, "astParentalRating");
	// TODO: multi language parental rating
		HxJSONWRITER_ObjectBegin(writer);
			//HxJSONWRITER_String(writer, "aucIsoCountryCode",pstParentalRating->aucIsoCountryCode);
			HxJSONWRITER_Integer(writer, "ucParentalRating", event->parentalRating);
		HxJSONWRITER_ObjectEnd(writer);
		HxJSONWRITER_ArrayEnd(writer);

	HxJSONWRITER_Integer(writer, "ucContentNibble", event->content[0] >> 8);
	HxJSONWRITER_Integer(writer, "ucUserNibble", 	event->content[0] & 0xFF);
	if (event->name)
		HxJSONWRITER_String(writer, "aucEventName", event->name);
	if (event->text)
		HxJSONWRITER_String(writer, "aucEventText", event->text);
	HxJSONWRITER_Integer(writer, "nEvtId", event->eventId);

	if (HxVECTOR_IS_EMPTY(event->component))
	{
		DxEventComponent_t	*component;
		HxJSONWRITER_Array(writer, "astComponent");
		HxVECTOR_EACH(event->component, DxEventComponent_t *, component, {
			HxJSONWRITER_ObjectBegin(writer);
				HxJSONWRITER_Integer(writer, "ucStreamContent", component->content);
				HxJSONWRITER_Integer(writer, "ucComponentType", component->type);
				HxJSONWRITER_String (writer, "aucIsoCountryCode", HLIB_LANG_IdTo639(component->language));
			HxJSONWRITER_ObjectEnd(writer);
		});
		HxJSONWRITER_ArrayEnd(writer);
	}

	#if defined(CONFIG_OP_FREESAT)
		/*
		HxJSONWRITER_Object(writer, "ftaCntMgmt");
			HxJSONWRITER_Integer(writer, "ucDoNotScramble", event->extension.fsa);
			HxJSONWRITER_Integer(writer, "ucCtrlRAccOverINet", 	pstEventInfo->ftaCntMgmt.ucCtrlRAccOverINet);
			HxJSONWRITER_Integer(writer, "ucDoNotApplyRevocation", 	pstEventInfo->ftaCntMgmt.ucDoNotApplyRevocation);
		HxJSONWRITER_ObjectEnd(writer);
		HxJSONWRITER_Object(writer, "guidance");
			HxJSONWRITER_Integer(writer, "eType", event->extension.fsateventinfo.fsatGuidance.guidancetype);
		HxJSONWRITER_ObjectEnd(writer);
		*/
	#endif

	if (HxVECTOR_IS_EMPTY(event->crid))
	{
		DxEventCRID_t *crid;
		HxJSONWRITER_Array(writer, "crids");
		HxVECTOR_EACH(event->crid, DxEventCRID_t *, crid, {
			HxJSONWRITER_ObjectBegin(writer);
				HxJSONWRITER_Integer(writer, "ucType", crid->type);
				if (crid->crid)
					HxJSONWRITER_String (writer, "szCrid", crid->crid);
			HxJSONWRITER_ObjectEnd(writer);
		});
		HxJSONWRITER_ArrayEnd(writer);
	}

	HxJSONWRITER_ObjectEnd(writer);

	HxJSONWRITER_Array(writer, "stExtEventInfo");
		HxJSONWRITER_ObjectBegin(writer);
			HxJSONWRITER_String(writer, "szIso639LangCode", HLIB_LANG_IdTo639(event->language));
			if (HxVECTOR_IS_EMPTY(event->item))
			{
				DxEventItem_t *item;
				HxJSONWRITER_Array(writer, "astItem");
				HxVECTOR_EACH(event->item, DxEventItem_t *, item, {
					HxJSONWRITER_ObjectBegin(writer);
						if (item->description)
							HxJSONWRITER_String(writer, "aucDescription", item->description);
						if (item->item)
							HxJSONWRITER_String(writer, "aucItem", item->item);
					HxJSONWRITER_ObjectEnd(writer);
				});
				HxJSONWRITER_ArrayEnd(writer);
			}
			if (event->longText)
				HxJSONWRITER_String(writer, "aucChar", event->longText);
		HxJSONWRITER_ObjectEnd(writer);
	HxJSONWRITER_ArrayEnd(writer);
#endif
	return NULL;
}


static HCHAR *		dlib_recording_EncodeTimestampService (HINT32 writer, const DxService_t *service)
{
	//1 asjkdfl;aksld;fjakl;sdjfkl;ajksl;djfkl;jkl;jkl;ajskl;djfklaj;sdfjk
	/*
		DxService_t는 따로 인코딩/디코딩 함수를 갖고 있으나, 망할 hjtsm 포멧 때문에 하위 호환성 맞춰 줘야함 ㅠㅠ
	 */
	HxJSONWRITER_Object(writer, "stServiceInfo");
		HxJSONWRITER_Integer(writer, "nChNumber",		service->lcn);
		HxJSONWRITER_String (writer, "aucServiceName",	service->name);
		HxJSONWRITER_Integer(writer, "usSvcId",			service->svcid);
		HxJSONWRITER_Integer(writer, "usTsId",			service->tsid);
		HxJSONWRITER_Integer(writer, "usOnId",			service->onid);
	#if defined(CONFIG_OP_FREESAT)
		/*
		HxJSONWRITER_Integer(writer, "nMasterChNumber",		service->);
		HxJSONWRITER_String (writer, "aucMasterServiceName", 	pstServiceInfo->aucMasterServiceName);
		HxJSONWRITER_Integer(writer, "usMasterSvcId",			pstServiceInfo->usMasterSvcId);
		HxJSONWRITER_Integer(writer, "usMasterTsId", 		pstServiceInfo->usMasterTsId);
		HxJSONWRITER_Integer(writer, "usMasterOnId", 		pstServiceInfo->usMasterOnId);
		*/
	#endif
		HxJSONWRITER_Integer(writer, "usPmtPid", 		service->pmtPid);
		HxJSONWRITER_Integer(writer, "usPcrPid", 		service->pcrPid);
		HxJSONWRITER_Integer(writer, "usTtxPid", 		service->ttxPid);
		//HxJSONWRITER_Integer(writer, "usCaptionPid", 	service-);
		HxJSONWRITER_Integer(writer, "usVideoPid",		service->videoPid);
		HxJSONWRITER_Integer(writer, "eVideoCodec",		service->videoCodec);
		HxJSONWRITER_Integer(writer, "usAudioPid",		service->audioPid);
		HxJSONWRITER_Integer(writer, "eAudioCodec",		service->audioCodec);
		HxJSONWRITER_Integer(writer, "usAudioAuxPid",	service->audioAuxPid);
		HxJSONWRITER_Integer(writer, "eAudioAuxCodec",	service->audioAuxCodec);
		HxJSONWRITER_Integer(writer, "usDolbyPid",		service->dolbyPid);
		HxJSONWRITER_Integer(writer, "eDolbyCodec",		service->dolbyCodec);
		HxJSONWRITER_Integer(writer, "bDolbyFlag",		service->dolbyFlag);

		HxJSONWRITER_Integer(writer, "eSvcType", 		service->svcType);
		HxJSONWRITER_Integer(writer, "eVideoType",		service->highDefinition);
		HxJSONWRITER_Integer(writer, "eCasType", 		service->casType);
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
		HxJSONWRITER_String (writer, "subtitleLanguage",	service->user.subtitleLanguage);
		HxJSONWRITER_Integer(writer, "hardOfHearing",		service->user.hardOfHearing);
#endif
		HxJSONWRITER_Integer(writer, "ucSubttlIdx",		service->user.subtitleTrack);
		HxJSONWRITER_Integer(writer, "bSubttlUserFlag",	service->user.subtitle);
		HxJSONWRITER_Integer(writer, "eSoundMode",		service->user.audioTrack);
		//HxJSONWRITER_Integer(writer, "eDualMono",		pstServiceInfo->eDualMono);
		HxJSONWRITER_Integer(writer, "bAudUserSetFlag",	service->user.audio);
	HxJSONWRITER_ObjectEnd(writer);

	return NULL;
}


HCHAR *				DxRECLISTDATA_EncodeTimestamp (const DxRecTimestamp_t *self)
{
	HINT32	writer;

	HxLOG_Assert(self);

	writer = HLIB_JSONWRITER_OpenEx(DLIB_Malloc_CB, DLIB_Free_CB);

	HxJSONWRITER_ObjectBegin(writer);
	{
		HxJSONWRITER_Object(writer, "Pvr_TimeStamp_t");
			HxJSONWRITER_String (writer, "eType", dlib_ConvertEnum2Str(DxNAMEOF(DxRecTimestampType_e), self->type));
			HxJSONWRITER_Integer(writer, "ulTime", self->time);
			HxJSONWRITER_Integer(writer, "ulPosition", self->position);
		HxJSONWRITER_ObjectEnd(writer);

		if (self->event)
		{
			dlib_recording_EncodeTimestampEvent(writer, self->event);
		}

		if (self->service)
		{
			dlib_recording_EncodeTimestampService(writer, self->service);
		}
	}
	HxJSONWRITER_ObjectEnd(writer);

	// TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return NULL;
}

DxRecTimestamp_t *	DxRECLISTDATA_DecodeTimestamp (HxJSON_t json)
{
	// TODO:!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	return NULL;
}



#if 1 // TODO: 위치 옮긴다.
#define	ERROR_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)

static HCHAR *	dlib_recording_Encrypt (const HCHAR *json, const HxCryptKey_t *key)
{
	HUINT8	*encrypted;
	HUINT32	size;
	HCHAR	*base64;

	ERROR_IF(HxSTD_StrEmpty(json), NULL);

	size      = strlen(json) + 1;
	ERROR_IF(!HLIB_JSON_IsJSON(json, size - 1), NULL);

	encrypted = (HUINT8 *)DLIB_Malloc(strlen(json) + 1);
	ERROR_IF(encrypted == NULL, NULL);

	switch (key->type)
	{
	case HxCRYPT_DES:
		HLIB_CRYPT_DES_EncryptData((char *)key->u.DES, (HUINT8 *)json, encrypted, (HINT32)size);
		break;

	case HxCRYPT_3DES:
		HLIB_CRYPT_DES3_EncryptData((char *)key->u.DES3, (HUINT8 *)json, encrypted, (HINT32)size);
		break;

	case HxCRYPT_AES:
	default:
		HxLOG_Error("%s():%d unknown type:%d\n", __FUNCTION__, __LINE__, key->type);
		DLIB_Free(encrypted);
		return NULL;
	}

	base64 = HLIB_MATH_EncodeBase64Easy(encrypted, size);
	DLIB_Free(encrypted);

	HxLOG_Debug("%s(%s,%s) -> %s\n", __FUNCTION__
		, json
		, key->type == HxCRYPT_DES  ? "DES"
		: key->type == HxCRYPT_3DES ? "3DES"
		: key->type == HxCRYPT_AES  ? "AES" : "Unknown"
		, base64
	);
	return base64;
}

static HCHAR *	dlib_recording_Decrypt (const HCHAR *base64, const HxCryptKey_t *key)
{
	HUINT8	*encrypted;
	HUINT8	*decrypted;
	HUINT32	size;

	ERROR_IF(!HLIB_MATH_IsBase64(base64), NULL);

	encrypted = (HUINT8 *)HLIB_MATH_DecodeBase64Easy(base64, &size);
	ERROR_IF(encrypted == NULL, NULL);

	if (size == 0)
	{
		HLIB_MATH_FreeBase64(encrypted);
		return NULL;
	}

	decrypted = (HUINT8 *)DLIB_Calloc(size);
	if (decrypted == NULL)
	{
		HLIB_MATH_FreeBase64(encrypted);
		return NULL;
	}

	switch (key->type)
	{
	case HxCRYPT_DES:
		HLIB_CRYPT_DES_DecryptData((char *)key->u.DES, encrypted, decrypted, (HINT32)size);
		break;

	case HxCRYPT_3DES:
		HLIB_CRYPT_DES3_DecryptData((char *)key->u.DES3, encrypted, decrypted, (HINT32)size);
		break;

	case HxCRYPT_AES:
	default:
		HxLOG_Error("%s():%d unknown type:%d\n", __FUNCTION__, __LINE__, key->type);
		DLIB_Free(decrypted);
		HLIB_MATH_FreeBase64(encrypted);
		return NULL;
	}
	HLIB_MATH_FreeBase64(encrypted);

	if (!HLIB_JSON_IsJSON(decrypted, size - 1))
	{
		HxLOG_Error("%s():%d Decrypt fail\n", __FUNCTION__, __LINE__);
		HLIB_LOG_Dump(decrypted, size, 0, TRUE);
		DLIB_Free(decrypted);
		return NULL;
	}

	HxLOG_Debug("%s(%s,%s) -> %s\n", __FUNCTION__
		, base64
		, key->type == HxCRYPT_DES  ? "DES"
		: key->type == HxCRYPT_3DES ? "3DES"
		: key->type == HxCRYPT_AES  ? "AES" : "Unknown"
		, decrypted
	);
	return (HCHAR *)decrypted;
}



HCHAR *	DxRECORDING_Encrypt (const HCHAR *json, const HxCryptKey_t *key)
{
	HxLOG_Assert(json && key);

	return dlib_recording_Encrypt(json, key);
}

HCHAR *	DxRECORDING_Decrypt (const HCHAR *base64, const HxCryptKey_t *key)
{
	HxLOG_Assert(base64 && key);

	return dlib_recording_Decrypt(base64, key);
}

void	DxRECORDING_FreeData(void *p)
{
	HLIB_MATH_FreeBase64(p);
}

#endif

