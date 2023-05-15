/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_fragment_demux.c $
 * $brcm_Revision: 14 $
 * $brcm_Date: 9/7/12 11:55a $
 *
 * Module Description:
 *
 * MP4 container parser library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_fragment_demux.c $
 *
 * 14   9/7/12 11:55a vsilyaev
 * SW7552-340: Fixed the error handling case
 *
 * 13   9/13/11 5:09p vsilyaev
 * SW7425-1219: Added handling of EOS sequence
 *
 * 12   8/18/11 5:33p vsilyaev
 * SW7468-69: Added support for AAC codec in RMFF format
 *
 * 11   8/3/11 3:09p vsilyaev
 * SWBLURAY-26343: Added handling for avc1 track type
 *
 * 10   6/24/11 1:52p vsilyaev
 * SWBLURAY-26343: Added hooks to allow delivery of Track Fragment and
 * samples to the application
 *
 * 9   5/26/11 2:46p vsilyaev
 * SW7425-615: Improved handling of base_data offset in the track header
 *
 * 8   5/17/11 11:36a vsilyaev
 * SW7420-1868: Try to handle negative values of 'composition time
 * offset'\
 *
 * 7   5/16/11 2:26p vsilyaev
 * SW7420-1868: Use composition_time_offset
 *
 * 6   4/22/11 12:46p vsilyaev
 * SW7420-1821: Updated metadata parsing in aac streams
 *
 * 5   5/11/10 5:08p vsilyaev
 * SW7405-3773: Added support for WMA Pro audio
 *
 * 4   5/11/10 12:54p vsilyaev
 * SW7405-4311: Fixed warning
 *
 * 3   5/10/10 6:28p vsilyaev
 * SW7405-3773: Added support for WMC1 video
 *
 * 2   5/7/10 1:22p vsilyaev
 * SW7405-3773: Fixed warning
 *
 * 1   5/6/10 3:35p vsilyaev
 * SW7405-3773: Demux for fragments of MP$ fragmented file
 *
 *******************************************************************************/
#include "bstd.h"
#include "blst_slist.h"
#include "biobits.h"
#include "bkni.h"
#if defined(HUMAX_PLATFORM_BASE)
#include "bkni_multi.h"
#include "basf_stream.h"
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
#include <stdio.h>
#include <string.h>
#include "drmmanager.h"
#endif
#include "common_crypto.h"
#endif
#include "bmp4_util.h"
#include "bmp4_fragment_demux.h"

BDBG_MODULE(bmp4_fragment_demux);

BDBG_OBJECT_ID(bmp4_fragment_demux);
BDBG_OBJECT_ID(bmp4_fragment_stream);

#define BDBG_MSG_TRACE(x)    /*BDBG_MSG(x)*/

#define B_MP4_STREAM_VEC_FRAME	0
#define B_MP4_STREAM_VEC_RAI    1
#define B_MP4_STREAM_VEC_EOS	2

typedef enum b_mp4_fragment_demux_state {
    b_mp4_fragment_demux_state_box, /* looking for the box */
    b_mp4_fragment_demux_state_capture, /* capturing box */
    b_mp4_fragment_demux_state_remux /* parse captured box */
} b_mp4_fragment_demux_state;

typedef struct b_mp4_fragment_box {
    uint32_t type;
    int start;
    size_t len;
    size_t header_len;
    size_t next; /* next reading point (inside the segment) */
} b_mp4_fragment_box;

typedef struct b_mp4_fragment_track_state {
    struct {
        uint64_t run_offset;
        uint64_t run_start_time;
    } next, current;
} b_mp4_fragment_track_state;

typedef struct b_mp4_fragment_header {
    uint64_t timescale;
    uint64_t starttime;
    uint32_t fourcc;
} b_mp4_fragment_header;

typedef struct b_mp4_fragment_codec_private {
    batom_t data;
} b_mp4_fragment_codec_private;

typedef struct b_mp4_fragment_track {
    bool next_sample_valid;
    bool fragment_run_valid;
    bool fragment_header_valid;
    bool movie_fragment_valid;
    bool movie_fragment_data_valid;
    bool track_eos;

#if defined(HUMAX_PLATFORM_BASE)
	/*****************/
	/* PIFF by ykkim */
	/*****************/
	bool stdp_valid;
	bool sample_encryption_valid;
	bool sample_encryption_check;
	bool uuid_parsed;
	bool senc_parsed;
	bmp4_ext_sample_encryption_box sample_encryption_box;
	bmp4_ext_sample_encryption next_sample_encryption;
	batom_cursor encryption_cursor;
	batom_cursor stdp_cursor;
    b_mp4_fragment_box stdp;
#endif

    unsigned next_sample_no;
    bmp4_track_fragment_header track_fragment_header;
    bmp4_track_fragment_run_header run_header;
    bmp4_track_fragment_run_state run_state;
    bmp4_track_fragment_run_sample next_sample;

    b_mp4_fragment_box root;
    b_mp4_fragment_box movie_fragment;
    b_mp4_fragment_box movie_fragment_data;
    b_mp4_fragment_box track_fragment;
    b_mp4_fragment_box track_fragment_run;
    b_mp4_fragment_box track_fragment_header_box;
    b_mp4_fragment_header fragment_header;
    b_mp4_fragment_codec_private codec_private;
    bmp4_trackextendsbox track_extends;
    batom_cursor track_cursor;
    b_mp4_fragment_track_state state;
    int (*payload_handler)(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, unsigned frame_no, uint32_t timestamp, batom_cursor *payload, size_t payload_len);
} b_mp4_fragment_track;

struct bmp4_fragment_stream {
    BDBG_OBJECT(bmp4_fragment_stream)
    BLST_S_ENTRY(bmp4_fragment_stream) link;
    unsigned stream_id;
    bool busy; /* set to true if currently for this track there is a process of converting fragment to individual frames */
    batom_accum_t data;
    batom_accum_t frame_accum;
    batom_pipe_t pipe_out;
    b_mp4_fragment_track track;
    bmp4_fragment_stream_cfg cfg;
    const batom_vec *vecs[3];
    struct {
        bmedia_adts_header adts_header;
    } aac;
} bmp4_fragment_stream;

struct bmp4_fragment_demux {
    BDBG_OBJECT(bmp4_fragment_demux)
    batom_accum_t accum;
    bmp4_fragment_demux_cfg config;
    size_t box_size;
    size_t box_header_size;
    uint64_t offset;
    BLST_S_HEAD(b_mp4_fragment_stream, bmp4_fragment_stream) streams;
    b_mp4_fragment_demux_state demux_state;
    unsigned next_fragment_track_ID; /* track ID of the next fragment to parse, it's set to not 0, if frame with the same ID is already parsed  */
    struct {
        void *nal_prefix;
        batom_vec nal_vec;
    } h264;

#if defined(HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	unsigned char* wrmheader;
	unsigned int wrmheaderlen;
	DRM_DECRYPT_CONTEXT stDecryptContext;
	CommonCryptoHandle cryptoHandle;
	NEXUS_KeySlotHandle keyPrdy;
	unsigned char* pEkl;
#if defined(CONFIG_PLAYREADY_25)
	bool bSetContext;
	DRM_APP_CONTEXT* pAppContext;
	DRM_VOID *f_pOEMContext;
	void*pDecryptBuf;
	int32_t nDecryptBufSize;
#endif
#endif
#endif
};

#if defined(HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
/******************************
*	PlayReady Callback
******************************/
extern httpclient_license_post_callback_t soap_post_callback;
extern httpclient_license_post_callback_t default_post_callback;
extern get_secure_clock_callback_t secure_clock_callback;

#if defined(CONFIG_PLAYREADY_25)
const DRM_WCHAR bmp4_rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('y','\0'),ONE_WCHAR('r','\0'), ONE_WCHAR('e','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('d','\0'),ONE_WCHAR('y','\0'), ONE_WCHAR('2','\0'), ONE_WCHAR('5','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')
};

#else
const DRM_WCHAR bmp4_rgwchStoreName [] =
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0')
};
#endif

DRM_RESULT bmp4_policy_callback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType, void *f_pv)
{
	BSTD_UNUSED(f_pvPolicyCallbackData);
	BSTD_UNUSED(f_dwCallbackType);
	BSTD_UNUSED(f_pv);
	return DRM_SUCCESS;
}

DRM_RESULT bmp4_commit_callback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType,void *f_pv)
{
	BSTD_UNUSED(f_pvPolicyCallbackData);
	BSTD_UNUSED(f_dwCallbackType);
	BSTD_UNUSED(f_pv);
	return DRM_SUCCESS;
}

DRM_CONTENT_SET_PROPERTY b_mp4_fragment_check_header_version(bmp4_fragment_demux_t demux)
{
	unsigned int uiOffset=0;
	DRM_CONTENT_SET_PROPERTY eVersion=DRM_CSP_HEADER_NOT_SET;

/*************************
*	WRM Validation check
*************************/
	if(demux->wrmheaderlen < 60)
	{
		BDBG_ERR(("Invalid WRM HEADER! too small!\n"));
		return eVersion;
	}

/*************************
*	WRMHEADER tag with version info
*************************/
	while(uiOffset <= (demux->wrmheaderlen-18))
	{
		if((demux->wrmheader[uiOffset] == 'v') && (demux->wrmheader[uiOffset+2] == 'e') && (demux->wrmheader[uiOffset+4] == 'r') &&
			(demux->wrmheader[uiOffset+6] == 's') && (demux->wrmheader[uiOffset+8] == 'i') && (demux->wrmheader[uiOffset+10] == 'o') && (demux->wrmheader[uiOffset+12] == 'n'))
		{
			break;
		}
		uiOffset++;
	}

	if(demux->wrmheader[uiOffset+18] == '2')
	{
		eVersion = DRM_CSP_V2_HEADER;
	}
	else if(demux->wrmheader[uiOffset+18] == '4')
	{
		eVersion = DRM_CSP_V4_HEADER;
	}
	else
	{
		BDBG_ERR(("Invalid version!\n"));
	}

	return eVersion;
}

void b_mp4_fragment_get_license(bmp4_fragment_demux_t demux)
{
	DRM_RESULT dResult;
	DRM_APP_CONTEXT* pAppContext = NULL;
	DRM_VOID *f_pOEMContext = NULL;
	DRM_BYTE *__macro_pbOpaqueBuffer = NULL;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(bmp4_rgwchStoreName);
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	DRM_DWORD f_cRights=0;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	DRM_BYTE* f_pbChallenge=NULL;
	DRM_DWORD f_pcbChallenge=0;
	DRM_BYTE* pucResponseData=NULL;
	DRM_BYTE* pucResponseStart=NULL;
	DRM_WCHAR32 uiResponseSize=0;
	DRM_BYTE *pbRevocationBuffer=NULL;
	DRM_LICENSE_RESPONSE LicenseResponse;
	int iResult;
	FILE* fd = NULL;

   DRM_CHAR rgchCustomData[ 2048 ] = "";
   DRM_CHAR *pRgchCustomData=NULL;
   DRM_DWORD cchCustomData = 0;
   char *g_tmp_customdata = NULL;

	BKNI_Memset(&LicenseResponse, 0, sizeof (DRM_LICENSE_RESPONSE));
	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;

	pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(pAppContext == NULL)
	{
		BDBG_ERR(("Out of memory!\n"));
		return;
	}

	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	if(__macro_pbOpaqueBuffer == NULL)
	{
		BDBG_ERR(("Out of memory!\n"));
		Oem_MemFree(pAppContext);
		return;
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		BDBG_ERR(("Oem_Create failed!\n"));
		Oem_MemFree(pAppContext);
		Oem_MemFree(__macro_pbOpaqueBuffer);
		return;
	}
#endif

/*************************
*	1. Drm Init
*************************/
	dResult = Drm_Initialize(pAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Initialize failed!\n"));
		goto Done;
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	if(secure_clock_callback != NULL)
	{
		secure_clock_callback(pAppContext);
	}
#endif


/*************************
*	2. Try to Bind
*************************/
	eHeaderVersion = b_mp4_fragment_check_header_version(demux);
	dResult = Drm_Content_SetProperty(pAppContext, eHeaderVersion, demux->wrmheader, demux->wrmheaderlen);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Content_SetProperty failed! 0x%X\n", dResult));
		goto Done;
	}

	dResult = Drm_Reader_Bind(pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)bmp4_policy_callback, NULL, &demux->stDecryptContext);
	if(dResult == DRM_SUCCESS)
	{
		BDBG_ERR(("License is already in SecureStore!\n"));
		goto Done;
	}

/*************************
*	3. make challenge
*************************/
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);

	fd = fopen("/var/lib/humaxtv/drm/custom.data","rb");
	if(fd == NULL)
	{
		BDBG_ERR(("No custom.data in /var/lib/humaxtv/drm\n"));
	}
	else
	{
		int url_lenght = 0;

		fseek(fd, 0, SEEK_END);
		url_lenght = ftell(fd);
		fseek(fd, 0, SEEK_SET);

		if(url_lenght != 0)
		{
			g_tmp_customdata = (DRM_CHAR*) Oem_MemAlloc(url_lenght + 1);
			memset(g_tmp_customdata, 0x0, url_lenght + 1);
			fread(g_tmp_customdata, url_lenght, 1, fd);
		}

		fclose(fd);
		system("rm -rf /var/lib/humaxtv/drm/custom.data");
	}


	memset(rgchCustomData, 0x00, sizeof(rgchCustomData));

	/* CUSTOM DATA */
	if(g_tmp_customdata != NULL)
	{
	   printf("pucCustomData is exist!\n");
	   /* Init */

	   pRgchCustomData = rgchCustomData;

	   /* DRM_STR */
	   DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), g_tmp_customdata );
	   DRM_STR_StringCchLengthA( rgchCustomData, SIZEOF( rgchCustomData ), &cchCustomData );
	}

	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		rgpdstrRights,
		f_cRights,
		NULL,
		pRgchCustomData,
		cchCustomData,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(f_pcchSilentURL * SIZEOF (DRM_CHAR));
	    f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc(f_pcbChallenge * SIZEOF(DRM_BYTE));

		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			rgpdstrRights,
			f_cRights,
			NULL,
			pRgchCustomData,
			cchCustomData,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
		f_pbChallenge[f_pcbChallenge] = '\0';
	}
	else
	{
		BDBG_ERR(("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult));
		goto Done;
	}
	/*Check Return generateChallenge*/
	BDBG_ERR(("Drm_LicenseAcq_GenerateChallenge return ! 0x%X\n", dResult));

	fd = fopen("/var/lib/humaxtv/drm/license.url","rb");
	if(fd == NULL)
	{
		BDBG_ERR(("No license.url in /var/lib/humaxtv/drm\n"));
	}
	else
	{
		int url_lenght = 0;

		fseek(fd, 0, SEEK_END);
		url_lenght = ftell(fd);
		fseek(fd, 0, SEEK_SET);
		
		if(url_lenght != 0)
		{
			Oem_MemFree(f_pchSilentURL);
			f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(url_lenght + 1);
			memset(f_pchSilentURL, 0x0, url_lenght + 1);
			fread(f_pchSilentURL, url_lenght, 1, fd);
		}

		fclose(fd);
		system("rm -rf /var/lib/humaxtv/drm/license.url");
	}

/*************************
*	4. Post
*************************/
	if(eHeaderVersion == DRM_CSP_V4_HEADER)
	{
		iResult = soap_post_callback(
			(char*)f_pchSilentURL,
			(char*)f_pbChallenge,
			0,
			2000,
			HDR_LICGET_ACTION_VALUE,
			&pucResponseData,
			&pucResponseStart,
			&uiResponseSize);
		if(iResult != 0)
		{
			BDBG_ERR(("bhttpclient_license_post_soap failed!\n"));
			goto Done;
		}
	}
	else
	{
		iResult = default_post_callback(
				(char*)f_pchSilentURL,
				(char*)f_pbChallenge,
				0,
				2000,
				NULL,
				&pucResponseData,
				&pucResponseStart,
				&uiResponseSize);
		if(iResult != 0)
		{
			BDBG_ERR(("bhttpclient_license_post_default failed!\n"));
			goto Done;
		}
	}

/*************************
*	5. Process
*************************/
	pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc(REVOCATION_BUFFER_SIZE);
	dResult = Drm_Revocation_SetBuffer(pAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Revocation_SetBuffer failed! [%x]\n", dResult));
	}

	dResult = Drm_LicenseAcq_ProcessResponse(pAppContext, DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
		NULL,
		NULL,
		pucResponseStart,
		uiResponseSize,
		&LicenseResponse);
	if(dResult == DRM_SUCCESS)
	{
		BDBG_ERR(("License is Stored well!\n"));
	}
	else if (dResult == DRM_S_MORE_DATA)
	{
		BDBG_ERR(("License is Stored well, and some of custom data exist!\n"));
	}
	else
	{
		BDBG_ERR(("Drm_LicenseAcq_ProcessResponse failed 0x%X\n", dResult));
	}

/*************************
*	6. Bind again
*************************/
	dResult = Drm_Reader_Bind(pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)bmp4_policy_callback, NULL, &demux->stDecryptContext);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Reader_Bind failed! 0x%X\n", dResult));
		goto Done;
	}

/*************************
*	7. commit
*************************/
	dResult = Drm_Reader_Commit(pAppContext, (DRMPFNPOLICYCALLBACK)bmp4_commit_callback, NULL);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Reader_Commit failed! 0x%X\n", dResult));
		goto Done;
	}

Done:
#if defined(CONFIG_PLAYREADY_25)
	demux->pAppContext = pAppContext;
	demux->bSetContext = TRUE;
#else
	Drm_Uninitialize(pAppContext);

	if(pAppContext != NULL)
	{
		Oem_MemFree(pAppContext);
	}
#endif
	if(f_pbChallenge != NULL)
	{
		Oem_MemFree(f_pbChallenge);
	}
	if(f_pchSilentURL != NULL)
	{
		Oem_MemFree(f_pchSilentURL);
	}
	if(pbRevocationBuffer != NULL)
	{
		Oem_MemFree(pbRevocationBuffer);
	}
	if(__macro_pbOpaqueBuffer != NULL)
	{
		Oem_MemFree(__macro_pbOpaqueBuffer);
	}
	if(g_tmp_customdata != NULL)
	{	
		Oem_MemFree(g_tmp_customdata);
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
#if defined(CONFIG_PLAYREADY_25)
	demux->f_pOEMContext = f_pOEMContext;
#else
	if(f_pOEMContext != NULL)
	{
		Oem_Destroy(f_pOEMContext);
		f_pOEMContext = NULL;
	}
#endif
#endif

	return;
}

static size_t b_mp4_atom_cursor_refill(batom_cursor *cursor)
{
	if (cursor->left==0)
	{
		unsigned pos;
		const batom_vec *vec;

		for(pos=cursor->pos,vec=&cursor->vec[pos];pos < cursor->count;vec++)
		{
			pos++;
			cursor->pos = pos;
			if (vec->len>0)
			{
				cursor->left = vec->len;
				cursor->cursor = vec->base;
				return (size_t)cursor->left;
			}
		}

		cursor->left = BATOM_EOF;
	}

	return 0;
}

size_t b_mp4_map_cursor_to_dmablks(batom_cursor *cursor, size_t count, NEXUS_DmaJobBlockSettings *blks, uint32_t nb_blks_avail, uint32_t *nb_blks_used)
{
    size_t left;
    uint32_t ii = 0;

    for(left=count; ii < nb_blks_avail; ) {
        const uint8_t *src=cursor->cursor;
        int src_left = cursor->left;

        if(src_left>=(int)left)
		{
            cursor->cursor = src+left;
            cursor->left = src_left-left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = left;

            left = 0;
            ii++;
            break;
        }
        if(src_left>=0)
		{
            cursor->cursor = src+src_left;
            cursor->left = 0;
            left -= src_left;
            NEXUS_DmaJob_GetDefaultBlockSettings(&blks[ii]);
            blks[ii].pSrcAddr  = (uint8_t *)src;
            blks[ii].pDestAddr = (uint8_t *)src;
            blks[ii].blockSize = src_left;
            ii++;

            if (b_mp4_atom_cursor_refill(cursor)==0)
			{
                break;
            }
        } else {
            break;
        }
    }
    (*nb_blks_used) = ii;
	return count-left;
}

#if defined(CONFIG_PLAYREADY_25)

size_t b_mp4_put_decrypted_data(batom_cursor *cursor, size_t count, uint8_t* pBuf)
{
    size_t left;
    uint32_t offset = 0;

	for(left=count; ;)
	{
		const uint8_t *src=cursor->cursor;
		int src_left = cursor->left;

		if(src_left>=(int)left)
		{
			BKNI_Memcpy(src, &pBuf[offset], left);
			cursor->cursor = src+left;
			cursor->left = src_left-left;
			left = 0;
			break;
		}
		if(src_left>=0)
		{
			BKNI_Memcpy(src, &pBuf[offset], src_left);
			cursor->cursor = src+src_left;
			cursor->left = 0;
			left -= src_left;
			offset+= src_left;

			if(b_mp4_atom_cursor_refill(cursor)==0)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return count-left;
}

size_t b_mp4_extract_encrypted_data(batom_cursor *cursor, size_t count, uint8_t* pBuf)
{
    size_t left;
    uint32_t offset = 0;

	for(left=count; ;)
	{
		const uint8_t *src=cursor->cursor;
		int src_left = cursor->left;

		if(src_left>=(int)left)
		{
			BKNI_Memcpy(&pBuf[offset], src, left);
			cursor->cursor = src+left;
			cursor->left = src_left-left;
			left = 0;
			break;
		}
		if(src_left>=0)
		{
			BKNI_Memcpy(&pBuf[offset], src, src_left);
			cursor->cursor = src+src_left;
			cursor->left = 0;
			left -= src_left;
			offset += src_left;

			if(b_mp4_atom_cursor_refill(cursor)==0)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	return count-left;
}

void b_mp4_DoDecrypt_sw(bmp4_fragment_demux_t demux, unsigned char* pucData, unsigned int uiSize, unsigned char* iv)
{
	DRM_BYTE *last_15_bytes;
	DRM_RESULT dr;
	static DRM_AES_COUNTER_MODE_CONTEXT m_aes_context;

/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		BDBG_ERR(("Buffer is NULL!\n"));
		return;
	}

/*************************
*	2. Cocktail decrypt
*************************/
	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}

	if(iv != NULL)
	{
		m_aes_context.qwInitializationVector = (DRM_UINT64)iv[8] << 56 | (DRM_UINT64)iv[9] << 48 | (DRM_UINT64)iv[10] << 40 | (DRM_UINT64)iv[11] << 32 | (DRM_UINT64)iv[12] << 24 | (DRM_UINT64)iv[13] << 16 | (DRM_UINT64)iv[14] << 8 | (DRM_UINT64)iv[15];
		m_aes_context.qwBlockOffset = (DRM_UINT64)iv[0] << 56 | (DRM_UINT64)iv[1] << 48 | (DRM_UINT64)iv[2] << 40 | (DRM_UINT64)iv[3] << 32 | (DRM_UINT64)iv[4] << 24 | (DRM_UINT64)iv[5] << 16 | (DRM_UINT64)iv[6] << 8 | (DRM_UINT64)iv[7];
		m_aes_context.bByteOffset = 0;
	}
	dr = Drm_Reader_InitDecrypt(&demux->stDecryptContext, last_15_bytes, uiSize);
	if (dr == DRM_SUCCESS)
	{
		dr = Drm_Reader_Decrypt(&demux->stDecryptContext, &m_aes_context, pucData, uiSize);
	}
	else
	{
		BDBG_ERR(("InitDecrypt failed = 0x%x\n", dr));
	}
	return;
}
#endif
void b_mp4_DoDecryptSample(bmp4_fragment_demux_t demux, bmp4_ext_sample_encryption_box* sample_encryption_box, unsigned int sample_no, batom_cursor* data, unsigned int uiSize, unsigned char *pIV)
{
	batom_checkpoint start;
	DRM_CIPHER_CONTEXT *cipher_ctx;
	bmp4_ext_sample_encryption* pSample;
	unsigned char* pEkl=NULL;
	NEXUS_DmaJobBlockSettings stBlkSettings[100];
	unsigned jj=0;
	size_t bytes_ref;
	uint32_t nb_Blks=1;
	uint32_t nb_blks_used=0;
	CommonCryptoJobSettings jobSettings;
	NEXUS_Error nResult;

/*************************
*	1. NULL Check
*************************/
	if((sample_encryption_box == NULL) || (data == NULL) || (pIV == NULL))
	{
		BDBG_ERR(("Invalid Parameter!\n"));
		return;
	}

	batom_cursor_save(data, &start);

/*************************
*	2. get key & iv
*************************/
	cipher_ctx = (DRM_CIPHER_CONTEXT* )demux->stDecryptContext.rgbBuffer;
	pEkl = demux->pEkl;
	pSample = &sample_encryption_box->samples[sample_no];

#if defined(CONFIG_PLAYREADY_25)
	if(sample_encryption_box->fullbox.flags &= 0x000002)
	{
		uint32_t uiEncryptedTotalSize=0;
		uint32_t uiOffset=0;
		void* pBuf = NULL;

		/* calculrate buffer size for decryting */
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			uiEncryptedTotalSize += pSample->entries[jj].BytesOfEncryptedData;
		}

		/* reallocate memory for temporary buffer of decryption if there is not enough space */
		if(demux->nDecryptBufSize < uiEncryptedTotalSize + 128/* default padding size*/)
		{
			if(demux->pDecryptBuf)
			{
				BKNI_Free(demux->pDecryptBuf);
				demux->pDecryptBuf = NULL;
			}

			demux->pDecryptBuf = (void*)BKNI_Malloc(uiEncryptedTotalSize + 128/* default padding size*/);
			if(demux->pDecryptBuf == NULL)
			{
				BDBG_ERR(("could not allocate\n", __FUNCTION__));
				return;
			}

			demux->nDecryptBufSize = uiEncryptedTotalSize + 128;
		}

		pBuf = demux->pDecryptBuf;

		/* arrange the data for decryption */
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			/* skip the clear data */
			batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].BytesOfClearData);

			/* read the encrypted data */
			bytes_ref = b_mp4_extract_encrypted_data((batom_cursor *)data, pSample->entries[jj].BytesOfEncryptedData, &pBuf[uiOffset]);
			if(bytes_ref != pSample->entries[jj].BytesOfEncryptedData)
			{
				 BDBG_ERR(("%s - out of DMA blks, return without decrypting 1\n", __FUNCTION__));
				 return;
			}
			uiOffset += bytes_ref;
		}

		/* decrypt only encrypted data by extracting from mp4 fragment */
		b_mp4_DoDecrypt_sw(demux, pBuf, uiEncryptedTotalSize, pSample->InitializationVector_128);

		batom_cursor_rollback(data, &start);

		batom_cursor_save(data, &start);

		uiOffset = 0;
		/* put decrypted data in mp4 fragment */
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			/* skip the clear data */
			batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].BytesOfClearData);

			/* copy decrypted data to mp4 cursor*/
			bytes_ref = b_mp4_put_decrypted_data((batom_cursor *)data, pSample->entries[jj].BytesOfEncryptedData, &pBuf[uiOffset]);
			if(bytes_ref != pSample->entries[jj].BytesOfEncryptedData)
			{
				 BDBG_ERR(("%s - out of DMA blks, return without decrypting 2\n", __FUNCTION__));
				 return;
			}
			uiOffset += bytes_ref;
		}
	}
	else
	{
		void* pBuf = NULL;

		/* reallocate memory for temporary buffer of decryption if there is not enough space */
		if(demux->nDecryptBufSize < uiSize + 128/* default padding size*/)
		{
			if(demux->pDecryptBuf)
			{
				BKNI_Free(demux->pDecryptBuf);
				demux->pDecryptBuf = NULL;
			}

			demux->pDecryptBuf = BKNI_Malloc(uiSize + 128/* default padding size*/);
			if(demux->pDecryptBuf == NULL)
			{
				BDBG_ERR(("could not allocate\n", __FUNCTION__));
				return;
			}
			demux->nDecryptBufSize = uiSize + 128;
		}
		pBuf = demux->pDecryptBuf;

		/* read the encrypted data */
		bytes_ref = b_mp4_extract_encrypted_data((batom_cursor *)data, uiSize, pBuf);
		if(bytes_ref != uiSize)
		{
			 BDBG_ERR(("%s - out of DMA blks, return without decrypting 3\n", __FUNCTION__));
			 return;
		}

		b_mp4_DoDecrypt_sw(demux, pBuf, uiSize, pSample->InitializationVector_128);

		batom_cursor_rollback(data, &start);

		batom_cursor_save(data, &start);

		/* copy decrypted data to mp4 cursor*/
		bytes_ref = b_mp4_put_decrypted_data((batom_cursor *)data, uiSize, pBuf);
		if(bytes_ref != uiSize)
		{
			 BDBG_ERR(("%s - out of DMA blks, return without decrypting 4\n", __FUNCTION__));
			 return;
		}
	}
#else

	BKNI_Memcpy(pEkl,   cipher_ctx->cipher.cipherAES.rgbContentKey+8, 8); /* Copy key.  H and L need to be swapped */
	BKNI_Memcpy(pEkl+8, cipher_ctx->cipher.cipherAES.rgbContentKey, 8);
	BKNI_Memcpy(&pEkl[16], pSample->InitializationVector_128, 16);
	BKNI_Memset(&pEkl[16], 0, 8);

    NEXUS_DmaJob_GetDefaultBlockSettings(&stBlkSettings[0]);
    stBlkSettings[0].pSrcAddr  = pEkl;
    stBlkSettings[0].pDestAddr = pEkl;
    stBlkSettings[0].blockSize = 32;
    stBlkSettings[0].resetCrypto              = true;
    stBlkSettings[0].scatterGatherCryptoStart = true;

    NEXUS_FlushCache(stBlkSettings[0].pSrcAddr, 32);

/*************************
*	3. dma setting
*************************/
	if(sample_encryption_box->fullbox.flags &= 0x000002)
	{
		for(jj = 0; jj < pSample->NumberOfEntries; jj++)
		{
			batom_cursor_skip((batom_cursor *)data, pSample->entries[jj].BytesOfClearData);
			bytes_ref = b_mp4_map_cursor_to_dmablks((batom_cursor *)data, pSample->entries[jj].BytesOfEncryptedData, &stBlkSettings[nb_Blks], 99, &nb_blks_used);
			if(bytes_ref != pSample->entries[jj].BytesOfEncryptedData)
			{
				 BDBG_ERR(("%s - out of DMA blks, return without decrypting\n", __FUNCTION__));
				 return;
			}
			nb_Blks += nb_blks_used;
		}
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}
	else
	{
		{
			bytes_ref = b_mp4_map_cursor_to_dmablks((batom_cursor *)data, uiSize, &stBlkSettings[1], 99, &nb_blks_used);
			if(bytes_ref != uiSize){
				 BDBG_ERR(("%s - out of DMA blks, return without decrypting\n", __FUNCTION__));
				 return;
			}
		}
		nb_Blks += nb_blks_used;
		stBlkSettings[nb_Blks-1].scatterGatherCryptoEnd = true;
	}

/*************************
*	4. decrypt
*************************/
	CommonCrypto_GetDefaultJobSettings(&jobSettings);
	jobSettings.keySlot = demux->keyPrdy;

	nResult = CommonCrypto_DmaXfer(demux->cryptoHandle, &jobSettings, stBlkSettings, nb_Blks);
	if(nResult != NEXUS_SUCCESS)
	{
		BDBG_ERR(("%s - CommonCrypto_DmaXfer failed\n", __FUNCTION__));
	}
#endif
	batom_cursor_rollback(data, &start);
}
#endif
#else
void
b_mp4_fragment_set_cb(bmp4_fragment_demux_t demux, void* pHSession, bmp4_prdy_decrypt_cb handler)
{
}
#endif

static void
b_mp4_fragment_box_init(b_mp4_fragment_box *box, uint32_t type)
{
    box->next = 0;
    box->type = type;
    box->start = -1;
    box->len = 0;
    return;
}

static void
b_mp4_fragment_box_set(b_mp4_fragment_box *box, unsigned start, size_t len, size_t header_len)
{
    BDBG_MSG_TRACE(("b_mp4_fragment_box_set:%#lx " B_MP4_TYPE_FORMAT " %lu:%u", (unsigned long)box, B_MP4_TYPE_ARG(box->type), (unsigned long)start, (unsigned)len));
    box->header_len = header_len;
    box->start = start;
    box->len = len;
    box->next = 0;
    return;
}

static int
b_mp4_fragment_box_peek(b_mp4_fragment_box *box, const batom_cursor *source, b_mp4_fragment_box *child, bool *end_of_data, bmp4_box *mp4_box, batom_cursor *destination)
{
    size_t box_size;
    size_t position;

    BDBG_ASSERT(box->start>=0);
    BATOM_CLONE(destination, source);
    position = box->start + box->next;
    if(position != batom_cursor_skip(destination, position)) {
        *end_of_data = true;
        return -1;
    }
    *end_of_data = false;
    box_size = bmp4_parse_box(destination, mp4_box);
    position = batom_cursor_pos(destination);
    if(box_size==0 || box_size>mp4_box->size) {
        if(position + BMP4_BOX_MAX_SIZE >= box->len) {
            *end_of_data = true;
        }
        return -1;
    }
    BDBG_MSG(("b_mp4_fragment_box_peek: %#lx:" B_MP4_TYPE_FORMAT " box: " B_MP4_TYPE_FORMAT ":%u (" B_MP4_TYPE_FORMAT ")", (unsigned long)box, B_MP4_TYPE_ARG(box->type), B_MP4_TYPE_ARG(mp4_box->type), (unsigned)mp4_box->size, B_MP4_TYPE_ARG(child->type)));
	if(mp4_box->type==child->type) {
        b_mp4_fragment_box_set(child, position, mp4_box->size-box_size, box_size);
    }
    return 0;
}

static int
b_mp4_fragment_box_next(b_mp4_fragment_box *box, const batom_cursor *source, b_mp4_fragment_box *child, bool *end_of_data, batom_cursor *destination)
{
    for(;;) {
        int rc;
        bmp4_box mp4_box;

        rc = b_mp4_fragment_box_peek(box, source, child, end_of_data, &mp4_box, destination);
        if(rc<0) {
            return rc;
        }
        box->next += mp4_box.size;
        if(mp4_box.type==child->type) {
            break;
        }
    }
    return 0;
}

typedef struct b_mp4_meta_atom_info {
    void *buffer;
    bmp4_fragment_demux_t demux;
} b_mp4_meta_atom_info;

static void
b_mp4_meta_atom_free(batom_t atom, void *user)
{
    const b_mp4_meta_atom_info *info= (b_mp4_meta_atom_info *)user;

    BSTD_UNUSED(atom);
	BDBG_MSG_TRACE(("b_mp4_meta_atom_free:%#lx %#lx", (unsigned long)atom, (unsigned long)user));
    BDBG_OBJECT_ASSERT(info->demux, bmp4_fragment_demux);
	BDBG_MSG(("b_mp4_meta_atom_free: demux:%#lx block:%#lx", (unsigned long)info->demux, (unsigned long)info->buffer));
    info->demux->config.alloc->bmem_free(info->demux->config.alloc, info->buffer);
	return;
}

static const batom_user b_mp4_meta_atom = {
	b_mp4_meta_atom_free,
	sizeof(b_mp4_meta_atom_info)
};



void
bmp4_fragment_demux_default_cfg(bmp4_fragment_demux_cfg *cfg)
{
    BKNI_Memset(cfg, 0, sizeof(*cfg));
    cfg->burst_frames = 4;
    return;
}

static void
b_mp4_fragment_track_init(b_mp4_fragment_track *track)
{
    BKNI_Memset(track, 0, sizeof(*track));
    b_mp4_fragment_box_init(&track->movie_fragment, BMP4_MOVIE_FRAGMENT);
    b_mp4_fragment_box_init(&track->movie_fragment_data, BMP4_MOVIE_DATA);
    b_mp4_fragment_box_init(&track->track_fragment, BMP4_TRACK_FRAGMENT);
    b_mp4_fragment_box_init(&track->track_fragment_header_box, BMP4_TRACK_FRAGMENT_HEADER);
    b_mp4_fragment_box_init(&track->track_fragment_run, BMP4_TRACK_FRAGMENT_RUN);
    b_mp4_fragment_box_init(&track->root, B_MP4_FRAGMENT_WRAPPER);

    return;
}

static int
b_mp4_fragment_demux_allocate_h264(bmp4_fragment_demux_t demux)
{
    demux->h264.nal_prefix = demux->config.alloc->bmem_alloc(demux->config.alloc, bmedia_nal_vec.len);
    if(!demux->h264.nal_prefix) { return -1; }
    BKNI_Memcpy(demux->h264.nal_prefix, bmedia_nal_vec.base, bmedia_nal_vec.len);
    BATOM_VEC_INIT(&demux->h264.nal_vec, demux->h264.nal_prefix, bmedia_nal_vec.len);
    return 0;
}

static void
b_mp4_fragment_demux_free_h264(bmp4_fragment_demux_t demux)
{
    demux->config.alloc->bmem_free(demux->config.alloc, demux->h264.nal_prefix);
    return;
}


bmp4_fragment_demux_t
bmp4_fragment_demux_create(const bmp4_fragment_demux_cfg *config)
{
    bmp4_fragment_demux_t demux;
    int rc;

    BDBG_ASSERT(config);
    if(config->factory==NULL || config->alloc==NULL) { (void)BERR_TRACE(BERR_NOT_SUPPORTED); goto err_config; }
    demux = BKNI_Malloc(sizeof(*demux));
    if(demux==NULL) { (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BDBG_OBJECT_INIT(demux, bmp4_fragment_demux);
    demux->config = *config;
    demux->offset = 0;
    demux->demux_state = b_mp4_fragment_demux_state_box;
    demux->next_fragment_track_ID = 0;
    BLST_S_INIT(&demux->streams);

    demux->accum = batom_accum_create(config->factory);
    if(!demux->accum) { (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_create_accum;}

    rc = b_mp4_fragment_demux_allocate_h264(demux);
    if(rc!=0) {(void)BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);goto err_h264;}

#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	demux->wrmheader = NULL;
	demux->pEkl = NULL;
#if defined(CONFIG_PLAYREADY_25)
	demux->bSetContext = FALSE;
	demux->pAppContext = NULL;
	demux->f_pOEMContext = NULL;
	demux->pDecryptBuf = NULL;
	demux->nDecryptBufSize = 0;
#endif
#endif
#endif
    return demux;
err_h264:
    batom_accum_destroy(demux->accum);
err_create_accum:
    BKNI_Free(demux);
err_alloc:
err_config:
    return NULL;
}

void
bmp4_fragment_demux_destroy(bmp4_fragment_demux_t demux)
{
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    b_mp4_fragment_demux_free_h264(demux);
    batom_accum_destroy(demux->accum);
    BDBG_OBJECT_DESTROY(demux, bmp4_fragment_demux);
    BKNI_Free(demux);
    return;
}

void
bmp4_fragment_demux_reset(bmp4_fragment_demux_t demux)
{
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    bmp4_fragment_demux_flush(demux);
    demux->demux_state = b_mp4_fragment_demux_state_box;
    demux->offset = 0;
    return;
}

static int
b_mp4_fragment_track_next(bmp4_fragment_demux_t demux, b_mp4_fragment_track *track, const batom_cursor *source, bool *end_of_fragment)
{
    int rc=0;
    bool end_of_data;

    for(;;) {
        if(track->fragment_header.starttime == (uint64_t)((int64_t)(-1))) {
            if(track->track_eos) {
                track->track_eos = false;
                *end_of_fragment = true;
                rc = -1;
            } else {
                track->track_eos = true;
            }
            break;
            /* end of stream */
        } else if(!track->movie_fragment_valid) {
            bmp4_movie_fragment_header movie_fragment_header;
            b_mp4_fragment_box movie_fragment_header_box;
            bmp4_box mdat;

            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx reading movie_fragment", (unsigned long)track));
            rc = b_mp4_fragment_box_next(&track->root, source, &track->movie_fragment, &end_of_data, &track->track_cursor);
            if(rc<0) {
                if(end_of_data) {
                    *end_of_fragment = true;
                    break;
                } else {
                    break;
                }
            }

            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx reading movie_fragment_header", (unsigned long)track));
            b_mp4_fragment_box_init(&movie_fragment_header_box, BMP4_MOVIE_FRAGMENT_HEADER);
            rc = b_mp4_fragment_box_next(&track->movie_fragment, source, &movie_fragment_header_box, &end_of_data, &track->track_cursor);
            if(rc<0) {
                break;
            }
            if(!bmp4_parse_movie_fragment_header(&track->track_cursor, &movie_fragment_header)) {
                return -1;
            }
            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx reading movie_fragment_data", (unsigned long)track));
            rc = b_mp4_fragment_box_peek(&track->root, source, &track->movie_fragment_data, &end_of_data, &mdat, &track->track_cursor);
            if(rc==0 && mdat.type == track->movie_fragment_data.type) {
                BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx movie_fragment_data at:%u", (unsigned long)track, (unsigned)track->movie_fragment_data.start));
                track->movie_fragment_data_valid = true;
            } else {
                track->movie_fragment_data_valid = false;
            }
            track->state.next.run_offset = track->state.current.run_offset = 0;
            track->movie_fragment_valid = true;
        }
        if(!track->fragment_header_valid) {

            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx reading track_fragment", (unsigned long)track));
            rc = b_mp4_fragment_box_next(&track->movie_fragment, source, &track->track_fragment, &end_of_data, &track->track_cursor);
            if(rc<0) {
                if(end_of_data) {
                    track->movie_fragment_valid = false; /* look for the next fragment */
                    continue;
                } else {
                    break;
                }
            }
            if(demux->config.traf_box) {
                demux->config.traf_box(demux->config.user_context, source, track->track_fragment.len);
            }
            rc = b_mp4_fragment_box_next(&track->track_fragment, source, &track->track_fragment_header_box, &end_of_data, &track->track_cursor);
            if(rc!=0) {
                return -1;
            }
            if(!bmp4_parse_track_fragment_header(&track->track_cursor, &track->track_fragment_header)) {
                return -1;
            }
#if 0
            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx found track %u:%u", (unsigned long)track, track->track_fragment_header.track_ID, track->track_extends.track_ID));
            if(track->track_fragment_header.track_ID != track->track_extends.track_ID) {
                continue; /* advance to the next fragment */
            }
#endif
            track->fragment_header_valid = true;
        }
        if(!track->fragment_run_valid) {
            BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx reading track_fragment_run", (unsigned long)track));
            rc = b_mp4_fragment_box_next(&track->track_fragment, source, &track->track_fragment_run, &end_of_data, &track->track_cursor);
            if(rc<0) {
                if(end_of_data) {
                    track->fragment_header_valid = false;
                    continue;
                } else {
                    break;
                }
            }
            if(!bmp4_parse_track_fragment_run_header(&track->track_cursor, &track->run_header)) {
                return -1;
            }
#if defined(HUMAX_PLATFORM_BASE)
            if(track->run_header.validate.data_offset && track->movie_fragment_data_valid )
            {
            	if(track->run_header.data_offset != ((unsigned)track->movie_fragment_data.start - (unsigned)(track->movie_fragment.start-track->movie_fragment.header_len)))
            	{
            		BDBG_WRN(("[Warning] Data Offset Diff (%u : %u)",(unsigned)track->run_header.data_offset,(unsigned)((unsigned)track->movie_fragment_data.start - (unsigned)(track->movie_fragment.start-track->movie_fragment.header_len))));
					track->run_header.data_offset = ((unsigned)track->movie_fragment_data.start - (unsigned)(track->movie_fragment.start-track->movie_fragment.header_len));
            	}
            }
#endif
            bmp4_init_track_fragment_run_state(&track->run_state);
            track->fragment_run_valid = true;
        }
        if(track->run_state.sample_no >= track->run_header.sample_count) {
            track->state.current = track->state.next;
            track->fragment_run_valid = false;
            continue;
        }

        track->next_sample_no = track->run_state.sample_no;
        if(!bmp4_parse_track_fragment_run_sample(&track->track_cursor, &track->track_fragment_header, &track->run_header, &track->track_extends, &track->run_state, &track->next_sample)) {
            track->fragment_run_valid = false;
            continue;
        }
        BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx track_ID:%u sample.offset:%u %s", (unsigned long)track, track->track_extends.track_ID, (unsigned)track->next_sample.offset, track->run_header.validate.data_offset?"run_header.data_offset":""));
        if(track->run_header.validate.data_offset) {
            track->next_sample.offset += track->run_header.data_offset;
        } else {
            track->next_sample.offset += track->state.current.run_offset;
        }
        track->state.next.run_offset = track->next_sample.offset + track->next_sample.size;
        BDBG_MSG_TRACE(("b_mp4_fragment_track_next:%#lx track_ID:%u run.offset:%u %s %s %u %u", (unsigned long)track, track->track_extends.track_ID, (unsigned)track->next_sample.offset, track->track_fragment_header.validate.base_data_offset?"track_fragment_header.base_data_offset":"", track->movie_fragment_data_valid?"MDAT.offset":"", (unsigned)track->movie_fragment_data.start, (unsigned)(track->movie_fragment.start-track->movie_fragment.header_len)));
        if(track->track_fragment_header.validate.base_data_offset) {
            /* it appears that in the case of individual fragment offset is relative to  location of the base_data_offset field inside the track fragment header */
            track->next_sample.offset += track->track_fragment_header.base_data_offset + track->track_fragment_header_box.start + 4 /* flags */ + 4 /* track_ID */;
        } else {
            if(track->movie_fragment_data_valid && !track->run_header.validate.data_offset)  {
                track->next_sample.offset += track->movie_fragment_data.start;
            } else {
                track->next_sample.offset += track->movie_fragment.start - track->movie_fragment.header_len;
            }
        }
        track->next_sample.time += track->state.current.run_start_time;
        track->state.next.run_start_time = track->next_sample.time + track->next_sample.duration;
#if defined(HUMAX_PLATFORM_BASE)
		/*****************/
		/* PIFF by ykkim */
		/*****************/
		if(!track->stdp_valid) {
			bmp4_box temp;
			b_mp4_fragment_box_init(&track->stdp, BMP4_INDPENDENT_DISPOSABLE_SAMPLE);
			rc = b_mp4_fragment_box_peek(&track->track_fragment, source, &track->stdp, &end_of_data, &temp, &track->stdp_cursor);
			if(rc == 0 && temp.type == track->stdp.type)
			{
				track->stdp.next = track->stdp.len;
			}
			else if (rc < 0)
			{
				track->uuid_parsed = true;
			}
			else
			{
				b_mp4_fragment_box_set(&track->stdp,track->track_fragment.start,track->track_fragment.len,track->track_fragment.header_len);
				track->stdp.next = track->track_fragment.next;
				BDBG_MSG(("didn't detect stdp box"));
			}
			track->stdp_valid = true;
			/* skipping stdp */
		}

		if( !track->senc_parsed )
		{
			bmp4_box temp;
			b_mp4_fragment_box senc;
			bmp4_fullbox fullbox;

			b_mp4_fragment_box_init(&senc, BMP4_SENCBOX);
			rc = b_mp4_fragment_box_next(&track->track_fragment, source, &senc, &end_of_data, &track->encryption_cursor);
			if(rc == 0)
			{
				BDBG_MSG(("Detect SENC Box"));
				/*bmp4_parse_fullbox(&track->track_cursor, &fullbox);*/
				if(bmp4_parse_ext_sebox(&track->encryption_cursor, &track->sample_encryption_box))
				{
					track->sample_encryption_valid = true;
				}
				else
				{
					track->sample_encryption_valid = false;
				}
			}
			track->senc_parsed = true;
		}

		if( !track->uuid_parsed )
		{
			unsigned i = 0;
			bmp4_box temp;
			b_mp4_fragment_box extendedBox[BMP4_UUID_EXTENDEDTYPE_MAX];
			batom_cursor extendedCursor;
			bmp4_uuid_extendedtype eType = BMP4_UUID_EXTENDEDTYPE_MAX;
			for(i = 0 ; i < BMP4_UUID_EXTENDEDTYPE_MAX ; i++)
			{
				b_mp4_fragment_box_init(&extendedBox[i], BMP4_EXTENDED);
				if(i == 0)
				{
					rc = b_mp4_fragment_box_peek(&track->stdp, source, &extendedBox[i], &end_of_data, &temp, &extendedCursor);
				}
				else
				{
					rc = b_mp4_fragment_box_peek(&extendedBox[i-1], source, &extendedBox[i], &end_of_data, &temp, &extendedCursor);
				}
				if(rc == 0 && extendedBox[i].type == temp.type)
				{
					extendedBox[i].next = extendedBox[i].len;
					eType = bmp4_find_ext_extendedtype(&extendedCursor);
					switch(eType)
					{
						case BMP4_UUID_EXTENDEDTYPE_SAMPLEENCRYPTIONBOX :
							BDBG_MSG(("Detect Sample Encryption Box"));
							BATOM_CLONE(&track->encryption_cursor,&extendedCursor);

							if(bmp4_parse_ext_sebox(&track->encryption_cursor, &track->sample_encryption_box))
							{
								track->sample_encryption_valid = true;
							}
							else
							{
								BDBG_ERR(("SAMPLE Encrytion Box is not Valid!!"));
								track->sample_encryption_valid = false;
							}
							break;
						case BMP4_UUID_EXTENDEDTYPE_TFXDBOX :
							BDBG_MSG(("Detect TFXD Box"));
							break;
						case BMP4_UUID_EXTENDEDTYPE_TFRFBOX :
							BDBG_MSG(("Detect TFRF Box"));
							break;
						default:
							break;
					}
				}
				else
				{
					break;
				}
			}
			track->uuid_parsed = true;
		}
		/* freesat g2 pay VOD -easelTV use senc box instead of  user extended box for encryption data*/
		/* senc box parse */
#endif
        break;
    }

    BDBG_MSG(("b_mp4_fragment_track_next:%#lx track_ID:%u sample offset:%u time:%u duration:%u size:%u flags:%#x composition_time_offset:%d %s", (unsigned long)track, (unsigned)track->track_extends.track_ID, (unsigned)track->next_sample.offset, (unsigned)track->next_sample.time, track->next_sample.duration, track->next_sample.size, track->next_sample.flags, (int)track->next_sample.composition_time_offset, *end_of_fragment?"EOF":""));

    return rc;
}

static bool
b_mp4_parse_fragment_header(batom_cursor *cursor, b_mp4_fragment_header *header)
{
    header->timescale = batom_cursor_uint64_be(cursor);
    header->starttime = batom_cursor_uint64_be(cursor);
    header->fourcc = batom_cursor_uint32_le(cursor);
    return !BATOM_IS_EOF(cursor);
}

static int
b_mp4_fragment_track_start(b_mp4_fragment_track *track, batom_accum_t accum, size_t header_len, size_t box_len, const batom_cursor *source)
{
    b_mp4_fragment_box box;
    batom_cursor cursor;
    int rc;
    bool end_of_data;
    batom_t track_extends;
    batom_cursor end_of_box;

    BDBG_ASSERT(header_len<box_len);
    b_mp4_fragment_box_set(&track->root, header_len, box_len-header_len, header_len);
    b_mp4_fragment_box_init(&box, B_MP4_FRAGMENT_HEADER);
    rc = b_mp4_fragment_box_next(&track->root, source, &box, &end_of_data, &cursor);
    if(rc<0) { return rc; }
    if(!b_mp4_parse_fragment_header(&cursor, &track->fragment_header)) { return -1; }
    b_mp4_fragment_box_init(&box, B_MP4_FRAGMENT_DATA);
    rc = b_mp4_fragment_box_next(&track->root, source, &box, &end_of_data, &cursor);
    if(rc<0) { return rc; }
    BATOM_CLONE(&end_of_box, &cursor);
    batom_cursor_skip(&end_of_box, box.len);
    track->codec_private.data = batom_accum_extract(accum, &cursor, &end_of_box, NULL, NULL);
    if(!track->codec_private.data) {return -1; }
    b_mp4_fragment_box_init(&box, BMP4_TRACK_EXTENDS);
    rc = b_mp4_fragment_box_next(&track->root, source, &box, &end_of_data, &cursor);
    if(rc<0) { return rc; }
    BATOM_CLONE(&end_of_box, &cursor);
    batom_cursor_skip(&end_of_box, box.len);
    track_extends = batom_accum_extract(accum, &cursor, &end_of_box, NULL, NULL);
    if(!track_extends) {return -1;}
    end_of_data = !bmp4_parse_trackextends(track_extends, &track->track_extends);
    batom_release(track_extends);
    if(end_of_data) { return -1;}

    track->next_sample_valid = false;
    track->fragment_run_valid = false;
    track->fragment_header_valid = false;
    track->movie_fragment_valid = false;
    track->track_eos = false;
    track->movie_fragment_data_valid = false;
#if defined(HUMAX_PLATFORM_BASE)
	/****************/
	/* PIFF by ykkim*/
	/****************/
	track->sample_encryption_valid = false;
	track->sample_encryption_check = false;
	track->uuid_parsed = false;
	track->stdp_valid = false;
	track->senc_parsed = false;
#endif
    track->state.current.run_offset = 0;
    track->state.current.run_start_time = track->fragment_header.starttime;
    track->state.next.run_offset = 0;
    track->state.next.run_start_time = track->fragment_header.starttime;

    return 0;
}

static void
b_mp4_fragment_track_clear(b_mp4_fragment_track *track)
{
    if(track->codec_private.data) {
        batom_release(track->codec_private.data);
        track->codec_private.data=NULL;
    }
    return;
}

static void
b_mp4_fragment_demux_report_error(bmp4_fragment_demux_t demux)
{
    if(demux->config.error) {
        demux->config.error(demux->config.user_context);
    }
    return;
}

static int
b_mp4_fragment_stream_start(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, const batom_cursor *cursor, batom_accum_t accum, size_t box_len)
{
    batom_cursor end_of_box;
    batom_t data;

    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    BDBG_OBJECT_ASSERT(stream, bmp4_fragment_stream);

    BATOM_CLONE(&end_of_box, cursor);
    batom_cursor_skip(&end_of_box, box_len);
    data = batom_accum_extract(accum, cursor, &end_of_box, NULL, NULL);
    if(!data) { return -1; }
    batom_accum_clear(stream->data);
    batom_accum_add_atom(stream->data, data);
    batom_release(data);
    stream->busy = true;
    return 0;
}

#if defined(HUMAX_PLATFORM_BASE)
uint32_t
convert_vword_be(uint8_t *c_cursor, unsigned v)
{
	uint32_t d;
	int c_left;
	for(d=0;v>0;) {
			d = (d<<8) | ((uint32_t)c_cursor[0]);
			c_left--;
			c_cursor++;
			v--;
	}
	return d;
}
#endif

static int
b_mp4_payload_handler_h264(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, unsigned frame_no, uint32_t timestamp, batom_cursor *payload, size_t payload_len)
{
    int rc=0;
    size_t left;
    unsigned count;
	bmedia_packet_header hdr;
    batom_t pes_atom;

    BDBG_MSG_TRACE(("b_mp4_payload_handler_h264:%#lx:%#lx:%u payload %u", (unsigned long)demux, (unsigned long)stream, frame_no, (unsigned)payload_len));

    batom_accum_clear(stream->frame_accum);

	BMEDIA_PACKET_HEADER_INIT(&hdr);
    BMEDIA_PES_SET_PTS(&hdr,bmedia_time2pts(timestamp, BMEDIA_TIME_SCALE_BASE));

    if(frame_no==0) { /* need to allocate data rai and append for pps,sps and  rai */
        stream->vecs[B_MP4_STREAM_VEC_FRAME] = &bmedia_nal_vec;
        stream->vecs[B_MP4_STREAM_VEC_RAI] = &bmedia_rai_h264;
        stream->vecs[B_MP4_STREAM_VEC_EOS] = &bmedia_eos_h264;

        hdr.header_type = B_MP4_STREAM_VEC_RAI;
        if(stream->track.codec_private.data) {
            if(stream->track.fragment_header.fourcc == BMEDIA_FOURCC('a','v','c','1')) {
                void *h264_meta;
                size_t meta_length = batom_len(stream->track.codec_private.data);
                batom_cursor cursor;
                b_mp4_meta_atom_info meta_info;
                bmedia_h264_meta meta;
                batom_t meta_marker;

                h264_meta = demux->config.alloc->bmem_alloc(demux->config.alloc, meta_length);
                if(h264_meta==NULL) { (void)BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);goto error; }
                BDBG_MSG(("b_mp4_payload_handler_h264:%#lx:%#lx allocated h264_meta:%#lx", (unsigned long)demux, (unsigned long)stream, (unsigned long)h264_meta));
                batom_cursor_from_atom(&cursor, stream->track.codec_private.data);
                batom_cursor_copy(&cursor, h264_meta, meta_length);
                if(!bmedia_read_h264_meta(&meta, h264_meta, meta_length)) { BDBG_ERR(("b_mp4_payload_handler_h264:%#lx:%#lx invalid avc1 meta data", (unsigned long)demux, (unsigned long)stream));}
                bmedia_copy_h264_meta_with_nal_vec(stream->frame_accum, &meta, &demux->h264.nal_vec);
                meta_info.buffer = h264_meta;
                meta_info.demux = demux;
                meta_marker = batom_from_range(demux->config.factory,demux->h264.nal_prefix, 1, &b_mp4_meta_atom, &meta_info); /* construct a vector that just carries 0 and marker so memory would get freed unsed metadata delivered */
                if(meta_marker==NULL) { (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto error;}
                batom_accum_add_atom(stream->frame_accum, meta_marker);
                batom_release(meta_marker);
            } else /* fourcc==BMEDIA_FOURCC('H','2','6','4')  */ {
                batom_accum_add_atom(stream->frame_accum, stream->track.codec_private.data);
            }
        }
        hdr.key_frame = true;
    }

    for(count=0,left=payload_len;left>0;count++) {
        batom_cursor nal_start;
        const size_t lengthSize = 4;
        uint32_t nal_len;
        if(left<=lengthSize) {
            BDBG_MSG_TRACE(("b_mp4_payload_handler_h264:%#lx:%#lx invalid frame, left %u", (unsigned long)demux, (unsigned long)stream, (unsigned)left));
            rc = -1;
            goto error;
        }
        left -= lengthSize;
        nal_len = batom_cursor_vword_be(payload, lengthSize);
        /*  BDBG_MSG(("nal_len:%u", nal_len)); */
        BATOM_CLONE(&nal_start, payload);
        if(nal_len==0 || nal_len > left || nal_len!=batom_cursor_skip(payload, nal_len)) {
            BDBG_MSG_TRACE(("b_mp4_payload_handler_h264:%#lx:%#lx not enough frame data %u:%u", (unsigned long)demux, (unsigned long)stream, (unsigned)nal_len, (unsigned)left));
            rc = -1;
            goto error;
        }
        if(count==0 && frame_no!=0) {
            hdr.header_type = B_MP4_STREAM_VEC_FRAME;
        } else {
            batom_accum_add_range(stream->frame_accum, demux->h264.nal_prefix, bmedia_nal_vec.len);
        }
        batom_accum_append(stream->frame_accum, stream->data, &nal_start, payload);
        left -= nal_len;
    }
    if(frame_no==0) { /* could free codec_private.data */
        if(stream->track.codec_private.data) {
            batom_release(stream->track.codec_private.data);
            stream->track.codec_private.data=NULL;
        }
    }
	pes_atom = batom_from_accum(stream->frame_accum, bmedia_atom, &hdr);
    if(pes_atom) {
        batom_pipe_push(stream->pipe_out, pes_atom);
    }
    return 0;

error:
    batom_accum_clear(stream->frame_accum);
    return -1;
}



static int
b_mp4_payload_handler_aac(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, unsigned frame_no, uint32_t timestamp, batom_cursor *payload, size_t payload_len)
{
    batom_cursor cursor;
    bmedia_adts_hdr hdr;
    batom_t pes_atom;

    BDBG_MSG_TRACE(("b_mp4_payload_handler_aac:%#lx:%#lx:%u payload %u", (unsigned long)demux, (unsigned long)stream, frame_no, (unsigned)payload_len));
    BSTD_UNUSED(demux);

    if(frame_no==0) {
        bmedia_info_aac aac_info;

        batom_cursor_from_atom(&cursor, stream->track.codec_private.data);

        if(!bmedia_info_probe_aac_info(&cursor, &aac_info)) {
            return -1;
        }

        bmedia_adts_header_init_aac(&stream->aac.adts_header,&aac_info);
    }
	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
    BMEDIA_PES_SET_PTS(&hdr.pes, bmedia_time2pts(timestamp, BMEDIA_TIME_SCALE_BASE));
    bmedia_adts_hdr_init(&hdr, &stream->aac.adts_header, payload_len);
    BATOM_CLONE(&cursor, payload);
    batom_cursor_skip(&cursor, payload_len);
    pes_atom = batom_accum_extract(stream->data, payload, &cursor, &bmedia_adts_atom, &hdr);

    if(pes_atom) {
        batom_pipe_push(stream->pipe_out, pes_atom);
    }
    return 0;
}

#if 0
typedef int16_t b_mp4_short;
typedef uint32_t b_mp4_dword;
typedef uint64_t b_mp4_reference_time;

typedef struct b_mp4_rect {
    b_mp4_short left;
    b_mp4_short top;
    b_mp4_short right;
    b_mp4_short bottom;
} b_mp4_rect;

typedef struct b_mp4_videoinfoheader {
    b_mp4_rect source;
    b_mp4_rect target;
    b_mp4_dword bitrate;
    b_mp4_dword bitErrorRate;
    b_mp4_reference_time avgTimePerFrame;
    bmedia_bitmapinfo header;
} b_mp4_videoinfoheader;

static bool
b_mp4_read_rect(b_mp4_rect *rect, batom_cursor *c)
{
    rect->left = batom_cursor_uint16_le(c);
    rect->top = batom_cursor_uint16_le(c);
    rect->right = batom_cursor_uint16_le(c);
    rect->bottom = batom_cursor_uint16_le(c);
    if(!BATOM_IS_EOF(c)) {
        BDBG_MSG(("b_mp4_read_rect: %u,%u,%u,%u", rect->left, rect->top, rect->right, rect->bottom));
    }
    return !BATOM_IS_EOF(c);
}

static bool
b_mp4_read_videoinfoheader(b_mp4_videoinfoheader *vf, batom_cursor *c)
{
    if(!b_mp4_read_rect(&vf->source, c)) {
        return false;
    }
    if(!b_mp4_read_rect(&vf->target, c)) {
        return false;
    }
    vf->bitrate = batom_cursor_uint32_le(c);
    vf->bitErrorRate = batom_cursor_uint32_le(c);
    vf->avgTimePerFrame = batom_cursor_uint64_le(c);
    if(BATOM_IS_EOF(c)) {
        return false;
    }
    return bmedia_read_bitmapinfo(&vf->header, c);
}
#endif

static int
b_mp4_payload_handler_generic(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, unsigned frame_no, uint32_t timestamp, batom_cursor *payload, size_t payload_len)
{
    batom_cursor cursor;
	bmedia_packet_header hdr;
    batom_t pes_atom;

    BDBG_MSG_TRACE(("b_mp4_payload_handler_generic:%#lx:%#lx:%u payload %u", (unsigned long)demux, (unsigned long)stream, frame_no, (unsigned)payload_len));
    BSTD_UNUSED(demux);

	BMEDIA_PACKET_HEADER_INIT(&hdr);
    BMEDIA_PES_SET_PTS(&hdr, bmedia_time2pts(timestamp, BMEDIA_TIME_SCALE_BASE));
    hdr.key_frame = frame_no==0;
    BATOM_CLONE(&cursor, payload);
    batom_cursor_skip(&cursor, payload_len);
    pes_atom = batom_accum_extract(stream->data, payload, &cursor, bmedia_atom, &hdr);

    if(pes_atom) {
        batom_pipe_push(stream->pipe_out, pes_atom);
    }
    return 0;
}

static int
b_mp4_payload_handler_wma_pro(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, unsigned frame_no, uint32_t timestamp, batom_cursor *payload, size_t payload_len)
{
    batom_cursor cursor;
    bmedia_bcma_hdr hdr;
    batom_t pes_atom;

    BDBG_MSG_TRACE(("b_mp4_payload_handler_wma_pro:%#lx:%#lx:%u payload %u", (unsigned long)demux, (unsigned long)stream, frame_no, (unsigned)payload_len));
    BSTD_UNUSED(demux);

    batom_accum_clear(stream->frame_accum);

    if(frame_no==0) {
        batom_cursor_from_atom(&cursor, stream->track.codec_private.data);
        stream->vecs[B_MP4_STREAM_VEC_FRAME] = &bmedia_frame_bcma;
    }
	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
    BMEDIA_PES_SET_PTS(&hdr.pes, bmedia_time2pts(timestamp, BMEDIA_TIME_SCALE_BASE));
	hdr.pes.header_type = B_MP4_STREAM_VEC_FRAME;
	hdr.pes.header_off = 4;
    hdr.pes.key_frame = true;
    bmedia_bcma_hdr_init(&hdr, payload_len);
    batom_accum_add_atom(stream->frame_accum, stream->track.codec_private.data);
    BATOM_CLONE(&cursor, payload);
    batom_cursor_skip(&cursor, payload_len);
    batom_accum_append(stream->frame_accum, stream->data, payload, &cursor);
    pes_atom = batom_from_accum(stream->frame_accum, &bmedia_bcma_atom, &hdr);

    if(pes_atom) {
        batom_pipe_push(stream->pipe_out, pes_atom);
    }
    return 0;
}

static int
b_mp4_fragment_stream_track_eos(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream)
{
	bmedia_packet_header hdr;
    batom_t pes_atom;
    uint32_t fourcc = stream->track.fragment_header.fourcc;

    BDBG_MSG_TRACE(("b_mp4_fragment_stream_track_eos:%#lx:%#lx", (unsigned long)demux, (unsigned long)stream));
    if(stream->track.payload_handler == b_mp4_payload_handler_h264) {
        stream->vecs[B_MP4_STREAM_VEC_EOS] = &bmedia_eos_h264;
    } else if( fourcc==BMEDIA_FOURCC('W','V','C','1')) {
        stream->vecs[B_MP4_STREAM_VEC_EOS] = &bmedia_eos_vc1;
    } else {
        stream->vecs[B_MP4_STREAM_VEC_EOS] = &bmedia_null_vec;
    }
	BMEDIA_PACKET_HEADER_INIT(&hdr);
    hdr.header_type = B_MP4_STREAM_VEC_EOS | B_MEDIA_PACKET_FLAG_EOS;
    pes_atom = batom_empty(demux->config.factory, bmedia_atom, &hdr);
    if(pes_atom) {
        batom_pipe_push(stream->pipe_out, pes_atom);
    }
    return 0;
}
static int
b_mp4_fragment_stream_process_frame(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream, uint32_t timestamp)
{
    int rc;
    batom_cursor frame;

    BSTD_UNUSED(demux);
    batom_cursor_from_accum(&frame, stream->data);
    if(stream->track.track_eos) {
        return b_mp4_fragment_stream_track_eos(demux, stream);
    }
    if(stream->track.next_sample.offset != batom_cursor_skip(&frame, stream->track.next_sample.offset)) {
        BDBG_MSG_TRACE(("b_mp4_fragment_stream_next:%#lx:%#lx frame out of bounds %u", (unsigned long)demux, (unsigned long)stream, (unsigned)stream->track.next_sample.offset));
        rc = -1;
        goto error;
    }
    if(stream->track.next_sample.size != batom_cursor_reserve(&frame, stream->track.next_sample.size)) {
        BDBG_MSG_TRACE(("b_mp4_fragment_stream_next:%#lx:%#lx not enough data %u", (unsigned long)demux, (unsigned long)stream, (unsigned)stream->track.next_sample.size));
        rc = -1;
        goto error;
    }
    /* batom_cursor_dump(&frame, "frame"); */
#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	if(stream->track.sample_encryption_valid)
	{
		b_mp4_DoDecryptSample(demux,
			&stream->track.sample_encryption_box,
			stream->track.next_sample_no,
			&frame,
			stream->track.next_sample.size,
			stream->track.sample_encryption_box.samples[stream->track.next_sample_no].InitializationVector_128);
	}
#endif
#else
    if(demux->config.sample) {
        demux->config.sample(demux->config.user_context, stream->track.track_extends.track_ID, stream->track.next_sample_no, &frame, stream->track.next_sample.size);
    }
#endif
    rc = stream->track.payload_handler(demux, stream, stream->track.next_sample_no, timestamp, &frame, stream->track.next_sample.size);
    /* rc = -1; */
error:
    return rc;
}

static void
b_mp4_fragment_stream_flush(bmp4_fragment_stream_t stream)
{
    batom_accum_clear(stream->frame_accum);
    batom_accum_clear(stream->data);
    stream->busy = false;
    b_mp4_fragment_track_clear(&stream->track);
    return;
}

int
b_mp4_fragment_set_handler(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream)
{
    int rc=0;
    uint32_t fourcc = stream->track.fragment_header.fourcc;
#if defined(HUMAX_PLATFORM_BASE)
	if(fourcc==BMEDIA_FOURCC('A','V','C','1') || fourcc==BMEDIA_FOURCC('H','2','6','4')
	|| fourcc==BMEDIA_FOURCC('a','v','c','1') || fourcc==BMEDIA_FOURCC('h','2','6','4') ) {
		stream->track.payload_handler = b_mp4_payload_handler_h264;
	} else if(fourcc==BMEDIA_FOURCC('M','P','4','A') || fourcc==BMEDIA_FOURCC('A','A','C','L')
	|| fourcc==BMEDIA_FOURCC('m','p','4','a') || fourcc==BMEDIA_FOURCC('a','a','c','l')) {
		stream->track.payload_handler = b_mp4_payload_handler_aac;
	} else if(fourcc==BMEDIA_FOURCC('W','V','C','1') || fourcc==BMEDIA_FOURCC('w','v','c','1')) {
		stream->track.payload_handler = b_mp4_payload_handler_generic;
	} else if(fourcc==BMEDIA_FOURCC('W','M','A','P') || fourcc==BMEDIA_FOURCC('w','m','a','p')) {
		stream->track.payload_handler = b_mp4_payload_handler_wma_pro;
	} else if(fourcc==BMEDIA_FOURCC('A','C','-','3') ||  fourcc==BMEDIA_FOURCC('E','C','-','3')
	|| fourcc==BMEDIA_FOURCC('a','c','-','3') ||  fourcc==BMEDIA_FOURCC('e','c','-','3')) {
		stream->track.payload_handler = b_mp4_payload_handler_generic;
	} else {
		BDBG_WRN(("b_mp4_fragment_set_handler:%#lx %#lx  unknown codec " BMEDIA_FOURCC_FORMAT " for track %u", (unsigned long)demux, (unsigned long)stream, BMEDIA_FOURCC_ARG(fourcc), stream->stream_id));
		rc=-1;
	}
#else
    if(fourcc==BMEDIA_FOURCC('a','v','c','1') || fourcc==BMEDIA_FOURCC('H','2','6','4') ) {
        stream->track.payload_handler = b_mp4_payload_handler_h264;
    } else if(fourcc==BMEDIA_FOURCC('m','p','4','a') || fourcc==BMEDIA_FOURCC('A','A','C','L')) {
        stream->track.payload_handler = b_mp4_payload_handler_aac;
    } else if(fourcc==BMEDIA_FOURCC('W','V','C','1')) {
        stream->track.payload_handler = b_mp4_payload_handler_generic;
    } else if(fourcc==BMEDIA_FOURCC('W','M','A','P')) {
        stream->track.payload_handler = b_mp4_payload_handler_wma_pro;
    } else if(fourcc==BMEDIA_FOURCC('a','c','-','3') ||  fourcc==BMEDIA_FOURCC('e','c','-','3')) {
        stream->track.payload_handler = b_mp4_payload_handler_generic;
    } else {
        BDBG_WRN(("b_mp4_fragment_set_handler:%#lx %#lx  unknown codec " BMEDIA_FOURCC_FORMAT " for track %u", (unsigned long)demux, (unsigned long)stream, BMEDIA_FOURCC_ARG(fourcc), stream->stream_id));
        rc=-1;
    }
#endif
    return rc;
}

static void
b_mp4_fragment_demux_parse_advance(bmp4_fragment_demux_t demux, size_t to_advance)
{
    batom_cursor cursor;
    batom_cursor_from_accum(&cursor, demux->accum);
    batom_cursor_skip(&cursor, to_advance);
    batom_accum_trim(demux->accum, &cursor);
    return;
}

static size_t
b_mp4_fragment_demux_parse_data(bmp4_fragment_demux_t demux, batom_pipe_t pipe, bmp4_fragment_stream_t  idle_stream)
{
    size_t len;
    batom_cursor cursor;
    bmp4_fragment_stream_t stream;

    for(len=0;idle_stream;) {
        batom_t atom=batom_pipe_pop(pipe);
        BDBG_MSG_TRACE(("bmp4_fragment_demux_feed:%#lx atom:%#lx len:%u state:%u", (unsigned long)demux, (unsigned long)atom, len, demux->demux_state));
        if(!atom) {
            break;
        }
        len += batom_len(atom);
	    batom_accum_add_atom(demux->accum, atom);
        batom_release(atom);
        for(;;) {
            if(demux->demux_state == b_mp4_fragment_demux_state_box) {
		        bmp4_box box;
                size_t box_header_size;

                BDBG_MSG(("bmp4_fragment_demux_feed:%#lx looking for box", (unsigned long)demux));
                batom_cursor_from_accum(&cursor, demux->accum);
                box_header_size = bmp4_parse_box(&cursor, &box);
                if(box_header_size==0) {
                    break;
                }
                if(box.type != B_MP4_FRAGMENT_WRAPPER) {
                    b_mp4_fragment_demux_parse_advance(demux, 1);
                    continue;
                }
                demux->box_size = box.size;
                demux->box_header_size = box_header_size;
                demux->demux_state = b_mp4_fragment_demux_state_capture;
            } else if(demux->demux_state == b_mp4_fragment_demux_state_capture) {
                size_t accum_len = batom_accum_len(demux->accum);
                BDBG_MSG(("bmp4_fragment_demux_feed:%#lx capturing data %u:%u:%u", (unsigned long)demux, accum_len, demux->box_size, len));
                if(accum_len >= demux->box_size) {
                    int rc;
                    BDBG_ASSERT(idle_stream);
                    batom_cursor_from_accum(&cursor, demux->accum);
                    rc=b_mp4_fragment_track_start(&idle_stream->track, demux->accum, demux->box_header_size, demux->box_size, &cursor);
                    if(rc<0) {
                        b_mp4_fragment_demux_report_error(demux);
                        break;
                    }
                    for(stream=BLST_S_FIRST(&demux->streams);stream!=NULL;stream=BLST_S_NEXT(stream, link)) {
                        if(stream->stream_id == idle_stream->track.track_extends.track_ID) {
                            BDBG_MSG(("bmp4_fragment_demux_feed:%#lx captured box for track:%#lx(%u) %s", (unsigned long)demux, (unsigned long)stream, stream->stream_id, stream->busy?"BUSY":""));
                            break;
                        }
                    }
                    if(stream) {
                        if(!stream->busy) {
                            if(stream!=idle_stream) {
                                stream->track = idle_stream->track; /* copy track state, it should be safe, no pointers there */
                                b_mp4_fragment_track_init(&idle_stream->track); /* clear remnants of copied data */
                            }
                            rc = b_mp4_fragment_stream_start(demux, stream, &cursor, demux->accum, demux->box_size);
                            if(rc!=0) {
                                b_mp4_fragment_track_clear(&idle_stream->track); /* recycle data */
                                idle_stream = NULL;
                                goto stream_done;
                            }
                            idle_stream = NULL;
                            rc = b_mp4_fragment_set_handler(demux, stream);
                            if(rc!=0) {
                                b_mp4_fragment_stream_flush(stream);
                                goto stream_done;
                            }
stream_done:
                            demux->demux_state = b_mp4_fragment_demux_state_box;
                            b_mp4_fragment_demux_parse_advance(demux, demux->box_size);
                        } else {
                            b_mp4_fragment_track_clear(&idle_stream->track); /* recycle data */
                            demux->next_fragment_track_ID = idle_stream->track.track_extends.track_ID;
                            demux->demux_state = b_mp4_fragment_demux_state_remux; /* need to parse captured blocks */
                        }
                    } else {
                        BDBG_MSG(("bmp4_fragment_demux_feed:%#lx discarding fragment %u", (unsigned long)demux, idle_stream->track.track_extends.track_ID));
                        b_mp4_fragment_demux_parse_advance(demux, demux->box_size);
                        b_mp4_fragment_track_clear(&idle_stream->track); /* recycle data */
                        demux->demux_state = b_mp4_fragment_demux_state_box;
                    }
                    if(idle_stream) {
                        b_mp4_fragment_track_clear(&idle_stream->track); /* recycle data */
                    }
                }
                break;
            } else {
                (void)BERR_TRACE(BERR_NOT_SUPPORTED);
                break;
            }
        }
    }
    return len;
}

static int
b_mp4_fragment_demux_remux_tracks(bmp4_fragment_demux_t demux)
{
    bmp4_fragment_stream_t  stream;
    bmp4_fragment_stream_t  min_stream;
    uint32_t min_time;
    unsigned i;

    for(i=0;i<demux->config.burst_frames;) {
        int rc;
        for(min_time=0, min_stream=NULL,stream=BLST_S_FIRST(&demux->streams);stream!=NULL;stream=BLST_S_NEXT(stream, link)) {
            uint32_t time;
            batom_cursor cursor;
            if(!stream->busy) {
                continue;
            }

            batom_cursor_from_accum(&cursor, stream->data);
            if(!stream->track.next_sample_valid) {
                bool end_of_fragment=false;
                rc = b_mp4_fragment_track_next(demux, &stream->track, &cursor, &end_of_fragment);
                if(rc!=0) {
                    if(end_of_fragment) {
                        BDBG_MSG(("b_mp4_fragment_demux_remux_tracks:%#lx reached end of track %#lx:%u", (unsigned long)demux, (unsigned long)stream, stream->stream_id));
                        b_mp4_fragment_stream_flush(stream);
                        if(demux->next_fragment_track_ID == stream->track.track_extends.track_ID) {
                            demux->next_fragment_track_ID = 0;
                            demux->demux_state = b_mp4_fragment_demux_state_box; /* need to capture more data */
                        }
                    }
                    continue;
                }
                stream->track.next_sample_valid=true;
            }
            time = (1000*(stream->track.next_sample.time + (int32_t)stream->track.next_sample.composition_time_offset)) / stream->track.fragment_header.timescale;
            if(!min_stream || min_time>time) {
                min_stream = stream;
                min_time = time;
            }
        }
        if(!min_stream) {
            break;
        }
        BDBG_MSG(("b_mp4_fragment_demux_remux_tracks:%#lx selected stream (%u:%u) %u:" BMEDIA_FOURCC_FORMAT " %#lx:%u", (unsigned long)demux, i, demux->config.burst_frames, min_stream->track.track_extends.track_ID, BMEDIA_FOURCC_ARG(min_stream->track.fragment_header.fourcc), (unsigned long)min_stream, min_time));
        rc=b_mp4_fragment_stream_process_frame(demux, min_stream, min_time);
        if(rc==0) {
            i++;
        }
        min_stream->track.next_sample_valid=false;
    }
    return 0;
}


size_t
bmp4_fragment_demux_feed(bmp4_fragment_demux_t demux, batom_pipe_t pipe)
{
    size_t len=0;
    bmp4_fragment_stream_t  idle_stream;
    bmp4_fragment_stream_t  stream;

    BDBG_MSG_TRACE(("bmp4_fragment_demux_feed>:%#lx %#lx", (unsigned long)demux, (unsigned long)pipe));
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    BDBG_ASSERT(pipe);

    if(demux->demux_state != b_mp4_fragment_demux_state_remux) {
        for(stream=BLST_S_FIRST(&demux->streams);stream!=NULL;stream=BLST_S_NEXT(stream, link)) {
            if(!stream->busy) {
                break;
            }
        }
        idle_stream = stream;
        if(idle_stream!=NULL || BLST_S_FIRST(&demux->streams)==NULL ) {
            len = b_mp4_fragment_demux_parse_data(demux, pipe, idle_stream);
        }
    }
    b_mp4_fragment_demux_remux_tracks(demux);
    BDBG_MSG_TRACE(("bmp4_fragment_demux_feed<: %#lx processed %u bytes state:%u", (unsigned long)demux, len, demux->demux_state));
    return len;
}

int
bmp4_fragment_demux_seek(bmp4_fragment_demux_t demux, uint64_t off)
{
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    if(off!=demux->offset) {
        bmp4_fragment_demux_flush(demux);
        demux->offset = off;
    }
    return 0;
}


void bmp4_fragment_demux_flush(bmp4_fragment_demux_t demux)
{
    bmp4_fragment_stream_t  stream;
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    batom_accum_clear(demux->accum);

    for(stream=BLST_S_FIRST(&demux->streams);stream!=NULL;stream=BLST_S_NEXT(stream, link)) {
        b_mp4_fragment_stream_flush(stream);
    }
    demux->next_fragment_track_ID = 0;
    demux->demux_state = b_mp4_fragment_demux_state_box;

    return;
}

void
bmp4_fragment_demux_get_default_stream_cfg(bmp4_fragment_demux_t demux, bmp4_fragment_stream_cfg *cfg)
{
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    BSTD_UNUSED(demux);
	BDBG_ASSERT(cfg);
    BKNI_Memset(cfg, 0, sizeof(*cfg));
    return;
}

void
bmp4_fragment_get_stream_cfg(bmp4_fragment_stream_t stream, bmedia_pes_stream_cfg *cfg)
{
    BDBG_OBJECT_ASSERT(stream, bmp4_fragment_stream);

	bmedia_pes_default_stream_cfg(cfg);
    cfg->nvecs = sizeof(stream->vecs)/sizeof(stream->vecs[0]);
    cfg->vecs = stream->vecs;
    return;
}


bmp4_fragment_stream_t
bmp4_fragment_stream_create(bmp4_fragment_demux_t demux, const bmp4_fragment_stream_cfg *cfg, unsigned stream_id, batom_pipe_t pipe_out)
{
    bmp4_fragment_stream_t  stream;
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);

    BDBG_ASSERT(cfg);
    BDBG_ASSERT(pipe_out);

    stream = BKNI_Malloc(sizeof(*stream));
    if(!stream) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BDBG_OBJECT_INIT(stream, bmp4_fragment_stream);
    stream->busy = false;
    stream->pipe_out = pipe_out;
    stream->stream_id = stream_id;
	stream->vecs[B_MP4_STREAM_VEC_FRAME] = &bmedia_null_vec;
	stream->vecs[B_MP4_STREAM_VEC_RAI] = &bmedia_null_vec;
	stream->vecs[B_MP4_STREAM_VEC_EOS] = &bmedia_null_vec;

    stream->frame_accum = batom_accum_create(demux->config.factory);
    if(!stream->frame_accum) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_frame_accum;}
    stream->data = batom_accum_create(demux->config.factory);
    if(!stream->data) {(void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_data;}
    b_mp4_fragment_track_init(&stream->track);
    BLST_S_INSERT_HEAD(&demux->streams, stream, link);

#if defined(HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	FILE* fd = NULL;
	NEXUS_Error nResult;
	CommonCryptoSettings cryptoSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings nexusConfig;

	if(demux->wrmheader == NULL)
	{
/*************************
*	1. Get License
*************************/
		fd = fopen("/var/lib/humaxtv/drm/wrm.header","rb");
		if(fd == NULL)
		{
			BDBG_ERR(("No wrm.header in /var/lib/humaxtv/drm\n"));
			demux->cryptoHandle = NULL;
			demux->keyPrdy = NULL;
		}
		else
		{
			BDBG_ERR(("wrm.header is in /var/lib/humaxtv/drm! It's PlayReady SSTR!\n"));
			fseek(fd, 0, SEEK_END);
			demux->wrmheaderlen = ftell(fd);
			fseek(fd, 0, SEEK_SET);

			NEXUS_Memory_Allocate(demux->wrmheaderlen, NULL, &demux->wrmheader);
			fread(demux->wrmheader, demux->wrmheaderlen, 1, fd);
			fclose(fd);
			b_mp4_fragment_get_license(demux);
			system("rm -rf /var/lib/humaxtv/drm/wrm.header");

/*************************
*	2. Init Crypto
*************************/
			CommonCrypto_GetDefaultSettings(&cryptoSettings);
			demux->cryptoHandle = CommonCrypto_Open(&cryptoSettings);

			NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
			keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

			demux->keyPrdy = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
			if(demux->keyPrdy == NULL)
			{
				BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
				return -1;
			}

			NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

			nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
			nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
			nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
			nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
			nexusConfig.enableExtKey		= TRUE;
			nexusConfig.enableExtIv 		= TRUE;
			nexusConfig.aesCounterSize		= NEXUS_SecurityAesCounterSize_e64Bits;

			nResult = NEXUS_Security_ConfigAlgorithm(demux->keyPrdy, &nexusConfig);
			if(nResult != NEXUS_SUCCESS)
			{
				BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
				return -1;
			}
			NEXUS_Memory_Allocate(80, NULL, (void **)&(demux->pEkl));
		}
	}
#endif
#endif

    return stream;

err_data:
    batom_accum_destroy(stream->frame_accum);
err_frame_accum:
    BKNI_Free(stream);
err_alloc:
    return NULL;
}

void
bmp4_fragment_stream_destroy(bmp4_fragment_demux_t demux, bmp4_fragment_stream_t stream)
{
    BDBG_OBJECT_ASSERT(demux, bmp4_fragment_demux);
    BDBG_OBJECT_ASSERT(stream, bmp4_fragment_stream);

#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
#if defined(CONFIG_PLAYREADY_25)
	if(demux->pDecryptBuf)
	{
		BKNI_Free(demux->pDecryptBuf);
		demux->pDecryptBuf = NULL;
	}
	demux->nDecryptBufSize = 0;
	if(demux->bSetContext)
	{
		Drm_Reader_Close(&(demux->stDecryptContext));
		demux->bSetContext = FALSE;
	}
	if(demux->pAppContext != NULL)
	{
		Drm_Uninitialize(demux->pAppContext);
		Oem_MemFree(demux->pAppContext);
		demux->pAppContext = NULL;
	}
	if(demux->f_pOEMContext != NULL)
	{
		Oem_Destroy(demux->f_pOEMContext);
		demux->f_pOEMContext = NULL;
	}
#endif
	if(demux->wrmheader != NULL)
	{
		NEXUS_Memory_Free(demux->wrmheader);
		demux->wrmheader = NULL;
	}

	if(demux->keyPrdy != NULL)
	{
		NEXUS_Security_FreeKeySlot(demux->keyPrdy);
		demux->keyPrdy = NULL;
	}

	if(demux->cryptoHandle != NULL)
	{
		CommonCrypto_Close(demux->cryptoHandle);
		demux->cryptoHandle = NULL;
	}

	if(demux->pEkl != NULL)
	{
		NEXUS_Memory_Free(demux->pEkl);
		demux->pEkl = NULL;
	}
#endif
#endif

    BLST_S_REMOVE(&demux->streams, stream, bmp4_fragment_stream, link);
    batom_accum_destroy(stream->data);
    batom_accum_destroy(stream->frame_accum);
    BDBG_OBJECT_DESTROY(stream, bmp4_fragment_stream);
    BKNI_Free(stream);
    return;
}
