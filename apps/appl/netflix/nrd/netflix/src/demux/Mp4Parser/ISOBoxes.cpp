/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ISOBoxes.h"

#include "Context.h"

#include "AVCBoxes.h"
#include "MP4Boxes.h"
#include "PIFFBoxes.h"
#include "NFLXBoxes.h"
#include "HEVCBoxes.h"
#include "DolbyBoxes.h"
#include "SampleGroupEntries.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"
#include <stdio.h>

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */

DEFINE_STD_BOXTYPE(MediaDataBox,                       "mdat");
DEFINE_STD_BOXTYPE(FileTypeBox,                        "ftyp");
DEFINE_STD_BOXTYPE(MovieBox,                           "moov");
DEFINE_STD_BOXTYPE(MovieHeaderBox,                     "mvhd");
DEFINE_STD_BOXTYPE(TrackBox,                           "trak");
DEFINE_STD_BOXTYPE(TrackHeaderBox,                     "tkhd");
DEFINE_STD_BOXTYPE(EditBox,                            "edts");
DEFINE_STD_BOXTYPE(EditListBox,                        "elst");
DEFINE_STD_BOXTYPE(MediaBox,                           "mdia");
DEFINE_STD_BOXTYPE(MediaHeaderBox,                     "mdhd");
DEFINE_STD_BOXTYPE(HandlerBox,                         "hdlr");
DEFINE_STD_BOXTYPE(MediaInformationBox,                "minf");
DEFINE_STD_BOXTYPE(VideoMediaHeaderBox,                "vmhd");
DEFINE_STD_BOXTYPE(SoundMediaHeaderBox,                "smhd");
DEFINE_STD_BOXTYPE(DataInformationBox,                 "dinf");
DEFINE_STD_BOXTYPE(DataReferenceBox,                   "dref");
DEFINE_STD_BOXTYPE(DataEntryUrlBox,                    "url ");
DEFINE_STD_BOXTYPE(DataEntryUrnBox,                    "urn ");
DEFINE_STD_BOXTYPE(SampleTableBox,                     "stbl");
DEFINE_STD_BOXTYPE(SampleDescriptionBox,               "stsd");
DEFINE_STD_BOXTYPE(PixelAspectRatioBox,                "pasp");
DEFINE_STD_BOXTYPE(ColourInformationBox,               "colr");
DEFINE_STD_BOXTYPE(ProtectionSchemeInfoBox,            "sinf");
DEFINE_STD_BOXTYPE(OriginalFormatBox,                  "frma");
DEFINE_STD_BOXTYPE(SchemeTypeBox,                      "schm");
DEFINE_STD_BOXTYPE(SchemeInformationBox,               "schi");
DEFINE_STD_BOXTYPE(TimeToSampleBox,                    "stts");
DEFINE_STD_BOXTYPE(CompositionOffsetBox,               "ctts");
DEFINE_STD_BOXTYPE(SampleSizeBox,                      "stsz");
DEFINE_STD_BOXTYPE(SampleToChunkBox,                   "stsc");
DEFINE_STD_BOXTYPE(ChunkOffsetBox,                     "stco");
DEFINE_STD_BOXTYPE(SampleGroupDescriptionBox,          "sgpd");
DEFINE_STD_BOXTYPE(MovieExtendsBox,                    "mvex");
DEFINE_STD_BOXTYPE(MovieExtendsHeaderBox,              "mehd");
DEFINE_STD_BOXTYPE(TrackExtendsBox,                    "trex");
DEFINE_STD_BOXTYPE(MovieFragmentBox,                   "moof");
DEFINE_STD_BOXTYPE(MovieFragmentHeaderBox,             "mfhd");
DEFINE_STD_BOXTYPE(TrackFragmentBox,                   "traf");
DEFINE_STD_BOXTYPE(TrackFragmentHeaderBox,             "tfhd");
DEFINE_STD_BOXTYPE(TrackRunBox,                        "trun");
DEFINE_STD_BOXTYPE(TrackFragmentDecodeTimeBox,         "tfdt");
DEFINE_STD_BOXTYPE(DisposableSamplesBox,               "sdtp");
DEFINE_STD_BOXTYPE(SampleToGroupBox,                   "sbgp");
DEFINE_STD_BOXTYPE(MovieFragmentRandomAccessBox,       "mfra");
DEFINE_STD_BOXTYPE(TrackFragmentRandomAccessBox,       "tfra");
DEFINE_STD_BOXTYPE(MovieFragmentRandomAccessOffsetBox, "mfro");

/*******************************************************************************
 * MediaDataBox
 */

bool MediaDataBox::readSpecifics(Reader& /*reader*/, Context& /*context*/)
{
    return true;
}

/*******************************************************************************
 * FileTypeBox
 */

void FileTypeBox::clearSpecifics()
{
    compatible_brands_.clear();
}

bool FileTypeBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    reader.read(major_brand_, 32);
    reader.read(minor_version_, 32);

    while(reader.good() && !reader.eos())
    {
        uint32_t compatible_brand;
        reader.read(compatible_brand, 32);
        compatible_brands_.push_back(compatible_brand);
    }

    return reader.good();
}

void FileTypeBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP_FOURCC(major_brand_);
    DUMP(minor_version_);

    list<uint32_t>::const_iterator iter;
    for(iter = compatible_brands_.begin(); iter != compatible_brands_.end(); iter ++)
        dumper.dumpFourCC("compatible_brands_", *iter);
}

/*******************************************************************************
 * MovieBox
 */

DEFINE_CHILD_BOXES(MovieBox);
    STD_CHILD_BOX(MovieHeaderBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(TrackBox, MANDATORY);
    STD_CHILD_BOX(MovieExtendsBox, OPTIONAL_UNIQUE);
    DUAL_CHILD_BOX(ProtectionSystemSpecificHeaderBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(MovieBox);

void MovieBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MovieBox::readSpecifics(Reader& reader, Context& context)
{
    context.movieParseBegin(startOffset_, boxSize_);

    if(context.parserState() != Context::PARSE_CONTINUE)
        return false;

    if(!readBoxes(context, CHILD_BOXES(MovieBox), childBoxes_, reader))
        return false;

    context.movieParseEnd();
    return true;
}

void MovieBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * MovieHeaderBox
 */

bool MovieHeaderBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    if(version_ == 1)
    {
        reader.read(creation_time_, 64);
        reader.read(modification_time_, 64);
        reader.read(timescale_, 32);
        reader.read(duration_, 64);
    }
    else if(version_ == 0)
    {
        reader.read(creation_time_, 32);
        reader.read(modification_time_, 32);
        reader.read(timescale_, 32);
        reader.read(duration_, 32);
    }
    else
    {
        return false;
    }

    reader.read(rate_, 32);
    reader.read(volume_, 16);
    reader.skipBytes(10); // const bit(16) reserved = 0;
                          // const unsigned int(32)[2] reserved = 0;
    reader.readBytes(matrix_);
    reader.skipBytes(24); // bit(32)[6] pre_defined = 0;
    reader.read(next_track_ID_, 32);

    return reader.good();
}

void MovieHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(creation_time_);
    DUMP(modification_time_);
    DUMP(timescale_);
    DUMP(duration_);
    DUMP(rate_);
    DUMP(volume_);
    DUMP_MATRIX(matrix_);
    DUMP(next_track_ID_);
}

/*******************************************************************************
 * TrackBox
 */

DEFINE_CHILD_BOXES(TrackBox);
    STD_CHILD_BOX(TrackHeaderBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(MediaBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(EditBox, OPTIONAL_UNIQUE);
DEFINE_CHILD_BOXES_END(TrackBox);

void TrackBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool TrackBox::readSpecifics(Reader& reader, Context& context)
{
    context.trackParseBegin(startOffset_, boxSize_);

    if(context.parserState() != Context::PARSE_CONTINUE)
        return false;

    if(!readBoxes(context, CHILD_BOXES(TrackBox), childBoxes_, reader))
        return false;

    context.trackParseEnd();
    return true;
}

void TrackBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * TrackHeaderBox
 */

bool TrackHeaderBox::readSpecifics(Reader& reader, Context& context)
{
    if(version_ == 1)
    {
        reader.read(creation_time_, 64);
        reader.read(modification_time_, 64);
        reader.read(track_ID_, 32);
        reader.skipBytes(4); // const unsigned int(32) reserved = 0;
        reader.read(duration_, 64);
    }
    else if(version_ == 0)
    {
        reader.read(creation_time_, 32);
        reader.read(modification_time_, 32);
        reader.read(track_ID_, 32);
        reader.skipBytes(4); // const unsigned int(32) reserved = 0;
        reader.read(duration_, 32);
    }
    else
    {
        return false;
    }

    reader.skipBytes(8); // const unsigned int(32)[2] reserved = 0;
    reader.read(layer_, 16);
    reader.read(alternate_group_, 16);
    reader.read(volume_, 16);
    reader.skipBytes(2); // const unsigned int(16) reserved = 0;
    reader.readBytes(matrix_);
    reader.read(width_, 32);
    reader.read(height_, 32);

    if(reader.good())
        context.currentTrackContext()->setTrackID(track_ID_);

    return reader.good();
}

void TrackHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(creation_time_);
    DUMP(modification_time_);
    DUMP(track_ID_);
    DUMP(duration_);
    DUMP(layer_);
    DUMP(alternate_group_);
    DUMP(volume_);
    DUMP_MATRIX(matrix_);
    DUMP(width_);
    DUMP(height_);
}

/*******************************************************************************
 * MediaBox
 */

DEFINE_CHILD_BOXES(MediaBox);
    STD_CHILD_BOX(MediaHeaderBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(HandlerBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(MediaInformationBox, MANDATORY_UNIQUE);
DEFINE_CHILD_BOXES_END(MediaBox);

void MediaBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MediaBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(MediaBox), childBoxes_, reader);
}

void MediaBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * MediaHeaderBox
 */

void MediaHeaderBox::clearSpecifics()
{
    language_.clear();
}

bool MediaHeaderBox::readSpecifics(Reader& reader, Context& context)
{
    if(version_ == 1)
    {
        reader.read(creation_time_, 64);
        reader.read(modification_time_, 64);
        reader.read(timescale_, 32);
        reader.read(duration_, 64);
    }
    else if(version_ == 0)
    {
        reader.read(creation_time_, 32);
        reader.read(modification_time_, 32);
        reader.read(timescale_, 32);
        reader.read(duration_, 32);
    }
    else
    {
        return false;
    }

    reader.skipBits(1); // bit(1) pad = 0;

    char char1 = 0;
    char char2 = 0;
    char char3 = 0;

    // unsigned int(5)[3] language; (ISO-639-2/T)
    reader.read(char1, 5);
    reader.read(char2, 5);
    reader.read(char3, 5);

    language_.push_back(char1 + 0x60);
    language_.push_back(char2 + 0x60);
    language_.push_back(char3 + 0x60);

    reader.skipBytes(2); // unsigned int(16) pre_defined = 0;

    if(reader.good())
        context.currentTrackContext()->setMediaTimeInfo(duration_, timescale_);

    return reader.good();
}

void MediaHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(creation_time_);
    DUMP(modification_time_);
    DUMP(timescale_);
    DUMP(duration_);
    DUMP(language_);
}

/*******************************************************************************
 * HandlerBox
 */

const uint32_t HandlerBox::VideoTrackType_         = Box::buildFourCC("vide");
const uint32_t HandlerBox::AudioTrackType_         = Box::buildFourCC("soun");
const uint32_t HandlerBox::HintTrackType_          = Box::buildFourCC("hint");
const uint32_t HandlerBox::TimedMetadataTrackType_ = Box::buildFourCC("meta");

bool HandlerBox::readSpecifics(Reader& reader, Context& context)
{
    reader.skipBytes(4); // unsigned int(32) pre_defined = 0;
    reader.read(handler_type_, 32);
    reader.skipBytes(12); // const unsigned int(32)[3] reserved = 0;

    if(reader.good())
        context.currentTrackContext()->setHandlerType(handler_type_);

    return reader.good();
}

void HandlerBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP_FOURCC(handler_type_);
}

/*******************************************************************************
 * MediaInformationBox
 */

DEFINE_CHILD_BOXES(MediaInformationBox);
    STD_CHILD_BOX(VideoMediaHeaderBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(SoundMediaHeaderBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(DataInformationBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(SampleTableBox, MANDATORY_UNIQUE);
DEFINE_CHILD_BOXES_END(MediaInformationBox);

void MediaInformationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MediaInformationBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(MediaInformationBox), childBoxes_, reader);
}

void MediaInformationBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * VideoMediaHeaderBox
 */

bool VideoMediaHeaderBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    reader.read(graphicsmode_, 16);
    reader.read(opcolor_red_, 16);
    reader.read(opcolor_green_, 16);
    reader.read(opcolor_blue_, 16);

    return reader.good();
}

void VideoMediaHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(graphicsmode_);
    DUMP(opcolor_red_);
    DUMP(opcolor_green_);
    DUMP(opcolor_blue_);
}

/*******************************************************************************
 * SoundMediaHeaderBox
 */

bool SoundMediaHeaderBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    reader.read(balance_, 16);
    reader.skipBytes(2); // const unsigned int(16) reserved = 0;

    return reader.good();
}

void SoundMediaHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(balance_);
}

/*******************************************************************************
 * DataInformationBox
 */

DEFINE_CHILD_BOXES(DataInformationBox);
    STD_CHILD_BOX(DataReferenceBox, MANDATORY_UNIQUE);
DEFINE_CHILD_BOXES_END(DataInformationBox);

void DataInformationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool DataInformationBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(DataInformationBox), childBoxes_, reader);
}

void DataInformationBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * DataReferenceBox
 */

DEFINE_CHILD_BOXES(DataReferenceBox);
    STD_CHILD_BOX(DataEntryUrlBox, OPTIONAL);
    STD_CHILD_BOX(DataEntryUrnBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(DataReferenceBox);

void DataReferenceBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool DataReferenceBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(entry_count_, 32);

    bool res = readBoxes(context,
                         CHILD_BOXES(DataReferenceBox),
                         childBoxes_,
                         reader);

    if(res && entry_count_ != childBoxes_.size())
        return false;

    return res;
}

void DataReferenceBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * DataEntryUrlBox
 */

bool DataEntryUrlBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    return reader.good();
}

/*******************************************************************************
 * DataEntryUrnBox
 */

bool DataEntryUrnBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    return reader.good();
}

/*******************************************************************************
 * SampleTableBox
 */

DEFINE_CHILD_BOXES(SampleTableBox);
    STD_CHILD_BOX(SampleDescriptionBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(TimeToSampleBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(CompositionOffsetBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(SampleSizeBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(SampleToChunkBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(ChunkOffsetBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(SampleToGroupBox, OPTIONAL);
    STD_CHILD_BOX(SampleGroupDescriptionBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(SampleTableBox);

void SampleTableBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool SampleTableBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(SampleTableBox), childBoxes_, reader);
}

void SampleTableBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * SampleDescriptionBox
 */

DEFINE_SAMPLE_ENTRY_TYPE(VisualSampleEntry, MANDATORY);
DEFINE_SAMPLE_ENTRY_TYPE(AudioSampleEntry, MANDATORY);

void SampleDescriptionBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool SampleDescriptionBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(entry_count_, 32);

    uint32_t handlerType = context.currentTrackContext()->handlerType();
    bool res = false;

    if(handlerType == HandlerBox::VideoTrackType_)
    {
        res = readBoxes(context,
                        SAMPLE_ENTRY_TYPE(VisualSampleEntry),
                        childBoxes_,
                        reader);
    }
    else if(handlerType == HandlerBox::AudioTrackType_)
    {
        res = readBoxes(context,
                        SAMPLE_ENTRY_TYPE(AudioSampleEntry),
                        childBoxes_,
                        reader);
    }
    else if(handlerType == HandlerBox::HintTrackType_)
    {

    }
    else if(handlerType == HandlerBox::TimedMetadataTrackType_)
    {

    }

    if(res && entry_count_ != childBoxes_.size())
        return false;

    return res;
}

void SampleDescriptionBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * VisualSampleEntry
 */

DEFINE_CHILD_BOXES(VisualSampleEntry);
    STD_CHILD_BOX(PixelAspectRatioBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(ColourInformationBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(AVCConfigurationBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(MVCConfigurationBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(HEVCConfigurationBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(DolbyVisionConfigurationBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(ESDBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(ProtectionSchemeInfoBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(VisualSampleEntry);

void VisualSampleEntry::clearSpecifics()
{
    childBoxes_.clear();
}

bool VisualSampleEntry::readSpecifics(Reader& reader, Context& context)
{
    char compressorname[32];

    reader.skipBytes(16); // unsigned int(16) pre_defined = 0;
                          // const unsigned int(16) reserved = 0;
                          // unsigned int(32)[3] pre_defined = 0;

    reader.read(width_, 16);
    reader.read(height_, 16);
    reader.read(horizresolution_, 32);
    reader.read(vertresolution_, 32);
    reader.skipBytes(4); // const unsigned int(32) reserved = 0;
    reader.read(frame_count_, 16);

    // compressorname
    for(int i =0; i<32; i++){
        reader.read(compressorname[i], 8);
    }
    compressorname[31] = 0;

    reader.read(depth_, 16);

    // pre_defined
    int16_t pre_defined;
    reader.read(pre_defined, 16);

    PRINT("VisualSampleEntry::%s\n", __func__);
    PRINT("width_ %d, height_ %d\n", width_, height_);
    PRINT("horizresolution 0x%x\n", horizresolution_ );
    PRINT("vertresolution_ 0x%x\n", vertresolution_ );
    PRINT("compressorname %s\n", compressorname);
    PRINT("pre_defined %d\n", pre_defined);

    if(reader.good())
    {
        context.currentTrackContext()->setSampleType(boxType_);
        context.currentTrackContext()->setVideoImageSize(width_, height_);
    }

    return readBoxes(context, CHILD_BOXES(VisualSampleEntry), childBoxes_, reader);
}

void VisualSampleEntry::dumpSpecifics(Dumper& dumper) const
{
    DUMP(width_);
    DUMP(height_);
    DUMP(horizresolution_);
    DUMP(vertresolution_);
    DUMP(frame_count_);
    DUMP(depth_);

    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * PixelAspectRatioBox
 */

bool PixelAspectRatioBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(hSpacing_, 32);
    reader.read(vSpacing_, 32);

    if(reader.good())
        context.currentTrackContext()->setVideoPixelAspectRatio(hSpacing_, vSpacing_);

    return reader.good();
}

void PixelAspectRatioBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(hSpacing_);
    DUMP(vSpacing_);
}

/*******************************************************************************
 * ColourInformationBox
 */
bool ColourInformationBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(colourType_, 32);

    if (colourType_ == buildFourCC("nclx") ){
        reader.read(colourPrimaries_, 16);
        reader.read(transferCharacteristics_, 16);
        reader.read(matrixCoefficients_, 16);
        reader.read(fullRangeFlag_, 1);
        reader.read(reserved_, 7);

        if(reader.good()){
            context.currentTrackContext()->setColorInformation( static_cast<int32_t> (colourPrimaries_),
                                                                static_cast<int32_t> (transferCharacteristics_),
                                                                static_cast<int32_t> (matrixCoefficients_),
                                                                static_cast<int32_t> (fullRangeFlag_));
        }
    } else {
        // rICC or prof : not supported
        context.currentTrackContext()->setColorInformation(-1, -1, -1, 0);
    }

    return reader.good();
}

void ColourInformationBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(colourType_);
    DUMP(colourPrimaries_);
    DUMP(transferCharacteristics_);
    DUMP(matrixCoefficients_);
    DUMP(fullRangeFlag_);
}


/*******************************************************************************
 * AudioSampleEntry
 */

DEFINE_CHILD_BOXES(AudioSampleEntry);
    STD_CHILD_BOX(ESDBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(EC3SpecificBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(OggVorbisSpecificInfo, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(ProtectionSchemeInfoBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(AudioSampleEntry);

void AudioSampleEntry::clearSpecifics()
{
    childBoxes_.clear();
}

bool AudioSampleEntry::readSpecifics(Reader& reader, Context& context)
{
    reader.skipBytes(8); // const unsigned int(32)[2] reserved = 0;
    reader.read(channelcount_, 16);
    reader.read(samplesize_, 16);

    reader.skipBytes(4); // unsigned int(16) pre_defined = 0;
                         // const unsigned int(16) reserved = 0;

    reader.read(samplerate_, 32);

    if(reader.good())
    {
        context.currentTrackContext()->setSampleType(boxType_);
        context.currentTrackContext()->setAudioSampleInfo(channelcount_,
                                                          samplesize_,
                                                          samplerate_);
    }

    return readBoxes(context, CHILD_BOXES(AudioSampleEntry), childBoxes_, reader);
}

void AudioSampleEntry::dumpSpecifics(Dumper& dumper) const
{
    DUMP(channelcount_);
    DUMP(samplesize_);
    DUMP(samplerate_);

    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * ProtectionSchemeInfoBox
 */

DEFINE_CHILD_BOXES(ProtectionSchemeInfoBox);
    STD_CHILD_BOX(OriginalFormatBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(SchemeTypeBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(SchemeInformationBox, OPTIONAL_UNIQUE);
DEFINE_CHILD_BOXES_END(ProtectionSchemeInfoBox);

void ProtectionSchemeInfoBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool ProtectionSchemeInfoBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context,
                     CHILD_BOXES(ProtectionSchemeInfoBox),
                     childBoxes_,
                     reader);
}

void ProtectionSchemeInfoBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * OriginalFormatBox
 */

bool OriginalFormatBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(data_format_, 32);

    if(reader.good())
        context.currentTrackContext()->setSampleType(data_format_);

    return reader.good();
}

void OriginalFormatBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP_FOURCC(data_format_);
}

/*******************************************************************************
 * SchemeTypeBox
 */

bool SchemeTypeBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(scheme_type_, 32);
    reader.read(scheme_version_, 32);

    // if (flags & 0x000001) { unsigned int(8) scheme_uri[]; }

    if(reader.good())
        context.currentTrackContext()->setCurrProtectionScheme(scheme_type_);

    return reader.good();
}

void SchemeTypeBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP_FOURCC(scheme_type_);
    DUMP(scheme_version_);
}

/*******************************************************************************
 * SchemeInformationBox
 */

DEFINE_CHILD_BOXES(SchemeInformationBox);
    DUAL_CHILD_BOX(TrackEncryptionBox, OPTIONAL_UNIQUE);
DEFINE_CHILD_BOXES_END(SchemeInformationBox);

void SchemeInformationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool SchemeInformationBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(SchemeInformationBox), childBoxes_, reader);
}

void SchemeInformationBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * TimeToSampleBox
 */

bool TimeToSampleBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(entry_count_, 32);

    if(!reader.good())
        return false;

    uint32_t i;
    for(i = 0; i < entry_count_; i ++)
    {
        TrackContext::TimeToSampleEntry entry;

        reader.read(entry.sample_count_, 32);
        reader.read(entry.sample_delta_, 32);

        if(!reader.good())
            return false;

        context.currentTrackContext()->timeToSampleEntries().push_back(entry);
    }

    return reader.good();
}

void TimeToSampleBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
}

/*******************************************************************************
 * CompositionOffsetBox
 */

bool CompositionOffsetBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(entry_count_, 32);

    if(!reader.good())
        return false;

    uint32_t i;
    for(i = 0; i < entry_count_; i ++)
    {
        TrackContext::CompositionOffsetEntry entry;

        reader.read(entry.sample_count_, 32);
        reader.read(entry.sample_offset_, 32);

        if(!reader.good())
            return false;

        context.currentTrackContext()->compositionOffsetEntries().push_back(entry);
    }

    return reader.good();
}

void CompositionOffsetBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
}

/*******************************************************************************
 * SampleSizeBox
 */

bool SampleSizeBox::readSpecifics(Reader& reader, Context& context)
{
    if(context.currentTrackContext()->sampleSizeEntries().size() > 0)
    {
        context.setParserState(Context::PARSE_ERROR_DUPLICATED_OBJECT);
        return false;
    }

    reader.read(sample_size_, 32);
    reader.read(sample_count_, 32);

    if(!reader.good())
        return false;

    // Nonzero 'sample_size_' means universal size exists
    if(sample_size_ != 0)
    {
        context.currentTrackContext()->setUniversalSampleSize(true);
        context.currentTrackContext()->sampleSizeEntries().push_back(sample_size_);
        return true;
    }

    context.currentTrackContext()->setUniversalSampleSize(false);

    uint32_t i;
    for(i = 0; i < sample_count_; i ++)
    {
        uint32_t size;
        reader.read(size, 32);

        if(!reader.good())
            return false;

        context.currentTrackContext()->sampleSizeEntries().push_back(size);
    }

    return reader.good();
}

void SampleSizeBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(sample_size_);
    DUMP(sample_count_);
}

/*******************************************************************************
 * SampleToChunkBox
 */

bool SampleToChunkBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(entry_count_, 32);

    if(!reader.good())
        return false;

    uint32_t i;
    for(i = 0; i < entry_count_; i ++)
    {
        TrackContext::SampleToChunkEntry entry;

        reader.read(entry.first_chunk_, 32);
        reader.read(entry.samples_per_chunk_, 32);
        reader.read(entry.sample_description_index_, 32);

        if(!reader.good())
            return false;

        context.currentTrackContext()->sampleToChunkEntries().push_back(entry);
    }

    return reader.good();
}

void SampleToChunkBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
}

/*******************************************************************************
 * ChunkOffsetBox
 */

bool ChunkOffsetBox::readSpecifics(Reader& reader, Context& context)
{
    if(context.currentTrackContext()->chunkOffsetEntries().size() > 0)
    {
        context.setParserState(Context::PARSE_ERROR_DUPLICATED_OBJECT);
        return false;
    }

    reader.read(entry_count_, 32);

    if(!reader.good())
        return false;

    uint32_t i;
    for(i = 0; i < entry_count_; i ++)
    {
        uint32_t offset;
        reader.read(offset, 32);

        if(!reader.good())
            return false;

        context.currentTrackContext()->chunkOffsetEntries().push_back(offset);
    }

    return reader.good();
}

void ChunkOffsetBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(entry_count_);
}

/*******************************************************************************
 * SampleToGroupDescriptionBox
 */
bool SampleGroupDescriptionBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(grouping_type_, 32);
    Dumper::dumpFourCC(grouping_type_);

    if(version_ == 1) {
	reader.read(default_length_, 32);
    } else {
	default_length_ = 0;
    }

    reader.read(entry_count_, 32);

    if( grouping_type_ == buildFourCC("scnm") ) {
        if(context.currentTrackContext()->handlerType() !=  buildFourCC("vide")) {
            return false;
        }

        // initialize scnm context
        context.currentTrackContext()->naluToMvcViewMaps().clear();
        context.currentTrackContext()->naluToMvcViewMaps().resize(entry_count_);

        // loop for each entry to complete context
        for(uint32_t i = 0; i < entry_count_; i++) {
            if(version_ == 1 && default_length_ == 0) {
                reader.read(description_length_, 32);
            }
            shared_ptr<ScalableNALUMapEntry> scalableNALUMapEntry(new ScalableNALUMapEntry);
            readSampleGroupEntries(context, scalableNALUMapEntry, i, reader);
        }
    }else if(grouping_type_ ==  buildFourCC("sync") ){
        if(context.currentTrackContext()->handlerType() !=  buildFourCC("vide")) {
            return false;
        }

        // initialize sync context

        // loop for each entry to complete context
        for(uint32_t i = 0; i < entry_count_; i++) {
            if(version_ == 1 && default_length_ == 0) {
                reader.read(description_length_, 32);
            }
            shared_ptr<SyncSampleEntry> syncSampleEntry(new SyncSampleEntry);
            readSampleGroupEntries(context, syncSampleEntry, i, reader);
        }
    }else if(grouping_type_ == buildFourCC("tscl") ) {
        if(context.currentTrackContext()->handlerType() !=  buildFourCC("vide")) {
            return false;
        }


        // initialize temporal layer context
        context.currentTrackContext()->temporalLayerEntries().clear();
        context.currentTrackContext()->temporalLayerEntries().resize(entry_count_);

        // loop for each entry to complete context
        for(uint32_t i = 0; i < entry_count_; i++) {
            if(version_ == 1 && default_length_ == 0) {
                reader.read(description_length_, 32);
            }
            shared_ptr<TemporalLayerEntry> temporalLayerEntry(new TemporalLayerEntry);
            readSampleGroupEntries(context, temporalLayerEntry, i, reader);
        }
    } else if (grouping_type_ == buildFourCC("tele")) {
        if(context.currentTrackContext()->handlerType() !=  buildFourCC("vide")) {
            return false;
        }

        // initialize temporal level context
        context.currentTrackContext()->temporalLevelEntries().clear();
        context.currentTrackContext()->temporalLevelEntries().resize(entry_count_);

        // loop for each entry to complete context
        for(uint32_t i = 0; i < entry_count_; i++) {
            if(version_ == 1 && default_length_ == 0) {
                reader.read(description_length_, 32);
            }
            shared_ptr<TemporalLevelEntry> temporalLevelEntry(new TemporalLevelEntry);
            readSampleGroupEntries(context, temporalLevelEntry, i, reader);
        }

    } else if (grouping_type_ == buildFourCC("tsas")) {
        if(context.currentTrackContext()->handlerType() !=  buildFourCC("vide")) {
            return false;
        }
        // initialize temporal sub-layer sample group entry context


    } else {
        /*
         * Skipping unkwon sample group enties
         */
        PRINT("SampleGroupDescriptionBox::%s", __func__);
        PRINT("SampleGroupDescriptionBox: unknown sample group entry : grouping_type_ [%d]\n", grouping_type_);
        PRINT("current stream position %lld\n", reader.tell());
        PRINT("current box start offset %lld\n", startOffset_);
        PRINT("current box size %lld\n", boxSize_);
        PRINT("SampleGroupDescriptionBox: skipping %lld bytes\n", boxSize_ - (reader.tell() - startOffset_) );

        reader.skipBytes(boxSize_ - (reader.tell() - startOffset_));
    }

    return true;
}

void SampleGroupDescriptionBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(grouping_type_);
    DUMP(default_length_);
    DUMP(entry_count_);
}

/*******************************************************************************
 * SampleToGroupBox
 */
bool SampleToGroupBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(grouping_type_, 32);

    reader.read(entry_count_, 32);
    uint32_t sample_count, group_description_index;
    uint32_t sampleIndex = 0;
    for(uint32_t i = 0; i < entry_count_; i++) {
	reader.read(sample_count, 32);
	reader.read(group_description_index, 32);
	for(uint32_t j = 0; j < sample_count; j++, sampleIndex++) {

	    if(context.currentTrackFragmentContext()->sampleEntries().size() <= sampleIndex) {
                // There are more sample to sample group entries than there are samples in the TRUN.
                // XXX add NTRACE. Assert().
                assert(0);
		return reader.good();
	    }

	    context.currentTrackFragmentContext()->
		sampleEntries()[sampleIndex].sampleGroup_ = group_description_index;

            if(grouping_type_ ==  buildFourCC("tscl")) {
                context.currentTrackFragmentContext()->
                    sampleEntries()[sampleIndex].temporalLayerId_ = group_description_index;
            }

            if(grouping_type_ ==  buildFourCC("tsas")) {
                context.currentTrackFragmentContext()->
                    sampleEntries()[sampleIndex].temporalLayerAccessSample_ = group_description_index;
            }
	}
    }

    return reader.good();
}

void SampleToGroupBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(grouping_type_);
    DUMP(entry_count_);
}
/*******************************************************************************
 * MovieExtendsBox
 */

DEFINE_CHILD_BOXES(MovieExtendsBox);
    STD_CHILD_BOX(MovieExtendsHeaderBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(TrackExtendsBox, MANDATORY);
DEFINE_CHILD_BOXES_END(MovieExtendsBox);

void MovieExtendsBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MovieExtendsBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(MovieExtendsBox), childBoxes_, reader);
}

void MovieExtendsBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * MovieExtendsHeaderBox
 */

bool MovieExtendsHeaderBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    if(version_ == 1)
    {
        reader.read(fragment_duration_, 64);
    }
    else if(version_ == 0)
    {
        reader.read(fragment_duration_, 32);
    }
    else
    {
        return false;
    }

    return reader.good();
}

void MovieExtendsHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(fragment_duration_);
}

/*******************************************************************************
 * TrackExtendsBox
 */

bool TrackExtendsBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(track_ID_, 32);
    reader.read(default_sample_description_index_, 32);
    reader.read(default_sample_duration_, 32);
    reader.read(default_sample_size_, 32);
    reader.read(default_sample_flags_, 32);

    if(reader.good())
    {
        shared_ptr<TrackContext> trackContext = context.findTrackContext(track_ID_);

        if(trackContext.get() == NULL)
            return false;

        trackContext->setDefaultSampleInfo(default_sample_description_index_,
                                           default_sample_duration_,
                                           default_sample_size_,
                                           default_sample_flags_);
    }

    return reader.good();
}

void TrackExtendsBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(track_ID_);
    DUMP(default_sample_description_index_);
    DUMP(default_sample_duration_);
    DUMP(default_sample_size_);
    DUMP(default_sample_flags_);
}

/*******************************************************************************
 * MovieFragmentBox
 */

DEFINE_CHILD_BOXES(MovieFragmentBox);
    STD_CHILD_BOX(MovieFragmentHeaderBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(TrackFragmentBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(MovieFragmentBox);

void MovieFragmentBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MovieFragmentBox::readSpecifics(Reader& reader, Context& context)
{
    context.movieFragmentParseBegin(startOffset_, boxSize_);

    if(context.parserState() != Context::PARSE_CONTINUE)
        return false;

    if(!readBoxes(context, CHILD_BOXES(MovieFragmentBox), childBoxes_, reader))
        return false;

    context.movieFragmentParseEnd();
    return true;
}

void MovieFragmentBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * MovieFragmentHeaderBox
 */

bool MovieFragmentHeaderBox::readSpecifics(Reader& reader, Context& /*context*/)
{
    reader.read(sequence_number_, 32);
    return reader.good();
}

void MovieFragmentHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(sequence_number_);
}

/*******************************************************************************
 * TrackFragmentBox
 */

DEFINE_CHILD_BOXES(TrackFragmentBox);
    STD_CHILD_BOX(TrackFragmentHeaderBox, MANDATORY_UNIQUE);
    STD_CHILD_BOX(TrackRunBox, OPTIONAL);
    DUAL_CHILD_BOX(SampleEncryptionBox, OPTIONAL_UNIQUE);
    STD_CHILD_BOX(DisposableSamplesBox, OPTIONAL);
    STD_CHILD_BOX(SampleToGroupBox, OPTIONAL);
    STD_CHILD_BOX(SampleGroupDescriptionBox, OPTIONAL);
    STD_CHILD_BOX(TrackFragmentDecodeTimeBox, OPTIONAL);
DEFINE_CHILD_BOXES_END(TrackFragmentBox);

void TrackFragmentBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool TrackFragmentBox::readSpecifics(Reader& reader, Context& context)
{
    context.trackFragmentParseBegin(startOffset_, boxSize_);

    if(context.parserState() != Context::PARSE_CONTINUE)
        return false;

    if(!readBoxes(context, CHILD_BOXES(TrackFragmentBox), childBoxes_, reader))
        return false;

#if 0
    unsigned int i;
    int j,k,l;
    shared_ptr<TrackFragmentContext> currFragContext = context.currentTrackFragmentContext();
    for(i = 0, j = 0, k = 0, l = 0 ; i < currFragContext->sampleEntries().size(); i ++)
    {
      TrackFragmentContext::SampleEntry& entry = currFragContext->sampleEntries()[i];
      PRINT("entry[%d]:\t startOffset_ %lld,\t size %u,\t temporalId %d\n",
            i, entry.startOffset_, entry.sampleSize_, entry.temporalLayerId_);

      if(entry.temporalLayerId_ == 0){
          j++;
      } else if (entry.temporalLayerId_ == 1) {
          k++;
      } else if (entry.temporalLayerId_ == 2) {
          l++;
      }
    }
    PRINT("TEMPORAL: id0 %d, id1 %d, id2 %d (out of total samples of %u)\n", j,k,l,i);
#endif

    context.trackFragmentParseEnd();

    return (context.parserState() == Context::PARSE_STOP);
}

void TrackFragmentBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * TrackFragmentHeaderBox
 */

bool TrackFragmentHeaderBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(track_ID_, 32);

    if(isFlagBitSet(base_data_offset_present_bit_))
        reader.read(base_data_offset_, 64);

    if(isFlagBitSet(sample_description_index_present_bit_))
        reader.read(sample_description_index_, 32);

    if(isFlagBitSet(default_sample_duration_present_bit_))
        reader.read(default_sample_duration_, 32);

    if(isFlagBitSet(default_sample_size_present_bit_))
        reader.read(default_sample_size_, 32);

    if(isFlagBitSet(default_sample_flags_present_bit_))
        reader.read(default_sample_flags_, 32);

    if(reader.good())
    {
        uint32_t sampleDescriptionIndex = 0;
        uint32_t defaultSampleDuration = 0;
        uint32_t defaultSampleSize = 0;
        uint32_t defaultSampleFlags = 0;

        shared_ptr<TrackContext> trackContext = context.findTrackContext(track_ID_);

        if(trackContext.get())
        {
            trackContext->getDefaultSampleInfo(sampleDescriptionIndex,
                                               defaultSampleDuration,
                                               defaultSampleSize,
                                               defaultSampleFlags);
        }

        shared_ptr<TrackFragmentContext> currFragContext =
            context.currentTrackFragmentContext();

        currFragContext->setTrackID(track_ID_);

        if(isFlagBitSet(base_data_offset_present_bit_))
            context.movieFragmentContext()->setBaseDataOffset(base_data_offset_);

        if(isFlagBitSet(sample_description_index_present_bit_))
            sampleDescriptionIndex = sample_description_index_;

        if(isFlagBitSet(default_sample_duration_present_bit_))
            defaultSampleDuration = default_sample_duration_;

        if(isFlagBitSet(default_sample_size_present_bit_))
            defaultSampleSize = default_sample_size_;

        if(isFlagBitSet(default_sample_flags_present_bit_))
            defaultSampleFlags = default_sample_flags_;

        currFragContext->setSampleInfo(sampleDescriptionIndex,
                                       defaultSampleDuration,
                                       defaultSampleSize,
                                       defaultSampleFlags,
                                       isFlagBitSet(duration_is_empty_bit_));

        currFragContext->setCurrentDataOffset(
            context.movieFragmentContext()->baseDataOffset());
    }

    return reader.good();
}

void TrackFragmentHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(track_ID_);

    if(isFlagBitSet(base_data_offset_present_bit_))
        DUMP(base_data_offset_);

    if(isFlagBitSet(sample_description_index_present_bit_))
        DUMP(sample_description_index_);

    if(isFlagBitSet(default_sample_duration_present_bit_))
        DUMP(default_sample_duration_);

    if(isFlagBitSet(default_sample_size_present_bit_))
        DUMP(default_sample_size_);

    if(isFlagBitSet(default_sample_flags_present_bit_))
        DUMP_BIN(default_sample_flags_);
}

/*******************************************************************************
 * TrackRunBox
 */
bool TrackRunBox::readSpecifics(Reader& reader, Context& context)
{
    shared_ptr<TrackFragmentContext> currFragContext = context.currentTrackFragmentContext();

    if(currFragContext->trackID() == 0)
        return false;

    // sample_count in this trun. One fragment can have multiple truns. One trun
    // can have multiple samples
    reader.read(sample_count_, 32);

    // data_offset is distance from start of moof to 1st data byte of mdat
    if(isFlagBitSet(data_offset_present_bit_))
        reader.read(data_offset_, 32);

    if(isFlagBitSet(first_sample_flags_present_bit_))
    {
        if(isFlagBitSet(sample_flags_present_bit_))
            return false;

        reader.read(first_sample_flags_, 32);
    }

    if(!reader.good())
        return false;

    uint32_t sampleDescriptionIndex = 0;
    uint32_t sampleDuration = 0;
    uint32_t sampleSize = 0;
    uint32_t sampleFlags = 0;
    bool durationIsEmpty = false;

    currFragContext->getSampleInfo(sampleDescriptionIndex,
                                   sampleDuration,
                                   sampleSize,
                                   sampleFlags,
                                   durationIsEmpty);

    ullong offset;
    offset = currFragContext->currentDataOffset();

    if(isFlagBitSet(data_offset_present_bit_))
        offset = context.movieFragmentContext()->baseDataOffset() + data_offset_;

    ullong DTS = currFragContext->currentDTS();
    uint32_t sampleCompositionTimeOffset = 0;
    for(uint32_t i = 0; i < sample_count_; i ++)
    {
        TrackFragmentContext::SampleEntry entry;
        if(isFlagBitSet(sample_duration_present_bit_))
            reader.read(sampleDuration, 32);

        if(isFlagBitSet(sample_size_present_bit_))
            reader.read(sampleSize, 32);

        if(isFlagBitSet(sample_flags_present_bit_))
            reader.read(sampleFlags, 32);

        if(isFlagBitSet(sample_composition_time_offsets_present_bit_))
            reader.read(sampleCompositionTimeOffset, 32);

        if(!reader.good())
            return false;

        entry.startOffset_ = offset;
        entry.sampleDescriptionIndex_ = sampleDescriptionIndex;
        entry.sampleSize_ = sampleSize;
        entry.sampleFlags_ = sampleFlags;
        entry.DTS_ = DTS;
        entry.CTS_ = DTS + sampleCompositionTimeOffset;
        entry.sampleCompositionTimeOffset_ = sampleCompositionTimeOffset;
        entry.sampleDuration_ = sampleDuration;
        entry.sampleGroup_ = 0;

        //PRINT("sample offset %d \n", offset );
        //PRINT("sample size %d \n", sampleSize );

        if(i == 0 && isFlagBitSet(first_sample_flags_present_bit_))
            entry.sampleFlags_ = first_sample_flags_;

        offset += sampleSize;
        DTS += sampleDuration;

        currFragContext->sampleEntries().push_back(entry);
    }

    currFragContext->setCurrentDataOffset(offset);
    currFragContext->setCurrentDTS(DTS);
    return reader.good();
}

void TrackRunBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(sample_count_);

    if(isFlagBitSet(data_offset_present_bit_))
        DUMP(data_offset_);

    if(isFlagBitSet(first_sample_flags_present_bit_))
        DUMP_BIN(first_sample_flags_);
}

/*******************************************************************************
 * IndependentAndDisposableSamplesBox
 */
bool DisposableSamplesBox::readSpecifics(Reader& reader,
                                                       Context& context)
{
    std::vector<netflix::containerlib::mp4parser::TrackFragmentContext::SampleEntry>& sampleEntries =
        context.currentTrackFragmentContext()->sampleEntries();

    shared_ptr<TrackFragmentContext> currFragContext =
        context.currentTrackFragmentContext();

    uint32_t i;
    char flags;
    for(i = 0; i < sampleEntries.size(); i++)
    {
        reader.read(flags, 8);
        sampleEntries[i].independentAndDisposableFlags_ = flags;
    }

    return reader.good();
}

void DisposableSamplesBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(sample_count_);
}

/*******************************************************************************
 * MovieFragmentRandomAccessBox
 */

DEFINE_CHILD_BOXES(MovieFragmentRandomAccessBox);
    STD_CHILD_BOX(TrackFragmentRandomAccessBox, OPTIONAL);
    STD_CHILD_BOX(MovieFragmentRandomAccessOffsetBox, MANDATORY_UNIQUE);
DEFINE_CHILD_BOXES_END(MovieFragmentRandomAccessBox);

void MovieFragmentRandomAccessBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MovieFragmentRandomAccessBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context,
                     CHILD_BOXES(MovieFragmentRandomAccessBox),
                     childBoxes_,
                     reader);
}

void MovieFragmentRandomAccessBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * TrackFragmentRandomAccessBox
 */

bool TrackFragmentRandomAccessBox::readSpecifics(Reader& reader, Context& context)
{
    uint32_t timeAndOffsetLength;

    if(version_ == 1)
        timeAndOffsetLength = 64;
    else if(version_ == 0)
        timeAndOffsetLength = 32;
    else
        return false;

    reader.read(track_ID_, 32);
    reader.skipBits(26); // const unsigned int(26) reserved = 0;
    reader.read(length_size_of_traf_num_, 2);
    reader.read(length_size_of_trun_num_, 2);
    reader.read(length_size_of_sample_num_, 2);
    reader.read(number_of_entry_, 32);

    if(!reader.good())
        return false;

    uint32_t trafNumLength = (length_size_of_traf_num_ + 1) * 8;
    uint32_t trunNumLength = (length_size_of_trun_num_ + 1) * 8;
    uint32_t sampleNumLength = (length_size_of_sample_num_ + 1) * 8;

    uint32_t i;
    for(i = 0; i < number_of_entry_; i ++)
    {
        TrackIndexContext::IndexEntry entry;

        reader.read(entry.time_, timeAndOffsetLength);
        reader.read(entry.moofOffset_, timeAndOffsetLength);
        reader.read(entry.trafNumber_, trafNumLength);
        reader.read(entry.trunNumber_, trunNumLength);
        reader.read(entry.sampleNumber_, sampleNumLength);

        if(!reader.good())
            return false;

        if(context.currentTrackIndexContext().get())
            context.currentTrackIndexContext()->indexEntries().push_back(entry);
    }

    if(context.currentTrackIndexContext().get())
        context.currentTrackIndexContext()->setTrackID(track_ID_);

    return true;
}

void TrackFragmentRandomAccessBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(track_ID_);
    DUMP(length_size_of_traf_num_);
    DUMP(length_size_of_trun_num_);
    DUMP(length_size_of_sample_num_);
    DUMP(number_of_entry_);
}

/*******************************************************************************
 * MovieFragmentRandomAccessOffsetBox
 */

bool MovieFragmentRandomAccessOffsetBox::readSpecifics(
        Reader& reader, Context& /*context*/)
{
    reader.read(size_, 32);
    return reader.good();
}

void MovieFragmentRandomAccessOffsetBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(size_);
}


/*******************************************************************************
 * Track fragment base media decode time box
 */

bool TrackFragmentDecodeTimeBox::readSpecifics(Reader& reader, Context& context)
{
    if(version_ == 1)
    {
        reader.read(baseMediaDecodeTime_, 64);
    }
    else if(version_ == 0)
    {
        reader.read(baseMediaDecodeTime_, 32);
    }
    else
    {
        return false;
    }

    if(context.currentTrackFragmentContext().get())
        context.currentTrackFragmentContext()->setBaseMediaDecodeTime(baseMediaDecodeTime_);

    return reader.good();

}

void TrackFragmentDecodeTimeBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(baseMediaDecodeTime_);
}

/*******************************************************************************
 * EditBox
 */
DEFINE_CHILD_BOXES(EditBox);
    STD_CHILD_BOX(EditListBox, OPTIONAL_UNIQUE);
DEFINE_CHILD_BOXES_END(EditBox);

bool EditBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context,
                     CHILD_BOXES(EditBox),
                     childBoxes_,
                     reader);
}

/*******************************************************************************
 * EditListBox
 */
bool EditListBox::readSpecifics(Reader& reader, Context& context)
{

    uint32_t entry_count;
    reader.read(entry_count, 32);

    for(uint32_t i = 0; i < entry_count; i ++)
    {
        TrackContext::EditListEntry entry;

        if(version_ == 1)
        {
            reader.read(entry.segment_duration_, 64);
            reader.read(entry.media_time_, 64);
        }
        else if(version_ == 0)
        {
            reader.read(entry.segment_duration_, 32);
            reader.read(entry.media_time_, 32);
        }
        reader.read(entry.media_rate_integer_, 16);
        reader.read(entry.media_rate_fraction_, 16);

        context.currentTrackContext()->editListEntries().push_back(entry);
    }

    return reader.good();
}
