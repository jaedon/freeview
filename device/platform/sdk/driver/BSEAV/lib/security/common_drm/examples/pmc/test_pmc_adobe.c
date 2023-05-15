/***************************************************************************
 *    (c)2010-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nexus_platform.h"
#include "nexus_memory.h"
#include "pmc_adobe.h"
#include "drm_metadata.h"
#include "drm_common.h"

#include "bstd.h"
#include "bdbg.h" 
#include "bkni.h"


int main(int argc, char* argv[])
{
	DrmRC rc = Drm_Success;
	uint32_t i = 0;
	uint8_t *pDmaBuf = NULL;
	uint32_t dataSize = 0;
	NEXUS_PlatformSettings platformSettings;

	/* init Nexus */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);

	/* initialize PMC */
    rc = DRM_Pmc_AdobeInit(argv[1] /*PMC bin*/);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error calling DRM_Pmc_AdobeInit...\n");
    	return 0;
    }

    /********************************************************************
     * ROOT DIGEST
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eRootDigest, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eRootDigest, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - RootDigest :\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

    /********************************************************************
     * TRANSPORT CERTIFICATE
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eTransportCert, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eTransportCert, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - TransportCert:\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);


    /********************************************************************
     * DEVIC CREDENTIAL
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eDeviceCred, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eDeviceCred, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - DeviceCred:\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}

	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);


    /********************************************************************
     * 	DEVICE CREDENTIAL PASSWORD
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eDeviceCredPwd, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eDeviceCredPwd, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - DeviceCredPwd:\n\t");
	for(i = 0; i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);


    /********************************************************************
     * SHARED DOMAIN 0
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom0, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom0, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom0:\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}

	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

    /********************************************************************
     * SHARED DOMAIN PASSWORD 0
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom0Pwd, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom0Pwd, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom0Pwd :\n\t");
	for(i = 0; i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);


    /********************************************************************
     * SHARED DOMAIN 1
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom1, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom1, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom1:\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}

	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

    /********************************************************************
     * SHARED DOMAIN PASSWORD 1
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom1Pwd, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom1Pwd, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom1Pwd :\n\t");
	for(i = 0; i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

    /********************************************************************
     * SHARED DOMAIN 2
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom2, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom2, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom2:\n\t");
	for(i = 0; i < 16; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("......\n\t.......");
	for(i = (dataSize-16); i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}

	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

    /********************************************************************
     * SHARED DOMAIN PASSWORD 2
    ********************************************************************/
    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom2Pwd, NULL, &dataSize);

    /* Allocate a continuous physical address buffer for DMA */
    NEXUS_Memory_Allocate(dataSize, NULL,(void **) &pDmaBuf);

    rc = DRM_Pmc_AdobeGetData(PmcAdobeDataType_eSharedDom2Pwd, pDmaBuf, &dataSize);
    if(rc != Drm_Success)
    {
    	printf("\n\t###   MAIN - Error fetching data...\n");
    	return 0;
    }

	printf("\n\tMAIN - SharedDom2Pwd :\n\t");
	for(i = 0; i < dataSize; i++)
	{
		printf("0x%02x ", pDmaBuf[i]);
	}
	printf("\n\n");

	NEXUS_Memory_Free(pDmaBuf);

	printf("\n\tMAIN - Done\n");
    return 0;
}


