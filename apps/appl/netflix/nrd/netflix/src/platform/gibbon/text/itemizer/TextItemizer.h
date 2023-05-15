/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextItemizer_H_
#define TextItemizer_H_

#include "TextItemizerCommon.h"
#include "TextAttributesItemizer.h"
#include "TextFontItemizer.h"
#include "TextDirectionItemizer.h"
#include "TextScriptItemizer.h"
#include "TextLocaleItemizer.h"
#include "TextWordBreakItemizer.h"

#include <nrdbase/Noncopyable.h>
#include <string>
#include <Style.h>
#include <nrdbase/tr1.h>

#include <hb.h>
#include <Debug.h>
#include <GibbonHarfbuzz.h>
#include <nrdbase/Backtrace.h>

namespace netflix {
namespace gibbon {

class GlyphString : public Noncopyable {
public:
    typedef shared_ptr<GlyphString> SharedPtr;
    GlyphString(hb_buffer_t* _buffer) : mBuffer(_buffer) {}
    ~GlyphString() {
        hb_buffer_destroy(mBuffer);
    }

    inline hb_buffer_t* buffer() const { return mBuffer; }
    inline unsigned int bufferLength() const {
        return hb_buffer_get_length(mBuffer);
    }

    hb_glyph_info_t* getGlyphInfos() const { return hb_buffer_get_glyph_infos(mBuffer, 0); }
    hb_glyph_position_t* getGlyphPositions() const { return hb_buffer_get_glyph_positions(mBuffer, 0); }

    // Find the harfbuzz buffer index for a string index
    enum IndexMode { Character, Boundary };
    uint32_t getBufferIndexForStringIndex(uint32_t index, IndexMode) const;

    uint32_t rtrim(uint32_t start, uint32_t end, Font::SharedPtr font);
    uint32_t ltrim(uint32_t start, uint32_t end, Font::SharedPtr font);

    Variant toVariant() const;

private:
    hb_buffer_t* mBuffer;
};


class TextItem {
public:
    typedef shared_ptr<TextItem> SharedPtr;
    TextItem(TextAttributeRun::SharedPtr _attribute, hb_buffer_t* _buffer) : attribute(_attribute), glyphString(_buffer) {};

    TextAttributeRun::SharedPtr attribute;
    GlyphString& getGlyphString() { return glyphString; }
    const GlyphString& getGlyphString() const { return glyphString; }

    Variant toVariant() const;
private:
    GlyphString glyphString;
};

struct TextItemizerData {
    icu::UnicodeString text;
    Style::SharedPtr style;
    std::vector<TextItem::SharedPtr> items;
    TextWordBreakRuns wordBreaks;

#ifdef BUILD_DEBUG
    // Temporary objects only on Debug builds
    TextAttributeRuns attributeRuns;
    TextDirectionRuns directionRuns;
    TextScriptRuns scriptRuns;
    TextLocaleRuns localeRuns;
    TextAttributeRuns finalRuns;
#endif

    void addItem(TextAttributeRun::SharedPtr attr, hb_buffer_t* hb_buffer) {
        items.push_back(TextItem::SharedPtr(new TextItem(attr, hb_buffer)));
    }

    TextItemizerData() : hasBottomLayer(false) {}
#if CAPTURE_COPY_CONSTRUCTORS
    TextItemizerData(const TextItemizerData&) : hasBottomLayer(false) {
        assert(false);
    }
#endif

    bool hasBottomLayer;
};

struct LayoutContext : public Noncopyable {
    // Initial state
    Rect* desiredBounds;
    Style::SharedPtr baseStyle;

    // Working variables
    Point currentPosition;
    Point previousPosition;
    Rect currentLogical;
    Rect previousLogical;
    Rect currentInk;
    Rect previousInk;
    Coordinate maxWidth;
    size_t lines;

    // New Lines
    Coordinate newLineHeight;
    uint32_t newLineBuffer;

    // Options
    bool firstWord;

    LayoutContext() : desiredBounds(0), maxWidth(0), lines(0), newLineHeight(0), newLineBuffer(0), firstWord(true) {}
    LayoutContext(Rect* bounds, const Style::SharedPtr& style) : desiredBounds(bounds), baseStyle(style), currentPosition(bounds->origin()), maxWidth(bounds->width), lines(0), newLineHeight(0), newLineBuffer(0), firstWord(true) {}

    void clear() {
        baseStyle.reset();
        currentPosition.clear();
        previousPosition.clear();
        currentLogical.clear();
        previousLogical.clear();
        currentInk.clear();
        previousInk.clear();
        maxWidth = 0;
        lines = 0;

        firstWord = true;
    }
};

struct tiRun {
    Rect inkBounds;
    Rect logicalBounds;
    TextItem::SharedPtr item;
    Coordinate originalHeight;
    Coordinate originalY;
    Coordinate originalInkY;
    int start;
    int end;
    uint32_t lastWordMark;

    Variant toVariant() const;

    tiRun() : originalHeight(0), originalY(0), originalInkY(0), start(0), end(0), lastWordMark(INT_MAX) {}
};

struct tiLine {
    Rect inkBounds;
    Rect logicalBounds;
    Rect bounds;
    std::vector<tiRun> runs;
    Coordinate maxAscent;
    Coordinate maxInkAscent;
    bool firstLine;

    tiLine() : maxAscent(0), maxInkAscent(0), firstLine(false) {}

    //void addRun(tiRun& run);
    void addRun(LayoutContext& context, const TextItem::SharedPtr item, uint32_t start, uint32_t end, uint32_t lastWordMark = INT_MAX);
    void updateRunsPosition(Coordinate deltaX, Coordinate deltaY, Coordinate deltaW = 0, Coordinate deltaH = 0);
    //void alignRuns(Coordinate topY, const Style::SharedPtr& style, Coordinate x, Coordinate width);
    void alignRuns(Coordinate baseY);

    tiRun* getNewRun();
    tiRun* copyNewRun(const tiRun& run);

    Variant toVariant() const;

    void rtrim(bool applyTracking = true);

#if CAPTURE_COPY_CONSTRUCTORS
    tiLine() : maxAscent(0), maxInkAscent(0) {}
    tiLine(const tiLine& /*old*/) : maxAscent(0) , maxInkAscent(0) {
        TIDEBUG("copying line");
        //TIDEBUG(Backtrace::backtrace()->toString());
    }
#endif
};

struct tiLayout {
    Rect desiredBounds;
    Style::SharedPtr style;

    Rect inkBounds;
    Rect logicalBounds;
    Rect bounds; // total bounds
    Point offset;
    int hpad, vpad;

    std::vector<tiLine> lines;
    tiLine* currentLine;

    bool hasBottomLayer;
    int lineCounter;
    int firstLine;
    int truncated;
    LayoutContext context;

    inline tiLayout()
        : offset(CoordinateMax, CoordinateMax), hpad(0), vpad(0),
          currentLine(0), hasBottomLayer(false), lineCounter(0), firstLine(0),
          truncated(false) {}

    inline tiLayout(Rect& _desiredBounds, Style::SharedPtr baseStyle)
        : desiredBounds(_desiredBounds), style(baseStyle),
          offset(CoordinateMax, CoordinateMax), hpad(0), vpad(0),
          currentLine(0), hasBottomLayer(false),
          lineCounter(0), firstLine(0), truncated(false),
          context(&_desiredBounds, baseStyle) {}


    inline void setBackgroundColor(const Color &color) {
        if(style)
            style->setBackgroundColor(color);
        for(std::vector<tiLine>::iterator lit = lines.begin(); lit != lines.end(); ++lit) {
            tiLine &line = (*lit);
            for(std::vector<tiRun>::iterator it = line.runs.begin(); it != line.runs.end(); ++it)
                (*it).item->attribute->style->setBackgroundColor(color);
        }
        hasBottomLayer = true;
    }

    inline void setColor(const Color &color) {
        if(style)
            style->setColor(color);
        for(std::vector<tiLine>::iterator lit = lines.begin(); lit != lines.end(); ++lit) {
            tiLine &line = (*lit);
            for(std::vector<tiRun>::iterator it = line.runs.begin(); it != line.runs.end(); ++it)
                (*it).item->attribute->style->setColor(color);
        }
    }

    inline void setEdgeEffect(const EdgeEffect &e) {
        if(style)
            style->setEdgeEffect(e);
    }

    inline void setShadow(const Shadow &s) {
        if(style)
            style->setShadow(s);
    }

    inline void setTextCursor(const TextCursor &c) {
        if(style)
            style->setTextCursor(c);
    }

    inline void clear() {
        style.reset();

        desiredBounds.clear();
        inkBounds.clear();
        logicalBounds.clear();
        bounds.clear();
        offset = Point(CoordinateMax, CoordinateMax);
        hpad = vpad = 0;

        lines.clear();
        currentLine = 0;

        hasBottomLayer = false;
        lineCounter = 0;
        firstLine = 0;
        context.clear();
    }


    void move(Coordinate deltaX, Coordinate deltaY, Coordinate deltaW = 0, Coordinate deltaH = 0);
    void calculateBounds() { move(0, 0); }
    bool realign(const Size &size);

    inline int getFirstLine() const { return firstLine; }
    inline void setFirstLine(int value) { if (value < 0 ) firstLine = 0; else firstLine = value; }

    inline int getLineCounter() const { return lineCounter; }
    inline void incrementLineCounter(int value = 1) { lineCounter += value; }
    inline int getTotalLines() const { return lines.size(); }

    inline bool wasTruncated() const { return truncated; }
    inline bool isTruncating() const { return !style->getTextTruncation().empty() || style->getMaxLines() != 0; }

    tiLine *getNewLine();
    inline tiLine *getCurrentLine() { return &lines.back(); }

    // Processing
    void alignLine(tiLine* line, bool useLayoutBounds = false);
    void finishLine(tiLine* line = 0);
    void moveRunsDown();

    Variant toVariant() const;

    void applyPadding();

#if CAPTURE_COPY_CONSTRUCTORS
    inline tiLayout() {}
    inline tiLayout(const tiLayout& /*old*/) { TIDEBUG("copying layout!"); }
#endif
};


class TextItemizer {
public:
    enum LayoutFlags {
        NO_LTRIM = 1 << 0,
        NO_RTRIM = 1 << 1
    };

    static TextItemizerData itemize(const char *utf8Text, const Style::SharedPtr &baseStyle);
    static void layout(const TextItemizerData &data, tiLayout &layout, const Rect& desiredBounds, unsigned int flags = 0);
    static void truncate(const TextItemizerData &data, tiLayout& layout);
    static void truncateLine(const TextItemizerData &data, tiLayout& layout, bool truncateLastLine = false);
};


inline Variant GlyphString::toVariant() const
{
    Variant data;
    data["length"] = bufferLength();
    return data;
}

inline Variant TextItem::toVariant() const
{
    Variant data;
    data["attribute"] = attribute->toVariant();
    data["glyphString"] = glyphString.toVariant();
    return data;
}


inline Variant tiRun::toVariant() const
{
    Variant data;
    data["start"] = start;
    data["end"] = end;
    data["logicalBounds"] = logicalBounds.toVariant();
    data["inkBounds"] = inkBounds.toVariant();
    data["item"] = item->toVariant();
    return data;
}

inline Variant tiLine::toVariant() const
{
    Variant data;
    data["logicalBounds"] = logicalBounds.toVariant();
    data["inkBounds"] = inkBounds.toVariant();

    Variant runData;
    std::vector<tiRun>::const_iterator it = runs.begin();
    const std::vector<tiRun>::const_iterator end = runs.end();
    while (it != end) {
        runData.push_back(it->toVariant());
        ++it;
    }
    data["runs"] = runData;

    return data;
}

inline Variant tiLayout::toVariant() const
{
    Variant data;
    data["bounds"] = bounds.toVariant();
    data["logicalBounds"] = logicalBounds.toVariant();
    data["inkBounds"] = inkBounds.toVariant();

    Variant lineData;
    std::vector<tiLine>::const_iterator it = lines.begin();
    const std::vector<tiLine>::const_iterator end = lines.end();
    while (it != end) {
        lineData.push_back(it->toVariant());
        ++it;
    }
    data["lines"] = lineData;

    return data;
}

inline hb_buffer_serialize_flags_t getSerializeFlags()
{
    unsigned int flags = HB_BUFFER_SERIALIZE_FLAG_DEFAULT;
    if (!Debug::DebugHarfbuzzClusters)
        flags |= HB_BUFFER_SERIALIZE_FLAG_NO_CLUSTERS;
    if (!Debug::DebugHarfbuzzPositions)
        flags |= HB_BUFFER_SERIALIZE_FLAG_NO_POSITIONS;
    if (!Debug::DebugHarfbuzzNames)
        flags |= HB_BUFFER_SERIALIZE_FLAG_NO_GLYPH_NAMES;

    return static_cast<hb_buffer_serialize_flags_t>(flags);
}

inline std::string hbToString(hb_buffer_t* buffer, hb_font_t* font, unsigned int start = 0, unsigned int end = INT_MAX)
{
    char textBuffer[4096];
    unsigned int num_glyphs;

    if (end == INT_MAX)
        num_glyphs = hb_buffer_get_length(buffer);
    else
        num_glyphs = end + 1;

    if (num_glyphs > hb_buffer_get_length(buffer))
        num_glyphs = hb_buffer_get_length(buffer);

    hb_buffer_serialize_flags_t flags = getSerializeFlags();

    while(start < num_glyphs) {
        unsigned int consumed;

        start += hb_buffer_serialize_glyphs(buffer, start, num_glyphs, textBuffer, sizeof(textBuffer), &consumed,
                                            font,
                                            HB_BUFFER_SERIALIZE_FORMAT_TEXT, flags);
       if (!consumed)
           break;
    }
    return std::string(textBuffer);
}


inline std::string hbToString(const TextItem& item, unsigned int start = 0, unsigned int end = INT_MAX)
{
    return hbToString(item.getGlyphString().buffer(), item.attribute->style->getFont()->getFreetype()->harfbuzzFont(), start, end);
}

inline std::ostream &operator<< (std::ostream &out, const TextItem& item)
{
    out << item.attribute->start << "-" << item.attribute->end << " [" << hbToString(item) << "]";

    return out;
}

inline std::ostream &operator<< (std::ostream &out, const TextItem* item)
{
    out << (*item);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const tiRun& run)
{
    out << "Text run:";

    out << "  Logical: " << run.logicalBounds << " - Ink: " << run.inkBounds << " | Ascent: " << run.originalY << std::endl;
    if (run.lastWordMark != INT_MAX) {
        out << "           Last word mark present: " << run.lastWordMark << std::endl;
    }
    out << "           Buffer offsets: " << run.start << " - " << run.end << std::endl;
    out << "           Buffer contents: " << hbToString(run.item->getGlyphString().buffer(), run.item->attribute->style->getFont()->getFreetype()->harfbuzzFont(), run.start, run.end);

    return out;
}

inline std::ostream &operator<< (std::ostream &out, const tiLine& line)
{
    std::vector<tiRun>::const_iterator it = line.runs.begin();
    std::vector<tiRun>::const_iterator end = line.runs.end();

    out << "Line: " << line.bounds <<" - Logical: " << line.logicalBounds << " - Ink: " << line.inkBounds << std::endl;

    int i = 0;
    for (; it != end; it++) {
        out << "      [" << std::setfill('0') << std::setw(2) << i << "] " << *it << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

inline std::ostream &operator<< (std::ostream &out, const tiLine* line)
{
    out << (*line);
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const tiLayout& layout)
{
    std::vector<tiLine>::const_iterator it = layout.lines.begin();
    std::vector<tiLine>::const_iterator end = layout.lines.end();

    out << "Itemizer layout - Logical: " << layout.logicalBounds << " - Ink: " << layout.inkBounds << std::endl;
    out << "                   Bounds: " << layout.bounds << " - Desired: " << layout.desiredBounds << std::endl;
    out << "                  Padding: [" <<layout.hpad << "x" << layout.vpad << "] - Indent: " << layout.style->getIndent() << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << *it << std::endl;
        ++i;
        out.flush();
    }

    return out;
}


inline std::ostream &operator<< (std::ostream &out, const TextItemizerData& itemizer)
{
    std::vector<TextItem::SharedPtr>::const_iterator it = itemizer.items.begin();
    std::vector<TextItem::SharedPtr>::const_iterator end = itemizer.items.end();

    out << "Text itemizer output for: " << itemizer.text << std::endl;

    int i = 0;
    for (; it != end; it++) {
        icu::UnicodeString text = icu::UnicodeString(itemizer.text, it->get()->attribute->start, it->get()->attribute->end - it->get()->attribute->start + 1);
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << "|" << text << "| " << std::endl;
        out << "      " << *it << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

}}

#endif /* TextItemizer_H_ */
