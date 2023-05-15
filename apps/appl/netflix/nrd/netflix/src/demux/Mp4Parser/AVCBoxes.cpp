/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "AVCBoxes.h"

#include "Context.h"

#include "Reader.h"
#include "Dumper.h"
#include "macros.h"

using namespace std;
using namespace netflix::containerlib::mp4parser;

/*******************************************************************************
 * Table of Box Types
 */

DEFINE_STD_BOXTYPE(AVCConfigurationBox, "avcC");
DEFINE_STD_BOXTYPE(MVCConfigurationBox, "mvcC");

/*******************************************************************************
 * AVCConfigurationBox
 */

DEFINE_CHILD_BOXES(AVCConfigurationBox);
DEFINE_CHILD_BOXES_END(AVCConfigurationBox);

void AVCConfigurationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool AVCConfigurationBox::readSpecifics(Reader& reader, Context& context)
{
    reader.read(configurationVersion_, 8);
    reader.read(AVCProfileIndication_, 8);
    reader.read(profile_compatibility_, 8);
    reader.read(AVCLevelIndication_, 8);
    reader.skipBits(6); // bit(6) reserved = '111111'b;
    reader.read(lengthSizeMinusOne_, 2);
    reader.skipBits(3); // bit(3) reserved = '111'b;

    reader.read(numOfSequenceParameterSets_, 5);



    if(!reader.good())
        return false;

    context.currentTrackContext()->setNALULengthSize(lengthSizeMinusOne_ + 1);

    uint32_t i;

    for(i = 0; i < numOfSequenceParameterSets_; i ++)
    {
        shared_ptr<TrackContext::ParameterSet>
            parameterSet(new TrackContext::ParameterSet);

        context.currentTrackContext()->
            sequenceParameterSets().push_back(parameterSet);

        uint32_t parameterSetLength;
        reader.read(parameterSetLength, 16);

        if(!reader.good())
            return false;

        parameterSet->resize(parameterSetLength);
        reader.readBytes(*parameterSet);
    }

    reader.read(numOfPictureParameterSets_, 8);

    if(!reader.good())
        return false;

    for(i = 0; i < numOfPictureParameterSets_; i ++)
    {
        shared_ptr<TrackContext::ParameterSet>
            parameterSet(new TrackContext::ParameterSet);

        context.currentTrackContext()->
            pictureParameterSets().push_back(parameterSet);

        uint32_t parameterSetLength;
        reader.read(parameterSetLength, 16);

        if(!reader.good())
            return false;

        parameterSet->resize(parameterSetLength);
        reader.readBytes(*parameterSet);
    }

    return readBoxes(context, CHILD_BOXES(AVCConfigurationBox), childBoxes_, reader);
}

void AVCConfigurationBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(configurationVersion_);
    DUMP(AVCProfileIndication_);
    DUMP(profile_compatibility_);
    DUMP(AVCLevelIndication_);
    DUMP(lengthSizeMinusOne_);
    DUMP(numOfSequenceParameterSets_);
    DUMP(numOfPictureParameterSets_);

    dumpBoxes(childBoxes_, dumper);
}


/*******************************************************************************
 * MVCConfigurationBox
 */

DEFINE_CHILD_BOXES(MVCConfigurationBox);
DEFINE_CHILD_BOXES_END(MVCConfigurationBox);

void MVCConfigurationBox::clearSpecifics()
{
    childBoxes_.clear();
}

bool MVCConfigurationBox::readSpecifics(Reader& reader, Context& context)
{
    char tmp;
    reader.read(configurationVersion_, 8);
    reader.read(AVCProfileIndication_, 8);
    reader.read(profile_compatibility_, 8);
    reader.read(AVCLevelIndication_, 8);
    reader.read(tmp, 1);
    complete_representation_ = tmp & 1;
    reader.read(tmp, 1);
    explicit_au_track_ = tmp & 1;
    reader.skipBits(4); // bit(4) reserved = '1111'b;
    reader.read(lengthSizeMinusOne_, 2);
    reader.skipBits(1); // bit(1) reserved = '0'b;

    reader.read(numOfSequenceParameterSets_, 7);

    if(!reader.good())
        return false;


    if(context.currentTrackContext()->NALULengthSize() != (uint32_t)(lengthSizeMinusOne_ + 1))
    return false;

    uint32_t i;

    for(i = 0; i < numOfSequenceParameterSets_; i ++)
    {
        shared_ptr<TrackContext::ParameterSet>
            parameterSet(new TrackContext::ParameterSet);

        context.currentTrackContext()->
            mvcSequenceParameterSets().push_back(parameterSet);

        uint32_t parameterSetLength;
        reader.read(parameterSetLength, 16);

        if(!reader.good())
            return false;

        parameterSet->resize(parameterSetLength);
        reader.readBytes(*parameterSet);
    }

    reader.read(numOfPictureParameterSets_, 8);

    if(!reader.good())
        return false;

    for(i = 0; i < numOfPictureParameterSets_; i ++)
    {
        shared_ptr<TrackContext::ParameterSet>
            parameterSet(new TrackContext::ParameterSet);

        context.currentTrackContext()->
            mvcPictureParameterSets().push_back(parameterSet);

        uint32_t parameterSetLength;
        reader.read(parameterSetLength, 16);

        if(!reader.good())
            return false;

        parameterSet->resize(parameterSetLength);
        reader.readBytes(*parameterSet);
    }

    return readBoxes(context, CHILD_BOXES(MVCConfigurationBox), childBoxes_, reader);
}

void MVCConfigurationBox::dumpSpecifics(Dumper& dumper) const
{
    DUMP(configurationVersion_);
    DUMP(AVCProfileIndication_);
    DUMP(profile_compatibility_);
    DUMP(AVCLevelIndication_);
    DUMP(complete_representation_);
    DUMP(explicit_au_track_);
    DUMP(lengthSizeMinusOne_);
    DUMP(numOfSequenceParameterSets_);
    DUMP(numOfPictureParameterSets_);

    dumpBoxes(childBoxes_, dumper);
}
