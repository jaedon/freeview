/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include <gibbon/config.h>

#include <Rect.h>
#include <Debug.h>
#include <Style.h>
#include <TextLayout.h>
#include <TextEngine.h>
#include <SurfaceCache.h>
#include <GraphicsEngine.h>
#include <IteratorsCache.h>
#include <TextTruncation.h>
#include <GibbonApplication.h>
#include <GibbonConfiguration.h>
#include <itemizer/TextItemizer.h>

#include <nrdbase/utf8.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/tr1.h>

#include <limits.h>
#include <string.h>
#include <strings.h>
#include <cstdio>

#include <algorithm>
#include <functional>
#include <cctype>
#include <ctype.h>
#include <locale>
#include <TextLocale.h>

#include <unicode/uchar.h>
#include <unicode/ubidi.h>
using namespace icu;

#include <FontManager.h>

using namespace netflix::gibbon;
using namespace netflix;

#ifdef BUILD_DEBUG
# define DEBUG_TEXT_LAYOUT_PARAMS         // Log the layout creation calls, with parameters
# define DEBUG_TEXT_LAYOUT                // Log the calculated layout
#endif

//#define DEBUG_TEXT_CHILD_LAYOUT_PARAMS   // [child layouts] log the layout creation calls
//#define DEBUG_TEXT_CHILD_LAYOUT          // [child layouts] log the calculated layout

#define RED Color(255, 0, 0)
#define GREEN Color(0, 255, 0)
#define BLUE Color(0, 0, 255)
#define YELLOW Color(255, 255, 0)
#define CYAN Color(0, 255, 255)
#define MAGENTA Color(255, 0, 255)

// Text layout!
// --------------------------------------------------------------------------------------------------------

void TextEngine::cleanup()
{
    IteratorsCache::clearIterators();
}

static inline int minExtentX(const TextItemizerData& data)
{
    // abomination
    int min = 0;
    std::vector<TextItem::SharedPtr>::const_iterator item = data.items.begin();
    const std::vector<TextItem::SharedPtr>::const_iterator end = data.items.end();
    while (item != end) {
        min = std::max(min, (*item)->attribute->style->getFont()->minExtentX());
        ++item;
    }
    return min;
}

void TextEngine::layoutText(tiLayout &layout, const TextItemizerData &data, const Rect& r, const int firstLine)
{
    layout.clear();
    if(!data.style->getBackgroundColor().empty())
        layout.hasBottomLayer = true;

    Rect rect(r);

    const Padding& p = data.style->getPadding();
    // Adjust bounds with padding
    int hpad = -1, vpad = -1;
    // int hpad = 0, vpad = 0;

    if (p.horizontal >= 0) {
        hpad = p.horizontal;
    } else {
        const Shadow& shadow = data.style->getShadow();
        const EdgeEffect& edgeEffect = data.style->getEdgeEffect();

        if(Debug::TestTextCalculatePadding)
            hpad = minExtentX(data);
        else
            hpad = data.style->getFont()->minExtentX();

        if (!shadow.empty()) {
            hpad = std::max(hpad, abs(shadow.offsetX));
        }

        if (!edgeEffect.empty()) {
            hpad = std::max(hpad, edgeEffect.width);
        }
    }

    if (p.vertical >= 0) {
        vpad = p.vertical;
    } else {
        const Shadow& shadow = data.style->getShadow();
        const EdgeEffect& edgeEffect = data.style->getEdgeEffect();

        vpad = 0;

        if (!shadow.empty()) {
            vpad = std::max(vpad, abs(shadow.offsetY));
        }
        if (!edgeEffect.empty()) {
            vpad = std::max(vpad, edgeEffect.width);
        }
    }

    if (rect.width > 0 && hpad > rect.width / 2)
        hpad = rect.width / 2;
    if (rect.height > 0 && vpad > rect.height / 2)
        vpad = rect.height / 2;

//====================================================================================================
// Calculate the layout

    layout.setFirstLine(firstLine);

    if (hpad > 0 || vpad > 0) {
        assert(hpad >= 0 && vpad >= 0);

        if (rect.width != CoordinateMax) {
            rect.width = std::max<Coordinate>(rect.width - (hpad * 2), 0);
        }
        // rect.x += hpad;

        if(rect.height != CoordinateMax) {
            rect.height = std::max<Coordinate>(rect.height - (vpad * 2), 0);
        }
        // rect.y += vpad;

        layout.hpad = hpad;
        layout.vpad = vpad;
    }

    // TIDEBUG("laying out in " << rect);

    TextItemizer::layout(data, layout, rect);
    if ((!data.style->getTextTruncation().empty()) || (data.style->getMaxLines() > 0)) {
        if (data.style->getWrap()) {
            TextItemizer::truncate(data, layout);
        } else {
            TextItemizer::truncateLine(data, layout);
        }
    }
//====================================================================================================

    if(layout.desiredBounds.width == CoordinateMax) {
        std::vector<tiLine>::iterator it = layout.lines.begin();
        std::vector<tiLine>::iterator end = layout.lines.end();
        while(it != end) {
            layout.alignLine(&(*it), true);
            ++it;
        }
    }

    Rect paddedBounds = layout.bounds;
    paddedBounds.width -= (layout.hpad * 2);
    paddedBounds.height -= (layout.vpad * 2);
    if (paddedBounds.height > layout.desiredBounds.height && layout.desiredBounds.height > 0)
        paddedBounds.height = layout.desiredBounds.height;

    // Vertical alignment. Can align vertically only if the container's height is known
    if(layout.desiredBounds.height != CoordinateMax && layout.bounds.height < layout.desiredBounds.height) {
        if(data.style->getAlign() & Style::ALIGN_CENTER_VERTICAL) {
            layout.move(0, static_cast<int>((layout.desiredBounds.height - paddedBounds.height) / 2 ));
        } else if(data.style->getAlign() & Style::ALIGN_BOTTOM) {
            layout.move(0, layout.desiredBounds.height - paddedBounds.height);
        }
    }

    // readd the padding
    if (hpad > 0 || vpad > 0) {
        layout.move(hpad, vpad);
    }

    if(layout.bounds.height > r.height)
        layout.bounds.height = r.height;

#ifdef DEBUG_TEXT_LAYOUT_PARAMS
    if(Debug::DebugTextLayoutParams)
        TDEBUG("Returning: " << layout.bounds << std::endl);
#endif
    // TIDEBUG("Returning: " << layout << std::endl);
// #ifdef DEBUG_TEXT_LAYOUT
//     if(Debug::DebugTextLayout)
//         TDEBUG(layout << std::endl);
// #endif
    //result.layout.offset = result.layout.bounds.origin();
}

// Drawing functions
// --------------------------------------------------------------------------------------------------------

static void drawTextBackgrounds(GraphicsEngine *engine, const Point &engineOffset, const tiLayout &layout, const Rect &dstRect)
{
    std::vector<tiLine>::const_iterator it = layout.lines.begin();
    std::vector<tiLine>::const_iterator end = layout.lines.end();

    Coordinate offsetY = 0;
    if(!layout.isTruncating() && layout.getFirstLine() > 0) {
        it += layout.getFirstLine();
        if(it >= end)
            it = end - 1;

        offsetY = it->bounds.y;
    }

    std::vector< std::pair<Rect,Color> > rectangles;

    while(it != end) { // Draw a line of text
        std::vector<tiRun>::const_iterator lit = it->runs.begin();
        std::vector<tiRun>::const_iterator lend = it->runs.end();

        Color previousBackground;
        Rect currentRect;

        while(lit != lend) { // Draw a single run
            // Background. Do this only if we are the bottom layer, as we need to render the background first
            // Inflate the rectangle by the edgeEffects width and unite the shadow rectangle to the background one.
            Style::SharedPtr style = lit->item->attribute->style;
            if(!style->getBackgroundColor().empty()) {
                // Make the background rectangle the same height as the line
                Rect bgRectangle = lit->logicalBounds;
                //TIDEBUG("background for: " << bgRectangle << " " << __LINE__);
                bgRectangle.y = it->logicalBounds.y - offsetY;
                bgRectangle.height = it->logicalBounds.height;
                bgRectangle += dstRect.origin();
                bgRectangle += engineOffset;

                // Ok, we have a rectangle... add it to our list of rectangles to draw
                // Unite consecutive rectangles with the same background
                if(previousBackground == style->getBackgroundColor() && currentRect.height == bgRectangle.height &&
                   currentRect.y == bgRectangle.y && currentRect.x + currentRect.width >= bgRectangle.x) {
                    currentRect.unite(bgRectangle);
                } else {
                    if(!currentRect.empty()) {
                        rectangles.push_back(std::make_pair(currentRect, previousBackground));
                    }

                    // See if the rectangle intersects the previous one horizontally and adjust it if that's the case
                    const Coordinate lastX = currentRect.x + currentRect.width;
                    if(currentRect.y == bgRectangle.y && lastX > bgRectangle.x)
                        bgRectangle.left(lastX);

                    currentRect = bgRectangle;
                    previousBackground = style->getBackgroundColor();
                }

            }
            ++lit;
        }

        if(!currentRect.empty()) {
            rectangles.push_back(std::make_pair(currentRect, previousBackground));
        }

        ++it;
    }

    // So many calculations, draw the rectangles already!
    for(std::vector< std::pair<Rect,Color> >::reverse_iterator rect = rectangles.rbegin(); rect != rectangles.rend(); ++rect)
        engine->fill(rect->first, rect->second);
}

static inline Coordinate baselineY(const Style::SharedPtr& style)
{
    Coordinate ascent = style->getFont()->getAscent(style);
    if(style->getEdgeEffect().type == EdgeEffect::EDGE_EFFECT_OUTLINE) {
        ascent -= style->getEdgeEffect().width;
    }
    return ascent;
}

// Low level function to draw text. Draws the full layout and allows callers to override the color (used to draw shadows and borders)
static void drawText(GraphicsEngine *engine, const Point &engineOffset, const tiLayout &layout, const Rect &dstRect, const Style::SharedPtr& /*style*/, const Color &forceColor = Color(0, 0, 0, 0))
{
    // drawhb
    //TIDEBUG("\nDRAW: +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    //TIDEBUG("draw!");
    //TIDEBUG("forceColor: " << forceColor);
    //TIDEBUG("lines: " << layout.lines.size());
    std::vector<tiLine>::const_iterator it = layout.lines.begin();
    std::vector<tiLine>::const_iterator end = layout.lines.end();

    Coordinate offsetY = 0;

    if(!layout.isTruncating() && layout.getFirstLine() > 0) {
        it += layout.getFirstLine();
        if(it >= end)
            it = end - 1;

        offsetY = it->logicalBounds.y - layout.bounds.y;
    }

    Rect firstOrigin;
    for( ; it != end; ++it) { // Draw a line of text
        // Stop rendering text if we are outside the surface's boundaries
        if((it->logicalBounds.y - offsetY >= dstRect.height))
            return;
        if(it->logicalBounds.empty())
            continue;

        //TIDEBUG(*it);

        std::vector<tiRun>::const_iterator lit = it->runs.begin();
        std::vector<tiRun>::const_iterator lend = it->runs.end();
        for( ; lit != lend; ++lit) { // Draw a single run
            if(lit->logicalBounds.empty())
                continue;

            Style::SharedPtr style = lit->item->attribute->style;

            Rect origin = lit->logicalBounds;
            const Coordinate originalHeight = lit->originalHeight;
            origin += dstRect.origin();
            origin.y -= offsetY;
            //origin.x += lit->inkBounds.x - layout.bounds.x;

            //origin.intersect(dstRect);
            if(origin.x + origin.width > dstRect.x + dstRect.width)
                origin.width -= (origin.x + origin.width) - (dstRect.x + dstRect.width);
            if(origin.y + origin.height > dstRect.y + dstRect.height)
                origin.height -= (origin.y + origin.height) - (dstRect.y + dstRect.height);

            // Calculate the underline and strikethrough rectangles
            Rect underline;
            if(style->getUnderline()) {
                underline.x = origin.x;
                underline.width = origin.width;
                // Set the underline thickness to be 1/14 of the font's height, defaulting to 1. 1/14 is a measurement both Adobe and Pango use.
                originalHeight < 14 ? underline.height = 1 : underline.height = static_cast<int>(originalHeight / 14);
                underline.y = origin.y + style->getFont()->getAscent(style) + underline.height;
            }

            Rect strike;
            if(style->getStrikethrough()) {
                strike.x = origin.x;
                strike.width = origin.width;
                // The strikethrough thickness is the same as the underline thickness
                originalHeight < 14 ? strike.height = 1 : strike.height = static_cast<int>(originalHeight / 14);
                strike.y = origin.y + style->getFont()->getAscent(style) - static_cast<int>(originalHeight / 4); // Position the strikethrough 1/4 of the distance above the baseline. Again, same value Pango/Adobe use.
            }

            Style::SharedPtr drawStyle = style;
            if(!forceColor.empty()) {
                drawStyle = style->clone();
                drawStyle->setColor(forceColor);
                EdgeEffect edge = drawStyle->getEdgeEffect();
                edge.color1 = forceColor;
                edge.color2 = forceColor;
                drawStyle->setEdgeEffect(edge);
            }

            // Draw text
            if(origin.width > 0 && origin.height > 0) {
                if(firstOrigin.empty() || origin.x < firstOrigin.x)
                    firstOrigin = origin;
                //engine->drawText(lit->text.c_str(), origin+engineOffset, firstOrigin+engineOffset, drawStyle);
                //drawText(engine, lit->text.c_str(), origin + engineOffset, firstOrigin + engineOffset, drawStyle);
                //TIDEBUG(" >>> draw1: " << origin + engineOffset);
                //TIDEBUG(" >>> draw2: " << firstOrigin + engineOffset);
                //TIDEBUG(" run bounds: " << lit->logicalBounds + engineOffset);
                //engine->drawText(lit->item->getGlyphString().buffer(), lit->start, lit->end + 1, firstOrigin + engineOffset, lit->inkBounds, drawStyle);
                engine->drawText(lit->item->getGlyphString().buffer(), lit->start, lit->end + 1, origin + engineOffset, firstOrigin + engineOffset, drawStyle);
            }

            if(Debug::DrawTextRunRectangles && !forceColor)
                engine->drawRect(origin+engineOffset, RED);

            if(Debug::DrawTextBaselines && !forceColor)
                engine->drawRect(Rect(origin.x + engineOffset.x,
                                      origin.y + engineOffset.y + baselineY(drawStyle),
                                      origin.width, 1),
                                 GREEN);


            // Draw underline
            if(!underline.empty()) {
                const EdgeEffect edge = drawStyle->getEdgeEffect();
                if(edge.type == EdgeEffect::EDGE_EFFECT_OUTLINE && !edge.empty()) {
                    const Color outlineColor = edge.color1;
                    const int outlineWidth = edge.width;

                    underline.x += outlineWidth;
                    underline.width -= 2 * outlineWidth;

                    Rect underlineOutline(underline);
                    underlineOutline.y -= outlineWidth;
                    underlineOutline.height = outlineWidth;
                    engine->fill(underlineOutline+engineOffset, outlineColor);

                    underlineOutline.y = underline.y + underline.height;
                    engine->fill(underlineOutline+engineOffset, outlineColor);

                    underlineOutline.x -= outlineWidth;
                    underlineOutline.y = underline.y - outlineWidth;
                    underlineOutline.width = outlineWidth;
                    underlineOutline.height = underline.height + (outlineWidth * 2);
                    engine->fill(underlineOutline+engineOffset, outlineColor);

                    underlineOutline.x = underline.x + underline.width;
                    engine->fill(underlineOutline+engineOffset, outlineColor);
                }
                engine->fill(underline+engineOffset, drawStyle->getColor());
            }

            // Draw strikethrough
            if(!strike.empty())
                engine->fill(strike+engineOffset, drawStyle->getColor());
        }

        if(Debug::DrawTextRectangles && !forceColor)
            engine->drawRect(Rect(it->logicalBounds.x + dstRect.x + engineOffset.x,
                                  it->logicalBounds.y + engineOffset.y - offsetY + dstRect.y, it->logicalBounds.size()), MAGENTA);
    }
    //TIDEBUG("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}



void TextEngine::drawLayout(GraphicsEngine *e, const tiLayout &layout, const Rect &dstRect)
{
    if(!layout.style || layout.bounds.empty()) //Problems parsing rich text might have made this to be null
        return;

    const Rect oldClip = e->clip();
    Rect newClip = dstRect.intersected(layout.bounds);
    if(!oldClip.empty()) {
        if(oldClip.overlaps(newClip))
            newClip.intersect(oldClip);
        else
            newClip = oldClip;
    }
    e->setClip(newClip);

    Point engineOffset, destinationOffset;

    GraphicsEngine tempEngine;
    GraphicsEngine *engine = e;
    Surface::SharedPtr renderSurface;
    if(GibbonConfiguration::cacheTextRender() && e->isDisplayList() && !GibbonConfiguration::screenRenderDisplayList()) {
        const unsigned flags = Surface::Flags_Accelerated|Surface::Flags_Alpha|Surface::Flags_RenderTarget;
        Size surfaceSize(std::min(dstRect.width, layout.bounds.width), std::min(dstRect.height, layout.bounds.height));
        if (layout.bounds.x + layout.hpad < 0)
            surfaceSize.width += -(layout.bounds.x + layout.hpad);
        if (layout.bounds.y + layout.vpad < 0)
            surfaceSize.height += -(layout.bounds.y + layout.vpad);

        renderSurface = GibbonApplication::instance()->getSurfaceCache()->requestSurface(surfaceSize.width, surfaceSize.height, flags);
        if(renderSurface) {
            if (layout.bounds.x < 0) {
                destinationOffset.x = -layout.bounds.x;
            } else {
                engineOffset.x -= layout.bounds.x;
            }
            if (layout.bounds.y < 0) {
                destinationOffset.y = -layout.bounds.y;
            } else {
                engineOffset.y -= layout.bounds.y;
            }
            tempEngine.begin(renderSurface);
            engine = &tempEngine;
            engine->clear(renderSurface->getRect());
            //renderSurface->setDescription("text: " + layout.text);
            renderSurface->setDescription("text: <itemized text>"); //TODO: SAM?
        }
    }

    // Draw the text backgrounds, but only if the layout is using them
    if(layout.hasBottomLayer)
        drawTextBackgrounds(e, Point(0, 0), layout, dstRect);

    // Drop shadow
    Shadow shadow = layout.style->getShadow();
    EdgeEffect edge = layout.style->getEdgeEffect();

    // Draw shadow
    if(!shadow.empty()) {
        Rect shadowRect = dstRect;

        int shadowX = shadow.offsetX;
        int shadowY = shadow.offsetY;

        // If we have a text border we need to begin counting the shadow offset outside of it
        if(edge.type != EdgeEffect::EDGE_EFFECT_OUTLINE && !edge.empty()) {
            shadowX > 0 ? shadowX += edge.width : shadowX -= edge.width;
            shadowY > 0 ? shadowY += edge.width : shadowY -= edge.width;
        }

        shadowRect.x += shadowX;
        shadowRect.y += shadowY;

        // for negative shadow offsets, increase the shadow rect size so that we
        // get shadow drawn in areas that would otherwise potentially be clipped
        if(shadowX < 0)
            shadowRect.width -= shadowX;
        if(shadowY < 0)
            shadowRect.height -= shadowY;

        engine->startText();
        drawText(engine, engineOffset + shadowRect.origin(), layout, dstRect, layout.style, shadow.color);
        engine->endText();
    }

    // Draw edge effects
    if(!edge.empty()) {
        Rect borderRect = dstRect;

        if(edge.type == EdgeEffect::EDGE_EFFECT_RAISED || edge.type == EdgeEffect::EDGE_EFFECT_DEPRESSED) {
            Color c1;
            Color c2;

            if(edge.type == EdgeEffect::EDGE_EFFECT_RAISED) {
                c1 = edge.color1;
                c2 = edge.color2;
            } else {
                c1 = edge.color2;
                c2 = edge.color1;
            }

            borderRect.x -= 1;
            borderRect.y -= 1;
            // make sure we don't clip the border if the regular glyphs are clipped
            borderRect.width += 1;
            borderRect.height += 1;
            //drawText(engine, engineOffset, layout, borderRect, c1);
            engine->startText();
            drawText(engine, engineOffset, layout, borderRect, layout.style, c1);
            engine->endText();

            borderRect.x = dstRect.x + 1;
            borderRect.y = dstRect.y + 1;
            // adjust the clip buffer back
            borderRect.width -= 1;
            borderRect.height -= 1;
            engine->startText();
            drawText(engine, engineOffset, layout, borderRect, layout.style, c2);
            engine->endText();
        }
    }

    // Draw text
    engine->startText();
    drawText(engine, engineOffset, layout, dstRect, layout.style);
    engine->endText();

    if(renderSurface) {
        engine->end();
        engine = 0;
        const Rect surfaceRect(renderSurface->getRect());
        e->blit(renderSurface, surfaceRect, surfaceRect+layout.bounds.origin()+dstRect.origin()+destinationOffset);
    }
    e->setClip(oldClip);
}

// Cursor functions
// --------------------------------------------------------------------------------------------------------

static inline Rect cursorProcessRun(const tiRun& run, uint32_t charIndex, uint32_t &actualCharIndex)
{
    const GlyphString& string = run.item->getGlyphString();
    charIndex = string.getBufferIndexForStringIndex(charIndex, GlyphString::Character);
    if (charIndex > static_cast<uint32_t>(run.end))
        charIndex = run.end + 1;
    hb_glyph_info_t* info = string.getGlyphInfos();
    const Style::SharedPtr& style = run.item->attribute->style;
    if (charIndex == INT_MAX) {
        const unsigned int len = string.bufferLength();
        if (!len)
            actualCharIndex = INT_MAX;
        else
            actualCharIndex = info[len - 1].cluster + 1;
        Rect logical;
        logical.x = run.logicalBounds.right();
        logical.y = run.logicalBounds.y;
        logical.height = run.logicalBounds.height;
        Coordinate w = style->getTextCursor().width;
        if (w == -1) {
            w = 2;
        }
        logical.width = w;
        logical.x -= w - 1;
        return logical;
    } else {
        actualCharIndex = info[charIndex].cluster;
    }

    // calculate the bounding box of the char
    const GibbonFreetype::SharedPtr& freetype = style->getFont()->getFreetype();
    Rect logical = freetype->getCursorRect(string.buffer(), style, run.start, charIndex);
    logical.y = run.logicalBounds.y;
    logical.x += run.logicalBounds.x;
    Coordinate w = style->getTextCursor().width;
    if (w <= 0) {
        // character width
        const Rect& charLogical = freetype->getCursorRect(string.buffer(), style, charIndex, charIndex + 1);
        w = std::max<Coordinate>(charLogical.width, 2);
    } else {
        // grow to the left
        logical.x -= w - 1;
    }
    logical.width = w;
    return logical;
}

Rect TextEngine::getCursorRect(const tiLayout& layout, uint32_t charIndex, uint32_t &actualCharIndex)
{
    std::vector<tiLine>::const_iterator line = layout.lines.begin();

    const tiRun* lastRun = 0;
    const tiRun* candidate = 0;
    const std::vector<tiLine>::const_iterator lineEnd = layout.lines.end();
    while (line != lineEnd) {
        std::vector<tiRun>::const_iterator run = line->runs.begin();
        const std::vector<tiRun>::const_iterator runEnd = line->runs.end();
        while (run != runEnd) {
            if (run->end < run->start) {
                ++run;
                continue;
            }
            const GlyphString& string = run->item->getGlyphString();
            const hb_glyph_info_t* info = string.getGlyphInfos();
            const uint32_t clusterStart = info[run->start].cluster;
            const uint32_t clusterEnd = info[run->end].cluster;

            if (charIndex >= clusterStart && charIndex <= clusterEnd) {
                return cursorProcessRun(*run, charIndex, actualCharIndex);
            }
            if (charIndex >= clusterStart || (!candidate && charIndex < clusterStart && charIndex < clusterEnd))
                candidate = &*run;
            lastRun = &*run;
            ++run;
        }
        ++line;
    }
    if (candidate) {
        return cursorProcessRun(*candidate, charIndex, actualCharIndex);
    } else if (lastRun) {
        const GlyphString& string = lastRun->item->getGlyphString();
        const hb_glyph_info_t* info = string.getGlyphInfos();
        return cursorProcessRun(*lastRun, info[lastRun->end].cluster + 1, actualCharIndex);
    }

    return Rect();
}

int TextEngine::getCharIndex(const tiLayout &/*layout*/, const Point &/*point*/)
{
    return 0;
/*    //!!!
    bool match = false;
    unsigned i = 0;

    while(i < layout.lines.size()) {
        if(layout.lines[i].bounds.contains(point)) {
            match = true;
            break;
        }
        ++i;
    }
    if(!match)
        return -1;

    const TextLayoutLine* line = &layout.lines[i];
    i = 0;
    match = false;

    while(i < line->runs.size()) {
        if(line->runs[i].bounds.contains(point)) {
            match = true;
            break;
        }
        ++i;
    }
    if(!match)
        return -1;

    const TextLayoutRun* run = &line->runs[i];
    return run->getCharIndex(point);
*/
}

int TextEngine::moveCursor(const tiLayout &/*layout*/, int /*current*/, int /*lines*/, int /*columns*/)
{
    return 0; //!!!
/*
    int i = 0, totalLines = 0;

    if(layout.lines.size() == 0)
        return -1;

    int currentPosition = current + columns;

    unsigned firstLine = layout.getFirstLine();
    const int ci = currentPosition < layout.lines[firstLine].charIndex ? layout.lines[firstLine].charIndex : currentPosition;

    int actual = 0;
    Rect r = getCursorRect(layout, ci, actual);

    TDEBUG("Current: " << current << ", ci: " << ci << " " << r);

    totalLines = layout.lines.size();

    while(i < totalLines) {
        if(layout.lines[i].charIndex <= ci && layout.lines[i].endCharIndex >= ci)
            break;
        ++i;
    }
    if(i == totalLines)
        i = totalLines - 1;

    printf("current line: %d\n", i);

    i += lines;

    if(i < 0)
        i = 0;

    if(i >= totalLines)
        i = totalLines - 1;

    printf("new line: %d\n", i);

    const Point p(r.x + 1, layout.lines[i].bounds.y + (layout.lines[i].bounds.height) / 2);
    TDEBUG("Point: " << p);

    int n = getCharIndex(layout, p);
    //int n = getCharIndex(layout, Point(r.x, layout.lines[i].bounds.y + 1));
    printf("movecursor to %d\n", n);
    return n;
*/
}
