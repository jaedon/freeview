/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef DEVICE_MP4DEMULTIPLEXER_H
#define DEVICE_MP4DEMULTIPLEXER_H

#include <vector>
#include <nrdbase/tr1.h>
#include <nrdbase/EventQueue.h>
#include <nrdbase/ProducerConsumerList.h>
#include <nrd/ISystem.h>

#include "../Demultiplexer.h"

namespace netflix {
namespace device {

struct AudioAttributes;
struct VideoAttributes;

/**
 * @class Mp4Demultiplexer Mp4Demultiplexer.h
 * @brief Implementation of MP4 demultiplexer.
 */
class DashDemultiplexer : public Demultiplexer
{
public:
    DashDemultiplexer();

    ~DashDemultiplexer();

    void deliverEndOfStream();

    esplayer::Status getNextSample(esplayer::ISampleWriter& sampleWriter);

    esplayer::Status getNextSampleSize(uint32_t& sampleSize,
                                       uint32_t viewNum);

    esplayer::Status getNextSampleTimestamps(llong& dts,
                                             llong& pts);

    shared_ptr<EventQueue> getAttachedEventQueue();

    shared_ptr<EventQueue> mEventQueue;

public:
    class DataBlock
    {
    public:
        DataBlock();
        ~DataBlock();

        /* Get the number of bytes between the current offset and the
           end of the block. */
        uint32_t getDataSize();

        /* Get a pointer to data starting at the current offset. */
        unsigned char* getData();

        /* Move the offset forward in the block.  This will reduce the
           data size correspondingly. */
        void advanceOffset(uint32_t numBytes);

        void setDataBuffer(IBufferManager::Buffer& buffer);
        void setStreamAttributes(StreamAttributes *attributes);

        void setIsParsed();
        bool isParsed();

        const IBufferManager::Buffer& getDataBuffer() const;
        const StreamAttributes* getStreamAttributes() const;
        void reset();

    private:
        IBufferManager::Buffer dataBuffer_;
        uint32_t offset_;
        StreamAttributes* streamAttributes_;
        bool isParsed_;
    };

    class DataBlockStream : public ByteStream
    {
    public:
        DataBlockStream(IConsumerListView<DataBlock>::iterator startBlock,
                        IConsumerListView<DataBlock>::iterator endBlock);

        void reset();

        virtual bool readByte(uint8_t& data);

        virtual bool readBytes(std::vector<uint8_t>& data);

        virtual bool readBytes(std::vector<uint8_t>& data, uint32_t offset, uint32_t numBytes)

        bool writeBytes(esplayer::ISampleWriter& writer, uint32_t numBytes);

        bool writeBytesInternal(esplayer::ISampleWriter& writer, uint32_t numBytes);

        virtual bool skipBytes(ullong numBytes);

        virtual ullong tell() const;

        virtual bool good() const;

        ullong getNumBytesAvailable();

    private:
        IConsumerListView<DataBlock>::iterator startBlock_;
        IConsumerListView<DataBlock>::iterator endBlock_;

        ullong totalOffset_; // Offset in the whole pool of bytes in the list
                               // of blocks.
        ullong totalSize_;   // The total number of bytes in all of the
                               // blocks.
        uint32_t currentBlockOffset_;

        IConsumerListView<DataBlock>::iterator currentBlock_;
        bool     good_;
    };

};

}}

#endif
