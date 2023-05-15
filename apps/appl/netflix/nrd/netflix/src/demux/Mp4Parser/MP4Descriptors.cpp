/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MP4Descriptors.h"

#include "Context.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Descriptors
 */

DEFINE_DESCRIPTOR(ES_Descriptor,           0x03); // ES_DescrTag
DEFINE_DESCRIPTOR(DecoderConfigDescriptor, 0x04); // DecoderConfigDescrTag
DEFINE_DESCRIPTOR(DecoderSpecificInfo,     0x05); // DecSpecificInfoTag

/*******************************************************************************
 * ES_Descriptor
 */

DEFINE_CHILD_DESCRIPTORS(ES_Descriptor);
    STD_CHILD_DESCRIPTOR(DecoderConfigDescriptor, MANDATORY_UNIQUE);
DEFINE_CHILD_DESCRIPTORS_END(ES_Descriptor);

void ES_Descriptor::clearSpecifics()
{
    childDescriptors_.clear();
}

bool ES_Descriptor::readSpecifics(Reader& reader, Context& context)
{
    reader.read(ES_ID_, 16);
    reader.read(streamDependenceFlag_, 1);
    reader.read(URL_Flag_, 1);
    reader.read(OCRstreamFlag_, 1);
    reader.read(streamPriority_, 5);

    if(!reader.good())
        return false;

    if(streamDependenceFlag_ == 1)
        reader.read(dependsOn_ES_ID_, 16);
    if(URL_Flag_ == 1)
    {
        reader.read(URLlength_, 8);

        if(!reader.good())
            return false;
        reader.skipBytes(URLlength_);
    }

    if(OCRstreamFlag_ == 1)
        reader.read(OCR_ES_Id_, 16);


    return readDescriptors(context,
                           CHILD_DESCRIPTORS(ES_Descriptor),
                           childDescriptors_,
                           reader);
}

void ES_Descriptor::dumpSpecifics(Dumper& dumper) const
{
    DUMP(ES_ID_);
    DUMP(streamDependenceFlag_);
    DUMP(URL_Flag_);
    DUMP(OCRstreamFlag_);
    DUMP(streamPriority_);

    if(streamDependenceFlag_ == 1)
        DUMP(dependsOn_ES_ID_);

    if(URL_Flag_ == 1)
        DUMP(URLlength_);

    if(OCRstreamFlag_ == 1)
        DUMP(OCR_ES_Id_);

    dumpDescriptors(childDescriptors_, dumper);
}

/*******************************************************************************
 * DecoderConfigDescriptor
 */

DEFINE_CHILD_DESCRIPTORS(DecoderConfigDescriptor);
    STD_CHILD_DESCRIPTOR(DecoderSpecificInfo, OPTIONAL_UNIQUE);
DEFINE_CHILD_DESCRIPTORS_END(DecoderConfigDescriptor);

void DecoderConfigDescriptor::clearSpecifics()
{
    childDescriptors_.clear();
}

bool DecoderConfigDescriptor::readSpecifics(Reader& reader, Context& context)
{
    reader.read(objectTypeIndication_, 8);
    reader.read(streamType_, 6);
    reader.read(upStream_, 1);
    reader.skipBits(1); // const bit(1) reserved=1;
    reader.read(bufferSizeDB_, 24);
    reader.read(maxBitrate_, 32);
    reader.read(avgBitrate_, 32);

    if(reader.good())
    {
        context.currentTrackContext()->setAudioElementaryStreamData(maxBitrate_, avgBitrate_);
    }


    return readDescriptors(context,
                           CHILD_DESCRIPTORS(DecoderConfigDescriptor),
                           childDescriptors_,
                           reader);
}

void DecoderConfigDescriptor::dumpSpecifics(Dumper& dumper) const
{
    DUMP(objectTypeIndication_);
    DUMP(streamType_);
    DUMP(upStream_);
    DUMP(bufferSizeDB_);
    DUMP(maxBitrate_);
    DUMP(avgBitrate_);

    dumpDescriptors(childDescriptors_, dumper);
}

/*******************************************************************************
 * DecoderSpecificInfo
 */

bool DecoderSpecificInfo::readSpecifics(Reader& reader, Context& context)
{
    uint32_t headerSize = static_cast<uint32_t>(reader.tell() - startOffset_);

    if(descriptorSize_ < headerSize)
        return false;

    uint32_t remainingSize = descriptorSize_ - headerSize;

    if(remainingSize > 0)
    {
        context.currentTrackContext()->decoderSpecificData().resize(remainingSize);
        reader.readBytes(context.currentTrackContext()->decoderSpecificData());

    }

    return reader.good();
}
