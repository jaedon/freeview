/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTCURSOR_H_
#define TEXTCURSOR_H_

#include "Color.h"
#include <nrdbase/Variant.h>

#define CURSOR_MAX INT_MAX - 1

namespace netflix {
namespace gibbon {

class TextCursor
{
public:
    enum CursorStyle {
        CURSOR_NONE,
        CURSOR_LINE
    };

    inline TextCursor() : visible(false), interval(500), color(0, 0, 0, 255), style(CURSOR_LINE), width(1) { }
    inline ~TextCursor() {}

    inline bool operator==(const TextCursor &c) const
    {
        return visible == c.visible && interval == c.interval && color == c.color && style == c.style && width == c.width;
    }

    inline bool operator!=(const TextCursor &c) const
    {
        return !(*this == c);
    }

    inline uint8_t diff(const TextCursor &c) const {
        if(c.visible != visible || c.interval != interval || c.style != style || c.width != width)
            return TextEngine::DIFF_LAYOUT|TextEngine::DIFF_RENDER;
        if(c.color != color)
            return TextEngine::DIFF_RENDER;
        return TextEngine::DIFF_NONE;
    }

    inline bool empty() const
    {
        return style == CURSOR_NONE || visible == false;
    }

    inline void clear()
    {
        style = CURSOR_LINE;
        visible = false;
        color = Color(0, 0, 0, 255);
        interval = 500;
        width = 1;
    }

    inline bool operator!() const
    {
        return empty();
    }

    bool visible;
    int interval;
    Color color;
    CursorStyle style;
    int width;

    inline std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Cursor (";
            if (visible) {
                out << "visible";
            } else {
                out << "not visible";
            }

            out << ", width: " << width;
            out << ", interval: " << interval;

            out << ", style: ";
            switch (style) {
            case CURSOR_NONE:
                out << "none";
                break;
            case CURSOR_LINE:
                out << "line";
                break;
            default:
                break;
            }

            out << " " << color;
        } else {
            out << (int)visible << ":" << width << ":" << interval << ":" << style << ":" << color;
        }
        return out.str();
    }

    Variant toVariant() const
    {
        Variant data;
        data["visible"] = visible;
        data["interval"] = interval;
        data["color"] = color.toVariant();
        data["style"] = style;
        data["width"] = width;
        return data;
    }
};

inline std::ostream &operator<< (std::ostream &out, const TextCursor &c)
{
    return out << c.toString();
}


}} // namespace netflix::gibbon


#endif /* TEXTCURSOR_H_ */
