/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EDIT_LIST_H
#define EDIT_LIST_H

#include <vector>
#include <nrdase/AseTimeStamp.h>

namespace netflix {
namespace ase {

class EditList
{
public:
    EditList();

    void setTimescale( ullong timescale );

    ullong getTimescale();

    bool empty();

    void add( ullong duration, llong media, bool dwell );

    /** Map Composition Time to Presentation Time
     *  @param cts [in] Composition Time Stamp
     *  @param pts [out] Presentation Time Stamp
     *  @return true if the provided CTS is to be presented according to the edit list
     */
    bool map( AseTimeStamp cts, AseTimeStamp& pts );

    void clear();

protected:
    struct Entry
    {
        ullong          mDuration;
        llong           mMediaTime;
        bool            mDwell;

        Entry( ullong duration, llong media, bool dwell )
            : mDuration( duration )
            , mMediaTime( media )
            , mDwell( dwell )
        {}
    };

    ullong              mTimescale;
    std::vector<Entry>  mEntries;
};

inline void EditList::clear()
{
    mEntries.clear();
}

inline ullong EditList::getTimescale()
{
    return mTimescale;
}

}}

#endif
