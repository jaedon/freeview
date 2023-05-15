/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <set>

#include "TraceAreas.h"

#include <nrdbase/Assert.h>
#include <nrdbase/LexicalCast.h>
#include <nrdbase/TraceAreas.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/Log.h>

using namespace netflix;

TraceAreas::TraceAreas()
    : mMutex(TRACEAREAS_MUTEX, "TraceAreas::Mutex")
{
}

std::string TraceAreas::spec() const
{
    ScopedMutex lock(mMutex);
    std::string spec;
    for (std::set<const TraceArea*>::const_iterator it = mAreas.begin(); it != mAreas.end(); ++it) {
        if (!spec.empty())
            spec += ",";
        spec += (*it)->getName();
    }
    return spec;
}
std::vector<std::string> TraceAreas::all()
{
    const std::vector<const TraceArea*> allAreas = TraceArea::getAllAreas();
    const int count = allAreas.size();
    std::vector<std::string> ret(count);
    for (int i=0; i<count; ++i) {
        ret[i] = allAreas.at(i)->getName();
    }
    return ret;
}

bool TraceAreas::set(std::string tok)
{
    const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas();

    std::string name;
    bool remove = false;
    if (tok[0] == '-') {
        name = tok.substr(1);
        remove = true;
    } else {
        name = tok;
    }

    bool didSet = false;

    {
        ScopedMutex lock(mMutex);

        // first, just try to find it by name, which is fast
        if (const TraceArea *area = TraceArea::find(name)) {
            if (remove) {
                mAreas.erase(area);
            } else {
                mAreas.insert(area);
            }
            didSet = true;
        } else {
            // couldn't find it by name, better search by group
            const bool all = (name == "all"); // minor optimization
            // ok, now walk over all the trace areas
            for (std::vector<const TraceArea*>::const_iterator ar = allAreas.begin(); ar != allAreas.end(); ++ar) {
                if (all) { // if we're setting "all" of them, just set it and move on to the next one
                    if (remove) {
                        mAreas.erase(*ar);
                    } else {
                        mAreas.insert(*ar);
                    }
                    didSet = true;
                    continue;
                }
                // we're not setting all of them, better search this area's
                // groups. this is our worst case.
                const std::vector<std::string> groups = (*ar)->getGroups();
                for (std::vector<std::string>::const_iterator g = groups.begin(); g != groups.end(); ++g) {
                    if (name == *g) {
                        if (remove) {
                            mAreas.erase(*ar);
                        } else {
                            mAreas.insert(*ar);
                        }
                        didSet = true;
                        break;
                    }
                }
            }
        }
    }
    return didSet;
}

void TraceAreas::parse(const std::string &p)
{
    std::set<std::string> badNames;
    {
        ScopedMutex lock(mMutex);
        mAreas.clear(); // this doesn't forget what we had before; mSpec gets appended to and we recheck it
        if (p == "none" || p.empty()) {
            mSpec.clear();
        } else {
            size_t sz = mSpec.size();
            StringTokenizer::tokenize(p, mSpec, ",");

            size_t s = 0;
            for (std::vector<std::string>::const_iterator tok = mSpec.begin(); tok != mSpec.end(); ++tok, ++s) {
                if (!set(*tok) && (s >= sz)) {
                    badNames.insert(*tok);
                }
            }
        }
        const std::vector<const TraceArea*> &allAreas = TraceArea::getAllAreas();
        for (std::vector<const TraceArea*>::const_iterator ar = allAreas.begin(); ar != allAreas.end(); ++ar) {
            mSeenAreas.insert(*ar);
        }
    }
    for (std::set<std::string>::const_iterator st = badNames.begin(); st != badNames.end(); ++st)
        Log::warn(TRACE_TRACE, "trace area %s does not exist", st->c_str());
}

bool TraceAreas::isEnabled(const TraceArea *area)
{
    ScopedMutex lock(mMutex);

    if (mAreas.find(area) != mAreas.end())
        return true;

    if (mSeenAreas.find(area) != mSeenAreas.end())
        return false;
    mSeenAreas.insert(area);

    // recalculate from mSpec
    for (std::vector<std::string>::const_iterator tok = mSpec.begin(); tok != mSpec.end(); ++tok) {
        set(*tok);
    }

    return (mAreas.find(area) != mAreas.end());
}
