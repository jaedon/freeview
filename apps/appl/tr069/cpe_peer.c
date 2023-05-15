/**************************************************************
 *	@file		cpe_peer.c
 *	TR-069 Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/07/20
 *	@author			humax
 *	@brief			peer function that will be call by iCube library
 **************************************************************/
#include	<hlib.h>
#include <hapi.h>

#include <CWMP_STB_API.h>
#include <CWMP_CPE_API.h>
#include "trport_common.h"


CWMP_STB_RESULT Cwmp_Stb_open()
{
	HERROR hErr;

	TRENTER;

	hErr = TRP_DB_Init();
	hErr = TRP_LoadDownloadInfosFromDB();

#if 0//def CONFIG_DEBUG
	hErr = TRP_TEST_CreateDeviceJson();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to create Device Object!!\n");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}
#endif

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_close ()
{
//	HERROR hErr;


	TRENTER;
	TRP_DB_Deinit();

	HxLOG_Debug("End Cwmp_Stb_close()\r\n");
#if 0//def CONFIG_DEBUG
	hErr = TRP_TEST_DestroyDeviceJson();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("Fail to destroy Device Object!!\n");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}
#endif

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_getParam (char *key, char **output, int nextlevel)
{
	TRENTER;

	if(output == NULL)
	{
		Critical("Null pointer!!\n");
		return CWMP_STB_RES_FAIL;
	}

	HxLOG_Debug(" key:[%s], nextlevel:[%d]\n", key, nextlevel);

	*output = TRP_GetParam(key);
	if(*output==NULL)
	{
		HxLOG_Error("couldn't get parameter value!\n");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_setParam (char *input)
{
	HERROR hErr = ERR_OK;

	TRENTER;

	hErr = TRP_SetParam(input);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("couldn't set parameter value!\n");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_addObject (char *key, unsigned int *num)
{
	HxLOG_Error(" STB_API: %s was called, but not implemented\n",__FUNCTION__);
	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_deleteObject (char *key)
{
	HxLOG_Error(" STB_API: %s was called, but not implemented\n",__FUNCTION__);
	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_primeDownload(CWMP_STB_DOWNLOAD_TYPE download_type,	char *target_file_name,	 int file_size, char **download_path, char **download_id)
{
	HERROR hErr = ERR_OK;

	TRENTER;
	HxLOG_Debug("download_type[%d] target_file_name[%s] file_size[%d]\n",download_type, target_file_name, file_size);

	if(download_path == NULL && download_id == NULL)
	{
		HxLOG_Critical("NULL pointer!!\n");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}

	switch(download_type)
	{
		case CWMP_STB_DL_FIRMWARE:
			hErr = TRP_GetDownloadPath(eDOWNLOAD_TYPE_FIRMWARE, target_file_name, file_size, download_path, download_id);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("couldn't get download info![%s:%d]\n", __FUNCTION__, __LINE__);
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			break;

		case CWMP_STB_DL_X_JCOM_CFG:
			hErr = TRP_GetDownloadPath(eDOWNLOAD_TYPE_X_JCOM_CFG, target_file_name, file_size, download_path, download_id);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("couldn't get download info![%s:%d]\n", __FUNCTION__, __LINE__);
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			break;

		case CWMP_STB_DL_X_JLABS_AUDIENCE_MEASUREMENT_CFG:
			hErr = TRP_GetDownloadPath(eDOWNLOAD_TYPE_X_JLABS_AM_CFG, target_file_name, file_size, download_path, download_id);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("couldn't get download info![%s:%d]\n", __FUNCTION__, __LINE__);
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			break;

		case CWMP_STB_DL_X_JCOM_PrivateMessage:
			hErr = TRP_GetDownloadPath(eDOWNLOAD_TYPE_X_JCOM_PrivateMessage, target_file_name, file_size, download_path, download_id);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("couldn't get download info![%s:%d]\n", __FUNCTION__, __LINE__);
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			break;

		case CWMP_STB_DL_VENDOR_CFG:
			break;

		case CWMP_STB_DL_WEBCONTENT:
		case CWMP_STB_DL_TONE:
		case CWMP_STB_DL_RINGER:
		default:
			HxLOG_Error("Download type is not supported! [%d]", download_type);//not phase 1
			TRLEAVE(CWMP_STB_RES_FAIL);
			break;
	}

	HxLOG_Debug("download_path[%s] download_id[%s]\n",*download_path, *download_id);
	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_applyDownload (char *download_id)
{
	HERROR hErr = ERR_OK;
	trport_download_type_e	type;

	TRENTER;
	CHECK_NULL(download_id);

	type = (trport_download_type_e)HLIB_STD_StrToUINT32(download_id);
	switch(type)
	{
		case eDOWNLOAD_TYPE_FIRMWARE:
			hErr = TRP_ApplyFirmware(type);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Fail to apply firmware download!!\n");
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			//retoob이 필요한 경우 pending을 return 한다.
			TRLEAVE(CWMP_STB_RES_PENDING);
			break;

		case eDOWNLOAD_TYPE_X_JCOM_CFG:
			hErr = TRP_ApplySTBconfig(type);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Fail to apply STBconfig download!!\n");
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			//retoob이 필요한 경우 pending을 return 한다.
			TRLEAVE(CWMP_STB_RES_PENDING);
			break;

		case eDOWNLOAD_TYPE_X_JLABS_AM_CFG:
			TRLEAVE(CWMP_STB_RES_SUCCESS);
			break;

		case eDOWNLOAD_TYPE_X_JCOM_PrivateMessage:
		case eDOWNLOAD_TYPE_VENDOR_CFG:
			TRLEAVE(CWMP_STB_RES_SUCCESS);
			break;

		case eDOWNLOAD_TYPE_WEBCONTENT:
		case eDOWNLOAD_TYPE_TONE:
		case eDOWNLOAD_TYPE_RINGER:
		default:
			HxLOG_Error("Download type is not supported! [%d]", type);//not phase 1
			TRLEAVE(CWMP_STB_RES_FAIL);
			break;
	}

	return CWMP_STB_RES_SUCCESS;
}

CWMP_STB_RESULT Cwmp_Stb_checkDownload (char *download_id)
{
	HERROR hErr = ERR_OK;
	trport_download_type_e	type;

	TRENTER;
	CHECK_NULL(download_id);

	type = (trport_download_type_e)HLIB_STD_StrToUINT32(download_id);

	switch(type)
	{
		case eDOWNLOAD_TYPE_FIRMWARE:
			hErr = TRP_CheckFirmware(type);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("Fail to apply firmware download!!\n");
				TRLEAVE(CWMP_STB_RES_FAIL);
			}
			break;
		case eDOWNLOAD_TYPE_X_JCOM_CFG:
		case eDOWNLOAD_TYPE_X_JLABS_AM_CFG:
			HxLOG_Debug("Cwmp_Stb_checkDownload() -> Download type[%d]\n", type);
			break;


		default:
			HxLOG_Error("Download type is not supported! [%d]\n", type);//not phase 1
			TRLEAVE(CWMP_STB_RES_FAIL);
			break;
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

CWMP_STB_RESULT Cwmp_Stb_notifyEvent(CWMP_STB_EVENT event_code,	char *additional_info)
{
	//HERROR 	hErr = ERR_OK;
	TRENTER;


	switch(event_code)
	{
		case CWMP_STB_EVT_REBOOT:
			HxLOG_Debug("[SHSEO] <-- Rcv. CWMP_STB_EVT_REBOOT\r\n");
			HxLOG_Debug("[SHSEO] do Cwmp_Cpe_close()\r\n");

			Cwmp_Cpe_close();

			HxLOG_Debug("[SHSEO] do TRP_EVENT_Reboot()\r\n");

			TRP_EVENT_Reboot();

			HxLOG_Debug("[SHSEO] end Cwmp_Stb_notifyEvent()\r\n");
			break;

		case CWMP_STB_EVT_FACTORYRESET:
			HxLOG_Error(" STB_API: %s was called, but not implemented! %d, %s\n",__FUNCTION__, event_code, additional_info);
			break;

		default:
			HxLOG_Error(" STB_API: %s was called, but not implemented! %d, %s\n",__FUNCTION__, event_code, additional_info);
			break;
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

int total =0;
//정상적으로 open 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_open(void)
{
	total = 0;
	if(TRP_FLASH_Open() != ERR_OK)
	{
		HxLOG_Error("can't flash open");
		TRLEAVE(CWMP_STB_RES_FAIL);
	}
	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

//정상적으로 close 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_close(void)
{
	if(TRP_FLASH_Close() != ERR_OK)
	{
		TRLEAVE(CWMP_STB_RES_FAIL);
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}

//정상적으로 close 했다면 0을, 실패했다면 -1을 반환
CWMP_STB_RESULT CWMP_STB_flash_write(const void *buf, size_t nbyte)
{
	int nRet;

	total += nbyte;
	nRet = TRP_FLASH_Write(buf, nbyte);
	HxLOG_Debug("total = %ld nRet = %d nbyte = %d\n", total, nRet, nbyte);
	if(nRet != nbyte)
	{
		TRLEAVE(CWMP_STB_RES_FAIL);
	}

	TRLEAVE(CWMP_STB_RES_SUCCESS);
}








