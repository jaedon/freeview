/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextItemizerCommon_H_
#define TextItemizerCommon_H_

#include <iomanip>
#include <nrdbase/tr1.h>
#include <nrdbase/Noncopyable.h>
#include <unicode/unistr.h>
#include <nrd/AppLog.h>


#define TIDEBUG(s) Log::info(TRACE_GIBBON_ITEMIZER) << s
#define TIRANGE(a,b) a<<"-"<<b
#define BASE_ITEMIZER_STREAM(x) static_cast<BaseTextRun>(x)

#define LTR 0
#define RTL 1

namespace netflix {
namespace gibbon {

class BaseTextRun {
public:
    BaseTextRun() : start(0), end(0) {}
    BaseTextRun(uint32_t _start, uint32_t _end) : start(_start), end(_end) {}
     virtual ~BaseTextRun() {}
    typedef shared_ptr<BaseTextRun> SharedPtr;
    uint32_t start;
    uint32_t end;
};

template<class T>
class BaseTextRuns {
public:
    typedef std::vector<T> TextRunsList;

    BaseTextRuns() {}
    virtual ~BaseTextRuns() {}

    void add(T item) {
        m_vector.push_back(item);
    }

    typename TextRunsList::size_type size() const {
        return m_vector.size();
    }

    const T operator[](typename TextRunsList::size_type index) {
        return m_vector[index];
    }

    inline const TextRunsList &get() const {
        return m_vector;
    }

    inline TextRunsList &get() {
        return m_vector;
    }

protected:
    TextRunsList m_vector;
};

inline std::ostream &operator<< (std::ostream &out, const netflix::gibbon::BaseTextRun& run)
{
    std::ios state(NULL);
    state.copyfmt(out);

    out << std::right << std::setfill(' ') << std::setw(3)  << run.start << "-" << std::setw(3) << std::left << run.end;

    out.copyfmt(state);
    return out;
}

}}

namespace U_ICU_NAMESPACE {
inline std::ostream &operator<< (std::ostream &out, const UnicodeString& text)
{
    std::string utf8;
    text.toUTF8String(utf8);
    out << utf8;
    return out;
}
}


#endif /* TextItemizerCommon_H_ */
