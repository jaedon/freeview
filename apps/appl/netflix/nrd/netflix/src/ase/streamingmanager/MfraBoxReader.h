/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef MFRA_READER_H
#define MFRA_READER_H

#include "BoxReader.h"

namespace netflix {
namespace ase {

class TfraBoxReader;

class MfraBoxReader : public BoxReader
{
public:
    MfraBoxReader( BoxReader const& other );

    bool readTfra( TfraBoxReader& tfra );
};

/*  aligned(8) class TrackFragmentRandomAccessBox extends FullBox('tfra', version, 0) {
        unsigned int(32) track_ID;
        const unsigned int(26) reserved = 0;
        unsigned int(2)     length_size_of_traf_num;
        unsigned int(2)     length_size_of_trun_num;
        unsigned int(2)     length_size_of_sample_num;
        unsigned int(32)    number_of_entry
        for(i=1; i <= number_of_entry; i++) {
            if(version==1) {
                unsigned int(64) time;
                unsigned int(64) moof_offset;
            } else {
                unsigned int(32) time;
                unsigned int(32) moof_offset;
            }

            unsigned int((length_size_of_traf_num+1)*8)      traf_number;
            unsigned int((length_size_of_trun_num+1)*8)      trun_number;
            unsigned int((length_size_of_sample_num+1) * 8)  sample_number;
        }
    }
*/

class TfraBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TfraBoxReader( BoxReader const& other );

    TfraBoxReader();

    bool readHeader( uint32_t& trackId  );

    bool goodForEntry() const;

    bool moreEntries() const;

    bool readEntry( ullong& time,
                    ullong& offset,
                    uint32_t& traf_number,
                    uint32_t& trun_number,
                    uint32_t& sample_number );
protected:
    uint32_t    mRemainingEntries;
    uint32_t    mSizeOfTrafNum;
    uint32_t    mSizeOfTrunNum;
    uint32_t    mSizeOfSampleNum;
};

inline TfraBoxReader::TfraBoxReader()
    : BoxReader()
    , mRemainingEntries( 0 )
{
}

inline bool MfraBoxReader::readTfra( TfraBoxReader& tfra )
{
    return readBox( tfra );
}


}} // namespace netflix::ase

#endif
