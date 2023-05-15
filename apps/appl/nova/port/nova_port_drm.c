/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_port.h>
#include <nova_cas.h>

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
void NOVA_PORT_LicenseNotifier(NOVA_CAS_DrmLicenseNotifier_t *pDrmListener, HBOOL bRequestLicenseAgain)
{
	NovaspreadTDrmLicense License;
	
	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t\t NOVA DRM : License Notifier\n");
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t [%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_DRM_INFO);
	HxLOG_Info("*\n");
	HxLOG_Info("*\t bRequestLicenseAgain(%d)\n", bRequestLicenseAgain);
	HxLOG_Info("*\t pDrmListener(0x%08x)\n", pDrmListener);
	HxLOG_Info("*\n");

	/*
	If an old license was given to NovaspreadDrmSetParameters(), and the DRM system determines that the license is no longer valid, 
	NOVASPREAD_NULL shall be passed to the listener. In this case NovaspreadServer will call NovaspreadDrmSetParameters() again, 
	but without an old license, to enforce the creation of a new one
	**/
	// old license가 valid하지 않은경우 NOVASPREAD_NULL로 전달한다.
	if(bRequestLicenseAgain == FALSE)
	{
		if(pDrmListener->pContext)
		{
			HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
			return;
		}

		HxLOG_Info("*\t OldLicense is not valid - return NOVASPREAD_NULL to Nova-Server\n");
		pDrmListener->DrmLicenseChangeListener(pDrmListener->pContext, NOVASPREAD_NULL);
		return;
	}

	if(pDrmListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pDrmListener is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
		return;
	}

	if(pDrmListener->DrmLicenseChangeListener == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- DrmLicenseChangeListener is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
		return;
	}

	#if 0
	if(pDrmListener->pLicense== NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pLicense is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
		return;
	}
	#endif

	if(pDrmListener->pContext == NULL)
	{
		HxLOG_Error("[%s:%d] -%s- pContext is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
		return;
	}

	License.License = pDrmListener->pLicense;
	License.LicenseLength = pDrmListener->ulSize;

	HxLOG_Info("*\t License - ulTransportSessionId(%d), ulRecordHandle(0x%08x), pContext(0x%08x), DrmLicenseChangeListener(0x%08x)\n"
		, pDrmListener->ulTransportSessionId
		, pDrmListener->ulRecordHandle
		, pDrmListener->pContext
		, pDrmListener->DrmLicenseChangeListener);
	NOVA_CAS_PrintHexDump("Send license to Nova-Server", License.License, License.LicenseLength);
	HxLOG_Info("\n******************************************************************************\n");

#if 1 // 2015/11/26 mhkang: Humax encodes temporally the license as base64 until Tara fixes it.
	{
		NovaspreadTDrmLicense EncodedLicense;

		EncodedLicense.License			= HLIB_STD_MemCalloc(License.LicenseLength * 2 + 1); // enough size

		HLIB_MATH_EncodeBase64(EncodedLicense.License, (const void *)License.License, License.LicenseLength);
		EncodedLicense.LicenseLength = strlen(EncodedLicense.License);
		pDrmListener->DrmLicenseChangeListener(pDrmListener->pContext, &EncodedLicense);

		printf("base64 license[size:%d]: %s\n",EncodedLicense.LicenseLength,   EncodedLicense.License);
		HLIB_STD_MemFree(EncodedLicense.License);

	}
#else
	pDrmListener->DrmLicenseChangeListener(pDrmListener->pContext, &License);
#endif
}

/* AS-20502_Novaspread-S-reference-manual_v-0-80-PreRelease.pdf에 NovaspreadDrmStart()와 NovaspreadDrmStop()가 없어 막고 시작한다. */
#if 0
PUBLIC NovaspreadTBoolean NovaspreadDrmStart (NovaspreadTUInt32 aTransportSessionId, NovaspreadTDrmLicenseParameters* aLicenseParameter, NovaspreadTDrmLicense* aLicense)
{
	HxLOG_Trace()
	return NOVASPREAD_TRUE;
}


PUBLIC void NovaspreadDrmStop (NovaspreadTUInt32 aTransportSessionId)
{
	HxLOG_Trace()
}
#endif

/****************************************************************************
* FUNCTION: NovaspreadDrmSetParameters
*
* DESCRIPTION:
*	This function sets parameters which shall be used for the re-encryption of the transport stream.
*
*	The TransportSessionId, which is passed to this function, can be got by a call to NovaspreadTunerGetTransportSessionId().
*	When NovaspreadDrmSetParameters() was called, after some time the LicenseChangeListener must be called to provide the license to NovaspreadServer.
*	See NovaspreadTDrmLicenseParameters for details.
*
* PARAMETERS:
*	- aTransportSessionId
*		For this TransportSession the re-encryption is started.
*	- aLicenseParameters
*		The parameters which shall be used for re-encryption.
*
* RETURNS:
*	- NOVASPREAD_TRUE 
*		if successful
*	- NOVASPREAD_FALSE
*		otherwise
*
* SEE ALSO:
*	- NovaspreadTunerGetTransportSessionId()
*	- NovaspreadTDrmLicenseParameters
*	- NovaspreadTDrmLicense
****************************************************************************/

PUBLIC NovaspreadTBoolean NovaspreadDrmSetParameters (NovaspreadTUInt32 aTransportSessionId, NovaspreadTDrmLicenseParameters * aLicenseParameters)
{
	NovaspreadTBoolean bRet = NOVASPREAD_FALSE;

	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t\t NOVA DRM : Set Parameters\n");
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t [%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_DRM_INFO);
	HxLOG_Info("*\n");
	HxLOG_Info("*\t aTransportSessionId(%d)\n", aTransportSessionId);
	HxLOG_Info("*\t aLicenseParameters(0x%08x)\n", aLicenseParameters);
	NOVA_CAS_DRM_PrintLicenseParameters(aLicenseParameters);
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");

	bRet = NOVA_CAS_DRM_DrmSetParameters(aTransportSessionId, aLicenseParameters);
	if(bRet == NOVASPREAD_FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- bRet is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
	}

	return bRet;	
}

/****************************************************************************
* FUNCTION: NovaspreadDrmSetLicenseChangeListener
*
* DESCRIPTION:
*	This function sets a LicenseChangeListener at NovaspreadDrm.
*
* PARAMETERS:
*	- aTransportSessionId
*		For this TransportSession the listener is set.
*	- aListener
*		The listener to be set. Pass NOVASPREAD_NULL to unset the listener.
*	- aContext
*		This context shall be passed unchanged to the listener.
*
* RETURNS:
*	- NOVASPREAD_TRUE 
*		if successful
*	- NOVASPREAD_FALSE
*		otherwise
*
****************************************************************************/

PUBLIC NovaspreadTBoolean NovaspreadDrmSetLicenseChangeListener (NovaspreadTUInt32  aTransportSessionId, NovaspreadTDrmLicenseChangeListener aListener,  void * aContext)
{
	NovaspreadTBoolean bRet = NOVASPREAD_FALSE;

	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t\t NOVA DRM : Regist Listener\n");
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");
	HxLOG_Info("*\n");
	HxLOG_Info("*\t [%s:%d] -%s-\n", __FUNCTION__, __LINE__, NOVA_PORT_DRM_INFO);
	HxLOG_Info("*\n");
	HxLOG_Info("*\t aTransportSessionId(%d)\n", aTransportSessionId);
	HxLOG_Info("*\t aContext(0x%08x)\n", aContext);	
	HxLOG_Info("*\t aListener(0x%08x)\n", aListener);
	HxLOG_Info("*\n");
	HxLOG_Info("\n******************************************************************************\n");

	bRet = NOVA_CAS_DRM_DrmSetLicenseChangeListener(aTransportSessionId, aListener, aContext);
	if(bRet == NOVASPREAD_FALSE)
	{
		HxLOG_Error("[%s:%d] -%s- bRet is fail\n",__FUNCTION__, __LINE__, NOVA_PORT_DRM_ERR);
}

	return bRet;		
}

/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/
