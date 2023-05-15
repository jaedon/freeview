/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __NETFLIX_CONTAINERLIB_MP4PARSER_ISOBoxes__
#define __NETFLIX_CONTAINERLIB_MP4PARSER_ISOBoxes__

#include "Base.h"

#include <string>

namespace netflix {
namespace containerlib {
namespace mp4parser {

/*******************************************************************************
 * MediaDataBox
 */

class MediaDataBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

/*******************************************************************************
 * FileTypeBox
 */

class FileTypeBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t major_brand_;
    uint32_t minor_version_;
    std::list<uint32_t> compatible_brands_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieBox
 */

class MovieBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieHeaderBox
 */

class MovieHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    ullong creation_time_;
    ullong modification_time_;
    uint32_t timescale_;
    ullong duration_;
    int32_t rate_;
    int16_t volume_;
    MATRIX matrix_;
    uint32_t next_track_ID_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackBox
 */

class TrackBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackHeaderBox
 */

class TrackHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    ullong creation_time_;
    ullong modification_time_;
    uint32_t track_ID_;
    ullong duration_;
    int16_t layer_;
    int16_t alternate_group_;
    int16_t volume_;
    MATRIX matrix_;
    uint32_t width_;
    uint32_t height_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MediaBox
 */

class MediaBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MediaHeaderBox
 */

class MediaHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    ullong creation_time_;
    ullong modification_time_;
    uint32_t timescale_;
    ullong duration_;
    std::string language_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * HandlerBox
 */

class HandlerBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

    static const uint32_t VideoTrackType_;
    static const uint32_t AudioTrackType_;
    static const uint32_t HintTrackType_;
    static const uint32_t TimedMetadataTrackType_;

private:
    uint32_t handler_type_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MediaInformationBox
 */

class MediaInformationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * VideoMediaHeaderBox
 */

class VideoMediaHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint16_t graphicsmode_;
    uint16_t opcolor_red_;
    uint16_t opcolor_green_;
    uint16_t opcolor_blue_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SoundMediaHeaderBox
 */

class SoundMediaHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    int16_t balance_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DataInformationBox
 */

class DataInformationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DataReferenceBox
 */

class DataReferenceBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * DataEntryUrlBox
 */

class DataEntryUrlBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

/*******************************************************************************
 * DataEntryUrnBox
 */

class DataEntryUrnBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

/*******************************************************************************
 * SampleTableBox
 */

class SampleTableBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleDescriptionBox
 */

class SampleDescriptionBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * VisualSampleEntry
 */

class VisualSampleEntry : public SampleEntry
{
private:
    uint16_t width_;
    uint16_t height_;
    uint32_t horizresolution_;
    uint32_t vertresolution_;
    uint16_t frame_count_;
    uint16_t depth_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * PixelAspectRatioBox
 */

class PixelAspectRatioBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t hSpacing_;
    uint32_t vSpacing_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * ColourInformationBox
 */
class ColourInformationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t colourType_;
    uint32_t colourPrimaries_;
    uint32_t transferCharacteristics_;
    uint32_t matrixCoefficients_;
    uint16_t fullRangeFlag_;
    uint16_t reserved_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;

};


/*******************************************************************************
 * AudioSampleEntry
 */

class AudioSampleEntry : public SampleEntry
{
private:
    uint16_t channelcount_;
    uint16_t samplesize_;
    uint32_t samplerate_;
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * ProtectionSchemeInfoBox
 */

class ProtectionSchemeInfoBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * OriginalFormatBox
 */

class OriginalFormatBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t data_format_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SchemeTypeBox
 */

class SchemeTypeBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t scheme_type_;
    uint32_t scheme_version_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SchemeInformationBox
 */

class SchemeInformationBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TimeToSampleBox
 */

class TimeToSampleBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * CompositionOffsetBox
 */

class CompositionOffsetBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleSizeBox
 */

class SampleSizeBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t sample_size_;
    uint32_t sample_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleToChunkBox
 */

class SampleToChunkBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * ChunkOffsetBox
 */

class ChunkOffsetBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t entry_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleGroupDescriptionBox
 */

class SampleGroupDescriptionBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t grouping_type_;
    uint32_t default_length_;
    uint32_t entry_count_;
    uint32_t description_length_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieExtendsBox
 */

class MovieExtendsBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieExtendsHeaderBox
 */

class MovieExtendsHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    ullong fragment_duration_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackExtendsBox
 */

class TrackExtendsBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t track_ID_;
    uint32_t default_sample_description_index_;
    uint32_t default_sample_duration_;
    uint32_t default_sample_size_;
    uint32_t default_sample_flags_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieFragmentBox
 */

class MovieFragmentBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieFragmentHeaderBox
 */

class MovieFragmentHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t sequence_number_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackFragmentBox
 */

class TrackFragmentBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackFragmentHeaderBox
 */

class TrackFragmentHeaderBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    static const uint32_t base_data_offset_present_bit_ = 0;
    static const uint32_t sample_description_index_present_bit_ = 1;
    static const uint32_t default_sample_duration_present_bit_ = 3;
    static const uint32_t default_sample_size_present_bit_ = 4;
    static const uint32_t default_sample_flags_present_bit_ = 5;
    static const uint32_t duration_is_empty_bit_ = 16;

private:
    uint32_t track_ID_;
    ullong base_data_offset_;
    uint32_t sample_description_index_;
    uint32_t default_sample_duration_;
    uint32_t default_sample_size_;
    uint32_t default_sample_flags_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackRunBox
 */

class TrackRunBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    static const uint32_t data_offset_present_bit_ = 0;
    static const uint32_t first_sample_flags_present_bit_ = 2;
    static const uint32_t sample_duration_present_bit_ = 8;
    static const uint32_t sample_size_present_bit_ = 9;
    static const uint32_t sample_flags_present_bit_ = 10;
    static const uint32_t sample_composition_time_offsets_present_bit_ = 11;

private:
    uint32_t sample_count_;
    int32_t data_offset_;
    uint32_t first_sample_flags_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * IndependentAndDisposableSamplesBox
 */

class DisposableSamplesBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t sample_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};


/*******************************************************************************
 * MovieFragmentRandomAccessBox
 */

class MovieFragmentRandomAccessBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    std::list< shared_ptr<Box> > childBoxes_;

protected:
    virtual void clearSpecifics();
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackFragmentRandomAccessBox
 */

class TrackFragmentRandomAccessBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t track_ID_;
    uint16_t length_size_of_traf_num_;
    uint16_t length_size_of_trun_num_;
    uint16_t length_size_of_sample_num_;
    uint32_t number_of_entry_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * SampleToGroupBox
 */

class SampleToGroupBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t grouping_type_;
    uint32_t entry_count_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * MovieFragmentRandomAccessOffsetBox
 */

class MovieFragmentRandomAccessOffsetBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint32_t size_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;
};

/*******************************************************************************
 * TrackFragmentDecodeTimeBox
 */

class TrackFragmentDecodeTimeBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;

private:
    uint64_t baseMediaDecodeTime_;

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& dumper) const;

};

/*******************************************************************************
 * EditBox
 */
class EditBox : public Box
{
public:
    static const uint32_t ExpectedBoxType_;
private:
    std::list< shared_ptr<Box> > childBoxes_;
protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}
};

/*******************************************************************************
 * EditListBox
 */
class EditListBox : public FullBox
{
public:
    static const uint32_t ExpectedBoxType_;
private:

protected:
    virtual void clearSpecifics() {}
    virtual bool readSpecifics(Reader& reader, Context& context);
    virtual void dumpSpecifics(Dumper& /*dumper*/) const {}

};


}}} // namespace netflix::containerlib::mp4parser

#endif // __NETFLIX_CONTAINERLIB_MP4PARSER_ISOBoxes__
