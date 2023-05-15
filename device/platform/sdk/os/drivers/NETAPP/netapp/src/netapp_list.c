/***************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: netapp_list.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/14/11 10:41a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/netapp_list.c $
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief Simple LinkList Implementation for NetApp
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * All the internal workings of NetApp that is included by all NetApp modules.
 *
 * @remarks This API should not be included by any external application code
 * as the API's are only used internal to NetApp.
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_priv.h"
#include "netapp_list.h"

struct sNETAPP_LIST
{
    NETAPP_LIST_NODE    *pHead;
    uint32_t            ulSize;
    uint32_t            ulNodeSize;
    uint32_t            ulCount;
    NETAPP_LIST_NODE    *pCurrent;
};

NETAPP_RETCODE NetAppListCreate(
    NETAPP_LIST_HANDLE      *phNetAppList,
    uint32_t                ulNodeSize)
{
    NETAPP_RETCODE          tRetCode    = NETAPP_SUCCESS;
    NETAPP_LIST_HANDLE      hNetAppList = NULL;

    if (phNetAppList == NULL)
    {
        NETAPP_ERR(("%s(): -- Invalid parameter!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    if (ulNodeSize <= sizeof(NETAPP_LIST_NODE))
    {
        NETAPP_ERR(("%s(): -- ulNodeSide must be equal to the sizeof(<your node>)!", __FUNCTION__));
        return NETAPP_INVALID_PARAMETER;
    }

    hNetAppList = (NETAPP_LIST_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_LIST));
    if (hNetAppList == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppList, 0, sizeof(struct sNETAPP_LIST));
    hNetAppList->ulNodeSize = ulNodeSize;

    *phNetAppList = hNetAppList;
err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppListDestroy(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    NETAPP_LIST_NODE    *pNext      = NULL;
    NETAPP_LIST_NODE    *pCurrent   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppList);

    NetAppListEmpty(hNetAppList);
    NetAppOSMemFree(hNetAppList);
    hNetAppList = NULL;

    return NETAPP_SUCCESS;
}


NETAPP_LIST_NODE* NetAppListAppend(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    NETAPP_LIST_NODE    *pNewElem   = NULL;
    NETAPP_LIST_NODE    *pCurrent   = NULL;
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;

    if (hNetAppList == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NULL;
    }

    pNewElem = (NETAPP_LIST_NODE*)NetAppOSMemAlloc(hNetAppList->ulNodeSize);
    if (pNewElem == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        return NULL;
    }

    memset(pNewElem, 0, hNetAppList->ulNodeSize);

    pNewElem->pNext = NULL;

    /* Add the node to the list */
    if (hNetAppList->pHead == NULL)
    {
        hNetAppList->pHead = pNewElem;
    }
    else
    {
        pCurrent = hNetAppList->pHead;
        while (pCurrent->pNext != NULL)
        {
            pCurrent = pCurrent->pNext;
        }
        pCurrent->pNext = pNewElem;
    }

    hNetAppList->ulCount++;
    return pNewElem;
}


NETAPP_RETCODE NetAppListRemove(
    NETAPP_LIST_HANDLE      hNetAppList,
    NETAPP_LIST_NODE        *pNode)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_LIST_NODE    *pCurrent   = NULL;
    NETAPP_LIST_NODE    *pPrevious  = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppList);
    NETAPP_PRIV_HANDLE_CHECK(pNode);

    pCurrent = pPrevious = hNetAppList->pHead;
    while ( (pCurrent != NULL) && (pCurrent != pNode) )
    {
        pPrevious = pCurrent;
        pCurrent = pCurrent->pNext;
    }

    /* Found the right reference */
    if (pCurrent != NULL)
    {
        if (pPrevious != pCurrent)
        {
            if (pCurrent->pNext != NULL)
                pPrevious->pNext = pCurrent->pNext;
            else
                pPrevious->pNext = NULL;
        }
        else
        {
            hNetAppList->pHead = pCurrent->pNext;
        }
        NetAppOSMemFree(pCurrent);
    }
    else
    {
        return NETAPP_FAILURE; /* Item not found */
    }
    hNetAppList->ulCount--;
    return NETAPP_SUCCESS;
}


NETAPP_LIST_NODE* NetAppListFind(
    NETAPP_LIST_HANDLE      hNetAppList,
    const void              *pSearchCond,
    uint32_t                ulLength)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_LIST_NODE    *pCurrent   = NULL;


    if ( (hNetAppList == NULL) || (pSearchCond == NULL) )
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NULL;
    }

    pCurrent = hNetAppList->pHead;

    while ( (pCurrent != NULL) && (memcmp((pCurrent + sizeof(NETAPP_LIST_NODE)), pSearchCond, ulLength) != 0) )
    {
        pCurrent = pCurrent->pNext;
    }

    /* Found the right reference? */
    return (pCurrent != NULL) ? pCurrent : NULL;
}


uint32_t NetAppListCount(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    if (hNetAppList == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return 0;
    }
    return hNetAppList->ulCount;
}


NETAPP_RETCODE NetAppListRewind(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppList);
    hNetAppList->pCurrent = hNetAppList->pHead;
    return NETAPP_SUCCESS;
}


NETAPP_LIST_NODE* NetAppListNext(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    NETAPP_LIST_NODE *pNode = NULL;

    if (hNetAppList == NULL)
    {
        NETAPP_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return NULL;
    }

    pNode = hNetAppList->pCurrent;
    if (hNetAppList->pCurrent != NULL)
    {
        hNetAppList->pCurrent = hNetAppList->pCurrent->pNext;
    }
    return pNode;
}

NETAPP_RETCODE NetAppListIsValid(
    NETAPP_LIST_HANDLE      hNetAppList,
    NETAPP_LIST_NODE        *pNode)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_LIST_NODE    *pCurrent   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppList);
    NETAPP_PRIV_HANDLE_CHECK(pNode);

    pCurrent = hNetAppList->pHead;

    while ( (pCurrent != NULL) && (pNode != pCurrent) )
    {
        pCurrent = pCurrent->pNext;
    }

    /* Found the right reference? */
    return (pCurrent != NULL) ? NETAPP_SUCCESS : NETAPP_NOT_FOUND;
}

NETAPP_RETCODE NetAppListEmpty(
    NETAPP_LIST_HANDLE      hNetAppList)
{
    NETAPP_RETCODE      tRetCode    = NETAPP_SUCCESS;
    NETAPP_LIST_NODE    *pCurrent   = NULL;
    NETAPP_LIST_NODE    *pNext   = NULL;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppList);

    pCurrent = hNetAppList->pHead;

    pCurrent = pNext = hNetAppList->pHead;
     while (pCurrent != NULL)
     {
        pNext = pCurrent->pNext;
        NetAppOSMemFree(pCurrent);
        pCurrent = pNext;
     }
     hNetAppList->pCurrent = hNetAppList->pHead = NULL;
     hNetAppList->ulCount = 0;

     return NETAPP_SUCCESS;
}


