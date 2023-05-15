/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <nrd/AppLog.h>
#include "FileSystem.h"

using namespace netflix;
using namespace netflix::device;

/*
 * **********
 * Dual Video
 * **********
 */

uint32_t FileSystem::maxVideoPipelines()
{
    return mVideoPipelineCapabilities.size();
}


std::vector<struct ISystem::VideoPipelineCapability> FileSystem::getVideoPipelineCapabilities()
{
    return mVideoPipelineCapabilities;
}


NFErr FileSystem::setVideoPipelineCapabilities(const std::vector<struct ISystem::VideoPipelineCapability>& caps)
{
    mVideoPipelineCapabilities.clear();
    mVideoPipelineCapabilities = caps;
    return NFErr_OK;
}

void FileSystem::setVideoProfileEnvelope(ISystem::CodecProfile maxHEVCProfileEnvelope,
                                         ISystem::CodecProfile maxAVCProfileEnvelope)
{
    NTRACE(TRACE_DPI, "FileSystem:setVideoProfileEnvelope maxHEVC %d, maxAVC %d",
           maxHEVCProfileEnvelope, maxAVCProfileEnvelope);

    mQaBridgeHEVCProfileEnvelope = maxHEVCProfileEnvelope;
    mQaBridgeAVCProfileEnvelope = maxAVCProfileEnvelope;
}

void FileSystem::getVideoProfileEnvelope(ISystem::CodecProfile &maxHEVCProfileEnvelope,
                                         ISystem::CodecProfile &maxAVCProfileEnvelope)
{
    NTRACE(TRACE_DPI, "FileSystem:getVideoProfileEnvelope maxHEVC %d, maxAVC %d",
           mQaBridgeHEVCProfileEnvelope, mQaBridgeAVCProfileEnvelope);

    maxHEVCProfileEnvelope = mQaBridgeHEVCProfileEnvelope;
    maxAVCProfileEnvelope = mQaBridgeAVCProfileEnvelope;
}

NFErr FileSystem::getRemainingVideoPipelineCapability(
    const std::vector<struct ISystem::VideoPipelineCapability>& pipelineInUse,
    struct ISystem::VideoPipelineCapability& pipelineToUse)
{
    /*
     * In this example, assume that combined HEVC decoder capability of both
     * pipeline can not go over L51 which has a max luma sample rate of 534773760.
     *
     * assume that combined AVC decoder capability of both pipeline can not go over L4.0
     */

    std::map<uint32_t, uint32_t> AVCSampleRate;
    std::map<uint32_t, uint32_t> HEVCSampleRate;

    // per AVC specification
    AVCSampleRate[(unsigned int)ISystem::AVC_MAIN_L30] = 10368000;
    AVCSampleRate[(unsigned int)ISystem::AVC_MAIN_L31] = 27648000; // more than twice of L30
    AVCSampleRate[(unsigned int)ISystem::AVC_MAIN_L40] = 62914560; // more than twice of L31
    // per HEVC specification
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L20] = 3686400;
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L21] = 7372800;   // twice of L20
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L30] = 16588800;  // more than twice of L21
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L31] = 33177600;  // twice of L30
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L40] = 66846720;  // twice of L31
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L41] = 133693440; // twice of L40
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L50] = 267386880; // twice of L41
    HEVCSampleRate[(unsigned int)ISystem::HEVC_MAIN10_L51] = 534773760; // twice of L50

    // device limitation : TODO: value injection for QA simulation
    uint32_t totalHEVCSampleRateLimit = HEVCSampleRate[mQaBridgeHEVCProfileEnvelope];
    uint32_t totalAVCSampleRateLimit = AVCSampleRate[mQaBridgeAVCProfileEnvelope];

    if(pipelineInUse.size() >= maxVideoPipelines() ){
        // we don't have any more pipeline left
        return NFErr_BadParameter;
    }

    if(pipelineInUse.size() == 0){
        // query for the single pipeline capability.
        pipelineToUse.isFullFeaturedPipeline = true;
        pipelineToUse.pipelineId = 1;
        pipelineToUse.maxHEVCCodecProfile = ISystem::HEVC_MAIN10_L51;
        pipelineToUse.maxAVCCodecProfile = ISystem::AVC_MAIN_L40;
        pipelineToUse.isDrmCapable = true;
        pipelineToUse.isFullPictureQuality = true;
        pipelineToUse.zOrder = ISystem::ZORDER_ANY;
    }

    if(pipelineInUse.size() == 1){
        // query for the second pipeline capability.

        // find out remaining HEVC power
        uint32_t remainingHEVCSampleRate = totalHEVCSampleRateLimit - pipelineInUse[0].maxHEVCCodecProfile;
        std::map<uint32_t, uint32_t>::reverse_iterator hevcit = HEVCSampleRate.rbegin();
        for( ; hevcit != HEVCSampleRate.rend(); hevcit++){
            if ( hevcit->second <= remainingHEVCSampleRate ){
                    break;
            }
        }
        pipelineToUse.maxHEVCCodecProfile = static_cast<ISystem::CodecProfile> (hevcit->first);

        // find out remaining AVC power
        uint32_t remainingAVCSampleRate = totalAVCSampleRateLimit - pipelineInUse[0].maxAVCCodecProfile;
        std::map<uint32_t, uint32_t>::reverse_iterator avcit = AVCSampleRate.rbegin();
        for( ; avcit != AVCSampleRate.rend(); avcit++){
            if ( avcit->second <= remainingAVCSampleRate ){
                    break;
            }
        }
        pipelineToUse.maxAVCCodecProfile = static_cast<ISystem::CodecProfile> (avcit->first);

        pipelineToUse.pipelineId = 2;
        pipelineToUse.isDrmCapable = true;
        pipelineToUse.isFullPictureQuality = true;
        pipelineToUse.zOrder = ISystem::ZORDER_ANY;
        pipelineToUse.isFullFeaturedPipeline = false;
    }

    return NFErr_OK;
}

/*
 * ************
 * Dolby Vision
 * ************
 */
void FileSystem::setDolbyVisionRendering(bool renderBL, bool renderEL)
{
    sQaBridgeRenderDolbyVisionBL = renderBL;
    sQaBridgeRenderDolbyVisionEL = renderEL;
}

void FileSystem::getDolbyVisionRendering(bool& renderBL, bool& renderEL)
{
    renderBL = sQaBridgeRenderDolbyVisionBL;
    renderEL = sQaBridgeRenderDolbyVisionEL;
}

void FileSystem::setDolbyVisionFileDump(std::string fileName)
{
    sQaBridgeDolbyFileDump = fileName;
}
