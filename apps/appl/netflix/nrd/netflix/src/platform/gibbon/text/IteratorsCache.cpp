/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <ctype.h>

#include <gibbon/config.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/utf8.h>

#include "IteratorsCache.h"
#include "TextEngine.h"

#include <unicode/rbbi.h>

using namespace icu;

using namespace netflix::gibbon;
using namespace netflix;

static IteratorsCache::Cache sTextIterators;
static Mutex sTextEngineIteratorsCacheMutex(netflix::UNTRACKED_MUTEX, "GibbonTextIteratorsCacheMutex");

IteratorsCache::Iterator* IteratorsCache::getLineIterator(const std::string& locale)
{
    ScopedMutex mutex(sTextEngineIteratorsCacheMutex);

    Iterator* result;

    Cache::iterator it = sTextIterators.find(locale);

    if (it == sTextIterators.end()) {
        result = new Iterator(locale);
        if (!result->isValid()) {
            delete result;
            return 0;
        }
    } else {
        result = it->second[0];
        it->second.erase(it->second.begin());

        if (it->second.size() == 0)
            sTextIterators.erase(locale);
    }

    return result;
}


void IteratorsCache::putIterator(const std::string& locale, Iterator* iterator)
{
    ScopedMutex mutex(sTextEngineIteratorsCacheMutex);

    Cache::iterator it = sTextIterators.find(locale);

    if (it != sTextIterators.end()) {
        it->second.push_back(iterator);
    } else {
        std::vector<Iterator*> its;
        its.push_back(iterator);
        sTextIterators[locale] = its;
    }
}

void IteratorsCache::clearIterators()
{
    ScopedMutex mutex(sTextEngineIteratorsCacheMutex);

    Cache::iterator it = sTextIterators.begin();

    while (it != sTextIterators.end()) {
        std::vector<Iterator*>::iterator bit = it->second.begin();
        while (bit != it->second.end()) {
            delete *bit;
            ++bit;
        }
        it->second.clear();
        ++it;
    }
    sTextIterators.clear();
}

IteratorsCache::Iterator::Iterator(const std::string& locale)
    : icu(0)
{
    UErrorCode status = U_ZERO_ERROR;
    UParseError parse;

    const icu::Locale ulocale(locale.c_str());
    if (!strcmp(ulocale.getLanguage(), "zh") ||
        !strcmp(ulocale.getLanguage(), "ja") ||
        !strcmp(ulocale.getLanguage(), "ko")) {
        const std::string rulesPath(Configuration::dataReadPath() + "/icu/icu_rules.txt");
        const DataBuffer rules = DataBuffer::fromFile(rulesPath);
        if (!rules.isEmpty()) {
            icu = new RuleBasedBreakIterator(UnicodeString::fromUTF8(StringPiece(rules.data<const char>(), rules.size())), parse, status);
        }
        if (!icu) {
            Log::error(TRACE_LOG, "Unable to load custom ICU rules, falling back to defaults (%s)", rulesPath.c_str());
        }
    }
    if (!icu) {
        icu = BreakIterator::createLineInstance(ulocale, status);
    }
    if (U_FAILURE(status)) {
        delete icu;
        Log::error(TRACE_LOG, "[%s:%d]: UNABLE TO INSTANTIATE BREAK ITERATOR FOR LOCALE %s (%s)", __func__, __LINE__, locale.c_str(), u_errorName(status));
        icu = 0;
    }
/*
    if (status == U_USING_FALLBACK_WARNING) {
        Log::error(TRACE_LOG) << "Exact match for locale '" << locale << "' not found, using a fallback one";
    };

    if (status == U_USING_DEFAULT_WARNING) {
        Log::error(TRACE_LOG) << "Unable to find locale '" << locale << "', using the default";
    };
*/
}

IteratorsCache::Iterator::~Iterator()
{
    delete icu;
}

bool IteratorsCache::Iterator::isValid() const
{
    return icu != 0;
}

// UTF-8 iterator. Not used anymore but left it it just in case
#if 0
void IteratorsCache::Iterator::setText(const char* text, int length)
{
    (void)length;
    UErrorCode status = U_ZERO_ERROR;
    UText *ut = NULL;
    ut = utext_openUTF8(ut, text, -1, &status);
    if (U_FAILURE(status)) {
        Log::error(TRACE_LOG, "[%s:%d]: Unable to open UTF-8 UText (%s)", __func__, __LINE__,u_errorName(status));
        return;
    }

    status = U_ZERO_ERROR;
    icu->setText(ut, status);
    utext_close(ut); //setText creates a shallow copy, we can safely close the UText
    if (U_FAILURE(status)) {
        Log::error(TRACE_LOG, "[%s:%d]: Unable to set UText (%s)", __func__, __LINE__,u_errorName(status));
        return;
    }
}
#endif

void IteratorsCache::Iterator::setText(const UnicodeString &text)
{
    icu->setText(text);
}


int IteratorsCache::Iterator::first()
{
    return icu->first();
}

int IteratorsCache::Iterator::next()
{
    return icu->next();
}

int IteratorsCache::Iterator::last()
{
    return icu->last();
}
