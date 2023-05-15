/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_utils.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 7/10/12 9:25a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_utils.c $
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 *
 ***************************************************************************/
 
/**************************** Include Files**********************************/ 
#include "framework.h"

/* Linux stuff */
#include <unistd.h>
#include <stdio.h>          /* for printf */
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>

#include "apetest_utils.h"
#include "apetest_cmdline.h"


/* xpt */
#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"

/* APE includes */
#include "bape.h"

/* DSP includes */
#include "bdsp.h"
#include "bdsp_raaga.h"

BDBG_MODULE(apetest_utils);





static struct {
    BAVC_AudioCompressionStd magnum;
    BDSP_AudioType bdsp;
} g_audioCodec_BavcBdsp[] = {
   {BAVC_AudioCompressionStd_eMax,          BDSP_AudioType_eMax},
   {BAVC_AudioCompressionStd_eMpegL1,       BDSP_AudioType_eMpeg},
   {BAVC_AudioCompressionStd_eMpegL3,       BDSP_AudioType_eMpeg},
   {BAVC_AudioCompressionStd_eAac,          BDSP_AudioType_eAacAdts},
   {BAVC_AudioCompressionStd_eAacPlus,      BDSP_AudioType_eAacAdts},
   {BAVC_AudioCompressionStd_eAacPlusAdts,  BDSP_AudioType_eAacAdts},
   {BAVC_AudioCompressionStd_eAacPlusLoas,  BDSP_AudioType_eAacLoas},
   {BAVC_AudioCompressionStd_eAc3,          BDSP_AudioType_eAc3},
   {BAVC_AudioCompressionStd_eAc3Plus,      BDSP_AudioType_eAc3Plus},
   {BAVC_AudioCompressionStd_eDts,          BDSP_AudioType_eDtsBroadcast},
   {BAVC_AudioCompressionStd_eLpcmHdDvd,    BDSP_AudioType_eLpcmHdDvd},
   {BAVC_AudioCompressionStd_eLpcmBd,       BDSP_AudioType_eLpcmBd},
   {BAVC_AudioCompressionStd_eDtshd,        BDSP_AudioType_eDtshd},
   {BAVC_AudioCompressionStd_eWmaStd,       BDSP_AudioType_eWmaStd},
   {BAVC_AudioCompressionStd_eWmaPro,       BDSP_AudioType_eWmaPro},
   {BAVC_AudioCompressionStd_eLpcmDvd,      BDSP_AudioType_eLpcmDvd},
   {BAVC_AudioCompressionStd_eAmr,          BDSP_AudioType_eAmr},
   {BAVC_AudioCompressionStd_eDra,          BDSP_AudioType_eDra},
   {BAVC_AudioCompressionStd_ePcmWav,       BDSP_AudioType_ePcmWav},
   {BAVC_AudioCompressionStd_eCook,         BDSP_AudioType_eRealAudioLbr}, 
   {BAVC_AudioCompressionStd_eVorbis,       BDSP_AudioType_eVorbis}

#if 0
    BDSP_AudioType_eAc3Plus
    BDSP_AudioType_eAacSbrAdts,      /* AAC_SBR ADTS */
    BDSP_AudioType_eAacSbrLoas,       /* AAC_SBR LOAS */
    BDSP_AudioType_eMlp,             /* MLP */
    BDSP_AudioType_ePcm,             /* PCM Data */
    BDSP_AudioType_eDtsLbr,          /* DTS-LBR */
    BDSP_AudioType_eDdp7_1,          /* DDP 7.1 */  
    BDSP_AudioType_eMpegMc,          /* MPEG MC*/        
    BDSP_AudioType_eLpcmDvdA,        /* LPCM A DVD*/
    BDSP_AudioType_eAdpcm,           /* ADPCM Decode*/   
    BDSP_AudioType_eG711G726,        /* G.711/G.726 Decode */   
    BDSP_AudioType_eG729,            /* G.729 Decode */   
    BDSP_AudioType_eVorbis,          /* Vorbis Decode */   
    BDSP_AudioType_eG723_1,          /* G.723.1 Decode */   
    BDSP_AudioType_eFlac,            /* Flac Decode */   
    BDSP_AudioType_eMax             /* Max value */
#endif
};


#define CONVERT_AVC_AUDIO(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].bdsp == bdsp_value) { \
            return g_struct[i].magnum; \
        } \
    } \
    printf("%s(%d) unable to find bdsp API value %d in %s", __FUNCTION__,__LINE__, bdsp_value, #g_struct); \
    return g_struct[0].magnum

#define CONVERT_MAGNUM_AUDIO(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].magnum == magnum_value) { \
            return g_struct[i].bdsp; \
        } \
    } \
    printf("%s(%d) unable to find Magnum value %d in %s",__FUNCTION__,__LINE__,  magnum_value, #g_struct); \
    return g_struct[0].bdsp


BAVC_AudioCompressionStd BdspAudio2BavcAudio(BDSP_AudioType bdsp_value)
{
    CONVERT_AVC_AUDIO(g_audioCodec_BavcBdsp);
}

BDSP_AudioType BavcAudio2BdspAudio(BAVC_AudioCompressionStd magnum_value)
{
    CONVERT_MAGNUM_AUDIO(g_audioCodec_BavcBdsp);
}



static struct {
    BAVC_AudioCompressionStd magnum;
    baudio_format settop;
} g_audioCodec_BavcBaudio[] = {
   {BAVC_AudioCompressionStd_eMax,          baudio_format_unknown},
   {BAVC_AudioCompressionStd_eMpegL1,       baudio_format_mpeg},
   {BAVC_AudioCompressionStd_eMpegL3,       baudio_format_mp3},
   {BAVC_AudioCompressionStd_eAac,          baudio_format_aac},
   {BAVC_AudioCompressionStd_eAacPlus,      baudio_format_aac_plus},
   {BAVC_AudioCompressionStd_eAacPlusAdts,  baudio_format_aac_plus_adts},
   {BAVC_AudioCompressionStd_eAacPlusLoas,  baudio_format_aac_plus_loas},
   {BAVC_AudioCompressionStd_eAc3,          baudio_format_ac3},
   {BAVC_AudioCompressionStd_eAc3Plus,      baudio_format_ac3_plus},
   {BAVC_AudioCompressionStd_eDts,          baudio_format_dts},
   {BAVC_AudioCompressionStd_eLpcmHdDvd,    baudio_format_lpcm_hddvd},
   {BAVC_AudioCompressionStd_eLpcmBd,       baudio_format_lpcm_bluray},
   {BAVC_AudioCompressionStd_eDtshd,        baudio_format_dts_hd},
   {BAVC_AudioCompressionStd_eWmaStd,       baudio_format_wma_std},
   {BAVC_AudioCompressionStd_eWmaPro,       baudio_format_wma_pro},
   {BAVC_AudioCompressionStd_eLpcmDvd,      baudio_format_lpcm_dvd},
   {BAVC_AudioCompressionStd_eAmr,          baudio_format_amr},
   {BAVC_AudioCompressionStd_eDra,          baudio_format_dra},
   {BAVC_AudioCompressionStd_ePcmWav,       baudio_format_pcm},
   {BAVC_AudioCompressionStd_eCook,         baudio_format_cook},
   {BAVC_AudioCompressionStd_eVorbis,       baudio_format_vorbis},
};


#define CONVERT_SETTOP_MAGNUM(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].magnum; \
        } \
    } \
    printf("%s(%d) unable to find Settop API value %d in %s", __FUNCTION__,__LINE__, settop_value, #g_struct); \
    return g_struct[0].magnum

#define CONVERT_MAGNUM_SETTOP(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].magnum == magnum_value) { \
            return g_struct[i].settop; \
        } \
    } \
    printf("%s(%d) unable to find Magnum value %d in %s",__FUNCTION__,__LINE__,  magnum_value, #g_struct); \
    return g_struct[0].settop


BAVC_AudioCompressionStd Baudio2BavcAudio(baudio_format settop_value)
{
    CONVERT_SETTOP_MAGNUM(g_audioCodec_BavcBaudio);
}

baudio_format BavcAudio2Baudio(BAVC_AudioCompressionStd magnum_value)
{
    CONVERT_MAGNUM_SETTOP(g_audioCodec_BavcBaudio);
}


BAVC_StreamType Bstream2BavcStream (bstream_mpeg_type type)
{
    switch (type)
    {
        /*not mapped
          BAVC_StreamType_eCms
          BAVC_StreamType_eTsBDRay
          */
        default:
        case bstream_mpeg_type_unknown:
        case bstream_mpeg_type_asf:     
        case bstream_mpeg_type_flv:     
        case bstream_mpeg_type_wav:     
        case bstream_mpeg_type_rmff:    
        case bstream_mpeg_type_ogg:     
        case bstream_mpeg_type_flac:    
        case bstream_mpeg_type_ape:     
        case bstream_mpeg_type_cdxa:     
        case bstream_mpeg_type_mp4_fragment:
            printf("%s(%d) type=%d unknown/can't map\n", __FUNCTION__, __LINE__, type);
            return ((BAVC_StreamType)-1);

        case bstream_mpeg_type_vob: 
            return (BAVC_StreamType_ePS);
        case bstream_mpeg_type_es:
            return (BAVC_StreamType_eEs);
        case bstream_mpeg_type_bes:      
            return (BAVC_StreamType_eBes);
        case bstream_mpeg_type_pes:     
            return (BAVC_StreamType_ePes);
        case bstream_mpeg_type_ts:      
            return (BAVC_StreamType_eTsMpeg);
        case bstream_mpeg_type_dss_es:
            return (BAVC_StreamType_eDssEs);
        case bstream_mpeg_type_dss_pes:
            return (BAVC_StreamType_eDssPes);
        case bstream_mpeg_type_avi:     
            return (BAVC_StreamType_eAVI);
        case bstream_mpeg_type_mpeg1:   
            return (BAVC_StreamType_eMpeg1System);
        case bstream_mpeg_type_mp4:     
            return (BAVC_StreamType_eMPEG4); 
        case bstream_mpeg_type_mkv:     
            return (BAVC_StreamType_eMKV);
    }
}


static const namevalue_t BAVC_AudioCompressionStd_Strs[] = {
    { "BAVC_AudioCompressionStd_eMpegL1",       BAVC_AudioCompressionStd_eMpegL1},
    { "BAVC_AudioCompressionStd_eMpegL2",       BAVC_AudioCompressionStd_eMpegL2},
    { "BAVC_AudioCompressionStd_eMpegL3",       BAVC_AudioCompressionStd_eMpegL3},
    { "BAVC_AudioCompressionStd_eAac",          BAVC_AudioCompressionStd_eAac},      
    { "BAVC_AudioCompressionStd_eAacAdts",      BAVC_AudioCompressionStd_eAacAdts},
    { "BAVC_AudioCompressionStd_eAacLoas",      BAVC_AudioCompressionStd_eAacLoas},
    { "BAVC_AudioCompressionStd_eAacPlus",      BAVC_AudioCompressionStd_eAacPlus}, 
    { "BAVC_AudioCompressionStd_eAacPlusLoas",  BAVC_AudioCompressionStd_eAacPlusLoas},
    { "BAVC_AudioCompressionStd_eAacPlusAdts",  BAVC_AudioCompressionStd_eAacPlusAdts},      
    { "BAVC_AudioCompressionStd_eAc3",          BAVC_AudioCompressionStd_eAc3},              
    { "BAVC_AudioCompressionStd_eAc3Plus",      BAVC_AudioCompressionStd_eAc3Plus},          
    { "BAVC_AudioCompressionStd_eAc3Lossless",  BAVC_AudioCompressionStd_eAc3Lossless},      
    { "BAVC_AudioCompressionStd_eDts",          BAVC_AudioCompressionStd_eDts},              
    { "BAVC_AudioCompressionStd_eDtshd",        BAVC_AudioCompressionStd_eDtshd},            
    { "BAVC_AudioCompressionStd_eDtsLegacy",    BAVC_AudioCompressionStd_eDtsLegacy},        
    { "BAVC_AudioCompressionStd_eWmaStd",       BAVC_AudioCompressionStd_eWmaStd},           
    { "BAVC_AudioCompressionStd_eWmaStdTs",     BAVC_AudioCompressionStd_eWmaStdTs},         
    { "BAVC_AudioCompressionStd_eWmaPro",       BAVC_AudioCompressionStd_eWmaPro},           
    { "BAVC_AudioCompressionStd_eMlp",          BAVC_AudioCompressionStd_eMlp},              
    { "BAVC_AudioCompressionStd_ePcm",          BAVC_AudioCompressionStd_ePcm},              
    { "BAVC_AudioCompressionStd_ePcmWav",       BAVC_AudioCompressionStd_ePcmWav},           
    { "BAVC_AudioCompressionStd_eLpcmDvd",      BAVC_AudioCompressionStd_eLpcmDvd},          
    { "BAVC_AudioCompressionStd_eLpcmHdDvd",    BAVC_AudioCompressionStd_eLpcmHdDvd},        
    { "BAVC_AudioCompressionStd_eLpcmBd",       BAVC_AudioCompressionStd_eLpcmBd},           
    { "BAVC_AudioCompressionStd_eLpcm1394",     BAVC_AudioCompressionStd_eLpcm1394},         
    { "BAVC_AudioCompressionStd_eAmr",          BAVC_AudioCompressionStd_eAmr},              
    { "BAVC_AudioCompressionStd_eDra",          BAVC_AudioCompressionStd_eDra},              
    { "BAVC_AudioCompressionStd_eCook",         BAVC_AudioCompressionStd_eCook},             
    { "BAVC_AudioCompressionStd_eAdpcm",        BAVC_AudioCompressionStd_eAdpcm},            
    { "BAVC_AudioCompressionStd_eSbc",          BAVC_AudioCompressionStd_eSbc},              
    { "BAVC_AudioCompressionStd_eVorbis",       BAVC_AudioCompressionStd_eVorbis},           
    { "BAVC_AudioCompressionStd_eG711",         BAVC_AudioCompressionStd_eG711},             
    { "BAVC_AudioCompressionStd_eG723_1",       BAVC_AudioCompressionStd_eG723_1},           
    { "BAVC_AudioCompressionStd_eG726",         BAVC_AudioCompressionStd_eG726},             
    { "BAVC_AudioCompressionStd_eG729",         BAVC_AudioCompressionStd_eG729},             
    { "BAVC_AudioCompressionStd_eFlac",         BAVC_AudioCompressionStd_eFlac},             
    { "BAVC_AudioCompressionStd_eMax",          BAVC_AudioCompressionStd_eMax},               
    { NULL,                                     0 }             
};

static const namevalue_t baudio_format_Strs[] = {
    {"baudio_format_unknown",           baudio_format_unknown           },
    {"baudio_format_mpeg",              baudio_format_mpeg              },
    {"baudio_format_mp3",               baudio_format_mp3               },
    {"baudio_format_aac",               baudio_format_aac               },
    {"baudio_format_aac_plus",          baudio_format_aac_plus          },
    {"baudio_format_aac_plus_adts",     baudio_format_aac_plus_adts     },
    {"baudio_format_aac_plus_loas",     baudio_format_aac_plus_loas     },
    {"baudio_format_ac3",               baudio_format_ac3               },
    {"baudio_format_ac3_plus",          baudio_format_ac3_plus          },
    {"baudio_format_dts",               baudio_format_dts               },
    {"baudio_format_lpcm_hddvd",        baudio_format_lpcm_hddvd        },
    {"baudio_format_lpcm_bluray",       baudio_format_lpcm_bluray       },
    {"baudio_format_dts_hd",            baudio_format_dts_hd            },
    {"baudio_format_wma_std",           baudio_format_wma_std           },
    {"baudio_format_wma_pro",           baudio_format_wma_pro           },
    {"baudio_format_lpcm_dvd",          baudio_format_lpcm_dvd          },
    {"baudio_format_g726",              baudio_format_g726              },
    {"baudio_format_adpcm",             baudio_format_adpcm             },
    {"baudio_format_dvi_adpcm",         baudio_format_dvi_adpcm         },
    {"baudio_format_avs",               baudio_format_avs               },
    {"baudio_format_pcm",               baudio_format_pcm               },
    {"baudio_format_amr",               baudio_format_amr               },
    {"baudio_format_vorbis",            baudio_format_vorbis            },
    {"baudio_format_flac",              baudio_format_flac              },
    {"baudio_format_ape",               baudio_format_ape               },
    {"baudio_format_dra",               baudio_format_dra               },
    {"baudio_format_cook",              baudio_format_cook              },
    {"baudio_format_dts_cd",            baudio_format_dts_cd            },
    {"baudio_format_lpcm_1394",         baudio_format_lpcm_1394         },
    { NULL,                             0                               }             
};

         
static const namevalue_t BAVC_StreamType_Strs[] = {
    {"BAVC_StreamType_eTsMpeg",          BAVC_StreamType_eTsMpeg        },
    {"BAVC_StreamType_eDssEs",           BAVC_StreamType_eDssEs         },
    {"BAVC_StreamType_ePes",             BAVC_StreamType_ePes           },
    {"BAVC_StreamType_eEs",              BAVC_StreamType_eEs            },
    {"BAVC_StreamType_eBes",             BAVC_StreamType_eBes           },
    {"BAVC_StreamType_eDssPes",          BAVC_StreamType_eDssPes        },
    {"BAVC_StreamType_ePS",              BAVC_StreamType_ePS            },
    {"BAVC_StreamType_eCms",             BAVC_StreamType_eCms           },   
    {"BAVC_StreamType_eTsBDRay",         BAVC_StreamType_eTsBDRay       },   
    {"BAVC_StreamType_eMpeg1System",     BAVC_StreamType_eMpeg1System   },   
    {"BAVC_StreamType_eAVI",             BAVC_StreamType_eAVI           },   
    {"BAVC_StreamType_eMPEG4",           BAVC_StreamType_eMPEG4         },   
    {"BAVC_StreamType_eMKV",             BAVC_StreamType_eMKV           },   
    { NULL,                              0                              }             
};                                                                    

static const namevalue_t bstream_mpeg_type_Strs[] = {
    {"bstream_mpeg_type_unknown",       bstream_mpeg_type_unknown       },    
    {"bstream_mpeg_type_es",            bstream_mpeg_type_es            },
    {"bstream_mpeg_type_bes",           bstream_mpeg_type_bes           },
    {"bstream_mpeg_type_pes",           bstream_mpeg_type_pes           },
    {"bstream_mpeg_type_ts",            bstream_mpeg_type_ts            },
    {"bstream_mpeg_type_dss_es",        bstream_mpeg_type_dss_es        },
    {"bstream_mpeg_type_dss_pes",       bstream_mpeg_type_dss_pes       },
    {"bstream_mpeg_type_vob",           bstream_mpeg_type_vob           },
    {"bstream_mpeg_type_asf",           bstream_mpeg_type_asf           },
    {"bstream_mpeg_type_avi",           bstream_mpeg_type_avi           },
    {"bstream_mpeg_type_mpeg1",         bstream_mpeg_type_mpeg1         },
    {"bstream_mpeg_type_mp4",           bstream_mpeg_type_mp4           },
    {"bstream_mpeg_type_flv",           bstream_mpeg_type_flv           },
    {"bstream_mpeg_type_mkv",           bstream_mpeg_type_mkv           },
    {"bstream_mpeg_type_wav",           bstream_mpeg_type_wav           },
    {"bstream_mpeg_type_rmff",          bstream_mpeg_type_rmff          },
    {"bstream_mpeg_type_mp4_fragment",  bstream_mpeg_type_mp4_fragment  },
    {"bstream_mpeg_type_ogg",           bstream_mpeg_type_ogg           },
    {"bstream_mpeg_type_flac",          bstream_mpeg_type_flac          },
    {"bstream_mpeg_type_ape",           bstream_mpeg_type_ape           },
    {"bstream_mpeg_type_cdxa",          bstream_mpeg_type_cdxa          },
    { NULL,                              0                              }             
};                                                                    
                                                                        
                                                                        
                                                                        

#if 0
static unsigned testApp_str2val(const namevalue_t *table, const char *name)
{    
    unsigned i;
    unsigned value;
    char *endptr;
    for (i=0;table[i].name;i++) {
        if (!strcasecmp(table[i].name, name)) {
            return table[i].value;
        }
    }
    value = strtol(name, &endptr, 0);
    if(!endptr || *endptr) { /* if valid, *endptr = '\0' */
        value = table[0].value;
    }
    printf("Unknown name '%s', using %u as value\n", name, value);
    return value;
}
#endif

static const char * val2str(const namevalue_t *table, unsigned value)
{
    unsigned i;
    for (i=0;table[i].name;i++) {
        if (table[i].value== value) {
            return table[i].name;
        }
    }
    printf("Unknown value %u, returning Unknown \n", value);
    return "Unknown";
}



const char * GetStr_bstream_mpeg_type(bstream_mpeg_type val)
{
    return val2str(bstream_mpeg_type_Strs, val);
}

const char * GetStr_baudio_format(baudio_format val)
{
    return val2str(baudio_format_Strs, val);
}

const char * GetStr_BAVC_AudioCompressionStd(BAVC_AudioCompressionStd val)
{
    return val2str(BAVC_AudioCompressionStd_Strs, val);
}

const char * GetStr_BAVC_StreamType(BAVC_StreamType val)
{
    return val2str(BAVC_StreamType_Strs, val);
}






bool APETEST_MenuParse (MENU_ENTRY *pMenu, char *cmd, unsigned int *opcode, int *arg_count)
{
    bool match;

    while (pMenu->cmd_opcode != MENU_INVALID)
    {
        match = strncmp(cmd, pMenu->cmd_name, strlen(cmd))== 0 ? true : false;
        if (match) 
        {
            *opcode = pMenu->cmd_opcode;
            *arg_count = pMenu->arg_count;
            return true;
        }
        pMenu++;
    }
    return false;
}

void APETEST_MenuPrintHelp (MENU_ENTRY *pMenu)
{
    int max_cmd = 7;
    int max_arg = 9;
    int len;
    MENU_ENTRY *pMenuTemp =  pMenu;

    while (pMenuTemp->cmd_opcode != MENU_INVALID)
    { 
        len = strlen(pMenuTemp->cmd_name);
        if (len > max_cmd) {
            max_cmd = len;
        }
        len = strlen(pMenuTemp->arg_desc);
        if (len > max_arg) {
            max_arg = len;
        }
        pMenuTemp++;
    }

    printf("%*s %*s %s\n", max_cmd, "Command",
           max_arg, "Arguments", "Description");
    printf("%*s %*s %s\n", max_cmd, "-------",
           max_arg, "---------", "-----------");

    pMenuTemp =  pMenu;
    while (pMenuTemp->cmd_opcode != MENU_INVALID)
    { 
        printf("%*s %*s %s\n", max_cmd, pMenuTemp->cmd_name,
               max_arg, pMenuTemp->arg_desc,pMenuTemp->cmd_desc);
        pMenuTemp++;
    }
}


bool  APETEST_MenuInputAvailable(void)  
{
  struct timeval tv;
  fd_set fds;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
  return (FD_ISSET(0, &fds));
}


void APETEST_MenuPrtPrompt (void)
{
    /* print the prompt */
    printf(">");
    fflush(stdout);
}

bool APETEST_MenuGetCmdBlock(MENU_ENTRY *pMenu, unsigned int *opcode, MENU_ARGS *pMenuArg)
{
    char delimiters[] = " \012";
    char *cmd;
    int x;
    int arg_count;

    fgets(pMenuArg->line, sizeof(pMenuArg->line), stdin);
    cmd = strtok(pMenuArg->line, delimiters);
    if (cmd == NULL || *cmd == '\000') {
        return false;
    }

    if (!APETEST_MenuParse(pMenu, cmd, opcode, &arg_count)) {
        printf("Unknown command '%s'\n\n", cmd);
        APETEST_MenuPrintHelp(pMenu);
        return false;
    }

    for (x = 0; x < arg_count; x++) {
        pMenuArg->argv[x] = strtok(NULL, delimiters);
        printf("pMenuArg->argv[%d]=%s\n", x, pMenuArg->argv[x]);

        if (pMenuArg->argv[x] == NULL) {
            printf("Illegal command - incorrect arguments '%s' \n\n", pMenuArg->line);
            return false;
        }
    }

    /* if the arguments were not correct, try again */
    if (x != arg_count) {
        printf("Incorrect number of arguments %d\n\n", arg_count);
        return false;
    }

    pMenuArg->argc = arg_count;

    return true;
}


#define MENU_SLEEP_TIME_MSECS (20)
bool APETEST_MenuGetCmd (volatile bool * pExit, MENU_ENTRY *pMenu, unsigned int *opcode, MENU_ARGS *pMenuArg)
{
    bool valid = false;


    printf("\n");
    APETEST_MenuPrtPrompt();

    while (!valid)
    {
        if (pExit != NULL)
        {
            if (*pExit)
            {
                break;
            }
        }

        if (APETEST_MenuInputAvailable())
        {
            valid=APETEST_MenuGetCmdBlock(pMenu, opcode, pMenuArg);
            if (!valid)
            {
                APETEST_MenuPrtPrompt();
            }
        }
        else
        {
            usleep(MENU_SLEEP_TIME_MSECS * 1000);
        }
    }

    return valid;
}


#if BDBG_DEBUG_BUILD

/* Get a user-configurable setting. This is useful for development. */
static const char *getConfig(const char *name)
{
#if defined(__vxworks) || defined(B_SYSTEM_linuxkernel)
    int i;
    for (i=0;i<g_config_total;i++) {
        if (!strcmp(g_config[i].name, name))
            return g_config[i].value;
    }
    return NULL;
#else
    /* linux user - get a value from the environment */
    return getenv(name);
#endif
}


/****************************************************************************
Summary: Helper Function that sets the debug level.
*****************************************************************************/
static void setModuleDebugLevel(
    const char              *modulelist, 
    BDBG_Level              level
)
{
    while (modulelist && *modulelist) {
        const char *end = strchr(modulelist, ',');
        if (!end) 
        {
            BDBG_SetModuleLevel(modulelist, level);
            break;
        }
        else 
        {
            int n = end-modulelist;
            char *buf = (char *)BKNI_Malloc(n+1);
            /* NOTE: memory leak, but this is debug */
            strncpy(buf, modulelist, n);
            buf[n] = 0;
            BDBG_SetModuleLevel(buf, level);
        }
        modulelist = ++end;
    }
}


void APETEST_InitModuleDebug(void)
{
    BDBG_Init();        
    setModuleDebugLevel(getConfig("trace_modules"), BDBG_eTrace);
    setModuleDebugLevel(getConfig("msg_modules"), BDBG_eMsg);
    setModuleDebugLevel(getConfig("wrn_modules"), BDBG_eWrn);
}
#endif /* BDBG_DEBUG_BUILD */





/* wait for the Decoder to get Done decoding */
void APETEST_WaitForDecoderDone(BAPE_DecoderHandle handle)
{
    BAPE_DecoderStatus decStatus;
    unsigned int framesDecodedLast;

    printf("\nwaitForDecoderDone");
    decStatus.framesDecoded = -1;
    do
    {
        printf(".");
        fflush(stdout);
        BKNI_Sleep(200);  /* wait longer than the longest frame time */
        framesDecodedLast = decStatus.framesDecoded;
        BAPE_Decoder_GetStatus(handle, &decStatus);
    }
    while ( decStatus.framesDecoded !=  framesDecodedLast);
}
