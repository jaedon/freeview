/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "WriteLimiter.h"
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/Configuration.h>
#include <assert.h>
#include <nrdbase/Log.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/tr1.h>

using namespace netflix;

WriteLimiter::WriteLimiter(const std::string &name)
    : mName(name), mLastWriteAmount(0), mLimit(-1), mUsed(0),
      mBlocked(0), mBlockedSize(0), mMutex(WRITELIMITER_MUTEX, "WriteLimiter::mMutex")
{
}

WriteLimiter::~WriteLimiter()
{
}

void WriteLimiter::init(int limit)
{
    const DataBuffer read = readPersistentData();
    {
        ScopedMutex lock(mMutex);
        mLimit = limit;

        if (Configuration::resetWriteLimits()) {
            NTRACE(TRACE_WRITELIMITER, "%s Resetting write limits", mName.c_str());
        } else if (static_cast<size_t>(read.size()) == sizeof(uint8_t) + sizeof(int) + sizeof(ullong)
                   && *read.data<uint8_t>() == PersistentDataVersion) {
            const unsigned char *data = read.data<const unsigned char>() + 1;
            mLastWriteAmount = *reinterpret_cast<const int *>(data);
            data += sizeof(int);
            mLastWriteTime = *reinterpret_cast<const ullong*>(data);
            NTRACE(TRACE_WRITELIMITER, "%s Restored %s and %d from persistent data", mName.c_str(),
                   mLastWriteTime.calendarTimeText().c_str(), mLastWriteAmount);
        } else if (read.isEmpty()) {
            NTRACE(TRACE_WRITELIMITER, "%s No persistent data", mName.c_str());
        } else {
            NTRACE(TRACE_WRITELIMITER, "%s No persistent data wrong size or version %d %d (%d)",
                   mName.c_str(), read.size(), *read.data<uint8_t>(), PersistentDataVersion);

        }
    }

    if (Time::hasServerTime())
        onTimeChanged();
}

int WriteLimiter::limit() const
{
    ScopedMutex lock(mMutex);
    return mLimit;
}

void WriteLimiter::setLimit(int limit)
{
    ScopedMutex lock(mMutex);
    mLimit = limit;
}

bool WriteLimiter::isEnabled() const
{
    ScopedMutex lock(mMutex);
    return mLimit > 0;
}

bool WriteLimiter::hasCurrentDate() const
{
    ScopedMutex lock(mMutex);
    return mStartTime.val();
}

int WriteLimiter::used() const
{
    ScopedMutex lock(mMutex);
    return mUsed;
}

void WriteLimiter::resetUsed()
{
    ScopedMutex lock(mMutex);
    mUsed = 0;
    mPending.clear();
}

int WriteLimiter::available() const
{
    ScopedMutex lock(mMutex);
    return std::max(0, mLimit - mUsed);
}

void WriteLimiter::commit(int size)
{
    int used;
    Time startTime;
    {
        ScopedMutex lock(mMutex);
        if (!mStartTime.val()) {
            NTRACE(TRACE_WRITELIMITER, "%s Adding pending entry of %d", mName.c_str(), size);
            const Pending pending = { Time::mono(), size };
            mPending.push_back(pending);
            return;
        }
        const Time now = Time::now();
        if ((mStartTime.val() / MilliSecondsPerDay) != (now.val() / MilliSecondsPerDay)) {
            NTRACE(TRACE_WRITELIMITER, "%s Resetting used to 0 because of %s vs %s", mName.c_str(),
                   mStartTime.calendarTimeText().c_str(), now.calendarTimeText().c_str());
            mUsed = 0;
            mStartTime = now;
        }
        mUsed += size;
        used = mUsed;
        startTime = mStartTime;
        NTRACE(TRACE_WRITELIMITER, "%s Committing %d bytes %d/%d (%.1f%%)", mName.c_str(), size, mUsed, mLimit,
               mLimit ? static_cast<float>((mUsed / static_cast<float>(mLimit))) * 100. : 0.);
    }
    DataBuffer data(sizeof(uint8_t) + sizeof(int) + sizeof(ullong), '\0');
    unsigned char *ch = data.data();
    *ch = static_cast<uint8_t>(PersistentDataVersion);
    ++ch;
    *reinterpret_cast<int*>(ch) = used;
    ch += sizeof(int);
    *reinterpret_cast<ullong*>(ch) = startTime.val();
    NTRACE(TRACE_WRITELIMITER, "%s Writing %s %d to persistent data", mName.c_str(),
           startTime.calendarTimeText().c_str(), mUsed);
    writePersistentData(data);
}

void WriteLimiter::onTimeChanged()
{
    ScopedMutex lock(mMutex);
    if (mStartTime.val())
        return;
    mStartTime = Time::now();
    const ullong dailyMs = mStartTime.val() % MilliSecondsPerDay;
    const Time limit = Time::mono() - dailyMs;
    NTRACE(TRACE_WRITELIMITER, "%s Got time changed %s limit is %llu", mName.c_str(), mStartTime.calendarTimeText().c_str(), limit.val());
    if ((mStartTime.val() / MilliSecondsPerDay) == (mLastWriteTime.val() / MilliSecondsPerDay)) {
        NTRACE(TRACE_WRITELIMITER, "%s Adding %d to used from persisted data %s and %s", mName.c_str(), mLastWriteAmount,
               mStartTime.calendarTimeText().c_str(), mLastWriteTime.calendarTimeText().c_str());
        mUsed += mLastWriteAmount;
    } else {
        NTRACE(TRACE_WRITELIMITER, "%s Discarding persisted data %d because of times %s vs %s", mName.c_str(), mLastWriteAmount,
               mStartTime.calendarTimeText().c_str(), mLastWriteTime.calendarTimeText().c_str());
    }
    for (std::vector<Pending>::const_iterator it = mPending.begin(); it != mPending.end(); ++it) {
        if (it->mono >= limit) {
            NTRACE(TRACE_WRITELIMITER, "%s Adding pending record %d at %llu", mName.c_str(), it->bytes, it->mono.val());
            mUsed += it->bytes;
        } else {
            NTRACE(TRACE_WRITELIMITER, "%s Discarding pending record %d at %llu", mName.c_str(), it->bytes, it->mono.val());
        }
    }
    mPending.clear();
}

std::vector<std::string> WriteLimiter::Dumper::consoleEntry(const unsigned int, const std::string&, const Variant& value)
{
    const int limit = value.mapValue<int>("limit");
    if (limit <= 0)
        return std::vector<std::string>(1, StringFormatter::sformat("%s: disabled", value.mapValue<std::string>("name").c_str()));

    const int used = value.mapValue<int>("used");
    return std::vector<std::string>(1, StringFormatter::sformat("%s: %s/%s (%.2f%%) blocked: %d (%s)",
                                                                value.mapValue<std::string>("name").c_str(),
                                                                prettyBytes(used).c_str(),
                                                                prettyBytes(limit).c_str(),
                                                                (static_cast<double>(used) / static_cast<double>(limit)) * 100.0,
                                                                value.mapValue<int>("blocked"),
                                                                prettyBytes(value.mapValue<int>("blockedSize")).c_str()));
}

void WriteLimiter::block(int size)
{
    ScopedMutex lock(mMutex);
    ++mBlocked;
    mBlockedSize += size;
}

int WriteLimiter::blocked() const
{
    ScopedMutex lock(mMutex);
    return mBlocked;
}

int WriteLimiter::blockedSize() const
{
    ScopedMutex lock(mMutex);
    return mBlockedSize;
}
