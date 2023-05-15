/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonConfiguration.h"
#include "TextFontItemizer.h"
#include "FontManager.h"

#include <unicode/schriter.h>

using namespace icu;

namespace netflix {
namespace gibbon {

static void addNewRun(TextAttributeRuns &textRuns, uint32_t start, uint32_t end, const Style::SharedPtr &baseStyle, const Font::SharedPtr &font)
{
    if(start == end)
        return;

/*
  TextAttributeRuns::TextAttributeRunsList& tr = textRuns.get();
    if(!tr.empty()) {
        Font::SharedPtr previousFont = tr.back()->style->getFont();
        if(previousFont->describe() == font->describe()) {
            tr.back()->end = end;
            return;
        }
    }
*/

    Style::SharedPtr newStyle = baseStyle->clone();
    newStyle->setFontFamily(font->getFamily());
    newStyle->setFontWeight(font->getWeight());
    newStyle->setFontStyle(font->getStyle());
    textRuns.addAttribute(start, end, newStyle);
}

void TextFontItemizer::processFonts(UnicodeString &text, TextAttributeRuns &textRuns)
{
    TextAttributeRuns copy;

    TextAttributeRuns::TextAttributeRunsList::const_iterator it = textRuns.get().begin();
    TextAttributeRuns::TextAttributeRunsList::const_iterator end = textRuns.get().end();

    uint32_t cp = 0;

    for(; it != end; it++) {
        TextAttributeRun &attr = **it;
        icu::StringCharacterIterator itr = icu::StringCharacterIterator(text, attr.start, attr.end, attr.start);

        Font::SharedPtr baseFont = attr.style->getFont();
        Font::SharedPtr currentFont = baseFont;
        Font::SharedPtr lastFont;
        unsigned long rangeStart = 0;
        unsigned long rangeEnd = UINT_MAX;

        uint32_t start = attr.start;
        uint32_t prev = start;
        int length = 0;

        while (itr.hasNext()) {
            prev = itr.getIndex();
            cp = itr.next32PostInc();

            // Keep using the same font if we have a space, a non-break space, a new line or a paragraph separator
            if((cp == 32 || cp == 160) || cp == 10 || cp == 0x2029 || ((cp >= rangeStart && cp <= rangeEnd) && currentFont->hasCodepoint(cp))) {
                ++length;
            } else {
                if(length > 0) {
                    if(currentFont != lastFont) {
                        lastFont = currentFont;
                        addNewRun(copy, start, start + length, attr.style, currentFont);
                    }
                }
                length = 1;
                start = prev;
                if(currentFont != baseFont && baseFont->hasCodepoint(cp)) {
                    currentFont = baseFont;
                    rangeStart = 0;
                    rangeEnd = UINT_MAX;
                } else {
                    std::string locale = attr.style->getLocale();
                    currentFont = FontManager::getFontForCodePoint(cp, baseFont->getFamily(), baseFont->getPixelSize(), baseFont->getWeight(), baseFont->getStyle(),
                                                                   locale, &rangeStart, &rangeEnd);
                    if(!currentFont) {
                        rangeStart = 0;
                        rangeEnd = UINT_MAX;
                        currentFont = baseFont;
                    }

                }
            }
        }

        if(currentFont != lastFont)
            addNewRun(copy, start, attr.end, attr.style, currentFont);
    }

    // Replace the original attribute runs with the new calculated ones
    if (copy.size() != 0)
        textRuns.swap(copy);
}

} /* namespace gibbon */
} /* namespace netflix */
