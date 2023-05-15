#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_rscmap.h>
#include <mgr_live.h>
#include <mgr_rc.h>
#include <mgr_media.h>


#include <namgr_main.h>
#include <svc_ch.h>
#include <svc_rec.h>
#include <svc_output.h>
#include <svc_mplayercso.h>

#include <_xmgr_rc_base.h>

#include <otl.h>

#include <hapi.h>


typedef	enum
{
	RC_Outputs_Type_UnKnown = -1,
	RC_Outputs_Type_None = 0,
	RC_Outputs_Type_Live,
	RC_Outputs_Type_App,
	RC_Outputs_Type_Record,
	RC_Outputs_Type_Media,
	RC_Outputs_Type_Max,
}RC_OUTPUTS_Type_e;


#define ___LOCAL_VARIABLES___
STATIC xMgrRCContext_t	s_stRCCtx;

STATIC BUS_Result_t xmgr_rc_base_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


#define ___LOCAL_FUNCTION___
STATIC xMgrRCContext_t* xmgr_rc_base_GetContext (void)
{
	return &s_stRCCtx;
}

STATIC void xmgr_rc_base_ClearContext (xMgrRCContext_t *pstContext)
{
	if (NULL == pstContext)
		return;

	HxSTD_MemSet (pstContext, 0, sizeof(xMgrRCContext_t));
	pstContext->eState	=	eRCState_Idle;

	return ;
}

STATIC HERROR xmgr_rc_base_GetOutputsInfo(void)
{
	HERROR			 		hError = ERR_FAIL;
	xMgrRCContext_t			*pstContext = NULL;
	HUINT32					ulActionId;
	MgrAction_Type_e		eActionType = eActionType_NONE;
	RC_OUTPUTS_Type_e		eOutputType = RC_Outputs_Type_UnKnown;

	Handle_t				hSvc = HANDLE_NULL;

	DbSvc_Info_t			stSvcInfo;

	SvcSi_Event_t			stEventInfo;
	DbSvc_TripleId_t 		stTripleId;

	MgrPb_Info_t 			stPbInfo;

	HUINT32 				i, ulComponentNum;

	pstContext = xmgr_rc_base_GetContext ();

	HxSTD_MemSet(&pstContext->stOutputInfo, 0x00, sizeof(xMgrRC_Outputs_Info_t));

	// Get setup information
	SVC_OUT_GetVolume(&pstContext->stOutputInfo.stSetupInfo.ulVolumne);
	pstContext->stOutputInfo.stSetupInfo.bMute = SVC_OUT_GetMasterAudioMute();
	DB_PARAM_GetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32 *)&pstContext->stOutputInfo.stSetupInfo.bSubtitle, 0);
	DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&pstContext->stOutputInfo.stSetupInfo.bAD, 0);

	// Get Service or Recording currently being played out..
	ulActionId = MGR_ACTION_GetMainActionId();
	eActionType = MGR_ACTION_GetType(ulActionId);

	pstContext->ulActionId = ulActionId;

	switch(eActionType)
	{
		case eViewType_WTV:
		case eViewType_ATV:
		case eViewType_TSR_PB:
		case eViewType_CHASE_PB: 		/**< View : Playing a HDD recoding service like TSR */
			{

				eOutputType = RC_Outputs_Type_Live;

				hError = MGR_LIVE_GetServiceHandle (ulActionId, eLiveViewPlaySvc_MASTER, &hSvc);
				if (hError != ERR_OK)
				{
					break;
				}

				HxSTD_MemSet(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));
				HxSTD_memset(&stEventInfo, 0x00, sizeof(SvcSi_Event_t));

				hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
				if (hError != ERR_OK)
				{
					break;
				}

				stTripleId.usOnId = DbSvc_GetOnId(stSvcInfo);
				stTripleId.usTsId = DbSvc_GetTsId(stSvcInfo);
				stTripleId.usSvcId = DbSvc_GetSvcId(stSvcInfo);

				hError = SVC_SI_GetEvtInfoFromPfEvent(ulActionId, stTripleId, TRUE, TRUE, &stEventInfo, NULL);
				if (hError != ERR_OK)
				{
					break;
				}

				//HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.szSourceId, 0x00, RC_MMGR_MAX_SERVICE_NAME_LEN);
				//HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.szSourceId, RC_MMGR_MAX_SERVICE_NAME_LEN, "%d", stSvcInfo.usSvcIdx);
				pstContext->stOutputInfo.stSvcInfo.usSourceId = stSvcInfo.usSvcIdx;
				pstContext->stOutputInfo.stSvcInfo.nContentId = stEventInfo.nEvtId;
				pstContext->stOutputInfo.stSvcInfo.bAvailable = TRUE;

				HxSTD_MemSet(pstContext->stOutputInfo.stEitInfo.szEvtName, 0x00, RC_MGR_MAX_STR);
				HxSTD_snprintf(pstContext->stOutputInfo.stEitInfo.szEvtName, RC_MGR_MAX_STR, "%s", stEventInfo.szName);
				pstContext->stOutputInfo.stEitInfo.ulStartTime = stEventInfo.ulStartTime;
				pstContext->stOutputInfo.stEitInfo.ulDuration = stEventInfo.ulDuration;
				HxSTD_MemSet(pstContext->stOutputInfo.stEitInfo.szDescription, 0x00, RC_MGR_MAX_STR);
				HxSTD_snprintf(pstContext->stOutputInfo.stEitInfo.szDescription, RC_MGR_MAX_STR, "%s", stEventInfo.szText);
				pstContext->stOutputInfo.stEitInfo.bAvailable;

				ulComponentNum = 0;
				/*Audio*/
				{
					SvcSi_AudioList_t stAudioList;

					HxSTD_MemSet (&stAudioList, 0, sizeof(SvcSi_AudioList_t));

					hError = SVC_SI_GetAudioList (ulActionId, DbSvc_GetSvcId(stSvcInfo), &stAudioList);
					if (ERR_OK == hError)
					{
						for( i=0 ; i<stAudioList.nAudio; i++ )
						{
							pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].eType = eRC_COMPONENT_AUDIO;
							HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 0x00, 4);
							HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 4, "%s", stAudioList.pstAudioInfo[i].stIsoLangInfo[0].szIso639LangCode);

							HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, 0x00, RC_MGR_MAX_COMPONENT_DES);
							HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, RC_MGR_MAX_COMPONENT_DES, "%s", stAudioList.pstAudioInfo[i].aucStreamName);
							ulComponentNum++;
						}
					}
					else
					{
						HxLOG_Error ("SVC_SI_GetAudioList Error. hSvc(0x%08x)\n", hSvc);
					}

					SVC_SI_FreeAudioList(&stAudioList);
				}

				/*Subtitle*/
				{
					SvcSi_SubtitleList_t stSubtitleList;

					HxSTD_MemSet (&stSubtitleList, 0, sizeof(SvcSi_SubtitleList_t));

					hError = SVC_SI_GetSubtitleList (ulActionId, DbSvc_GetSvcId(stSvcInfo), &stSubtitleList);
					if (ERR_OK == hError)
					{
						for( i=0 ; i<stSubtitleList.ulSubtitleNum; i++ )
						{
							pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].eType = eRC_COMPONENT_SUBTITLE;
							switch(stSubtitleList.pstSubtitleInfo[i].eType)
							{
								case eSI_SUBTITLE_DVB:
								{
									HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, 0x00, RC_MGR_MAX_COMPONENT_DES);
									HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, RC_MGR_MAX_COMPONENT_DES, "DVB-Subtitles");

									HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 0x00, 4);
									HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 4, "%s", stSubtitleList.pstSubtitleInfo[i].utArg.stDvbInfo.aucLangCode);

									ulComponentNum++;
								}
								break;

								case eSI_SUBTITLE_EBU:
								{
									HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, 0x00, RC_MGR_MAX_COMPONENT_DES);
									HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, RC_MGR_MAX_COMPONENT_DES, "EBU-Subtitles");

									HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 0x00, 4);
									HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 4, "%s", stSubtitleList.pstSubtitleInfo[i].utArg.stEbuInfo.aucLangCode);

									ulComponentNum++;
								}
								break;

								case eSI_SUBTITLE_SUPERCC:
								default:
									break;
							}
						}
					}
					else
					{
						HxLOG_Error ("SVC_SI_GetSubtitleList Error. hSvc(0x%08x)\n", hSvc);
					}

					SVC_SI_FreeSubtitleList(&stSubtitleList);
				}
				pstContext->stOutputInfo.stSvcInfo.ulComponentNum = ulComponentNum;

				//if(eActionType == eViewType_TSR_PB)
				{
					//UNIXTIME	 	ulCurTime;

					HxSTD_memset(&stPbInfo, 0, sizeof(MgrPb_Info_t));

					//ulCurTime = (UNIXTIME)HLIB_STD_GetSystemTime();
					hError = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
					if (ERR_OK == hError)
					{
						pstContext->stOutputInfo.stTsrInfo.ulTimeStamp = stPbInfo.ulEndTime;
						pstContext->stOutputInfo.stTsrInfo.ulOffset = stPbInfo.ulEndTime - stPbInfo.ulPlayTime;
						if(stPbInfo.nPlaySpeed == 0)
						{
							pstContext->stOutputInfo.stTsrInfo.nSpeed = 0;
						}
						else
						{
							pstContext->stOutputInfo.stTsrInfo.nSpeed = stPbInfo.nPlaySpeed % 100;
						}
						pstContext->stOutputInfo.stTsrInfo.ulBufferDuration = stPbInfo.ulEndTime - stPbInfo.ulStartTime;
						pstContext->stOutputInfo.stTsrInfo.bAvailable = TRUE;
					}
					else
					{
						HxLOG_Error ("MGR_PLAYBACK_GetPlaybackInfo Error. ulActionId(%d)\n", ulActionId);
					}
				}

				// CA Info
				{
					HUINT8					ucSCNum;
					TSmartcardInfoItem		*pxSmartcardInfoArray = NULL;

					hError = MW_CA_NA_GetSystemInfo(NULL, NULL, (TUnsignedInt8*)&ucSCNum, &pxSmartcardInfoArray);
					if(ERR_OK == hError)
					{
						pstContext->stOutputInfo.stCasInfo.ulCasId = (HUINT32)pxSmartcardInfoArray[0].ecmCaSystemId;
						MW_CA_NA_GetCurrScStatus(0, (TSmartcardState*)&pstContext->stOutputInfo.stCasInfo.ulStatus);
						MW_CA_NA_GetAccessStatus((TCaAccess*)&pstContext->stOutputInfo.stCasInfo.ulErrorCode);
						pstContext->stOutputInfo.stCasInfo.bAvailable = TRUE;
					}
					else
					{
						HxLOG_Error ("MW_CA_NA_GetSystemInfo Error. \n");
					}
				}
			}
			break;

		case eViewType_PB:				/**< View : Playing HDD recorded services */
			{
				SvcMeta_Service_t stRecSvcInfo;

				eOutputType = RC_Outputs_Type_Record;
				HxSTD_memset(&stPbInfo, 0, sizeof(MgrPb_Info_t));

				hError = MGR_PLAYBACK_GetPlaybackInfo(ulActionId, &stPbInfo);
				if (ERR_OK == hError)
				{
					pstContext->stOutputInfo.stPbInfo.ulDuration = stPbInfo.ulEndTime - stPbInfo.ulStartTime;
					pstContext->stOutputInfo.stPbInfo.ulCurPos = stPbInfo.ulPlayTime - stPbInfo.ulStartTime;
					if(stPbInfo.nPlaySpeed == 0)
					{
						pstContext->stOutputInfo.stPbInfo.nSpeed = 0;
					}
					else
					{
						pstContext->stOutputInfo.stPbInfo.nSpeed = stPbInfo.nPlaySpeed % 100;
					}
					pstContext->stOutputInfo.stPbInfo.bAvailable = TRUE;

					HxSTD_memset(&stRecSvcInfo, 0, sizeof(SvcMeta_Service_t));
					if (SVC_META_GetServiceInfo(stPbInfo.ulContentId, &stRecSvcInfo) == ERR_OK)
					{
						//HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.szSourceId, 0x00, RC_MMGR_MAX_SERVICE_NAME_LEN);
						//HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.szSourceId, RC_MMGR_MAX_SERVICE_NAME_LEN, "%s", stRecSvcInfo.aucServiceName);
						pstContext->stOutputInfo.stSvcInfo.usSourceId = stRecSvcInfo.usSvcId;
						pstContext->stOutputInfo.stSvcInfo.nContentId = (HINT32)stPbInfo.ulContentId;
						pstContext->stOutputInfo.stSvcInfo.bAvailable = TRUE;

						ulComponentNum = 0;
						/*Audio*/
						{
							for( i=0 ; i < stRecSvcInfo.stAudioInfoList.ulItemNum; i++ )
							{
								pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].eType = eRC_COMPONENT_AUDIO;
								HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 0x00, 4);
								HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 4, "%s", stRecSvcInfo.stAudioInfoList.astAudioInfo[i].stLangInfo[0].szIso639LangCode);

								HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, 0x00, RC_MGR_MAX_COMPONENT_DES);
								ulComponentNum++;
							}
						}

						/*Subtitle*/
						{
							for( i=0 ; i<stRecSvcInfo.stSbtlInfoList.ulItemNum; i++ )
							{
								pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].eType = eRC_COMPONENT_SUBTITLE;
								HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, 0x00, RC_MGR_MAX_COMPONENT_DES);
								HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szDescription, RC_MGR_MAX_COMPONENT_DES, "Subtitles");

								HxSTD_MemSet(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 0x00, 4);
								HxSTD_snprintf(pstContext->stOutputInfo.stSvcInfo.stEsInfo[ulComponentNum].szLang, 4, "%s", stRecSvcInfo.stSbtlInfoList.astSbtlInfo[i].aucLangCode);

								ulComponentNum++;
							}
						}
						pstContext->stOutputInfo.stSvcInfo.ulComponentNum = ulComponentNum;
					}
					else
					{
						HxLOG_Error ("SVC_META_GetServiceInfo Error. ulActionId(%d)\n", ulActionId);
					}
				}
				else
				{
					HxLOG_Error ("MGR_PLAYBACK_GetPlaybackInfo Error. ulActionId(%d)\n", ulActionId);
				}

			}

			break;

		case eViewType_IP:
		case eViewType_MEDIA:
		case eViewType_MOVIE_PB:
			{
				SvcCso_t 	*pMplayer = SVC_CSO_GetActivePlayer(ulActionId);
				HINT8 		*szMediaURI = OxMGR_Malloc(SVC_CSO_MAX_URL_LEN);
				HINT8 		*pszName = NULL;


				eOutputType = RC_Outputs_Type_App;

				if((NULL == pMplayer) || (NULL == szMediaURI))
				{
					eOutputType = RC_Outputs_Type_None;
					return hError;
				}

				HxSTD_MemSet(szMediaURI, 0, 2048);
				HxSTD_snprintf(szMediaURI, SVC_CSO_MAX_URL_LEN, "%s", pMplayer->szUrl);

				HxLOG_Error("\t @@@@ GetOutputsInfos[%d] URL :\n");
				HxLOG_Error("\t\t  %s\n", szMediaURI);
				HxLOG_Error("\t\t  %s\n\n", pMplayer->szUrl);

				if(HxSTD_StrNCmp(szMediaURI, "file://", 7) == 0)
				{
					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szSourceId, 0x00, RC_MGR_MAX_URL);
					HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szSourceId, RC_MGR_MAX_URL, "RECORDED");

					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szContentId, 0x00, RC_MGR_MAX_URL);
					if(eSVC_CSO_STATE_PLAYING == pMplayer->eState)
					{
						HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szContentId, RC_MGR_MAX_URL, "%s", &szMediaURI[7]);
					}

					pstContext->stOutputInfo.stAppsInfo.eAppType = eRC_APPS_EMBEDDED;
				}
				else if(HxSTD_StrNCmp(szMediaURI, "http://", 7) == 0 || HxSTD_StrNCmp(szMediaURI, "https://", 8) == 0)
				{
					pszName = HxSTD_StrRChar(szMediaURI,'?');
					pszName[0] = '0';
					pszName++;

					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szSourceId, 0x00, RC_MGR_MAX_URL);
					HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szSourceId, RC_MGR_MAX_URL, "%s", szMediaURI);

					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szContentId, 0x00, RC_MGR_MAX_URL);
					if(eSVC_CSO_STATE_PLAYING == pMplayer->eState)
					{
						HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szContentId, RC_MGR_MAX_URL, "%s", pszName);
					}
					pstContext->stOutputInfo.stAppsInfo.eAppType = eRC_APPS_HBBTV;
				}
				else if(HxSTD_StrNCmp(szMediaURI, "YOUTUBE", 7) == 0)
				{
					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szSourceId, 0x00, RC_MGR_MAX_URL);
					HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szSourceId, RC_MGR_MAX_URL, "YOUTUBE");

					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szContentId, 0x00, RC_MGR_MAX_URL);
					if(eSVC_CSO_STATE_PLAYING == pMplayer->eState)
					{
						HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szContentId, RC_MGR_MAX_URL, "%s", &szMediaURI[7]);
					}
					pstContext->stOutputInfo.stAppsInfo.eAppType = eRC_APPS_NATIVE;
				}
				else
				{
					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szSourceId, 0x00, RC_MGR_MAX_URL);
					HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szSourceId, RC_MGR_MAX_URL, "OTHER");

					HxSTD_MemSet(pstContext->stOutputInfo.stAppsInfo.szContentId, 0x00, RC_MGR_MAX_URL);
					if(eSVC_CSO_STATE_PLAYING == pMplayer->eState)
					{
						HxSTD_snprintf(pstContext->stOutputInfo.stAppsInfo.szContentId, RC_MGR_MAX_URL, "%s", szMediaURI);
					}
					pstContext->stOutputInfo.stAppsInfo.eAppType = eRC_APPS_OTHER;
				}

				pstContext->stOutputInfo.stAppsInfo.bAvailable = TRUE;

				OxMGR_Free(szMediaURI);
			}
			break;

		case eActionType_NONE:
			#if 1
			eOutputType = RC_Outputs_Type_None;
			#else
			for(i = 0;i< 2;i++)
			{
				hError = DialServer_GetStateApp(i,szAppUrl,szAppName,&ulAppState,&bReady,&ulSvcid,szAppCid);
				if(ulAppState == 1)
				{
					break;
				}
			}
			if(i != 2)
			{

				if(eAmState == eAmState_ACTIVE_STANDBY)
				{
					hError = local_rc_loadElement(RC_ELEMENT_LIST_OUTPUTS_NONE, &pstElement);
					if( hError != ERR_OK )
					{
						HxLOG_Error("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
						return hError;
					}
					eOutputType = RC_Outputs_Type_None;
				}
				else
				{
					// Media 등의 경우에도 None이지만 App state 가 1 인 경우가 존재하는 경우에만 처리하도록 함.
					hError = local_rc_loadElement(RC_ELEMENT_LIST_OUTPUT_3RDPARTY_APP, &pstElement);
					if( hError != ERR_OK )
					{
						HxLOG_Error("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
						return hError;
					}
					eOutputType = RC_Outputs_Type_App;
				}
			}
			else
			{
				hError = local_rc_loadElement(RC_ELEMENT_LIST_OUTPUTS_NONE, &pstElement);
				if( hError != ERR_OK )
				{
					HxLOG_Error("[%s][%d]  load element ERROR \n",__FUNCTION__,__LINE__);
					return hError;
				}
				eOutputType = RC_Outputs_Type_None;

			}
			#endif
			break;

		default :
			return hError;
	}

	return hError;
}


#define XMGR_MAX_RC_LISTS  	3

enum tag_List_ID
{
	eLIST_ID_DVB_ALL = 0,
	eLIST_ID_DVB_DEFAULT_VISIBLE,
	eLIST_ID_DVB_CUSTOM_VISIBLE,
};

static HINT8	*s_pszListId[XMGR_MAX_RC_LISTS] =
{
	"dvb-all",
	"dvb-default-visible",
	"dvb-custom-visible"
};


STATIC HERROR xmgr_rc_base_GetSourceInfos(HINT8 *pszListId, HINT32 *pnSrcNum, xMgrRC_Sources_Info_t **ppstSrcInfos)
{
	HERROR			 		hError = ERR_FAIL;
	DbSvc_Attr_t			stAttr;
	HUINT32					i, ulSvcCnt= 0,ulSvcIdx = 0;
	Handle_t				*phSvcList = NULL;
	DbSvc_Group_e 			eSvcGroup;
	DbSvc_Info_t			*pstServiceInfo = NULL;
	xMgrRC_Sources_Info_t 	*pstSrcInfos = NULL;

	//UNIXTIME				uxCurrentTime;
	//HUINT16					usEvtId;
	//HUINT8					EvtName[RC_MGR_MAX_STR];

	for(i = 0; i < XMGR_MAX_RC_LISTS; i++)
	{
		if(HxSTD_StrCmp(pszListId, s_pszListId[i]) == 0)
		{
			HxLOG_Error(" xmgr_rc_base_GetSourceInfos[%d] => Request List ID(%s)..!!\n", __LINE__, s_pszListId[i]);
			break;
		}
	}

	if(XMGR_MAX_RC_LISTS == i)
	{
		HxLOG_Error(" xmgr_rc_base_GetSourceInfos[%d] => Invalid List ID(%s)..!!\n", __LINE__, pszListId);
		return ERR_FAIL;
	}

	DB_SVC_InitSvcAttribute (&stAttr);
	eSvcGroup = eSvcGroup_All;
	stAttr.eSvcType = eDxSVC_TYPE_All;
	stAttr.eVideoType = eDxVIDEO_TYPE_ALL;
	stAttr.eSatType = eSatType_ALL;
	stAttr.eCasType = eDxCAS_TYPE_All;
	stAttr.eOrgDeliType = eDxDELIVERY_TYPE_ALL;
	stAttr.eSortType = eSortType_NUMBER;

	if(eLIST_ID_DVB_ALL == i)
	{
		stAttr.eVisibleType = eLcnVisibleSvc_FindAll;
		stAttr.eSvcOpType = -1;
		stAttr.eNotSvcOpType = eDxOPERATOR_None;
	}
	else if(eLIST_ID_DVB_DEFAULT_VISIBLE == i)
	{
		stAttr.eVisibleType = eLcnVisibleSvc_FindAll;
		stAttr.eSvcOpType = eDxOPERATOR_Ses;
		stAttr.eNotSvcOpType = eDxOPERATOR_None;
	}
	else
	{
		stAttr.eVisibleType = eLcnVisibleSvc_FindAll;
		stAttr.eSvcOpType = -1;
		stAttr.eNotSvcOpType = eDxOPERATOR_Ses;
	}

	*pnSrcNum = 0;
	*ppstSrcInfos = NULL;

	hError = DB_SVC_FindServiceListWithInfo(eSvcGroup, &stAttr, &ulSvcCnt, &phSvcList, &pstServiceInfo);
	if(hError != ERR_OK)
	{
		//No Channel
		HxLOG_Error(" xmgr_rc_base_GetSourceInfos[%d] => No Channel\n",__LINE__);
	}
	else
	{
		if((pstServiceInfo != NULL) && (ulSvcCnt != 0))
		{
			pstSrcInfos = HLIB_STD_MemAlloc(ulSvcCnt * sizeof(xMgrRC_Sources_Info_t));

			if(NULL == pstSrcInfos)
			{
				HxLOG_Error(" xmgr_rc_base_GetSourceInfos[%d] => MAlloc fail..!!\n", __LINE__);
			}
			else
			{
				HxLOG_Error("\t\t @@@@ [%d] Gathering ServiceInfos => Num : %d\n",__LINE__, ulSvcCnt);
				for(ulSvcIdx=0; ulSvcIdx < ulSvcCnt; ulSvcIdx++)
				{
					//<source sid="505" sref="dvb://2.2045.6301" default-content-id="23DA" logo="http://www.bbc.com/images/bbc1logo.gif" lcn="101" />
					pstSrcInfos[ulSvcIdx].usSid = pstServiceInfo[ulSvcIdx].usSvcIdx;
					pstSrcInfos[ulSvcIdx].usLcn = pstServiceInfo[ulSvcIdx].usLcn;
					HxSTD_snprintf(pstSrcInfos[ulSvcIdx].szSvcName, RC_MMGR_MAX_SERVICE_NAME_LEN, "%s", pstServiceInfo[ulSvcIdx].szSvcName); //sid

					/*
					if(VK_CLOCK_GetTime((unsigned long*)&uxCurrentTime) == ERR_OK)
					{
						hError = MW_EPG_GetEventIdAndNameByTime(hSvc, uxCurrentTime, &usEvtId, EvtName, EVT_NAME_LEN_MAX);
						if(hError == ERR_OK)
						{
			 				HxSTD_snprintf(szAttrBuf[2], STRING_LEN_MAX, "%X",usEvtId); //default-content-id (current time event id)
							st_rcAttr.pucAttr3 = szAttrBuf[2];
						}
					}
					*/
				}

				*pnSrcNum = (HINT32)ulSvcCnt;
				*ppstSrcInfos = pstSrcInfos;
			}
		}
		else
		{
			//No Channel
			HxLOG_Error(" xmgr_rc_base_GetSourceInfos[%d] => No Channel\n",__LINE__);
		}

	}

	DB_SVC_FreeServiceInfoList(NULL, phSvcList, pstServiceInfo);

	return ERR_OK;
}


/*
STATIC HERROR xmgr_rc_base_Setup(xMgrRC_Outputs_Setup_t	*pstSetupInfo)
{
	HERROR			 		hError = ERR_FAIL;

	hError = SVC_OUT_SetVolume(pstSetupInfo->ulVolumne);
	hError = SVC_OUT_SetMasterAudioMute(pstSetupInfo->bMute);

	hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_SUBTITLE_SETTING, (HUINT32)pstSetupInfo->bSubtitle, 0);
	hError = DB_PARAM_SetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32)pstSetupInfo->bAD, 0);

	return hError;
}
*/


STATIC HERROR xmgr_rc_base_SetChannel(xMgrRC_Outputs_Program_t *pstSvcInfo)
{
	HERROR			 		hError = ERR_FAIL;
	Handle_t				hSvc = HANDLE_NULL;
	HUINT16					usSvcIdx;

	HUINT32					ulActionId;

	usSvcIdx = pstSvcInfo->usSourceId;

	hError = DB_SVC_GetServiceHandleBySvcIdx(usSvcIdx, &hSvc);
	if(hError == ERR_OK)
	{
		// Get Service or Recording currently being played out..
		ulActionId = MGR_ACTION_GetMainActionId();

		hError = MGR_LIVE_CmdSendServiceStartByActionId(ulActionId, hSvc);
	}

	return hError;
}


STATIC HERROR xmgr_rc_base_UpdateServices(HUINT32 ulSvcNum, xMgrRC_Sources_Info_t *pstSrcInfo)
{
	HERROR			 		hError = ERR_FAIL;
	Handle_t				hSvc = HANDLE_NULL;
	HUINT16					i, usSvcIdx;
	DbSvc_Info_t			stSvcInfo;

	for(i = 0; i < ulSvcNum; i++)
	{
		usSvcIdx = pstSrcInfo[i].usSid;
		hError = DB_SVC_GetServiceHandleBySvcIdx(usSvcIdx, &hSvc);
		if(hError == ERR_OK)
		{
			hError = DB_SVC_GetServiceInfo(hSvc, &stSvcInfo);
			if(hError == ERR_OK)
			{
				stSvcInfo.usLcn = pstSrcInfo[i].usLcn;
				hError = DB_SVC_UpdateServiceInfo(hSvc, &stSvcInfo);
			}
		}
	}

	return hError;
}

#if 0
STATIC HERROR xmgr_rc_base_LaunchApp(xMgrRC_Outputs_Apps_t *pstAppInfo)
{
	HERROR			 		hError = ERR_FAIL;
	HUINT32					ulActionId;

	HUINT8					szUrl[RC_MGR_MAX_URL];
	HUINT8					szSid[RC_MGR_MAX_URL];

	MGR_MPlayer_t *player = NULL;

	HxSTD_MemSet(szSid, 0x00, RC_MGR_MAX_URL);
	HxSTD_snprintf(szSid, RC_MGR_MAX_URL, "%s", pstAppInfo->szSourceId);

	HxSTD_MemSet(szUrl, 0x00, RC_MGR_MAX_URL);
	if(HxSTD_StrNCmp(szSid, "RECORDED", 8) == 0)
	{
		HxSTD_snprintf(szUrl, RC_MGR_MAX_URL, "file://%s", pstAppInfo->szContentId);
	}
	else if(HxSTD_StrNCmp(szSid, "http://", 7) == 0 || HxSTD_StrNCmp(szSid, "https://", 8) == 0)
	{
		HxSTD_snprintf(szUrl, RC_MGR_MAX_URL, "%s?%s", szSid, pstAppInfo->szContentId);
	}
	else if(HxSTD_StrNCmp(szSid, "YOUTUBE", 7) == 0)
	{
		HxSTD_snprintf(szUrl, RC_MGR_MAX_URL, "%s%s", szSid, pstAppInfo->szContentId);
	}
	else
	{
		HxSTD_snprintf(szUrl, RC_MGR_MAX_URL, "%s", pstAppInfo->szContentId);
	}

	ulActionId = MGR_ACTION_GetMainActionId();

	player = MGR_MPlayer_GetActivePlayer(ulActionId);
	if ( player == NULL )
	{
		HxLOG_Error("MGR_MPlayer_Create is fail..!\n");
		return hError;
	}

	hError = MGR_MPlayer_Probe(player, szUrl, eSVC_CSO_PLAYTYPE_AUTO);
	if ( hError != ERR_OK )
	{
		HxLOG_Error("MGR_MPlayer_Probe is fail(0x%X)..!\n", hError);
		return hError;
	}

	hError = MGR_MPlayer_Play(player, 0);
	if (hError != ERR_OK)
	{
		HxLOG_Error("MGR_MPlayer_Play is fail(0x%X)..!\n", hError);
	}

	return hError;
}
#endif

STATIC HERROR xmgr_rc_base_GetCaInfo(xMgrRC_CA_Info_t *pstCaInfo)
{
	HERROR					hError = ERR_FAIL;
	HUINT8					ucSCNum;
	TSmartcardInfoItem		*pxSmartcardInfoArray = NULL;
	NaChipsetInfo_t 		*pstChipInfo = NULL;
	TIrdInfo				*pstTIrdInfo = NULL;

	hError = MW_CA_NA_GetChipsetInfo(&pstChipInfo);
	if(ERR_OK == hError)
	{
		HxSTD_snprintf(pstCaInfo->aucCsId, RC_MGR_MAX_CA_STR, "%s", pstChipInfo->pcNuid);
		HxSTD_snprintf(pstCaInfo->aucCsType, RC_MGR_MAX_CA_STR, "%s", pstChipInfo->pcChipsetType);
		HxSTD_snprintf(pstCaInfo->aucCsVer, RC_MGR_MAX_CA_STR, "%s", pstChipInfo->pcChipsetRev);
	}

	hError = MW_CA_NA_GetSystemInfo(NULL, &pstTIrdInfo, (TUnsignedInt8*)&ucSCNum, &pxSmartcardInfoArray);
	if(ERR_OK == hError)
	{
		HxSTD_snprintf(pstCaInfo->aucCaVender, RC_MGR_MAX_CA_STR, "%s", pstTIrdInfo->pProjectName);
		HxSTD_snprintf(pstCaInfo->aucCaVer, RC_MGR_MAX_CA_STR, "%s", pstTIrdInfo->pVersion);
		HxSTD_snprintf(pstCaInfo->aucCaSN, RC_MGR_MAX_CA_STR, "%s", pstTIrdInfo->pSerialNumber);

		if(ucSCNum > 0)
		{
			pstCaInfo->bScInserted = TRUE;
			HxSTD_snprintf(pstCaInfo->aucScType, RC_MGR_MAX_CA_STR, "%s", pxSmartcardInfoArray[0].pVersion);
			HxSTD_snprintf(pstCaInfo->aucScNUm, RC_MGR_MAX_CA_STR, "%s", pxSmartcardInfoArray[0].pSerialNumber);

			MW_CA_NA_GetCurrScStatus(0, (TSmartcardState*)&pstCaInfo->ulScStatus);
			if(pstCaInfo->ulScStatus == CA_SMARTCARD_STATE_INVALID)
			{
				pstCaInfo->bScSuitable = FALSE;
			}
			else
			{
				pstCaInfo->bScSuitable = TRUE;
			}
		}
		else
		{
			pstCaInfo->bScInserted = FALSE;
		}
	}
	else
	{
		HxLOG_Error ("MW_CA_NA_GetSystemInfo Error. \n");
	}

	return hError;
}

/*
STATIC HBOOL xmgr_rc_base_CheckValidPidList (HINT32 nPidsCount, HUINT16 *pusPids)
{
	if ( (nPidsCount <= 0) || (DxRSV_PIDTABLE_COUNT <= nPidsCount) )
	{
		HxLOG_Error ("Error. : nPidsCount is invaild (%d)\n", nPidsCount);
		return FALSE;
	}

	if (NULL == pusPids)
	{
		HxLOG_Error ("Error. : Invaild Arguments\n");
		return FALSE;
	}

	return TRUE;
}


STATIC HERROR xmgr_rc_base_SetupAction (HUINT32 ulActionId, MgrRC_Setup_t *pstSetup)
{
	HERROR hErr = ERR_FAIL;

	hErr = MGR_ACTION_SetupRC (ulActionId, &pstSetup->stTuneParam, eAmPriority_VIEW_ACTION);
	if (hErr != ERR_OK)
	{
		HxLOG_Error ("can't take MGR_ACTION_SetupRC() err=%d\n", hErr);
		return hErr;
	}

	return ERR_OK;
}
*/

static HERROR	__rc_signal_onRcGetOutputs(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT8 *pszRequestApi = NULL;
	HINT8 *pszRequestItem = NULL;

	HxLOG_Error("__rc_signal_onRcRequest\n");

	HxLOG_Assert(nArgc == 2);

	pszRequestApi = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));
	pszRequestItem = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[1]));

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_GET_OUTPUTS, 0, (HINT32)pszRequestApi, (HINT32)pszRequestItem, 0);

	return ERR_OK;
}

/*
static HERROR	__rc_signal_onRcSetup(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	void					*pData;
	xMgrRC_Outputs_Setup_t	*pstSetupInfo = NULL;

	HxLOG_Error("__rc_signal_onRcSetup\n");

	HxLOG_Assert(nArgc == 1);

	pData = (void*)HxOBJECT_BIN_DATA(apArgv[0]);
	pstSetupInfo = HLIB_STD_MemAlloc(sizeof(xMgrRC_Outputs_Setup_t));
	HxSTD_MemCopy(pstSetupInfo, pData, sizeof(xMgrRC_Outputs_Setup_t));

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_SETUP, 0, (HINT32)pstSetupInfo, 0, 0);

	return ERR_OK;
}
*/

static HERROR	__rc_signal_onRcSetChannel(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	void						*pData;
	xMgrRC_Outputs_Program_t	*pstSvcInfo = NULL;

	HxLOG_Error("__rc_signal_onRcSetChannel\n");

	HxLOG_Assert(nArgc == 1);

	pData = (void*)HxOBJECT_BIN_DATA(apArgv[0]);
	pstSvcInfo = HLIB_STD_MemAlloc(sizeof(xMgrRC_Outputs_Program_t));
	HxSTD_MemCopy(pstSvcInfo, pData, sizeof(xMgrRC_Outputs_Program_t));

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_SET_CAHNNEL, 0, (HINT32)pstSvcInfo, 0, 0);

	return ERR_OK;
}

static HERROR	__rc_signal_onRcGetSources(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HINT8 *pszListApi = NULL;
	HINT8 *pszSources = NULL;

	HxLOG_Error("__rc_signal_onRcGetSources\n");

	HxLOG_Assert(nArgc == 2);

	pszListApi = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));
	pszSources = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[1]));

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_GET_SOURCES, 0, (HINT32)pszListApi, (HINT32)pszSources, 0);

	return ERR_OK;
}

static HERROR	__rc_signal_onRcUpdateSources(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	xMgrRC_Sources_Info_t  		*pstSrcInfos;
	HINT32						nSrcNum = 0;
	HINT8 						*pszSources = NULL;

	HxLOG_Error("__rc_signal_onRcUpdateSources\n");

	HxLOG_Assert(nArgc == 3);

	pszSources = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));
	nSrcNum = HxOBJECT_INT(apArgv[1]);
	pstSrcInfos = (xMgrRC_Sources_Info_t*)HxOBJECT_BIN_DATA(apArgv[2]);

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_GET_OUTPUTS, 0, (HINT32)pszSources, nSrcNum, (HINT32)pstSrcInfos);

	return ERR_OK;
}

/*
static HERROR	__rc_signal_onRcLaunchApp(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	void						*pData;
	xMgrRC_Outputs_Apps_t		*pstAppInfo = NULL;

	HxLOG_Error("__rc_signal_onRcSetChannel\n");

	HxLOG_Assert(nArgc == 1);

	pData = (void*)HxOBJECT_BIN_DATA(apArgv[0]);
	pstAppInfo = HLIB_STD_MemAlloc(sizeof(xMgrRC_Outputs_Apps_t));
	HxSTD_MemCopy(pstAppInfo, pData, sizeof(xMgrRC_Outputs_Apps_t));

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_LAUNCH_APP, 0, (HINT32)pstAppInfo, 0, 0);

	return ERR_OK;
}
*/

static HERROR	__rc_signal_onRcGetCaInfo(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	void						*pData;

	HxLOG_Error("__rc_signal_onRcGetCaInfo\n");

	HxLOG_Assert(nArgc == 1);

	pData = (void*)HxOBJECT_BIN_DATA(apArgv[0]);

	BUS_SendMessage((BUS_Callback_t)xmgr_rc_base_Proc, eMEVT_RC_GET_CA_INFO, 0, 0, 0, 0);

	return ERR_OK;
}



#define ___MSG_FUNCTION___
STATIC BUS_Result_t xmgr_rc_base_msgBusCreate (xMgrRCContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HERROR				hErr		= 	ERR_OK;


	/* setup memrec pstContext */
	{
		pstContext->eState			= eRCState_ReqStart;
		pstContext->pfActionProc	= BUS_MGR_GetMgrCallback(BUS_MGR_GetThis());
	}

	HAPI_ConnectSignal("signal:onRcRequestOutputs", NULL, __rc_signal_onRcGetOutputs);
	//PI_ConnectSignal("signal:onRcSetup", NULL, __rc_signal_onRcSetup);
	HAPI_ConnectSignal("signal:onRcSetChannel", NULL, __rc_signal_onRcSetChannel);

	HAPI_ConnectSignal("signal:onRcRequestSources", NULL, __rc_signal_onRcGetSources);
	HAPI_ConnectSignal("signal:onRcUpdateSources", NULL, __rc_signal_onRcUpdateSources);

	//HAPI_ConnectSignal("signal:onRcLaunchApp", NULL, __rc_signal_onRcLaunchApp);
	HAPI_ConnectSignal("signal:onRcRequestCaInfo", NULL, __rc_signal_onRcGetCaInfo);

	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_BREAK;
}

STATIC BUS_Result_t xmgr_rc_base_msgBusDestroy (xMgrRCContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HERROR hErr = ERR_FAIL;

#if 0
	if (eRCState_Running <= pstContext->eState)
	{
		/* stop service */
		hErr = SVC_REC_Stop (pstContext->hAction, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_REC_Stop. Error!!!\n");
		}

		hErr = SVC_REC_Close(pstContext->hAction, TRUE);
		if (ERR_OK != hErr)
		{
			HxLOG_Error("SVC_REC_Close. Error!!!\n");
		}
	}

	hErr = SVC_CH_StopTune (pstContext->hAction, (HUINT16)-1, eSyncMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("SVC_CH_StopTune. Error!!!\n");
	}
#endif

	/* emit event */
	//BUS_SendMessage (NULL, eMEVT_RC_NOTIFY_START_STOPPED, 0, 0, 0, 0);

	/* clean context */
	xmgr_rc_base_ClearContext (pstContext);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_BREAK;
}

#if 0
STATIC BUS_Result_t xmgr_rc_base_evtChLocked (xMgrRCContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if(eRCState_Running == pstContext->eState)
	{
		HxLOG_Warning("rc already running... (ulRsvSlotId = %d)\n", pstContext->stSetup.ulRsvSlotId);
		return MESSAGE_PASS;
	}


	{
		HERROR hErr;
		HINT32 i;
		SvcRec_PIDList_t	stRecPidInfo;

		SvcRec_Setup_t stSetup;
		HxSTD_MemSet(&stSetup, 0, sizeof(SvcRec_Setup_t));

		stSetup.eRecActType =	eRec_ActType_Satip;
		stSetup.eRecOutType =	eRec_OutType_MemoryBuf;

		stSetup.eSvcType	=	eDxSVC_TYPE_TV;
		stSetup.bEncryption =	FALSE;
		stSetup.eVideoCodec =	eDxVIDEO_CODEC_MPEG2;
		stSetup.ulStartTime =	0;

		stSetup.unOutArg.hMemoryBuf =	pstContext->stSetup.hRingBuf;


#if defined(CONFIG_DEBUG)
		HxLOG_Debug ("------------<XMGR_RC:START>------------\n");
		HxLOG_Debug ("\t[hAction\t\t:\t0x%x]\n", pstContext->hAction);
		HxLOG_Debug ("\t[hRingBuf\t\t:\t0x%x]\n", pstContext->stSetup.hRingBuf);
		HxLOG_Debug ("------------<START:XMGR_RC>------------\n");
#endif

		hErr = SVC_REC_Setup (pstContext->hAction, &stSetup, TRUE);
		if (ERR_OK != hErr)
			return hErr;

		SVC_REC_CleanReqPidInfo (FALSE, &stRecPidInfo);

		/*PID copy*/
		if (-1 == pstContext->stSetup.nPIDsCount)
		{
			//ALL_PID
			for (i=0 ; i<DxRSV_PIDTABLE_COUNT ; i++ )
			{
				stRecPidInfo.stUnknown.stPIDs[i].usPid =  i;
			}
		}
		else
		{
			for (i=0 ; i<pstContext->stSetup.nPIDsCount ; i++ )
			{
				stRecPidInfo.stUnknown.stPIDs[i].usPid =  pstContext->stSetup.usPIDs[i];
			}
		}

		SVC_REC_SetPID( pstContext->hAction, &stRecPidInfo, TRUE );
		SVC_REC_Start( pstContext->hAction, TRUE );
	}

	/* emit event */
	BUS_SendMessage(NULL, eMEVT_RC_NOTIFY_STARTED, pstContext->hAction, 0, 0, 0);

	pstContext->eState = eRCState_Running;

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);

	return MESSAGE_PASS;
}

STATIC BUS_Result_t xmgr_rc_base_evtChNoSignal (xMgrRCContext_t *pstContext, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Error ("NoSignal !!!\n");

#if 0
	/* emit event */
	BUS_SendMessage(NULL, eMEVT_RC_NOTIFY_START_FAILED, pstContext->hAction, eMgrRCFailed_NOSIGNAL, pstContext->stSetup.ulRsvSlotId, 0);

	NOT_USED_PARAM (hAction);
	NOT_USED_PARAM (p1);
	NOT_USED_PARAM (p2);
	NOT_USED_PARAM (p3);
#else
	HxLOG_Error ("Waiting for signal re-connection...\n");
#endif

	return MESSAGE_PASS;
}
#endif

#define ___PROC_FUNCTION___
STATIC BUS_Result_t xmgr_rc_base_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t eRet = MESSAGE_PASS;

	xMgrRCContext_t			*pstContext = xmgr_rc_base_GetContext ();

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : RC : CREATE \n");
		HxLOG_Warning ("********************************************************************************\n");

		HxLOG_Error("xmgr_rc_base_Proc : MESSAGE : eMEVT_BUS_CREATE\n");
		eRet = xmgr_rc_base_msgBusCreate (pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_BUS_DESTROY:
		HxLOG_Warning ("********************************************************************************\n");
		HxLOG_Warning ("*  XMGR : RC : DESTROY \n");
		HxLOG_Warning ("********************************************************************************\n");

		HxLOG_Info("MESSAGE : eMEVT_BUS_DESTROY\n");
		eRet = xmgr_rc_base_msgBusDestroy (pstContext, hAction, p1, p2, p3);
		break;

	case eMEVT_RC_GET_OUTPUTS:
		HxLOG_Error("eMEVT_RC_GET_OUTPUTS => p1(%s), p2(%s) \n", (HINT8*)p1, (HINT8*)p2);
		if(HxSTD_StrStr((HINT8*)p1, "/rc/outputs/0") != NULL)
		{
			xmgr_rc_base_GetOutputsInfo();

			HxLOG_Error("HAPI_BroadcastSignal => signal:onRcReplyOutputs\n");
			HAPI_BroadcastSignal("signal:onRcReplyOutputs", 0, "b", &pstContext->stOutputInfo, sizeof(xMgrRC_Outputs_Info_t));
		}
		HLIB_STD_MemFree((void*)p1);
		HLIB_STD_MemFree((void*)p2);
		break;


/*
	case eMEVT_RC_SETUP:
		{
			xMgrRC_Outputs_Setup_t	*pstSetupInfo = (xMgrRC_Outputs_Setup_t*)p1;

			HxLOG_Error("eMEVT_RC_SETUP \n");

			xmgr_rc_base_Setup(pstSetupInfo);

			HLIB_STD_MemFree((void*)p1);
		}
		break;
*/

	case eMEVT_RC_SET_CAHNNEL:
		{
			xMgrRC_Outputs_Program_t *pstSvcInfo = (xMgrRC_Outputs_Program_t*)p1;

			HxLOG_Error("eMEVT_RC_SET_CAHNNEL \n");

			xmgr_rc_base_SetChannel(pstSvcInfo);

			//HxLOG_Error("HAPI_BroadcastSignal => signal:onRcChChanged\n");
			//HAPI_BroadcastSignal("signal:onRcChChanged", 0, "i", result_value);  // result [0 OK | 1 Parental Lock..]

			HLIB_STD_MemFree((void*)p1);
		}
		break;

	case eMEVT_RC_GET_SOURCES:
		HxLOG_Error("eMEVT_RC_GET_SOURCES => p1(%s), p2(%s) \n", (HINT8*)p1, (HINT8*)p2);
		if((HxSTD_StrStr((HINT8*)p1, "/rc/sources") != NULL) || (HxSTD_StrStr((HINT8*)p1, "/rc/sourcelists") != NULL))
		{
			HINT32					nSrcNum;
			xMgrRC_Sources_Info_t	*pstSrcInfo = NULL;
			if(xmgr_rc_base_GetSourceInfos((HINT8*)p2, &nSrcNum, &pstSrcInfo) == ERR_OK)
			{
				HxLOG_Error("HAPI_BroadcastSignal => signal:onRcReplySources(OK)\n");
				HAPI_BroadcastSignal("signal:onRcReplySources", 0, "ib", nSrcNum, pstSrcInfo, (sizeof(xMgrRC_Sources_Info_t) * nSrcNum));
			}
			else
			{
				HxLOG_Error("HAPI_BroadcastSignal => signal:onRcReplySources(FAIL)\n");
				HAPI_BroadcastSignal("signal:onRcReplySources", 0, "ib", -1, 0, 0);
			}

			if(NULL != pstSrcInfo)
			{
				HLIB_STD_MemFree((void*)pstSrcInfo);
			}
		}
		else
		{
			HxLOG_Error("HAPI_BroadcastSignal => signal:onRcReplySources(FAIL)\n");
			HAPI_BroadcastSignal("signal:onRcReplySources", 0, "ib", -1, 0, 0);
		}
		HLIB_STD_MemFree((void*)p1);
		HLIB_STD_MemFree((void*)p2);
		break;

	case eMEVT_RC_UPDATE_SOURCES:
		{
			xMgrRC_Sources_Info_t	*pstSrcInfos = (xMgrRC_Sources_Info_t*)p3;
			HxLOG_Error("eMEVT_RC_UPDATE_SOURCES => p1(%s), p2(%d) \n", (HINT8*)p1, p2);

			xmgr_rc_base_UpdateServices(p2, pstSrcInfos);

			HLIB_STD_MemFree((void*)p1);
			HLIB_STD_MemFree((void*)p3);
		}
		break;

	/*
	case eMEVT_RC_LAUNCH_APP:
		{
			xMgrRC_Outputs_Apps_t *pstAppInfo = (xMgrRC_Outputs_Apps_t*)p1;

			HxLOG_Error("eMEVT_RC_LAUNCH_APP \n");

			xmgr_rc_base_LaunchApp(pstAppInfo);

			//HxLOG_Error("HAPI_BroadcastSignal => signal:onRcChChanged\n");
			//HAPI_BroadcastSignal("signal:onRcChChanged", 0, "i", result_value);  // result [0 OK | 1 Parental Lock..]

			HLIB_STD_MemFree((void*)p1);
		}
		break;
	*/

	case eMEVT_RC_GET_CA_INFO:
		{
			xMgrRC_CA_Info_t stCaInfo;

			HxLOG_Error("eMEVT_RC_GET_CA_INFO \n");

			xmgr_rc_base_GetCaInfo(&stCaInfo);

			HxLOG_Error("HAPI_BroadcastSignal => signal:onRcReplyCaInfo\n");
			HAPI_BroadcastSignal("signal:onRcReplyCaInfo", 0, "b", &stCaInfo, sizeof(xMgrRC_CA_Info_t));  // result [0 OK | 1 Parental Lock..]
		}
		break;

/*
	case eSEVT_CH_LOCKED :
		CHECK_RC_ACTION_HANDLE();
		HxLOG_Info("MESSAGE : eSEVT_CH_LOCKED\n");
		eRet = xmgr_rc_base_evtChLocked (pstContext, hAction, p1, p2, p3);
		break;
	case eSEVT_CH_NO_SIGNAL :
		CHECK_RC_ACTION_HANDLE();
		HxLOG_Info("MESSAGE : eSEVT_CH_NO_SIGNAL\n");
		eRet = xmgr_rc_base_evtChNoSignal (pstContext, hAction, p1, p2, p3);
		break;
*/
	default:
		break;
	}

	return (MESSAGE_BREAK == eRet) ? MESSAGE_BREAK : MESSAGE_PASS;
}

#define ___PLUGIN_FUNCTION___
HERROR xmgr_rc_Init_BASE (void)
{
	/* Clear Context */
	xmgr_rc_base_ClearContext (&s_stRCCtx);

	return ERR_OK;
}

HERROR xmgr_rc_Start_BASE (void)
{
	HERROR hErr = ERR_FAIL;

	/* Create MSG Proc */
	{
		hErr = BUS_MGR_Create (	"RC_Mgr",
								APP_ACTION_PRIORITY,
								(BUS_Callback_t)xmgr_rc_base_Proc,
								0,
								0,
								0,
								0);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("Error. : Create Proc\n");
		}
	}

	return hErr;
}

#if 0
HERROR xmgr_rc_ChangePids_BASE (HUINT32 ulActionId, HINT32 nPidsCount, HUINT16 *pusPids)
{
	HINT32 				i;
	SvcRec_PIDList_t	stRecPidInfo;
	xMgrRCContext_t* pstContext = NULL;

	pstContext = xmgr_rc_base_GetContext (ulActionId - eActionId_REC_FIRST);
	if (NULL == pstContext)
		return ERR_FAIL;
/*
	if (eRCState_Running > pstContext->eState)
	{
		HxLOG_Error ("!!! RC not running !!!\n");
		return ERR_FAIL;
	}
*/
	if( xmgr_rc_base_CheckValidPidList (nPidsCount, pusPids)	== FALSE )
		return ERR_FAIL;

	/*PID copy*/
	SVC_REC_CleanReqPidInfo (FALSE, &stRecPidInfo);
		for (i=0 ; i<nPidsCount ; i++ )
		{
			stRecPidInfo.stUnknown.stPIDs[i].usPid = pusPids[i];
		}

	SVC_REC_SetPID( pstContext->hAction, &stRecPidInfo, TRUE );

	return ERR_OK;
}
#endif

HERROR xmgr_rc_Stop_BASE (void)
{
	xMgrRCContext_t* pstContext = NULL;

	pstContext = xmgr_rc_base_GetContext ();

	if (NULL != pstContext)
	{
		BUS_MGR_Destroy (pstContext->pfActionProc);

		pstContext->eState = eRCState_Stopped;


		HxLOG_Debug("Debug. RC STOP!\n");
	}
	else
	{
		HxLOG_Error("Error. not found RC's context\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
