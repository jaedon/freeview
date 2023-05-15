/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "VideoESPlayerNative.h"
#include "MediaUtils.h"
#include "ESPlayerConstants.h"
#include "ReferenceClockNative.h"
#include "DecoderLock.h"
#include "DecoderStatsCounter.h"
#include "VideoDecoderNative.h"
#include "NaluHelper.h"
#include "NativeBridge.h"

#include <nrdbase/Log.h>
#include <cstring>
#include <fcntl.h>

#include <nrd/NrdApplication.h>
#include <nrdbase/ScopedMutex.h>
#include <nrddpi/Common.h>

#include "cso_def.h"
#include "cso_uva.h"

using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace netflix;
using namespace std;

#define FF_INPUT_BUFFER_PADDING_SIZE (16)    /* from avcodec.h */
#if 0 /* dragon */
void ffmpeg_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    if(level > ESPlayerConstants::CODEC_LOG_LEVEL)
        return;

    // If the message is just info or something less serious -- which in ffmpeg's
    // error reporting corresponds to a higher level -- just send it to the
    // MEDIAPLAYBACK trace.  If the level is more serious, a warning, error, or fatal, log it
    // via Log::error so that it gets sent to log blobs.
    static const int buffsize = 4096;
    char buffer[buffsize];
    NRDP_UNUSED(ptr);

    vsnprintf(buffer, buffsize, fmt, vl);
    if(level < AV_LOG_INFO)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "[ffmpeg] level: %d, msg: %s", level, buffer);

/*
        // We use the contexts opaque field as a mechanism to keep track of the number of errors reported by ffmpeg.
        if(ptr && ((AVCodecContext*)ptr)->opaque)
        {
            ((DecoderStatsCounter*)(((AVCodecContext*)ptr)->opaque))->decodingError();
        }
*/
    } else
    {
        NTRACE(TRACE_MEDIAPLAYBACK, "[ffmpeg] level: %d, msg: %s", level, buffer);
    }
}
#endif

#if 0
VideoDecoderNative::VideoDecoderNative(const VideoAttributes& attributes,
                                       ReferenceClockNative& referenceClock,
                                       const VideoRenderer::VideoRendererInfo& rendererInfo,
                                       uint32_t pipelineId,
                                       bool& error)
  : VideoDecoder(rendererInfo, pipelineId),
    referenceClock_(referenceClock),
  #if 0 /* dragon */
    codecContext_(NULL),
    decodedFrame_(NULL),
    imageConvertContext_(NULL),
  #endif
    videoDroppedFrameCountDec_(0),
    consecutiveDroppedFrameCount_(0),
    dropUntilNextGOP_(false),
    mAttributes(attributes)
#else
VideoDecoderNative::VideoDecoderNative(const VideoAttributes& attributes,
                                       uint32_t pipelineId,
                                       bool& error)
  : VideoDecoder(pipelineId),
  #if 0 /* dragon */
    codecContext_(NULL),
    decodedFrame_(NULL),
    imageConvertContext_(NULL),
  #endif
    videoDroppedFrameCountDec_(0),
    consecutiveDroppedFrameCount_(0),
    dropUntilNextGOP_(false),
    mAttributes(attributes)
#endif
{
    memset(&m_stH264ExtraData, 0x00, sizeof(H264ExtraData_t));

    m_stH264ExtraData.width = attributes.mImageWidth;
    m_stH264ExtraData.height = attributes.mImageHeight;
    if (attributes.mCodecSpecificData.size())
    {
        m_stH264ExtraData.extradata_size = attributes.mCodecSpecificDataLen[0];
    }
    
    if(m_stH264ExtraData.extradata_size > 0)
    {
        m_stH264ExtraData.extradata = new unsigned char[m_stH264ExtraData.extradata_size + FF_INPUT_BUFFER_PADDING_SIZE];
        memcpy(m_stH264ExtraData.extradata, attributes.mCodecSpecificData[0], m_stH264ExtraData.extradata_size);
    }

#if 0 /* dragon */
    // ffmpeg error report - only for VideoDecoderNative that use ffmpeg
    // decodeer
    ESPlayerConstants::CODEC_LOG_LEVEL = AV_LOG_INFO;
    av_log_set_callback(ffmpeg_log_callback);

    // Register selected video decoders from FFMPEG
    avcodec_register_all();

    currentPixelAspectRatioX_ = mAttributes.mPixelAspectRatioX;
    currentPixelAspectRatioY_ = mAttributes.mPixelAspectRatioY;

    // Initialize AVCodecContext according to the video attributes
    //
    codecContext_ = avcodec_alloc_context3(NULL);
    if(codecContext_ == NULL){
        cleanUp();
        Log::error(TRACE_MEDIAPLAYBACK, "[%d] video decoder avcodec_alloc_context3 failed", mPipelineId);
        error = true;
        return;
    }
    codecContext_->codec_type      = AVMEDIA_TYPE_VIDEO;
    codecContext_->codec_tag       = MediaUtils::getFourCCFromCodecParam(attributes.mCodecParam);
    codecContext_->width           = attributes.mImageWidth;
    codecContext_->height          = attributes.mImageHeight;
    if(attributes.mCodecSpecificDataLen.size()){
        codecContext_->extradata_size  = attributes.mCodecSpecificDataLen[0];
    }
    codecContext_->opaque = &statsCounter_; // We attach our stats counter to the
                                            // codec context here. When ffmpeg's
                                            // error reporting callback gets
                                            // called, the codecContext is
                                            // passed in as one of the
                                            // arguments.  This allows us to use
                                            // the stats counter to count the
                                            // error. The opaque field is
                                            // defined to be user-specific, so
                                            // it's kosher for us to do this.

    if(codecContext_->extradata_size > 0)
    {
        codecContext_->extradata = new unsigned char[codecContext_->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE];
        memcpy(codecContext_->extradata,
               attributes.mCodecSpecificData[0],
               codecContext_->extradata_size);
    }

    switch(codecContext_->codec_tag)
    {
    case _FOURCC_LE_('W','M','V','3'): codecContext_->codec_id = CODEC_ID_WMV3;  break;
    case _FOURCC_LE_('W','V','C','1'): codecContext_->codec_id = CODEC_ID_VC1;   break;
    case _FOURCC_LE_('W','M','V','A'): codecContext_->codec_id = CODEC_ID_VC1;   break;
    case _FOURCC_LE_('M','P','4','V'): codecContext_->codec_id = CODEC_ID_MPEG4; break;
    case _FOURCC_LE_('a','v','c','1'): codecContext_->codec_id = CODEC_ID_H264;  break;
    default:                           codecContext_->codec_id = CODEC_ID_NONE;
    }

    // Locate registered FFMPEG video decoder (according to codec_id)
    // and open (init) the decoder instance.

    if(codecContext_->codec_id != CODEC_ID_NONE)
    {
        AVCodec* vcc = avcodec_find_decoder(codecContext_->codec_id);
        if(vcc == NULL || avcodec_open2(codecContext_, vcc, NULL) < 0)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "video decoder failed to open");
            codecContext_->codec_id = CODEC_ID_NONE;
        }
    }

    if(codecContext_->codec_id == CODEC_ID_NONE) {
        cleanUp();
        Log::error(TRACE_MEDIAPLAYBACK, "[%d] video decoder is not properly initialized", mPipelineId);
        error = true;
        return;
    }

    // Initialize AVFrame
    //
    decodedFrame_ = avcodec_alloc_frame();
    if (!decodedFrame_){
        Log::error(TRACE_MEDIAPLAYBACK, "[%d] can't allocate decodedFrame_ buffer", mPipelineId);
        error=true;
        return;
    }

    // Initialize AVPacket
    av_init_packet(&avpkt_);

#if defined(DBG_FILEDUMP)
    // Open file for dump
    fp_ = fopen("videodump.264", "wb");
#endif
    error = false;
#endif
    error = false;
}

VideoDecoderNative::~VideoDecoderNative()
{
#if defined(DBG_FILEDUMP)
    fclose(fp_);
#endif
    cleanUp();
}

// *NOTE* Workaround for H.264 seamless video stream switch.
// Even though the FFMPEG codec seems to handle SPS/PPS update properly,
// there is no way to find out the actual dimension of the decoded picture
// from the output AVFrame data structure. Therefore, we still need to
// push out the last reference frame from the decoder, and restart the
// decoder, as well as image scaler, to prepare for the new stream.

// Returns whether you should reset the codec.
bool VideoDecoderNative::getUpdatedH264ExtraData(
    unsigned char*& new_extradata, int& new_extradata_size,
    const vector<const uint8_t*>& paramSets, const vector<uint32_t>& paramSetLens,
    int& new_width, int& new_height) const
{
    bool resetCodec = false;

    // It is assumed that: whenever the AU_KEY_FRAME flag is set (i.e., we
    // are at the beginning of GOP), the first two NAL units of the access-unit
    // data will carry SPS and PPS. There should be no SPS or PPS NAL units after
    // the first two.
    //
    // As a result, we can compare the stored SPS/PPS (in codecContext_->extradata)
    // with the content of the first two NAL units to find out if codec reset is needed.


    if( (paramSets.size() > 0) &&
        (paramSets.size() == paramSetLens.size()))
    {
        if((static_cast<uint32_t>(m_stH264ExtraData.extradata_size) != paramSetLens[0])
            || (memcmp(paramSets[0], (uint8_t *)m_stH264ExtraData.extradata, paramSetLens[0])
            != 0))
        {
            resetCodec = true;

            new_extradata_size = paramSetLens[0];
            new_extradata = new unsigned char[new_extradata_size];

            memcpy(new_extradata, paramSets[0], paramSetLens[0]);
            new_width = m_stH264ExtraData.width;
            new_height = m_stH264ExtraData.height;
        }
#if 0
        if((static_cast<uint32_t>(codecContext_->extradata_size) != paramSetLens[0]) ||
           (memcmp(paramSets[0], (uint8_t*)codecContext_->extradata,
                   paramSetLens[0]) != 0))
        {
            resetCodec = true;

            new_extradata_size = paramSetLens[0];
            new_extradata = new unsigned char[new_extradata_size];

            memcpy(new_extradata, paramSets[0], paramSetLens[0]);

            new_width  = codecContext_->width;  // the FFMPEG H.264 decoder will adjust it when reopened.
            new_height = codecContext_->height; // the FFMPEG H.264 decoder will adjust it when reopened.
        }
#endif
    }

    return resetCodec;
}


bool VideoDecoderNative::checkIfShouldDropFrame(const int64_t& PTS, const int64_t& currPresentationTime,
                                                const bool dropable) const
{
    bool dropFrame = false;
#if 0 /* dragon */
    if(dropable && PTS != INVALID_TIMESTAMP)
    {
        if(currPresentationTime != INVALID_TIMESTAMP &&
           currPresentationTime > PTS + ESPlayerConstants::VIDEO_DROP_THRESHOLD_DEC)
        {
            if(consecutiveDroppedFrameCount_ < ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_DEC)
            {
                dropFrame = true;
            }
        }
    }
#endif
    return dropFrame;
}

 // Drop the rest of the GOP if the decoder is really far behind.
bool VideoDecoderNative::checkIfShouldDropUntilNextGOP(const ISampleAttributes *pSampleAttr,
                                                       const int64_t& currPresentationTime)
{
#if 0 /* dragon */
    if(pSampleAttr->isKeyFrame())
    {
        dropUntilNextGOP_ = false;
    }

    if(currPresentationTime != INVALID_TIMESTAMP &&
       currPresentationTime > pSampleAttr->getPTS() + ESPlayerConstants::VIDEO_DROP_THRESHOLD_WHOLE_GOP)
    {
        dropUntilNextGOP_ = true;
    }
#endif
    return dropUntilNextGOP_;
}

bool VideoDecoderNative::resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                                        const int& new_extradata_size,
                                                        const int& new_width,
                                                        const int& new_height)
{
    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] VideoDecoderNative::%s", mPipelineId, __func__);
#if 0 /* dragon */
    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());
    unsigned int old_codec_tag = codecContext_->codec_tag;
    AVCodecID old_codec_id  = codecContext_->codec_id;
    AVCodec* vcc = avcodec_find_decoder(old_codec_id);
    if (vcc == NULL){
        Log::error(TRACE_MEDIAPLAYBACK,
                   "[%d] VideoDecoderNative::%s avcodec_find_decoder failed", mPipelineId, __func__);
    }

    int ret = 0;
    ret = avcodec_close(codecContext_);
    if( ret<0 ){
        Log::error(TRACE_MEDIAPLAYBACK,
                   "[%d] VideoDecoderNative::%s avcodec_close failed %d", mPipelineId, __func__, ret);
        return false;
    }

    if(codecContext_->extradata_size > 0){
        delete [] codecContext_->extradata;
    }

    av_free(codecContext_);
    codecContext_ = NULL;
    codecContext_ = avcodec_alloc_context3(NULL);
    codecContext_->codec_type      = AVMEDIA_TYPE_VIDEO;
    codecContext_->codec_tag       = old_codec_tag;
    codecContext_->width           = new_width;
    codecContext_->height          = new_height;
    codecContext_->extradata_size  = new_extradata_size;
    codecContext_->extradata       = new_extradata;
    codecContext_->codec_id        = old_codec_id;

    ret = avcodec_open2(codecContext_, vcc, NULL);
    if ( ret< 0 ){
        Log::error(TRACE_MEDIAPLAYBACK,
                   "[%d] VideoDecoderNative::%s avcodec_open2 failed %d", mPipelineId, __func__, ret);
        codecContext_->codec_id = CODEC_ID_NONE;
        return false;
    }
    resetTimestampQueue();

    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] new stream width %d, height %d, extradata size %d "
           , mPipelineId, new_width, new_height, new_extradata_size);
#endif
    return true;
}

ESNativeError VideoDecoderNative::decode(const ISampleAttributes *pSampleAttr,
                                                     const unsigned char* auData, uint32_t auDataSize,
                                                     UVAWriteFrame *pstUvaWriteFrame, int32_t nCsoVideoCodec, 
                                                     bool& gotPicture, bool& resend,
                                                     Format3D& format3D)
{
   // flush processing
    if( (pSampleAttr == NULL) && (auData == NULL) && (auDataSize = 0) ){
        // not necessary for ffmpeg
        return skeletonDeviceSpecific_NoError;
    }

    bool resetCodec = false, dropable = false, dropFrame = false;
    unsigned char* new_extradata = 0;
    int new_extradata_size = 0, new_width = 0, new_height = 0;
#if 0
    int64_t currPresentationTime = referenceClock_.getCurrPresentationTime();
#endif
    //assert(pSampleAttr == NULL || (pSampleAttr->getTimescale() == DeviceConstants::DEFAULT_TIMESCALE));
    NRDP_UNUSED(format3D);


    int64_t PTS = INVALID_TIMESTAMP;
    bool keyFrame = false;
    (void)keyFrame;

    // Interpret a NULL pSampleAttr as an end of stream signal. A zero-length
    // auData will request the avcodec to push out the last picture.
    if(pSampleAttr != NULL)
    {
        PTS = pSampleAttr->getPTS();
#if 0
        if(codecContext_->codec_id == CODEC_ID_H264)
#else
        if(nCsoVideoCodec == CSO_VCODEC_H264)
#endif
        {
            if(pSampleAttr->isKeyFrame())
            {
                keyFrame = true;
                VideoAttributes* attr = NULL;

                if(pSampleAttr->getMediaAttributes() == NULL)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "[%d] VideoDecoderNative: isKeyFrame but "
                               "getMediaAttributes() returns NULL", mPipelineId);
                    assert(0);
                    return skeletonDeviceSpecific_MissingSampleAttributes;
                }

                if((attr = pSampleAttr->getMediaAttributes()->mVideoAttributes) == NULL)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "[%d] VideoDecoderNative: isKeyFrame but "
                               "getMediaAttributes()->mVideoAttributes is NULL", mPipelineId);
                    assert(0);
                    return skeletonDeviceSpecific_MissingSampleAttributes;
                }

                // update to mAttribute is needed because we want to know how
                // the data looks like when passed to decoder.
                mAttributes.mImageWidth = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageWidth;
                mAttributes.mImageHeight = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageHeight;
                mAttributes.mPixelAspectRatioX
                    =  pSampleAttr->getMediaAttributes()->mVideoAttributes->mPixelAspectRatioX;
                mAttributes.mPixelAspectRatioY
                    =  pSampleAttr->getMediaAttributes()->mVideoAttributes->mPixelAspectRatioY;

#if 1 /* These are unnecessary because the playback checking for SD contents under HDCP:on will be based on maxImageSize */
                (void)NativeBridge::playbackGroup().notifyStreamCurrentSize(mAttributes.mImageWidth, mAttributes.mImageHeight);
                NTRACE(TRACE_MEDIAPLAYBACK, "VideoDecoderNative--mAttributes(mImageWidth:%d, mImageHeight:%d)\n", mAttributes.mImageWidth, mAttributes.mImageHeight);
                
                (void)NativeBridge::playbackGroup().notifyStreamCurrentPAR(mAttributes.mPixelAspectRatioX, mAttributes.mPixelAspectRatioY);
#endif

                resetCodec = getUpdatedH264ExtraData(new_extradata, new_extradata_size,
                                                     attr->mCodecSpecificData,
                                                     attr->mCodecSpecificDataLen,
                                                     new_width,
                                                     new_height);
                if(resetCodec)
                {
                    Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoDecoderNative::%s:%d] resetCodec!!\n", __func__, __LINE__);
                    auData = NULL;
                    auDataSize = 0;

                    m_stH264ExtraData.extradata_size = new_extradata_size;
                    m_stH264ExtraData.extradata = new_extradata;
                }
            }

#if 0
            dropable = !pSampleAttr->isDependedOn();
            dropFrame = checkIfShouldDropFrame(PTS, currPresentationTime, dropable);
#endif
        }
#if 0
        dropFrame |= checkIfShouldDropUntilNextGOP(pSampleAttr, currPresentationTime);
#endif
#if 1
        /* Set the UVA Frame Structure */
        if(pstUvaWriteFrame != NULL)
        {
            pstUvaWriteFrame->reset();

            pstUvaWriteFrame->pts = PTS;
            pstUvaWriteFrame->dts = pSampleAttr->getDTS();
            pstUvaWriteFrame->duration = 0.0;    /* There is no way to get a duration at this function */
            pstUvaWriteFrame->data_size = auDataSize;
            pstUvaWriteFrame->data = (void *)auData;
            pstUvaWriteFrame->ext_size = m_stH264ExtraData.extradata_size;
            pstUvaWriteFrame->ext_data = (void *)m_stH264ExtraData.extradata;
        }
        else
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [VideoDecoderNative::%s:%d] pstUvaWriteFrame is NULL\n", __func__, __LINE__);
        }
#endif
    }
    int ret = 0;
#if 0
    if(!dropFrame)
    {
        int got_picture = 0;

        // populate the AVPacket
        avpkt_.data = const_cast<unsigned char*>(auData);
        avpkt_.size = auDataSize;

#if defined(DBG_FILEDUMP)
        fwrite(static_cast<const void*>(auData), sizeof(char), auDataSize, fp_);
#endif
#ifdef NRDP_HAS_TRACING
        static uint64_t prevPTS;
        NTRACE(TRACE_MEDIAPLAYBACK, "[%d] avcodec_decode_video2 au PTS %lld\t(diff %lld)\t size %u\t %s"
               , mPipelineId, PTS, PTS - prevPTS, auDataSize, keyFrame?"key":"");
        prevPTS = PTS;
#endif
        ret = avcodec_decode_video2(codecContext_, decodedFrame_, &got_picture, &avpkt_);
        if(ret >= 0)
        {
            statsCounter_.frameDecoded();
            // enque PTS, instead of directly attaching it to framebuffer here
            // since frame with this PTS will come back later with got_picture
            // set
            enqueueTimestamp(PTS);

            gotPicture = (got_picture != 0);
            if(gotPicture){
                imageConvert(*decodedFrame_, frameBuffer);
                // dequeue PTS and set it on this frame.
                frameBuffer.setTimestamp(dequeueTimestamp());
            }
        } else {
            Log::warn(TRACE_MEDIAPLAYBACK, "[%d] avcodec_decode_video2 return error auSize %d for PTS %lld isKeyFrame=%d",
                      mPipelineId, auDataSize, PTS, pSampleAttr->isKeyFrame());
            h264NaluHelper h;
            h.print(auData, auDataSize);
            h.dump(auData, auDataSize, "au_dump.h264");
            assert(0);
        }
        consecutiveDroppedFrameCount_ = 0;
    } else {
        gotPicture = false;
        consecutiveDroppedFrameCount_++;
        videoDroppedFrameCountDec_ ++;
        resend = false;
        statsCounter_.frameDropped();
        NTRACE(TRACE_MEDIAPLAYBACK,
               "[%d] Video Decoder drops frame at decode%s: dts=%lld currPresentationTime=%lld (diff: %lld)\n",
               mPipelineId,
               dropUntilNextGOP_ ? " (dropping remainder of GOP)" : "",
               PTS, currPresentationTime,
               currPresentationTime - PTS);
    }

    if(resetCodec)
    {
        if(!resetCodecWithUpdatedExtraData(new_extradata, new_extradata_size,
                                           new_width, new_height)){
            return skeletonDeviceSpecific_FfmpegDecoderError;
        }
    }
#endif

    resend = resetCodec;
    if(ret < 0)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "[%d] %s avcodec_decode_video2 failed %d: set gotPicture false\n", mPipelineId, __func__, ret);
        assert(0);
        gotPicture = false;
    }
    return skeletonDeviceSpecific_NoError;
}

void VideoDecoderNative::reset()
{
#if 0 /* dragon */
    // "avcodec_flush_buffers()" does not work for all avcodec(s), for example, both vc1 and wma
    // decoders do not expose any flush() function pointer. Therefore, it is safer to reset the
    // decoder by closing and re-opening it, instead of using avcodec_flush_buffers().

    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());

    if (codecContext_->codec_id != CODEC_ID_NONE) {
        avcodec_flush_buffers(codecContext_);
    }

    resetTimestampQueue();
    consecutiveDroppedFrameCount_ = 0;
    dropUntilNextGOP_ = false;
#endif
}

void VideoDecoderNative::imageConvert(
    AVFrame& decodedFrame)
{
#if 0 /* dragon */
    PixelFormat dstPixelFormat;
    unsigned char* dstPlanes[4];
    int dstPitches[4];

    switch(rendererInfo_.outputPixelFormat_)
    {
    case _FOURCC_LE_('Y','V','1','2'):

        dstPixelFormat = PIX_FMT_YUV420P;

        dstPlanes[0] = frameBuffer.plane(0); // Y
        dstPlanes[1] = frameBuffer.plane(2); // U
        dstPlanes[2] = frameBuffer.plane(1); // V
        dstPlanes[3] = NULL;

        dstPitches[0] = frameBuffer.pitch(0); // Y
        dstPitches[1] = frameBuffer.pitch(2); // U
        dstPitches[2] = frameBuffer.pitch(1); // V
        dstPitches[3] = 0;
        break;

    case _FOURCC_LE_('R','G','B',' '):

        dstPixelFormat = PIX_FMT_RGB32;

        dstPlanes[0] = frameBuffer.plane(0); // RGB
        dstPlanes[1] = NULL;
        dstPlanes[2] = NULL;
        dstPlanes[3] = NULL;

        dstPitches[0] = frameBuffer.pitch(0); // RGB
        dstPitches[1] = 0;
        dstPitches[2] = 0;
        dstPitches[3] = 0;
        break;

    default:
        dstPixelFormat = PIX_FMT_RGB32;
        assert(false); // unexpected output pixel format
    }

    uint32_t srcWidth, srcHeight;
    uint32_t dstWidth, dstHeight;
    uint32_t dstRectX, dstRectY, dstRectWidth, dstRectHeight;

    // Software Scaling here
    srcWidth  = decodedFrame.width;
    srcHeight = decodedFrame.height;
    dstWidth  = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    dstHeight = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

    if( srcWidth == mAttributes.mImageWidth && srcHeight == mAttributes.mImageHeight ){
        // if decoded frame's width/height are different from what was fed into decoder,
        // we are still handling previous GOP's data
        currentPixelAspectRatioX_ =  mAttributes.mPixelAspectRatioX;
        currentPixelAspectRatioY_ =  mAttributes.mPixelAspectRatioY;
    }

    VideoRenderer::calcDstRectOnScreen(currentPixelAspectRatioX_,
                                       currentPixelAspectRatioY_,
                                       ESPlayerConstants::VIDEO_REN_SCREEN_PAR_X,
                                       ESPlayerConstants::VIDEO_REN_SCREEN_PAR_Y,
                                       srcWidth,
                                       srcHeight,
                                       ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH,
                                       ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT,
                                       dstRectX,
                                       dstRectY,
                                       dstRectWidth,
                                       dstRectHeight);

    dstWidth = dstRectWidth;
    dstHeight = dstRectHeight;

    /*
    printf("SCALER PAR %d/%d src %d/%d, dstXY %d/%d, dstWH %d/%d\n",
           currentPixelAspectRatioX_, currentPixelAspectRatioY_,
           srcWidth, srcHeight,
           dstRectX, dstRectY, dstRectWidth, dstRectHeight);
    */

    imageConvertContext_ = sws_getCachedContext(imageConvertContext_,
                                                srcWidth,
                                                srcHeight,
                                                codecContext_->pix_fmt,
                                                dstWidth,
                                                dstHeight,
                                                dstPixelFormat,
                                                SWS_BICUBIC, NULL, NULL, NULL);

    sws_scale(imageConvertContext_,
              decodedFrame.data,
              decodedFrame.linesize,
              0,
              srcHeight,
              dstPlanes,
              dstPitches);

    frameBuffer.setWidth(dstWidth);
    frameBuffer.setHeight(dstHeight);
#endif
}

void VideoDecoderNative::cleanUp()
{
    if(m_stH264ExtraData.extradata_size > 0)
    {
        if(m_stH264ExtraData.extradata != NULL)
        {
            delete [] m_stH264ExtraData.extradata;
            m_stH264ExtraData.extradata = NULL;
        }    
    }

#if 0 /* dragon */
    if(codecContext_ != NULL)
    {
        if(codecContext_->codec != NULL)
            avcodec_close(codecContext_);

        if(codecContext_->extradata_size > 0)
            delete [] codecContext_->extradata;

        av_free(codecContext_);

        codecContext_ = NULL;
    }

    if(decodedFrame_ != NULL)
        avcodec_free_frame(&decodedFrame_);
    decodedFrame_ = NULL;

    if(imageConvertContext_ != NULL)
        sws_freeContext(imageConvertContext_);
    imageConvertContext_ = NULL;
#endif
}
