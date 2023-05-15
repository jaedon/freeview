/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef ObjectCount_h
#define ObjectCount_h

#include <nrdbase/config.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/ScopedMutex.h>
#include <string>
#include <vector>
#include <set>

#define NRDP_DEFINE_OBJECTCOUNT_RECORD(recordtype, name) namespace netflix { namespace ObjectCount { recordtype name(#name); } }
#define NRDP_DECLARE_OBJECTCOUNT_RECORD(recordtype, name) namespace netflix { namespace ObjectCount { extern recordtype name; } }
#define NRDP_DEFINE_OBJECTCOUNT(name) NRDP_DEFINE_OBJECTCOUNT_RECORD(Record, name)
#define NRDP_DECLARE_OBJECTCOUNT(name) NRDP_DECLARE_OBJECTCOUNT_RECORD(Record, name);

#ifdef NRDP_HAS_OBJECTCOUNT
#  define NRDP_OBJECTCOUNT_COUNT(record, add) netflix::ObjectCount::record.count(add)
#  define NRDP_OBJECTCOUNT_REF(record, obj) netflix::ObjectCount::record.count(+1, obj)
#  define NRDP_OBJECTCOUNT_DEREF(record, obj) netflix::ObjectCount::record.count(-1, obj)
#  define NRDP_OBJECTCOUNT_DESCRIBE(obj, desc) obj.setObjectCountDescription(desc)
#else
#  define NRDP_OBJECTCOUNT_COUNT(record, add)
#  define NRDP_OBJECTCOUNT_REF(record, obj)
#  define NRDP_OBJECTCOUNT_DEREF(record, obj)
#  define NRDP_OBJECTCOUNT_DESCRIBE(obj, desc)
#endif

namespace netflix {
class Dumper;
class Variant;

namespace ObjectCount
{
enum DumpFlags {
    None        = 0x00,
    ShowObjects = 0x01,
    ShowDiff    = 0x02,
    Shutdown    = 0x04
};
void init();
struct Record {
#ifndef NRDP_HAS_OBJECTCOUNT
    inline Record(const std::string &) {}
    virtual ~Record() {}
    inline long long currentCost() { return 0; }
    virtual void begin() {}
    virtual void end() {}
#else
    Record(const std::string &n);
    virtual ~Record();
    virtual void begin() {}
    virtual void end() {}
    inline void count(long long add, void *object = 0)
    {
        if (sShutdown)
            return;

        ScopedMutex lock(mutex);
        current += add;
        assert(current >= 0);
        if (add > 0) {
            if (object) {
                assert(objects.find(object) == objects.end());
                objects.insert(object);
            }
            high = std::max(current, high);
            total += add;
        } else if (object) {
            assert(objects.find(object) != objects.end());
            objects.erase(object);
        }
    }
    virtual size_t cost(void *) const { return 0; }
    virtual std::string describe(void *object) const { return StringFormatter::sformat("%p", object); }

    inline long long currentCost()
    {
        if (sShutdown)
            return 0;
        int result = 0;
        ScopedMutex lock(mutex);
        begin();
        for(std::set<void*>::const_iterator oit = objects.begin(); oit != objects.end(); ++oit)
            result += cost((*oit));
        end();
        return result;
    }

    const std::string name;
    long long current, prevCurrent, high, prevHigh, total, prevTotal, prevCost;
    mutable Mutex mutex;
    std::set<void*> objects;
    static bool sShutdown;
#endif
};

std::vector<std::string> types();
void dump(unsigned int flags = None, const std::string &filter = std::string(), Variant* variant = 0);
};

}

#endif
