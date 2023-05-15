#if defined (CONFIG_DEBUG) && defined(CONFIG_MW_CAS_NAGRA_PRM)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <tapi.h>
#include <mgr_action.h>

#include <namgr_main.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"
#include "local_include/_xmgr_cas_na_prm.h"


/*******************************************************************/
/********************      Specification     ***********************/
/*******************************************************************
/*
	목적
		- DVL ver2.6 Integration Test를 진행
	
	기본 command
		- cmddvl start 0 xxx	: xxx는 credential load하기위한 index임
		- cmddvl stop
		- cmddvl restart
		
	설명
		- http://svn.humaxdigital.com:3000/projects/iCordPRO/files - iCordPro_DVL_Integration_Test_방법.doc
*/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/
TTransportSessionId s_xDvlTSid = 2;
TUnsignedInt8		*s_pDvlFirstCredentials = NULL;
TDvlHandle			*s_pxDvlPlaybackHandle = NULL;
TUnsignedInt8		*s_pxDvlOutputCredentials = NULL;
static 				HUINT8 ucSetTimeCnt = 0;

#if 0//defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
typedef struct
{
	TSize usSize;
	TUnsignedInt8 *pucData;
}XmgrCas_NaCmdOutputCredentialInfo_t;


#define REC_MAX_NUM						(10)
#define DEFAULT_EMI_FOR_RECORD			(0x20) // SES를 위한 EMI는 0x20이다 .다른 model인경우에는 확인이 필요핟.

typedef struct
{
	TTransportSessionId xTSid;
	TDvlHandle	  	*pxRecordHandle;
	TDvlRecordStatus xRecordingStatus;
	TDvlRecordSessionParameters *pxRecordParameters;
	XmgrCas_NaCmdOutputCredentialInfo_t stCreInfo;
}XmgrCas_NaCmdRecordSessionInformation_t;
#endif

#define UPDATE_SECIFICLCMMETADATA
#if defined(UPDATE_SECIFICLCMMETADATA)
#define MAKE_USAGERULE_VAL(val, shift, mask_num)		((val>>shift)&(mask_num==1?0x01:mask_num==2?0x03:mask_num==3?0x07:mask_num==4?0x0f:0x0)) // 쉬운 알고리즘으로 변경하자.
#define DEFAULT_PLAYCOUNT							(255) // 임의로 정함.

typedef struct
{
	HUINT8 cVersion								: 2;	/* ignore */
	HUINT8 cAnticipation_flag						: 1;	/* ignore */
	HUINT8 cRecording_flag						: 1;
	HUINT8 cOriginal_sub_required					: 1;	/* ignore */
	HUINT8 cDvr_sub_required					: 1;	/* ignore */
	HUINT8 cTrans_coding_control				: 1;
	HUINT8 cImage_constraint						: 1;

	HUINT8 cTrick_mode_control					: 4;
	HUINT8 cPlaycount							: 4;	/* ignore */

	HUINT8 cConcurrent_viewing					: 3;
	HUINT8 cDigital_only_token					: 1;
	HUINT8 cUnprotected_digital_output_token		: 1;
	HUINT8 cHdcp_token							: 1;
	HUINT8 cDtcp_token							: 1;
	HUINT8 cWMDRM_token						: 1;

	HUINT8 cPRM_token							: 1;
	HUINT8 cOthers_token						: 1;
	HUINT8 cCci									: 2;
	HUINT8 cUnprotected_analog_output_token	: 1;
	HUINT8 cRetention_state						: 3;
#ifdef SUPPORT_USAGERELE_EXT_DATA
	HUINT8 cPc_threshold						: 4;	/* UR_ext_data */
	HUINT16 sPc_bitmap 							: 12;	/* UR_ext_data */
#endif
#if defined(SUPPORT_USAGERULE_RECOVERY_OUTPUT)
	XmgrCas_NaPrmRecoveryUsageRule_t stUR_Recovery;		/* 여기에 사용할지는 좀더 고민하자 */
#endif
}XmgrCas_NaPrmUsageRule_t;

typedef struct
{
	XmgrCas_NaPrmUsageRule_t				stUsageRule;
	XmgrCas_NaCmdOutputCredentialInfo_t 	stNewSpecificMetadata;
	XmgrCas_NaCmdOutputCredentialInfo_t 	stOutputLcmCredential;
}XmgrCas_NaCmdUpdateSpecificMetadata_t;

typedef struct
{
	XmgrCas_NaCmdUpdateSpecificMetadata_t 	stUpSMD;
	HUINT32 									ulVodPlayCount;	
}XmgrCas_NaCmdPlaybackControl_t;
#endif

#if defined(UPDATE_SECIFICLCMMETADATA)
XmgrCas_NaCmdPlaybackControl_t				stPlayControl;
#endif

XmgrCas_NaCmdOutputCredentialInfo_t 			NaCmdOutputCredentialInfo;
#if 0//defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
XmgrCas_NaCmdRecordSessionInformation_t		NaCmdRecInfo[REC_MAX_NUM];
#endif

extern HUINT8 *_xmgr_cas_naprm_GetDvlStatusName(TDvlAccessStatus status);
extern HERROR xmgr_cas_naprm_ParseUsageRule(HBOOL bDescriptorTagFlag, HUINT8 *pucSpecificMatadata, XmgrCas_NaPrmUsageRule_t *pstUR);

void _xmgr_cas_NaCmd_DvlPrintCredentialsInformationt(
        TCredentialsType    xCredentialsType,
  		TCredentialsUnion* pxCredentialsUnion,
        TSize               xGenericMetadataSize,
  		TUnsignedInt8*     pxGenericMetadata,
        TSize               xSpecificMetadataSize,
  		TUnsignedInt8*     pxSpecificMetadata
)
{
	HUINT32	i;
	VK_TASK_Sleep(500);
	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("\t xCredentialsType : %d\n", xCredentialsType);
	switch(xCredentialsType)
	{
		case CREDENTIALS_TYPE_PROFILE:
			{
				TProfileData	profileData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_PROFILE :	\n");
				HxSTD_memset(&profileData, 0x00, sizeof(TProfileData));
				HxSTD_memcpy(&profileData, &(pxCredentialsUnion->profileData), sizeof(TProfileData));
				HLIB_CMD_Printf("+\t\t * profileData.profileId : %ld\n", profileData.profileId);
				HLIB_CMD_Printf("+\t\t * profileData.profileVersion : %ld\n", profileData.profileVersion);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(profileData.profileExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (profileData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HLIB_CMD_Printf("+\t\t * profileData.contentViewingWindowDuration : %ld \n", profileData.contentViewingWindowDuration);
				HLIB_CMD_Printf("+\t\t * profileData.contentRetentionDuration : %ld \n", profileData.contentRetentionDuration);
			}
			break;

		case CREDENTIALS_TYPE_VOD_EMM:
			{
				TVodEmmData		vodEmmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_VOD_EMM :	\n");
				HxSTD_memset(&vodEmmData, 0x00, sizeof(TVodEmmData));
				HxSTD_memcpy(&vodEmmData, &(pxCredentialsUnion->vodEmmData), sizeof(TVodEmmData));
				HLIB_CMD_Printf("+\t\t * vodEmmData.irdSerialNumber : %ld\n", vodEmmData.irdSerialNumber);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentId : %ld\n", vodEmmData.contentId);
				HLIB_CMD_Printf("+\t\t * vodEmmData.vodEmmId : %ld \n", vodEmmData.vodEmmId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowDuration : %ld \n", vodEmmData.contentViewingWindowDuration);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentFirstVisualizationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEmmData.contentViewingWindowEndDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE \n");
				}
				else if (vodEmmData.contentViewingWindowRelativity == VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION)
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION \n");
				}
				else	// LAST_VIEWING_WINDOW_RELATIVITY
				{
					HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowRelativity : LAST_VIEWING_WINDOW_RELATIVITY \n");
				}

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentIsCiphered : %d \n", vodEmmData.contentIsCiphered);
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentKeyIsProtected : %d \n", vodEmmData.contentKeyIsProtected);
				HLIB_CMD_Printf("+\t\t * vodEmmData.nuidValue : 0x%lx \n", vodEmmData.nuidValue);
			}
			break;

		case CREDENTIALS_TYPE_VOD_ECM:
			{
				TVodEcmData		vodEcmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_VOD_ECM :	\n");
				HxSTD_memset(&vodEcmData, 0x00, sizeof(TVodEcmData));
				HxSTD_memcpy(&vodEcmData, &(pxCredentialsUnion->vodEcmData), sizeof(TVodEcmData));
				HLIB_CMD_Printf("+\t\t * vodEcmData.contentId : %ld\n", vodEcmData.contentId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(vodEcmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond);

				HLIB_CMD_Printf("+\t\t * vodEcmData.contentEmi : %ld \n", vodEcmData.contentEmi);
				HLIB_CMD_Printf("+\t\t * vodEmcData.contentKeyIsProtected : %d \n", vodEcmData.contentKeyIsProtected);
			}
			break;

 		case CREDENTIALS_TYPE_LCM:
			{
				TLcmData	LcmData;
				HxDATETIME_t	dateTime;

				HLIB_CMD_Printf("+\t\t CREDENTIALS_TYPE_LCM :	\n");
				HxSTD_memset(&LcmData, 0x00, sizeof(TLcmData));
				HxSTD_memcpy(&LcmData, &(pxCredentialsUnion->lcmData), sizeof(TLcmData));

				HLIB_CMD_Printf("+\t\t * LcmData.irdSerialNumber : %ld\n", LcmData.irdSerialNumber);
				HLIB_CMD_Printf("+\t\t * LcmData.profileId : %ld\n", LcmData.profileId);
				HLIB_CMD_Printf("+\t\t * LcmData.collectionId : %ld \n", LcmData.collectionId);

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.credentialsCreationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentExpirationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentExpirationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * credentialsCreationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );				

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentViewingWindowDuration : %ld \n", LcmData.contentViewingWindowDuration);				

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentFirstVisualizationDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentFirstVisualizationDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentViewingWindowEndDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentViewingWindowEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );	

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentAppendEndDate, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentAppendEndDate : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_DATETIME_ConvertUnixTimeToDateTime(LcmData.contentViewingWindowRelativity, &dateTime);
				HLIB_CMD_Printf("+\t\t * contentViewingWindowRelativity : ( %d.%d.%d) - (%d:%d:%d)\n", dateTime.stDate.usYear,
															dateTime.stDate.ucMonth,
															dateTime.stDate.ucDay,
															dateTime.stTime.ucHour,
															dateTime.stTime.ucMinute,
															dateTime.stTime.ucSecond );

				HLIB_CMD_Printf("+\t\t * vodEmmData.contentAccessStatus : %s \n", _xmgr_cas_naprm_GetDvlStatusName(LcmData.contentAccessStatus));
				HLIB_CMD_Printf("+\t\t * vodEmmData.contentEmi :0x%x \n", LcmData.contentEmi);
				HLIB_CMD_Printf("+\t\t * vodEmmData.nuidValue :0x%08xx \n", LcmData.nuidValue);
				HLIB_CMD_Printf("+\t\t * vodEmmData.chipsetType : %s \n", &LcmData.chipsetType[0]);
 			}
			break;

		case CREDENTIALS_TYPE_HOME_NETWORK_LCM:

			break;

		default:

			break;
	}

	HLIB_CMD_Printf("\t xGenericMetadataSize : %d\n", xGenericMetadataSize);
	for( i = 0; i < xGenericMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxGenericMetadata[i]);

	}
	HLIB_CMD_Printf("\n");

	HLIB_CMD_Printf("\t xSpecificMetadataSize : %d\n", xSpecificMetadataSize);
	for( i = 0; i < xSpecificMetadataSize; i ++)
	{
		if(i != 0 && i%7 == 0)
		{
			HLIB_CMD_Printf("\n");
		}
		HLIB_CMD_Printf("[0x%02x]", pxSpecificMetadata[i]);

	}
	HLIB_CMD_Printf("\n");

	HLIB_CMD_Printf("###########################################################\n");
	HLIB_CMD_Printf("###########################################################\n");
	VK_TASK_Sleep(500);
}

STATIC void _xmgr_ca_NaCmd_MemFree(TUnsignedInt8 * pData)
{
	if(pData != NULL)
	{
		HLIB_MEM_Free(pData);
	}

	pData = NULL;
}

STATIC HERROR _xmgr_ca_nadvl_SetTime(HUINT16 year, HUINT8 month, HUINT8 day, HUINT8 hour, HUINT8 minute, HUINT8 second)
{
	HxDATETIME_t	stDateTime;
	HUINT32			ulUnixTime;

	stDateTime.stDate.usYear = year;
	stDateTime.stDate.ucMonth = month;
	stDateTime.stDate.ucDay = day;
	stDateTime.stTime.ucHour = hour;
	stDateTime.stTime.ucMinute = minute;
	stDateTime.stTime.ucSecond = second;
	stDateTime.stTime.usMillisecond = 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulUnixTime) == ERR_OK)
	{
		HxLOG_Print("[Set time (%d.%d.%d, %d:%d:%d)]\n", year, month, day, hour, minute, second);
		if(TAPI_SetTime(TxSOURCE_SET_BY_USERINPUT, ulUnixTime) == ERR_OK)
			return ERR_OK;
	}

	return ERR_FAIL;
}

STATIC void _xmgr_cas_NaCmd_loadCredentioalData(HUINT8 ucCredNum, TSize *tFirstCredentialsSize, TUnsignedInt8 **ppFirstCredentials)
{
	HUINT8		aucFileName[128] = {0, };
	HUINT8		*paszFileName[] = {"Credential_V.bin",		/* 0 */
									"Credential_J.bin",	/* 1 */
									"Credential_L.bin",	/* 2 */
									"Credential_M.bin",	/* 3 */
									"Credential_T.bin",	/* 4 */
									"Credential_U.bin",	/* 5 */
									"Credential_C.bin",	/* 6 */
									"Credential_X.bin",	/* 7 */
									"Credential_Y.bin",	/* 8 */
									"Credential_Z.bin",	/* 9 */
									"Credential_W.bin",	/* 10 */
									"PAM_A.bin"};		/* 11 */

	if(ucCredNum > 10)
	{
		HLIB_CMD_Printf("[%s][%d] Error ucCredNum [%d]\n", __FUNCTION__, __LINE__, ucCredNum);
		return;
	}

	if(0)
	{
		HxSTD_sprintf(aucFileName, "/media/drive1/dvl/%s", paszFileName[ucCredNum]); // HDD가 삽입이 안된경우.
	}
	else
	{
		HxSTD_sprintf(aucFileName, "/media/drive2/dvl/%s", paszFileName[ucCredNum]); // HDD가 삽입된뎡우 
	}

	HLIB_CMD_Printf("Credential File Name : [%s]\n", aucFileName);


	if(HLIB_FILE_ReadFile((const HUINT8 *)aucFileName, (void **)ppFirstCredentials, tFirstCredentialsSize) != ERR_OK)
	{
		*tFirstCredentialsSize = 0;
		HLIB_CMD_Printf("[%s][%d] Error in HLIB_FILE_ReadFile[%s]\n", __FUNCTION__, __LINE__, aucFileName);
	}

	HLIB_CMD_Printf("Credential data size : [%d]\n", *tFirstCredentialsSize);

	// 단지 시간을 처음 시작시 한번만 setting한다.
	if(ucSetTimeCnt != 0)
	{
		return;
	}
	
	ucSetTimeCnt++;
	
	_xmgr_ca_nadvl_SetTime(2015, 9, 17, 0, 0, 0);
	//_xmgr_ca_nadvl_SetTime(2013, 3, 26, 1, 0, 1);	/* Credential_M.bin 를 이용하는 경우에는 2013.03.26 1.0.1의 시간을 설정해 주어야한다. */
}

#if defined(UPDATE_SECIFICLCMMETADATA)
STATIC HERROR xmgr_cas_NaCmd_InitUpdateSpecificMetadata(XmgrCas_NaCmdUpdateSpecificMetadata_t * pSMD)
{
	if(pSMD == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pSMD, 0, sizeof(XmgrCas_NaCmdUpdateSpecificMetadata_t));

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaCmd_InitPlayControl(XmgrCas_NaCmdPlaybackControl_t * pPlayControl)
{
	if(pPlayControl == NULL)
	{
		return ERR_FAIL;
	}

	HxSTD_memset(pPlayControl, 0, sizeof(XmgrCas_NaCmdPlaybackControl_t));
	pPlayControl->ulVodPlayCount = DEFAULT_PLAYCOUNT;

	return ERR_OK;
}

STATIC HERROR _xmgr_cas_NaCmd_MakeNewSpecificMetadata(XmgrCas_NaCmdOutputCredentialInfo_t *pUpSMD, TSize  xSpecificMetadataSize, const TUnsignedInt8 *  pxSpecificMetadata)
{
	if(pUpSMD == NULL || pxSpecificMetadata == NULL || xSpecificMetadataSize <= 0)
	{
		return ERR_FAIL;
	}

	pUpSMD->pucData = (HUINT8*)HLIB_STD_MemAlloc(xSpecificMetadataSize + 1);

	if(pUpSMD->pucData)
	{
		HxSTD_memset(pUpSMD->pucData, 0, xSpecificMetadataSize + 1);
		HxSTD_memcpy(pUpSMD->pucData, pxSpecificMetadata, xSpecificMetadataSize);

		#if 1
		{
			// playcount만 생각하자.
			HUINT8 playCount = MAKE_USAGERULE_VAL(pUpSMD->pucData[1], 0, 4);
			HLIB_CMD_Printf("================================================\n");
			HLIB_CMD_Printf("\t\t+ OLD_PLAYCOUNT = %d(%d)\n", playCount, stPlayControl.ulVodPlayCount);
			HLIB_CMD_Printf("\t\t+ pxMakeSpecificMetadata[1] = %d\n", pUpSMD->pucData[1]);

			if(playCount > 0)
			{
				playCount--;
			}

			pUpSMD->pucData[1] = ( pUpSMD->pucData[1]&0xf0) | (playCount&0x0f);
			HLIB_CMD_Printf("\t\t+ NEW_PLAYCOUNT = %d\n", playCount);
			HLIB_CMD_Printf("\t\t+ pxMakeSpecificMetadata[1] = %d\n", pUpSMD->pucData[1]);			
			HLIB_CMD_Printf("================================================\n");			
		}
		#endif
		
		pUpSMD->usSize = xSpecificMetadataSize;

		HLIB_CMD_Printf("[%s][%d] size(%d)\n", __FUNCTION__, __LINE__, pUpSMD->usSize );	
	}
	else
	{
		HxLOG_Critical("HLIB_STD_MemAlloc Error\n");
		return ERR_FAIL;
	}

	return ERR_OK;	
}
#endif

STATIC HERROR _xmgr_cas_NaCmd_UpdateSpecificMetadata(TSize  xInLcmCredentialsSize, const TUnsignedInt8 *  pxInLcmCredentials, TSize  xSpecificMetadataSize, const TUnsignedInt8 *  pxSpecificMetadata,
									TSize *  pxOutLcmCredentialsSize, const TUnsignedInt8 **  ppxOutLcmCredentials)
{
	if(pxInLcmCredentials == NULL || pxSpecificMetadata == NULL)
	{
		HLIB_CMD_Printf("\t Error in pxSpecificMetadata\n");
		return ERR_FAIL;
	}

	if(xmgr_cas_NadvlUpdateSpecificLcmMetadata(xInLcmCredentialsSize, pxInLcmCredentials, xSpecificMetadataSize, pxSpecificMetadata, pxOutLcmCredentialsSize, ppxOutLcmCredentials) != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlUpdateSpecificMetadata\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void _xmgr_cas_NaCmd_GetCredentialsInformation(TSize xCredentialsSize, const TUnsignedInt8* pxCredentials)
{
	HERROR hErr = ERR_OK;
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;	
	TCredentialsType 	xCredentialsType;
	TCredentialsUnion	*pxCredentialsUnion;

	hErr = xmgr_cas_NadvlCredentialsInformationRequest(xCredentialsSize, (const TUnsignedInt8 *)pxCredentials, &xCredentialsType, (const TCredentialsUnion **)&pxCredentialsUnion, &xGenericMetadataSize, (const TUnsignedInt8 **)&pxGenericMetadata, &xSpecificMetadataSize, (const TUnsignedInt8 **)&pxSpecificMetadata);

	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlCredentialsInformationRequest\n");
	}
	else
	{
		_xmgr_cas_NaCmd_DvlPrintCredentialsInformationt(xCredentialsType, pxCredentialsUnion, xGenericMetadataSize, pxGenericMetadata, xSpecificMetadataSize, pxSpecificMetadata);

#if defined(UPDATE_SECIFICLCMMETADATA)
		if(pxSpecificMetadata != NULL)
		{
			xmgr_cas_NaCmd_InitUpdateSpecificMetadata(&stPlayControl.stUpSMD);
			xmgr_cas_naprm_ParseUsageRule(FALSE, pxSpecificMetadata, &stPlayControl.stUpSMD.stUsageRule);
			stPlayControl.ulVodPlayCount = stPlayControl.stUpSMD.stUsageRule.cPlaycount;
		}
#endif

		if(pxCredentialsUnion != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer((const TUnsignedInt8 *)pxCredentialsUnion);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxGenericMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxGenericMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}

		if(pxSpecificMetadata != NULL)
		{
			hErr = xmgr_cas_NadvlReleaseBuffer(pxSpecificMetadata);
			if(hErr != ERR_OK)
			{
				HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
			}
		}
	}
}

STATIC void _xmgr_cas_NaCmd_RelaseBuffer(void)
{
	HERROR hErr;

	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);

	if(s_pxDvlOutputCredentials != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(s_pxDvlOutputCredentials);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}
		s_pxDvlOutputCredentials = NULL;
	}

	if(s_pDvlFirstCredentials)
	{
		_xmgr_ca_NaCmd_MemFree(s_pDvlFirstCredentials);
		s_pDvlFirstCredentials = NULL;
	}
}

STATIC void _xmgr_cas_NaCmd_CloseSession(void)
{
	HERROR hErr;

	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);

	VK_TASK_Sleep(1000);

	_xmgr_cas_NaCmd_RelaseBuffer();
	if(s_pxDvlPlaybackHandle != NULL)
	{
		hErr = xmgr_cas_NadvlStopPlayback(s_pxDvlPlaybackHandle);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		s_pxDvlPlaybackHandle = NULL;
	}
 
#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	hErr = xmgr_cas_RemoveTransportSessionIdDirect(s_xDvlTSid);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_RemoveTransportSessionIdDirect\n");
		return;
	}
#endif
}

STATIC HERROR _xmgr_cas_NaCmd_GetOutputCredential(TUnsignedInt8 **pxOutputCredentials, TSize *usSize)
{
	if(NaCmdOutputCredentialInfo.pucData)
	{
		*pxOutputCredentials = NaCmdOutputCredentialInfo.pucData;
		*usSize = NaCmdOutputCredentialInfo.usSize;
		HLIB_CMD_Printf("[%s][%d] size(%d)\n", __FUNCTION__, __LINE__, *usSize);		
		return ERR_OK;
	}
	else
	{
		HLIB_CMD_Printf("[%s][%d] OutputCredential is NULL\n");		
		return ERR_FAIL;
	}
}

STATIC void _xmgr_cas_NaCmd_ReplaceCredential(TSize xOutputCredentialsSize, const TUnsignedInt8 *pxOutputCredentials)
{
	if(NaCmdOutputCredentialInfo.pucData)
	{
		_xmgr_ca_NaCmd_MemFree(NaCmdOutputCredentialInfo.pucData);
		NaCmdOutputCredentialInfo.usSize = 0;
	}

	NaCmdOutputCredentialInfo.pucData = (HUINT8*)HLIB_STD_MemAlloc(xOutputCredentialsSize + 1);
	if(NaCmdOutputCredentialInfo.pucData)
	{
		HxSTD_memset(NaCmdOutputCredentialInfo.pucData, 0, xOutputCredentialsSize + 1);
		HxSTD_memcpy(NaCmdOutputCredentialInfo.pucData, pxOutputCredentials, xOutputCredentialsSize);
		NaCmdOutputCredentialInfo.usSize = xOutputCredentialsSize;

		HLIB_CMD_Printf("[%s][%d] size(%d)\n", __FUNCTION__, __LINE__, xOutputCredentialsSize);		
	}
	else
	{
		HxLOG_Critical("HLIB_STD_MemAlloc Error\n");
	}
}

STATIC void _xmgr_cas_NaCmd_dvlPlaybackErrorCB(TDvlHandle* pxPlaybackHandle, TDvlAccessStatus  xPlaybackStatus)
{
	HLIB_CMD_Printf("\n\n============================================================================\n");
	HLIB_CMD_Printf("+				 DVL ERROR CALLBACK\n");
	HLIB_CMD_Printf("============================================================================\n");
	
	HLIB_CMD_Printf("[%s][%d] pxPlaybackHandle:[%p], xPlaybackStatus[%d]\n", __FUNCTION__, __LINE__, pxPlaybackHandle, xPlaybackStatus);
	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xPlaybackStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}

STATIC void _xmgr_cas_NaCmd_ReStartVODPlayback(HUINT32 ulTsId)
{
	HERROR				hErr;
	HUINT32				/*i, */steps = 0;
	
	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;

	TDvlAccessStatus	xPlaybackStatus;
	TSize				xOutputCredentialsSize;
	TUnsignedInt8		*s_pxOutputCredential = NULL;

#if defined(UPDATE_SECIFICLCMMETADATA)
	XmgrCas_NaCmdOutputCredentialInfo_t 	*pUpSMD = NULL;
	XmgrCas_NaCmdOutputCredentialInfo_t 	*pLCM = NULL;
#endif

	HLIB_CMD_Printf("\n\n============================================================================\n");
	HLIB_CMD_Printf("+				 DVL TEST CMD : DVL RESTART\n");
	HLIB_CMD_Printf("============================================================================\n");
		
	HLIB_CMD_Printf("[%s][%d]\n",__FUNCTION__, __LINE__);

	s_xDvlTSid = ulTsId;

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_AddTransportSessionIdForPrm\n", steps++);
	hErr = xmgr_cas_AddTransportSessionIdDirect(s_xDvlTSid);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_AddTransportSessionIdDirect\n");
		return;
	}

	stPlayControl.ulVodPlayCount = DEFAULT_PLAYCOUNT; // 다시 정리하자.	
#endif

	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_GetOutputCredential\n", steps++);	
	_xmgr_cas_NaCmd_GetOutputCredential(&s_pxOutputCredential, &xOutputCredentialsSize);

	HLIB_CMD_Printf("[%s][%d] xOutputCredentialsSize(%d)\n",__FUNCTION__, __LINE__, xOutputCredentialsSize);

	if(s_pxOutputCredential == NULL || xOutputCredentialsSize == 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error s_pxDvlOutputCredentials (size = %d)\n", __FUNCTION__, __LINE__, xOutputCredentialsSize);
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}

	VK_TASK_Sleep(1000);

	#if 1
	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_GetCredentialsInformation\n", steps++);	
	_xmgr_cas_NaCmd_GetCredentialsInformation(xOutputCredentialsSize, (const TUnsignedInt8 *)s_pxOutputCredential);
	#endif

#if defined(UPDATE_SECIFICLCMMETADATA)
	if(stPlayControl.ulVodPlayCount == 0)
	{
		HLIB_CMD_Printf("\n\n============================================================================\n");
		HLIB_CMD_Printf("+\t\t* Error PlayCount is not valid\n");
		HLIB_CMD_Printf("============================================================================\n");		
		HLIB_CMD_Printf("[%s][%d] Error Playcount = 0\n", __FUNCTION__, __LINE__, xOutputCredentialsSize);
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}
#endif
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlStartPlayback\n", steps++);	
	hErr = xmgr_cas_NadvlStartPlayback(s_xDvlTSid
										, xOutputCredentialsSize
										, s_pxOutputCredential
										, _xmgr_cas_NaCmd_dvlPlaybackErrorCB
										, &xOutputCredentialsSize
										, (const TUnsignedInt8 **)&s_pxDvlOutputCredentials
										, &xPlaybackStatus
										, &xGenericMetadataSize
										, (const TUnsignedInt8 **)&pxGenericMetadata
										, &xSpecificMetadataSize
										, (const TUnsignedInt8 **)&pxSpecificMetadata
										, &s_pxDvlPlaybackHandle);

	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlPlaybackSetEntitlements\n");
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}

	#if 0 // test only 막아야한다.
	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_GetCredentialsInformation\n", steps++);	
	_xmgr_cas_NaCmd_GetCredentialsInformation(xOutputCredentialsSize, (const TUnsignedInt8 *)s_pxDvlOutputCredentials);
	#endif

	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_ReplaceCredential\n", steps++);	
	_xmgr_cas_NaCmd_ReplaceCredential(xOutputCredentialsSize, s_pxDvlOutputCredentials);

#if defined(UPDATE_SECIFICLCMMETADATA)
	pUpSMD =  &stPlayControl.stUpSMD.stNewSpecificMetadata;
	pLCM = &stPlayControl.stUpSMD.stOutputLcmCredential;
	if(pUpSMD->pucData)
	{
		_xmgr_ca_NaCmd_MemFree(pUpSMD->pucData);
	}

	if(_xmgr_cas_NaCmd_MakeNewSpecificMetadata(pUpSMD, xSpecificMetadataSize, pxSpecificMetadata) == ERR_OK)
	{
		HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_UpdateSpecificMetadata\n", steps++);	
		if(_xmgr_cas_NaCmd_UpdateSpecificMetadata(xOutputCredentialsSize , s_pxDvlOutputCredentials
													, pUpSMD->usSize, (const TUnsignedInt8 *)pUpSMD->pucData
													, &pLCM->usSize, (const TUnsignedInt8 **)&pLCM->pucData) == ERR_OK)
		{
			if(pLCM->pucData != NULL && pLCM->usSize > 0)
			{
				_xmgr_cas_NaCmd_ReplaceCredential(pLCM->usSize, pLCM->pucData);
			}
		}
	}
#endif

	if(s_pxDvlOutputCredentials != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(s_pxDvlOutputCredentials);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		s_pxDvlOutputCredentials = NULL;
	}

#if defined(UPDATE_SECIFICLCMMETADATA)
	if(pLCM->pucData != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(pLCM->pucData);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		pLCM->pucData = NULL;
	}
#endif

	HLIB_CMD_Printf("###### Step [%d] End of [%s]\n", steps++ , __FUNCTION__);

	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xPlaybackStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}

STATIC void _xmgr_cas_NaCmd_StartVODPlayback(HUINT32 ulTsId, HUINT8 ucCredNum)
{
	HERROR				hErr;
	HUINT32				/*i, */steps = 0;
	static HBOOL		bInitStartFlag = FALSE;

	TSize				xGenericMetadataSize;
	TUnsignedInt8		*pxGenericMetadata;
	TSize			 	xSpecificMetadataSize;
	TUnsignedInt8		*pxSpecificMetadata;
	TSize				tFirstCredentialsSize;

	TDvlAccessStatus	xPlaybackStatus;
	TSize				xOutputCredentialsSize;

#if defined(UPDATE_SECIFICLCMMETADATA)
	XmgrCas_NaCmdOutputCredentialInfo_t 	*pUpSMD = NULL;
	XmgrCas_NaCmdOutputCredentialInfo_t 	*pLCM = NULL;
#endif	

	HLIB_CMD_Printf("\n\n============================================================================\n");
	HLIB_CMD_Printf("+				 DVL TEST CMD : DVL START\n");
	HLIB_CMD_Printf("============================================================================\n");

	HLIB_CMD_Printf("[%s][%d], ucCredNum [%d]\n",__FUNCTION__, __LINE__, ucCredNum);

	s_xDvlTSid = ulTsId;

#if defined(UPDATE_SECIFICLCMMETADATA)
	if(bInitStartFlag == FALSE)
	{
		_xmgr_cas_NaCmd_InitPlayControl(&stPlayControl);
	}
	bInitStartFlag = TRUE;
	stPlayControl.ulVodPlayCount = DEFAULT_PLAYCOUNT;
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALAPI_7_1_ABOVE)
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_AddTransportSessionIdForPrm\n", steps++);
	hErr = xmgr_cas_AddTransportSessionIdDirect(s_xDvlTSid);
	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_AddTransportSessionIdDirect\n");
		return;
	}
#endif

	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_loadCredentioalData\n", steps++);	
	_xmgr_cas_NaCmd_loadCredentioalData(ucCredNum, &tFirstCredentialsSize, &s_pDvlFirstCredentials);
	
	if(tFirstCredentialsSize == 0)
	{
		HLIB_CMD_Printf("[%s][%d] Error CredentialSize : [%d]\n", __FUNCTION__, __LINE__, tFirstCredentialsSize);
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}

	VK_TASK_Sleep(1000);

	#if 1
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlCredentialsInformationRequest\n", steps++);
	_xmgr_cas_NaCmd_GetCredentialsInformation(tFirstCredentialsSize, (const TUnsignedInt8 *)s_pDvlFirstCredentials);
	#endif

#if defined(UPDATE_SECIFICLCMMETADATA)
	if(stPlayControl.ulVodPlayCount == 0)
	{
		HLIB_CMD_Printf("\n\n============================================================================\n");
		HLIB_CMD_Printf("+				 Error PlayCount is not valid\n");
		HLIB_CMD_Printf("============================================================================\n");		
		HLIB_CMD_Printf("[%s][%d] Error Playcount = 0\n", __FUNCTION__, __LINE__, xOutputCredentialsSize);
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}
#endif

	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlStartPlayback\n", steps++);
	hErr = xmgr_cas_NadvlStartPlayback(s_xDvlTSid
										, tFirstCredentialsSize
										, s_pDvlFirstCredentials
										, _xmgr_cas_NaCmd_dvlPlaybackErrorCB
										, &xOutputCredentialsSize
										, (const TUnsignedInt8 **)&s_pxDvlOutputCredentials
										, &xPlaybackStatus
										, &xGenericMetadataSize
										, (const TUnsignedInt8 **)&pxGenericMetadata
										, &xSpecificMetadataSize
										, (const TUnsignedInt8 **)&pxSpecificMetadata
										, &s_pxDvlPlaybackHandle);

	if(hErr != ERR_OK)
	{
		HLIB_CMD_Printf("\t Error in xmgr_cas_NadvlPlaybackSetEntitlements\n");
		_xmgr_cas_NaCmd_CloseSession();
		return;
	}
	HLIB_CMD_Printf("xOutputCredentialsSize [%d], s_pxDvlOutputCredentials [%p], s_pxDvlOutputCredentials [%p]\n", xOutputCredentialsSize, s_pxDvlOutputCredentials, &s_pxDvlOutputCredentials);

	#if 1
	HLIB_CMD_Printf("###### Step [%d] xmgr_cas_NadvlCredentialsInformationRequest\n", steps++);
	_xmgr_cas_NaCmd_GetCredentialsInformation(xOutputCredentialsSize, (const TUnsignedInt8 *)s_pxDvlOutputCredentials);
	#endif

	HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_ReplaceCredential\n", steps++);
	_xmgr_cas_NaCmd_ReplaceCredential(xOutputCredentialsSize, s_pxDvlOutputCredentials);

#if defined(UPDATE_SECIFICLCMMETADATA)
	pUpSMD =  &stPlayControl.stUpSMD.stNewSpecificMetadata;
	pLCM = &stPlayControl.stUpSMD.stOutputLcmCredential;
	if(pUpSMD->pucData)
	{
		_xmgr_ca_NaCmd_MemFree(pUpSMD->pucData);
	}

	if(_xmgr_cas_NaCmd_MakeNewSpecificMetadata(pUpSMD, xSpecificMetadataSize, pxSpecificMetadata) == ERR_OK)
	{
		HLIB_CMD_Printf("###### Step [%d] _xmgr_cas_NaCmd_UpdateSpecificMetadata\n", steps++);	
		if(_xmgr_cas_NaCmd_UpdateSpecificMetadata(xOutputCredentialsSize , s_pxDvlOutputCredentials
													, pUpSMD->usSize, (const TUnsignedInt8 *)pUpSMD->pucData
													, &pLCM->usSize, (const TUnsignedInt8 **)&pLCM->pucData) == ERR_OK)
		{
			if(pLCM->pucData != NULL && pLCM->usSize > 0)
			{
				_xmgr_cas_NaCmd_ReplaceCredential(pLCM->usSize, pLCM->pucData);
			}
		}
	}
#endif

	if(s_pxDvlOutputCredentials != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(s_pxDvlOutputCredentials);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		s_pxDvlOutputCredentials = NULL;
	}

#if defined(UPDATE_SECIFICLCMMETADATA)
	if(pLCM->pucData != NULL)
	{
		hErr = xmgr_cas_NadvlReleaseBuffer(pLCM->pucData);
		if(hErr != ERR_OK)
		{
			HLIB_CMD_Printf("[%s][%d] hErr : [0x%x]\n", __FUNCTION__, __LINE__, hErr);
		}

		pLCM->pucData = NULL;
	}	
#endif


	HLIB_CMD_Printf("###### Step [%d] End of [%s]\n", steps++ , __FUNCTION__);

	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xPlaybackStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xPlaybackStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}

STATIC void _xmgr_cas_NaCmd_StopVODPlayback(void)
{
	HLIB_CMD_Printf("\n\n============================================================================\n");
	HLIB_CMD_Printf("+				 DVL TEST CMD : DVL STOP\n");
	HLIB_CMD_Printf("============================================================================\n");	
	HLIB_CMD_Printf("###### [%s]\n", __FUNCTION__);
	
	_xmgr_cas_NaCmd_CloseSession();
}

#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
STATIC void _xmgr_cas_NaCmd_PrintRecordSession(void)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	HxLOG_Debug("\n=====================================================================\n");
	HxLOG_Debug("+			DVL TEST CMD : RECORD INFO\n");
	HxLOG_Debug("=====================================================================\n");

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		HxLOG_Debug("[%d] xTSid(0x%08x), pxRecordHandle(0x%08x), pxRecordParameters(0x%08x), xRecordingStatus(%d) ---- ", i, pRecInfo->xTSid, pRecInfo->pxRecordHandle, pRecInfo->pxRecordParameters, pRecInfo->xRecordingStatus);
		if(pRecInfo->stCreInfo.pucData)
		{
			HxLOG_Debug("CredentialData : OK..........CreInfoSize(%d)\n", pRecInfo->stCreInfo.usSize);
		}
		else
		{
			HxLOG_Debug("CredentialData : Not OK..........CreInfoSize(%d)\n", pRecInfo->stCreInfo.usSize);
		}
	}
	HxLOG_Debug("\n");
}

STATIC void _xmgr_cas_NaCmd_InitRecordSessionByIndex(int index)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = &NaCmdRecInfo[index];

	if(pRecInfo)
	{
		if(pRecInfo->pxRecordParameters)
		{
			xmgr_cas_NadvlReleaseBuffer((TUnsignedInt8 *)pRecInfo->pxRecordParameters);
			pRecInfo->pxRecordParameters = NULL;
		}

		pRecInfo->xTSid = 0xff;
		pRecInfo->pxRecordHandle = NULL;
		pRecInfo->xRecordingStatus = DVL_RECORD_GRANTED;
		pRecInfo->stCreInfo.usSize = 0;

		_xmgr_ca_NaCmd_MemFree(pRecInfo->stCreInfo.pucData);
	}
}

STATIC void _xmgr_cas_NaCmd_InitRecordSession(void)
{
	int i = 0;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		_xmgr_cas_NaCmd_InitRecordSessionByIndex(i);
	}
}

STATIC HERROR _xmgr_cas_NaCmd_MakeRecordSession(TDvlRecordSessionParameters *pxRecordParameters, TTransportSessionId xTSid, TDvlHandle* pxRecordHandle, TSize xCredentialsSize, const TUnsignedInt8* pxCredentials, TDvlRecordStatus xRecordingStatus)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;

	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->stCreInfo.pucData == NULL)
		{
			pRecInfo->stCreInfo.pucData = (HUINT8*)HLIB_STD_MemAlloc(xCredentialsSize + 1);
			if(pRecInfo->stCreInfo.pucData)
			{
				pRecInfo->pxRecordParameters = pxRecordParameters;
				pRecInfo->pxRecordHandle = pxRecordHandle;
				pRecInfo->xTSid = xTSid;
				pRecInfo->xRecordingStatus = xRecordingStatus;

				HxSTD_memset(pRecInfo->stCreInfo.pucData, 0, xCredentialsSize + 1);
				HxSTD_memcpy(pRecInfo->stCreInfo.pucData, pxCredentials, xCredentialsSize);
				pRecInfo->stCreInfo.usSize = xCredentialsSize;
				
				HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d), pxRecordHandle(0x%08x), CredentialSize(%d), xRecordingStatus(%d)\n", __FUNCTION__, __LINE__, i, xTSid, pxRecordHandle, xCredentialsSize, xRecordingStatus);
				hErr = ERR_OK;
				break;
			}
			else
			{
				HxLOG_Critical("HLIB_STD_MemAlloc Error\n");
			}
		}
	}
	return hErr;
}

STATIC HERROR _xmgr_cas_NaCmd_FreeRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			_xmgr_cas_NaCmd_InitRecordSessionByIndex(i);
			hErr = ERR_OK;
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);
			break;
		}
	}
	return hErr;	
}

STATIC HERROR _xmgr_cas_NaCmd_FreeRecordSessionFromTsId(TTransportSessionId xTSid)
{
	int i = 0;
	HERROR hErr = ERR_FAIL;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}
		
		if(pRecInfo->xTSid == xTSid)
		{
			_xmgr_cas_NaCmd_InitRecordSessionByIndex(i); 
			hErr = ERR_OK;
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTSid);
			break;
		}
	}
	return hErr;
}

STATIC XmgrCas_NaCmdRecordSessionInformation_t * _xmgr_cas_NaCmd_GetRecordSessionFromTsId(TTransportSessionId xTSid)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->xTSid == xTSid)
		{
			HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d)\n", __FUNCTION__, __LINE__, i, xTSid);
			return &NaCmdRecInfo[i];
		}
	}
	HxLOG_Debug("[%s][%d] index[%d] = xTSid(%d) is not found\n", __FUNCTION__, __LINE__, i, xTSid);
	return NULL;
}

STATIC XmgrCas_NaCmdRecordSessionInformation_t * _xmgr_cas_NaCmd_GetRecordSessionFromRecordHandle(TDvlHandle* pxRecordHandle)
{
	int i = 0;
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	for(i=0;i<REC_MAX_NUM;i++)
	{
		pRecInfo = &NaCmdRecInfo[i];
		if(pRecInfo == NULL)
		{
			continue;
		}

		if(pRecInfo->pxRecordHandle == pxRecordHandle)
		{
			HxLOG_Debug("[%s][%d] index[%d] = pxRecordHandle(0x%08x)\n", __FUNCTION__, __LINE__, i, pxRecordHandle);
			return &NaCmdRecInfo[i];
		}
	}
	return NULL;	
}

STATIC void _xmgr_cas_NaCmd_dvlRecordErrorCB(TDvlHandle* pxRecordHandle, TDvlRecordStatus  xRecordStatus)
{
	HLIB_CMD_Printf("[%s][%d] pxRecordHandle:[%p], xRecordStatus[%d]\n", __FUNCTION__, __LINE__, pxRecordHandle, xRecordStatus);
	HLIB_CMD_Printf("\n\n\n\n\033[1;92m ############################################################################ \033[0m \n");
	HLIB_CMD_Printf("\t \033[1;92m xRecordStatus : [%s] \033[0m \n", _xmgr_cas_naprm_GetDvlStatusName(xRecordStatus));
	HLIB_CMD_Printf("\033[1;92m ############################################################################ \033[0m \n\n\n\n\n");
}

STATIC void _xmgr_cas_NaCmd_StartRecordSession(int tsid)
{
	TDvlRecordSessionParameters *pxRecordParameters = NULL;
	TSize xOutputCredentialsSize = 0;
	const TUnsignedInt8* pxOutputCredentials = NULL;
	TDvlRecordStatus xRecordingStatus = DVL_RECORD_GRANTED;
	TDvlHandle* pxRecordHandle = NULL;
	static HBOOL bRecInitFlag = TRUE;

	TSize					xSpecificMetadataSize = 3;
	TUnsignedInt8			xSpecificMetadata[3] = {0x12, 0x7a, 0x14};//{0xa5, 0x04, 0x01, 0xf0,  0x04, 0x38};

	if(bRecInitFlag == TRUE)
	{
		_xmgr_cas_NaCmd_InitRecordSession();
		bRecInitFlag = FALSE;
	}

	if(xmgr_cas_NadvlCreateRecordSessionParameters((TDvlRecordSessionParameters**)&pxRecordParameters) == ERR_OK)
	{
		#if 1 // test only
		pxRecordParameters->tsId = tsid; // tsid를 등록해야함..
		pxRecordParameters->pErrorCB = _xmgr_cas_NaCmd_dvlRecordErrorCB;
		pxRecordParameters->collectionId = 0;  // ?????
		pxRecordParameters->inputCredentialsSize = 0; // inpurt credential size는 없음.
		pxRecordParameters->pInputCredentials = NULL; // input credentials은 없음
		pxRecordParameters->specificMetadataSize = xSpecificMetadataSize; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->pSpecificMetadata = &xSpecificMetadata[0]; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->viewingWindowRelativity = VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->viewingWindowDuration = 60; // meta 정보를 얻어와 set해야함.
		pxRecordParameters->retentionDuration = 60; // meta 정보를 얻어와 set해야함.		
		pxRecordParameters->emi = DEFAULT_EMI_FOR_RECORD;
		#endif

		xmgr_cas_naprm_pvr_PrintRecordSessionParameters(pxRecordParameters);

		if(xmgr_cas_NadvlStartRecordEx(pxRecordParameters, &xOutputCredentialsSize, &pxOutputCredentials, &xRecordingStatus, &pxRecordHandle) == ERR_OK)
		{
			if(xOutputCredentialsSize == 0 || pxOutputCredentials == NULL)
			{
				HxLOG_Error("pxOutputCredentials is not fail\n");
				return;
			}

//			xmgr_cas_naprm_PrintHexDump("Outputcredential", (HUINT8*)pxOutputCredentials, (HUINT32)xOutputCredentialsSize);
#if 1
			_xmgr_cas_NaCmd_GetCredentialsInformation(xOutputCredentialsSize, (const TUnsignedInt8 *)pxOutputCredentials);
#endif
			_xmgr_cas_NaCmd_MakeRecordSession(pxRecordParameters, tsid, pxRecordHandle, xOutputCredentialsSize, pxOutputCredentials, xRecordingStatus);

			if(pxOutputCredentials)
			{
				xmgr_cas_NadvlReleaseBuffer(pxOutputCredentials);
			}
		}
		else
		{
			HxLOG_Error("xmgr_cas_NadvlStartRecordEx() is fail\n");
		}
	}
	else
	{
		HxLOG_Error("xmgr_cas_NadvlCreateRecordSessionParameters() is fail\n");		
	}
	_xmgr_cas_NaCmd_PrintRecordSession();
}

STATIC void _xmgr_cas_NaCmd_StopRecordSession(int tsId)
{
	XmgrCas_NaCmdRecordSessionInformation_t *pRecInfo = NULL;

	_xmgr_cas_NaCmd_PrintRecordSession();
	pRecInfo = _xmgr_cas_NaCmd_GetRecordSessionFromTsId(tsId);

	if(pRecInfo)
	{
		(void)xmgr_cas_NadvlStopRecord(pRecInfo->pxRecordHandle);
		(void)_xmgr_cas_NaCmd_FreeRecordSessionFromRecordHandle(pRecInfo->pxRecordHandle);
	}
}
#endif


STATIC int _xmag_cas_NaDvlCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	
	if( aucArg == NULL )
	{
		return HxCMD_ERR;
	}
	
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "PSTART") == 0 )
	{
		int tsid, cred_num;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
				if( aucArg != NULL )
				{
					if( sscanf(aucArg, "%d", &cred_num) != 0 )
					{

						_xmgr_cas_NaCmd_StartVODPlayback(tsid, cred_num);
						return HxCMD_OK;
					}
				}
			}
		}
	}
	else if(strcmp(aucStr, "PSTOP") == 0 )
	{
		_xmgr_cas_NaCmd_StopVODPlayback();
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "PRESTART") == 0 )
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				_xmgr_cas_NaCmd_ReStartVODPlayback(tsid);
				return HxCMD_OK;
			}
		}		
	}	
#if defined(CONFIG_MW_CAS_NAGRA_PRM_VOD_PVR)
	else if(strcmp(aucStr, "RSTART") == 0 )
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				_xmgr_cas_NaCmd_StartRecordSession(tsid);
				return HxCMD_OK;
			}
		}		
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "RSTOP") == 0 )
	{
		int tsid;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &tsid) != 0 )
			{
				_xmgr_cas_NaCmd_StopRecordSession(tsid);
				return HxCMD_OK;
			}
		}		
		return HxCMD_OK;
	}
#endif
	return HxCMD_ERR;
}

void xmgr_cas_NaDvlCmdInit(HxCMD_Handle_t hCmdHandle)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
						_xmag_cas_NaDvlCommand,
						"cmddvl",						/* keyword */
						"Nagra DVL Debug, TV Manager\n cmddvl [cmd] (start[tsid][cerd_num] | stop | restart[tsid])",	/* help   */
						"cmddvl [cmd] ( start [tsid][cerd_num] | stop | restart [tsid])");	/* usage  */
}
#endif
