/*
*@ File: tc_appkit_sysinfo.c
*@ Author: Peter, vdthe@humaxdigital.com
*@ Date: 10-Feb-2017 2:35 PM
*@ Description: TC for appkit_sysinfo: STB info and Tuner Info
*/

#define ___HEADER______________________________________________________________________

#include "atkit.h"

#define ___DEFINE______________________________________________________________________

#define MAX_BUFFSIZE 1024

#define ___STATIC_VARIABLES____________________________________________________________


static HxJSON_t s_manifest = NULL;


void TC_ColdBootDone_fnNotifier(void)
{
	printf("ColdBootDone fnNotifier \n");
}


HBOOL TC_APK_SYSINFO_STB_AntennaPower(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStastus = FALSE;
	HBOOL bGetStatus = FALSE; 

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStastus = HLIB_JSON_Object_GetBoolean(j_argument, "AntennaPower");
		
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}
	hErr = APK_SYSINFO_STB_GetAntennaPower(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetAntennaPower()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(bOriginStatus)
	{
		printf("AntennaPower current status: TRUE\n");
	}
	else
	{
		printf("AntennaPower current status: FALSE\n");
	}
	hErr = APK_SYSINFO_STB_SetAntennaPower(bSetStastus);
	if(hErr != ERR_OK)
	{
	    printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_SetAntennaPower()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	hErr = APK_SYSINFO_STB_GetAntennaPower(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetAntennaPower()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(bSetStastus != bGetStatus)
	{
		printf("[%s] TEST_FALL: Fail to Set/Get AntennaPower\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}
	APK_SYSINFO_STB_SetAntennaPower(bOriginStatus);
	return TRUE;
}

HBOOL TC_APK_SYSINFO_STB_FirstBootOnProduction(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStastus = FALSE;
	HBOOL bGetStatus = FALSE; 

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStastus = HLIB_JSON_Object_GetBoolean(j_argument, "FirstBootOnProduction");
		
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}
	hErr = APK_SYSINFO_STB_GetFirstBootOnProduction(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetFirstBootOnProduction()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	if(bOriginStatus)
	{
		printf("FirstBootOnProduction current status: TRUE\n");
	}
	else
	{
		printf("FirstBootOnProduction current status: FALSE\n");
	}
	hErr = APK_SYSINFO_STB_SetFirstBootOnProduction(bSetStastus);
	if(hErr != ERR_OK)
	{
	    printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_SetFirstBootOnProduction()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	hErr = APK_SYSINFO_STB_GetFirstBootOnProduction(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetFirstBootOnProduction()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(bSetStastus != bGetStatus)
	{
		printf("[%s] TEST_FALL: Fail to Set/Get FirstBootOnProduction\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}
	APK_SYSINFO_STB_SetFirstBootOnProduction(bOriginStatus);
	return TRUE;
}

#if defined(CONFIG_NETFLIX_APP)

HBOOL TC_APK_SYSINFO_STB_NetflixESN(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HCHAR szNetFlixESN[128] = {0,};

	hErr = APK_SYSINFO_STB_GetNetflixESN(szNetFlixESN, 128);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetNetFlixESN() -Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("NetFlixESN: %s \n", szNetFlixESN);
	return TRUE;
}

#endif

HBOOL TC_APK_SYSINFO_STB_ServiceUpdateFlag(HxJSON_t j_args)
{
	HERROR hErr = ERR_FAIL;
	HxJSON_t j_argument = NULL;
	HBOOL bOriginStatus = FALSE;
	HBOOL bSetStastus = FALSE;
	HBOOL bGetStatus = FALSE; 

	j_argument = HLIB_JSON_Object_GetObject(j_args, "argument");
	if(j_argument != NULL)
	{
		bSetStastus = HLIB_JSON_Object_GetBoolean(j_argument, "ServiceUpdateFlag");
		
	}
	else
	{
		printf("[%s] TESTING.. : argument is NULL\n", __FUNCTION__);

	}
	hErr = APK_SYSINFO_STB_GetServiceUpdateFlag(&bOriginStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetServiceUpdateFlag()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(bOriginStatus)
	{
		printf("ServiceUpdateFlag current status: TRUE\n");
	}
	else
	{
		printf("ServiceUpdateFlag current status: FALSE\n");
	}

	hErr = APK_SYSINFO_STB_SetServiceUpdateFlag(bSetStastus);
	if(hErr != ERR_OK)
	{
	    printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetServiceUpdateFlag()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	hErr = APK_SYSINFO_STB_GetServiceUpdateFlag(&bGetStatus);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetServiceUpdateFlag()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}

	if(bSetStastus != bGetStatus)
	{
		printf("[%s] TEST_FALL: Fail to Set/Get ServiceUpdateFlag\n", __FUNCTION__);
		fflush(stdout);
		return FALSE;
	}
	APK_SYSINFO_STB_SetServiceUpdateFlag(bOriginStatus);
	return TRUE;
}

HBOOL TC_APK_SYSINFO_STB_GetInfo(HxJSON_t j_args)
{	
	HERROR hErr = ERR_FAIL;
	HCHAR szBuff[MAX_BUFFSIZE] = {0, };
	HBOOL bColdBootDone = FALSE;
	HUINT32 ulCustomCode = 0;
	HUINT32 ulLaunchAppMode = 0;
	HUINT8 ucSCS = 0, ucJTAG = 0, ucOTP = 0, ucCWE = 0;
	HUINT32 ulMarketId = 0;

	/* Get Model name */
	hErr = APK_SYSINFO_STB_GetModelName(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetModelName()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Model name: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);	

	/* Get Update list */
	hErr = APK_SYSINFO_STB_GetUpdatedList(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetUpdateList()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Updated List: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Software version */
	hErr = APK_SYSINFO_STB_GetSoftwareVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetSoftwareVersion()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Software version: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Packet version */
	hErr = APK_SYSINFO_STB_GetPackageVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetPacketVersion()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Package Version: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Loader name */
	hErr = APK_SYSINFO_STB_GetLoaderName(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetLoaderName()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Loader name: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Loader version */
	hErr = APK_SYSINFO_STB_GetLoaderVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetLoaderVersion()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Loader version: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Upgrader version */
	hErr = APK_SYSINFO_STB_GetUpgraderVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetUpgraderVersion- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Upgrader version: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);	

	/* Get Manufacture name */
	hErr = APK_SYSINFO_STB_GetManufacturerName(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetManufactureName()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}	
	printf("[STB_INFO]Manufacturer name: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get System Id */
	hErr = APK_SYSINFO_STB_GetSystemId(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetSystemId()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]System Id: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Device Id */
	hErr = APK_SYSINFO_STB_GetDeviceId(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetDeviceId()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Device Id: %s\n", szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);	

	/* Get ChipId */
	hErr = APK_SYSINFO_STB_GetChipId(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetChipId()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]ChipId: %s\n",szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Obama version */
	hErr = APK_SYSINFO_STB_GetObamaVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetObamaVersion()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Obama version: %s\n",szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Micom version */
	hErr = APK_SYSINFO_STB_GetMicomVersion(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetObamaVersion()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("Micom version: %s\n",szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Update date*/
	hErr = APK_SYSINFO_STB_GetUpdateDate(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetUpdateDate()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Update Date: %s\n",szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Serial number */
	hErr = APK_SYSINFO_STB_GetSerialNumber(szBuff, MAX_BUFFSIZE);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetSerialNumber()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Serial number: %s\n",szBuff);
	memset(szBuff, 0x00, MAX_BUFFSIZE);

	/* Get Custom code */
	hErr = APK_SYSINFO_STB_GetCustomCode(&ulCustomCode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetCustomCode()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Custom code: %d\n", ulCustomCode);

	/* Get Fusing info */
	hErr = APK_SYSINFO_STB_GetFusingInfo (&ucSCS, &ucJTAG, &ucOTP, &ucCWE, &ulMarketId);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetFusingInfo()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Fusing info - SCS:%d, JTAG: %d, OTP: %d, CWE: %d, MarketId: %d\n", ucSCS, ucJTAG, ucOTP, ucCWE, ulMarketId);

	/* Get Launch app mode */
	hErr = APK_SYSINFO_STB_GetLaunchAppMode(&ulLaunchAppMode);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetLauchAppMode()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[STB_INFO]Launch app mode: %d\n", ulLaunchAppMode);

	/*Get ColdBootDone status*/
	hErr = APK_SYSINFO_STB_GetColdBootDone(&bColdBootDone, TC_ColdBootDone_fnNotifier);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_STB_GetColdBootDone()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	if(bColdBootDone)
	{
		printf("[STB_INFO]ColdBootDone: TRUE\n");
	}
	else
	{
		printf("[STB_INFO]ColdBootDone: FALSE\n");
	}


	return TRUE;
}

HBOOL TC_APK_SYSINFO_TUNER_GetInfo(HxJSON_t jManifest)
{
	HERROR hErr = ERR_FAIL;
	HINT32 nTunerId = 0;
	HUINT32 ulDeviceNum = 0;
	DxDeliveryType_e eDevType = eDxDELIVERY_TYPE_ALL;
	HBOOL bLocked = FALSE;
	HUINT32 ulSignalLevel = 0;
	HUINT32 ulSignalQuality = 0;
	HINT32 nAgc = 0;
	HFLOAT32 fBer = 0.0f;
	HFLOAT32 fSnr = 0.0f;
	HFLOAT32 fInputPower = 0.0f;
	DxTuneParam_t stTuneParam;
	HINT32 i = 0;
	/* TUNNER Get Device number 		*/
	hErr = APK_SYSINFO_TUNER_GetDeviceNum(&ulDeviceNum);
	if(hErr != ERR_OK)
	{
		printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetDeviceNum()- Err(%d)\n", __FUNCTION__, hErr);
		fflush(stdout);
		return FALSE;
	}
	printf("[TUNER_INFO] Device number: %d\n", ulDeviceNum);
	for(i = 0; i < ulDeviceNum; i++)
	{

		printf("******** TESTING for tuner %d ***********\n", i+1);

		/*	TUNNER Get Device type 			*/
		hErr = APK_SYSINFO_TUNER_GetDevType(nTunerId, &eDevType);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetDevType()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		switch(eDevType)
		{
			case eDxDELIVERY_TYPE_SAT:
				printf("[TUNER_INFO] Delivery type SAT\n");
				break;
			case eDxDELIVERY_TYPE_TER:
				printf("[TUNER_INFO] Delivery type TER\n");
				break;
			case eDxDELIVERY_TYPE_CAB:
				printf("[TUNER_INFO] Delivery type CAB\n");
				break;
			case eDxDELIVERY_TYPE_MULTICAST:
				printf("[TUNER_INFO] Delivery type MULTICAST\n");
				break;
			default:
				printf("[TUNER_INFO] Unknow Delivery type\n");
				break;						
		}

		/*	TUNNER Is locked 				*/
		hErr = APK_SYSINFO_TUNER_IsLocked(nTunerId, &bLocked);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_IsLocked()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		if(bLocked)
		{
			printf("[TUNER_INFO] Islocked: TRUE\n");
		}
		else
		{
			printf("[TUNER_INFO] IsLocked: FALSE\n");
		}

		/*	TUNNER Get signal level 		*/
		hErr = APK_SYSINFO_TUNER_GetSignalLevel(nTunerId, &ulSignalLevel);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetSignalLevel()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] Signal level: %d\n", ulSignalLevel);

		/* 	TUNNER Get signal quality 		*/
		hErr = APK_SYSINFO_TUNER_GetSignalQuality(nTunerId, &ulSignalQuality);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetSignalQuality()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] Signal quality: %d\n", ulSignalQuality);

		/*	TUNNER Get Agc 					*/
		hErr = APK_SYSINFO_TUNER_GetAgc(nTunerId, &nAgc);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetAgc()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] Agc: %d\n", nAgc);	

		/*	TUNNER Get Ber 					*/
		hErr = APK_SYSINFO_TUNER_GetBer(nTunerId, &fBer);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetBer()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] Ber: %f\n", fBer);

		/*	TUNNER Get SNR					*/
		hErr = APK_SYSINFO_TUNER_GetSnr(nTunerId, &fSnr);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetSnr()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] SNR: %f\n", fSnr);

		/*	TUNNER Get Input Power 			*/
		hErr = APK_SYSINFO_TUNER_GetInputPower(nTunerId, &fInputPower);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetInputPower()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		printf("[TUNER_INFO] Input Power: %f\n", fInputPower);

		/*	TUNNER Get Input Param 			*/
		memset(&stTuneParam, 0, sizeof(stTuneParam));
		hErr = APK_SYSINFO_TUNER_GetTuneParam(nTunerId, &stTuneParam);
		if(hErr != ERR_OK)
		{
			printf("[%s] TEST_FALL: Fail to APK_SYSINFO_TUNER_GetInputPower()- Err(%d)\n", __FUNCTION__, hErr);
			fflush(stdout);
			return FALSE;
		}
		switch(stTuneParam.eDeliType)
		{
			case eDxDELIVERY_TYPE_SAT:
				printf("[TUNER_INFO] Tunner groupd Id: %d\n"
					   "			 Tunner Rf Input Id: %d \n"
					   "			 Antenna Type: %d\n"
					   "[DVB-S]		 Frequency: %d\n"
					   "             Symbol Rate: %d\n"
					   	 ,stTuneParam.sat.ulTunerGroupId
					   	 ,stTuneParam.sat.ulRfInputId
					   	 ,stTuneParam.sat.antennaType
					   	 ,stTuneParam.sat.tuningInfo.ulFrequency
					   	 ,stTuneParam.sat.tuningInfo.ulSymbolRate);
				break;
			case eDxDELIVERY_TYPE_TER:
				printf("[TUNER_INFO] Tuner group Id: %d\n"
					   "             Frequency: %d\n"
						 ,stTuneParam.ter.ulTunerGroupId
						 ,stTuneParam.ter.ulFrequency);
				switch(stTuneParam.ter.eBandWidth)
				{
					case eDxTER_BANDWIDTH_8M:
						printf("	         Bandwidth: 8MHz\n");
						break;
					case eDxTER_BANDWIDTH_7M:
						printf("	         Bandwidth: 7MHz\n");
						break;
					case eDxTER_BANDWIDTH_6M:
						printf("	         Bandwidth: 6MHz\n");
						break;
					case eDxTER_T2BANDWIDTH_5MHZ:
						printf("			 Bandwidth DVB-T2 5MHz\n");
						break;
					case eDxTER_T2BANDWIDTH_10MHZ:
						printf("			 Bandwidth DVB-T2 10MHz\n");
						break;
					case eDxTER_T2BANDWIDTH_1P7MHZ:
						printf("			 Bandwidth DVB-T2 1P7MHz\n");
						break;
					default:
						break;			
				}
				break;

			case eDxDELIVERY_TYPE_CAB:
				printf("[TUNER_INFO] Tuner group Id: %d\n"
						"			 Frequency: %d\n"
						"			 Symbolrate: %d\n"
					,stTuneParam.cab.ulTunerGroupId
					,stTuneParam.cab.ulFrequency
					,stTuneParam.cab.ulSymbolRate);
				break;

#if defined(CONFIG_MW_CH_IP)
			case eDxDELIVERY_TYPE_MULTICAST:
				printf("[TUNER_INFO] Tuner group Id: %d\n"
						"			 Ip Channel URL: %s\n"
					,stTuneParam.ip.ulTunerGroupId
					,stTuneParam.ip.szIpChannelURL);
				break;
#endif
			default:
				break;
		}
		
		nTunerId ++;
		fflush(stdout);
	}

	/*	TUNNER Get Tunner Id by TripleId */



	return TRUE;
}
void TC_APPKIT_SYSINFO_Init(HxJSON_t jManifest)
{
	HERROR hErr = ERR_FAIL;

	printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
	s_manifest = jManifest;

	hErr = APK_SYSINFO_STB_Init();
	if(hErr != ERR_OK)
	{
		printf("APK_SYSINFO_STB_Init - Err (%d)\n", hErr);
	}
	hErr = APK_SYSINFO_TUNER_Init();
	if(hErr != ERR_OK)
	{
		printf("APK_SYSINFO_TUNNER_Init -Err(%d)\n", hErr);
	}
	HLIB_STD_TaskSleep(2000); // Waiting for get infomation

	hErr = APK_SYSINFO_STB_Initialized();
	if(hErr != ERR_OK)
	{
		printf("APK_SYSINFO_STB_Initialized- Err (%d)\n", hErr);
	}	

	ADD_TC(TC_APK_SYSINFO_STB_AntennaPower);
	ADD_TC(TC_APK_SYSINFO_STB_FirstBootOnProduction);
#if defined(CONFIG_NETFLIX_APP)
	ADD_TC(TC_APK_SYSINFO_STB_NetflixESN);
#endif
	ADD_TC(TC_APK_SYSINFO_STB_ServiceUpdateFlag);
	ADD_TC(TC_APK_SYSINFO_STB_GetInfo);
	ADD_TC(TC_APK_SYSINFO_TUNER_GetInfo);
}

void TC_APPKIT_SYSINFO_DeInit(void)
{
    printf("[%s]\n", __FUNCTION__);
	fflush(stdout);
}





