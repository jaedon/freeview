/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextDirectionItemizer_H_
#define TextDirectionItemizer_H_

#include "TextItemizerCommon.h"
#include <Style.h>
#include <TextLocale.h>

namespace netflix {
namespace gibbon {

class TextDirectionRun : public BaseTextRun {
public:
    enum Direction {
        DIRECTION_LTR,
        DIRECTION_RTL
    };

    typedef netflix::gibbon::BaseTextRuns<TextDirectionRun>::TextRunsList TextDirectionRunsList;
    typedef shared_ptr<TextDirectionRun> SharedPtr;
    TextDirectionRun(uint32_t _start, uint32_t _end, int _sequence, Direction _direction, TextDirectionRunsList::size_type _visualOrder) : BaseTextRun(_start, _end), direction(_direction), visualOrder(_visualOrder), sequence(_sequence) {}
    virtual ~TextDirectionRun() {}

    Direction direction;
    int visualOrder;
    int sequence;
};

class TextDirectionRuns : public BaseTextRuns<TextDirectionRun::SharedPtr> {
public:
    typedef std::vector<TextDirectionRun::SharedPtr> TextDirectionRunsList;
    typedef BaseTextRuns<TextDirectionRun::SharedPtr> Base;
    void addRun(uint32_t start, uint32_t end, int sequence, TextDirectionRun::Direction direction);
    void toLogicalOrder();
    void toVisualOrder();

    TextDirectionRuns() : Base() {}
#if CAPTURE_COPY_CONSTRUCTORS
    TextDirectionRuns(const TextDirectionRuns&) : Base() {
        assert(false);
    }
#endif
};


class TextDirectionItemizer {
public:
    static void processDirection(TextDirectionRuns &textRuns, icu::UnicodeString &text, Style::SharedPtr style);
};

//-----------------------------------------------------------------------------

inline std::ostream &operator<< (std::ostream &out, const TextDirectionRun::Direction dir)
{
    if (dir == TextDirectionRun::DIRECTION_LTR)
        out << "LTR";
    else if (dir == TextDirectionRun::DIRECTION_RTL)
        out << "RTL";
    else
        out << "###";

    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextDirectionRun& dir)
{
    out << BASE_ITEMIZER_STREAM(dir) << ": " << dir.direction << " (" << dir.visualOrder << "-" << dir.sequence << ")";
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextDirectionRun* dir)
{
    out << (*dir);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextDirectionRuns& runs)
{
    TextDirectionRuns::TextDirectionRunsList::const_iterator it = runs.get().begin();
    TextDirectionRuns::TextDirectionRunsList::const_iterator end = runs.get().end();

    out << "Text direction runs" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << it->get() << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

}}

#endif /* TextDirectionItemizer_H_ */
