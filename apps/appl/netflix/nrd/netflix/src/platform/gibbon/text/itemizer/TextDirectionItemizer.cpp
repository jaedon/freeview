/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextDirectionItemizer.h"
#include <unicode/ubidi.h>
#include <algorithm>

using namespace netflix::gibbon;
using namespace icu;

void TextDirectionRuns::addRun(uint32_t start, uint32_t end, int sequence, TextDirectionRun::Direction direction)
{
    Base::TextRunsList::size_type count = Base::get().size();
    Base::add(TextDirectionRun::SharedPtr(new TextDirectionRun(start, end, sequence, direction, count)));
}

inline bool visual_order(const TextDirectionRun::SharedPtr& first, const TextDirectionRun::SharedPtr& second)
{
    return (first->visualOrder < second->visualOrder);
};

inline bool logical_order(const TextDirectionRun::SharedPtr& first, const TextDirectionRun::SharedPtr& second)
{
    return (first->start < second->start);
};

void TextDirectionRuns::toVisualOrder()
{
    std::sort(Base::get().begin(), Base::get().end(), visual_order);
}

void TextDirectionRuns::toLogicalOrder()
{
    std::sort(Base::get().begin(), Base::get().end(), logical_order);
}

inline TextDirectionRun::Direction getDirectionFromICU(UBiDiDirection dir)
{
    return dir == UBIDI_RTL ? TextDirectionRun::DIRECTION_RTL : TextDirectionRun::DIRECTION_LTR;
}

void TextDirectionItemizer::processDirection(TextDirectionRuns& textRuns, UnicodeString& text, Style::SharedPtr style)
{
    UBiDi *bi = ubidi_open();
    UErrorCode bidi_error = U_ZERO_ERROR;
    UBiDiLevel baseDir;

    baseDir = style->getDirection();

    ubidi_setPara(bi, text.getBuffer(), text.length(), baseDir, NULL, &bidi_error);
    if (U_FAILURE(bidi_error)) {
        Log::error(TRACE_GIBBON_TEXT) << "Error " << bidi_error << "(" << u_errorName(bidi_error) << ") setting up ubidi";
        return;
    }

    int32_t count = ubidi_countRuns(bi, &bidi_error);
    int32_t start, length;
    UBiDiDirection dir;

    for (int i = 0; i < count; ++i) {
        dir = ubidi_getVisualRun(bi, i, &start, &length);
        textRuns.addRun(start, start + length, 0, getDirectionFromICU(dir));
    }

    ubidi_close(bi);
}
