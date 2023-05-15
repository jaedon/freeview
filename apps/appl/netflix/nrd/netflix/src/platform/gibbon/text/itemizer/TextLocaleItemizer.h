/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextLocaleItemizer_H
#define TextLocaleItemizer_H

#include "TextItemizerCommon.h"

#include "TextAttributesItemizer.h"
#include <string>

namespace netflix {
namespace gibbon {

class TextLocaleRun : public BaseTextRun {
public:
    typedef shared_ptr<TextLocaleRun> SharedPtr;
    TextLocaleRun(uint32_t _start, uint32_t _end, const std::string &_locale) : BaseTextRun(_start, _end), locale(_locale) {}
    virtual ~TextLocaleRun() {}

    std::string locale;
};

class TextLocaleRuns : public BaseTextRuns<TextLocaleRun::SharedPtr> {
public:
    typedef std::vector<TextLocaleRun::SharedPtr> TextLocaleRunsList;
    typedef BaseTextRuns<TextLocaleRun::SharedPtr> Base;
    void addRun(uint32_t start, uint32_t end, const std::string &locale);

    TextLocaleRuns() : Base() {}
#if CAPTURE_COPY_CONSTRUCTORS
    TextLocaleRuns(const TextLocaleRuns&) : Base() {
        assert(false);
    }
#endif
};

class TextLocaleItemizer {
public:
    static void processLocales(TextLocaleRuns& textRuns, const icu::UnicodeString& text, TextAttributeRuns &attr);
};

inline std::ostream &operator<< (std::ostream &out, const TextLocaleRun& run)
{
    out << BASE_ITEMIZER_STREAM(run) << ": " << run.locale;
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextLocaleRun* run)
{
    out << (*run);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextLocaleRuns& runs)
{
    TextLocaleRuns::TextLocaleRunsList::const_iterator it = runs.get().begin();
    TextLocaleRuns::TextLocaleRunsList::const_iterator end = runs.get().end();

    out << "Text locale runs" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << it->get() << std::endl;
        ++i;
        out.flush();
    }

    return out;
}


}}

#endif /* TextLocaleItemizer_H */
