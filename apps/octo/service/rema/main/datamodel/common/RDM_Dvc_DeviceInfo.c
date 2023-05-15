/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* �� 2011-2012 Humax Co., Ltd.
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
#include <oapi.h>
#include <hapi.h>
#include <rema_int.h>

#include <di_system.h>

#include "RDM_Dvc_DeviceInfo.h"

#include "RMOD_INT.h"





#define ___DEFINES___

#define ___MACROS___

#define ___TYPEDEF___

#define ___STATIC_VARIABLES___

#define ___INTERNAL_FUNCTIONS___
#define ___PUBLIC_FUNCTIONS___

#if 0 /* ������� �ʽ��ϴ�. */

//Device.Deviceinfo.Manufacturer�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_Manufacturer(HCWMP_GETTER_PARAM)
{
    if(ERR_OK == OAPI_SYSINFO_STB_GetManufacturerName(pszBuf, nBufSize))
    {
	return ehCwmp_R_OK;
    }
    return ehCwmp_R_INTERNAL_ERROR;
}

//Device.Deviceinfo.ManufacturerOUI�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_ManufacturerOUI(HCWMP_GETTER_PARAM)
{
    //humax OUI - 0x000378
    sprintf(pszBuf,"000378");
    return ehCwmp_R_OK;
}

//Device.Deviceinfo.ModelName�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_ModelName(HCWMP_GETTER_PARAM)
{

    if(ERR_OK == OAPI_SYSINFO_STB_GetModelName(pszBuf,  nBufSize))
    {
	return ehCwmp_R_OK;
    }
    return ehCwmp_R_INTERNAL_ERROR;
}

//Device.Deviceinfo.Description�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_Description(HCWMP_GETTER_PARAM)
{
    //OP�� ���� ���� �̿��ϸ� �ɵ� �մϴ�.
    snprintf(pszBuf,nBufSize, "Humax STB");
    return ehCwmp_R_OK;
}

//Device.Deviceinfo.SerialNumber�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_SerialNumber(HCWMP_GETTER_PARAM)
{
    if(ERR_OK == OAPI_SYSINFO_STB_GetSerialNo(pszBuf,  nBufSize))
    {
	return ehCwmp_R_OK;
    }
    return ehCwmp_R_INTERNAL_ERROR;
}

//Device.Deviceinfo.HardwareVersion�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_HardwareVersion(HCWMP_GETTER_PARAM)
{

    unsigned int unHwRev=0;
    // OP/ACS�� �䱸�ϴ� �� �� HW Revision GPIO ���� Ȯ���ؼ� ���� ó���ؾ��մϴ�.
    // HardwareVersion�� value�� string-64�Դϴ�.
    if( DI_ERR_OK == DI_SYSTEM_GetSystemInfo(DI_SYS_HW_REVISION, (void *)&unHwRev, sizeof(unsigned int)) )
    {
       switch(unHwRev)
       {
           case 0x00: snprintf(pszBuf,nBufSize, "0.0.0"); break;
           case 0x01: snprintf(pszBuf,nBufSize, "0.1.0"); break;
	    // TDB
           default :
               break;

       }
    }

    return ehCwmp_R_INTERNAL_ERROR;
}

//Device.Deviceinfo.SoftwareVersion�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_SoftwareVersion(HCWMP_GETTER_PARAM)
{

    if(ERR_OK == OAPI_SYSINFO_STB_GetSoftwareVersion(pszBuf,  nBufSize))
    {
	return ehCwmp_R_OK;
    }
    return ehCwmp_R_INTERNAL_ERROR;
}

//Device.Deviceinfo.DeviceStatus�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_DeviceStatus(HCWMP_GETTER_PARAM)
{
    //TR157A3  ������ �Һи���. �׸��� TR181I2A6���� ������.
    snprintf(pszBuf,nBufSize, "UP");
    return ehCwmp_R_OK;
}

//Device.Deviceinfo.UpTime�� getter
hCwmpResult_e get_RDM_Dvc_DvcInf_UpTime(HCWMP_GETTER_PARAM)
{
    HUINT32 ulValue =0;
     if(ERR_OK == RMOD_INT_GetUpTime(&ulValue))
    {
	*pnOutLen = hCwmpUtil_UINT32ToStr(ulValue, pszBuf, nBufSize);
	return ehCwmp_R_OK;
    }

    return ehCwmp_R_INTERNAL_ERROR;
}
#endif

/*
* ���⼭ ���� deviceinfo�� ���� datamodel�� ���� �մϴ�.
*/


#define ___EXTERNAL_FUNCTIONS__


#define _____END_OF_FILE_________

