/***************************************************************************
 *     (c)2004-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_platform_audio_log.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/29/12 3:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/platforms/common/src/nexus_platform_audio_log.c $
 * 
 * 6   10/29/12 3:33p jgarrett
 * SW7425-4157: Adding audio_logs_enabled=y
 * 
 * 5   9/14/12 2:07p jgarrett
 * SW7422-400: Removing pthread_cancel for thread termination to ensure
 *  files are closed
 * 
 * 4   4/17/12 5:08p jtna
 * SW7231-577: Coverity Defect ID:36199 TAINTED_STRING
 * 
 * 3   2/10/12 11:33a vsilyaev
 * SW7425-2063: Disable audio log for Android builds
 * 
 * SW7425-2063/1   12/28/11 12:32p alexpan
 * SW7425-2063: Disable audio log for Android builds
 * 
 * 2   11/9/11 10:13a erickson
 * SW7422-400: fix compilation without NEXUS_PLATFORM_AUDIO_LOG
 * 
 * 1   11/8/11 6:06p jgarrett
 * SW7422-400: Adding audio debug log support in kernel mode
 * 
 ************************************************************/
#include "nexus_platform_module.h"
#include "nexus_platform_audio_log.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

BDBG_MODULE(nexus_platform_audio_log);

#ifdef B_REFSW_ANDROID
#undef NEXUS_PLATFORM_AUDIO_LOG
#endif 

#if NEXUS_PLATFORM_AUDIO_LOG
static bool g_audioLogThreadDone;
static pthread_t g_audioLogThread;

static void *NEXUS_Platform_P_AudioLogThread(void *pParam)
{
    unsigned dsp, type;
    const char *pEnv;
    static char pathname[PATH_MAX];
    const void *pBuffer;
    size_t bufferSize;
    NEXUS_Error errCode;

    struct
    {
        bool enabled;
        struct
        {
            FILE *pFile;
            unsigned index;
        } dsp[NEXUS_NUM_AUDIO_DSP];
    } debugData[NEXUS_AudioDspDebugType_eMax];

    BKNI_Memset(&debugData, 0, sizeof(debugData));
    BSTD_UNUSED(pParam);

    /* Initialize */
    for ( type = 0; type < NEXUS_AudioDspDebugType_eMax; type++ )
    {
        switch ( type )
        {
        case NEXUS_AudioDspDebugType_eDramMessage:
            pEnv = NEXUS_GetEnv("audio_debug_file");
            if ( NULL == pEnv && NEXUS_GetEnv("audio_logs_enabled") )
            {
                pEnv = "audio_debug";
            }
            break;
        case NEXUS_AudioDspDebugType_eUartMessage:
            pEnv = NEXUS_GetEnv("audio_uart_file");
            if ( NULL == pEnv && NEXUS_GetEnv("audio_logs_enabled") )
            {
                pEnv = "audio_uart";
            }
            break;
        case NEXUS_AudioDspDebugType_eCoreDump:
            pEnv = NEXUS_GetEnv("audio_core_file");
            if ( NULL == pEnv && NEXUS_GetEnv("audio_logs_enabled") )
            {
                pEnv = "audio_core";
            }
            break;
        default:
            pEnv = NULL;
            break;
        }
        if ( NULL != pEnv )
        {
            debugData[type].enabled = true;

            for ( dsp = 0; dsp < NEXUS_NUM_AUDIO_DSP; dsp++ )
            {        
                if ( type == NEXUS_AudioDspDebugType_eCoreDump )
                {
                    snprintf(pathname, sizeof(pathname), "%s.%u.0", pEnv, dsp);
                }
                else
                {
                    snprintf(pathname, sizeof(pathname), "%s.%u", pEnv, dsp);
                }
                /* coverity[tainted_string] */
                debugData[type].dsp[dsp].pFile = fopen(pathname, "wb+");
                if ( NULL == debugData[type].dsp[dsp].pFile )
                {
                    goto exit;
                }
            }    
        }
    }

    while ( false == g_audioLogThreadDone )
    {
        for ( type = 0; type < NEXUS_AudioDspDebugType_eMax; type++ )
        {
            if ( debugData[type].enabled )
            {
                for ( dsp = 0; dsp < NEXUS_NUM_AUDIO_DSP; dsp++ )
                {
                    bool logged=false;
                    do
                    {
                        errCode = NEXUS_AudioDsp_GetDebugBuffer(dsp, type, &pBuffer, &bufferSize);
                        if ( errCode != BERR_SUCCESS || bufferSize == 0 )
                        {
                            break;
                        }                        
                        (void)fwrite(pBuffer, bufferSize, 1, debugData[type].dsp[dsp].pFile);
                        NEXUS_AudioDsp_DebugReadComplete(dsp, type, bufferSize);
                        BDBG_MSG(("Wrote %u bytes of debug type %u", bufferSize, type));
                        logged = true;
                    } while ( 1 );

                    if ( logged == true && type == NEXUS_AudioDspDebugType_eCoreDump )
                    {
                        /* Increment file name for each core dump */
                        fclose(debugData[type].dsp[dsp].pFile);
                        debugData[type].dsp[dsp].index++;
                        snprintf(pathname, sizeof(pathname), "%s.%u.%u", NEXUS_GetEnv("audio_core_file"), dsp, debugData[type].dsp[dsp].index);
                        BDBG_MSG(("Opening audio core dump file %u", pathname));
                        /* coverity[tainted_string] */
                        debugData[type].dsp[dsp].pFile = fopen(pathname, "wb+");
                        if ( NULL == debugData[type].dsp[dsp].pFile )
                        {
                            BDBG_ERR(("Unable to open audio debug logfile %u", pathname));
                            goto exit;
                        }                            
                    }
                }
            }
        }
        BKNI_Sleep(100);
    }

exit:
    for ( type = 0; type < NEXUS_AudioDspDebugType_eMax; type++ )
    {
        for ( dsp = 0; dsp < NEXUS_NUM_AUDIO_DSP; dsp++ )
        {
            if ( NULL != debugData[type].dsp[dsp].pFile )
            {
                fclose(debugData[type].dsp[dsp].pFile);
                debugData[type].dsp[dsp].pFile = NULL;
            }
        }
    }

    pthread_exit(NULL);
    return NULL;
}
#endif

NEXUS_Error NEXUS_Platform_P_InitAudioLog(void)
{
#if NEXUS_PLATFORM_AUDIO_LOG
    if ( NEXUS_GetEnv("audio_uart_file") ||
         NEXUS_GetEnv("audio_debug_file") ||
         NEXUS_GetEnv("audio_core_file") ||
         NEXUS_GetEnv("audio_logs_enabled") )
    {
        if ( pthread_create(&g_audioLogThread, NULL, NEXUS_Platform_P_AudioLogThread, NULL) )
        {
            return BERR_TRACE(BERR_OS_ERROR);
        }
    }
#endif

    return BERR_SUCCESS;
}

void NEXUS_Platform_P_UninitAudioLog(void)
{
#if NEXUS_PLATFORM_AUDIO_LOG
    if ( (pthread_t) NULL != g_audioLogThread )
    {
        g_audioLogThreadDone = true;
        pthread_join(g_audioLogThread, NULL);
        g_audioLogThread = (pthread_t)NULL;
        g_audioLogThreadDone = false;
    }
#endif
}

