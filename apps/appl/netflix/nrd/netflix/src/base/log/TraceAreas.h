/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#ifndef TraceAreas_h
#define TraceAreas_h

#include <set>
#include <string>
#include <nrdbase/StdInt.h>

#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/WeakPtrSet.h>

namespace netflix {

/**
 * @class TraceAreas TraceAreas.h
 * @brief Class for specifying trace areas.
 */
class TraceAreas
{
public:
    TraceAreas();
    void parse(const std::string &spec);
    std::string spec() const;
    bool isEmpty() const { ScopedMutex lock(mMutex); return mAreas.empty(); }
    std::set<const TraceArea*> areas() const { ScopedMutex lock(mMutex); return mAreas; }
    bool isEnabled(const TraceArea *area);

    static std::vector<std::string> all();

private:
    bool set(std::string area);

    std::vector<std::string> mSpec;
    std::set<const TraceArea*> mAreas;
    std::set<const TraceArea*> mSeenAreas;
    mutable Mutex mMutex;
};

} // namespace netflix

#endif
