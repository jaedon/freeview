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
 * $brcm_Workfile: $
 * $brcm_Revision: $
 * $brcm_Date: $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: $
 *
 ***************************************************************************/
/**
 * @brief NetApp Private HTTP Methods
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * A quick and dirty implementation of an http post to perform a google voice
 * search (or any other type of post method) without having to use wget or use
 * libcurl
 */

#include "netapp_priv.h"
#include "netapp_http.h"

#define __STRICT_ANSI__
#include "json.h"

#ifndef MAXLINE
#define MAXLINE 4096
#endif

#define NETAPP_FLAC_CMD     "/usr/local/bin/flac"
#define NETAPP_FLAC_OUTPUT  "/tmp/out.flac"
#define GOOGLE_SEARCH_STR   "http://www.google.com/speech-api/v1/recognize?lang="
#define GOOGLE_HEADER       "Content-Type: audio/x-flac; rate=16000"
#define ISO639_LEN          2
#define SEARCH_TIMEOUT      3000

typedef struct
{
    NETAPP_BT_HID_VOICE_INFO tHidVoiceInfo;
    char                     cLanguage[ISO639_LEN +1];
} NETAPP_VOICE_SEARCH_INFO;


static void NetAppHttp_P_VoiceRecognitionSearch(
    void                    *pParam);


static NETAPP_RETCODE NetAppHttpPrivate_P_PostFile(
    NETAPP_VOICE_SEARCH_INFO *pSearchInfo,
    char                     **pResult);


NETAPP_RETCODE NetAppHttpPrivateVoiceSearch(
    NETAPP_BT_HID_VOICE_INFO *pHidVoiceInfo,
    const char               *pLanguage)
{
    NETAPP_RETCODE          tRetCode = NETAPP_SUCCESS;
    uint32_t                ulThread = 0;
    NETAPP_VOICE_SEARCH_INFO *pSearchInfo = NULL;

    NETAPP_PRIV_HANDLE_CHECK(pHidVoiceInfo);

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));
    /* Allocated memory will be freed in the spawned thread */
    pSearchInfo = (NETAPP_VOICE_SEARCH_INFO*)NetAppOSMemAlloc(sizeof(NETAPP_VOICE_SEARCH_INFO));
    if (pSearchInfo == NULL)
    {
        NETAPP_ERR(("%s(): Ran out of Memory!", __FUNCTION__));
        return NETAPP_OUT_OF_MEMORY;
    }
    memset(pSearchInfo, 0, sizeof(NETAPP_VOICE_SEARCH_INFO));

    pSearchInfo->tHidVoiceInfo = *pHidVoiceInfo;
    strncpy(pSearchInfo->cLanguage, pLanguage, sizeof(pSearchInfo->cLanguage)/sizeof(pSearchInfo->cLanguage[0]) -1);

    ulThread = NetAppOSTaskSpawn("NetAppHttp_P_VoiceRecognitionSearch", NETAPP_OS_PRIORITY_LOW,
        256*1024, NetAppHttp_P_VoiceRecognitionSearch, pSearchInfo);

    if (ulThread <=0)
    {
        NetAppOSMemFree(pSearchInfo);
        tRetCode = NETAPP_FAILURE;
    }

err_out:
    return tRetCode;
}


/*******************************************************************************
 *                       Static NetApp Http Functions
 ******************************************************************************/
static NETAPP_RETCODE NetAppHttpPrivate_P_PostFile(
    NETAPP_VOICE_SEARCH_INFO *pSearchInfo,
    char                     **pResult)
{
    NETAPP_RETCODE      tRetCode = NETAPP_FAILURE;
    int                 sockfd = 0;
    struct sockaddr_in  servaddr;
    char                **pptr;
    struct hostent      *hptr;
    char                cBuf[MAXLINE + 1];
    int                 written = 0;
    char                str[50];
    ssize_t             n;
    FILE                *pFile = NULL;
    uint32_t            ulFileSize = 0;
    char                cHostName[124];
    uint32_t            ulPort;
    char                *pStart = NULL;
    struct timeval      tm;
    fd_set              tFDSet;
    char                *pTmp = NULL;
    int                 contentLength = 0;

    // JSON string conversion
    json_object         *pJsonObject            = NULL;
    json_object         *pJsonHypothesesArray   = NULL;
    json_object         *pJsonHypotheses        = NULL;
    json_object         *pJsonUtterance         = NULL;

    NETAPP_PRIV_HANDLE_CHECK(pSearchInfo);
    NETAPP_PRIV_HANDLE_CHECK(pResult);

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));

    sscanf(GOOGLE_SEARCH_STR, "http://%99[^/][^\n]", cHostName);

    hptr = gethostbyname(cHostName);
    if ( (hptr == NULL) || (hptr->h_addrtype != AF_INET) || ((pptr = hptr->h_addr_list) == NULL) )
    {
        NETAPP_ERR(("%s(): Cannot get hotname info for %s!", __FUNCTION__, cHostName));
        goto err_out;
    }

    inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        NETAPP_ERR(("%s(): Cannot open socket, error: %s!", __FUNCTION__, strerror(errno)));
        goto err_out;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(80);
    inet_pton(AF_INET, str, &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
    {
        NETAPP_ERR(("%s(): Cannot connect to %s!", __FUNCTION__, str));
        goto err_out;
    }

    if ((pFile = fopen(NETAPP_FLAC_OUTPUT, "r")) == NULL)
    {
        NETAPP_ERR(("%s(): Cannot open the file! hotname info!", __FUNCTION__));
        goto err_out;

    }

    fseek(pFile, 0L, SEEK_END);
    ulFileSize = ftell(pFile);
    rewind(pFile);

    snprintf(cBuf, sizeof(cBuf),
        "POST %s%s HTTP/1.0\r\n"
        "Host: %s\r\n"
        "Content-Type: audio/x-flac; rate=%d\r\n"
        "Content-length: %d\r\n\r\n",
        GOOGLE_SEARCH_STR, pSearchInfo->cLanguage, cHostName, pSearchInfo->tHidVoiceInfo.sampleRate, ulFileSize);

    write(sockfd, cBuf, strlen(cBuf));

    while (!feof(pFile) && written < ulFileSize)
    {
        int towrite;
        n = fread(cBuf, 1, sizeof(cBuf), pFile);
        if (n == 0)
            break;
        towrite = MIN (ulFileSize - written, n);

        if (write(sockfd, cBuf, towrite) < 0)
        {
            NETAPP_ERR(("%s(): Error Writing file %s!", __FUNCTION__, NETAPP_FLAC_OUTPUT));
            goto err_out;
        }
      written += towrite;
    }
    NETAPP_MSG(("%s(): Wrote: %d bytes from %s, waiting for an answer...", __FUNCTION__, written, NETAPP_FLAC_OUTPUT));


    /* Wait for a response but not forever */
    tm.tv_sec  = 0;
    tm.tv_usec = SEARCH_TIMEOUT * 1000;

    FD_ZERO(&tFDSet);
    FD_SET(sockfd, &tFDSet);

    if (select(sockfd + 1, &tFDSet, NULL, NULL, &tm) <= 0)
    {
        NETAPP_ERR(("%s(): Timed out waiting for a response from %s", __FUNCTION__, cHostName));
        goto err_out;
    }

    n = read(sockfd, cBuf, MAXLINE);
    if (n <= 0)
    {
        NETAPP_ERR(("%s(): Got back crap!", __FUNCTION__));
        goto err_out;
    }
    cBuf[n] = '\0';


    NETAPP_MSG(("%s(): Received: [%s]", __FUNCTION__, cBuf));

    /* Check to see if there was an http error first */
    if (strstr(cBuf, "HTTP/1.0 400") != NULL)
    {
        NETAPP_ERR(("%s(): Lookup Failed!", __FUNCTION__));
        goto err_out;
    }

    /* Parse the content length */
    if ((pTmp = strstr(cBuf, "Content-Length: ")) != NULL)
    {
        pTmp += strlen("Content-Length: ");
        contentLength = atoi(pTmp);
        NETAPP_MSG(("%s(): Content length=%d", __FUNCTION__, contentLength));
    }

    /* Need to skip past the http header returned, we're not going to read it */
    if ((pStart = strstr(cBuf, "\r\n\r\n")) == NULL)
    {
        NETAPP_ERR(("%s(): Mal formated HTTP response!", __FUNCTION__));
        goto err_out;
    }
    pStart += strlen("\r\n\r\n");

    if ((pStart - cBuf) >= strlen(cBuf))
    {
        NETAPP_ERR(("%s(): Empty response, wait for more", __FUNCTION__));

        tm.tv_sec  = 0;
        tm.tv_usec = SEARCH_TIMEOUT * 1000;

        FD_ZERO(&tFDSet);
        FD_SET(sockfd, &tFDSet);

        if (select(sockfd + 1, &tFDSet, NULL, NULL, &tm) <= 0)
        {
            NETAPP_ERR(("%s(): Timed out waiting for a response from %s", __FUNCTION__, cHostName));
            goto err_out;
        }

        n = read(sockfd, cBuf, MAXLINE);
        if (n < contentLength)
        {
            NETAPP_ERR(("%s(): Got back crap, only %d!", __FUNCTION__, n));
            goto err_out;
        }
        pStart = cBuf;
    }

    NETAPP_MSG(("%s(): JSON is: [%s]!", __FUNCTION__, pStart));

    /* Now lets parse the JSON to get our result! */
    if ((pJsonObject = json_tokener_parse(pStart)) == NULL)
    {
        NETAPP_ERR(("%s(): Cannot parse JSON result!", __FUNCTION__));
        goto err_out;
    }

    if ( ((pJsonHypothesesArray = json_object_object_get(pJsonObject, "hypotheses")) == NULL) ||
         (json_object_get_type(pJsonHypothesesArray) != json_type_array) )
    {
        NETAPP_ERR(("%s(): Cannot parse JSON for the Hypotheses or the Hypotheses is not an array!", __FUNCTION__));
        goto err_out;
    }

    /* Now we have an array of Hypothesis elements, we will take the first one */
    if ((pJsonHypotheses = json_object_array_get_idx(pJsonHypothesesArray, 0)) == NULL)
    {
        NETAPP_ERR(("%s(): Cannot get first Hypotheses in the array!", __FUNCTION__));
        goto err_out;
    }

    if ((pJsonUtterance = json_object_object_get(pJsonHypotheses, "utterance")) != NULL)
    {
        *pResult = strdup(json_object_get_string(pJsonUtterance));
    }

    NETAPP_MSG(("%s(): Leaving!", __FUNCTION__));
    tRetCode = NETAPP_SUCCESS;

err_out:
    if (pJsonUtterance != NULL)
    {
        json_object_put(pJsonUtterance);
    }

    if (pJsonHypotheses != NULL)
    {
        json_object_put(pJsonHypotheses);
    }

    if (pJsonHypothesesArray != NULL)
    {
        json_object_put(pJsonHypothesesArray);
    }

    if (pJsonObject != NULL)
    {
        json_object_put(pJsonObject);
    }

    if (pFile != NULL)
    {
        fclose(pFile);
    }
    if (sockfd > 0)
    {
        close(sockfd);
    }
    return tRetCode;
}


static void NetAppHttp_P_VoiceRecognitionSearch(
    void                    *pParam)
{
    NETAPP_RETCODE          tRetCode = NETAPP_FAILURE;
    NETAPP_VOICE_SEARCH_INFO *pSearchInfo = (NETAPP_VOICE_SEARCH_INFO*)pParam;
    char                    *pResult = NULL;
    char                    cBuf[512];

    if (pSearchInfo == NULL)
    {
        NETAPP_ERR(("%s(): Invalid Parameter!", __FUNCTION__));
        goto err_out;
    }

    NETAPP_MSG(("%s(): Entered!", __FUNCTION__));
    snprintf(cBuf, sizeof(cBuf),
        "-f --bps %d --channels %d --endian little --sign signed --sample-rate %d %s -o %s",
        pSearchInfo->tHidVoiceInfo.bitsPerSample, pSearchInfo->tHidVoiceInfo.nbChannels, pSearchInfo->tHidVoiceInfo.sampleRate,
        pSearchInfo->tHidVoiceInfo.hidAudioFilename, NETAPP_FLAC_OUTPUT);

    NETAPP_MSG(("%s(): Converting PCM to flac of %s", __FUNCTION__, pSearchInfo->tHidVoiceInfo.hidAudioFilename));
    // System call to convert PCM to FLAC
    NETAPP_PRIV_API_CHECK(NetAppPrivateSystem(NETAPP_FLAC_CMD, cBuf, NULL,
        NETAPP_WAIT_FOREVER, NULL, NULL, 0, NULL), tRetCode);

    // http put to convert
    NETAPP_PRIV_API_CHECK(NetAppHttpPrivate_P_PostFile(pSearchInfo, &pResult), tRetCode);

    // JSON string conversion

    tRetCode = NETAPP_SUCCESS;

err_out:

    if ( (tRetCode != NETAPP_SUCCESS) && (pResult == NULL) )
    {
        pResult = strdup("");
    }

    if (pSearchInfo != NULL)
    {
        NetAppOSMemFree(pSearchInfo);
    }

    NETAPP_DO_CALLBACK(NETAPP_CB_VOICE_REC_DONE, pResult, strlen(pResult)+1, 0, tRetCode, NETAPP_IFACE_MAX);

    if (pResult != NULL)
    {
        free(pResult);
    }
    NetAppOSTaskExit();
}
