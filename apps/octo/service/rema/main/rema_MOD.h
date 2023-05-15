/*******************************************************************
File Description
********************************************************************/
/**
@file	  rema_MOD.h
@brief

Description:  									\n
\n

Copyright (c) 2013 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
*
*/
/*
* (c) 2011-2013 Humax Co., Ltd.
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
* CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.

* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/



#ifndef	__REMA_MODULE_H__
#define	__REMA_MODULE_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <rema_int.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************/
    /********************      Global Variables     ********************/
    /*******************************************************************/

    /*******************************************************************/
    /********************      Macro Definition     ********************/
    /*******************************************************************/
#if 1
#define rema_MOD_QInit(stTemp) rema_MOD_CheckInit(&stTemp.stMOD)

#define rema_MOD_QUpdate(stTemp) rema_MOD_CheckUpdate(&stTemp.stMOD)

#define rema_MOD_QDeinit(stTemp) rema_MOD_CallDeinit(&stTemp.stMOD)


#define rema_MOD_QResetUpdate(stTemp) { \
    if(stTemp.stMOD.bIsInit == TRUE) { \
    stTemp.stMOD.bIsUpdate = FALSE; \
    } \
};

    /*******************************************************************/
    /********************      Typedef         *************************/
    /*******************************************************************/
    typedef struct Base_MOD_t
    {
		HBOOL bIsInit;
		HBOOL bIsUpdate;
		HINT32 (*pcbMod_Init)(void);
		HINT32 (*pcbMod_Deinit)(void);
		HINT32 (*pcbMod_Update)(void);
    }Base_MOD_t;
    /*******************************************************************/
    /********************      Public Functions     ********************/
    /*******************************************************************/
    HBOOL rema_module_Init(void);
    void rema_module_Deinit(void);

    /*
    CWMP_STB_RESULT rema_MOD_cb_encrypt(CwmpcCpe *cpe, const uint8_t *src, size_t srcSize,
	uint8_t *dest, size_t *destSize);
    CWMP_STB_RESULT rema_MOD_cb_decrypt(CwmpcCpe *cpe, const uint8_t *src, size_t srcSize,
	uint8_t *dest, size_t *destSize);
    CWMP_STB_RESULT rema_MOD_cb_initDataModel(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager);
    void rema_MOD_cb_beginAccessDM(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager);
    void rema_MOD_cb_endAccessDM(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager);
    void rema_MOD_cb_beginSetParams(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager);
    CWMP_STB_RESULT rema_MOD_cb_endSetParams(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager);
    CWMP_STB_RESULT rema_MOD_cb_addObject(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager,
	const char *name, int pd, uint32_t *num);
    CWMP_STB_RESULT rema_MOD_cb_deleteObject(hCwmpCpeDM_t *pstCpe, hCwmpDMManager_t *pstDMManager,	const char *name, int pd);
    void rema_MOD_cb_reboot(CwmpcCpe *cpe);
    void rema_MOD_cb_factoryReset(CwmpcCpe *cpe);
    CWMP_STB_RESULT rema_MOD_cb_primeDownload(CwmpcCpe *cpe, const char *type,
	const char *url, const char *target, uint32_t fileSize, CwmpTransferDescriptor_t *dld);
    hCwmpResult_e rema_MOD_cb_applyDownload(CwmpcCpe *cpe,
	const hCwmpTransferDescriptor_t *dld);
    hCwmpResult_e rema_MOD_cb_checkDownload(CwmpcCpe *cpe,
	const hCwmpTransferDescriptor_t *dld);

    hCwmpResult_e rema_MOD_cb_failedDownload(hCwmpCpe_t *cpe,
	const hCwmpTransferDescriptor_t *dld, 
	const char *pszFaultCode);
    hCwmpTransResult_e rema_MOD_cb_primeUpload(hCwmpCpe_t *cpe, const char *type,
	const char *url, char **ppTargeturl,
	hCwmpTransferDescriptor_t *dld);

    hCwmpResult_e rema_MOD_cb_finishedUpload(hCwmpCpe_t *cpe,
	const hCwmpTransferDescriptor_t *dld, 
	const char *type, 
	const char *pszFaultCode);
    hCwmpResult_e rema_MOD_cb_applyAutoxfer(hCwmpCpe_t *cpe,
	const hCwmpAutonTransferDescriptor_t *dld);

    void * rema_MOD_cb_Ex_Trans_Open(int nTransfer_Method,const char *pFiletype, int nMode);
    int  rema_MOD_cb_Ex_Trans_Write(void *pvHandle, const void *pvBuf, int nBufLen);
    int  rema_MOD_cb_Ex_Trans_Read(void *pvHandle, void *pvBuf, int nBufLen,  int *pnReadLen);
    void rema_MOD_cb_Ex_Trans_Close(void *pvHandle);*/
    /*******************************************************************/
    /********************      extern Functions     ********************/
    /*******************************************************************/
    static inline HBOOL rema_MOD_CheckInit(Base_MOD_t *psMod)
    {
	if(!psMod)
	    return FALSE; 

	if(psMod->bIsInit == FALSE)
	{  
	    if(psMod->pcbMod_Init && psMod->pcbMod_Init() == 0 ) 
	    {
		psMod->bIsInit = TRUE; 
		psMod->bIsUpdate = FALSE; 
	    }
	} 
	return psMod->bIsInit; 
    }; 

    static inline HBOOL rema_MOD_CheckUpdate(Base_MOD_t *psMod)
    {
	if(!psMod)
	    return FALSE; 

	if(psMod->bIsInit == FALSE)
	{
	    rema_MOD_CheckInit(psMod);
	}
	if(psMod->bIsInit == TRUE )
	{
	    if(psMod->bIsUpdate == FALSE)
	    {
		if(psMod->pcbMod_Update && psMod->pcbMod_Update() == 0 ) 
		    psMod->bIsUpdate = TRUE; 
	    }
	} 
	return psMod->bIsUpdate; 
    }; 
    static inline HBOOL rema_MOD_CallDeinit(Base_MOD_t *psMod)
    {
	if(psMod->bIsInit == TRUE)
	{
	    if(psMod->pcbMod_Deinit && psMod->pcbMod_Deinit() == 0 ) 
	    {
		psMod->bIsUpdate = FALSE; 
		psMod->bIsUpdate = FALSE;
	    }
	}
	return psMod->bIsInit;
    };
#endif
#ifdef __cplusplus
}
#endif

#endif
