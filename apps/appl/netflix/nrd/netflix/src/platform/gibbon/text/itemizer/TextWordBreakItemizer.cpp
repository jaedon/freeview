/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextWordBreakItemizer.h"
#include <IteratorsCache.h>
#include <nrd/AppLog.h>
#include <iterator>

using namespace netflix::gibbon;
using namespace icu;

#ifdef BUILD_DEBUG
void TextWordBreakRuns::addRun(uint32_t start, uint32_t end, bool newParagraph, UnicodeString word)
{
    TextWordBreakRun run = TextWordBreakRun(start, end, newParagraph, word);
    Base::add(run);
}
#endif
void TextWordBreakRuns::addRun(uint32_t start, uint32_t end, bool newParagraph)
{
    TextWordBreakRun run = TextWordBreakRun(start, end, newParagraph);
    Base::add(run);
}

bool cmp(const TextWordBreakRun &a, const TextWordBreakRun &b)
{
    return a.start < b.start;
}

bool cmpReverse(const TextWordBreakRun &a, const TextWordBreakRun &b)
{
    return a.start < b.start;
}

TextWordBreakRun TextWordBreakRuns::getBoundary(uint32_t start, uint32_t end, uint32_t &index) const
{
    const TextWordBreakRunsList &breaks = Base::get();

    TextWordBreakRunsList::const_iterator it = std::upper_bound(breaks.begin(), breaks.end(), TextWordBreakRun(start, 0, false), cmp);
    if (it == breaks.end() || it->start > end) {
        index = INT_MAX;
        bool nextIsNewParagraph = it != breaks.end() && it->start == end + 1 ? it->newParagraph: false;

        return TextWordBreakRun(INT_MAX, INT_MAX, nextIsNewParagraph);
    }
    index = std::distance(breaks.begin(), it);
    return *it;
}

TextWordBreakRun TextWordBreakRuns::getRightBoundary(uint32_t /*start*/, uint32_t end, uint32_t &index) const
{
    const TextWordBreakRunsList &breaks = Base::get();

    TextWordBreakRunsList::const_iterator it = std::upper_bound(breaks.begin(), breaks.end(), TextWordBreakRun(end, 0, false), cmpReverse);

    if (it == breaks.end() /*|| it->start < start*/) {
        index = INT_MAX;
        return TextWordBreakRun(INT_MAX, INT_MAX, false);
    }
    index = std::distance(breaks.begin(), it);
    return *it;
}

TextWordBreakRun TextWordBreakRuns::getPreviousBoundary(uint32_t &index) const
{
    const TextWordBreakRunsList &breaks = Base::get();

    if (index == 0 || index == INT_MAX || breaks.size() == 0 || (index > breaks.size()))
        return TextWordBreakRun(INT_MAX, INT_MAX, false);
    index--;
    return breaks[index];
}

TextWordBreakRun TextWordBreakRuns::getNextBoundary(uint32_t &index) const
{
    const TextWordBreakRunsList &breaks = Base::get();

    if (index == INT_MAX || breaks.size() == 0 || (index >= breaks.size() - 1))
        return TextWordBreakRun(INT_MAX, INT_MAX, false);
    index++;
    return breaks[index];
}

TextWordBreakRun TextWordBreakRuns::iterateBoundaries(uint32_t start, uint32_t end, uint32_t &index, int direction) const
{
    TextWordBreakRun result;

    if (index == 0) { // initialize
        if (direction == LTR) {
            result = getBoundary(start, end, index);
            return result;
        } else {
            result = getRightBoundary(start, end, index);
            return result;
        }
    }
    result = getNextBoundary(index);
    return result;
}

TextWordBreakRuns TextWordBreakItemizer::processWordBreaks(UnicodeString &text, TextLocaleRuns& localeRuns)
{
    TextWordBreakRuns wordBreaks;
    TextLocaleRuns::TextLocaleRunsList::const_iterator it = localeRuns.get().begin();

    bool insertPSEP = false;
    bool hasPSEP = false;

    int32_t start = 0;
    int32_t end = 0;
    uint32_t offset = 0;

    while (it != localeRuns.get().end()) {
        //TIDEBUG("Word breaking for locale " << it->get()->locale);
        IteratorsCache::Iterator *bi = IteratorsCache::getLineIterator(it->get()->locale);
        assert(bi != NULL);

        uint32_t s = it->get()->start;
        uint32_t e = it->get()->end;
        if (e == INT_MAX)
            e = text.length();

        UnicodeString sentence = UnicodeString(text, s, e - s + 1);
        //TIDEBUG("string: |" << sentence << "|");
        bi->setText(sentence);

        //TODO: See if we can use a UCharCharacterIterator, as it would be faster.
#if 0
        UErrorCode status = U_ZERO_ERROR;
        UText *ut = NULL;
        //UCharCharacterIterator citr = UCharCharacterIterator(text.getBuffer(), e - s, s + 1);
        UCharCharacterIterator citr = UCharCharacterIterator(text.getBuffer(), text.length(), s);

        ut = utext_openCharacterIterator(ut, &citr, &status);
        if (U_FAILURE(status)) {
            Log::error(TRACE_LOG, "[%s:%d]: Unable to open CharacterIterator UText (%s)", __func__, __LINE__, u_errorName(status));
        }

        status = U_ZERO_ERROR;
        bi->setText(ut, status);
        if (U_FAILURE(status)) {
            Log::error(TRACE_LOG, "[%s:%d]: Unable to ->setText (%s)", __func__, __LINE__,u_errorName(status));
        }
#endif

        start = bi->first();
        end = start;
        offset = it->get()->start;

        for (end = bi->next(); end != IteratorsCache::Iterator::DONE; start = end, end = bi->next()) {
            insertPSEP = hasPSEP;
            hasPSEP = false;
            UChar32 lastCharacter = sentence.char32At(end - 1);
            if (lastCharacter == 0x000A || // U+000A LINE FEED (\n)
                lastCharacter == 0x2029) {  // U+2029 PARAGRAPH SEPARATOR (PSEP)
                hasPSEP = true;
            }

#ifdef BUILD_DEBUG
            UnicodeString word;
            word.setTo(sentence, start, end - start);
            wordBreaks.addRun(offset + start, offset + end - 1, insertPSEP, word);
            //TIDEBUG("|" << word << "|" << hasPSEP);
#else
            wordBreaks.addRun(offset + start, offset + end - 1, insertPSEP);
            //UnicodeString word;
            //word.setTo(sentence, start, end - start);
            //TIDEBUG("|" << word << "|");
#endif
        }

        // We are done with the iterator. Put it back in the cache.
        IteratorsCache::putIterator(it->get()->locale, bi);

        ++it;
    }

    // Check for trailing new lines and add the appropiate synthetic word boundaries
    if (hasPSEP) {
#ifdef BUILD_DEBUG
        wordBreaks.addRun(start, start, true, UnicodeString("\n"));
#else
        wordBreaks.addRun(start, start, true);
#endif
    }

    return wordBreaks;
}
