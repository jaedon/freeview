/**
	@file     hlib_av.c
	@brief   libpng interface function file

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <hlib.h>
#include <assert.h>

#include <dlfcn.h>

#include "_hlib_stdlib.h"
#include "_hlib_mem.h"
#include "_hlib_sem.h"
#include "_hlib_log.h"

#include "_hlib_av.h"

#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libavutil/lfg.h>
#include <libswscale/swscale.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	RETURN_IF(expr, err)		do{if(expr)return (err);}while(0)
#define	RETURN_IF_VOID(expr)		do{if(expr)return;}while(0)
#define	RETURN_IF_GOTO(expr, gt)	do{if(expr)goto gt;}while(0)
#define	FFMPEG_PROBE_SIZE			0x800000			//	8MB

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL		bEnable;
	HUINT32		ulSemId;
	void		*pvAVFormatHandle;
	void		*pvAVCodecHandle;
	void		*pvSWScaleHandle;
	void 		*pvAVUtil;

	//	Symbol Link for FFMPEG Used function.
	//	libavformat
	void		(*fn_av_register_all)			(void);
	int			(*fn_avformat_open_input)		(AVFormatContext **ps, const char *filename, AVInputFormat *fmt, AVDictionary **options);
	int			(*fn_avformat_find_stream_info)	(AVFormatContext *ic, AVDictionary **options);
	void		(*fn_avformat_close_input)		(AVFormatContext **s);
	int			(*fn_av_read_frame)				(AVFormatContext *s, AVPacket *pkt);
	int			(*fn_av_seek_frame)				(AVFormatContext *s, int stream_index, int64_t timestamp, int flags);

	// libavformat_context
	AVFormatContext* 	(*fn_avformat_alloc_context)	(void);
	void				(*fn_avformat_free_context)		(AVFormatContext *s);

	//	libavcodec
	void		(*fn_avcodec_register_all)		(void);
	AVCodec*	(*fn_avcodec_find_decoder)		(enum AVCodecID id);
	int			(*fn_avcodec_open2)				(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
	int			(*fn_avcodec_close)				(AVCodecContext *avctx);
	AVFrame*	(*fn_avcodec_alloc_frame)		(void);
	void		(*fn_avcodec_free_frame)		(AVFrame **frame);
	int			(*fn_avpicture_get_size)		(enum PixelFormat pix_fmt, int width, int height);
	int			(*fn_avpicture_fill)			(AVPicture *picture, uint8_t *ptr, enum PixelFormat pix_fmt, int width, int height);
	int			(*fn_avcodec_decode_video2)		(AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, const AVPacket *avpkt);
	void		(*fn_av_free_packet)			(AVPacket *pkt);
	void		(*fn_avcodec_flush_buffers)		(AVCodecContext *avctx);
	void		(*fn_avcodec_get_frame_defaults)(AVFrame *frame);
	void		(*fn_avpicture_deinterlace)		(AVPicture *dst, const AVPicture *src, enum PixelFormat pix_fmt, int width, int height);

	//	libswscale
	struct SwsContext	*(*fn_sws_getContext)	(int srcW, int srcH, enum PixelFormat srcFormat, int dstW, int dstH, enum PixelFormat dstFormat,
												 int flags, SwsFilter *srcFilter, SwsFilter *dstFilter, const double *param);
	int					(*fn_sws_scale)			(struct SwsContext *c, const uint8_t *const srcSlice[], const int srcStride[],
												 int srcSliceY, int srcSliceH, uint8_t *const dst[], const int dstStride[]);
	void				(*fn_sws_freeContext)	(struct SwsContext *swsContext);
	void				(*fn_av_log_set_level)	(int level);
	int					(*fn_av_strerror)		(int errnum, char *errbuf, size_t errbuf_size);

} ffmpeg_init_t;

struct _HxAV_tag
{
	AVFormatContext	*fmt_ctx;

	HINT32			nVideoStreamCount;
	HINT32			nVideoStreamIndex;
	AVStream		*pstVideoStream;
	AVCodecContext	*pstVideoCodecContext;
	AVCodec			*pstVideoCodec;

	HINT32			nAudioStreamCount;
	HINT32			nAudioStreamIndex;
	AVStream		*pstAudioStream;
	AVCodecContext	*pstAudioCodecContext;
	AVCodec			*pstAudioCodec;

	AVStream		*pstSubtitleStream;

	AVPacket		*pstPacket;
	AVFrame			*pstFrame;
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*******************************************************************************************
 * Static
 ******************************************************************************************/
static ffmpeg_init_t*	ffmpeg_getInstance(void)
{
	static ffmpeg_init_t	*s_pstFFMPEGInit = NULL;

	if (s_pstFFMPEGInit)
	{
		HxSEMT_Get(s_pstFFMPEGInit->ulSemId);
		if (s_pstFFMPEGInit->bEnable)
			return s_pstFFMPEGInit;

		HxSEMT_Release(s_pstFFMPEGInit->ulSemId);
		return NULL;
	} else
	{
		ffmpeg_init_t	*ffmpeg = HLIB_MEM_Calloc(sizeof(ffmpeg_init_t));

		s_pstFFMPEGInit = ffmpeg;
		HxSEMT_Create(&ffmpeg->ulSemId, "hxavsem", 0);
		HxSEMT_Get(ffmpeg->ulSemId);

		ffmpeg->pvAVFormatHandle = dlopen("/usr/lib/libavformat.so", RTLD_LAZY);
		ffmpeg->pvAVCodecHandle  = dlopen("/usr/lib/libavcodec.so", RTLD_LAZY);
		ffmpeg->pvSWScaleHandle  = dlopen("/usr/lib/libswscale.so", RTLD_LAZY);
		ffmpeg->pvAVUtil		 = dlopen("/usr/lib/libavutil.so", 	RTLD_LAZY);

		if (!(ffmpeg->pvAVFormatHandle && ffmpeg->pvAVCodecHandle && ffmpeg->pvSWScaleHandle))
		{
			HxLOG_Critical("[%s](%d) can't find ffmpeg library in /usr/lib directory!!!\n");
			HxLOG_Assert(0);

			return NULL;
		}

		ffmpeg->fn_av_register_all				= dlsym(ffmpeg->pvAVFormatHandle, "av_register_all");
		ffmpeg->fn_avformat_open_input			= dlsym(ffmpeg->pvAVFormatHandle, "avformat_open_input");
		ffmpeg->fn_avformat_find_stream_info	= dlsym(ffmpeg->pvAVFormatHandle, "avformat_find_stream_info");
		ffmpeg->fn_avformat_close_input			= dlsym(ffmpeg->pvAVFormatHandle, "avformat_close_input");
		ffmpeg->fn_av_read_frame				= dlsym(ffmpeg->pvAVFormatHandle, "av_read_frame");
		ffmpeg->fn_av_seek_frame				= dlsym(ffmpeg->pvAVFormatHandle, "av_seek_frame");

		ffmpeg->fn_avformat_alloc_context		= dlsym(ffmpeg->pvAVFormatHandle, "avformat_alloc_context");
		ffmpeg->fn_avformat_free_context		= dlsym(ffmpeg->pvAVFormatHandle, "avformat_free_context");

		ffmpeg->fn_avcodec_register_all			= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_register_all");
		ffmpeg->fn_avcodec_find_decoder			= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_find_decoder");
		ffmpeg->fn_avcodec_open2				= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_open2");
		ffmpeg->fn_avcodec_close				= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_close");
		ffmpeg->fn_avcodec_alloc_frame			= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_alloc_frame");
		ffmpeg->fn_avcodec_free_frame			= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_free_frame");
		ffmpeg->fn_avpicture_get_size			= dlsym(ffmpeg->pvAVCodecHandle,  "avpicture_get_size");
		ffmpeg->fn_avpicture_fill				= dlsym(ffmpeg->pvAVCodecHandle,  "avpicture_fill");
		ffmpeg->fn_avcodec_decode_video2		= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_decode_video2");
		ffmpeg->fn_av_free_packet				= dlsym(ffmpeg->pvAVCodecHandle,  "av_free_packet");
		ffmpeg->fn_avcodec_flush_buffers		= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_flush_buffers");
		ffmpeg->fn_avcodec_get_frame_defaults	= dlsym(ffmpeg->pvAVCodecHandle,  "avcodec_get_frame_defaults");
		ffmpeg->fn_avpicture_deinterlace		= dlsym(ffmpeg->pvAVCodecHandle,  "avpicture_deinterlace");

		ffmpeg->fn_sws_getContext				= dlsym(ffmpeg->pvSWScaleHandle,  "sws_getContext");
		ffmpeg->fn_sws_scale					= dlsym(ffmpeg->pvSWScaleHandle,  "sws_scale");
		ffmpeg->fn_sws_freeContext				= dlsym(ffmpeg->pvSWScaleHandle,  "sws_freeContext");
		ffmpeg->fn_av_log_set_level				= dlsym(ffmpeg->pvAVUtil, 		  "av_log_set_level");
		ffmpeg->fn_av_strerror					= dlsym(ffmpeg->pvAVUtil,         "av_strerror");

		HxLOG_Assert(ffmpeg->fn_av_register_all);
		HxLOG_Assert(ffmpeg->fn_avformat_open_input);
		HxLOG_Assert(ffmpeg->fn_avformat_find_stream_info);
		HxLOG_Assert(ffmpeg->fn_avformat_close_input);
		HxLOG_Assert(ffmpeg->fn_av_read_frame);
		HxLOG_Assert(ffmpeg->fn_av_seek_frame);

		HxLOG_Assert(ffmpeg->fn_avcodec_register_all);
		HxLOG_Assert(ffmpeg->fn_avcodec_find_decoder);
		HxLOG_Assert(ffmpeg->fn_avcodec_open2);
		HxLOG_Assert(ffmpeg->fn_avcodec_close);
		HxLOG_Assert(ffmpeg->fn_avcodec_alloc_frame);
		HxLOG_Assert(ffmpeg->fn_avcodec_free_frame);
		HxLOG_Assert(ffmpeg->fn_avpicture_fill);
		HxLOG_Assert(ffmpeg->fn_avcodec_decode_video2);
		HxLOG_Assert(ffmpeg->fn_av_free_packet);
		HxLOG_Assert(ffmpeg->fn_avcodec_flush_buffers);
		HxLOG_Assert(ffmpeg->fn_avcodec_get_frame_defaults);

		HxLOG_Assert(ffmpeg->fn_sws_getContext);
		HxLOG_Assert(ffmpeg->fn_sws_scale);
		HxLOG_Assert(ffmpeg->fn_sws_freeContext);
		HxLOG_Assert(ffmpeg->fn_av_log_set_level);
		HxLOG_Assert(ffmpeg->fn_av_strerror);

		ffmpeg->fn_av_register_all();
		ffmpeg->fn_avcodec_register_all();
		ffmpeg->fn_av_log_set_level(AV_LOG_QUIET);

		ffmpeg->bEnable = TRUE;
		s_pstFFMPEGInit = ffmpeg;
	}

	return s_pstFFMPEGInit;
}

static HBOOL	hlib_av_GetVideoPacket(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo)
{
	HBOOL	bRet = FALSE;
	HINT32	nTry = 0;

	if (pstInfo->pstPacket)
	{
		ffmpeg->fn_av_free_packet(pstInfo->pstPacket);
		HLIB_MEM_Free(pstInfo->pstPacket);
	}

	pstInfo->pstPacket = (AVPacket*)HLIB_MEM_Calloc(sizeof(AVPacket));
	while (nTry++ < 1000)
	{
		if (ffmpeg->fn_av_read_frame(pstInfo->fmt_ctx, pstInfo->pstPacket) >= 0)
		{
			if (pstInfo->pstPacket->stream_index == pstInfo->nVideoStreamIndex)
			{
				bRet = TRUE;
				break;
			} else
				ffmpeg->fn_av_free_packet(pstInfo->pstPacket);
		} else
		{
			break;
		}
	}

	return bRet;
}

static HBOOL	hlib_av_DecodeVideoPacket(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo)
{
	HINT32	nFinished, nDecoded;

	if (pstInfo->pstPacket->stream_index != pstInfo->nVideoStreamIndex || NULL == pstInfo->pstVideoCodec)
		return FALSE;

	ffmpeg->fn_avcodec_get_frame_defaults(pstInfo->pstFrame);
	nDecoded = ffmpeg->fn_avcodec_decode_video2(pstInfo->pstVideoCodecContext, pstInfo->pstFrame, &nFinished, pstInfo->pstPacket);
	if (nDecoded < 0)
	{
		HxLOG_Error("Fail to decode video frame [pFrame:%p]\n", pstInfo->pstFrame);
		return FALSE;
	}
	HxLOG_Debug("[%s] -- pFrame : %p \n", __FUNCTION__, pstInfo->pstFrame);
	return (nFinished > 0) ? TRUE : FALSE;
}

static HERROR	hlib_av_SeekVideoStream(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo, HUINT32 ulTime)
{
	HINT64	timestamp;
	HINT32	ret;
	HINT32	nKeyFrame = 0;
	HBOOL	bFrame;
	HINT32	nCount;

	RETURN_IF(ffmpeg == NULL, ERR_FAIL);

	timestamp = AV_TIME_BASE * (HINT64)ulTime;
	if (timestamp < 0)
		timestamp = 0;

	ret = ffmpeg->fn_av_seek_frame(pstInfo->fmt_ctx, -1, timestamp, 0);
	if (ret >= 0)
	{
		ffmpeg->fn_avcodec_flush_buffers(pstInfo->pstVideoCodecContext);
	} else
	{
		HxLOG_Error("fail to seek video stream.\n");
		return ERR_FAIL;
	}

	do
	{
		nCount = 0;
		bFrame = FALSE;
		while (!bFrame && nCount < 20)
		{
            HBOOL  bPacket;

            bPacket = hlib_av_GetVideoPacket(ffmpeg, pstInfo);
            if (bPacket == FALSE)
				return ERR_FAIL;
          	bFrame = hlib_av_DecodeVideoPacket(ffmpeg, pstInfo);
			++nCount;
		}

		++nKeyFrame;
	} while ((!bFrame || pstInfo->pstFrame->key_frame) && nKeyFrame < 200);

	if (bFrame == FALSE)
	{
		HxLOG_Error("fail to seek video stream\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HBOOL		hlib_av_DecodeVideoFrame(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo)
{
	HBOOL	bFinished = FALSE;
	HINT32	nCount;

	nCount = 0;
	while ((!bFinished && hlib_av_GetVideoPacket(ffmpeg, pstInfo)) && nCount < 20)
	{
		bFinished = hlib_av_DecodeVideoPacket(ffmpeg, pstInfo);
		nCount++;
	}

	if (!bFinished)
		HxLOG_Error("decode frame error \n");

	return bFinished;
}

static HBOOL	hlib_av_InitVideoStream(ffmpeg_init_t *ffmpeg, HxAV_t *pstAVInfo)
{
	HINT32	i	= 0;
	HBOOL	bFindVideoStream	= FALSE;

	pstAVInfo->nVideoStreamCount = 0;
	pstAVInfo->nVideoStreamIndex = -1;
	for (i = 0; i < pstAVInfo->fmt_ctx->nb_streams; i++)
	{
		if (pstAVInfo->fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if( bFindVideoStream == FALSE )
			{
				pstAVInfo->nVideoStreamIndex = i;
				pstAVInfo->pstVideoStream = pstAVInfo->fmt_ctx->streams[i];
				bFindVideoStream = TRUE;
			}
			pstAVInfo->nVideoStreamCount++;
		}
	}

	if (bFindVideoStream == FALSE)
	{
		HxLOG_Error("Can't find Video stream in [%s]\n", pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}

	pstAVInfo->pstVideoCodecContext = pstAVInfo->pstVideoStream->codec;
	pstAVInfo->pstVideoCodec = ffmpeg->fn_avcodec_find_decoder(pstAVInfo->pstVideoCodecContext->codec_id);
	pstAVInfo->pstVideoCodecContext->codec = pstAVInfo->pstVideoCodec;

	if (pstAVInfo->pstVideoCodec == NULL)
	{
		HxLOG_Error("can't find video codec [%s]\n", pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}
	HxLOG_Debug("[%s] -- codec : %p, codec_type : %d \n", __FUNCTION__, pstAVInfo->pstVideoCodec, pstAVInfo->pstVideoCodec->type);

	if (ffmpeg->fn_avcodec_open2(pstAVInfo->pstVideoCodecContext, pstAVInfo->pstVideoCodec, NULL) < 0)
	{
		HxLOG_Error("could not open video codec! [%s][%s]\n", pstAVInfo->pstVideoCodec->name, pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}
	return TRUE;
}

static HBOOL	hlib_av_InitAudioStream(ffmpeg_init_t *ffmpeg, HxAV_t *pstAVInfo)
{
	HINT32	i	= 0;
	HBOOL	bFindAudioStream	= FALSE;

	pstAVInfo->nAudioStreamCount = 0;
	pstAVInfo->nAudioStreamIndex = -1;
	for (i = 0; i < pstAVInfo->fmt_ctx->nb_streams; i++)
	{
		if (pstAVInfo->fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			if( bFindAudioStream == FALSE )
			{
				pstAVInfo->nAudioStreamIndex = i;
				pstAVInfo->pstAudioStream = pstAVInfo->fmt_ctx->streams[i];
				bFindAudioStream = TRUE;
			}
			pstAVInfo->nAudioStreamCount++;
		}
	}

	if (bFindAudioStream == FALSE)
	{
		HxLOG_Error("Can't find Audio stream in [%s]\n", pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}

	pstAVInfo->pstAudioCodecContext = pstAVInfo->pstAudioStream->codec;
	pstAVInfo->pstAudioCodec = ffmpeg->fn_avcodec_find_decoder(pstAVInfo->pstAudioCodecContext->codec_id);
	pstAVInfo->pstAudioCodecContext->codec = pstAVInfo->pstAudioCodec;

	if (pstAVInfo->pstAudioCodec == NULL)
	{
		HxLOG_Error("can't find audio codec [%s]\n", pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}
	HxLOG_Debug("[%s] -- codec : %p, codec_type : %d \n", __FUNCTION__, pstAVInfo->pstAudioCodec, pstAVInfo->pstAudioCodec->type);

	if (ffmpeg->fn_avcodec_open2(pstAVInfo->pstAudioCodecContext, pstAVInfo->pstAudioCodec, NULL) < 0)
	{
		HxLOG_Error("could not open audio codec! [%s][%s]\n", pstAVInfo->pstAudioCodec->name, pstAVInfo->fmt_ctx->filename);
		return FALSE;
	}
	return TRUE;

}

static HBOOL	hlib_av_InitSubtitleStream(ffmpeg_init_t *ffmpeg, HxAV_t *pstAVInfo)
{
	return FALSE;
}

static HINT32	hlib_av_Decode_interrupt_cb	(void *ctx)
{
	AVFormatContext *context = (AVFormatContext *)ctx;
	HERROR			ret = 0;

	HxLOG_Assert(context);
	ret = HLIB_FILE_IsValid((HCHAR *)context->filename);
	return ret;
}

/*******************************************************************************************
 * Public
 ******************************************************************************************/

HxAV_t*		hlib_av_Open(ffmpeg_init_t *ffmpeg, const HCHAR *pszFileName,
							HINT32 (*interrupt_callback)(void*),
							void *interrupt_userdata, HBOOL bOnlyAudio)
{
	HxAV_t		*pstAVInfo = NULL;
	HUINT8		*pBuf = NULL;
	HBOOL		bRet = FALSE;

	int		err;
	AVFormatContext	*fmt_ctx = NULL;


	RETURN_IF((ffmpeg == NULL), NULL);

	if (HLIB_FILE_Exist(pszFileName) != TRUE)
		goto err_rtn;

	if ((err = ffmpeg->fn_avformat_open_input(&fmt_ctx, pszFileName, NULL, NULL)) < 0)
	{
//		char	szBuffer[128];
//		ffmpeg->fn_av_strerror(err, szBuffer, 127);
//		HxLOG_Error("can't open file [%s] - err [%x][%s]\n", pszFileName, err, szBuffer);
		HxLOG_Error("can't open file [%s] - err [%x]\n", pszFileName, err);
		goto err_rtn;
	}

	fmt_ctx->probesize 						= FFMPEG_PROBE_SIZE;
	if(interrupt_callback)
	{
		fmt_ctx->interrupt_callback.callback	= interrupt_callback;
		fmt_ctx->interrupt_callback.opaque		= interrupt_userdata;
	}

	if (ffmpeg->fn_avformat_find_stream_info(fmt_ctx, NULL) < 0)
	{
		HxLOG_Error("can't find stream info [%s]\n", pszFileName);
		goto err_rtn;
	}

	pstAVInfo = (HxAV_t*)HLIB_MEM_Calloc(sizeof(HxAV_t));
	pstAVInfo->fmt_ctx    = fmt_ctx;
	pstAVInfo->pstFrame   = ffmpeg->fn_avcodec_alloc_frame();

	if ( !bOnlyAudio )
		bRet |= hlib_av_InitVideoStream(ffmpeg, pstAVInfo);
	bRet |= hlib_av_InitAudioStream(ffmpeg, pstAVInfo);
	hlib_av_InitSubtitleStream(ffmpeg, pstAVInfo);

	//	Video나 Audio가 존재한다면, 미디어 화일로 인식.
	if (bRet == TRUE)
		return pstAVInfo;

err_rtn:

	if (pBuf)
		HLIB_MEM_Free(pBuf);

	if (fmt_ctx)
		ffmpeg->fn_avformat_close_input(&fmt_ctx);

	if (pstAVInfo)
	{
		if (pstAVInfo->pstVideoCodecContext)
		{
			ffmpeg->fn_avcodec_close(pstAVInfo->pstVideoCodecContext);
		}

		if (pstAVInfo->pstAudioCodecContext)
		{
			ffmpeg->fn_avcodec_close(pstAVInfo->pstAudioCodecContext);
		}

		//	TODO:	destroy subtitle codec

		if (pstAVInfo->pstPacket)
		{
			ffmpeg->fn_av_free_packet(pstAVInfo->pstPacket);
			HLIB_MEM_Free(pstAVInfo->pstPacket);
		}

		if (pstAVInfo->pstFrame)
			ffmpeg->fn_avcodec_free_frame(&pstAVInfo->pstFrame);

		HLIB_MEM_Free(pstAVInfo);
	}

	return NULL;
}

void		hlib_av_Close(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo)
{
	RETURN_IF_VOID(ffmpeg == NULL);

	if (pstInfo->pstVideoCodecContext)
	{
		ffmpeg->fn_avcodec_close(pstInfo->pstVideoCodecContext);
	}

	if (pstInfo->pstAudioCodecContext)
	{
		ffmpeg->fn_avcodec_close(pstInfo->pstAudioCodecContext);
	}

	//	TODO:	destroy subtitle codec

	if (pstInfo->fmt_ctx)
		ffmpeg->fn_avformat_close_input(&pstInfo->fmt_ctx);

	if (pstInfo->pstPacket)
	{
		ffmpeg->fn_av_free_packet(pstInfo->pstPacket);
		HLIB_MEM_Free(pstInfo->pstPacket);
	}

	if (pstInfo->pstFrame)
		ffmpeg->fn_avcodec_free_frame(&pstInfo->pstFrame);

	HLIB_MEM_Free(pstInfo);
}

HERROR		hlib_av_Getvideosize(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo, HINT32 *pnWidth, HINT32 *pnHeight)
{
	HxLOG_Assert(pstInfo && pstInfo->fmt_ctx);

	RETURN_IF(!pstInfo->pstVideoCodecContext, ERR_FAIL);

	if (pnWidth)
		*pnWidth = pstInfo->pstVideoCodecContext->width;
	if (pnHeight)
		*pnHeight = pstInfo->pstVideoCodecContext->height;

	return ERR_OK;
}

HERROR		hlib_av_Getstreamduration(ffmpeg_init_t *ffmpeg, HxAV_t *pstInfo, HUINT32 *pulDuration)
{
	HxLOG_Assert(pstInfo && pstInfo->fmt_ctx);

	if (pulDuration)
		*pulDuration = pstInfo->fmt_ctx->duration / AV_TIME_BASE;

	return ERR_OK;
}

//
HERROR	HLIB_AV_GetVideoStreamInfo(const HCHAR *pszFileName,
									HxAV_SteamInfo_t *pstStreamInfo,
									HINT32 (*interrupt_callback)(void*),
									void *interrupt_userdata)
{
	ffmpeg_init_t		*ffmpeg = ffmpeg_getInstance();
	HxAV_t				*pstAVInfo;
	HINT32				nDuration = -1;
	HxAV_StreamType_e	eStreamType = eHxAV_STREAMTYPE_UNKNOWN;

	if (ffmpeg == NULL)
		return ERR_FAIL;

	pstAVInfo = hlib_av_Open(ffmpeg, pszFileName, interrupt_callback, interrupt_userdata, FALSE);
	RETURN_IF_GOTO(pstAVInfo == NULL, err_rtn);

	RETURN_IF_GOTO(!pstAVInfo->pstVideoCodecContext, err_rtn);
	RETURN_IF_GOTO(!pstAVInfo->pstVideoStream, err_rtn);
	RETURN_IF_GOTO(!pstAVInfo->pstVideoCodec, err_rtn);

	if (pstStreamInfo)
	{
		//1 Video Information
		pstStreamInfo->nVideoTrack = pstAVInfo->nVideoStreamCount;
		if( pstStreamInfo->nVideoTrack > 0 )
		{
			HLIB_STD_StrNCpySafe(pstStreamInfo->stVideoInfo.szCodecName, pstAVInfo->pstVideoCodec->name, 32);
			pstStreamInfo->stVideoInfo.nWidth 			= pstAVInfo->pstVideoCodecContext->width;
			pstStreamInfo->stVideoInfo.nHeight			= pstAVInfo->pstVideoCodecContext->height;
			pstStreamInfo->stVideoInfo.nBitRate 		= pstAVInfo->pstVideoCodecContext->bit_rate;
		}

		//1 Audio Information
		if (pstAVInfo->pstAudioCodec && pstAVInfo->pstAudioCodecContext)
		{
			pstStreamInfo->nAudioTrack = pstAVInfo->nAudioStreamCount;
			if( pstStreamInfo->nAudioTrack > 0 )
			{
				HLIB_STD_StrNCpySafe(pstStreamInfo->stAudioInfo.szCodecName, pstAVInfo->pstAudioCodec->name, 32);
				pstStreamInfo->stAudioInfo.nBitPerSample	= pstAVInfo->pstAudioCodecContext->bits_per_raw_sample;
				pstStreamInfo->stAudioInfo.nSampleRate 		= pstAVInfo->pstAudioCodecContext->sample_rate;
				pstStreamInfo->stAudioInfo.nBitRate			= pstAVInfo->pstAudioCodecContext->bit_rate;
				pstStreamInfo->stAudioInfo.nChannels		= pstAVInfo->pstAudioCodecContext->channels;
			}
		}


		//1 Common Information
		if( pstStreamInfo->nVideoTrack > 0 )
		{
			HLIB_STD_StrNCpySafe(pstStreamInfo->szCodecName, pstStreamInfo->stVideoInfo.szCodecName, 32);
			eStreamType = eHxAV_STREAMTYPE_VIDEO;
		}
		else if( pstStreamInfo->nAudioTrack > 0 )
		{
			HLIB_STD_StrNCpySafe(pstStreamInfo->szCodecName, pstStreamInfo->stAudioInfo.szCodecName, 32);
			eStreamType = eHxAV_STREAMTYPE_AUDIO;
		}
		else
		{
			eStreamType = eHxAV_STREAMTYPE_OTHER;
		}

		pstStreamInfo->nWidth 	= pstStreamInfo->stVideoInfo.nWidth;
		pstStreamInfo->nHeight 	= pstStreamInfo->stVideoInfo.nHeight;

		nDuration = (HINT32)(pstAVInfo->fmt_ctx->duration / AV_TIME_BASE);
		pstStreamInfo->nDuration = nDuration < 0 ? -1 : nDuration;

		pstStreamInfo->llFrameCount 	= pstAVInfo->pstVideoStream->nb_frames;
		pstStreamInfo->nStreamType 		= eStreamType;
		pstStreamInfo->nBitRate 		= pstAVInfo->fmt_ctx->bit_rate;


#if 0
	HxLOG_Debug( "[ HxAV ] ( %s ) AV Stream Information++++++++++++++++++++++++++++\n", pstAVInfo->fmt_ctx->filename );
	HxLOG_Debug( "\t[ HxAV ] Common Information---------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Duration    \t\t\t\t = %d\n", nDuration );
	HxLOG_Debug( "\t[ HxAV ] total bitrate \t\t\t\t = %d\n", pstStreamInfo->nBitRate );
	HxLOG_Debug( "\t[ HxAV ] --------------------------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Video Information---------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Video Track \t\t\t\t = %d\n", pstStreamInfo->nVideoTrack );
	HxLOG_Debug( "\t[ HxAV ] Codec name  \t\t\t\t = %s\n", pstStreamInfo->stVideoInfo.szCodecName );
	HxLOG_Debug( "\t[ HxAV ] Resolution  \t\t\t\t = %d x %d\n", pstStreamInfo->stVideoInfo.nWidth, pstStreamInfo->stVideoInfo.nHeight );
	HxLOG_Debug( "\t[ HxAV ] Bit Rate    \t\t\t\t = %d\n", pstStreamInfo->stVideoInfo.nBitRate );
	HxLOG_Debug( "\t[ HxAV ] --------------------------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Audio Information---------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Audio Track \t\t\t\t = %d\n", pstStreamInfo->nAudioTrack );
	HxLOG_Debug( "\t[ HxAV ] Codec name  \t\t\t\t = %s\n", pstStreamInfo->stAudioInfo.szCodecName );
	HxLOG_Debug( "\t[ HxAV ] SampleRate  \t\t\t\t = %d\n", pstStreamInfo->stAudioInfo.nSampleRate );
	HxLOG_Debug( "\t[ HxAV ] Bit Rate    \t\t\t\t = %d\n", pstStreamInfo->stAudioInfo.nBitRate );
	HxLOG_Debug( "\t[ HxAV ] nChannels   \t\t\t\t = %d\n", pstStreamInfo->stAudioInfo.nChannels );
	HxLOG_Debug( "\t[ HxAV ] --------------------------------------------------\n" );
	HxLOG_Debug( "[ HxAV ] ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n" );
#endif


	}

	hlib_av_Close(ffmpeg, pstAVInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return ERR_OK;

err_rtn:
	if (pstAVInfo)
		hlib_av_Close(ffmpeg, pstAVInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return ERR_FAIL;
}

HERROR	HLIB_AV_GetAudioStreamInfo(const HCHAR *pszFileName,
									HxAV_AudioSteamInfo_t *pstAudioStreamInfo,
									HINT32 (*interrupt_callback)(void*),
									void *interrupt_userdata)
{
	ffmpeg_init_t		*ffmpeg = ffmpeg_getInstance();
	HxAV_t				*pstAVInfo = NULL;
	HINT32				nDuration = -1;
	HxAV_StreamType_e	eStreamType = eHxAV_STREAMTYPE_UNKNOWN;

	if (ffmpeg == NULL)
		return ERR_FAIL;

	RETURN_IF_GOTO(pstAudioStreamInfo == NULL, err_rtn);

	pstAVInfo = hlib_av_Open(ffmpeg, pszFileName, interrupt_callback, interrupt_userdata, TRUE);
	RETURN_IF_GOTO(pstAVInfo == NULL, err_rtn);
	RETURN_IF_GOTO(pstAVInfo->pstAudioCodec == NULL, err_rtn);

	//1 Audio Information
	if (pstAVInfo->pstAudioCodec && pstAVInfo->pstAudioCodecContext)
	{
		pstAudioStreamInfo->nAudioTrack = pstAVInfo->nAudioStreamCount;
		if( pstAudioStreamInfo->nAudioTrack > 0 )
		{
			HLIB_STD_StrNCpySafe(pstAudioStreamInfo->stAudioInfo.szCodecName, pstAVInfo->pstAudioCodec->name,
					sizeof(pstAudioStreamInfo->stAudioInfo.szCodecName));
			pstAudioStreamInfo->stAudioInfo.nBitPerSample	= pstAVInfo->pstAudioCodecContext->bits_per_raw_sample;
			pstAudioStreamInfo->stAudioInfo.nSampleRate 		= pstAVInfo->pstAudioCodecContext->sample_rate;
			pstAudioStreamInfo->stAudioInfo.nBitRate			= pstAVInfo->pstAudioCodecContext->bit_rate;
			pstAudioStreamInfo->stAudioInfo.nChannels		= pstAVInfo->pstAudioCodecContext->channels;
		}
	}

	//1 Common Information
	if( pstAudioStreamInfo->nAudioTrack > 0 )
	{
		eStreamType = eHxAV_STREAMTYPE_AUDIO;
	}
	else
	{
		eStreamType = eHxAV_STREAMTYPE_OTHER;
	}

	nDuration = (HINT32)(pstAVInfo->fmt_ctx->duration / AV_TIME_BASE);
	pstAudioStreamInfo->nDuration = nDuration < 0 ? -1 : nDuration;

	pstAudioStreamInfo->nStreamType 		= eStreamType;
	pstAudioStreamInfo->nBitRate 		= pstAVInfo->fmt_ctx->bit_rate;

#if 0
	HxLOG_Debug( "[ HxAV ] ( %s ) Audio Stream Information++++++++++++++++++++++++++++\n", pstAVInfo->fmt_ctx->filename );
	HxLOG_Debug( "\t[ HxAV ] Common Information---------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Duration    \t\t\t\t = %d\n", nDuration );
	HxLOG_Debug( "\t[ HxAV ] total bitrate \t\t\t\t = %d\n", pstAudioStreamInfo->nBitRate );
	HxLOG_Debug( "\t[ HxAV ] --------------------------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Audio Information---------------------------------\n" );
	HxLOG_Debug( "\t[ HxAV ] Audio Track \t\t\t\t = %d\n", pstAudioStreamInfo->nAudioTrack );
	HxLOG_Debug( "\t[ HxAV ] Codec name  \t\t\t\t = %s\n", pstAudioStreamInfo->stAudioInfo.szCodecName );
	HxLOG_Debug( "\t[ HxAV ] SampleRate  \t\t\t\t = %d\n", pstAudioStreamInfo->stAudioInfo.nSampleRate );
	HxLOG_Debug( "\t[ HxAV ] Bit Rate    \t\t\t\t = %d\n", pstAudioStreamInfo->stAudioInfo.nBitRate );
	HxLOG_Debug( "\t[ HxAV ] nChannels   \t\t\t\t = %d\n", pstAudioStreamInfo->stAudioInfo.nChannels );
	HxLOG_Debug( "\t[ HxAV ] --------------------------------------------------\n" );
	HxLOG_Debug( "[ HxAV ] ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n" );
#endif

	hlib_av_Close(ffmpeg, pstAVInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return ERR_OK;

err_rtn:
	if (pstAVInfo)
		hlib_av_Close(ffmpeg, pstAVInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return ERR_FAIL;
}


//
HxAV_ImageInfo_t*		HLIB_AV_GetVideoThumbnail(const HCHAR *pszFileName,
												HxAV_SteamInfo_t *pstStreamInfo,
												HUINT32 ulTime,
												HBOOL (*recalc_size)(HINT32 *pnDstWidth, HINT32 *pnDstHeight, HINT32 nSrcWidth, HINT32 nSrcHeight),
												HINT32 (*interrupt_callback)(void*),
												void *interrupt_userdata)
{
	ffmpeg_init_t		*ffmpeg = ffmpeg_getInstance();
	HxAV_t				*avInfo;
	HBOOL				bIsDir = FALSE;
	HBOOL				bIsValid = FALSE;
	HINT32				nNumByte, nDuration, nWidth, nHeight;
	AVFrame				*pstFrameRGB = NULL;
	HxAV_ImageInfo_t	*pstBuffer = NULL;
	struct	SwsContext	*cvtCtx;

	//	ffmpeg thumbnail추출 우선 막음.
	if (ffmpeg == NULL)
		return NULL;

	// vaild check
	bIsValid = HLIB_DIR_IsExist((HCHAR *)pszFileName);
	if(bIsValid == FALSE)
 		return NULL;

	// dir인 경우에는 thumbnail 하지 말자~
	bIsDir = HLIB_DIR_IsDirPath((HCHAR *)pszFileName);
	if(bIsDir == TRUE)
 		return NULL;

	avInfo = hlib_av_Open(ffmpeg, pszFileName, interrupt_callback, interrupt_userdata, FALSE);
	RETURN_IF_GOTO(avInfo == NULL, err_rtn);

	RETURN_IF_GOTO(!avInfo->pstVideoCodecContext, err_rtn);
	RETURN_IF_GOTO(!avInfo->pstVideoCodecContext->codec, err_rtn);
	RETURN_IF_GOTO(!avInfo->pstVideoStream, err_rtn);
	RETURN_IF_GOTO(!avInfo->pstVideoCodec, err_rtn);

	if (pstStreamInfo)
	{
		HLIB_STD_StrNCpySafe(pstStreamInfo->szCodecName, avInfo->pstVideoCodec->name, 32);
		pstStreamInfo->nWidth = avInfo->pstVideoCodecContext->width;
		pstStreamInfo->nHeight = avInfo->pstVideoCodecContext->height;
		nDuration = (HINT32)(avInfo->fmt_ctx->duration / AV_TIME_BASE);
		pstStreamInfo->nDuration = nDuration < 0 ? -1 : nDuration;
		pstStreamInfo->llFrameCount = avInfo->pstVideoStream->nb_frames;
		pstStreamInfo->nStreamType = avInfo->pstVideoCodec->type;
	}

	if(FALSE == hlib_av_DecodeVideoFrame(ffmpeg, avInfo))
	{
		HxLOG_Debug("hlib_av_DecodeVideoFrame - fail\n", __FUNCTION__);
		goto err_rtn;
	}

	HxLOG_Debug("hlib_av_DecodeVideoFrame - success\n", __FUNCTION__);
	if (hlib_av_SeekVideoStream(ffmpeg, avInfo, ulTime) != ERR_OK)
	{
		goto err_rtn;
	}
	if (avInfo->pstFrame->interlaced_frame)
	{
		ffmpeg->fn_avpicture_deinterlace((AVPicture*)avInfo->pstFrame, (AVPicture*)avInfo->pstFrame,
			avInfo->pstVideoCodecContext->pix_fmt, avInfo->pstVideoCodecContext->width, avInfo->pstVideoCodecContext->height);
	}

	if (recalc_size)
	{
		if (recalc_size(&nWidth, &nHeight, avInfo->pstVideoCodecContext->width, avInfo->pstVideoCodecContext->height) == FALSE)
		{
			nWidth = avInfo->pstVideoCodecContext->width;
			nHeight = avInfo->pstVideoCodecContext->height;
		}
	}
	else
	{
		nWidth = avInfo->pstVideoCodecContext->width;
		nHeight = avInfo->pstVideoCodecContext->height;
	}

	pstFrameRGB = ffmpeg->fn_avcodec_alloc_frame();
	nNumByte = ffmpeg->fn_avpicture_get_size(PIX_FMT_RGB24, nWidth, nHeight);
	pstBuffer = HLIB_MEM_Malloc(sizeof(HxAV_ImageInfo_t) + nNumByte * sizeof(HUINT8));
	pstBuffer->nWidth = nWidth;
	pstBuffer->nHeight = nHeight;
	ffmpeg->fn_avpicture_fill((AVPicture*)pstFrameRGB, pstBuffer->data, PIX_FMT_RGB24, nWidth, nHeight);

#if	1
	cvtCtx = ffmpeg->fn_sws_getContext(avInfo->pstVideoCodecContext->width, avInfo->pstVideoCodecContext->height, avInfo->pstVideoCodecContext->pix_fmt,
						nWidth, nHeight,
						PIX_FMT_RGB24,
						/*SWS_BICUBIC*/SWS_FAST_BILINEAR, NULL, NULL, NULL);
	ffmpeg->fn_sws_scale(cvtCtx, (const uint8_t * const*)avInfo->pstFrame->data, avInfo->pstFrame->linesize, 0, avInfo->pstVideoCodecContext->height,
			pstFrameRGB->data, pstFrameRGB->linesize);
	ffmpeg->fn_sws_freeContext(cvtCtx);
#endif

	if (pstFrameRGB)
		ffmpeg->fn_avcodec_free_frame(&pstFrameRGB);

	hlib_av_Close(ffmpeg, avInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return pstBuffer;

err_rtn:

	if (pstBuffer)
		HLIB_MEM_Free(pstBuffer);

	if (avInfo)
		hlib_av_Close(ffmpeg, avInfo);

	HxSEMT_Release(ffmpeg->ulSemId);

	return NULL;
}

void	HLIB_AV_FreeImageInfo(HxAV_ImageInfo_t *pstImageInfo)
{
	HLIB_MEM_Free(pstImageInfo);
}

