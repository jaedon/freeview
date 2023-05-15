/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextAttributesItemizer_H_
#define TextAttributesItemizer_H_

#include "TextItemizerCommon.h"
#include <vector>
#include <stdint.h>
#include <Style.h>

namespace netflix {
namespace gibbon {

class Style;

class TextAttributeRun : public BaseTextRun {
public:
    typedef shared_ptr<TextAttributeRun> SharedPtr;
    inline TextAttributeRun(uint32_t _start, uint32_t _end, const Style::SharedPtr &_style, int _visualOrder, int _sequence) :
        BaseTextRun(_start, _end), style(_style), visualOrder(_visualOrder), sequence(_sequence) {}

    Style::SharedPtr style;
    int visualOrder;
    int sequence;

    Variant toVariant() const;
};

class TextAttributeRuns : public BaseTextRuns<TextAttributeRun::SharedPtr> {
public:
    typedef std::vector<TextAttributeRun::SharedPtr> TextAttributeRunsList;
    typedef BaseTextRuns<TextAttributeRun::SharedPtr> Base;
    void addAttribute(uint32_t start, uint32_t end, const Style::SharedPtr &style, int visualOrder, int sequence);
    void addAttribute(uint32_t start, uint32_t end, const Style::SharedPtr &style);
    void swap(TextAttributeRuns &target);

    inline TextAttributeRuns() : Base(), hasBottomLayer(false) {}
#if CAPTURE_COPY_CONSTRUCTORS
    inline TextAttributeRuns(const TextAttributeRuns&) : Base(), hasBottomLayer(false) { assert(false); }
#endif

    bool hasBottomLayer;
};

struct TextItemizerData;
//-----------------------------------------------------------------------------

class TextAttributesItemizer {
public:
    static void processAttributes(TextAttributeRuns &attr, icu::UnicodeString &text, const char *utf8text, const Style::SharedPtr &baseStyle);
};

//-----------------------------------------------------------------------------

inline Variant TextAttributeRun::toVariant() const
{
    Variant data;
    data["style"] = style->toVariant();
    data["visualOrder"] = visualOrder;
    data["sequence"] = sequence;
    return data;
}


inline std::ostream &operator<< (std::ostream &out, const TextAttributeRun& attr)
{
    out << BASE_ITEMIZER_STREAM(attr) << ": [" << std::setw(3) << std::setfill('0') << attr.visualOrder << "-" << attr.sequence << "]";

    Font::SharedPtr font = attr.style->getFont();
    out << " " << font->getFamily();
    if (font->getWeight() == Font::WEIGHT_BOLD)
        out << " Bold";
    if (font->getStyle() == Font::STYLE_ITALIC)
        out << " Italic";
    out << " " << font->getPixelSize();
    out << " ";

    if (attr.style->getDirection() == 0)
        out << "[LTR]";
    else
        out << "[RTL]";
    out << " " << attr.style->getScript();
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextAttributeRun *attr)
{
    out << (*attr);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextAttributeRuns &runs)
{
    TextAttributeRuns::TextAttributeRunsList::const_iterator it = runs.get().begin();
    TextAttributeRuns::TextAttributeRunsList::const_iterator end = runs.get().end();

    out << "Attributes" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << it->get() << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

}}

#endif /* TextAttributesItemizer_H_ */
