/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextItemizer.h"
#include "GibbonConfiguration.h"
#include <TextClusterIterator.h>

using namespace netflix::gibbon;
using namespace icu;

#define LTR 0
#define RTL 1

bool compareClusters(const hb_glyph_info_t &left, const hb_glyph_info_t &right) {
    return left.cluster < right.cluster;
}

bool compareClustersRTL(const hb_glyph_info_t &left, const hb_glyph_info_t &right) {
    return left.cluster > right.cluster;
}

uint32_t GlyphString::getBufferIndexForStringIndex(uint32_t index, IndexMode mode) const {
    hb_glyph_info_t* info = getGlyphInfos();
    unsigned int length = bufferLength();

    hb_direction_t direction = hb_buffer_get_direction(mBuffer);

    if(length == 0) {
        return 0;
    } else if(index == INT_MAX) {
        return INT_MAX;
    } else if (index > info[length - 1].cluster) {
        return INT_MAX;
    }

    hb_glyph_info_t search;
    search.cluster = index;
    hb_glyph_info_t* item;

    if(direction == HB_DIRECTION_LTR) {
        if (mode == Boundary) {
            item = std::lower_bound(info, info + length, search, compareClusters);
            if(item == info)
                return INT_MAX;
            --item;
        } else {
            item = std::lower_bound(info, info + length, search, compareClusters);
            if (item == info + length)
                return INT_MAX;
        }
    } else {
        if (mode == Boundary) {
            item = std::upper_bound(info, info + length, search, compareClustersRTL);
            if(item == info)
                return INT_MAX;
        } else {
            item = std::upper_bound(info, info + length, search, compareClustersRTL);
            if (item == info)
                return INT_MAX;
            --item;
        }
    }


    return item - info;
}


// Assuming the buffer boundaries are correct, so we don't have to check.
uint32_t GlyphString::rtrim(uint32_t start, uint32_t end, Font::SharedPtr font)
{
    assert(start <= end);

    hb_glyph_info_t* info = getGlyphInfos();
    if(end == INT_MAX)
        end = bufferLength() - 1;

    while (font->isClassification(Font::CLASS_SPACE, info[end].codepoint)) {
        if (end == 0) {
            return INT_MAX; // The whole glyphString has been rtrimmed
        }
        --end;
        if(end < start)
            return start;
    }
    return end;
}

uint32_t GlyphString::ltrim(uint32_t start, uint32_t end, Font::SharedPtr font)
{
    assert(start <= end);

    hb_glyph_info_t* info = getGlyphInfos();
    if(end == INT_MAX)
        end = bufferLength() - 1;

    while (font->isClassification(Font::CLASS_SPACE, info[start].codepoint)) {
        ++start;
        if(start >= end)
            return INT_MAX; // The whole glyphString has been ltrimmed
    }
    return start;
}


static inline bool visual_order(const TextAttributeRun::SharedPtr& first, const TextAttributeRun::SharedPtr& second)
{
    if(first->visualOrder != second->visualOrder)
        return first->visualOrder < second->visualOrder;
    return (first->sequence > second->sequence);
};


static inline hb_buffer_t* createHarfbuzzBuffer(const Style::SharedPtr style, const uint16_t* text, int text_length, unsigned int item_offset, int item_length)
{
    hb_buffer_t* buf = hb_buffer_create();

    hb_buffer_add_utf16(buf, text, text_length, item_offset, item_length);
    hb_buffer_set_script(buf, hb_script_from_string(style->getScript().c_str(), 4));

    const std::string& locale = style->getLocale();
    hb_buffer_set_language(buf, hb_language_from_string(locale.c_str(), locale.length()));

    if(style->getDirection() == 0)
        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
    else
        hb_buffer_set_direction(buf, HB_DIRECTION_RTL);

    const Typography& typography = style->getTypography();
    const std::string feature_settings = typography.fontFeatures;
    const GibbonFreetype::SharedPtr ft = style->getFont()->getFreetype();
    ft->harfbuzz()->setKerning(typography.kerning);
    if(feature_settings.empty()) {
        hb_shape(ft->harfbuzzFont(), buf, 0, 0);
    } else {
        std::stringstream fs(feature_settings);
        std::string item;
        std::vector<hb_feature_t> features;
        while (std::getline(fs, item, ',')) {
            hb_feature_t feature;
            if(hb_feature_from_string(item.c_str(), item.size(), &feature)) {
                features.push_back(feature);
            }
        }
        hb_shape(ft->harfbuzzFont(), buf, &features[0], features.size());
    }

    return buf;
}

static inline hb_buffer_t* createHarfbuzzBuffer(const Style::SharedPtr style, const UnicodeString& text, unsigned int item_offset, int item_length)
{
    return createHarfbuzzBuffer(style, reinterpret_cast<const uint16_t*>(text.getBuffer()), text.length(), item_offset, item_length);
}

#if 0
static inline hb_buffer_t* createHarfbuzzBuffer(const Style::SharedPtr style, const std::string& utf8Text)
{
    UnicodeString text = UnicodeString::fromUTF8(utf8Text);
    return createHarfbuzzBuffer(style, text, 0, text.length());
}
#endif

TextItemizerData TextItemizer::itemize(const char* utf8Text, const Style::SharedPtr &style)
{
    TextItemizerData layout;
    layout.style = style;
#ifdef BUILD_DEBUG
    TextAttributeRuns& attr = layout.attributeRuns;
    TextDirectionRuns& dir = layout.directionRuns;
    TextScriptRuns& scripts = layout.scriptRuns;
    TextLocaleRuns& locales = layout.localeRuns;
    TextAttributeRuns& mergedAttributes = layout.finalRuns;
#else
    TextAttributeRuns attr;
    TextDirectionRuns dir;
    TextScriptRuns scripts;
    TextLocaleRuns locales;
    TextAttributeRuns mergedAttributes;
#endif
    // Get the text attribute runs, and convert the text to UTF-16

    TextAttributesItemizer::processAttributes(attr, layout.text, utf8Text, style);
    layout.hasBottomLayer = attr.hasBottomLayer;
    //TIDEBUG("Attributes:\n" << attr);

    // Subdivide the attribute runs in fonts
    TextFontItemizer::processFonts(layout.text, attr);
    //TIDEBUG("Fonts:\n" << attr);

    // If this is true it means that parsing rich text resulted in an empty string
    // like "<b></b>"
    if (attr.size() == 0)
        return layout;

    // Process the text with the BiDi algorithm to get the visual order
    TextDirectionItemizer::processDirection(dir, layout.text, style);
    //TIDEBUG("Direction:\n" << dir);
    if (dir.size() == 0)
        return layout;

    // Get the text's script type so we can pick the right shaper
    TextScriptItemizer::processScripts(scripts, layout.text);
    //TIDEBUG("Scripts:\n" << scripts);
    if (scripts.size() == 0)
        return layout;

    // Merge the different run collections
    bool allDone = false;
    uint32_t index = 0;

    bool attributesDone = false;
    uint32_t attributesPos = 0;
    TextAttributeRuns::TextAttributeRunsList::const_iterator attributesIt = attr.get().begin();

    bool scriptsDone = false;
    uint32_t scriptsPos = 0;
    TextScriptRuns::TextScriptRunsList::const_iterator scriptsIt = scripts.get().begin();

    bool directionDone = false;
    uint32_t directionPos = 0;
    dir.toLogicalOrder();
    TextDirectionRuns::TextDirectionRunsList::const_iterator directionIt = dir.get().begin();

    Style::SharedPtr currentStyle;
    int currentVisualOrder = 0;
    int currentSequence = 0;
    int currentDirection = 0;

    do {
        if(!attributesDone)
            attributesPos = attributesIt->get()->start;
        if(!scriptsDone)
            scriptsPos = scriptsIt->get()->start;
        if(!directionDone)
            directionPos = directionIt->get()->start;

        //TIDEBUG("[loop] index: " << index << " attr:" << attributesPos << " script: " << scriptsPos << " direction: " << directionPos);

        Style::SharedPtr style;

        if(currentStyle)
            style = currentStyle->clone();

        if(!attributesDone && attributesPos == index) {
            //TIDEBUG(" [apply attr] index: " << index << " attr: " << attributesIt->get()->style);

            int currentDir = -1;
            std::string currentScript = "";
            if(style) {
                currentDir = style->getDirection();
                currentScript = style->getScript();
            }
            style = attributesIt->get()->style->clone();
            if(currentDir != -1) {
                style->setDirection(currentDir);
                style->setScript(currentScript);
            }

            ++attributesIt;
            if(attributesIt == attr.get().end()) {
                attributesDone = true;
                attributesPos = layout.text.length();
            } else {
                attributesPos = attributesIt->get()->start;
            }
        }

        if(!scriptsDone && scriptsPos == index) {
            //TIDEBUG(" [apply script] index: " << index << " script: " << scriptsIt->get()->script);
            style->setScript(scriptsIt->get()->script);

            ++scriptsIt;
            if(scriptsIt == scripts.get().end()) {
                scriptsDone = true;
                scriptsPos = layout.text.length();
            } else {
                scriptsPos = scriptsIt->get()->start;
            }
        }

        if(!directionDone && directionPos == index) {
            //TIDEBUG(" [apply direction] index: " << index << " direction: " << directionIt->get()->direction);
            currentDirection = directionIt->get()->direction;
            style->setDirection(currentDirection);
            currentVisualOrder = directionIt->get()->visualOrder;
            currentSequence = 0;

            ++directionIt;
            if(directionIt == dir.get().end()) {
                directionDone = true;
                directionPos = layout.text.length();
            } else {
                directionPos = directionIt->get()->start;
            }
        }

        //TIDEBUG("[ADD RUN] position: " << index << " Style: " << style);
        uint32_t start = index;
        index = std::min(std::min(attributesPos, scriptsPos), directionPos);
        mergedAttributes.addAttribute(start, index - 1, style, currentVisualOrder, currentSequence);
        if(currentDirection == 1)
            currentSequence++;

        if(scriptsDone && directionDone && attributesDone)
            allDone = true;

        currentStyle = style->clone();

    } while (!allDone);


    if(style->getWrap()){
        // After everything is merged, word wrap the text

        // First, find out the different locales we might have in the text so we
        // can pick the right word wrapping method (ie, dictionary based for Thai)
        TextLocaleItemizer::processLocales(locales, layout.text, mergedAttributes);
        //TIDEBUG("Locales:\n" << locales);
        layout.wordBreaks = TextWordBreakItemizer::processWordBreaks(layout.text, locales);
        //TIDEBUG("Word breaking boundaries:\n" << layout.wordBreaks);
    }

    // Reorder items in visual order if necessary
    if(dir.get().size() > 1) {
        std::stable_sort(mergedAttributes.get().begin(), mergedAttributes.get().end(), visual_order);
    }

    //TIDEBUG("Final items:\n" << mergedAttributes);

    // Create the items
    TextAttributeRuns::TextAttributeRunsList::const_iterator it = mergedAttributes.get().begin();
    TextAttributeRuns::TextAttributeRunsList::const_iterator end = mergedAttributes.get().end();

    for(; it != end; it++) {
        TextAttributeRun::SharedPtr attr = *it;

        hb_buffer_t* buf = createHarfbuzzBuffer(attr->style, layout.text, attr->start, attr->end - attr->start + 1);
        layout.addItem(*it, buf);
    }

    return layout;
}

inline void tiLayout::applyPadding()
{
    Coordinate indent = this->style->getIndent();
    if (this->lines.size() == 1 && indent > 0) {
        // We have an indented single line, we need to extend the bounds to include it as there will be no second line that will cause the bounds to include indent.
        this->logicalBounds.x -= indent;
        this->inkBounds.x -= indent;
        this->bounds.x -= indent;
        this->logicalBounds.width += indent;
        this->inkBounds.width += indent;
        this->bounds.width += indent;
    }

    bounds.x -= hpad;
    bounds.y -= vpad;
    bounds.width += (hpad * 2);
    bounds.height += (vpad * 2);
}

tiLine* tiLayout::getNewLine() {
    lines.push_back(tiLine());
    context.lines = lines.size();
    return &lines.back();
}

bool tiLayout::realign(const Size &sz)
{
    if (sz.width == CoordinateMax && sz.height == CoordinateMax)
        return false;

    Size size = sz;
    size.width = std::max<Coordinate>(size.width - (hpad * 2), 0);
    size.height = std::max<Coordinate>(size.height - (vpad * 2), 0);

    Rect paddedBounds = bounds;
    paddedBounds.width -= (hpad * 2);
    paddedBounds.height -= (vpad * 2);
    if (paddedBounds.height > desiredBounds.height && desiredBounds.height > 0)
        paddedBounds.height = desiredBounds.height;

    const uint8_t align = style->getAlign();
    Coordinate moveX = 0, moveY = 0;

    if(size.height != CoordinateMax) {
        Coordinate newY = 0;
        if(align & Style::ALIGN_CENTER_VERTICAL)
            newY = static_cast<int>((size.height - paddedBounds.height ) / 2);
        else if(align & Style::ALIGN_BOTTOM)
            newY = size.height - paddedBounds.height;
        desiredBounds.height = size.height;
        context.desiredBounds->height = desiredBounds.height;

        moveY = newY - paddedBounds.y;
    }

    if(size.width != CoordinateMax && size.width) {
        Coordinate newX = 0;
        if(align & Style::ALIGN_RIGHT)
            newX = size.width - paddedBounds.width;
        else if(align & Style::ALIGN_CENTER_HORIZONTAL)
            newX = static_cast<int>((size.width - paddedBounds.width ) / 2);
        desiredBounds.width = size.width;
        context.desiredBounds->width = desiredBounds.width;

        moveX = newX - paddedBounds.x;
    }

    if(moveX || moveY) {
        move(moveX, moveY);
        return true;
    }

    return false;
}

tiRun* tiLine::getNewRun()
{
    runs.push_back(tiRun());
    return &runs.back();
}

tiRun* tiLine::copyNewRun(const tiRun& run) {
    tiRun* newRun = getNewRun();
    newRun->start = run.start;
    newRun->end = run.end;
    newRun->logicalBounds = run.logicalBounds;
    newRun->inkBounds = run.inkBounds;
    newRun->originalHeight = run.originalHeight;
    newRun->originalY = run.originalY;
    newRun->originalInkY = run.originalInkY;
    newRun->lastWordMark = run.lastWordMark;
    newRun->item = run.item;
    return newRun;
}

void tiLine::rtrim(bool applyTracking) {
    if (runs.size() == 0)
        return;

    std::vector<tiRun>::reverse_iterator it;
    std::vector<tiRun>::size_type index = 0;
    bool found = false;
    bool needsMaxAscent = false;

    for (it = runs.rbegin(); it != runs.rend(); ++it) {
        tiRun& run = *it;

        Style::SharedPtr style = run.item->attribute->style;
        GlyphString& gs = run.item->getGlyphString();

        uint32_t result;
        if (run.end > run.start)
            result = gs.rtrim(run.start, run.end, style->getFont());
        else
            result = run.end;

        uint32_t end = run.end;
        if (result != INT_MAX) {
            index = std::distance(runs.begin(), it.base()) - 1;
            found = true;
            if (result == end)
                break;

            logicalBounds.width -= run.logicalBounds.width;
            inkBounds.width -= run.inkBounds.x + run.inkBounds.width;

            run.end = result;
            Rect trimmedLogical, trimmedInk;
            style->getFont()->getFreetype()->getExtents(gs.buffer(), style, &trimmedLogical, &trimmedInk, run.start, run.end + 1);
            run.logicalBounds.width = trimmedLogical.width;
            run.inkBounds.width = trimmedInk.width;

            logicalBounds.width += run.logicalBounds.width;
            inkBounds.width += run.inkBounds.x + run.inkBounds.width;
            break;
        } else { // We have removed the whole run, recalculate the max ascents
            logicalBounds.width -= run.logicalBounds.width;
            inkBounds.width -= run.inkBounds.x + run.inkBounds.width;

            needsMaxAscent = true;
        }
    }

    if (found) {
        runs.erase(runs.begin() + index + 1, runs.end());

        // Recalculate the maximum ascent
        if (needsMaxAscent) {
            maxAscent = 0;
            maxInkAscent = 0;
            for(std::vector<tiRun>::iterator fit = runs.begin(); fit < runs.end(); ++fit) {
                if(fit->logicalBounds.y < maxAscent)
                    maxAscent = fit->originalY;

                if(fit->inkBounds.y < maxInkAscent)
                    maxInkAscent = fit->originalInkY;
            }
        }

        //alignRuns();
    }

    if (applyTracking && runs.size() != 0) {
        double tracking = runs.back().item->attribute->style->getTypography().tracking;
        runs.back().logicalBounds.width -= tracking;
    }
}

void tiLine::addRun(LayoutContext& context, const TextItem::SharedPtr item, uint32_t start, uint32_t end, uint32_t lastWordMark)
{
    //assert(end >= start);
    tiRun* run = getNewRun();
    run->item = item;
    run->start = start;
    run->end = end;
    run->lastWordMark = lastWordMark;
    run->logicalBounds = context.currentLogical;
    run->inkBounds = context.currentInk;
    context.currentLogical.clear();
    context.currentInk.clear();

    run->originalHeight = run->logicalBounds.height;
    run->originalY = run->logicalBounds.y;
    run->originalInkY = run->inkBounds.y;

    if(run->logicalBounds.y < maxAscent)
        maxAscent = run->logicalBounds.y;

    if(run->inkBounds.y < maxInkAscent)
        maxInkAscent = run->inkBounds.y;
}


void tiLine::updateRunsPosition(Coordinate deltaX, Coordinate deltaY, Coordinate deltaW, Coordinate deltaH)
{
    std::vector<tiRun>::iterator it = runs.begin();
    std::vector<tiRun>::iterator end = runs.end();
    const Rect delta(deltaX, deltaY, deltaW, deltaH);

    while(it != end) {
        it->logicalBounds += delta;
        it->inkBounds += delta;
        ++it;
    }

    bounds += delta;
    logicalBounds += delta;
    inkBounds += delta;
}


void tiLayout::alignLine(tiLine* line, bool useLayoutBounds)
{
    Coordinate offsetX = CoordinateMax;
    Coordinate width = 0;
    Coordinate x = 0;

    if(!useLayoutBounds) {
        //if(line->logicalBounds.width > desiredBounds.width)
        //    return;

        x = desiredBounds.x;
        width = desiredBounds.width;

    } else {
        //if(bounds.width > desiredBounds.width)
        //    return;

        x = bounds.x;
        width = bounds.width;
    }

    uint8_t alignment = context.baseStyle->getAlign();

    // Flip alignment if we are RTL
    if(context.baseStyle->getDirection() == 1) {
        if(alignment & Style::ALIGN_LEFT) {
            alignment = Style::ALIGN_RIGHT;
        } else if(alignment & Style::ALIGN_RIGHT) {
            alignment = Style::ALIGN_LEFT;
        }
    }

    Coordinate indent = style->getIndent();
    Coordinate indentOffset = 0;
    if (indent > 0) {
        if (lines.size() == 1 || line->firstLine) {
            indentOffset = indent;
        }
    } else if (indent < 0) {
        if (lines.size() > 1 && !line->firstLine) {
            indentOffset = -indent;
        }
    }

    if (alignment & Style::ALIGN_LEFT) {
        offsetX = x + indentOffset;
    } else if (alignment & Style::ALIGN_RIGHT) {
        offsetX = (width - line->logicalBounds.width) - indentOffset;/// - x;
    } else if(alignment & Style::ALIGN_CENTER_HORIZONTAL) {
        offsetX = static_cast<int>((width - x - line->logicalBounds.width) / 2) - x;
    }

    if(offsetX != CoordinateMax) {
        offsetX = offsetX - line->logicalBounds.x;
        line->updateRunsPosition(offsetX, 0);
    }
}


void tiLine::alignRuns(Coordinate baseY)
{
    std::vector<tiRun>::iterator it = runs.begin();
    std::vector<tiRun>::iterator end = runs.end();

    bounds.empty();
    logicalBounds.empty();
    inkBounds.empty();

    for(; it != end; ++it) {
        Coordinate logicalY = it->originalY; //it->logicalBounds.y;
        it->logicalBounds.y -= maxAscent - baseY;
        it->inkBounds.y = it->logicalBounds.y - (logicalY - it->originalInkY/*inkBounds.y*/);

        Coordinate rise = it->item->attribute->style->getRise();
        if (rise != 0) {
            it->logicalBounds.y -= rise;
            it->inkBounds.y -= rise;
        }

        logicalBounds.unite(it->logicalBounds);
        inkBounds.unite(it->inkBounds);
    }

    bounds.unite(logicalBounds);
}

//--------

static void resetXPosition(LayoutContext& context, bool newLine = false)
{
    // This function exists so we can do things whenever a new line start, like add indentation

    context.currentPosition.x = context.desiredBounds->x;

    Coordinate indent = context.baseStyle->getIndent();

    if (!(context.baseStyle->getAlign() & Style::ALIGN_CENTER_HORIZONTAL)) {
        if (indent > 0) {
            if (context.lines == 0 || newLine ) {
                context.currentPosition.x += indent;
            }
        } else if (indent < 0) {
            if (context.lines > 0 && !newLine) {
                context.currentPosition.x += -indent;
            }
        }
    }

    context.firstWord = true;
    context.previousPosition = context.currentPosition;
    context.currentLogical = Rect();
    context.currentInk = Rect();
    context.maxWidth = context.desiredBounds->width;
}

static void addWord(LayoutContext& context, Rect& logical, Rect& ink, bool usePrevious = false)
{
    if(usePrevious) {
        context.currentLogical = context.previousLogical;
        context.currentInk = context.previousInk;
        context.currentPosition = context.previousPosition;

    } else {
        // Save the rectangles for later, when we need to right trim
        // the last word of the line
        context.previousLogical = context.currentLogical;
        context.previousInk = context.currentInk;
        context.previousPosition = context.currentPosition;
    }

    logical.x = context.currentPosition.x;
    ink.x += context.currentPosition.x;

    context.currentLogical.unite(logical);
    context.currentInk.unite(ink);
    context.currentPosition.x += logical.width;
}

void tiLayout::finishLine(tiLine* line)
{
    if (line == 0)
        line = currentLine;

    if(getLineCounter() >= getFirstLine()) {
        line->alignRuns(context.currentPosition.y);

        // Calculate lineHeight using CSS's box model. Leading is defined as the remaining space between the measured line height and the one specified by the user.
        // Half the leading should be applied to the top and half to the bottom, making the line vertically centered in the specified line height.
        Coordinate lineHeight = context.baseStyle->getLineHeight();
        if(lineHeight == CoordinateMax)
            lineHeight = line->logicalBounds.height;

        const Coordinate leading = lineHeight - line->logicalBounds.height;
        const int halfLeading = static_cast<int>(leading / 2);

        if(leading != 0) {
            line->updateRunsPosition(0, halfLeading);
            line->logicalBounds.height += leading;
            line->logicalBounds.y -= halfLeading;
            line->inkBounds.y -= halfLeading;
        }

        context.currentPosition.y += lineHeight;

        // If the width is not known, we can't align the line horizontally
        if(desiredBounds.width != CoordinateMax)
            alignLine(line);

        logicalBounds.unite(line->logicalBounds);
        inkBounds.unite(inkBounds);
        bounds.unite(logicalBounds);
    } else {
        incrementLineCounter();
    }
}

void tiLayout::move(Coordinate deltaX, Coordinate deltaY, Coordinate /*deltaW*/, Coordinate /*deltaH*/)
{
    bounds.clear();
    logicalBounds.clear();
    inkBounds.clear();

    std::vector<tiLine>::iterator it = lines.begin();
    const std::vector<tiLine>::iterator end = lines.end();

    while(it != end) {
        it->updateRunsPosition(deltaX, deltaY, 0, 0);
        logicalBounds.unite(it->logicalBounds);
        inkBounds.unite(it->inkBounds);
        bounds.unite(logicalBounds);
        ++it;
    }

    applyPadding();
}

void tiLayout::moveRunsDown() {
    tiLine* newLine = this->getNewLine();
    tiLine& previousLine = this->lines[this->lines.size() - 2];
    this->currentLine = newLine;

    bool found = false;
    std::vector<tiRun>::reverse_iterator it;
    std::vector<tiRun>::size_type index = 0;
    bool deleteSplitRun = false;

    uint32_t splitRunStart = 0;
    uint32_t splitRunEnd = 0;

    // First, find the last run that starts a segmented word boundary
    for (it = previousLine.runs.rbegin(); it != previousLine.runs.rend(); ++it) {
        tiRun& run = *it;
        if (run.lastWordMark != INT_MAX) {
            // We found a word marker. Split the item at that point and rtrim it
            index = std::distance(previousLine.runs.begin(), it.base()) - 1;
            Style::SharedPtr style = run.item->attribute->style;

            uint32_t start = run.start;

            if (index == 0 && run.lastWordMark == start && run.start == run.end) {
                // Rare worst possible case, the split point is at the beginning of the first run of the previous line.
                // This is a long word, so we just create the new line and bail out early.
                finishLine(&previousLine);
                resetXPosition(context);
                return;
            }

            if (run.start != run.end || run.lastWordMark != start) {
                // We will split the run. Save the remaining offsets for later.
                splitRunEnd = run.end;
                splitRunStart = run.lastWordMark + 1;
                run.end = run.lastWordMark;

                if (run.end > run.start) {
                    int result = run.item->getGlyphString().rtrim(run.start, run.end, style->getFont());
                    if (result == INT_MAX) {
                        //assert (result != INT_MAX);
                        deleteSplitRun = true; // if we get INT_MAX, remove the whole run as we have trimmed it entirely.
                        break;
                    }

                    run.end = result;
                }
                // Measure the run again
                Coordinate logicalX = run.logicalBounds.x; // We need to save the X because getExtents clears the passed rectangle
                Coordinate inkX = run.inkBounds.x;
                style->getFont()->getFreetype()->getExtents(run.item->getGlyphString().buffer(), style, &run.logicalBounds, &run.inkBounds, run.start, run.end + 1);
                run.logicalBounds.x = logicalX;
                run.inkBounds.x = inkX;
            } else {
                deleteSplitRun = true; // We are splitting at the run's start, which means no splitting but moving it completely to the next line
            }

            found = true;
            break;
        }
    }

    // Save the context's rectangles
    resetXPosition(context);

    if (found) {
        for(std::vector<tiRun>::iterator fit = previousLine.runs.begin() + index; fit < previousLine.runs.end(); ++fit) {
            tiRun* newRun = newLine->copyNewRun(*fit);
            if (newLine->runs.size() == 1) {
                // First run. This is the item we need to split.
                newRun->start = splitRunStart;
                newRun->end = splitRunEnd;
                Style::SharedPtr style = newRun->item->attribute->style;
                style->getFont()->getFreetype()->getExtents(newRun->item->getGlyphString().buffer(), style, &newRun->logicalBounds, &newRun->inkBounds, newRun->start, newRun->end + 1);
            }

            newRun->logicalBounds.x = context.currentPosition.x;
            newRun->inkBounds.x = context.currentPosition.x;
            newRun->logicalBounds.y = newRun->originalY;
            newRun->inkBounds.y = newRun->originalInkY;

            if (newRun->logicalBounds.y < newLine->maxAscent)
                newLine->maxAscent = newRun->logicalBounds.y;

            if (newRun->inkBounds.y < newLine->maxInkAscent)
                newLine->maxInkAscent = newRun->inkBounds.y;

            context.currentPosition.x += newRun->logicalBounds.width;
        }

        if (!deleteSplitRun)
            ++index;

        previousLine.runs.erase(previousLine.runs.begin() + index, previousLine.runs.end());
        previousLine.rtrim();
    }

    finishLine(&previousLine);
}

//--------

// Create a text layout for the ellipsis, so we can benefit from font fallbacks, etc
// TODO: Maybe this should return a raw layout run?
void getEllipsis(const Style::SharedPtr style, Rect& logical, Rect& ink, TextItem::SharedPtr& item) {
    const TextTruncation& t = style->getTextTruncation();
    if (t.ellipsis.empty())
        return;

    TextItemizerData ellipsisData = TextItemizer::itemize(t.ellipsis.c_str(), style);
    tiLayout ellipsisLayout;
    Rect r = Rect(0,0, CoordinateMax, CoordinateMax);
    TextItemizer::layout(ellipsisData, ellipsisLayout, r, TextItemizer::NO_LTRIM);

    assert(ellipsisLayout.lines.size() != 0);
    const tiLine& line = ellipsisLayout.lines[0];

    assert(line.runs.size() != 0);
    const tiRun& ellipsisRun = ellipsisLayout.lines[0].runs[0];

    item = ellipsisRun.item;
    logical = ellipsisRun.logicalBounds;
    ink = ellipsisRun.inkBounds;

    logical.y = line.maxAscent;
    ink.y = line.maxInkAscent;
}


void TextItemizer::truncateLine(const TextItemizerData& /*data*/, tiLayout& layout, bool truncateLastLine)
{
    if (!truncateLastLine && (layout.logicalBounds.width <= layout.desiredBounds.width || layout.lines.size() == 0)) {
        layout.truncated = false;
        return;
    }

    layout.truncated = true;

    const Style::SharedPtr style = layout.style;
    Rect ellipsisLogical, ellipsisInk;
    TextItem::SharedPtr ellipsis;

    getEllipsis(style, ellipsisLogical, ellipsisInk, ellipsis); // This should be cached

    layout.context.clear();
    Coordinate x = layout.desiredBounds.x;
    Coordinate width = 0;

    tiLine* line;
    if (truncateLastLine) {
        line = &layout.lines.back();
    } else {
        line = &layout.lines.front();
    }
    //GlyphString& gs = run.item->getGlyphString();

    std::vector<tiRun>::reverse_iterator rit;
    //std::vector<tiRun>::size_type index = 0;
    std::vector<tiRun>::iterator it = line->runs.begin();
    std::vector<tiRun>::iterator end = line->runs.end();

    const TextTruncation& t = style->getTextTruncation();
    double tracking = layout.style->getTypography().tracking;
    TextTruncation::Position p = t.position;

    if (style->getWrap() && p != TextTruncation::POSITION_NONE)
        p = TextTruncation::POSITION_END;

    switch (p) {
    case TextTruncation::POSITION_NONE:
        return;

    case TextTruncation::POSITION_START:
        width = layout.desiredBounds.width;// - ellipsisLogical.width;
        for (rit = line->runs.rbegin(); rit != line->runs.rend(); ++rit) {
            if (x + rit->logicalBounds.width + ellipsisLogical.width > layout.desiredBounds.width) {
                GlyphString& gs = rit->item->getGlyphString();
                const GibbonFreetype::SharedPtr freeType = it->item->attribute->style->getFont()->getFreetype();
                hb_buffer_t* buf = rit->item->getGlyphString().buffer();

                TextClusterReverseIterator cl = TextClusterReverseIterator(buf, rit->start, rit->end + 1);

                while (!cl.atEnd()) {
                    Rect logical, ink;

                    freeType->getExtents(buf, style, &logical, &ink, cl.pos(), rit->end + 1);
                    if (x + logical.width + ellipsisLogical.width > layout.desiredBounds.width) {
                        break;
                    } else {
                        rit->start = cl.pos();
                    }

                    cl++;
                }

                if (cl.pos() == rit->end) {
                    // remove the whole run
                    ellipsisLogical.x = rit->logicalBounds.x;
                    --rit;
                    break;
                }

                // Measure it so we can position the ellipsis
                //Coordinate runX = width - rit->logical.width;
                rit->start = gs.ltrim(rit->start, rit->end, it->item->attribute->style->getFont());
                freeType->getExtents(buf, style, &rit->logicalBounds, &rit->inkBounds, rit->start, rit->end + 1);
                Coordinate runX = width - rit->logicalBounds.width;
                rit->logicalBounds.x = runX;
                rit->inkBounds.x = runX;

                ellipsisLogical.x = rit->logicalBounds.x - ellipsisLogical.width;
                break;

            } else {
                rit->logicalBounds.x = width - rit->logicalBounds.width;
                width -= rit->logicalBounds.width;
                x += rit->logicalBounds.width;
            }
        }

        line->runs.erase(line->runs.begin(), --rit.base());

        layout.context.currentLogical = ellipsisLogical;
        layout.context.currentInk = ellipsisInk;
        layout.context.baseStyle = layout.style;

        line->addRun(layout.context, ellipsis, 0, ellipsis->getGlyphString().bufferLength() - 1, 0);

        layout.context.currentPosition.y = line->logicalBounds.y;
        line->bounds.clear();
        line->logicalBounds.clear();
        line->inkBounds.clear();

        // Reset the line's runs to their original position
        it = line->runs.begin();
        end = line->runs.end();

        for(; it != end; ++it) {
            it->logicalBounds.y = it->originalY;
            it->inkBounds.y = it->originalInkY;
            it->logicalBounds.height = it->originalHeight;
        }

        layout.finishLine(line);

        layout.calculateBounds();
        layout.offset.x = layout.bounds.x;

        break;

    case TextTruncation::POSITION_MIDDLE:
    case TextTruncation::POSITION_END:
        for(; it != end; ++it) {
            if (x + it->logicalBounds.width + ellipsisLogical.width + tracking > layout.desiredBounds.width) {
                // We found the run we need to truncate
                GlyphString& gs = it->item->getGlyphString();
                const GibbonFreetype::SharedPtr freeType = it->item->attribute->style->getFont()->getFreetype();
                hb_buffer_t* buf = it->item->getGlyphString().buffer();

                TextClusterIterator cl = TextClusterIterator(buf, it->start, it->end + 1);

                while (!cl.atEnd()) {
                    Rect logical, ink;

                    freeType->getExtents(buf, style, &logical, &ink, it->start, cl.pos() + 1);
                    if (x + logical.width + ellipsisLogical.width > layout.desiredBounds.width)
                        break;

                    cl++;
                }

                if (cl.pos() == it->start) {
                    // remove the whole run
                    ellipsisLogical.x = it->logicalBounds.x + tracking;
                    --it;
                    break;

                } else {
                    it->end = cl.pos() - 1;
                }

                // Measure it so we can position the ellipsis
                Coordinate runX = it->logicalBounds.x;
                it->end = gs.rtrim(it->start, it->end, it->item->attribute->style->getFont());
                freeType->getExtents(buf, style, &it->logicalBounds, &it->inkBounds, it->start, it->end + 1);
                it->logicalBounds.x = runX;
                it->inkBounds.x = runX;

                ellipsisLogical.x = it->logicalBounds.x + it->logicalBounds.width;

                break;
            } else {
                x += it->logicalBounds.width;
            }
        }

        line->runs.erase(it + 1, line->runs.end());

        layout.context.currentLogical = ellipsisLogical;
        layout.context.currentInk = ellipsisInk;
        layout.context.baseStyle = layout.style;

        line->addRun(layout.context, ellipsis, 0, ellipsis->getGlyphString().bufferLength() - 1, 0);

        layout.context.currentPosition.y = line->logicalBounds.y;
        line->bounds.clear();
        line->logicalBounds.clear();
        line->inkBounds.clear();

        // Reset the line's runs to their original position
        it = line->runs.begin();
        end = line->runs.end();

        Coordinate x = layout.desiredBounds.x;
        for(; it != end; ++it) {
            it->logicalBounds.x = x;
            it->inkBounds.x = x;
            it->logicalBounds.y = it->originalY;
            it->inkBounds.y = it->originalInkY;
            it->logicalBounds.height = it->originalHeight;
            x += it->logicalBounds.width;
        }

        layout.finishLine(line);
        layout.alignLine(line, false);

        layout.calculateBounds();
        layout.offset.x = layout.bounds.x;
    }
}


// This exists so we can potentially keep a layout and truncate on demand without recalculating
// We should add parameters to pass a maxLines and const TextTruncation&
void TextItemizer::truncate(const TextItemizerData &data, tiLayout& layout)
{
    layout.truncated = false;

    const Style::SharedPtr style = layout.style;

    unsigned maxLines = style->getMaxLines();
    //const TextTruncation& t = layout.style->getTextTruncation();

    if (layout.getFirstLine() != 0 ) {
        layout.lines.erase(layout.lines.begin(), layout.lines.begin() + layout.getFirstLine());
    }

    // Cap to maxlines
    if (maxLines != 0 && layout.lines.size() > maxLines) {
        layout.lines.erase(layout.lines.begin() + maxLines, layout.lines.end());
        layout.truncated = true;
    }

    // Find the first line that's out of bounds
    //if (layout.bounds.height > layout.desiredBounds.height) {
        std::vector<tiLine>::iterator lit = layout.lines.begin();
        std::vector<tiLine>::iterator lend = layout.lines.end();

        for( ;lit != lend; ++lit) { // Draw a line of text
            if (lit->logicalBounds.y + lit->logicalBounds.height > layout.desiredBounds.height) {
                layout.truncated = true;
                layout.lines.erase(lit, lend);
                break;
            }
        }
    //}

    if (!layout.truncated)
        return;

    if (layout.lines.empty() || layout.style->getTextTruncation().position == TextTruncation::POSITION_NONE) {
        layout.calculateBounds();
        return;
    }

    tiLine& lastLine = layout.lines.back();

    Rect ellipsisLogical, ellipsisInk;
    TextItem::SharedPtr ellipsis;
    getEllipsis(style, ellipsisLogical, ellipsisInk, ellipsis); // This should be cached

    layout.context.clear();
    double tracking = layout.style->getTypography().tracking;

    if (lastLine.logicalBounds.width + ellipsisLogical.width + tracking > layout.desiredBounds.width) {
        // The ellipsis doesn't fit. We need to cut some space
        Coordinate needed = lastLine.logicalBounds.width + ellipsisLogical.width - layout.desiredBounds.width + tracking;

        std::vector<tiRun>::reverse_iterator it;
        std::vector<tiRun>::size_type index = INT_MAX;
        uint32_t boundaryIndex = 0;

        for (it = lastLine.runs.rbegin(); it != lastLine.runs.rend(); ++it) {
            tiRun& run = *it;

            // Remove runs until we have made our space
            if (needed - run.logicalBounds.width < 0) {
                // We found our run. Now remove words until the ellipsis fits
                index = std::distance(lastLine.runs.begin(), it.base()) - 1;

                Style::SharedPtr style = run.item->attribute->style;
                GlyphString& gs = run.item->getGlyphString();
                hb_glyph_info_t* infos = gs.getGlyphInfos();
                hb_buffer_t* buf = gs.buffer();
                GibbonFreetype::SharedPtr freetype = style->getFont()->getFreetype();

                uint32_t clusterStart = infos[run.start].cluster;
                uint32_t clusterEnd = infos[run.end].cluster;

                uint32_t boundary = 0;

                TextWordBreakRun wordBreak = data.wordBreaks.getRightBoundary(clusterStart, clusterEnd, boundaryIndex);
                if (boundaryIndex == INT_MAX)
                    boundaryIndex = data.wordBreaks.size();

                Rect removeRect;

                // Iterate through the word breaking boundaries until we have enough space
                do {
                    wordBreak = data.wordBreaks.getPreviousBoundary(boundaryIndex);
                    boundary = gs.getBufferIndexForStringIndex(wordBreak.start, GlyphString::Boundary);
                    if (boundary == INT_MAX /*|| boundary < static_cast<uint32_t>(run.start)*/) {
                        // even the first word of the run needs to be removed, remove the entire run
                        //boundary = run.start;
                        break;
                    }
                    freetype->getExtents(buf, style, &removeRect, 0, boundary, run.end + 1);
                } while (removeRect.width < ellipsisLogical.width || wordBreak.start == INT_MAX);

                if (static_cast<int>(boundary) != INT_MAX && static_cast<int>(boundary) >= run.start) {
                    // Right trim the run
                    uint32_t result = gs.rtrim(run.start, boundary, style->getFont());
                    run.end = result;
                    Coordinate lx = run.logicalBounds.x;
                    Coordinate ix = run.inkBounds.x;

                    // Measure it so we can position the ellipsis
                    freetype->getExtents(buf, style, &run.logicalBounds, &run.inkBounds, run.start, run.end + 1);
                    run.logicalBounds.x = lx;
                    run.inkBounds.x = ix;

                    ellipsisLogical.x = run.logicalBounds.x + run.logicalBounds.width;

                } else {
                    if (index == 0) {
                        index = INT_MAX;
                    } else {
                        --index;
                        if (wordBreak.start != clusterStart)
                            continue;
                    }
                    ellipsisLogical.x = run.logicalBounds.x;
                }

                if (index == INT_MAX) {
                    // The whole line has been removed, use character based truncation by using the single line truncation function
                    truncateLine(data, layout, true);
                    return;
                } else {
                    // Remove the leftover runs
                    lastLine.runs.erase(lastLine.runs.begin() + index + 1, lastLine.runs.end());
                }

                break;

            } else {
                needed -= run.logicalBounds.width;
            }
        }

        if (index == INT_MAX) { // we couldn't make enough space for the ellipsis. Go to character truncation.
            truncateLine(data, layout, true);
            return;
        }

    } else {
        ellipsisLogical.x = lastLine.logicalBounds.x + lastLine.logicalBounds.width;
    }

    // Right trim to avoid dangling spaces belonging to other runs
    if (lastLine.runs.size() != 0) {
        lastLine.rtrim(false);
        tiRun& lastRun = lastLine.runs.back();
        lastRun.logicalBounds.width += tracking;
        ellipsisLogical.x = lastRun.logicalBounds.right();
    }

    ellipsisInk.x = ellipsisLogical.x;

    layout.context.currentLogical = ellipsisLogical;
    layout.context.currentInk = ellipsisInk;
    layout.context.baseStyle = layout.style;

    lastLine.addRun(layout.context, ellipsis, 0, ellipsis->getGlyphString().bufferLength() - 1, 0);

    layout.context.currentPosition.y = lastLine.logicalBounds.y;
    lastLine.bounds.clear();
    lastLine.logicalBounds.clear();
    lastLine.inkBounds.clear();

    // Reset the line's runs to their original position
    std::vector<tiRun>::iterator it = lastLine.runs.begin();
    std::vector<tiRun>::iterator end = lastLine.runs.end();

    Coordinate x = layout.desiredBounds.x;
    for(; it != end; ++it) {
        it->logicalBounds.x = x;
        it->inkBounds.x = x;
        it->logicalBounds.y = it->originalY;
        it->inkBounds.y = it->originalInkY;
        it->logicalBounds.height = it->originalHeight;
        x += it->logicalBounds.width;
    }

    layout.finishLine(&lastLine);
    layout.alignLine(&lastLine, false);

    layout.calculateBounds();
    layout.offset.x = layout.bounds.x;
}


//--------

 void TextItemizer::layout(const TextItemizerData &data, tiLayout &layout, const Rect &r, unsigned int flags)
{
     std::vector<TextItem::SharedPtr>::const_iterator lit = data.items.begin();
     std::vector<TextItem::SharedPtr>::const_iterator lend = data.items.end();

     layout.desiredBounds = r;
     layout.style = data.style;

     layout.context.desiredBounds = &layout.desiredBounds;
     layout.context.baseStyle = layout.style;
     layout.context.maxWidth = layout.desiredBounds.width;

     resetXPosition(layout.context);

     layout.hasBottomLayer = data.hasBottomLayer || !layout.style->getBackgroundColor().empty();

     if (data.items.size() == 0) {
         return;
     }

     layout.currentLine = layout.getNewLine();

     double tracking = layout.style->getTypography().tracking;

     //TIDEBUG("\n..............................................");
     //TIDEBUG("Itemizer layout for: " << r);

     /*int baseDirection = baseStyle->getDirection();*/

     bool layoutFinished = false;
     const bool wordWrapping = data.style->getWrap();

     bool clearNextLine = false;

     enum Operations {
         Iterating = 0,
         StartComposingWord = 1,
         ComposingWord = 2,
         AddingLongWord = 3
     };

     Operations currentOperation = Iterating;
     uint32_t wordMark = INT_MAX;

     int i = 0;
     for(; lit != lend; lit++) {
         Rect logical, ink;
         TextItem& item = *lit->get();
         //TIDEBUG("\n ITEM ====================\n");
         //TIDEBUG(item);

         uint32_t clusterStart = 0;
         uint32_t clusterEnd = 0;
         Rect validLogical, validInk;
         Rect previousLogical, previousInk;

         // Cache frequently used values
         Style::SharedPtr style = item.attribute->style;
         Font::SharedPtr font = style->getFont();
         GibbonFreetype::SharedPtr freeType = style->getFont()->getFreetype();
         GlyphString& hb = item.getGlyphString();
         unsigned int bufferLength = hb.bufferLength();
         hb_glyph_info_t* glyphInfos = hb.getGlyphInfos();
         int direction = LTR;//item.attribute->style->getDirection();

         TextWordBreakRun wordBreak;

         uint32_t bufferStart = 0;
         uint32_t bufferEnd = 0;
         //uint32_t previousBufferStart = 0;
         uint32_t previousBufferEnd = INT_MAX;
         uint32_t currentLineStart = 0;
         clusterStart = glyphInfos[bufferStart].cluster;
         clusterEnd = glyphInfos[bufferLength - 1].cluster;
         uint32_t boundaryIndex = 0;
         bool newLineAfter = false;
         bool wordBoundaryAfter = false;
         bool noWordBoundaries = false;
         wordMark = INT_MAX;
         layout.context.newLineBuffer = 0;
         layout.context.newLineHeight = 0;

         if(direction == LTR) {
             wordBreak = data.wordBreaks.getBoundary(clusterStart, clusterEnd, boundaryIndex);
             if(wordBreak.start == INT_MAX) {
                 wordBreak = data.wordBreaks.getBoundary(clusterStart, clusterEnd + 1, boundaryIndex);
                 if (wordBreak.start == clusterEnd + 1) {
                     wordBoundaryAfter = true;

                 } else {
                     wordBoundaryAfter = false;

                     if (currentOperation != ComposingWord) {
                         wordMark = bufferLength - 1;
                         currentOperation = StartComposingWord;
                     }
                 }

                 // If the next boundary, outside this buffer, contains a new line
                 // do not adjust it
                 if (!wordBreak.newParagraph)
                     wordBreak.start = clusterEnd;

                 noWordBoundaries = true;
             }
         } else {
             wordBreak = data.wordBreaks.getRightBoundary(clusterStart, clusterEnd, boundaryIndex);
         }

         bufferEnd = wordBreak.start;

         //layout.context.firstWord = true;

         while (wordBreak.start != INT_MAX) {
             uint32_t boundary = wordBreak.start;
             uint32_t boundaryCluster;

             if (noWordBoundaries) {
                 boundaryCluster = hb.bufferLength() - 1;
             } else {
                 boundaryCluster = hb.getBufferIndexForStringIndex(boundary, GlyphString::Boundary);
             }

             if(direction == LTR) {
                 bufferEnd = boundaryCluster;
             } else {
                 bufferStart = boundaryCluster;
             }

             //TIDEBUG("word: " << hbToString(item, bufferStart, bufferEnd));

             if(wordBreak.newParagraph && wordBreak.start == glyphInfos[bufferEnd].cluster + 1) {
                 // Check where in the word the new line is
                 if (glyphInfos[bufferEnd].codepoint == 0) {
                     if (bufferEnd == 0) {
                         // Single item with a new line, mark it with INT_MAX
                         // so we add an empty run to fill a line.
                         bufferEnd = INT_MAX;

                     } else {
                         // If the current words ends in a \n, decrease the buffer end
                         // so a square is not drawn. We will need to increase this later.
                         --bufferEnd;
                     }
               } else {
                     // The new line glyph is on the next item, which will be a trailing
                     // one. Just clear the line and add an empty run on the next pass.
                     clearNextLine = true;
                 }

                 newLineAfter = true;
             }

             Rect logical, ink;
             if (bufferEnd != INT_MAX) {
                 freeType->getExtents(hb.buffer(), style, &logical, &ink, bufferStart, bufferEnd + 1);
             }
             Coordinate currentRunWidth = logical.width;

             if (!wordWrapping || layout.context.currentPosition.x + currentRunWidth <= layout.context.maxWidth) {
                 if (layout.context.firstWord && !(flags & NO_LTRIM)) {
                     // Left trim the first word of a new line
                     uint32_t result = hb.ltrim(bufferStart, bufferEnd + 1, font);
                     if (result == INT_MAX) {
                         freeType->getExtents(hb.buffer(), style, &logical, &ink, result, bufferEnd + 1);
                         currentLineStart = bufferEnd + 1;
                     } else if (result != bufferStart) {
                         freeType->getExtents(hb.buffer(), style, &logical, &ink, result, bufferEnd + 1);
                         currentLineStart = result;
                     }
                     layout.context.firstWord = false;
                 }

                 if (!noWordBoundaries || (noWordBoundaries && wordBoundaryAfter)) {
                     currentOperation = Iterating;
                 }

                 addWord(layout.context, logical, ink);

             } else { // The word doesn't fit
                 uint32_t currentLineEnd = bufferEnd;
                 bool trimmed = false;

                 if(wordWrapping) {
                     // See if the word's last glyph tracking is making it not fit
                     /*if (tracking > 0 && layout.context.currentPosition.x + currentRunWidth - tracking <= layout.context.maxWidth) {
                         trimmed = true;
                         logical.width -= tracking; //tracking should not affect the ink
                         addWord(layout.context, logical, ink);
                         currentLineEnd = bufferEnd;

                     } else {*/
                         // Right trim the word to see if it fits  TODO: ltrim on RTL?
                         uint32_t result;
                         if (bufferEnd != INT_MAX) {
                             result = hb.rtrim(bufferStart, bufferEnd, font);
                         } else {
                             result = INT_MAX;
                         }

                         if (result == INT_MAX) {
                             trimmed = true;

                         } else if (result != bufferEnd) {
                             Rect trimmedLogical, trimmedInk;
                             freeType->getExtents(hb.buffer(), style, &trimmedLogical, &trimmedInk, bufferStart, result + 1);
                             Coordinate trimmedRunWidth = trimmedLogical.width;

                             if(layout.context.currentPosition.x + trimmedRunWidth <= layout.context.maxWidth) {
                                 // The trimmed word fits. Add it and finish the line.
                                 trimmed = true;
                                 currentLineEnd = result;
                                 addWord(layout.context, trimmedLogical, trimmedInk);
                                 currentOperation = Iterating;

                                 // If the word fits trimmed at the end of line a new line is implied, so there's no need to process a <br> if present
                                 if (newLineAfter)
                                     bufferEnd++; //TODO: RTL?
                                 newLineAfter = false;

                             }
                         }
                     //}

                     if(!trimmed) {
                         // The word doesn't fit. Check if it's a long word.
                         if (logical.width - tracking > layout.context.maxWidth) {
                             currentOperation = AddingLongWord;
                             // This word is longer than the line width. Add it in chunks breaking it when it reaches the end of the line.
                             Rect charLogical, charInk;
                             Rect validLogical, validInk;
                             uint32_t wordOffset = bufferStart;
                             uint32_t position = 0;
                             uint32_t endPosition = 0;
                             bool isSingleChar = false;

                             TextClusterIterator ci = TextClusterIterator(hb, bufferStart, bufferEnd + 1);

                             while (!ci.atEnd()) {
                                 position = ci.pos();
                                 ci++;
                                 endPosition = ci.pos();
                                 freeType->getExtents(hb.buffer(), style, &charLogical, &charInk, wordOffset, endPosition);

                                 if ((position == bufferStart) && (layout.context.currentPosition.x + charLogical.width > layout.context.maxWidth) && (charLogical.width <= layout.context.maxWidth)) {
                                     layout.currentLine->addRun(layout.context, *lit, currentLineStart, bufferStart - 1);
                                     layout.finishLine();
                                     resetXPosition(layout.context);
                                     layout.currentLine = layout.getNewLine();
                                     currentLineStart = bufferStart;
                                 }

                                 if (validLogical.empty()) { // Just in case we hit the add word case on the first glyph of the line
                                     validLogical = charLogical;
                                     validInk = charInk;
                                 }

                                 if (position - wordOffset == 0 && charLogical.width > layout.context.maxWidth) {
                                     isSingleChar = true;
                                 }

                                 if (isSingleChar || (layout.context.currentPosition.x + charLogical.width > layout.context.maxWidth)) {
                                     addWord(layout.context, validLogical, validInk);
                                     wordOffset = position;
                                     if (isSingleChar /*|| position == wordOffset*//*bufferStart*/) {
                                         isSingleChar = false;
                                         layout.currentLine->addRun(layout.context, *lit, currentLineStart, position);
                                         currentLineStart = position + 1;
                                     } else {
                                         //if (position != wordOffset) {
                                             layout.currentLine->addRun(layout.context, *lit, currentLineStart, position - 1);
                                             currentLineStart = wordOffset;
                                         //}
                                     }

                                     layout.finishLine();
                                     resetXPosition(layout.context);
                                     layout.currentLine = layout.getNewLine();

                                     validLogical.clear();
                                     validInk.clear();
                                 } else {
                                     validLogical = charLogical;
                                     validInk = charInk;
                                 }
                             }

                             // Handle leftover glyphs. We could have either the remaining of the word or a single cluster.
                             // A single leftover cluster would have forced the loop exit, so we need to measure it.

                             if (position < bufferEnd) {
                                 freeType->getExtents(hb.buffer(), style, &validLogical, &validInk, position, bufferEnd + 1);
                             } else if (wordOffset == bufferEnd) { // we have a single character leftover
                                 validLogical = charLogical;
                                 validInk = charInk;
                             }

                             if (!validLogical.empty()) {
                                 addWord(layout.context, validLogical, validInk);
                             }

                         } else {
                             // It is not a long word. See if we have runs on the previous line that belong to this word.
                             // If that's the case we need to move them down and finish the previous line
                             if(currentOperation == ComposingWord && layout.lines.size() != 0) {
                                 if (bufferStart != previousBufferEnd && previousBufferEnd != INT_MAX)
                                     layout.currentLine->addRun(layout.context, *lit, currentLineStart, previousBufferEnd, wordMark);
                                 layout.moveRunsDown();
                             } else {
                                 currentLineEnd = previousBufferEnd;
                                 // We have everything we need to finish the line, but we have
                                 // to right trim so we can properly right/center align it, if
                                 // requested.
                                 /*
                                 if (previousBufferEnd != INT_MAX){
                                     uint32_t result = hb.rtrim(previousBufferStart, previousBufferEnd, font);
                                     if(result != INT_MAX && result != previousBufferEnd) {
                                         Rect trimmedLogical, trimmedInk;
                                         freeType->getExtents(hb.buffer(), style, &trimmedLogical, &trimmedInk, previousBufferStart, result + 1);
                                         addWord(layout.context, trimmedLogical, trimmedInk, true); // add again the last word of the line, this time trimmed
                                         currentLineEnd = result;
                                     } else {
                                         currentLineEnd = previousBufferEnd;
                                     }
                                 } else {
                                     currentLineEnd = INT_MAX;
                                 }
                                 */
                             }
                             if(currentOperation == StartComposingWord)
                                 currentOperation = Iterating;
                         }
                     }
                 } else {
                     addWord(layout.context, logical, ink);
                 }

                 if(currentOperation == Iterating) { // Add a new line only if we haven't moved runs down from the previous line
                     if (currentLineEnd != INT_MAX && currentLineStart <= currentLineEnd) {
                         layout.currentLine->addRun(layout.context, *lit, currentLineStart, currentLineEnd, wordMark);
                         layout.currentLine->rtrim();
                         previousBufferEnd = INT_MAX;
                     }
                     layout.finishLine();
                     resetXPosition(layout.context);
                 }

                 // If no more words fit and we are not wrapping, we are done.
                 if(!wordWrapping) {
                     layoutFinished = true;
                 }

                 if (currentOperation == AddingLongWord) {
                     currentOperation = Iterating;
                 } else {
                     // If the word didn't fit trimmed on the previous line, we add it to the newly created one.
                     if(!trimmed) {
                         uint32_t result = hb.ltrim(bufferStart, bufferEnd, font);
                         if (result == INT_MAX) {
                             freeType->getExtents(hb.buffer(), style, &logical, &ink, result, bufferEnd + 1);
                             bufferStart = bufferEnd;
                         } else if (result != bufferStart) {
                             // TODO: This should be a function to add the first word of a line
                             freeType->getExtents(hb.buffer(), style, &logical, &ink, result, bufferEnd + 1);
                             bufferStart = result;
                         }

                         addWord(layout.context, logical, ink);
                         layout.context.firstWord = false;
                     }


                     if (currentOperation == Iterating) {
                         layout.currentLine = layout.getNewLine();
                     } else {
                         currentOperation = Iterating;
                     }

                     if(trimmed) {
                         //currentOperation = Iterating;
                         if(direction == LTR) {
                             currentLineStart = bufferEnd + 1;
                         } else {
                             currentLineStart = bufferEnd + 1;
                             //assert(false);
                         }
                     } else {
                         if(direction == LTR) {
                             currentLineStart = bufferStart;
                         } else {
                             currentLineStart = bufferStart;
                             //assert(false);
                         }
                     }
                 }
             }

             if(newLineAfter) {
                 layout.context.newLineHeight = freeType->getAscent(style) - freeType->getDescent(style);
                 layout.context.newLineBuffer = bufferEnd + 1;

                 if (layout.context.currentLogical.height == 0 || bufferEnd == INT_MAX) {
                     // This is a blank new line, with no text
                     // Ask the font for its line height and return it
                     layout.context.currentLogical.height = layout.context.newLineHeight;
                     layout.context.currentInk.width = 0;
                 }

                 if (bufferEnd != INT_MAX && bufferEnd >= currentLineStart) {
                     layout.currentLine->addRun(layout.context, *lit, currentLineStart, bufferEnd);
                 } else {
                     layout.context.currentLogical.width = 0;
                     layout.context.currentInk.width = 0;
                     layout.currentLine->addRun(layout.context, *lit, bufferStart, bufferStart);
                 }
                 layout.currentLine->rtrim();
                 layout.finishLine();
                 resetXPosition(layout.context, true);

                 layout.currentLine = layout.getNewLine();
                 layout.currentLine->firstLine = true;
                 newLineAfter = false;
                 if (bufferEnd != INT_MAX && glyphInfos[bufferEnd + 1].codepoint == 0) {
                     ++bufferEnd;
                 }

                 if (bufferEnd == INT_MAX) {
                     bufferEnd = bufferStart;
                 }


                 if(direction == LTR) {
                     currentLineStart = bufferEnd + 1;
                 } else {
                     currentLineStart = bufferEnd + 1;
                     //assert(false);
                 }

             }

             // If the next boundary is INT_MAX we create a fake boundary at the end of the buffer
             // so we can do a last run of the loop and then we set it to INT_MAX so it triggers
             // the loop's exit condition.
             if (currentOperation == StartComposingWord)
                 currentOperation = ComposingWord;


             if(noWordBoundaries) {
                 wordBreak.start = INT_MAX;
             } else {
                 wordBreak = data.wordBreaks.getNextBoundary(boundaryIndex);
                 boundary = wordBreak.start;
                 boundaryCluster = hb.getBufferIndexForStringIndex(boundary, GlyphString::Boundary);

                 if (wordBreak.start > clusterEnd + 1 && lit + 1 != lend) {
                     // We are ending on a non breaking boundary. If the next run's word
                     // doesn't fit we need to move this word down.
                     wordMark = bufferEnd; // ### RTL??
                     currentOperation = StartComposingWord; //ComposingWord
                     noWordBoundaries = true;
                 } else if (boundaryCluster == INT_MAX /*|| wordBreak.start >= clusterEnd*/) {
                     noWordBoundaries = true;
                 }

                 if (wordBreak.start == INT_MAX) {
                     wordBreak.start = clusterEnd;
                 }
             }

             //previousBufferStart = bufferStart;
             previousBufferEnd = bufferEnd;
             previousLogical = logical;
             previousInk = ink;

             if(direction == LTR) {
                 bufferStart = bufferEnd + 1;

             } else {
                 bufferEnd = bufferStart - 1;
                 bufferStart = 0;
             }
         }

         if(layoutFinished)
             break;


         // Remaining buffer after the last word boundary
         if (direction == LTR) {
             if (clearNextLine) {
                 layout.context.currentLogical.width = 0;
                 layout.context.currentInk.width = 0;
                 clearNextLine = false;
             }

             if (currentLineStart <= bufferLength - 1) {
                 layout.currentLine->addRun(layout.context, *lit, currentLineStart, bufferLength - 1, wordMark);
             }
         }

         ++i;
     }

     if(!layoutFinished) {
         layout.currentLine->rtrim();
         layout.finishLine();

         // Add an empty trailing line if necessary (we know it by its run being 0 as it has no runs)
         if (layout.currentLine->logicalBounds.height == 0 && layout.context.newLineHeight != 0) {
             layout.context.currentLogical.height = layout.context.newLineHeight;
             lit--;
             layout.currentLine->maxAscent = 0;
             layout.currentLine->addRun(layout.context, *lit, layout.context.newLineBuffer, layout.context.newLineBuffer);
             layout.finishLine();
         }

         if (layout.currentLine->runs.size() == 0) {
             layout.lines.pop_back();
         }

     }

     layout.applyPadding();

     layout.offset = layout.bounds.origin();

     //TIDEBUG("finished layout: " << layout);
}

