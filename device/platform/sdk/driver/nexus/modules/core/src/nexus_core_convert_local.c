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
* $brcm_Workfile: nexus_core_convert_local.c $
* $brcm_Revision: 9 $
* $brcm_Date: 4/12/12 4:00p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/7400/src/nexus_core_convert_local.c $
* 
* 9   4/12/12 4:00p jgarrett
* SW7425-2378: Adding Monkey's Audio and MLP
* 
* 8   3/14/12 4:59p erickson
* SW7429-18: merge
* 
* SW7429-18_20120120/1   3/14/12 4:42p gskerl
* SW7429-18: Added missing conversions for DtsHd and DtsLegacy.
* 
* 7   2/14/12 12:46p vsilyaev
* SW7425-2258: Added Motion Jpeg codec type
* 
* 6   2/7/12 7:13p jgarrett
* SW7425-2136: Adding FLAC audio
* 
* 5   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 4   12/29/11 1:14p jgarrett
* SW7231-500: Adding IEEE-1394 LPCM
* 
* 3   12/14/11 4:52p jgarrett
* SW7425-1950: Consolidating audio codec conversion routines into core
*  module
* 
* 2   8/15/11 3:11p vsilyaev
* SW7425-938: Added mapping for SH263 and VP8 video codecs
* 
* 1   4/15/11 7:39p vsilyaev
* SW7425-232: Local versions of convert functions
* 
***************************************************************************/

#include "nexus_base.h"
#include "nexus_types.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_core_convert_local);

static const struct {
    NEXUS_VideoCodec codec;
    BAVC_VideoCompressionStd std;
} g_Nexus_VideoCodecs[] = {
    {NEXUS_VideoCodec_eH263, BAVC_VideoCompressionStd_eH263},
    {NEXUS_VideoCodec_eH264, BAVC_VideoCompressionStd_eH264},
    {NEXUS_VideoCodec_eH264_Mvc, BAVC_VideoCompressionStd_eMVC},
    {NEXUS_VideoCodec_eH264_Svc, BAVC_VideoCompressionStd_eSVC},
    {NEXUS_VideoCodec_eVc1, BAVC_VideoCompressionStd_eVC1},
    {NEXUS_VideoCodec_eVc1SimpleMain, BAVC_VideoCompressionStd_eVC1SimpleMain},
    {NEXUS_VideoCodec_eMpeg1, BAVC_VideoCompressionStd_eMPEG1},
    {NEXUS_VideoCodec_eMpeg4Part2,BAVC_VideoCompressionStd_eMPEG4Part2},
    {NEXUS_VideoCodec_eDivx311, BAVC_VideoCompressionStd_eMPEG4Part2},
    {NEXUS_VideoCodec_eAvs, BAVC_VideoCompressionStd_eAVS},
    {NEXUS_VideoCodec_eRv40, BAVC_VideoCompressionStd_eRV9}, /* Codec ID used by Real is RV40, Real Video 9 is just a version of software first introduced implementation of RV40 video codec */
    {NEXUS_VideoCodec_eVp6, BAVC_VideoCompressionStd_eVP6},  /* Codec most often used in Flash Video */
    {NEXUS_VideoCodec_eVp7, BAVC_VideoCompressionStd_eVP7}, 
    {NEXUS_VideoCodec_eVp8, BAVC_VideoCompressionStd_eVP8},  /* WebM video codec */
    {NEXUS_VideoCodec_eMotionJpeg, BAVC_VideoCompressionStd_eMOTION_JPEG},  
    {NEXUS_VideoCodec_eSpark, BAVC_VideoCompressionStd_eSPARK},  /* Another codec for Flash Video */
    {NEXUS_VideoCodec_eMpeg2, BAVC_VideoCompressionStd_eMPEG2},
    {NEXUS_VideoCodec_eMpeg2, BAVC_VideoCompressionStd_eMPEG2DTV},
    {NEXUS_VideoCodec_eMpeg2, BAVC_VideoCompressionStd_eMPEG2_DSS_PES}
};

BAVC_VideoCompressionStd
NEXUS_P_VideoCodec_ToMagnum(NEXUS_VideoCodec codec, NEXUS_TransportType transportType)
{
    if(codec!=NEXUS_VideoCodec_eMpeg2) {
        unsigned i;
        for(i=0;i<sizeof(g_Nexus_VideoCodecs)/sizeof(*g_Nexus_VideoCodecs);i++) {
            if(g_Nexus_VideoCodecs[i].codec == codec) {
                return g_Nexus_VideoCodecs[i].std;
            }
        }
    }
    /* MPEG2 varieties */
    switch(transportType) {
    case NEXUS_TransportType_eDssEs:
        return BAVC_VideoCompressionStd_eMPEG2DTV;
    case NEXUS_TransportType_eDssPes:
        return BAVC_VideoCompressionStd_eMPEG2_DSS_PES;
    default:
        return BAVC_VideoCompressionStd_eMPEG2;
    }
}

NEXUS_VideoCodec
NEXUS_P_VideoCodec_FromMagnum(BAVC_VideoCompressionStd std)
{
    unsigned i;
    for(i=0;i<sizeof(g_Nexus_VideoCodecs)/sizeof(*g_Nexus_VideoCodecs);i++) {
        if(g_Nexus_VideoCodecs[i].std == std) {
            return g_Nexus_VideoCodecs[i].codec;
        }
    }
    BDBG_WRN(("unsupported BAVC_VideoCompressionStd:%u", std));
    return NEXUS_VideoCodec_eUnknown;
}

BAVC_AudioCompressionStd NEXUS_P_AudioCodec_ToMagnum(NEXUS_AudioCodec codec)
{
    switch ( codec )
    {
    case NEXUS_AudioCodec_eMpeg:
        /* Really, L1,L2,L3 are equivalent */
        return BAVC_AudioCompressionStd_eMpegL2;
    case NEXUS_AudioCodec_eMp3:
        return BAVC_AudioCompressionStd_eMpegL3;
    case NEXUS_AudioCodec_eAc3:
        return BAVC_AudioCompressionStd_eAc3;
    case NEXUS_AudioCodec_eAc3Plus:
        return BAVC_AudioCompressionStd_eAc3Plus;
    case NEXUS_AudioCodec_eAacAdts:
        return BAVC_AudioCompressionStd_eAacAdts;
    case NEXUS_AudioCodec_eAacLoas:
        return BAVC_AudioCompressionStd_eAacLoas;
    case NEXUS_AudioCodec_eAacPlusLoas:
        return BAVC_AudioCompressionStd_eAacPlusLoas;
    case NEXUS_AudioCodec_eAacPlusAdts:
        return BAVC_AudioCompressionStd_eAacPlusAdts;
    case NEXUS_AudioCodec_eDts:
        return BAVC_AudioCompressionStd_eDts;
    case NEXUS_AudioCodec_eDtsHd:
        return BAVC_AudioCompressionStd_eDtshd;
    case NEXUS_AudioCodec_eDtsLegacy:
        return BAVC_AudioCompressionStd_eDtsLegacy;
    case NEXUS_AudioCodec_eWmaStd:
        return BAVC_AudioCompressionStd_eWmaStd;
    case NEXUS_AudioCodec_eWmaStdTs:
        return BAVC_AudioCompressionStd_eWmaStdTs;
    case NEXUS_AudioCodec_eWmaPro:
        return BAVC_AudioCompressionStd_eWmaPro;
    case NEXUS_AudioCodec_ePcm:
        return BAVC_AudioCompressionStd_ePcm;
    case NEXUS_AudioCodec_ePcmWav:
        return BAVC_AudioCompressionStd_ePcmWav;
    case NEXUS_AudioCodec_eLpcmDvd:
        return BAVC_AudioCompressionStd_eLpcmDvd;
    case NEXUS_AudioCodec_eLpcmHdDvd:
        return BAVC_AudioCompressionStd_eLpcmHdDvd;
    case NEXUS_AudioCodec_eLpcmBluRay:
        return BAVC_AudioCompressionStd_eLpcmBd;
    case NEXUS_AudioCodec_eAmr:
        return BAVC_AudioCompressionStd_eAmr;
    case NEXUS_AudioCodec_eDra:
        return BAVC_AudioCompressionStd_eDra;
    case NEXUS_AudioCodec_eCook:
        return BAVC_AudioCompressionStd_eCook;
    case NEXUS_AudioCodec_eAdpcm:
        return BAVC_AudioCompressionStd_eAdpcm;
    case NEXUS_AudioCodec_eVorbis:
        return BAVC_AudioCompressionStd_eVorbis;
    case NEXUS_AudioCodec_eLpcm1394:
        return BAVC_AudioCompressionStd_eLpcm1394;
    case NEXUS_AudioCodec_eG711:
        return BAVC_AudioCompressionStd_eG711;
    case NEXUS_AudioCodec_eG723_1:
        return BAVC_AudioCompressionStd_eG723_1;
    case NEXUS_AudioCodec_eG726:
        return BAVC_AudioCompressionStd_eG726;
    case NEXUS_AudioCodec_eG729:
        return BAVC_AudioCompressionStd_eG729;
    case NEXUS_AudioCodec_eFlac:
        return BAVC_AudioCompressionStd_eFlac;
    case NEXUS_AudioCodec_eMlp:
        return BAVC_AudioCompressionStd_eMlp;
    case NEXUS_AudioCodec_eApe:
        return BAVC_AudioCompressionStd_eApe;
    default:
        return BAVC_AudioCompressionStd_eMax;
    }
}

NEXUS_AudioCodec NEXUS_P_AudioCodec_FromMagnum(BAVC_AudioCompressionStd std)
{
    switch ( std )
    {
    case BAVC_AudioCompressionStd_eMpegL1:
    case BAVC_AudioCompressionStd_eMpegL2:
        return NEXUS_AudioCodec_eMpeg;
    case BAVC_AudioCompressionStd_eMpegL3:
        return NEXUS_AudioCodec_eMp3;
    case BAVC_AudioCompressionStd_eAc3:
        return NEXUS_AudioCodec_eAc3;
    case BAVC_AudioCompressionStd_eAc3Plus:
        return NEXUS_AudioCodec_eAc3Plus;
    case BAVC_AudioCompressionStd_eAacAdts:
        return NEXUS_AudioCodec_eAacAdts;
    case BAVC_AudioCompressionStd_eAacLoas:
        return NEXUS_AudioCodec_eAacLoas;
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        return NEXUS_AudioCodec_eAacPlusLoas;
    case BAVC_AudioCompressionStd_eAacPlusAdts:
        return NEXUS_AudioCodec_eAacPlusAdts;
    case BAVC_AudioCompressionStd_eDts:
        return NEXUS_AudioCodec_eDts;
    case BAVC_AudioCompressionStd_eDtshd:
        return NEXUS_AudioCodec_eDtsHd;
    case BAVC_AudioCompressionStd_eDtsLegacy:
        return NEXUS_AudioCodec_eDtsLegacy;
    case BAVC_AudioCompressionStd_eWmaStd:
        return NEXUS_AudioCodec_eWmaStd;
    case BAVC_AudioCompressionStd_eWmaStdTs:
        return NEXUS_AudioCodec_eWmaStdTs;
    case BAVC_AudioCompressionStd_eWmaPro:
        return NEXUS_AudioCodec_eWmaPro;
    case BAVC_AudioCompressionStd_ePcm:
        return NEXUS_AudioCodec_ePcm;
    case BAVC_AudioCompressionStd_ePcmWav:
        return NEXUS_AudioCodec_ePcmWav;
    case BAVC_AudioCompressionStd_eLpcmDvd:
        return NEXUS_AudioCodec_eLpcmDvd;
    case BAVC_AudioCompressionStd_eLpcmHdDvd:
        return NEXUS_AudioCodec_eLpcmHdDvd;
    case BAVC_AudioCompressionStd_eLpcmBd:
        return NEXUS_AudioCodec_eLpcmBluRay;
    case BAVC_AudioCompressionStd_eAmr:
        return NEXUS_AudioCodec_eAmr;
    case BAVC_AudioCompressionStd_eDra:
        return NEXUS_AudioCodec_eDra;
    case BAVC_AudioCompressionStd_eCook:
        return NEXUS_AudioCodec_eCook;
    case BAVC_AudioCompressionStd_eAdpcm:
        return NEXUS_AudioCodec_eAdpcm;
    case BAVC_AudioCompressionStd_eVorbis:
        return NEXUS_AudioCodec_eVorbis;
    case BAVC_AudioCompressionStd_eLpcm1394:
        return NEXUS_AudioCodec_eLpcm1394;
    case BAVC_AudioCompressionStd_eG711:
        return NEXUS_AudioCodec_eG711;
    case BAVC_AudioCompressionStd_eG723_1:
        return NEXUS_AudioCodec_eG723_1;
    case BAVC_AudioCompressionStd_eG726:
        return NEXUS_AudioCodec_eG726;
    case BAVC_AudioCompressionStd_eG729:
        return NEXUS_AudioCodec_eG729;
    case BAVC_AudioCompressionStd_eFlac:
        return NEXUS_AudioCodec_eFlac;
    case BAVC_AudioCompressionStd_eMlp:
        return NEXUS_AudioCodec_eMlp;
    case BAVC_AudioCompressionStd_eApe:
        return NEXUS_AudioCodec_eApe;
    default:
        return NEXUS_AudioCodec_eMax;
    }    
}

