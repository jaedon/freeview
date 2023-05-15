#ifndef	__XMGR_CAS_NA_PRM_INT_H__
#define	__XMGR_CAS_NA_PRM_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <bus.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#define DEFAULT_EMI_FOR_RECORD			(0x20) // SES를 위한 EMI는 0x20이다 .다른 model인경우에는 확인이 필요핟.
#define EX_COPY_CONTROL_BYTE			(3)		// Multiscreen_DRM_Nagra_API_1.6.pdf ..... 3.1 CAK API . ECM COPY CONTROL BYTES EXTRACTION
#define MAX_CAS_INFO_STRING_LEGTH		(50)

#define REC_MAX_NUM						(10)
#define DEFAULT_EMI_FOR_RECORD			(0x20) // SES를 위한 EMI는 0x20이다 .다른 model인경우에는 확인이 필요핟.
#define INVALID_TRANSPORTSESSIONID		(0xffffffff)
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HUINT8 		*pData;
	HUINT32 	ulSize;
} XmgrCas_NaPrmCredential_t;

typedef struct
{
	HUINT8 *pData;
	HUINT32 ulSize;
} XmgrCas_NaPrmUsageRules_t;

typedef struct
{
	HUINT32 	ulTransportSessionId;
	XmgrCas_NaPrmCredential_t	stOldLicense;
	HUINT32 	ulCollectionId;
	HUINT32 	ulDuration;
	XmgrCas_NaPrmUsageRules_t	stUsageRules;

	HUINT8		usRelativity;
	HUINT32		ulRetentionDuration;
	XmgrCas_NaPrmCredential_t	stNewLicense;
} XmgrCas_NaPrm_SetRecordParameters_t;

typedef struct
{
	TSize usSize;
	TUnsignedInt8 *pucData;
}XmgrCas_NaCmdOutputCredentialInfo_t;

typedef struct
{
	TTransportSessionId xTSid;
	TDvlHandle	  	*pxRecordHandle;
	TDvlRecordStatus xRecordingStatus;
	TDvlRecordSessionParameters *pxRecordParameters;
	XmgrCas_NaCmdOutputCredentialInfo_t stCreInfo;
}XmgrCas_NaCmdRecordSessionInformation_t;

typedef struct
{
	HUINT8 	ucChipsetUid[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVendor[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucChipsetRevision[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaVersion[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucCaNumber[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8  ucSmartcardInserted;
	HUINT8 	ucSmartcardSuitable;
	HUINT8 	ucSmartcardType[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardNumber[MAX_CAS_INFO_STRING_LEGTH];
	HUINT8 	ucSmartcardStatus;
	HUINT32	ulExpirationDate;
} XmgrCas_NaCmdCaInfo_t;

XmgrCas_NaCmdOutputCredentialInfo_t 	NaCmdOutputCredentialInfo;
XmgrCas_NaCmdRecordSessionInformation_t		NaCmdRecInfo[REC_MAX_NUM];

// print
#if defined(CONFIG_DEBUG)
void xmgr_cas_naprm_pvr_PrintRecordSessionParameters(TDvlRecordSessionParameters *pxParam);
#endif
void xmgr_cas_naprm_PrintHexDump(HUINT8 *pusTitle, HUINT8 *pusData, HUINT32 ulSize);

HUINT8 *_xmgr_cas_naprm_GetDvlStatusName(TDvlAccessStatus status);
HERROR xmgr_cas_naprm_pvr_GetLcmDataFromCredential(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TLcmData *pLcmData);
void xmgr_cas_naprm_pvr_InitRecordSession(void);
// NOVA
HERROR xmgr_cas_naprm_pvr_NovaGetNewLicense(XmgrCas_NaPrm_SetRecordParameters_t *pDrmParam);
HERROR xmgr_cas_prm_NovaDrmServiceUsageRules(Handle_t hAction, HUINT32 p1, HUINT32 p2, HUINT32 p3);
HERROR xmgr_cas_naprm_pvr_NovaStopRecordSession(HUINT32 ulTransportSessionId);
HERROR xmgr_cas_prm_NovaDrmServiceUsageRules(Handle_t hAction, HUINT32 p1, HUINT32 p2, HUINT32 p3);
/*******************************************************************/
/********************      Public Functions     ********************/

#endif /* !__XMGR_CAS_NA_PRM_INT_H__ */

