#include <apk.h>


#include <vkernel.h>

#include <dbus-c++/dbus.h>
#include <as_apk_scenario_base.h>

#define MAX_BUFFSIZE	1024

char const * const cApkScenarioBase::BUSNAME_APK_SCENARIO_BASE = "Octopus.Appkit.Scenario.Base";
char const * const cApkScenarioBase::BUSPATH_APK_SCENARIO_BASE = "/Octopus/Appkit/Scenario/Base";

cApkScenarioBase::cApkScenarioBase(DBus::Connection &connection)
  : DBus::ObjectAdaptor(connection, BUSPATH_APK_SCENARIO_BASE)
{
	/* apkit scenario base */
    {	
		HERROR hErr	=	ERR_FAIL;
		hErr = APK_SCENARIO_Init ();
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("APK_SCENARIO_Init Error : %d", hErr);
		}		
    }
}

std::string cApkScenarioBase::GetMainLanguage()
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetMainLanguage (szBuff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetMainLanguage Error : %d", hErr);
	}	

	return std::string(szBuff);
}

void cApkScenarioBase::SetMainLanguage(const std::string& language)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetMainLanguage ((const HCHAR *)&language[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetMainLanguage Error : %d", hErr);
	}	

	return ;
}

std::string cApkScenarioBase::GetAudioLanguage(const int32_t& index)
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetAudioLanguage ((HINT32)index, szBuff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetAudioLanguage Error : %d", hErr);
	}	

	return std::string(szBuff);
}

void cApkScenarioBase::SetAudioLanguage(const int32_t& index, const std::string& language)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetAudioLanguage ((HINT32)index, (const HCHAR *)&language[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetAudioLanguage Error : %d", hErr);
	}	

	return ;
}

std::string cApkScenarioBase::GetSubtitleLanguage(const int32_t& index)
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetSubtitleLanguage ((HINT32)index, szBuff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetSubtitleLanguage Error : %d", hErr);
	}	

	return std::string(szBuff);
}

void cApkScenarioBase::SetSubtitleLanguage(const int32_t& index, const std::string& language)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetSubtitleLanguage ((HINT32)index, (const HCHAR *)&language[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetSubtitleLanguage Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetHardOfHearing()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled	=	FALSE;
		
	hErr = APK_SCENARIO_GetHardOfHearing (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetHardOfHearing Error : %d", hErr);
	}	

	return (bool)bEnabled;
}

void cApkScenarioBase::SetHardOfHearing(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetHardOfHearing ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetHardOfHearing Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetMaturityRating()
{
	HERROR hErr =	ERR_FAIL;

	DxRatingAge_e eRating	=	eDxRATING_VIEW_ALL;
		
	hErr = APK_SCENARIO_GetMaturityRating (&eRating);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetMaturityRating Error : %d", hErr);
	}	

	return (uint32_t)eRating;
}

void cApkScenarioBase::SetMaturityRating(const uint32_t& rating)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetMaturityRating ((DxRatingAge_e)rating);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetMaturityRating Error : %d", hErr);
	}	
	
	return ;
}

uint32_t cApkScenarioBase::GetCountryCode()
{
	HERROR hErr =	ERR_FAIL;

	HxCountry_e eCountryCode	=	eCountryID_NONE;
	
	hErr = APK_SCENARIO_GetCountryCode (&eCountryCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetCountryCode Error : %d", hErr);
	}	

	return (uint32_t)eCountryCode;
}

void cApkScenarioBase::SetCountryCode(const uint32_t& countryCode)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetCountryCode ((HxCountry_e)countryCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetCountryCode Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetOperatorType()
{
	HERROR hErr =	ERR_FAIL;

	DxOperator_e eOpType	=	eDxOPERATOR_None;
	
	hErr = APK_SCENARIO_GetOperatorType (&eOpType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetOperatorType Error : %d", hErr);
	}	

	return (uint32_t)eOpType;
}

void cApkScenarioBase::SetOperatorType(const uint32_t& opType)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetOperatorType ((DxOperator_e)opType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetOperatorType Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetAntennaConnection()
{
	HERROR hErr =	ERR_FAIL;

	DxAntennaConnection_e eConnectionType	=	eDxANTCONN_ONECABLE;
		
	hErr = APK_SCENARIO_GetAntennaConnection (&eConnectionType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetAntennaConnection Error : %d", hErr);
	}	

	return (uint32_t)eConnectionType;
}

void cApkScenarioBase::SetAntennaConnection(const uint32_t& connectionType)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetAntennaConnection ((DxAntennaConnection_e)connectionType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetAntennaConnection Error : %d", hErr);
	}	

	return ;
}

int32_t cApkScenarioBase::GetStartPadding()
{
	HERROR hErr =	ERR_FAIL;

	HINT32 nPadding	=	0;
		
	hErr = APK_SCENARIO_GetStartPadding (&nPadding);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetStartPadding Error : %d", hErr);
	}	
	
	return (HINT32)nPadding;
}

void cApkScenarioBase::SetStartPadding(const int32_t& padding)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetStartPadding ((HINT32)padding);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetStartPadding Error : %d", hErr);
	}	

	return ;
}

int32_t cApkScenarioBase::GetEndPadding()
{
	HERROR hErr =	ERR_FAIL;

	HINT32 nPadding	=	0;
		
	hErr = APK_SCENARIO_GetEndPadding (&nPadding);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetEndPadding Error : %d", hErr);
	}	

	return (int32_t)nPadding;
}

void cApkScenarioBase::SetEndPadding(const int32_t& padding)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetEndPadding ((HINT32)padding);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetEndPadding Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetAutoDelete()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bAutoDelete = FALSE;
		
	hErr = APK_SCENARIO_GetAutoDelete (&bAutoDelete);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetAutoDelete Error : %d", hErr);
	}	

	return (bool)bAutoDelete;
}

void cApkScenarioBase::SetAutoDelete(const bool& autoDelete)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetAutoDelete ((HBOOL)autoDelete);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetAutoDelete Error : %d", hErr);
	}

	return ;
}

std::string cApkScenarioBase::GetPostCode()
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetPostCode (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetPostCode Error : %d", hErr);
	}	

	return std::string(szBuff);
}

void cApkScenarioBase::SetPostCode(const std::string& postcode)
{
	HERROR hErr =	ERR_FAIL;

	hErr = APK_SCENARIO_SetPostCode ((const HCHAR *)&postcode[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetPostCode Error : %d", hErr);
	}	
	
	return ;
}

int32_t cApkScenarioBase::GetGMTOffset()
{
	HERROR hErr =	ERR_FAIL;

	HINT32 nGmtOffset	=	0;
		
	hErr = APK_SCENARIO_GetGMTOffset (&nGmtOffset);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetGMTOffset Error : %d", hErr);
	}	

	return (int32_t)nGmtOffset;
}

void cApkScenarioBase::SetGMTOffset(const int32_t& gmtOffset)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetGMTOffset ((HINT32)gmtOffset);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetGMTOffset Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetChannelSetupPIN()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled	=	FALSE;
		
	hErr = APK_SCENARIO_GetChannelSetupPIN (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetChannelSetupPIN Error : %d", hErr);
	}	

	return (bool)bEnabled;
}

void cApkScenarioBase::SetChannelSetupPIN(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetChannelSetupPIN ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetChannelSetupPIN Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetGuidancePolicy()
{
	HERROR hErr =	ERR_FAIL;

	DxGuidancePolicy_e eGuidancePolicy	=	eDxGUIDANCEPOLICY_OFF;
		
	hErr = APK_SCENARIO_GetGuidancePolicy (&eGuidancePolicy);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetGuidancePolicy Error : %d", hErr);
	}	

	return (uint32_t)eGuidancePolicy;
}

void cApkScenarioBase::SetGuidancePolicy(const uint32_t& guidancePolicy)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetGuidancePolicy ((DxGuidancePolicy_e)guidancePolicy);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetGuidancePolicy Error : %d", hErr);
	}

	return ;
}

bool cApkScenarioBase::GetSubtitleSetting()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bSubtitle	=	FALSE;
	
	hErr = APK_SCENARIO_GetSubtitleSetting (&bSubtitle);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetSubtitleSetting Error : %d", hErr);
	}	

	return (bool)bSubtitle;
}

void cApkScenarioBase::SetSubtitleSetting(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetSubtitleSetting ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetSubtitleSetting Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetDayLightSaving()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled;
	
	hErr = APK_SCENARIO_GetDayLightSaving (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetDayLightSaving Error : %d", hErr);
	}	
	
	return (bool)bEnabled;
}

void cApkScenarioBase::SetDayLightSaving(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetDayLightSaving ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetDayLightSaving Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetTimeUpdate()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled	=	FALSE;
		
	hErr = APK_SCENARIO_GetTimeUpdate (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetTimeUpdate Error : %d", hErr);
	}	

	return (bool)bEnabled;
}

void cApkScenarioBase::SetTimeUpdate(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetTimeUpdate ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetTimeUpdate Error : %d", hErr);
	}	

	return ;
}

std::string cApkScenarioBase::GetBouquetName()
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetBouquetName (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetBouquetName Error : %d", hErr);
	}	

	return std::string(szBuff);
}

void cApkScenarioBase::SetBouquetName(const std::string& bouquet)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetBouquetName ((const HCHAR *)&bouquet[0]);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetBouquetName Error : %d", hErr);
	}	

	return ;
}

int32_t cApkScenarioBase::GetBouquetID()
{
	HERROR hErr =	ERR_FAIL;

	HINT32 nBouquetId	=	0;
		
	hErr = APK_SCENARIO_GetBouquetID (&nBouquetId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetBouquetID Error : %d", hErr);
	}	

	return (int32_t)nBouquetId;
}

void cApkScenarioBase::SetBouquetID(const int32_t& bouquetId)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetBouquetID ((HINT32)bouquetId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetBouquetID Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetSIMode()
{
	HERROR hErr =	ERR_FAIL;

	DxSIMode_e eSIMode	=	eDxSIMODE_UNKNOWN;
		
	hErr = APK_SCENARIO_GetSIMode (&eSIMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetSIMode Error : %d", hErr);
	}	

	return (uint32_t)eSIMode;
}

void cApkScenarioBase::SetSIMode(const uint32_t& SIMode)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetSIMode ((DxSIMode_e)SIMode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetSIMode Error : %d", hErr);
	}	

	return ;
}

int32_t cApkScenarioBase::GetRegionCode()
{
	HERROR hErr =	ERR_FAIL;

	HINT32 nRegionCode	=	0;
		
	hErr = APK_SCENARIO_GetRegionCode (&nRegionCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetRegionCode Error : %d", hErr);
	}	

	return (int32_t)nRegionCode;
}

void cApkScenarioBase::SetRegionCode(const int32_t& regionCode)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetRegionCode ((HINT32)regionCode);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetRegionCode Error : %d", hErr);
	}	

	return ;
}

uint32_t cApkScenarioBase::GetAdultChannels()
{
	HERROR hErr =	ERR_FAIL;

	DxAdultChannelType_e eAdultChannel	=	eDxADULTCHANNEL_DISPLAY;
	
	hErr = APK_SCENARIO_GetAdultChannels (&eAdultChannel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetAdultChannels Error : %d", hErr);
	}	

	return (uint32_t)eAdultChannel;
}

void cApkScenarioBase::SetAdultChannels(const uint32_t& adultChannel)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetAdultChannels ((DxAdultChannelType_e)adultChannel);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetAdultChannels Error : %d", hErr);
	}	

	return ;
}

std::string cApkScenarioBase::GetSWUpdateVersion()
{
	HERROR hErr =	ERR_FAIL;

	HCHAR szBuff[MAX_BUFFSIZE] = {0,};

	hErr = APK_SCENARIO_GetSWUpdateVersion (szBuff, MAX_BUFFSIZE);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetSWUpdateVersion Error : %d", hErr);
	}	
	
	return std::string(szBuff);
}

void cApkScenarioBase::SetSatRecReceiverId(const int32_t& receiverId)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetSatRecReceiverId ((HINT32)receiverId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetSatRecReceiverId Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetTVTVEnable()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled	=	FALSE;
		
	hErr = APK_SCENARIO_GetTVTVEnable (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetTVTVEnable Error : %d", hErr);
	}	

	return (bool)bEnabled;
}

void cApkScenarioBase::SetTVTVEnable(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetTVTVEnable ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetTVTVEnable Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetTVTVEPGDownload_StandbyEnable()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bEnabled	=	FALSE;
	
	hErr = APK_SCENARIO_GetTVTVEPGDownload_StandbyEnable (&bEnabled);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetTVTVEPGDownload_StandbyEnable Error : %d", hErr);
	}	

	return (bool)bEnabled;
}

void cApkScenarioBase::SetTVTVEPGDownload_StandbyEnable(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetTVTVEPGDownload_StandbyEnable Error : %d", hErr);
	}	

	return ;
}

bool cApkScenarioBase::GetDescrambleOnOff()
{
	HERROR hErr =	ERR_FAIL;

	HBOOL bOnOff	=	FALSE;

	hErr = APK_SCENARIO_GetDescrambleOnOff (&bOnOff);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_GetDescrambleOnOff Error : %d", hErr);
	}	

	return (bool)bOnOff;
}

void cApkScenarioBase::SetDescrambleOnOff(const bool& enable)
{
	HERROR hErr =	ERR_FAIL;
	hErr = APK_SCENARIO_SetDescrambleOnOff ((HBOOL)enable);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("APK_SCENARIO_SetDescrambleOnOff Error : %d", hErr);
	}

	return ;
}
