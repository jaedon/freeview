/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "ObjectCount.h"
#include <algorithm>
#include <nrdbase/Dumper.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/Variant.h>

using namespace netflix;

namespace netflix {
namespace ObjectCount {

#ifndef NRDP_HAS_OBJECTCOUNT
void init() {}
void dump(unsigned int, const std::string &, netflix::Variant*) {}
std::vector<std::string> types() {return std::vector<std::string>(); }
#else
static pthread_once_t sOnce = PTHREAD_ONCE_INIT;
static struct Data
{
    std::map<std::string, Record *> records;
} *sData = 0;
bool Record::sShutdown = false;
static void initData() { sData = new ObjectCount::Data(); }
static Data &data()
{
    pthread_once(&sOnce, ObjectCount::initData);
    return *sData;
}

void init()
{
    initData();
}

Record::Record(const std::string &n)
    : name(n), current(0), prevCurrent(0), high(0), prevHigh(0),
      total(0), prevTotal(0), prevCost(0),
      mutex(OBJECT_COUNT_MUTEX, "ObjectCountRecord")
{
    Data &d = data();
    d.records[name] = this;
}

Record::~Record()
{
    sShutdown = true;
    Data &d = data();
    d.records.erase(name);
}

std::vector<std::string> types()
{
    assert(!Record::sShutdown);
    Data &d = data();
    return mapKeys(d.records);
}

class ObjectCountDumper : public Dumper
{
public:
    ObjectCountDumper(unsigned int flags) : mFlags(flags) { }

    std::vector<std::string> &objects(const std::string &key) { return mObjects[key]; }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx,
                                                  const std::string& section,
                                                  const netflix::Variant& value);

private:
    unsigned int mFlags;
    std::map<std::string, std::vector<std::string> > mObjects;
};

std::vector<std::string> ObjectCountDumper::consoleEntry(const unsigned int /*idx*/,
                                                         const std::string &section,
                                                         const netflix::Variant &value)
{
    std::vector<std::string> ret;
    if (mFlags & ObjectCount::ShowDiff) {
        std::string desc = StringFormatter::sformat("%s: %s: %s %s: %s %s: %s",
                                                    section.c_str(),
                                                    "cur", prettyFormat(Format_Number, value["cur"].integer(), value["prevCur"].integer()).c_str(),
                                                    "high", prettyFormat(Format_Number, value["high"].integer(), value["prevHigh"].integer()).c_str(),
                                                    "total", prettyFormat(Format_Number, value["total"].integer(), value["prevTotal"].integer()).c_str());
        if(const int cost = value["cost"].integer())
            desc += StringFormatter::sformat(" cost: %s", prettyFormat(Format_Bytes, cost, value["prevCost"].integer()).c_str());
        ret.push_back(desc);
    } else {
        std::string desc = StringFormatter::sformat("%s: %s: %s %s: %s %s: %s",
                                                    section.c_str(),
                                                    "cur", prettyFormat(Format_Number, value["cur"].integer()).c_str(),
                                                    "high", prettyFormat(Format_Number, value["high"].integer()).c_str(),
                                                    "total", prettyFormat(Format_Number, value["total"].integer()).c_str());
        if(const int cost = value["cost"].integer())
            desc += StringFormatter::sformat(" cost: %s", prettyFormat(Format_Bytes, cost).c_str());
        ret.push_back(desc);
    }
    if (mFlags & ObjectCount::ShowObjects) {
        int count = 0;
        const std::vector<std::string> &objects = mObjects[section];
        for (std::vector<std::string>::const_iterator oit = objects.begin(); oit != objects.end(); ++oit)
            ret.push_back(StringFormatter::sformat(" %d: %s", ++count, oit->c_str()));
    }
    return ret;
}

void dump(unsigned int flags, const std::string &filter, netflix::Variant *variant)
{
    if (Record::sShutdown)
        return;
    ObjectCountDumper dumper(flags);
    if(flags & ShowDiff)
        dumper.keys() << "cur" << "prevCur" << "high" << "prevHigh" << "total" << "prevTotal" << "cost" << "prevCost";
    else
        dumper.keys() << "cur" << "high" << "total" << "cost";
    {
        Data &d = data();
        for (std::map<std::string, Record*>::const_iterator it = d.records.begin(); it != d.records.end(); ++it) {
            Record *record = it->second;
            if ((!filter.empty() && !contains(record->name, filter)) || ((flags & Shutdown) && !record->current))
                continue;

            ScopedMutex l(record->mutex);
            record->begin();
            int cost = 0;
            std::vector<std::string> objects;
            for(std::set<void*>::const_iterator oit = record->objects.begin(); oit != record->objects.end(); ++oit) {
                if (flags & ShowObjects)
                    objects.push_back(record->describe((*oit)));
                cost += record->cost((*oit));
            }
            if(flags & ShowDiff) {
                dumper.values(record->name) << record->current << record->prevCurrent
                                            << record->high << record->prevHigh
                                            << record->total << record->prevTotal
                                            << cost << record->prevCost;
                record->prevCost = cost;
                record->prevCurrent = record->current;
                record->prevHigh = record->high;
                record->prevTotal = record->total;
            } else {
                dumper.values(record->name) << record->current << record->high
                                            << record->total << cost;
            }
            dumper.objects(record->name) = objects;
            record->end();
        }
    }

    if (variant) {
        *variant = dumper.variant();
    } else {
        const std::vector<std::string>& out = dumper.toConsole();
        for (std::vector<std::string>::const_iterator it = out.begin(); it != out.end(); ++it) {
            Log::Message message(TRACE_LOG, Log::Warn);
            message.m_flags &= ~(Log::Message::SendToJS);
            message.m_message = *it;
            message.send();
        }
    }
}
#endif
}
}
