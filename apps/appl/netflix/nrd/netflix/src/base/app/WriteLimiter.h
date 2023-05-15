/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef WriteLimiter_h
#define WriteLimiter_h

#include <vector>
#include <nrdbase/Mutex.h>
#include <nrdbase/DataBuffer.h>
#include <nrdbase/Variant.h>
#include <nrdbase/Time.h>
#include <nrdbase/Dumper.h>

namespace netflix {

class WriteLimiter
{
public:
    WriteLimiter(const std::string &name);
    virtual ~WriteLimiter();

    void init(int limit);

    bool isEnabled() const;

    void setLimit(int limit);
    void resetUsed();
    int limit() const;
    bool hasCurrentDate() const;
    int used() const;
    int available() const;
    int blocked() const;
    int blockedSize() const;
    void commit(int size);
    void block(int size);
    void onTimeChanged();
    std::string name() const { return mName; }

    class Dumper : public netflix::Dumper
    {
    public:
        Dumper()
            : netflix::Dumper()
        {
            keys() << "name" << "limit" << "used" << "blocked" << "blockedSize";
        }
        Dumper &operator<<(const WriteLimiter &limiter)
        {
            values() << limiter.name() << limiter.limit() << limiter.used() << limiter.blocked() << limiter.blockedSize();
            return *this;
        }
    protected:
        virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
    };

protected:
    virtual DataBuffer readPersistentData() = 0;
    virtual bool writePersistentData(const DataBuffer &data) = 0;
private:
    enum { PersistentDataVersion = 1 };
    const std::string mName;
    struct Pending {
        Time mono;
        int bytes;
    };
    Time mLastWriteTime;
    int mLastWriteAmount;
    std::vector<Pending> mPending;
    int mLimit, mUsed, mBlocked, mBlockedSize;
    Time mStartTime;
    mutable Mutex mMutex;
};

}

#endif
