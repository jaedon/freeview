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
 * $brcm_Workfile: netapp_db_sqlite.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/3/12 4:18p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /NetApp/netapp/src/netapp_db_sqlite.c $
 * 
 * 2   2/3/12 4:18p steven
 * SW7231-638: NetApp Version 6.2
 ***************************************************************************/
/**
 * @brief NetApp Database API
 *
 * The Database API is used to store and retrieve settings using the SQLite
 * backend that comes from the libs or external. Built-in settings storage and
 * retrieval is optional feature of NetApp.
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Private Internal API's called by netapp
 *
 * Feel free to
 *
 * @remarks No private API will fetch the NetApp Mutex.
 *
 */
#include "netapp_db.h"
#include "sqlite3.h"


#define NETAPP_DB_DEFAULT_PATH                  "/tmp/"
#define NETAPP_DB_NAME                          "NetApp.db"
#define NETAPP_DB_ENTITY                        "NETAPP_DB_ENTITY"


#define NETAPP_DB_SQLITE_CHECK(api, tRetCode)                                   \
{                                                                               \
    char * err = NULL;                                                          \
    if (api != SQLITE_OK)                                                       \
    {                                                                           \
        if (err != NULL)                                                        \
        {                                                                       \
            NETAPP_ERR(("%s(): sqlite3_exec failed: %s\n", __FUNCTION__, err)); \
            sqlite3_free(err);                                                  \
        }                                                                       \
        tRetCode = NETAPP_FAILURE;                                              \
        goto err_out;                                                           \
    }                                                                           \
}


struct sNETAPP_DB
{
    sqlite3 *sqlDB;
};

static NETAPP_RETCODE NetAppDBPrivate_P_CreateTables(
    NETAPP_DB_HANDLE        hNetAppDB);


static NETAPP_RETCODE NetAppDBPrivate_P_DeleteTables(
    NETAPP_DB_HANDLE        hNetAppDB);


/*******************************************************************************
 *                 Public (but Internal to NetApp) Database API's
 ******************************************************************************/
NETAPP_RETCODE NetAppDBPrivateInit(
    NETAPP_DB_HANDLE        *phNetAppDB,
    const char              *pDBPath)
{
    NETAPP_DB_HANDLE        hNetAppDB = NULL;
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    unsigned int            i;
    struct stat             statbuf;
    char                    tmp[128];

    NETAPP_PRIV_HANDLE_CHECK(phNetAppDB);

    hNetAppDB = (NETAPP_DB_HANDLE)NetAppOSMemAlloc(sizeof(struct sNETAPP_DB));
    if (hNetAppDB == NULL)
    {
        NETAPP_ERR(("%s() -- MemAlloc error!", __FUNCTION__));
        tRetCode = NETAPP_OUT_OF_MEMORY;
        goto err_out;
    }

    memset(hNetAppDB, 0, sizeof(struct sNETAPP_DB));

    snprintf(tmp, sizeof(tmp), "%s/%s", (pDBPath != NULL) ? pDBPath : NETAPP_DB_DEFAULT_PATH, NETAPP_DB_NAME);

    NETAPP_DB_SQLITE_CHECK(sqlite3_open(tmp, &hNetAppDB->sqlDB), tRetCode);

    NETAPP_PRIV_API_CHECK(NetAppDBPrivate_P_CreateTables(hNetAppDB), tRetCode);

    *phNetAppDB = hNetAppDB;

err_out:
    return tRetCode;

}

NETAPP_RETCODE NetAppDBPrivateDeinit(
    NETAPP_DB_HANDLE        hNetAppDB)
{
    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);

    if (hNetAppDB->sqlDB != NULL)
    {
        sqlite3_close(hNetAppDB->sqlDB);
    }

    NetAppOSMemFree(hNetAppDB);
    return NETAPP_SUCCESS;
}


NETAPP_RETCODE NetAppDBPrivateGetValue(
    NETAPP_DB_HANDLE        hNetAppDB,
    char                    *pAttribute,
    char                    **pValue)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;
    sqlite3_stmt    *stmt;
    const char      *tail;

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);
    NETAPP_PRIV_HANDLE_CHECK(pAttribute);
    NETAPP_PRIV_HANDLE_CHECK(pValue);

    char * sqlRequest = sqlite3_mprintf("SELECT value FROM EAV WHERE entity = '%q' AND attribute = '%q';", NETAPP_DB_ENTITY, pAttribute);
    if (sqlRequest == NULL)
    {
        NETAPP_ERR(("sqlite3_mprintf failed\n"));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    NETAPP_DB_SQLITE_CHECK(sqlite3_prepare(hNetAppDB->sqlDB, sqlRequest, strlen(sqlRequest), &stmt, &tail), tRetCode);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *value = (const char *)sqlite3_column_text(stmt, 0);
        if (value == NULL)
        {
            NETAPP_ERR(("sqlite3_mprintf failed\n"));
            tRetCode = NETAPP_FAILURE;
            goto err_out;
        }
        else if (strcmp(value, "") != 0)
        {
            *pValue = strdup(value);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_free(sqlRequest);

err_out:
    return tRetCode;
}


NETAPP_RETCODE NetAppDBPrivateSetValue(
    NETAPP_DB_HANDLE        hNetAppDB,
    char                    *pAttribute,
    const char              *pValue)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);
    NETAPP_PRIV_HANDLE_CHECK(pAttribute);
    NETAPP_PRIV_HANDLE_CHECK(pValue);

    char * sqlRequest = sqlite3_mprintf("INSERT OR REPLACE INTO EAV(entity, attribute, value) VALUES('%q', '%q', '%q');",
        NETAPP_DB_ENTITY, pAttribute, pValue);

    if (sqlRequest == NULL)
    {
        NETAPP_ERR(("sqlite3_mprintf failed\n"));
        tRetCode = NETAPP_FAILURE;
        goto err_out;
    }

    NETAPP_DB_SQLITE_CHECK(sqlite3_exec(hNetAppDB->sqlDB, sqlRequest, NULL, NULL, &err), tRetCode);

err_out:
    if (sqlRequest != NULL)
    {
        sqlite3_free(sqlRequest);
    }
    return tRetCode;
}


NETAPP_RETCODE NetAppDBPrivateReset(
    NETAPP_DB_HANDLE        hNetAppDB)
{
    NETAPP_RETCODE  tRetCode = NETAPP_SUCCESS;

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);

    NETAPP_PRIV_API_CHECK(NetAppDBPrivate_P_DeleteTables(hNetAppDB), tRetCode);
    NETAPP_PRIV_API_CHECK(NetAppDBPrivate_P_CreateTables(hNetAppDB), tRetCode);

err_out:
    return tRetCode;
}



/*******************************************************************************
 *                 Private (Internal to NetApp DB) API's
 ******************************************************************************/
static NETAPP_RETCODE NetAppDBPrivate_P_CreateTables(
    NETAPP_DB_HANDLE        hNetAppDB)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    const char              *sqlRequest = "CREATE TABLE IF NOT EXISTS EAV(key integer primary key, entity text, attribute text, value text);";

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);

    NETAPP_DB_SQLITE_CHECK(sqlite3_exec(hNetAppDB->sqlDB, sqlRequest, NULL, NULL, &err), tRetCode);

    sqlRequest = "CREATE UNIQUE INDEX IF NOT EXISTS EAV_idx ON EAV(entity, attribute);";
    NETAPP_DB_SQLITE_CHECK(sqlite3_exec(hNetAppDB->sqlDB, sqlRequest, NULL, NULL, &err), tRetCode);

err_out:
    return tRetCode;
}


static NETAPP_RETCODE NetAppDBPrivate_P_DeleteTables(
    NETAPP_DB_HANDLE        hNetAppDB)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    const char              *sqlRequest = "DROP TABLE EAV;";

    NETAPP_PRIV_HANDLE_CHECK(hNetAppDB);

    NETAPP_DB_SQLITE_CHECK(sqlite3_exec(hNetAppDB->sqlDB, sqlRequest, NULL, NULL, &err), tRetCode);

err_out:
    return tRetCode;
}
