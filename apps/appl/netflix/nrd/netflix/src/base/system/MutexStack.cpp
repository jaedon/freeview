/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "MutexStack.h"

#include <pthread.h>
#include <vector>

#include <nrdbase/Assert.h>
#include <nrdbase/config.h>
#include <nrdbase/Log.h>
#include <nrdbase/StringFormatter.h>
#include <nrdbase/ThreadLocalStore.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/Backtrace.h>
#include <cstdlib>

using namespace netflix;

#ifdef NRDP_MUTEX_STACK

static const std::string s_key("MutexStack");

//#define DBG_PRINTF(...)

#define DBG_PRINTF printf
#define ERR_PRINTF printf

static void nf_mutexStackErr()
{
}

//////////
//
// MutexStackLocal
//
//////////
class MutexStackLocal
{
public:
    static MutexStackLocal *current();

public:
    bool contains(const char *name) {
        for (size_t i=0; i<mStack.size(); ++i) {
            if (mStack[i].second == name)
                return true;
        }
        return false;
    }

    void push(int rank, const char *name)
    {
        // if(rank != -1 && !name) {
        //     DBG_PRINTF("You didn't name this mutex!\n");
        // }

        // check against top of stack
        const bool found = contains(name); // check to see if we already own this lock
        if(!found && !mStack.empty()) {
            const RankType top = mStack[mStack.size() - 1];
            if (rank >= top.first)
            {
                DBG_PRINTF("***************************************************\n");
                DBG_PRINTF("Warning: push mutex (%d,%s) when top is (%d,%s)\n",
                           rank, name, top.first, top.second);
                DBG_PRINTF("MutexStack: %s\n", toString().c_str());
#ifdef NRDP_HAS_EXECINFO_H
                shared_ptr<Backtrace> backtrace = Backtrace::backtrace();
                DBG_PRINTF("Backtrace:\n%s\n", backtrace->toString().c_str());
#endif
                DBG_PRINTF("***************************************************\n");
                nf_mutexStackErr();
            }
        }
        mStack.push_back(std::make_pair(rank, name));
    }

    int size() const
    {
        return mStack.size();
    }

    void pop(int rank, const char *name)
    {
        if (mStack.empty())
        {
            ERR_PRINTF("stack underflow popping (%d,%s)\n", rank, name);
            abort();
        }

        // check against top of stack
        RankType top = mStack[mStack.size() - 1];
        if ((rank != top.first) || (name != top.second))
        {
            ERR_PRINTF("Pop mutex (%d,%s) when top is (%d,%s)\n",
                       rank, name, top.first, top.second);
            DBG_PRINTF("MutexStack: %s\n", toString().c_str());
            abort();
        }
        mStack.pop_back();
    }

    std::string toString() const
    {
        std::string str;
        for (size_t i=0; i<mStack.size(); ++i)
        {
            if (!str.empty())
                str += " ";
            RankType el = mStack[i];
            str += StringFormatter::sformat("(%d,%s)", el.first, el.second);
        }
        return str;
    }

public:
    virtual ~MutexStackLocal() {}

private:
    MutexStackLocal() {}

private:
    typedef std::pair<int, const char *> RankType;
    std::vector<RankType> mStack;
};

static bool sStoreShutdown = false;
class MutexStackLocalStore : public ThreadLocalStore<MutexStackLocal>
{
public:
    ~MutexStackLocalStore()
    {
        sStoreShutdown = true;
    }
};
static MutexStackLocalStore sStoreLocals;

MutexStackLocal *MutexStackLocal::current()
{
    if (sStoreShutdown)
        return 0;
    MutexStackLocal *stack = sStoreLocals.get();
    if (!stack) {
        // create ourselves
        stack = new MutexStackLocal;
        sStoreLocals.set(stack);
    }

    return stack;
}

//////////
//
// lock
//
//////////
void MutexStack::lock(int rank, const char *name)
{
    // ignore negative ranks
    if (rank < 0 || !Configuration::mutexStackEnabled())
        return;

    if (MutexStackLocal* local = MutexStackLocal::current())
        local->push(rank, name);
}

//////////
//
// unlock
//
//////////
void MutexStack::unlock(int rank, const char *name)
{
    // ignore negative ranks
    if (rank < 0 || !Configuration::mutexStackEnabled())
        return;

    if (MutexStackLocal* local = MutexStackLocal::current())
        local->pop(rank, name);
}

//////////
//
// locked
//
//////////
bool MutexStack::locked(const char *name)
{
    if (!Configuration::mutexStackEnabled())
        return true;
    if (MutexStackLocal* local = MutexStackLocal::current())
        return local->contains(name);
    return false;
}

//////////
//
// size
//
//////////
int MutexStack::size()
{
    if (!Configuration::mutexStackEnabled())
        return 0;
    if (MutexStackLocal* local = MutexStackLocal::current())
        return local->size();
    return 0;
}


void MutexStack::dump()
{
    if (Configuration::mutexStackEnabled())
        printf("%s\n", MutexStackLocal::current() ? MutexStackLocal::current()->toString().c_str() : "");
}
#endif // NRDP_MUTEX_STACK
