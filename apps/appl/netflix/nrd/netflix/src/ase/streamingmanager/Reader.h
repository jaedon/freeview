/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef READER_H
#define READER_H

#include <limits>
#include <list>
#include <stddef.h>
#include <nrdase/AseCommonDataTypes.h>

namespace netflix {
namespace ase {

/** Very simple memory reader for reading data from
 *  ISO Base Media Files */

typedef uint32_t fourcc_t;
typedef std::pair<ullong,ullong> nf_uuid_t;

nf_uuid_t make_uuid( uint32_t, uint16_t, uint16_t, uint16_t, ullong );

class Reader
{
public:

    class BlockList
    {
    public:

        class const_iterator;

        typedef std::list<std::pair<uint8_t const*,uint8_t const*> > container_type;

        BlockList();

        BlockList( uint8_t const* pData, size_t size );

        void push_back( uint8_t const* pData, size_t size );

        bool pop_front();

        bool pop_front( size_t n );

        void extend( uint8_t const* pData, size_t size );

        void clear();

        bool empty() const;

        const_iterator begin() const;

        const_iterator end() const;

        //bool contiguous( const_iterator const& start, const_iterator const& end ) const;

        class const_iterator
        {
        public:
            friend class BlockList;

            const_iterator( container_type const& blocks, container_type::const_iterator block, uint8_t const* ptr );

            //const_iterator( const_iterator const& other );

            //const_iterator& operator=( const_iterator const& other );

            uint8_t operator*() const;

            const_iterator& operator++();

            const_iterator operator++( int );

            const_iterator& operator+=( size_t rhs );

            const_iterator& operator-=( size_t rhs );

            const_iterator operator+( size_t rhs ) const;

            const_iterator operator-( size_t rhs ) const;

            ptrdiff_t operator-( const_iterator const& rhs ) const;

            bool operator>( const_iterator const& rhs ) const;

            bool operator>=( const_iterator const& rhs ) const;

            bool operator<( const_iterator const& rhs ) const;

            bool operator<=( const_iterator const& rhs ) const;

            bool operator==( const_iterator const& rhs ) const;

            bool operator!=( const_iterator const& rhs ) const;

            uint8_t const* get() const { return mPtr; }

            bool reset();

        public:
            bool valid() const;

        protected:

            container_type const*            mBlocks;

            container_type::const_iterator   mBlock;

            uint8_t const*                  mPtr;

        };

    protected:
        container_type mBlocks;

    };

    class Mark
    {
        friend class Reader;

        Mark( BlockList::const_iterator position ) : mPosition( position ) {}

        BlockList::const_iterator mPosition;
    };

public:
    Reader( uint8_t const* pData, size_t size );

    Reader( const Reader& other );

    Reader( Reader const& other, BlockList::const_iterator pEnd );

    Reader();

    void reset();

    void push_back( uint8_t const* pData, size_t size );

    bool pop_front();

    bool pop_front( size_t n );

    Reader& operator=( Reader const& other );

    bool good() const;

    bool goodfor( size_t n ) const;

    bool eos() const;

    bool skip( size_t n );

    /** Read data into a basic type (u)int(8|16|32|64)_t */
    template<typename T>
    bool read( T& data );

    /** Read data into a basic type, reading only the specified number of bytes */
    template<typename T>
    bool read( T& data, unsigned int bytes );

    /** Read and unpack a uint32 */
    //template<int a, int b, int c = 0, int d = 0>
    //bool unpack32( uint32_t* pA, uint32_t* pB, uint32_t* pC = 0, uint32_t* pD = 0 );

    // Default template arguments for function templates are only supported
    // in C++0x (gcc4.4), so we have to do the following ...
    template<int a, int b, int c, int d>
    bool unpack32( uint32_t* pA, uint32_t* pB, uint32_t* pC, uint32_t* pD );

    template<int a, int b, int c>
    bool unpack32( uint32_t* pA, uint32_t* pB, uint32_t* pC );

    template<int a, int b>
    bool unpack32( uint32_t* pA, uint32_t* pB );

    /** Extend the reader data - provided data must overlap with an extend current position */
    bool extend( uint8_t const* pData, size_t size );

    Mark mark() const;

    void move( Mark mark );

protected:
    /** Template struct for getting the type of integers of various size
        uint<number>::type is defined within specializations of this struct
     */
    template<size_t size> struct uint { /* typedef <uint type> type; */ };

    /** return a mask for the low order n bits */
    static uint32_t mask( int n );

protected:
    template<typename uint_t>
    void read_uint( uint_t& data );

    template<typename uint_t>
    void read_uint( uint_t& data, unsigned int bytes );

protected:
    BlockList                   mData;
    BlockList::const_iterator   mpCurrent;
    BlockList::const_iterator   mpEnd;
    bool                        mGood;
};


inline Reader::BlockList::BlockList()
{
}

inline Reader::BlockList::BlockList( uint8_t const* pData, size_t size )
{
    push_back( pData, size );
}

inline void Reader::BlockList::push_back( uint8_t const* pData, size_t size )
{
    mBlocks.push_back( std::make_pair( pData, pData + size ) );
}

inline bool Reader::BlockList::pop_front( )
{
    if ( mBlocks.empty() )
        return false;

    mBlocks.erase( mBlocks.begin() );

    return true;
}

inline bool Reader::BlockList::pop_front( size_t n )
{
    while( !mBlocks.empty() && (size_t) ( mBlocks.front().second - mBlocks.front().first ) <= n )
    {
        n -= mBlocks.front().second - mBlocks.front().first;

        mBlocks.erase( mBlocks.begin() );
    }

    if ( mBlocks.empty() )
        return ( n == 0 );

    mBlocks.front().first += n;

    return true;
}

inline void Reader::BlockList::extend( uint8_t const* pData, size_t size )
{
    if ( mBlocks.empty() || ( pData + size ) < mBlocks.back().first || pData >= mBlocks.back().second )
    {
        push_back( pData, size );
    }
    else
    {
        assert( pData >= mBlocks.back().first );
        assert( pData + size >= mBlocks.back().second );
        mBlocks.back().second = pData + size;
    }
}

inline void Reader::BlockList::clear()
{
    mBlocks.clear();
}

inline bool Reader::BlockList::empty() const
{
    return mBlocks.empty();
}

/*inline bool Reader::BlockList::contiguous( const_iterator const& start, const_iterator const& end ) const
{
    return ( start.mBlock == end.mBlock ) || ( end.mBlock == mBlocks.end() && start.mBlock == (mBlocks.end() - 1) ) ;
}*/

inline Reader::BlockList::const_iterator Reader::BlockList::begin() const
{
    return const_iterator( mBlocks, mBlocks.begin(), mBlocks.empty() ? 0 : mBlocks.front().first );
}

inline Reader::BlockList::const_iterator Reader::BlockList::end() const
{
    container_type::const_iterator last = mBlocks.end();
    if ( mBlocks.empty() )
        return const_iterator( mBlocks, last, 0 );
    --last;
    return const_iterator( mBlocks, last, mBlocks.back().second );
}

inline bool Reader::BlockList::const_iterator::valid() const
{
    return ( mBlocks->empty()
                || ( ( mPtr >= mBlock->first && mPtr < mBlock->second )
                            || ( mPtr == mBlock->second && mPtr == mBlocks->back().second ) ) );
}


inline Reader::BlockList::const_iterator::const_iterator( container_type const& blocks, container_type::const_iterator block, uint8_t const* ptr )
    : mBlocks( &blocks )
    , mBlock( block )
    , mPtr( ptr )
{
    assert( valid() );
}

inline uint8_t Reader::BlockList::const_iterator::operator*() const
{
    return *mPtr;
}

inline Reader::BlockList::const_iterator& Reader::BlockList::const_iterator::operator++()
{
    assert( valid() );
    if ( mBlocks->empty() ) return *this;
    assert( mPtr < mBlock->second );
    ++mPtr;
    if( mPtr == mBlock->second )
    {
        ++mBlock;
        if ( mBlock == mBlocks->end() ) { --mBlock; }
        else mPtr = mBlock->first;
    }

    return *this;
}

inline Reader::BlockList::const_iterator Reader::BlockList::const_iterator::operator++( int )
{
    const_iterator result = *this;
    ++(*this);
    return result;
}

inline Reader::BlockList::const_iterator& Reader::BlockList::const_iterator::operator+=( size_t rhs )
{
    assert( valid() );
    if ( mBlocks->empty() ) return *this;
    mPtr += rhs;
    while( mPtr >= mBlock->second )
    {
        ptrdiff_t const offset = mPtr - mBlock->second;

        ++mBlock;

        if ( mBlock == mBlocks->end() ) { --mBlock; mPtr = mBlock->second; break; }

        mPtr = mBlock->first + offset;
    }

    return *this;
}

inline Reader::BlockList::const_iterator& Reader::BlockList::const_iterator::operator-=( size_t rhs )
{
    assert( valid() );
    if ( mBlocks->empty() ) return *this;

    mPtr -= rhs;
    while( mPtr < mBlock->first )
    {
        if ( mBlock == mBlocks->begin() ) { mPtr = mBlock->first; break; }

        ptrdiff_t const offset = mBlock->first - mPtr;

        --mBlock;

        mPtr = mBlock->second - offset;
    }

    return *this;
}

inline Reader::BlockList::const_iterator Reader::BlockList::const_iterator::operator+( size_t rhs ) const
{
    const_iterator result( *this );
    result += rhs;
    return result;
}

inline Reader::BlockList::const_iterator Reader::BlockList::const_iterator::operator-( size_t rhs ) const
{
    const_iterator result( *this );
    result -= rhs;
    return result;
}

inline ptrdiff_t Reader::BlockList::const_iterator::operator-( const_iterator const& rhs ) const
{
    assert( valid() );
    assert( rhs.valid() );
    assert( mBlocks == rhs.mBlocks );
    assert( *this >= rhs );

    if ( rhs.mBlock == mBlock ) return mPtr - rhs.mPtr;
    if ( *this < rhs ) return - ( rhs - *this );
    ptrdiff_t result = rhs.mBlock->second - rhs.mPtr;
    container_type::const_iterator it = rhs.mBlock;
    ++it;
    for( ; it != mBlock; ++it)
    {
        assert( it != mBlocks->end() );
        result += ( it->second - it->first );
    }

    result += mPtr - it->first;

    return result;
}

inline bool Reader::BlockList::const_iterator::operator>( const_iterator const& rhs ) const
{
    assert( valid() );
    assert( rhs.valid() );
    assert( mBlocks == rhs.mBlocks );
    if ( rhs.mBlock == mBlock ) return ( mPtr > rhs.mPtr );
    if ( mBlock == mBlocks->begin() ) return false;
    if ( rhs.mBlock == mBlocks->begin() ) return true;
    container_type::const_iterator it = mBlock;
    for( ; it != rhs.mBlock && it != mBlocks->end(); ++it);
    return ( it == mBlocks->end() );
}

inline bool Reader::BlockList::const_iterator::operator>=( const_iterator const& rhs ) const
{
    return ( mPtr == rhs.mPtr ) || (*this > rhs );
}

inline bool Reader::BlockList::const_iterator::operator<( const_iterator const& rhs ) const
{
    return ( rhs > *this );
}

inline bool Reader::BlockList::const_iterator::operator<=( const_iterator const& rhs ) const
{
    return ( mPtr == rhs.mPtr ) || (*this < rhs );
}

inline bool Reader::BlockList::const_iterator::operator==( const_iterator const& rhs ) const
{
    assert( valid() );
    assert( rhs.valid() );
    assert( mBlocks == rhs.mBlocks );
    return ( mPtr == rhs.mPtr );
}

inline bool Reader::BlockList::const_iterator::operator!=( const_iterator const& rhs ) const
{
    assert( valid() );
    assert( rhs.valid() );
    assert( mBlocks == rhs.mBlocks );
    return ( mPtr != rhs.mPtr );
}

inline bool Reader::BlockList::const_iterator::reset()
{
    if ( mBlocks->empty() )
    {
        mBlock = mBlocks->end();
        return true;
    }

    container_type::const_iterator it = mBlocks->begin();
    for( ; it != mBlocks->end(); ++it )
    {
        if ( mPtr >= it->first && mPtr < it->second )
        {
            mBlock = it;
            return true;
        }
    }

    if ( mPtr == mBlocks->back().second )
    {
        mBlock = mBlocks->end(); --mBlock;
        return true;
    }

    return false;
}

inline Reader::Reader( uint8_t const* pData, size_t size )
    : mData( pData, size )
    , mpCurrent( mData.begin() )
    , mpEnd( mData.end() )
    , mGood( size > 0 )
{
}

inline Reader::Reader( Reader const& other )
    : mData( other.mData )
    , mpCurrent( mData.begin() + ( other.mpCurrent - other.mData.begin() ) )
    , mpEnd( mData.begin() + ( other.mpEnd - other.mData.begin() ) )
    , mGood( other.mGood )
{
}

inline Reader::Reader( Reader const& other, BlockList::const_iterator pEnd )
    : mData( other.mData )
    , mpCurrent( mData.begin() + ( other.mpCurrent - other.mData.begin() ) )
    , mpEnd( mData.begin() + ( pEnd - other.mData.begin() ) )
    , mGood( other.mGood )
{
    ptrdiff_t diff = ( pEnd - other.mData.begin() );

    mpEnd = mData.begin() + diff;

    assert( mpEnd.valid() );
}

inline Reader::Reader()
    : mData()
    , mpCurrent( mData.end() )
    , mpEnd( mData.end() )
    , mGood( false )
{
}


inline void Reader::reset()
{
    mData.clear();

    mpCurrent = mData.end();
    mpEnd = mData.end();

    mGood = false;
}

inline void Reader::push_back( uint8_t const* pData, size_t size )
{
    bool const empty = ( mData.empty() );
    bool const end = ( mpEnd == mData.end() );

    mData.push_back( pData, size );

    if ( empty )
    {
        mGood = true;
        mpCurrent = mData.begin();
        mpEnd = mData.end();
    }
    else if ( end )
    {
        mpEnd = mData.end();
    }
}

inline bool Reader::pop_front( )
{
    return mData.pop_front() && mpCurrent.reset() && mpEnd.reset();
}

inline bool Reader::pop_front( size_t n )
{
    return mData.pop_front( n ) && mpCurrent.reset() && mpEnd.reset();
}

inline Reader& Reader::operator=( Reader const& other )
{
    mData = other.mData;
    mpCurrent = mData.begin() + ( other.mpCurrent - other.mData.begin() );
    mpEnd = mData.begin() + ( other.mpEnd - other.mData.begin() );
    mGood = other.mGood;
    return *this;
}

inline bool Reader::good() const
{
    return mGood;
}

inline bool Reader::eos() const
{
    return mpCurrent == mpEnd;
}

inline bool Reader::goodfor( size_t n ) const
{
    return ( (size_t) ( mpEnd - mpCurrent ) >= n );
}

inline bool Reader::skip( size_t n )
{
    if ( !goodfor( n ) )
        return false;

    mpCurrent += n;

    return true;
}

// Specializations of the uint struct
template<> struct Reader::uint<1> { typedef uint8_t type; };
template<> struct Reader::uint<2> { typedef uint16_t type; };
template<> struct Reader::uint<4> { typedef uint32_t type; };
template<> struct Reader::uint<8> { typedef ullong type; };
template<> struct Reader::uint<16> { typedef nf_uuid_t type; };

template<typename T>
inline bool Reader::read( T& data )
{
    typedef typename uint<sizeof( T )>::type uint_t;
    if ( !goodfor( sizeof( data ) ) ) return ( mGood = false );
    read_uint<uint_t>( reinterpret_cast<uint_t&>( data ) );
    return mGood;
}

template<typename T>
inline bool Reader::read( T& data, unsigned int bytes )
{
    assert( bytes <= sizeof( T ) );
    typedef typename uint<sizeof( T )>::type uint_t;
    if ( !goodfor( bytes ) ) return ( mGood = false );
    read_uint<uint_t>( reinterpret_cast<uint_t&>( data ), bytes );
    if ( std::numeric_limits<T>::is_signed && bytes < sizeof( T ) )
    {
        if ( data & ( 0x80 << ((bytes-1)*8) ) )
        {
            data |= ((T)-1)^(((T)1<<bytes)-1);
        }
    }
    return true;
}

template<int a, int b, int c, int d>
inline bool Reader::unpack32( uint32_t* pA, uint32_t* pB, uint32_t* pC, uint32_t* pD )
{
    uint32_t data = 0;
    if ( !read( data ) ) return false;

    *pA = data >> ( b + c + d );
    *pB = ( data & mask( b + c + d ) ) >> ( c + d );
    if ( c && pC ) *pC = ( data & mask( c + d ) ) >> d;
    if ( d && pD ) *pD = ( data & mask( d ) );

    return true;
}

template<int a, int b, int c>
inline bool Reader::unpack32( uint32_t* pA, uint32_t* pB, uint32_t* pC )
{
    return unpack32<a,b,c,0>( pA, pB, pC, 0 );
}

template<int a, int b>
inline bool Reader::unpack32( uint32_t* pA, uint32_t* pB )
{
    return unpack32<a,b,0,0>( pA, pB, 0, 0 );
}

template<>
inline void Reader::read_uint<uint8_t>( uint8_t& data )
{
    data = *mpCurrent++;
}

template<>
inline void Reader::read_uint<uint16_t>( uint16_t& data )
{
    //data = ntohs( *static_cast<uint16_t*>( mpCurrent ) ); mpCurrent += 2;
    data = ( static_cast<uint16_t>( *mpCurrent ) << 8 )
                + ( static_cast<uint16_t>( *(mpCurrent + 1) ) );
    mpCurrent += 2;
}

template<>
inline void Reader::read_uint<uint32_t>( uint32_t& data )
{
    //data = ntohl( *static_cast<uint32_t*>( mpCurrent ) ); mpCurrent += 4;
    data =  ( static_cast<uint32_t>( *mpCurrent ) << 24 )
            + ( static_cast<uint32_t>( *(mpCurrent + 1) ) << 16 )
            + ( static_cast<uint32_t>( *(mpCurrent + 2) ) << 8 )
            + ( static_cast<uint32_t>( *(mpCurrent + 3) ) );
    mpCurrent += 4;
}

template<>
inline void Reader::read_uint<ullong>( ullong& data )
{
    //data = ntohll( *static_cast<ullong*>( mpCurrent ) ); mpCurrent += 8;
    data =  ( static_cast<ullong>( *mpCurrent ) << 56 )
            + ( static_cast<ullong>( *(mpCurrent+1) ) << 48 )
            + ( static_cast<ullong>( *(mpCurrent+2) ) << 40 )
            + ( static_cast<ullong>( *(mpCurrent+3) ) << 32 )
            + ( static_cast<ullong>( *(mpCurrent+4) ) << 24 )
            + ( static_cast<ullong>( *(mpCurrent+5) ) << 16 )
            + ( static_cast<ullong>( *(mpCurrent+6) ) << 8 )
            + ( static_cast<ullong>( *(mpCurrent+7) ) );
    mpCurrent += 8;
}

template<>
inline void Reader::read_uint<nf_uuid_t>( nf_uuid_t& data )
{
    read_uint( data.first );
    read_uint( data.second );
}

template<typename T>
inline void Reader::read_uint( T& data, unsigned int bytes )
{
    data = 0;
    while( bytes-- ) data |= (T)(*(mpCurrent++)) << (8*bytes);
}

inline uint32_t Reader::mask( int n )
{
    return (uint32_t) (((ullong)1)<<(n)) - 1;
}

inline nf_uuid_t make_uuid( uint32_t a, uint16_t b, uint16_t c, uint16_t d, ullong e )
{
    ullong first, second;
    first = (((ullong)a) << 32) | (((ullong)b) << 16 ) | c;
    second = (((ullong)d) << 48) | (e&0xffffffffffffULL);
    return std::make_pair( first, second );
}

inline bool Reader::extend( uint8_t const* pData, size_t size )
{
    mData.extend( pData, size );

    mpEnd = mData.end();

    return true;
}

inline Reader::Mark Reader::mark() const
{
    assert( mGood );
    return Mark( mpCurrent );
}

inline void Reader::move( Mark mark )
{
    mpCurrent = mark.mPosition;
    mGood = true;
}

}}

#endif /* READER_H */
