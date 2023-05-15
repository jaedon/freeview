/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AudioDecoderNative.h"
#include "../ESPlayerNative.h"
#include "../MediaUtils.h"
#include <cstring>

#ifdef HAVE_DDPLUS
// Dolby DD+ library header
#include <DDPlusDecPlugin.h>
#endif

#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Endian.h>
#include <nrddpi/Common.h>
#include "../DecoderLock.h"

using namespace std;
using namespace netflix::device::esplayer;
using namespace netflix;


#include "AudioDecoderNative_OggParser.h"

const unsigned char AudioDecoderNative::GUID_FORMAT_OggIdentHeader[] =
    // 95388704-162C-42A9-8149-C3577C12AAF9
    { 0x04, 0x87, 0x38, 0x95,
      0x2C, 0x16, 0xA9, 0x42,
      0x81, 0x49, 0xC3, 0x57,
      0x7C, 0x12, 0xAA, 0xF9 };

const unsigned char AudioDecoderNative::GUID_FORMAT_DolbyDigitalPlus[] =
    // A7FB87AF-2D02-42FB-A4D4-05CD93843BDD
    { 0xAF, 0x87, 0xFB, 0xA7,
      0x02, 0x2D, 0xFB, 0x42,
      0xA4, 0xD4, 0x05, 0xCD,
      0x93, 0x84, 0x3B, 0xDD };

AudioDecoderNative::AudioDecoderNative(uint32_t numOutputChannels)
  : AudioDecoder(numOutputChannels),
    codecContext_(NULL),
#ifdef HAVE_DDPLUS
    ddplusdecContext_(NULL),
#endif
      oggPacketParser_(new OggPacketParser)
{
    // Register selected audio decoders from FFMPEG
    avcodec_register_all();
    decoded_frame_ = NULL;
    av_init_packet(&avpkt_);
    decoded_frame_=avcodec_alloc_frame();
    swr_=NULL;
}

AudioDecoderNative::~AudioDecoderNative()
{
#ifdef HAVE_DDPLUS
    if(ddplusdecContext_ != NULL)
    {
        ddplusdec_close(ddplusdecContext_);
        ddplusdecContext_ = NULL;
    }
#endif

    if(codecContext_ != NULL)
    {
        if(codecContext_->codec != NULL)
            avcodec_close(codecContext_);

        if(codecContext_->extradata_size > 0)
            delete [] codecContext_->extradata;

        av_free(codecContext_);

        codecContext_ = NULL;
    }

    avcodec_free_frame(&decoded_frame_);
    if (swr_){
        swr_free(&swr_);
        swr_=NULL;
    }
}

bool AudioDecoderNative::decode(const ISampleAttributes*  pSampleAttr,
                                const unsigned char* auData, uint32_t auDataSize,
                                AudioESPlayerNative::DecodedAudioBuffer& decodedAudioBuffer)
{
    int bytes; // total decoded bytes
    bool attachPTS = true;

    //   if((accessUnitAttributes.flags_ & AU_END_OF_STREAM) != 0)
    //   return true; // do nothing for the EOS access-unit
    assert(pSampleAttr);

    if(codecContext_->codec_id == CODEC_ID_VORBIS)
    {
        unsigned char* outputBuffer = decodedAudioBuffer.data();
        int capacity = decodedAudioBuffer.size();

        bytes = 0;

        vector<shared_ptr<vector<unsigned char> > >& packets =
            oggPacketParser_->getStoredPackets();

        // If there are left-over packets, don't attach the PTS to the decoded samples
        // since the PTS could be off by a lot.

        if(packets.size() > 0)
            attachPTS = false;

        oggPacketParser_->parseOggPages(auData, auDataSize, false);
        packets = oggPacketParser_->getStoredPackets();

        uint32_t i;
        for(i = 0; i < packets.size(); i ++)
        {
            int decodedBytes;

            if(capacity - bytes < 48000 * 4) // FFMPEG demands certain minimum output space when trying to decode.
                break;

            if(!decode(&(*packets[i])[0], packets[i]->size(),
                       &outputBuffer[bytes], capacity - bytes, decodedBytes))
            {
                statsCounter_.decodingError(); // If there is more than one
                                               // error in this frame of
                                               // samples, it's ok that we count
                                               // more that one decoding error
                                               // here. We'll only be report
                                               // that the frame had one more
                                               // errors up to the SDK anyway.
                continue; // ignore this bad packet.
            }

            bytes += decodedBytes;
        }
        statsCounter_.frameDecoded();
        oggPacketParser_->freeStoredPacketsPartially(i);

        // FIXME: for OGG Vorbis, the time-stamp attached to the output samples could be
        //        shifted, this might cause minor AV synchronization problems.
    }
    else
    {
        if(!decode(auData, auDataSize, decodedAudioBuffer.data(), decodedAudioBuffer.size(), bytes)) {
            statsCounter_.decodingError(); // If ffmpeg separately informs
                                           // statCounter_ of the decoding error
                                           // via the callback in
                                           // DecoderManager, that's fine. We'll
                                           // only be reporting that the frame
                                           // had one or more errors up to the
                                           // SDK anyway.
            return false;
        }
        statsCounter_.frameDecoded();
    }

    // Currently it is assumed that audio decoder always consumes the whole access-unit,
    // this might not be true if media formats other than ASF are used. If that happens,
    // the value in "len" should be taken into account.

    decodedAudioBuffer.setDataBytes(bytes);

    if(attachPTS)

        decodedAudioBuffer.setTimestamp(pSampleAttr->getPTS()); // accessUnitAttributes.DTS_);

    return true;
}

bool AudioDecoderNative::decode(
        const unsigned char* auData, uint32_t auDataSize,
        unsigned char* outputBuffer, int capacity, int& decodedBytes)
{
    if(codecContext_->codec_id == CODEC_ID_EAC3)
    {
        uint16_t* words = reinterpret_cast<uint16_t*>(
                              const_cast<unsigned char*>(auData));

        uint32_t wordCount = (auDataSize >> 1);

        if(wordCount > 0 && words[0] == 0x770B)
        {
            // swap bytes of each words (16-bits-long) if necessary.

            uint32_t i;
            for(i = 0; i < wordCount; i ++)
                words[i] = netflix::Endian::byteswap<uint16_t>(words[i]);
        }

#ifdef HAVE_DDPLUS
        decodedBytes = ddplusdec_decode(ddplusdecContext_,
                                        outputBuffer,
                                        capacity,
                                        auData,
                                        auDataSize);
#endif
    }
    else
    {
        int got_frame_ptr;

        avpkt_.data=const_cast<unsigned char*>(auData);
        avpkt_.size=auDataSize;

        avcodec_get_frame_defaults(decoded_frame_);
        int len = avcodec_decode_audio4(codecContext_,
                                        decoded_frame_,
                                        &got_frame_ptr,
                                        &avpkt_);

        if (got_frame_ptr>0){
            decodedBytes = av_samples_get_buffer_size(NULL, codecContext_->channels,
                                                      decoded_frame_->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
            if (decodedBytes > capacity){
                fprintf(stderr, "overflow : decoded bytes %d are bigger than capacity %d\n", decodedBytes, capacity);
                return false;
            }else{
                // test if we need to do a resample
                if (codecContext_->sample_fmt!=AV_SAMPLE_FMT_S16){  //need resample
                    if (swr_==NULL){ // alloc and init resampler context
                        swr_=swr_alloc_set_opts(NULL,
                                                codecContext_->channel_layout,
                                                AV_SAMPLE_FMT_S16,
                                                codecContext_->sample_rate,
                                                codecContext_->channel_layout,
                                                codecContext_->sample_fmt,
                                                codecContext_->sample_rate,
                                                0,
                                                NULL);
                        if (swr_init(swr_)!=0){
                            fprintf(stderr, "swr_init error\n");
                            return false;
                        }
                    }
                    swr_convert(swr_,
                                &outputBuffer,
                                decoded_frame_->nb_samples,
                                (const uint8_t**) (decoded_frame_->extended_data),
                                decoded_frame_->nb_samples);
                    //memcpy(outputBuffer, decoded_frame_->data[0], decodedBytes);
                } else {
                    // doing a mem copy for now
                    memcpy(outputBuffer, decoded_frame_->data[0], decodedBytes);
                }
            }
        }else{
            decodedBytes=0;
        }

        if(len < 0)
            return false;
    }

    return (decodedBytes > 0)? true : false;
}

void AudioDecoderNative::reset()
{
    // "avcodec_flush_buffers()" does not work for all avcodec(s), for example, both vc1 and wma
    // decoders do not expose any flush() function pointer. Therefore, it is safer to reset the
    // decoder by closing and re-opening it, instead of using avcodec_flush_buffers().

    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());

    if(codecContext_ != NULL)
    {
        if(codecContext_->codec_id == CODEC_ID_EAC3)
        {
#ifdef HAVE_DDPLUS
            ddplusdec_close(ddplusdecContext_);
            ddplusdecContext_ = ddplusdec_open();
#endif
        }
        else
        {
            avcodec_close(codecContext_);
            avcodec_open2(codecContext_, avcodec_find_decoder(codecContext_->codec_id), NULL);
        }
    }

    oggPacketParser_->reset();

    if (swr_){
        swr_free(&swr_);
        swr_=NULL;
    }
}

bool AudioDecoderNative::updateAttributes(const AudioAttributes* attributes)
{
    ScopedMutex scopedMutex(DecoderLock::mutexForOpenClose());

    if(codecContext_ != NULL) // do some clean-up if the codec-context already exists.
    {
        if(codecContext_->codec_id == CODEC_ID_EAC3)
        {
#ifdef HAVE_DDPLUS
            ddplusdec_close(ddplusdecContext_);
            ddplusdecContext_ = NULL;
#endif
        }
        else
        {
            avcodec_close(codecContext_);
        }

        if(codecContext_->extradata_size > 0)
            delete [] codecContext_->extradata;
    }
    else // otherwise, create a new codec-context.
    {
        codecContext_ = avcodec_alloc_context3(NULL);
    }

    codecContext_->codec_type      = AVMEDIA_TYPE_AUDIO;
    codecContext_->codec_tag       = MediaUtils::getFourCCFromCodecParam(attributes->mCodecParam);
    codecContext_->channels        = attributes->mNumberOfChannels;
    codecContext_->sample_rate     = attributes->mSamplesPerSecond;
    codecContext_->bit_rate        = attributes->mBitrate; // Do we really need this? (attributes.bytesPerSecond_ << 3);
    codecContext_->block_align     = attributes->mBlockAlignment;
    codecContext_->extradata_size  = attributes->mCodecSpecificDataLen;

    if(codecContext_->extradata_size > 0)
    {
        codecContext_->extradata = new unsigned char[codecContext_->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE];
        memcpy(codecContext_->extradata,
               attributes->mCodecSpecificData,
               attributes->mCodecSpecificDataLen);
    }

    switch(codecContext_->codec_tag)
    {
    case 0x0160: codecContext_->codec_id = CODEC_ID_WMAV1; break;
    case 0x0161: codecContext_->codec_id = CODEC_ID_WMAV2; break;
    case 0x1610: codecContext_->codec_id = CODEC_ID_AAC;   break;
    case 0xEAC3: codecContext_->codec_id = CODEC_ID_EAC3;  break;
    case 0x6771:
        handleOggIdentHeader(attributes->mCodecSpecificData, attributes->mCodecSpecificDataLen);
        break;
    case 0xFFFE:
        handleWaveFormatExtensible(attributes->mCodecSpecificData, attributes->mCodecSpecificDataLen);
        break;
    default:
        codecContext_->codec_id = CODEC_ID_NONE;
    }

    if(codecContext_->codec_id == CODEC_ID_NONE)
        return false; // something wrong

    // Decoder should output the same number of channels as the audio renderer requested.
    //

    codecContext_->request_channels = numOutputChannels_;

    if(codecContext_->codec_id == CODEC_ID_EAC3)
    {
#ifdef HAVE_DDPLUS
        ddplusdecContext_ = ddplusdec_open();
#endif
    }
    else
    {
        // Locate registered FFMPEG audio decoder (according to codec_id)
        // and open (init) the decoder instance.

        AVCodec* acc = avcodec_find_decoder(codecContext_->codec_id);
        if(acc == NULL || avcodec_open2(codecContext_, acc, NULL) < 0)
        {
            // TODO: log
            return false; // something wrong
        }
    }

    codecContext_->opaque = &statsCounter_; // We attach our stats counter to the
                                            // codec context here. When ffmpeg's
                                            // error reporting callback gets
                                            // called, the codecContext is
                                            // passed in as one of the
                                            // arguments.  This allows us to use
                                            // the stats counter to count the
                                            // error. The opaque
                                            // field is defined to be
                                            // user-specific, so it's kosher for
                                            // us to do this.

    if (swr_){
        swr_free(&swr_);
        swr_=NULL;
    }
    return true;
}

void AudioDecoderNative::handleWaveFormatExtensible(const uint8_t* codecSpecificData, uint32_t codecSpecificDataLen)
{
    codecContext_->codec_id = CODEC_ID_NONE;

    if(codecSpecificDataLen < 22)
        return;

    if(memcmp(&codecSpecificData[6], GUID_FORMAT_DolbyDigitalPlus, 16) == 0)
    {
        codecContext_->codec_id = CODEC_ID_EAC3;
    }
    else if(memcmp(&codecSpecificData[6], GUID_FORMAT_OggIdentHeader, 16) == 0)
    {
        handleOggIdentHeader(&codecSpecificData[22], codecSpecificDataLen - 22);
    }
}

void AudioDecoderNative::handleOggIdentHeader(const unsigned char* data, uint32_t bytes)
{
    codecContext_->codec_id = CODEC_ID_NONE;

    oggPacketParser_->parseOggPages(data, bytes, true);

    vector<shared_ptr<vector<unsigned char> > >& packets =
        oggPacketParser_->getStoredPackets();

    if(packets.size() == 3) // must be exactly 3 XIPH headers for Vorbis
    {
        codecContext_->extradata_size = 0;

        uint32_t i;
        for(i = 0; i < packets.size(); i ++)
        {
            // add fields to provide the sizes (big endian) of the XIPH header
            codecContext_->extradata[codecContext_->extradata_size] =
                (packets[i]->size() >> 8) & 0xFF;
            codecContext_->extradata[codecContext_->extradata_size + 1] =
                (packets[i]->size()) & 0xFF;

            codecContext_->extradata_size += 2;

            memcpy(&codecContext_->extradata[codecContext_->extradata_size],
                   &(*packets[i])[0], packets[i]->size());

            codecContext_->extradata_size += packets[i]->size();
        }

        codecContext_->codec_id = CODEC_ID_VORBIS; // assume that only Vorbis could be in Ogg.
    }

    oggPacketParser_->reset();
}
