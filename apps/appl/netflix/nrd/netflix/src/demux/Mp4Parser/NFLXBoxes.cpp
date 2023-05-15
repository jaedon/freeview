/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "NFLXBoxes.h"

#include "Context.h"

#include "ISOBoxes.h"
#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;
using netflix::containerlib::mp4parser::UUID;

/*******************************************************************************
 * Table of Box Types
 */

DEFINE_EXT_BOXTYPE(NetflixHeaderBox,
    0x6674654E, 0x696C, 0x5078, 0x6966, 0x665374726D21LL);

DEFINE_EXT_BOXTYPE(NetflixIndexBox,
    0x6674654E, 0x696C, 0x5078, 0x6966, 0x66496E646578LL);

DEFINE_EXT_BOXTYPE(NetflixTrackIndexBox,
    0x6674654E, 0x696C, 0x5478, 0x7261, 0x6B496E646578LL);

DEFINE_STD_BOXTYPE(OggVorbisSpecificInfo, "ovsp");
DEFINE_STD_BOXTYPE(EC3SpecificBox, "dec3");

/*******************************************************************************
 * NetflixHeaderBox
 */

bool NetflixHeaderBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(fileSize_, 64);
    reader.read(timescale_, 64);
    reader.read(duration_, 64);
    reader.read(offsetToFirstMoof_, 64);
    reader.read(sizeOfAllMoofAndMDat_, 64);

    if(reader.good())
    {
        context.netflixHeaderFound(fileSize_,
                                   timescale_,
                                   duration_,
                                   offsetToFirstMoof_,
                                   sizeOfAllMoofAndMDat_);
    }

    return reader.good();
}

void NetflixHeaderBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(offsetToFirstMoof_);
}

/*******************************************************************************
 * NetflixIndexBox
 */

DEFINE_CHILD_BOXES(NetflixIndexBox);
    EXT_CHILD_BOX(NetflixTrackIndexBox, MANDATORY);
DEFINE_CHILD_BOXES_END(NetflixIndexBox);

void NetflixIndexBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool NetflixIndexBox::readSpecifics(Reader& reader, Context& context)
{
    return readBoxes(context, CHILD_BOXES(NetflixIndexBox), childBoxes_, reader);
}

void NetflixIndexBox::dumpSpecifics(Dumper& dumper) const
{
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * NetflixTrackIndexBox
 */

DEFINE_CHILD_BOXES(NetflixTrackIndexBox);
    STD_CHILD_BOX(TrackFragmentRandomAccessBox, MANDATORY_UNIQUE);
DEFINE_CHILD_BOXES_END(NetflixTrackIndexBox);

void NetflixTrackIndexBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool NetflixTrackIndexBox::readSpecifics(Reader& reader, Context& context)
{
    context.trackIndexParseBegin(startOffset_, boxSize_);

    if(context.parserState() != Context::PARSE_CONTINUE)
        return false;

    reader.read(timescale_, 64);

    if(reader.good())
        context.currentTrackIndexContext()->setTimescale(timescale_);

    if(!readBoxes(context, CHILD_BOXES(NetflixTrackIndexBox), childBoxes_, reader))
        return false;

    context.trackIndexParseEnd();
    return true;
}

void NetflixTrackIndexBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(timescale_);
    dumpBoxes(childBoxes_, dumper);
}

/*******************************************************************************
 * OggVorbisSpecificInfo
 */

bool OggVorbisSpecificInfo::readSpecifics(Reader& reader, Context& context)
{
    uint32_t headerSize = static_cast<uint32_t>(reader.tell() - startOffset_);

    if(boxSize_ < headerSize)
        return false;

    uint32_t remainingSize = boxSize_ - headerSize;

    if(remainingSize > 0)
    {
        context.currentTrackContext()->decoderSpecificData().resize(remainingSize);
        reader.readBytes(context.currentTrackContext()->decoderSpecificData());
    }

    return reader.good();
}

/*******************************************************************************
 * EC3SpecificBox
 */

bool EC3SpecificBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(data_rate_, 13);
    reader.read(num_ind_sub_, 3);

    for (uint32_t i = 0; i <= num_ind_sub_; i++) {
        reader.read(ind_sub_stream_[i].fscod_, 2);
        reader.read(ind_sub_stream_[i].bsid_, 5);
        reader.read(ind_sub_stream_[i].bsmod_, 5);
        reader.read(ind_sub_stream_[i].acmod_, 3);
        reader.read(ind_sub_stream_[i].lfeon_, 1);
        reader.read(ind_sub_stream_[i].reserved_, 3);
        reader.read(ind_sub_stream_[i].num_dep_sub_, 4);

        if (ind_sub_stream_[i].num_dep_sub_ > 0) {
            reader.read(ind_sub_stream_[i].chan_loc_, 9);
        } else {
            reader.read(ind_sub_stream_[i].reserved2_, 1);
        }
    }

    // If VBR then data_rate_ is max bitrate, if not then it is average bitrate.
    context.currentTrackContext()->setAudioElementaryStreamData(data_rate_, data_rate_);

    return reader.good();
}

