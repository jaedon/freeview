/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef BOX_READER_H
#define BOX_READER_H

#include "Reader.h"

#include <string.h>

namespace netflix {
namespace ase {

class BoxReader : public Reader
{
public:
    static const fourcc_t UUID_4CC;

    BoxReader( ullong offset, uint8_t const* pData, size_t size );

    BoxReader( BoxReader const& other );

    //BoxReader( ullong offset, Reader const& other );

    BoxReader();

    void reset();

    void push_back( uint8_t const* pData, size_t size );

    bool pop_front( );

    bool pop_front( size_t n );

    bool skip( size_t n );

    bool readBoxHeader();

    bool readFullBoxHeader();

    bool isType( uint32_t const type ) const;

    bool isType( nf_uuid_t const& uuid ) const;

    bool isExtendedType() const;

    ullong getOffset() const;

    fourcc_t getType() const;

    ullong getSize() const;

    nf_uuid_t const& getExtendedType( ) const;

    uint32_t getVersion() const;

    uint32_t getFlags() const;

    bool complete() const;

    ullong getMissing() const;

    template<typename box_t>
    bool readBox( box_t& box );

    bool nextBox();

    static uint32_t getFourcc( const char *str );

    void getboxdata( uint8_t const*& pStart, size_t& size ) const;

protected:
    ullong                      mOffset;
    uint32_t                    mType;
    nf_uuid_t                   mExtendedType;
    ullong                      mSize;
    ullong                      mMissing;
    uint32_t                    mVersion;
    uint32_t                    mFlags;
    BlockList::const_iterator   mpBoxStart;
};

inline BoxReader::BoxReader( ullong offset, uint8_t const* pData, size_t size )
    : Reader( pData, size )
    , mOffset( offset )
    , mType( 0 )
    , mSize( 0 )
    , mMissing( 0 )
    , mpBoxStart( mData.begin() )
{
}

inline BoxReader::BoxReader( BoxReader const& other )
    : Reader( other, ( other.mMissing ? other.mpEnd : other.mpBoxStart + other.mSize ) )
    , mOffset( other.mOffset )
    , mType( other.mType )
    , mSize( other.mSize )
    , mMissing( other.mMissing )
    , mVersion( other.mVersion )
    , mFlags( other.mFlags )
    , mpBoxStart( mData.begin() + ( other.mpBoxStart - other.mData.begin() ) )
{
    memcpy( &mExtendedType, &other.mExtendedType, 16 );
}

#if 0
inline BoxReader::BoxReader( ullong offset, Reader const& other )
    : Reader( other )
    , mOffset( offset )
    , mType( 0 )
    , mSize( 0 )
    , mMissing( 0 )
    , mpBoxStart( 0 )
{
}
#endif

inline BoxReader::BoxReader()
    : Reader()
    , mOffset( 0 )
    , mType( 0 )
    , mSize( 0 )
    , mMissing( 0 )
    , mVersion( 0 )
    , mFlags( 0 )
    , mpBoxStart( mData.end() )
{
}

inline void BoxReader::push_back( uint8_t const* pData, size_t size )
{
    Reader::push_back( pData, size );

    mMissing = mMissing > size ? ( mMissing - size ) : 0;
}

inline bool BoxReader::pop_front( )
{
    return Reader::pop_front() && mpBoxStart.reset();
}

inline bool BoxReader::pop_front( size_t n )
{
    return Reader::pop_front( n ) && mpBoxStart.reset();
}

inline bool BoxReader::skip( size_t n )
{
    if ( !Reader::skip( n ) )
        return false;

    mpBoxStart = mData.end();

    mOffset += n;
    mType = 0;
    mSize = 0;
    mMissing = 0;
    mVersion = 0;
    mFlags = 0;

    return true;
}


inline bool BoxReader::isExtendedType() const
{
    return mType == UUID_4CC;
}

inline fourcc_t BoxReader::getType() const
{
    return mType;
}

inline nf_uuid_t const& BoxReader::getExtendedType() const
{
    return mExtendedType;
}

inline ullong BoxReader::getOffset() const
{
    return mOffset;
}

inline ullong BoxReader::getSize() const
{
    return mSize;
}

inline ullong BoxReader::getMissing() const
{
    return mMissing;
}

inline bool BoxReader::complete() const
{
    return ( mMissing == 0 );
}

inline uint32_t BoxReader::getVersion() const
{
    return mVersion;
}

inline uint32_t BoxReader::getFlags() const
{
    return mFlags;
}

inline uint32_t BoxReader::getFourcc( const char *str )
{
    assert(strlen(str) == 4);

    return (str[0] << 24) + (str[1] << 16) + (str[2] <<  8) + str[3];
}

inline bool BoxReader::isType( uint32_t const type ) const
{
    return mType == type;
}

inline bool BoxReader::isType( nf_uuid_t const& uuid ) const
{
    return isExtendedType()
                && memcmp( &uuid, &mExtendedType, 16 ) == 0;
}

template<typename box_t>
bool BoxReader::readBox( box_t& box )
{
    if ( !readBoxHeader( ) )
        return false;

    if ( !isType( box_t::type ) )
        return false;

    box = box_t( *this );

    nextBox();

    return true;
}

inline void BoxReader::getboxdata( uint8_t const*& pStart, size_t& size ) const
{
    pStart = mpBoxStart.get();

    size = (size_t) ( mpEnd - mpBoxStart );
}

}}
#endif
