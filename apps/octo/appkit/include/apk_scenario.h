#ifndef __APK_SCENARIO_H__
#define __APK_SCENARIO_H__

HERROR	APK_SCENARIO_Init(void);
HERROR	APK_SCENARIO_DeInit(void);
HERROR	APK_SCENARIO_GetMainLanguage(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetMainLanguage(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetAudioLanguage(HINT32 nIndex, HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetAudioLanguage(HINT32 nIndex, const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetAudioLanguageUndefined(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetAudioLanguageUndefined(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetAudioLanguageOriginal(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetAudioLanguageOriginal(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetAudioLanguageDefault(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetAudioLanguageDefault(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetSubtitleLanguage(HINT32 nIndex, HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetSubtitleLanguage(HINT32 nIndex, const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetSubtitleLanguageUndefined(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetSubtitleLanguageUndefined(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetSubtitleLanguageOriginal(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetSubtitleLanguageOriginal(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetSubtitleLanguageDefault(HCHAR *pszISO639);
HERROR	APK_SCENARIO_SetSubtitleLanguageDefault(const HCHAR *pszISO639);
HERROR	APK_SCENARIO_GetHardOfHearing(HBOOL *pbEnabled);
HERROR	APK_SCENARIO_SetHardOfHearing(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetMaturityRating(DxRatingAge_e *peRating);
HERROR	APK_SCENARIO_SetMaturityRating(DxRatingAge_e eRating);
HERROR	APK_SCENARIO_GetCountryCode(HxCountry_e *peCountryCode);
HERROR	APK_SCENARIO_SetCountryCode(HxCountry_e eCountryCode);
HERROR	APK_SCENARIO_GetOperatorType(DxOperator_e *peOpType);
HERROR	APK_SCENARIO_SetOperatorType(DxOperator_e eOpType);
HERROR	APK_SCENARIO_GetAntennaConnection(DxAntennaConnection_e *peConnectionType);
HERROR	APK_SCENARIO_SetAntennaConnection(DxAntennaConnection_e eConnectionType);
HERROR	APK_SCENARIO_GetStartPadding(HINT32 *pnPadding);
HERROR	APK_SCENARIO_SetStartPadding(HINT32 nPadding);
HERROR	APK_SCENARIO_GetEndPadding(HINT32 *pnPadding);
HERROR	APK_SCENARIO_SetEndPadding(HINT32 nPadding);
HERROR	APK_SCENARIO_GetAutoDelete(HBOOL *pbAutoDelete);
HERROR	APK_SCENARIO_SetAutoDelete(HBOOL bAutoDelete);
HERROR	APK_SCENARIO_GetPostCode(HCHAR *pszPostcode, HINT32 nMax);
HERROR	APK_SCENARIO_SetPostCode(const HCHAR *pszPostcode);
HERROR	APK_SCENARIO_GetGMTOffset(HINT32 *pnGmtOffset);
HERROR	APK_SCENARIO_SetGMTOffset(HINT32 nGmtOffset);
HERROR	APK_SCENARIO_GetChannelSetupPIN(HBOOL *pbEnabled);
HERROR	APK_SCENARIO_SetChannelSetupPIN(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetGuidancePolicy(DxGuidancePolicy_e *peGuidancePolicy);
HERROR	APK_SCENARIO_SetGuidancePolicy(DxGuidancePolicy_e eGuidancePolicy);
HERROR	APK_SCENARIO_GetSubtitleSetting(HBOOL *pbSubtitle);
HERROR	APK_SCENARIO_SetSubtitleSetting(HBOOL bSubtitle);
HERROR	APK_SCENARIO_GetDayLightSaving(HBOOL *pbEnabled);
HERROR	APK_SCENARIO_SetDayLightSaving(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetTimeUpdate(HBOOL *pnEnabled);
HERROR	APK_SCENARIO_SetTimeUpdate(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetBouquetName(HCHAR *pszBouquet, HINT32 nMax);
HERROR	APK_SCENARIO_SetBouquetName(const HCHAR *pszBouquet);
HERROR	APK_SCENARIO_GetBouquetID(HINT32 *pnBouquetId);
HERROR	APK_SCENARIO_SetBouquetID(HINT32 nBouquetId);
HERROR	APK_SCENARIO_GetSIMode(DxSIMode_e *peSIMode);
HERROR	APK_SCENARIO_SetSIMode(DxSIMode_e eSIMode);
HERROR	APK_SCENARIO_GetRegionCode(HINT32 *pnRegionCode);
HERROR	APK_SCENARIO_SetRegionCode(HINT32 nRegionCode);
HERROR	APK_SCENARIO_GetAdultChannels(DxAdultChannelType_e *peAdultChannel);
HERROR	APK_SCENARIO_SetAdultChannels(DxAdultChannelType_e eAdultChannel);
HERROR	APK_SCENARIO_GetSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax);
HERROR	APK_SCENARIO_RemoveSWUpdateVersion(void);
HERROR	APK_SCENARIO_GetIPSWUpdateVersion(HCHAR *pszSWUpdateVersion, HINT32 nMax);
HERROR	APK_SCENARIO_SetSatRecReceiverId(HINT32 nReceiverId);
HERROR	APK_SCENARIO_GetSatRecUpdateInterval(HUINT32 *pUpdateInterval);
HERROR	APK_SCENARIO_SetSatRecUpdateInterval(DxSatRecIntervalType_e eIntervalType, HUINT16 nTime);
HERROR	APK_SCENARIO_GetTVTVEnable(HBOOL *pbEnabled);
HERROR	APK_SCENARIO_SetTVTVEnable(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetTVTVEPGDownload_StandbyEnable(HBOOL *pbEnabled);
HERROR	APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable(HBOOL bEnabled);
HERROR	APK_SCENARIO_GetFavChListNumberingMode(HBOOL *pbFavChListNumberingMode);
HERROR	APK_SCENARIO_SetFavChListNumberingMode(HBOOL bFavChListNumberingMode);
HERROR	APK_SCENARIO_GetDescrambleOnOff(HBOOL *pbOnOff);
HERROR	APK_SCENARIO_SetDescrambleOnOff(HBOOL bOnOff);
HERROR	APK_SCENARIO_GetNetworkMsgInfo(HCHAR *pszNetworkMsg, HINT32 nMax);
HERROR	APK_SCENARIO_SetNtpServerURL(HCHAR * psNtpUrl);

/* Freeview Connected (FVC) */
HERROR	APK_SCENARIO_GetFvcBaseUrl(HCHAR *pszUrl, HINT32 ulLen);
HERROR	APK_SCENARIO_SetFvcBaseUrl(const HCHAR *pszUrl);
HERROR	APK_SCENARIO_GetFvcAuthHash(HCHAR *str, HINT32 ulLen);
HERROR	APK_SCENARIO_SetFvcAuthHash(const HCHAR *str);
HERROR	APK_SCENARIO_GetFvcAuthTimeStamp(HCHAR *str, HINT32 ulLen);
HERROR	APK_SCENARIO_SetFvcAuthTimeStamp(const HCHAR *str);

HERROR	APK_SCENARIO_GetLiveStreamPriority (DxLiveStreamPriority_e *pePriority);
HERROR	APK_SCENARIO_SetLiveStreamPriority (DxLiveStreamPriority_e ePriority);

HERROR	APK_SCENARIO_GetBatUpdateOnOff(HBOOL *pbOnOff);
HERROR	APK_SCENARIO_SetBatUpdateOnOff(HBOOL bOnOff);
HERROR	APK_SCENARIO_GetNitLcnUpdateOnOff(HBOOL *pbOnOff);
HERROR	APK_SCENARIO_SetNitLcnUpdateOnOff(HBOOL bOnOff);


HERROR	APK_SCENARIO_GetLastServiceGroup(HINT32 *pnLastSvcGroup);
HERROR	APK_SCENARIO_SetLastServiceGroup(HINT32 nLastSvcGroup);

HERROR	APK_SCENARIO_GetAutomaticPowerdown(HUINT32 *pTimeout);
HERROR	APK_SCENARIO_SetAutomaticPowerdown (HUINT32 ulTimeout);


HERROR	APK_SCENARIO_GetPriorityHDMIOutput(HBOOL *pEnable);
HERROR	APK_SCENARIO_SetPriorityHDMIOutput(HBOOL bEnable);

HERROR	APK_SCENARIO_SendKeyFromApp(HUINT32 ulKeySymbol);

#endif // __APK_SCENARIO_H__



