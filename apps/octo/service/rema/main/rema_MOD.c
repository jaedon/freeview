/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define ___HEADER_FILES___

#include <vkernel.h>
#include <hlib.h>

#include "rema_MOD.h"
#include <hapi.h>
#include <papi.h>

//OTA
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <ondk.h>

#include "rema_DM.h"

//#include "RMOD_INT.h"
#include "RMOD_DI.h"
//#include "RMOD_SVC_Obama.h"
//#include "rema_module_DI.h"
//#include "rema_module_Internal.h"
//#include "rema_module_service.h"

#if 0
#define ___DEFINES___
#define DFILE_TYPE_STR_1_FIRMWARE		"1 Firmware Upgrade Image"
#define DFILE_TYPE_STR_2_WEB_CONTENT		"2 Web Content"
#define DFILE_TYPE_STR_3_VENDOR_CFG		"3 Vendor Configuration File"
#define DFILE_TYPE_STR_4_VENDOR_LOG		"4 Vendor Log File"
#define DFILE_TYPE_STR_4_TONE			"4 Tone File"
#define DFILE_TYPE_STR_5_RINGER			"5 Ringer File"
#define DFILE_TYPE_STR_6_VENDOR_CFG_X		"6 Vendor Configuration File"
#define DFILE_TYPE_STR_7_VENDOR_LOG_X		"7 Vendor Log File"
#define DFILE_TYPE_STR_1_VENDOR_CFG		"1 Vendor Configuration File"
#define DFILE_TYPE_STR_2_VENDOR_LOG		"2 Vendor Log File"
#define ___MACROS___

#define ___TYPEDEF___
#define NOT_READY_TO_WRITE 	(-1)
#define RAWFW_OFFSET	(0x86f0000)
#define ___STRUCT___
#define ___STATIC_VARIABLES___
int g_fd = NOT_READY_TO_WRITE;
static HERROR	__system_file_notifier(HINT32 nItemState, const HCHAR *pszName, void *pvUser)
{
    RemLOG_Print("[evt:%d, file:%s]\n", nItemState, pszName);
    return ERR_OK;
}


/*-----------------------------------------------------------------------------
*  HAMA Event
*-----------------------------------------------------------------------------*/
static HERROR	__system_bus_signal_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
    PxSTATUS_e			ePapiStatus;
    HxLOG_Assert(nArgc == 1);



    ePapiStatus = HxOBJECT_INT(apArgv[0]);
    switch(ePapiStatus)
    {
    case ePAMA_STATUS_Reboot:
	HxLOG_Print("[%s:%d] ePAMA_STATUS_Reboot \n", __FUNCTION__, __LINE__);

	break;

    case ePAMA_STATUS_Shutdown:
	HxLOG_Print("[%s:%d] ePAMA_STATUS_ShutdownDone\n", __FUNCTION__, __LINE__);

	break;
    case ePAMA_STATUS_ShutdownDone:
	HxLOG_Print("[%s:%d] ePAMA_STATUS_ShutdownDone\n", __FUNCTION__, __LINE__);

	break;

    case ePAMA_STATUS_Operation:
	HxLOG_Print("[%s:%d] ePAMA_STATUS_Operation\n", __FUNCTION__, __LINE__);
	break;

    default:
	break;
    }


    return ERR_OK;
}


#define ___INTERNAL_FUNCTIONS___
static HINT32 rema_module_resetUpdate(void)
{
    return 0; 
}
#define ___PUBLIC_FUNCTIONS___


HBOOL rema_module_Init(void)
{
    REM_FUNC_ENTER;


    RMOD_INT_Init();
    RMOD_DI_Init();
    RMOD_SVC_Obama_Init();
   

    rema_DM_Init();

    REM_FUNC_LEAVE;
    HAPI_Init();
    //HAPI_SetRegistryListener(NULL, __system_file_notifier);

    HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, __system_bus_signal_onSystemStateChanged);

    return TRUE;
}





hCwmpResult_e rema_MOD_cb_encrypt(hCwmpCpe_t *cpe, const uint8_t *src, size_t srcSize,
				  uint8_t *dest, size_t *destSize)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_decrypt(hCwmpCpe_t *cpe, const uint8_t *src, size_t srcSize,
				  uint8_t *dest, size_t *destSize)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}


hCwmpResult_e rema_MOD_cb_initDataModel(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager)
{
    // Add key objects.



    int dev = hCwmpDMManager_AddObject(pstDMManager, "Device", -1, false);
    int di = hCwmpDMManager_AddObject(pstDMManager, "DeviceInfo", dev, false);
    //  hCwmpDMManager_AddObject(dm, "DeviceInfo", dev, false);

    REM_FUNC_ENTER;

    hCwmpDMManager_AddAllInParams(pstDMManager);

    rema_DM_Reister((void *)pstDMManager);
    // Add all internal parameters.

    RemLOG_Info("To xxx\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

void rema_MOD_cb_beginAccessDM(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager)
{
    REM_FUNC_ENTER;

    rema_module_resetUpdate();

    REM_FUNC_LEAVE;

    return ; 
}

void rema_MOD_cb_endAccessDM(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ; 
}

void rema_MOD_cb_beginSetParams(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ; 
}

hCwmpResult_e rema_MOD_cb_endSetParams(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_addObject(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager,
				    const char *name, int pd, uint32_t *num)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be %s \n", name);

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_deleteObject(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager,
				       const char *name, int pd)
{

    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

void rema_MOD_cb_reboot(hCwmpCpe_t *cpe)
{
    REM_FUNC_ENTER;

    PAPI_RequestReboot();

    REM_FUNC_LEAVE;

    return ; 
}

void rema_MOD_cb_factoryReset(hCwmpCpe_t *cpe)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ; 
}

hCwmpTransResult_e rema_MOD_cb_primeDownload(hCwmpCpe_t *cpe, const char *type,
					     const char *url, const char *target,
					     uint32_t fileSize,
					     hCwmpTransferDescriptor_t *dld)
{
    REM_FUNC_ENTER;
    ////===============================================///
#if 0 //일본향 code 재활용 못함
    RemLOG_Info("To be\n");
    if(!strcmp(type, DFILE_TYPE_STR_1_FIRMWARE))
    {
	dld->nTransfer_Method = 1 ; 
	sprintf(dld->szTfId, "firmware");
	sprintf(dld->szTfPath, "firmware");
    }
    else
    {
	return ehCwmp_TR_NOT_SUPPORT; 
    }
    //dld->nTransfer_Method = HCWMP_TRANSFER_USE_DEFAULT
#endif
    /////////////////////////////////////////////////////
    REM_FUNC_LEAVE;

    return ehCwmp_TR_OK; 
}

hCwmpResult_e rema_MOD_cb_applyDownload(hCwmpCpe_t *cpe,
					const hCwmpTransferDescriptor_t *dld)
{
    REM_FUNC_ENTER;
    ////===============================================///
#if 0 //일본향 code 재활용 못함
    RemLOG_Info("To be\n");
    if(!strcmp(dld->szTfId, "firmware"))
    {
	return ehCwmp_R_PENDING;
    }
#endif
    ////===============================================///
    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_failedDownload(hCwmpCpe_t *cpe,
					 const hCwmpTransferDescriptor_t *dld, 
					 const char *pszFaultCode)
{

    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_checkDownload(hCwmpCpe_t *cpe,
					const hCwmpTransferDescriptor_t *dld)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpTransResult_e rema_MOD_cb_primeUpload(hCwmpCpe_t *cpe, const char *type,
					   const char *url, char **ppTargeturl,
					   hCwmpTransferDescriptor_t *dld)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_finishedUpload(hCwmpCpe_t *cpe,
					 const hCwmpTransferDescriptor_t *dld, 
					 const char *type, 
					 const char *pszFaultCode)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}

hCwmpResult_e rema_MOD_cb_applyAutoxfer(hCwmpCpe_t *cpe,
					const hCwmpAutonTransferDescriptor_t *dld)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return ehCwmp_R_OK; 
}
void * rema_MOD_cb_Ex_Trans_Open(int nTransfer_Method,const char *pFiletype, int nMode)
{
    REM_FUNC_ENTER;
    ////===============================================///
#if 0 //일본향 code 재활용 못함
    unsigned char *mntNode = "/dev/mtdblock6";
    if(g_fd != NOT_READY_TO_WRITE)
    {
	//HxLog_Error("already opened![%s:%d]\n", __FUNCTION__, __LINE__);
	//TRLEAVE(ERR_FAIL);
    }

    /* Open Device Node */
    if((g_fd = open(mntNode, O_SYNC | O_RDWR)) < 0)
    {
	//HxLog_Error("open error (%s)[%s:%d]\n", mntNode, __FUNCTION__, __LINE__);
	g_fd = NOT_READY_TO_WRITE;
	//TRLEAVE(ERR_FAIL);
    }

    /* set init position */
    if(RAWFW_OFFSET != lseek(g_fd, RAWFW_OFFSET, SEEK_SET))
    {
	//HxLog_Error("lseek error[%s:%d]\n", __FUNCTION__, __LINE__);
	//TRP_FLASH_Close();
	//TRLEAVE(ERR_FAIL);
    }
#endif
    ////===============================================///
    REM_FUNC_LEAVE;

    return 0; 
}
int  rema_MOD_cb_Ex_Trans_Write(void *pvHandle, const void *pvBuf, int nBufLen)
{
    REM_FUNC_ENTER;
    ////===============================================///
#if 0 //일본향 code 재활용 못함
    int nRet;
    if(g_fd == NOT_READY_TO_WRITE)
    {
	//HxLog_Error("not ready to write! open first![%s:%d]\n", __FUNCTION__, __LINE__);
	//TRLEAVE(0);	
    }

    nRet = write(g_fd, pvBuf, nBufLen);
    if(nRet <= 0)
    {
	//HxLog_Error("write MTD block error[%s:%d]\n", __FUNCTION__, __LINE__);
	//TRLEAVE(0);
    }
    //HxSTD_TaskSleep(10);
#endif
    ////===============================================///

    REM_FUNC_LEAVE;

    return 0; 
}
int  rema_MOD_cb_Ex_Trans_Read(void *pvHandle, void *pvBuf, int nBufLen,  int *pnReadLen)
{
    REM_FUNC_ENTER;

    RemLOG_Info("To be\n");

    REM_FUNC_LEAVE;

    return 0; 
}
void rema_MOD_cb_Ex_Trans_Close(void *pvHandle)
{
    REM_FUNC_ENTER;
    ////===============================================///
#if 0 //일본향 code 재활용 못함
    if(g_fd == NOT_READY_TO_WRITE)
    {
	//HxLog_Error("there is nothing to close![%s:%d]\n", __FUNCTION__, __LINE__);

    }

    /* flush buffer cache */
    //nRet = ioctl(fd, BLKFLSBUF, 0);
    if(ioctl(g_fd, 0x20001261, 0) < 0)
    {
	//HxLog_Error("Sync MTD block error[%s:%d]\n", __FUNCTION__, __LINE__);

    }

    close(g_fd);
    g_fd = NOT_READY_TO_WRITE;
#endif
    ////===============================================///

    REM_FUNC_LEAVE;

    return ; 
}
#endif



HBOOL rema_module_Init(void)
{
    REM_FUNC_ENTER;


    RMOD_INT_Init();
    RMOD_DI_Init();
    RMOD_SVC_Obama_Init();
   
    rema_DM_Init();

    HAPI_Init();
    //HAPI_SetRegistryListener(NULL, __system_file_notifier);

    //HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, __system_bus_signal_onSystemStateChanged);
    REM_FUNC_LEAVE;

    return TRUE;
}

void rema_module_Deinit(void)
{
    REM_FUNC_ENTER;


    rema_DM_Deinit();

    RMOD_INT_Deinit();
    RMOD_DI_Deinit();
    RMOD_SVC_Obama_Deinit();


    REM_FUNC_LEAVE;

    return;
}


#define ___EXTERNAL_FUNCTIONS___

#define _____END_OF_FILE_________

