/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef MOOV_BOX_READER_H
#define MOOV_BOX_READER_H

#include <vector>
#include "BoxReader.h"

namespace netflix {
namespace ase {

class MvhdBoxReader;
class MvexBoxReader;
class MehdBoxReader;

/*  aligned(8) class MovieBox extends Box('moov'){ }
*/

class MoovBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    MoovBoxReader();

    MoovBoxReader( BoxReader const& other );

    bool readMvhdBox( MvhdBoxReader& mvhdreader );

    bool readMvexBox( MvexBoxReader& mvexreader );
};

/* aligned(8) class MovieHeaderBox extends FullBox('mvhd', version, 0)
{
    if (version==1) {
      unsigned int(64)  creation_time;
      unsigned int(64)  modification_time;
      unsigned int(32)  timescale;
      unsigned int(64)  duration;
    } else {                                // version==0
      unsigned int(32)  creation_time;
      unsigned int(32)  modification_time;
      unsigned int(32)  timescale;
      unsigned int(32)  duration;
    }
    template int(32) rate = 0x00010000; // typically 1.0
    template int(16) volume = 0x0100; // typically, full volume const bit(16) reserved = 0;
    const unsigned int(32)[2] reserved = 0;
    template int(32)[9] matrix = { 0x00010000,0,0,0,0x00010000,0,0,0,0x40000000 };

    // Unity matrix
    bit(32)[6]  pre_defined = 0;
    unsigned int(32)  next_track_ID;
}
*/

class MvhdBoxReader : public BoxReader
{
public:
    typedef uint32_t MatrixType[9];

    static fourcc_t const type;

    MvhdBoxReader();

    MvhdBoxReader( BoxReader const& other );

    bool readParams(    ullong& creation_time,
                        ullong& modification_time,
                        uint32_t& timescale,
                        ullong& duration,
                        uint32_t& rate,
                        uint16_t& volume );

    bool readMatrix( MatrixType& matrix );

    bool readNextTrack( uint32_t& next_track );
};

class TrakBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    TrakBoxReader( BoxReader const& other ) : BoxReader( other ) {}
};

class MdiaBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    MdiaBoxReader( BoxReader const& other ) : BoxReader( other ) {}
};

/* aligned(8) class MediaHeaderBox extends FullBox('mdhd', version, 0) {
    if (version==1) {
        unsigned int(64)  creation_time;
        unsigned int(64)  modification_time;
        unsigned int(32)  timescale;
        unsigned int(64)  duration;
    } else { // version==0
        unsigned int(32)  creation_time;
        unsigned int(32)  modification_time;
        unsigned int(32)  timescale;
        unsigned int(32)  duration;
    }
    bit(1) pad = 0;
    unsigned int(5)[3] language; // ISO-639-2/T language code
    unsigned int(16) pre_defined = 0;
}
*/

class MdhdBoxReader : public BoxReader
{
public:
    typedef char language_t[3];

    static fourcc_t const type;

    MdhdBoxReader( BoxReader const& other ) : BoxReader( other ) {}

    bool readParams(    ullong& creation_time,
                        ullong& modification_time,
                        uint32_t& timescale,
                        ullong& duration,
                        language_t& language );
};

class EdtsBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    EdtsBoxReader();

    EdtsBoxReader( BoxReader const& other ) : BoxReader( other ) {}
};

/* aligned(8) class MovieExtendsBox extends Box('mvex'){ } */

class MvexBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    MvexBoxReader();

    MvexBoxReader( BoxReader const& other );

    bool readMehdBox( MehdBoxReader& mvhdreader );
};

/* aligned(8) class MovieExtendsHeaderBox extends FullBox('mehd', version, 0) { if (version==1) {
      unsigned int(64)  fragment_duration;
   } else { // version==0
      unsigned int(32)  fragment_duration;
   }
}
*/

class MehdBoxReader : public BoxReader
{
public:
    static fourcc_t const type;

    MehdBoxReader();

    MehdBoxReader( BoxReader const& other );

    bool readDuration( ullong& fragment_duration );
};

/* aligned(8) class EditListBox extends FullBox('elst', version, 0)
 * {
 *      unsigned int(32) entry_count;
 *      for (i=1; i <= entry_count; i++) {
 *          if (version==1) {
 *              unsigned int(64) segment_duration;
 *              int(64) media_time;
 *          } else { // version==0
 *              unsigned int(32) segment_duration;
 *              int(32)  media_time;
 *
 *          }
 *          int(16) media_rate_integer;
 *          int(16) media_rate_fraction = 0;
 *       }
 * }
 */
class ElstBoxReader : BoxReader
{
public:
    static fourcc_t const type;

    ElstBoxReader();

    ElstBoxReader( BoxReader const& other );

    bool readHeader();

    bool moreEntries() const;

    bool readEntry( ullong& segment_duration,
                    llong& media_time,
                    int16_t& media_rate_integer,
                    int16_t& media_rate_fraction );

protected:
    uint32_t mRemainingEntries;
};

inline MoovBoxReader::MoovBoxReader()
    : BoxReader()
{
}

inline MoovBoxReader::MoovBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}

inline bool MoovBoxReader::readMvhdBox( MvhdBoxReader& mvhdreader )
{
    return readBox( mvhdreader );
}

inline MvhdBoxReader::MvhdBoxReader()
    : BoxReader()
{
}

inline MvhdBoxReader::MvhdBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}

inline bool MvhdBoxReader::readNextTrack( uint32_t& next_track )
{
    return read( next_track );
}

inline MvexBoxReader::MvexBoxReader()
    : BoxReader()
{
}

inline MvexBoxReader::MvexBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}

inline bool MvexBoxReader::readMehdBox( MehdBoxReader& mehdreader )
{
    return readBox( mehdreader );
}

inline MehdBoxReader::MehdBoxReader()
    : BoxReader()
{
}

inline MehdBoxReader::MehdBoxReader( BoxReader const& other )
    : BoxReader( other )
{
}

inline ElstBoxReader::ElstBoxReader()
    : BoxReader()
    , mRemainingEntries( 0 )
{
}

inline ElstBoxReader::ElstBoxReader( BoxReader const& other )
    : BoxReader( other )
    , mRemainingEntries( 0 )
{
}

inline bool ElstBoxReader::moreEntries() const
{
    return mRemainingEntries > 0;
}


}} // namespace netflix::ase

#endif /* FTYP_BOX_READER_H */
