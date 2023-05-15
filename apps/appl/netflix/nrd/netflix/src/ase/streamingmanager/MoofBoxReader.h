/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MOOF_BOX_READER_H
#define MOOF_BOX_READER_H

#include "BoxReader.h"

namespace netflix {
namespace ase {

class MoofBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    MoofBoxReader( BoxReader const& other )
        : BoxReader( other )
    {}
};

class TrafBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TrafBoxReader( BoxReader const& other )
        : BoxReader( other )
    {}

};

/* aligned(8) class TrackFragmentHeaderBox extends FullBox('tfhd', 0, tf_flags){
 *  unsigned int(32) track_ID;
 *  // all the following are optional fields
 *  unsigned int(64) base_data_offset;
 *  unsigned int(32) sample_description_index;
 *  unsigned int(32) default_sample_duration;
 *  unsigned int(32) default_sample_size;
 *  unsigned int(32) default_sample_flags;
 * }
 */

class TfhdBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TfhdBoxReader( BoxReader const& other )
        : BoxReader( other )
    {}

    bool readHeader(    uint32_t&   track_ID,
                        bool&       base_data_offset_present,
                        ullong&     base_data_offset,
                        bool&       sample_description_index_present,
                        uint32_t&   sample_description_index,
                        bool&       default_sample_duation_present,
                        uint32_t&   default_sample_duation,
                        bool&       default_sample_size_present,
                        uint32_t&   default_sample_size,
                        bool&       default_sample_flags_present,
                        uint32_t&   default_sample_flags,
                        bool&       duration_is_empty,
                        bool&       default_base_is_moof );
};

/* aligned(8) class TrackRunBox extends FullBox('trun', version, tr_flags) {
 *  unsigned int(32) sample_count;
 *  // the following are optional fields
 *  signed int(32) data_offset;
 *  unsigned int(32) first_sample_flags;
 *  // all fields in the following array are optional
 *  {
 *     unsigned int(32)  sample_duration;
 *     unsigned int(32)  sample_size;
 *     unsigned int(32)  sample_flags
 *     if (version == 0)
 *        { unsigned int(32)
 *     else
 *        { signed int(32)
 *  }[ sample_count ]
 *}
 */

class TrunBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TrunBoxReader( BoxReader const& other )
        : BoxReader( other )
        , mSampleCount( 0 )
    {}

    bool readHeader(    uint32_t& sample_count,
                        bool& data_offset_present,
                        uint32_t& data_offset,
                        bool& first_sample_flags_present,
                        uint32_t& first_sample_flags,
                        bool& sample_duration_present,
                        bool& sample_size_present,
                        bool& sample_flags_present,
                        bool& sample_composition_time_offsets_present );

    bool moreEntries() const;

    bool readEntry( uint32_t& sample_duration,
                    uint32_t& sample_size,
                    uint32_t& sample_flags,
                    int32_t& sample_composition_time_offset );
protected:

    bool mSampleDurationPresent;
    bool mSampleSizePresent;
    bool mSampleFlagsPresent;
    bool mSampleCompositionTimeOffsetsPresent;

    uint32_t    mSampleCount;
};

/* aligned(8) class TrackFragmentBaseMediaDecodeTimeBox extends FullBox('tfdt', version, 0) {
 *  if (version==1) {
 *      unsigned int(64) baseMediaDecodeTime;
 *  } else { // version==0
 *      unsigned int(32) baseMediaDecodeTime;
 *  }
 *}
*/

class TfdtBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TfdtBoxReader( BoxReader const& other )
        : BoxReader( other )
    {}

    bool readHeader( llong& baseMediaDecodeTime );
};

}}

#endif


