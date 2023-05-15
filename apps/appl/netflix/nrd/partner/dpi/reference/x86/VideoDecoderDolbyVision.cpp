/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifdef _FORTIFY_SOURCE
#undef _FORTIFY_SOURCE
#endif
#define _FORTIFY_SOURCE 0

#include "../VideoESPlayerNative.h"
#include "../MediaUtils.h"
#include "../VideoRenderer.h"
#include "../ESPlayerConstants.h"
#include "../ReferenceClockNative.h"
#include "../FileSystem.h"
#include "VideoDecoderDolbyVision.h"

#include <nrdbase/Log.h>
#include <cstring>
#include <string.h> //memset
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include <nrd/NrdApplication.h>
#include <nrdbase/ScopedMutex.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"

/*
 * for Vanguard decoder
 */
#pragma GCC diagnostic ignored "-Wunused-function"

#include <hevc.h>
#include <hevc/hevc_dec_api.h>
#include <hevc/hevc_videoframe_api.h>

extern "C"
{
#include <libswscale/swscale.h>
#include <emmintrin.h>
#include <mmintrin.h>
}

//#define FILE_DUMP 1
//#define HEVC_DEBUG 1
#ifndef HEVC_DEBUG
#define PRINT(fmt, ...)
#else
#include <stdio.h>
#define PRINT(fmt, ...) printf("[HEVC_DEBUG] "fmt, __VA_ARGS__ )
#endif



using namespace netflix::device::esplayer;
using namespace std;
using namespace netflix;

VideoDecoderDolbyVision::VideoDecoderDolbyVision(const VideoAttributes& attributes,
                                                 ReferenceClockNative& referenceClock,
                                                 const VideoRenderer::VideoRendererInfo& rendererInfo,
                                                 uint32_t pipelineId,
                                                 bool& error)
  : VideoDecoder(rendererInfo, pipelineId),
    referenceClock_(referenceClock),
    videoDroppedFrameCountDec_(0),
    consecutiveDroppedFrameCount_(0),
    dropUntilNextGOP_(false),
    mAttributes(attributes),
    timestamp_(0),
    frameFeedCounter_(0),
    frameReadCounter_(0),
    imageConvertContext_(NULL),
    enablePrintDTS_(false),
    initialBitsDepth_(0),
    currentBitsDepth_(0),
    fpDolbyFile(NULL)
{
    PRINT("VideoDecoderDolbyVision::%s\n", __func__ );

    currentPixelAspectRatioX_ = mAttributes.mPixelAspectRatioX;
    currentPixelAspectRatioY_ = mAttributes.mPixelAspectRatioY;

    // disable ffmpeg error report
    av_log_set_callback(NULL);

    init_buffer(3840, 2160);
#if defined(FILE_DUMP)
    fp_ = fopen("raw_capture.265", "wb");
#endif

    // for QA test convienience
    char* printDTS = getenv("NF_VERBOSE_DTS");
    if (printDTS != NULL) {
        enablePrintDTS_ = true;
    }

    hevc_error_t r;
    memset(&decoderSettings_, 0 , sizeof(decoderSettings_));
    decoderSettings_.size = sizeof(hevcd_settings_t);
    hevcd_default_settings(&decoderSettings_);

    decoderSettings_.mt.disable = false;
#if 0
     // <0 is 'calculate automatically'.  It seems to use a huge amount.
    decoderSettings_.mt.num_threads = -1;
#else
    decoderSettings_.mt.num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    decoderSettings_.mt.num_frames = 4;
    decoderSettings_.mt.num_wf_lines = 4;
#endif

    r = hevcd_check_settings(&decoderSettings_);
    if (r != HEVC_OK) {
        Log::error(TRACE_MEDIAPLAYBACK, "Error in settings#%d: %s\n", r, hevc_error_text(r));
        error = true;
    }

    r = hevcd_open(&decoderHandle_, &decoderSettings_, NULL);
    if (r != HEVC_OK) {
        Log::error(TRACE_MEDIAPLAYBACK, "hevcd_open() FAILED with RC=%d\n", r);
        error = true;
    }

    if ( !FileSystem::sQaBridgeDolbyFileDump.empty() ){
        static const std::string dataDirectory = Configuration::dataReadPath();
        // const time_t currentTime = time(NULL);
        //std::string dateString = ctime(&currentTime);
        mDolbyFileDumpPath = dataDirectory + "/" + FileSystem::sQaBridgeDolbyFileDump + ".265";
        fpDolbyFile = fopen(mDolbyFileDumpPath.c_str(), "wb");
    }
}

VideoDecoderDolbyVision::~VideoDecoderDolbyVision()
{
    PRINT("VideoDecoderDolbyVision::%s\n", __func__ );

    hevc_error_t r;
    r = hevcd_set_flush(decoderHandle_);
    if ( r != HEVC_OK ) {
        Log::error(TRACE_MEDIAPLAYBACK, "hevcd_set_flush failed with %d", r);
        assert(0);
        }

    r = hevcd_close(decoderHandle_);
    if (r == HEVC_OK){
        NTRACE(TRACE_MEDIAPLAYBACK, "hevc_close successful");
    } else {
        Log::error(TRACE_MEDIAPLAYBACK, "hevcd_close failed with %d", r);
        assert(0);
    }

#if defined(FILE_DUMP)
    fclose(fp_);
#endif

    if (fpDolbyFile) {
        fclose(fpDolbyFile);
    }

    cleanup_buffer();

    if (imageConvertContext_ != NULL) {
        sws_freeContext(imageConvertContext_);
    }
    imageConvertContext_ = NULL;
}

bool VideoDecoderDolbyVision::checkIfShouldDropFrame(const int64_t& PTS,
                                              const int64_t& currPresentationTime,
                                              const bool dropable) const
{
    bool dropFrame = false;

    if(dropable && PTS != INVALID_TIMESTAMP)
    {
        if(currPresentationTime != INVALID_TIMESTAMP &&
           currPresentationTime > PTS + ESPlayerConstants::VIDEO_DROP_THRESHOLD_DEC)
        {
            if(consecutiveDroppedFrameCount_ < ESPlayerConstants::MAX_CONSECUTIVE_VIDEO_DROP_DEC)
            {
                PRINT("FRAME_DROP:%s STC %lld, PTS %lld\n", __func__, currPresentationTime, PTS);
                dropFrame = true;
            }
        }
    }
    return dropFrame;
}

 // Drop the rest of the GOP if the decoder is really far behind.
bool VideoDecoderDolbyVision::checkIfShouldDropUntilNextGOP(const ISampleAttributes *pSampleAttr,
                                                     const int64_t& currPresentationTime)
{
    if(pSampleAttr->isKeyFrame())
    {
        dropUntilNextGOP_ = false;
    }

    if(currPresentationTime != INVALID_TIMESTAMP &&
       currPresentationTime > pSampleAttr->getPTS()  + ESPlayerConstants::VIDEO_DROP_THRESHOLD_WHOLE_GOP)
    {
        PRINT("FRAME_DROP:%s STC %lld, PTS %lld - drop until next GOP\n",
              __func__, currPresentationTime, pSampleAttr->getPTS());
        dropUntilNextGOP_ = true;
    }
    return dropUntilNextGOP_;
}

void VideoDecoderDolbyVision::resetCodecWithUpdatedExtraData(unsigned char*& new_extradata,
                                                      const int& new_extradata_size,
                                                      const int& new_width,
                                                      const int& new_height)
{
    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());

    NRDP_UNUSED(new_extradata);
    NRDP_UNUSED(new_extradata_size);
    NRDP_UNUSED(new_width);
    NRDP_UNUSED(new_height);

    resetTimestampQueue();
}

hevc_error_t VideoDecoderDolbyVision::onFrameDecoded(hevc_vframe_descriptor_t * dec_frame)
{
    hevcd_frame_info_t *frame_info = GET_DEC_FRAME_INFO(dec_frame);
    NRDP_UNUSED(frame_info);

    PRINT("FrameNum[%d]\t POC[%d]\t SliceType[%s]\t QP[%d]\t #ofSlices[%d]\t SarW/H[%d/%d]\t Resolution[%4dx%-4d]\t TS[%d]\t luma[%d]\t chroma[%d]\t \n"
          ,frame_info->frame_num
          ,frame_info->poc
          ,frame_info->slice_type == 0 ? "B" : frame_info->slice_type == 1 ? "P" : "I"
          ,frame_info->qp_used
          ,frame_info->num_slices
          ,frame_info->sar_width
          ,frame_info->sar_height
          ,dec_frame->vp_frame.width
          ,dec_frame->vp_frame.height
          ,(int)dec_frame->timestamp
          ,dec_frame->vp_frame.luma_bits
          ,dec_frame->vp_frame.chroma_bits
        );

    PRINT("Y stride %d\t", dec_frame->vp_frame.stride[0]);
    PRINT("U stride %d\t", dec_frame->vp_frame.stride[1]);
    PRINT("V stride %d\n", dec_frame->vp_frame.stride[2]);

    int height = dec_frame->vp_frame.height;
    int width = dec_frame->vp_frame.width;
    int bytes_per_pel = 2; // as of sep release of decoder

    unsigned char *dest;
    unsigned char *pchar;
    int i, stride;

    if (dec_frame->vp_frame.luma_bits == 10) {
        // New ffmpeg supports 10-bits so conversion is not needed.
    } else if (dec_frame->vp_frame.luma_bits == 8) {

        // Y component
        pchar = (unsigned char *)(dec_frame->vp_frame.data[0]);
        stride = dec_frame->vp_frame.stride[0];
        dest = y8_data;
        for(i = 0 ; i<height; i++, pchar += stride, dest +=width ) {
             vector_copy_lowerbyte(dest, pchar, width, bytes_per_pel);
         }

        // Cb component
        pchar = (unsigned char *)(dec_frame->vp_frame.data[1]);
        stride = dec_frame->vp_frame.stride[1];
        dest = cb8_data;
        for(i = 0 ; i<height/2; i++, pchar += stride, dest +=(width/2) ) {
            vector_copy_lowerbyte(dest, pchar, width/2, bytes_per_pel);
        }

        // Cr component
        pchar = (unsigned char *)(dec_frame->vp_frame.data[2]);
        stride = dec_frame->vp_frame.stride[2];
        dest = cr8_data;
        for(i = 0 ; i<height/2; i++, pchar += stride, dest +=(width/2) ) {
            vector_copy_lowerbyte(dest, pchar, width/2, bytes_per_pel);
        }
    }
    return HEVC_OK;
}

/*
 * ********************************************************************
 * TODO : replace Vanguard HEVC decoder with Dolby-Vanguard HDR decoder
 * ********************************************************************
 */

RefAppDeviceSpecificError VideoDecoderDolbyVision::decode(const ISampleAttributes *pSampleAttr,
                                                   const unsigned char* auData,
                                                   uint32_t auDataSize,
                                                   VideoRenderer::VideoFrameBuffer& frameBuffer,
                                                   bool& gotPicture,
                                                   bool& resend,
                                                   Format3D& format3D)
{
    NRDP_UNUSED(format3D);

    bool dropFrame = false;
    bool dropable = false;

    // flush processing
    if( (pSampleAttr == NULL) && (auData == NULL) && (auDataSize == 0)){
        // explicit flush necessary for vanguard dec
        hevc_error_t r;
        r = hevcd_set_flush(decoderHandle_);
        if ( r != HEVC_OK ) {
            Log::error(TRACE_MEDIAPLAYBACK, "hevcd_set_flush failed with %d", r);
            assert(0);
            return refAppDeviceSpecific_HevcDecoderError;
        }

        r = hevcd_close(decoderHandle_);
        if (r != HEVC_OK){
            Log::error(TRACE_MEDIAPLAYBACK, "hevcd_close failed with %d", r);
            assert(0);
            return refAppDeviceSpecific_HevcDecoderError;
        }

        r = hevcd_open(&decoderHandle_, &decoderSettings_, NULL);
        if (r != HEVC_OK) {
            Log::error(TRACE_MEDIAPLAYBACK, "hevcd_open failed with %d", r);
            assert(0);
            return refAppDeviceSpecific_HevcDecoderError;
        }

        NTRACE(TRACE_MEDIAPLAYBACK, "flush hevc codec successful");
        return refAppDeviceSpecific_NoError;
    }

    /*
     * get PTS
     */
    int64_t currPresentationTime = referenceClock_.getCurrPresentationTime();
    int64_t PTS = INVALID_TIMESTAMP;
    int64_t DTS = INVALID_TIMESTAMP;
    if(pSampleAttr != NULL)
    {

        // Interpret a NULL pSampleAttr as an end of stream signal. A zero-length
        // auData will request the avcodec to push out the last picture.
        PTS = pSampleAttr->getPTS();
        DTS = pSampleAttr->getDTS();

        if(pSampleAttr->isKeyFrame()){
            if(!pSampleAttr->getMediaAttributes())
            {
                Log::error(TRACE_MEDIAPLAYBACK, "[%d] VideoDecoderDolbyVision: isKeyFrame but "
                           "getMediaAttributes() returns NULL", mPipelineId);

                return refAppDeviceSpecific_MissingSampleAttributes;
            }
            if( !(pSampleAttr->getMediaAttributes()->mVideoAttributes) )
            {
                Log::error(TRACE_MEDIAPLAYBACK, "[%d] VideoDecoderDolbyVision: isKeyFrame but "
                           "getMediaAttributes()->mVideoAttributes is NULL", mPipelineId);
                return refAppDeviceSpecific_MissingSampleAttributes;
            }

            // We do not update all attributes here. We updated only needed to
            // handle. If more data needed to be updated, update here
            mAttributes.mImageWidth = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageWidth;
            mAttributes.mImageHeight = pSampleAttr->getMediaAttributes()->mVideoAttributes->mImageHeight;
            mAttributes.mPixelAspectRatioX
                =  pSampleAttr->getMediaAttributes()->mVideoAttributes->mPixelAspectRatioX;
            mAttributes.mPixelAspectRatioY
                =  pSampleAttr->getMediaAttributes()->mVideoAttributes->mPixelAspectRatioY;

            if(currentBitsDepth_ == 0 && pSampleAttr->getMediaAttributes()->mVideoAttributes->mBitsDepthLuma )
            {
                // remember first stream's bit depth
                initialBitsDepth_ = pSampleAttr->getMediaAttributes()->mVideoAttributes->mBitsDepthLuma;

            }
            currentBitsDepth_ = pSampleAttr->getMediaAttributes()->mVideoAttributes->mBitsDepthLuma;

            // if bitDepth is different from first stream's bit depth, return stream error
            if( (initialBitsDepth_ != 0) && (initialBitsDepth_ != currentBitsDepth_) )
            {
                NTRACE(TRACE_MEDIAPLAYBACK, "[%d] first stream's BitsDepth is %d, but now BitsDepth is %d",
                       mPipelineId, initialBitsDepth_, currentBitsDepth_);

                return refAppDeviceSpecific_WrongBitDepth;
            }

            for (uint32_t i = 0;
                 i < pSampleAttr->getMediaAttributes()->mVideoAttributes->mCodecSpecificDataLen.size();
                 i++){
                int received = 0;
                hevc_error_t r;
                timestamp_ = static_cast<hevc_timestamp_t> (PTS);
                r = hevcd_feed_data(
                    decoderHandle_,
                    (uint8_t *)(pSampleAttr->getMediaAttributes()->mVideoAttributes->mCodecSpecificData[i]),
                    pSampleAttr->getMediaAttributes()->mVideoAttributes->mCodecSpecificDataLen[i],
                    0, timestamp_, NULL, &received);

                if (r == HEVC_WARN_CANT_RECEIVE_DATA) {
                    if (received != (int)pSampleAttr->getMediaAttributes()->mVideoAttributes->mCodecSpecificDataLen[i]) {
                        Log::error(TRACE_MEDIAPLAYBACK, "[%d] hevcd_feed_data was unable to receive data %d", mPipelineId, r);
                    }
                }

                if(r != HEVC_OK){
                    NTRACE(TRACE_MEDIAPLAYBACK, "[%d] hevcd_feed_data for DolbyVision codecSpecificData returned %d", mPipelineId, r);
                }
            }
        }

        // if we are dumping to file for dolby vision stream verification, we should not drop any single bit.
        if(!fpDolbyFile) {
            bool dropOnEitherLateBLorEL = true;
#if 0
            bool renderBLFrames;
            bool renderELFrames;
            FileSystem::getDolbyVisionRendering(renderBLFrames, renderELFrames);

            if( (renderBLFrames && renderELFrames) || ( !renderBLFrames && !renderELFrames ) ){
                renderBLFrames = true;
                renderELFrames = false;
            }

            if ((!pSampleAttr->isDolbyVisionELFrame() && renderBLFrames) ||
                (pSampleAttr->isDolbyVisionELFrame() && renderELFrames)) {
                dropOnEitherLateBLorEL = false;
            }
#endif
            if (dropOnEitherLateBLorEL) {
                dropable = !pSampleAttr->isDependedOn();
                dropFrame = checkIfShouldDropFrame(PTS, currPresentationTime, dropable);
                dropFrame |= checkIfShouldDropUntilNextGOP(pSampleAttr, currPresentationTime);
            }
        }
    }

    if(enablePrintDTS_){
        NRDP_UNUSED(DTS);
        NTRACE(TRACE_MEDIAPLAYBACK
               , "[%d] VideoDecoderDolbyVision::%s\t dataSize %d\t CurrentTime %lld\t PTS %lld\t DTS %lld DolbyVisionEL %d \t %s"
               , mPipelineId, __func__ , auDataSize, currPresentationTime, PTS, DTS, pSampleAttr->isDolbyVisionELFrame(), dropFrame?"drop":"");
    }

    /*
     * Decode
     */
    if (!dropFrame) {
        hevc_error_t r;
        timestamp_ = static_cast<hevc_timestamp_t> (PTS);

        if(pSampleAttr){
            if(fpDolbyFile){
                if(!pSampleAttr->isDolbyVisionELFrame()){
                    fwrite(auData, 1, auDataSize, fpDolbyFile);
                } else {
                    writeDolbyVisionEL2File(pSampleAttr, auData, auDataSize);
                }
            }

            /*
             * filter BL or EL for debugging with QABridge
             */
            bool renderBLFrames;
            bool renderELFrames;
            FileSystem::getDolbyVisionRendering(renderBLFrames, renderELFrames);

            if( (renderBLFrames && renderELFrames) || ( !renderBLFrames && !renderELFrames ) ){
                renderBLFrames = true;
                renderELFrames = false;
            }

            if(!renderBLFrames){
                if(!pSampleAttr->isDolbyVisionELFrame()){
                    return refAppDeviceSpecific_NoError;
                }
            }
            if(!renderELFrames){
                if(pSampleAttr->isDolbyVisionELFrame()){
                    return refAppDeviceSpecific_NoError;
                }
            }
        }

#if defined(FILE_DUMP)
        fwrite(auData, 1, auDataSize, fp_);
#endif
        int bytesToCopy = auDataSize;
        int received = 0;
        do {
            r = hevcd_feed_data(decoderHandle_, (uint8_t*)&auData[auDataSize - bytesToCopy], bytesToCopy, 2, timestamp_, NULL, &received);
            switch (r) {
            case HEVC_OK:
                // enque PTS, instead of directly attaching it to framebuffer here
                // since frame with this PTS will come back later with got_picture
                // set
                enqueueTimestamp(PTS);
                break;
            case HEVC_WARN_CANT_RECEIVE_DATA:
                if (HEVC_OK != hevcd_wait_for_can_receive(decoderHandle_)) {
                    Log::error(TRACE_MEDIAPLAYBACK, "hevcd_wait_for_can_receive failed");
                }
                bytesToCopy -= received;
                break;
            default:
                Log::error(TRACE_MEDIAPLAYBACK, "hevcd_feed_data unexpectedly returned %d (%s)\n", r, hevc_error_text(r));
                assert(0);
            }
        } while (r != HEVC_OK);
        hevcd_start_decode(decoderHandle_, 1);

        /**
         * Try to take next decoded frame.
         * Pointer returned by this function remains valid until next call to this function.
         * In order to use it for a longer time call hevc_videoframe_addref() function.
         * In such case it must be released explicitly by hevc_videoframe_release() call.
         * @param handle  - decoder instance handle (returned by hevcd_open())
         * @param p_vframe - where to put pointer to output frame
         * @return HEVC_OK=frame is ready;
         *         HEVC_WARN_NOT_READY=frame not ready;
         *         HEVC_WARN_EOS=end of stream (no more frames expected)
         */

        hevc_vframe_descriptor_t *outframe = NULL;
        r = hevcd_get_frame( decoderHandle_, &outframe);
        if ( r == HEVC_OK ){
            PRINT("%s:frame with %llu PTS_DECODED is ready\n", __func__, timestamp_);
            onFrameDecoded(outframe);
            imageConvert(outframe, frameBuffer);
            frameBuffer.setTimestamp(dequeueTimestamp());
            gotPicture = true;
        } else if ( r == HEVC_WARN_NOT_READY ) {
            PRINT("%s:frame is not ready\n", __func__);
            gotPicture = false;
        } else if ( r == HEVC_WARN_EOS ) {
            PRINT("%s:EOS\n", __func__);
            gotPicture = false;
            // EOS processing
        }
        consecutiveDroppedFrameCount_ = 0;
    } else {
        gotPicture = false;
        consecutiveDroppedFrameCount_++;
        videoDroppedFrameCountDec_++;
        resend = false;
        statsCounter_.frameDropped();
        NTRACE(TRACE_MEDIAPLAYBACK,
               "[%d] Video Decoder drops frame at decode%s: pts=%lld currPresentationTime=%lld (diff: %lld)\n",
               mPipelineId,
               dropUntilNextGOP_ ? " (dropping remainder of GOP)" : "",
               PTS, currPresentationTime,
               currPresentationTime - PTS);
    }

    return refAppDeviceSpecific_NoError;
}

void VideoDecoderDolbyVision::reset()
{
    PRINT("VideoDecoderDolbyVision::%s\n", __func__ );
    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());
    resetTimestampQueue();
    consecutiveDroppedFrameCount_ = 0;
    dropUntilNextGOP_ = false;
}

void VideoDecoderDolbyVision::imageConvert(ptr_hevc_vframe_descriptor_t p_vframe,
                                    VideoRenderer::VideoFrameBuffer& frameBuffer)
{
    PixelFormat dstPixelFormat;
    unsigned char* dstPlanes[4];
    int dstPitches[4];
    uint32_t srcWidth, srcHeight;
    uint32_t dstWidth, dstHeight;

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

        //printf("Y dstPitchtes %d \n", dstPitches[0]);
        //printf("V dstPitchtes %d \n", dstPitches[1]);
        //printf("U dstPitchtes %d \n", dstPitches[2]);
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

        //printf("RGB dstPitchtes %d \n", dstPitches[0]);
        break;

    default:
        dstPixelFormat = PIX_FMT_RGB32;
        assert(false); // unexpected output pixel format
    }


    // We should not use renderer's scaler. Renderer's scaler does not know how
    // many bits are used. We do bit handling here, as well as scaling

    uint32_t dstRectX, dstRectY, dstRectWidth, dstRectHeight;
    (void)dstRectX;
    (void)dstRectY;
    (void)dstRectWidth;
    (void)dstRectHeight;

    // Software Scaling here
    srcWidth  = p_vframe->vp_frame.width;
    srcHeight = p_vframe->vp_frame.height;
    dstWidth  = ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH;
    dstHeight = ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT;

    /*
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
    */
    /*
      NTRACE(TRACE_MEDIAPLAYBACK, "DolbyVision Rendering info : pixel asp %d/%d",
      mAttributes.mPixelAspectRatioX,
      mAttributes.mPixelAspectRatioY);
      NTRACE(TRACE_MEDIAPLAYBACK, "DolbyVision Rendering info : srdWidth/srcHeight %d/%d", srcWidth, srcHeight);
      NTRACE(TRACE_MEDIAPLAYBACK, "DolbyVision Rendering info : dstWidth/dstHeight %d/%d", dstWidth, dstHeight);
    */

    imageConvertContext_ = sws_getCachedContext(imageConvertContext_,
                                                srcWidth,
                                                srcHeight,
                                                (p_vframe->vp_frame.luma_bits == 10)?PIX_FMT_YUV420P10LE:PIX_FMT_YUV420P,
                                                dstWidth,
                                                dstHeight,
                                                dstPixelFormat,
                                                SWS_BICUBIC, NULL, NULL, NULL);
    if (p_vframe->vp_frame.luma_bits == 10) {
        const uint8_t* const srcSlices[3] = {(uint8_t*)p_vframe->vp_frame.data[0],
                                             (uint8_t*)p_vframe->vp_frame.data[1],
                                             (uint8_t*)p_vframe->vp_frame.data[2]};
        const int srcStrides[3] = {(int)p_vframe->vp_frame.stride[0],
                                   (int)p_vframe->vp_frame.stride[1],
                                   p_vframe->vp_frame.stride[2]};
        sws_scale(imageConvertContext_,
                  srcSlices,
                  srcStrides,
                  0,
                  srcHeight,
                  dstPlanes,
                  dstPitches);

    } else if (p_vframe->vp_frame.luma_bits == 8) {
        const uint8_t* const srcSlices[3] = {(uint8_t*)y8_data, (uint8_t*)cb8_data, (uint8_t*)cr8_data};
        const int srcStrides[3] = {(int)srcWidth, (int)srcWidth/2, (int)srcWidth/2};
        sws_scale(imageConvertContext_,
                  srcSlices,
                  srcStrides,
                  0,
                  srcHeight,
                  dstPlanes,
                  dstPitches);
    }

    frameBuffer.setWidth(dstWidth);
    frameBuffer.setHeight(dstHeight);
}

void VideoDecoderDolbyVision::init_buffer(int width, int height)
{
    // 8bit chopped buffer
    posix_memalign((void**)&y8_data, 4096, width * height);
    posix_memalign((void**)&cb8_data, 4096, width * height / 4);
    posix_memalign((void**)&cr8_data, 4096, width * height / 4);
}

void VideoDecoderDolbyVision::cleanup_buffer()
{
    free(y8_data);
    free(cb8_data);
    free(cr8_data);
}

void VideoDecoderDolbyVision::shift_and_copy_lowerbyte(unsigned char* dest, unsigned char* src, int size)
{
    int i = 0;
    unsigned short from;
    unsigned char to;
    for(i=0; i*2 < size; i++) {
        from = *(unsigned short*)(src + i*2);
        to = from >> 2;
        *(dest++) = to;
    }
}

// Take 8-bit data in 16-bit word and save to 8-bit data in 8-bit char
void VideoDecoderDolbyVision::vector_copy_lowerbyte(void * dst,
                                                    const void * src,
                                                    const size_t width,
                                                    const size_t bytes_per_pel)
{
    __m128i l0;
    __m128i l1;
    __m128i l2;
    __m128i l3;

    __m128i * vptr_src = (__m128i *)src;
    __m128i * vptr_dst = (__m128i *)dst;

    const int iterations = (width*bytes_per_pel) / (sizeof(__m128i)*4); // 4 for cache line multiple of m128i
    const int remainder  = (width*bytes_per_pel) % (sizeof(__m128i)*4);

    // Cache lines are 64-bytes on Intel.
    for (int i = 0; i < iterations; i++, vptr_src++, vptr_dst++) {
        l0 = _mm_load_si128(  vptr_src);
        l1 = _mm_load_si128(++vptr_src);
        l2 = _mm_load_si128(++vptr_src);
        l3 = _mm_load_si128(++vptr_src);

        l0 = _mm_packus_epi16(l0, l1);
        l1 = _mm_packus_epi16(l2, l3);

        _mm_storeu_si128(  vptr_dst, l0);
        _mm_storeu_si128(++vptr_dst, l1);
    }

    char* src8 = (char*)vptr_src;
    char* dst8 = (char*)vptr_dst;
    for (int j = 0; j < remainder; j++){
        unsigned char from = *(unsigned char*)(src8 + j*bytes_per_pel);
        *(dst8++) = from;
    }
}

void VideoDecoderDolbyVision::writeDolbyVisionEL2File(const ISampleAttributes *pSampleAttr,
                                                      const unsigned char* auData,
                                                      uint32_t auDataSize)
{
    uint32_t offset = 0;
    uint8_t dolby_code[2] = {0x7e, 0x01};
    uint32_t writtenSize = 0;
    std::vector<uint32_t> nalSizes;
    if(pSampleAttr)
        nalSizes = pSampleAttr->getNaluSizes();

    //printf("auDataSize %d \n", auDataSize);
    for(unsigned int i=0; i<nalSizes.size(); i++){
        //printf("nalsize %d, ", nalSizes[i]);
        if(nalSizes[i]){
            // 1. write size bytes of 4
            writtenSize += fwrite(auData + offset, 1, 4, fpDolbyFile);

            // 2. check if 0x7c 0x01. If not write dolby code of 0x7e, 0x01
            if( *(auData + offset + 4) != 0x7c || *(auData + offset + 5) != 0x1){
                fwrite(dolby_code, 1, 2, fpDolbyFile);
            }

            // 3. write nal data bytes (nal size include start code)
            writtenSize += fwrite(auData + offset + 4, 1, nalSizes[i] - 4, fpDolbyFile);

            // 4. increment offset by nalSize[i]
            offset +=  nalSizes[i];
            //printf("written %d \n", writtenSize);
        }
    }
    //printf("\n");

    if( writtenSize != auDataSize){
        //printf("something wrong\n");
        assert(0);
    }
}
