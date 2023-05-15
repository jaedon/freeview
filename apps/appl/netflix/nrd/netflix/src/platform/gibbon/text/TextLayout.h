/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTLAYOUT_H_
#define TEXTLAYOUT_H_

#include <nrdbase/tr1.h>
#include <nrdbase/Log.h>
#include <nrdbase/Variant.h>
#include <nrd/AppLog.h>
#include <Point.h>
#include <Rect.h>
#include <Style.h>
#include <string>
#include <vector>
#include "itemizer/TextItemizer.h"

#define TDEBUG(s) Log::warn(TRACE_GIBBON_TEXT) << s

namespace netflix {
namespace gibbon {

struct TextLayoutRun {

    enum Type {
        TYPE_TEXT,
        TYPE_SPACE,
        TYPE_ELLIPSIS
    };

    netflix::gibbon::Rect bounds;
    Coordinate originalHeight;
    std::string text;
    netflix::gibbon::Style::SharedPtr style;
    int charIndex;
    int endCharIndex;
    Type type;

    TextLayoutRun() : bounds(0, 0, 0, 0), originalHeight(0), text(""), style(), charIndex(0), endCharIndex(0), type(TYPE_TEXT) { }
    TextLayoutRun(const Rect& _bounds, const std::string& _text, netflix::gibbon::Style::SharedPtr _style, const int _charIndex, const Type _type = TYPE_TEXT) : bounds(_bounds), originalHeight(_bounds.height), text(_text), style(_style), charIndex(_charIndex), endCharIndex(0), type(_type) { }

    void clear();
    Rect getCharRect(const int charIndex, int &actualCharIndex, const Coordinate defaultWidth = CoordinateMax) const;
    int getCharIndex(const Point &point) const;

    Variant toVariant() const;
};

struct TextLayoutLine {
    netflix::gibbon::Rect bounds;
    std::vector<TextLayoutRun> runs;
    Coordinate maxAscent;
    Coordinate fixedHeight;
    int charIndex;
    int endCharIndex;
    Point firstOrigin;

    TextLayoutLine() : bounds(0, 0, 0, 0), maxAscent(0), fixedHeight(CoordinateMax), charIndex(0), endCharIndex(0) { }

    void clear();
    void addRun(const TextLayoutRun &run);
    const Point end();
    void alignRuns();
    void calculateBounds();
    void updateRunsPosition(Coordinate deltaX, Coordinate deltaY);

    Variant toVariant() const;
};

struct TextLayout {
    netflix::gibbon::Rect bounds; // text's logical bounds
    netflix::gibbon::Rect desiredBounds; // the bounds passed to us
    std::vector<TextLayoutLine> lines;
    TextLayoutLine currentLine;
    Coordinate currentMaxHeight;
    netflix::gibbon::Style::SharedPtr style;
    TextLayoutLine* previousLine;
    Coordinate lineHeight;
    Point currentPosition;
    Point offset;
    std::string text;
    bool firstWord;
    //tiLayout hack;

    TextLayout() : bounds(0, 0, 0, 0), currentMaxHeight(0), style(), previousLine(NULL), lineHeight(0), currentPosition(0, 0), offset(CoordinateMax, CoordinateMax), firstWord(false), mFinished(false), mHasBottomLayer(false), mWasTruncated(false), mFirstLineProcessing(true), mFirstLine(0), mLineCounter(0), mRenderingByCharacter(false) { }

    void calculateBounds();
    void addLine(const TextLayoutLine &line);
    void move(Coordinate deltaX, Coordinate deltaY);

    inline bool hasBottomLayer() const { return mHasBottomLayer; }
    inline bool isFinished() const { return mFinished; }
    inline bool wasTruncated() const { return mWasTruncated; }
    inline int getFirstLine() const { return mFirstLine; }
    inline int getTotalLines() const { return lines.size(); }
    inline int getLineCounter() const { return mLineCounter; }
    inline bool getFirstLineProcessing() const { return mFirstLineProcessing; }
    inline bool getRenderingByCharacter() const { return mRenderingByCharacter; }

    inline bool isTruncating() const { return !style->getTextTruncation().empty() || style->getMaxLines() != 0; }

    void setIsFinished(bool value) { mFinished = value; }
    void setHasBottomLayer(bool value) { mHasBottomLayer = value; }
    void setWasTruncated(bool value) { mWasTruncated = value; }
    void setFirstLine(int value) { if (value < 0 ) mFirstLine = 0; else mFirstLine = value; }
    void setFirstLineProcessing(bool value) { mFirstLineProcessing = value; }
    void incrementLineCounter(int value = 1) { mLineCounter += value; }
    void setRenderingByCharacter(bool value) { mRenderingByCharacter = value; }

    bool realign(const Size &size);

    Variant toVariant() const;

private:
    bool mFinished;
    bool mHasBottomLayer;
    bool mWasTruncated;
    bool mFirstLineProcessing;
    int mFirstLine;
    int mLineCounter;
    bool mRenderingByCharacter;
};

inline Variant TextLayoutRun::toVariant() const
{
    Variant data;
    data["text"] = text;
    data["x"] = bounds.x;
    data["y"] = bounds.y;
    data["width"] = bounds.width;
    data["height"] = bounds.height;
    data["charIndex"] = charIndex;
    data["endCharIndex"] = endCharIndex;
    data["type"] = type;
    data["style"] = style->toVariant();
    return data;
}

inline std::ostream &operator<< (std::ostream &out, const TextLayoutRun& run)
{
    out << "|" << run.text.c_str() << "| " << run.bounds << " (" << run.charIndex << " - " << run.endCharIndex << ") ";

    switch(run.type) {
    case TextLayoutRun::TYPE_TEXT:
        out << "T";
        break;
    case TextLayoutRun::TYPE_ELLIPSIS:
        out << "E";
        break;
    default:
        out << "-";
        break;
    }

    out << " " << simpleStyle(run.style);

    return out;
}

inline Variant TextLayoutLine::toVariant() const
{
    Variant data;
    data["x"] = bounds.x;
    data["y"] = bounds.y;
    data["width"] = bounds.width;
    data["height"] = bounds.height;
    data["charIndex"] = charIndex;
    data["endCharIndex"] = endCharIndex;

    Variant runData;
    std::vector<TextLayoutRun>::const_iterator it = runs.begin();
    const std::vector<TextLayoutRun>::const_iterator end = runs.end();
    while (it != end) {
        runData.push_back(it->toVariant());
        ++it;
    }
    data["runs"] = runData;

    return data;
}

inline std::ostream &operator<< (std::ostream &out, const TextLayoutLine& line)
{
    std::vector<TextLayoutRun>::const_iterator it = line.runs.begin();
    std::vector<TextLayoutRun>::const_iterator end = line.runs.end();

    out << "Runs: " << line.runs.size() << " " << line.bounds << " (" << line.charIndex << " - " << line.endCharIndex << ")" << std::endl;

    for (; it != end; it++) {
        out << "   " << *it << std::endl;
        out.flush();
    }

    return out;
}

inline Variant TextLayout::toVariant() const
{
    Variant data;
    data["x"] = bounds.x;
    data["y"] = bounds.y;
    data["width"] = bounds.width;
    data["height"] = bounds.height;

    Variant lineData;
    std::vector<TextLayoutLine>::const_iterator it = lines.begin();
    const std::vector<TextLayoutLine>::const_iterator end = lines.end();
    while (it != end) {
        lineData.push_back(it->toVariant());
        ++it;
    }
    data["lines"] = lineData;

    return data;
}

inline std::ostream &operator<< (std::ostream &out, const TextLayout& layout)
{
    out << std::endl << "Text layout - lines: " << layout.lines.size() << " " << layout.bounds << std::endl;
    out << layout.style << std::endl;

    int i = 1;
    std::vector<TextLayoutLine>::const_iterator it = layout.lines.begin();
    std::vector<TextLayoutLine>::const_iterator end = layout.lines.end();

    while (it != end) {
        out << "[" << std::setfill('0') << std::setw(2) << i << "] " << *it;
        ++it;
        ++i;
    }

    return out;
}

}} // namespace netflix::gibbon

#endif /* TEXTLAYOUT_H_ */
