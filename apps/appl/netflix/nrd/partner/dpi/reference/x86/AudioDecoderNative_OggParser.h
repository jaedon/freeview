/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_AUDIODECODERNATIVE_OGGPARSER_H
#define DEVICE_AUDIODECODERNATIVE_OGGPARSER_H

#include <cassert>
#include <vector>
#include <nrdbase/tr1.h>
#include "AudioDecoderNative.h"

namespace netflix {
namespace device {
namespace esplayer {

class AudioDecoderNative::OggPacketParser
{
private:

    std::vector<shared_ptr<std::vector<unsigned char> > > freePackets_;
    std::vector<shared_ptr<std::vector<unsigned char> > > storedPackets_;
    shared_ptr<std::vector<unsigned char> >          unfinishedPacket_;

    void finishPacketIfAny()
    {
        if(unfinishedPacket_.get() != NULL)
        {
            storedPackets_.push_back(unfinishedPacket_);
            unfinishedPacket_.reset();
        }
    }

public:

    void reset()
    {
        finishPacketIfAny();
        freeStoredPackets();
    }

    std::vector<shared_ptr<std::vector<unsigned char> > >& getStoredPackets()
    {
        return storedPackets_;
    }

    void freeStoredPackets()
    {
        uint32_t i;
        for(i = 0; i < storedPackets_.size(); i ++)
            freePackets_.push_back(storedPackets_[i]);

        storedPackets_.clear();
    }

    void freeStoredPacketsPartially(uint32_t numOfLeadingPacketsToFree)
    {
        assert(numOfLeadingPacketsToFree <= storedPackets_.size());

        if(numOfLeadingPacketsToFree == 0)
            return;

        if(numOfLeadingPacketsToFree == storedPackets_.size())
        {
            freeStoredPackets();
            return;
        }

        uint32_t i;
        for(i = 0; i < storedPackets_.size(); i ++)
        {
            if(i < numOfLeadingPacketsToFree)
                freePackets_.push_back(storedPackets_[i]);
            else
                storedPackets_[i - numOfLeadingPacketsToFree] = storedPackets_[i];
        }

        storedPackets_.resize(storedPackets_.size() - numOfLeadingPacketsToFree);
    }

    void parseOggPages(const unsigned char* data, uint32_t bytes, bool eos)
    {
        while(bytes >= 27)
        {
            if(memcmp(data, "OggS", 4) != 0)
                return;

            unsigned char headerTypeFlags = data[5];
            unsigned char pageSegments = data[26];

            if((headerTypeFlags & 0x01) == 0) // *not* continued from previous page
                finishPacketIfAny();

            if(bytes < (uint32_t)27 + pageSegments)
                return;

            const unsigned char* lacing = &data[27];

            data = &data[27 + pageSegments];
            bytes -= 27 + pageSegments;

            unsigned int i;
            for(i = 0; i < pageSegments; i ++)
            {
                if(lacing[i] > 0)
                {
                    if(bytes < lacing[i])
                        return;

                    if(unfinishedPacket_.get() == NULL) // create a new packet if none exists
                    {
                        if(freePackets_.size() > 0) // try to reuse old packets if any available
                        {
                            unfinishedPacket_ = freePackets_.back();
                            freePackets_.pop_back();
                            unfinishedPacket_->resize(0);
                        }
                        else
                        {
                            unfinishedPacket_.reset(new std::vector<unsigned char>());
                        }
                    }

                    uint32_t offset = unfinishedPacket_->size();
                    unfinishedPacket_->resize(offset + lacing[i]);
                    memcpy(&(*unfinishedPacket_)[offset], &data[0], lacing[i]);

                    data = &data[lacing[i]];
                    bytes -= lacing[i];
                }

                if(lacing[i] < 255)
                    finishPacketIfAny();
            }

            if((headerTypeFlags & 0x04) != 0) // this is the last page of stream
                finishPacketIfAny();
        }

        if(eos)
            finishPacketIfAny();
    }
};

} // namespace esplayer
} // namespace device
} // namespace netflix


#endif // DEVICE_AUDIODECODERNATIVE_OGGPARSER_H
