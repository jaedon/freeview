/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#ifndef TextWordBreakItemizer_H
#define TextWordBreakItemizer_H

#include "TextItemizerCommon.h"
#include "TextLocaleItemizer.h"

namespace netflix {
namespace gibbon {

class TextWordBreakRun : public BaseTextRun {
public:
    typedef shared_ptr<TextWordBreakRun> SharedPtr;
    TextWordBreakRun() : BaseTextRun(INT_MAX, INT_MAX), newParagraph(false) {}
    TextWordBreakRun(uint32_t _start, uint32_t _end, bool _newParagraph) : BaseTextRun(_start, _end), newParagraph(_newParagraph) {}
#ifdef BUILD_DEBUG
    TextWordBreakRun(uint32_t _start, uint32_t _end, bool _newParagraph, icu::UnicodeString _word) : BaseTextRun(_start, _end), newParagraph(_newParagraph), word(_word) {}
#endif
    virtual ~TextWordBreakRun() {}

    bool newParagraph;
#ifdef BUILD_DEBUG
    icu::UnicodeString word;
#endif
};

class TextWordBreakRuns : public BaseTextRuns<TextWordBreakRun> {
public:
    typedef std::vector<TextWordBreakRun> TextWordBreakRunsList;
    typedef BaseTextRuns<TextWordBreakRun> Base;
    void addRun(uint32_t start, uint32_t end, bool newParagraph);
#ifdef BUILD_DEBUG
    void addRun(uint32_t start, uint32_t end, bool newParagraph, icu::UnicodeString word);
#endif
    TextWordBreakRun getBoundary(uint32_t start, uint32_t end, uint32_t &index) const;
    TextWordBreakRun getRightBoundary(uint32_t start, uint32_t end, uint32_t &index) const;
    TextWordBreakRun getPreviousBoundary(uint32_t &index) const;
    TextWordBreakRun getNextBoundary(uint32_t &index) const;
    TextWordBreakRun iterateBoundaries(uint32_t start, uint32_t end, uint32_t &index, int direction) const;

    TextWordBreakRuns() : Base() {}
#if CAPTURE_COPY_CONSTRUCTORS
    TextWordBreakRuns(const TextWordBreakRuns &) : Base() {
        assert(false);
    }
#endif
};

class TextWordBreakItemizer {
public:
    static TextWordBreakRuns processWordBreaks(icu::UnicodeString &text, TextLocaleRuns& localeRuns);
};

inline std::ostream &operator<< (std::ostream &out, const TextWordBreakRun& run)
{
    out << BASE_ITEMIZER_STREAM(run) << ":";
    if (run.newParagraph)
        out << " (PSEP)";
#ifdef BUILD_DEBUG
    out << " |" << run.word << "|";
#endif
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextWordBreakRun* run)
{
    out << (*run);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextWordBreakRuns& runs)
{
    TextWordBreakRuns::TextWordBreakRunsList::const_iterator it = runs.get().begin();
    TextWordBreakRuns::TextWordBreakRunsList::const_iterator end = runs.get().end();

    out << "Word breaks" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << *it << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

}}

#endif /* TextWordBreakItemizer_H */
