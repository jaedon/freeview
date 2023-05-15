/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextLayout.h"
#include <Debug.h>
#include <Style.h>
#include <nrdbase/utf8.h>

using namespace netflix::gibbon;
using namespace netflix;

void TextLayoutRun::clear()
{
    bounds.clear();
    text.clear();
    style.reset();
    charIndex = 0;
    endCharIndex = 0;
    type = TYPE_TEXT;
}

Rect TextLayoutRun::getCharRect(const int charIndex, int &actualCharIndex, const Coordinate defaultWidth) const
{
    std::string::const_iterator it = text.begin();
    std::string::const_iterator end = text.end();

    int ci = charIndex < 0 ? 0 : charIndex;

    int cnt = 0;
    while(it != end && cnt < ci) {
        utf8::unchecked::next(it);
        ++cnt;
    }

    int advance;
    Rect r = style->getFont()->getBounds(std::string(text.begin(), it).c_str(), style, &advance);

    Coordinate width = CoordinateMax;

    if(defaultWidth == CoordinateMax && it != text.end()) {
        unsigned char utf[5] = {0, 0, 0, 0, 0};
        uint32_t c = utf8::unchecked::next(it);
        utf8::unchecked::append(c, utf);
        width = style->getFont()->getBounds(reinterpret_cast<const char*>(utf), style).width;

        if(width < 0 || width == CoordinateMax)
            width = style->getTextCursor().width;
    } else {
        width = defaultWidth;
    }

    if(width == CoordinateMax)
        width = 2;

    r.x = bounds.x + advance;
    r.y = bounds.y;
    r.height = bounds.height;
    r.width = width;

    actualCharIndex = cnt + this->charIndex;

    if(style->getEdgeEffect().type == EdgeEffect::EDGE_EFFECT_OUTLINE) {
        r.x += style->getEdgeEffect().width;
    }

    return r;
}

int TextLayoutRun::getCharIndex(const Point &point) const
{
    std::string::const_iterator it = text.begin();
    std::string::const_iterator end = text.end();

    Coordinate currentWidth = bounds.x;
    int cnt = 0;

    while(it != end) {
        unsigned char utf[5] = {0, 0, 0, 0, 0};
        uint32_t c = utf8::unchecked::next(it);
        utf8::unchecked::append(c, utf);

        currentWidth += style->getFont()->getBounds(reinterpret_cast<const char*>(utf), style).width;
        if(point.x <= currentWidth)
            break;
        ++cnt;
    }

    return cnt + this->charIndex;
}

void TextLayoutLine::clear()
{
    bounds.clear();
    runs.clear();
    firstOrigin.x = firstOrigin.y = 0;
    maxAscent = 0;
    fixedHeight = CoordinateMax;
    charIndex = 0;
}

void TextLayoutLine::addRun(const TextLayoutRun &run)
{
    runs.push_back(run);
    bounds.unite(run.bounds);
}

const Point TextLayoutLine::end()
{
    return Point(bounds.x + bounds.width, bounds.y);
}

void TextLayoutLine::alignRuns()
{
    bounds.clear();

    std::vector<TextLayoutRun>::iterator it = runs.begin();
    std::vector<TextLayoutRun>::iterator end = runs.end();

    while(it != end) {
        // Align all runs by baseline. Use the base ascent if we have a top offset (meaning it's a super/subscript)

        Coordinate vertical = 0;
        if(it->style->getFont()->getAscent(it->style) < maxAscent) {
            /*if(it->style->getTopOffset()) {
                vertical = maxAscent - it->style->getBaseAscent();
            } else {*/
                vertical = (maxAscent - it->style->getFont()->getAscent(it->style));
            //}
            it->bounds.y += vertical;
        }

        bounds.unite(it->bounds);
        ++it;
    }
}

void TextLayoutLine::calculateBounds()
{
    bounds.clear();
    maxAscent = 0;

    std::vector<TextLayoutRun>::iterator it = runs.begin();
    std::vector<TextLayoutRun>::iterator end = runs.end();

    while(it != end) {
        // Keep track of the biggest ascent, in case we have different font sizes in the same line.
        if(it->style->getFont()->getAscent(it->style) > maxAscent)
            maxAscent = it->style->getFont()->getAscent(it->style);

        if(fixedHeight != CoordinateMax)
            it->bounds.height = fixedHeight;

        bounds.unite(it->bounds);
        ++it;
    }
}

void TextLayoutLine::updateRunsPosition(Coordinate deltaX, Coordinate deltaY)
{
    std::vector<TextLayoutRun>::iterator it = runs.begin();
    std::vector<TextLayoutRun>::iterator end = runs.end();

    while(it != end) {
        it->bounds.x += deltaX;
        it->bounds.y += deltaY;
        ++it;
    }

    bounds.x += deltaX;
    bounds.y += deltaY;
}

void TextLayout::calculateBounds()
{
    bounds.clear();

    std::vector<TextLayoutLine>::const_iterator it = lines.begin();
    std::vector<TextLayoutLine>::const_iterator end = lines.end();

    while(it != end) {
        bounds.unite(it->bounds);
        ++it;
    }
}

bool TextLayout::realign(const Size &size)
{
    const uint8_t align = style->getAlign();
    Coordinate moveX = 0, moveY = 0;
    if(size.height != CoordinateMax) {
        if(bounds.height < size.height) {
            Coordinate newY = 0;
            if(align & Style::ALIGN_CENTER_VERTICAL)
                newY = static_cast<int>((size.height - bounds.height ) / 2);
            else if(align & Style::ALIGN_BOTTOM)
                newY = size.height - bounds.height;
            desiredBounds.height = size.height;
            if(newY != offset.y) {
                if(offset.y != CoordinateMax)
                    newY -= offset.y;
                moveY = newY;
            }
        } else if(offset.y != CoordinateMax) {
            moveY = -offset.y;
        }
    }
    if(size.width != CoordinateMax) {
        if(bounds.width < size.width) {
            Coordinate newX = 0;
            if(align & Style::ALIGN_RIGHT)
                newX = size.width - bounds.width;
            else if(align & Style::ALIGN_CENTER_HORIZONTAL) {
                newX = static_cast<int>((size.width - bounds.width ) / 2);
            }
            desiredBounds.width = size.width;
            if(newX != offset.x) {
                if(offset.x != CoordinateMax)
                    newX -= offset.x;
                moveX = newX;
            }
        } else if(offset.x != CoordinateMax) {
            moveX = -offset.x;
        }
    }
    if(moveX || moveY) {
        move(moveX, moveY);

        const Shadow s = style->getShadow();
        const EdgeEffect e = style->getEdgeEffect();

        if(!s.empty()) {
            bounds.width += abs(s.offsetX);
            bounds.height += abs(s.offsetY);

            if(s.offsetX < 0)
                bounds.x += s.offsetX;

            if(s.offsetY < 0)
                bounds.y += s.offsetY;
        }

        // Move the origin a bit, and adjust the rectangle dimensions depending on the width of the edgeEffect
        if(!e.empty()) {
            if(e.type != EdgeEffect::EDGE_EFFECT_OUTLINE) {
                int width = e.width;
                if(e.type == EdgeEffect::EDGE_EFFECT_DEPRESSED || e.type == EdgeEffect::EDGE_EFFECT_RAISED)
                    width = 1;

                bounds.x -= width;
                bounds.y -= width;
                bounds.width += (2 * width);
                bounds.height += (2 * width);
            }
        }
        return true;
    }
    return false;
}

void TextLayout::addLine(const TextLayoutLine &line)
{
    lines.push_back(line);
    bounds.unite(line.bounds);
}

void TextLayout::move(Coordinate deltaX, Coordinate deltaY)
{
    if(deltaX) {
        if(offset.x == CoordinateMax)
            offset.x = deltaX;
        else
            offset.x += deltaX;
    }
    if(deltaY) {
        if(offset.y == CoordinateMax)
            offset.y = deltaY;
        else
            offset.y += deltaY;
    }

    std::vector<TextLayoutLine>::iterator it = lines.begin();
    std::vector<TextLayoutLine>::iterator end = lines.end();

    while(it != end) {
        it->updateRunsPosition(deltaX, deltaY);
        ++it;
        if(Debug::DebugTextLayout) {
            TDEBUG(*this << std::endl);
        }
    }

    calculateBounds();
}
