/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef STYLE_H
#define STYLE_H

#include <nrdbase/tr1.h>
#include <nrdbase/Variant.h>
#include <Font.h>
#include "Color.h"
#include "Rect.h"
#include "Shadow.h"
#include "Padding.h"
#include "TextTruncation.h"
#include "EdgeEffect.h"
#include "TextCursor.h"
#include "Typography.h"
#include <string>
#include <nrdbase/StdInt.h>
#include <gibbon/config.h>

namespace netflix {
namespace gibbon {

class Style : public enable_shared_from_this<Style>
{
public:
    typedef shared_ptr<Style> SharedPtr;

    Style();
    ~Style();

    Style::SharedPtr clone() const
    {
        Style::SharedPtr copy;
        copy.reset(new Style(*this));
        return copy;
    }

    enum Align
    {
        //HORIZONTAL
        ALIGN_LEFT = 0x01,
        ALIGN_RIGHT = 0x02,
        ALIGN_CENTER_HORIZONTAL = 0x04,
        //VERTICAL
        ALIGN_TOP = 0x10,
        ALIGN_BOTTOM = 0x20,
        ALIGN_CENTER_VERTICAL = 0x40
    };

    inline std::string describe() const {
        if(mDescription.empty()) {
            std::ostringstream stream;
            stream << getFontFamily() << ":" << getFontPixelSize() << ":" << (int)getFontWeight() << ":" << (int)getFontStyle() << ":"
                   << getAlign() << ":" << getMaxLines() << ":" << getLineHeight() << ":" << (int)getWrap() << ":" << getIndent() << ":"
                   << getTextTruncation().toString() << ":" << getShadow().toString() << ":" << getColor().toString(false) << ":" << getBackgroundColor().toString(false) << ":"
                   << getBackgroundColor().toString(false) << ":" << (int)getRichText() << ":" << getScript() << ":" <<(int)getDirection() << ":" <<(int)getNeedsPDI() << ":"
                   << (int)getUnderline() << ":" << (int)getStrikethrough() << ":" << getEdgeEffect().toString() << ":" << getLocale() << ":" << getPadding().toString() << ":"
                   << getTextCursor().toString() << ":" << getTypography().toString();
            mDescription = stream.str();
        }
        return mDescription;
    }

    uint8_t diff(Style::SharedPtr style) const;

    void setFontFamily(std::string family);
    inline std::string getFontFamily() const { return mFamily; }

    void setLineHeight(Coordinate lineHeight);
    inline Coordinate getLineHeight() const { return mLineHeight; }

    void setFontPixelSize(uint32_t pxSize);
    inline uint32_t getFontPixelSize() const { return mPixelSize; }

    void setFontWeight(Font::Weight weight);
    inline Font::Weight getFontWeight() const { return mWeight; }

    void setFontStyle(Font::Style style);
    inline Font::Style getFontStyle() const { return mStyle; }

    inline void setRichText(bool richText) { mRichText = richText; dirtyDescription(); }
    inline bool getRichText() const { return mRichText; }

    inline void setDirection(int direction) { mDirection = direction; dirtyDescription(); }
    inline int getDirection() const { return mDirection; }

    inline void setNeedsPDI(int needs) { mNeedsPDI = needs; dirtyDescription(); }
    inline int getNeedsPDI() const { return mNeedsPDI; }

    inline void setColor(const Color& c) { mColor = c; dirtyDescription(); }
    inline Color getColor() const { return mColor; }

    inline void setBackgroundColor(const Color& c) { mBackgroundColor = c; dirtyDescription(); }
    inline Color getBackgroundColor() const { return mBackgroundColor; }

    inline void setShadow(const Shadow& s) { mShadow = s; dirtyDescription(); }
    inline Shadow getShadow() const { return mShadow; }

    inline void setPadding(const Padding& p) { mPadding = p; dirtyDescription(); }
    inline Padding getPadding() const { return mPadding; }

    inline void setTypography(const Typography& t) { mTypography = t; dirtyDescription(); }
    inline Typography getTypography() const { return mTypography; }

    inline void setUnderline(bool underline) { mUnderline = underline; dirtyDescription(); }
    inline bool getUnderline() const { return mUnderline; }

    inline void setStrikethrough(bool strike) { mStrikethrough = strike; dirtyDescription(); }
    inline bool getStrikethrough() const { return mStrikethrough; }

    inline void setTextTruncation(const TextTruncation& t) { mTruncation = t; dirtyDescription(); }
    inline TextTruncation getTextTruncation() const { return mTruncation; }

    inline void setTextCursor(const TextCursor& c) { mCursor = c; dirtyDescription(); }
    inline TextCursor getTextCursor() const{ return mCursor; }

    inline void setEdgeEffect(const EdgeEffect& e) { mEdgeEffect = e; dirtyDescription(); }
    inline EdgeEffect getEdgeEffect() const { return mEdgeEffect; }

    inline void setAlign(uint8_t align) { mAlign = align; dirtyDescription(); }
    inline uint8_t getAlign() const { return mAlign; }

    inline void setWrap(bool wrap) { mWrap = wrap; dirtyDescription(); }
    inline bool getWrap() const { return mWrap; }

    inline void setMaxLines(unsigned maxLines) { mMaxLines = maxLines; dirtyDescription(); }
    inline unsigned getMaxLines() const { return mMaxLines; }

    void setIndent(Coordinate indent) { mIndent = indent; dirtyDescription(); }
    inline Coordinate getIndent() const { return mIndent; }

    inline bool isVisible() const { return mColor.a || mShadow.isVisible(); }

    inline void setRise(Coordinate amount) { mRise = amount; dirtyDescription(); }
    inline Coordinate getRise() const { return mRise; }

    void setFont(Font::SharedPtr font);
    Font::SharedPtr getFont() const;

    void setLocale(const std::string& locale);
    inline const std::string& getLocale() const { return mLocale; }

    inline void dirtyFont() { mFont.reset(); dirtyDescription(); }
    inline void dirtyDescription() { mDescription.clear(); }

    void setScript(const std::string& script) { mScript = script; dirtyDescription(); }
    inline const std::string& getScript() const { return mScript; }

    Variant toVariant() const;

    inline bool operator==(const Style& s) const {
        return this->describe() == s.describe();
    }

    inline bool operator!=(const Style& s) const {
        return !(*this == s);
    }

    inline bool equals(const Style& s) const {
        return *this == s;
    }

    inline bool equals(const Style::SharedPtr& s) const {
        return *this == *s.get();
    }

protected:
    bool mWrap;
    std::string mFamily;
    uint32_t mPixelSize;
    Coordinate mLineHeight;
    Coordinate mIndent;
    Font::Weight mWeight;
    Font::Style mStyle;
    Color mColor;
    Color mBackgroundColor;
    Shadow mShadow;
    Padding mPadding;
    Typography mTypography;
    TextTruncation mTruncation;
    TextCursor mCursor;
    EdgeEffect mEdgeEffect;
    uint8_t mAlign;
    unsigned mMaxLines;
    bool mUnderline;
    bool mStrikethrough;
    bool mRichText;
    int mDirection;
    bool mNeedsPDI;
    Coordinate mRise;
    std::string mLocale;
    mutable Font::SharedPtr mFont;
    mutable std::string mDescription;
    std::string mScript;
    static Style::SharedPtr sDefaultStyle;
};

inline std::ostream& operator<< (std::ostream& out, const Font::Weight weight)
{
    out << "Bold: ";
    switch (weight) {
    case Font::WEIGHT_NORMAL:
        out << "no";
        break;
    case Font::WEIGHT_BOLD:
        out << "yes";
        break;
    default:
        break;
    }
    return out;
}

inline std::ostream& operator<< (std::ostream& out, const Font::Style variant)
{
    out << "Variant: ";
    switch (variant) {
    case Font::STYLE_NORMAL:
        out << "normal";
        break;
    case Font::STYLE_ITALIC:
        out << "italic";
        break;
    default:
        break;
    }
    return out;
}

inline Variant Style::toVariant() const
{
    Variant data;
    data["ptr"] = const_cast<Style*>(this);
    data["family"] = mFamily;
    data["wrap"] = mWrap;
    data["pixelSize"] = mPixelSize;
    data["lineHeight"] = mLineHeight;
    data["indent"] = mIndent;
    data["weight"] = mWrap;
    data["style"] = mStyle;
    data["color"] = mColor.toVariant();
    data["backgroundColor"] = mBackgroundColor.toVariant();
    data["shadow"] = mShadow.toVariant();
    data["padding"] = mPadding.toVariant();
    data["typography"] = mTypography.toVariant();
    data["truncation"] = mTruncation.toVariant();
    data["cursor"] = mCursor.toVariant();
    data["edgeEffect"] = mEdgeEffect.toVariant();
    data["align"] = mAlign;
    data["maxLines"] = mMaxLines;
    data["underline"] = mUnderline;
    data["strikethrough"] = mStrikethrough;
    data["richText"] = mRichText;
    data["direction"] = mDirection;
    data["needsPDI"] = mNeedsPDI;
    data["rise"] = mRise;
    data["locale"] = mLocale;
    data["script"] = mScript;
    return data;
}

struct simpleStyle {
    const Style::SharedPtr mStyle;
    simpleStyle(const Style::SharedPtr style) : mStyle(style) { };
};


inline std::ostream& operator<< (std::ostream& out, const simpleStyle value)
{
    const Style::SharedPtr style = value.mStyle;
    out << "Style: " << style.get() << " {";
    out << "(" << style->getFontFamily() << ", " << style->getFontPixelSize() << ", " << style->getFontWeight() << ", " << style->getFontStyle() << ") ";
    out << "(" << style->getColor() << ", U:" << style->getUnderline() << ", S:" << style->getStrikethrough() << ")";
    {
        std::string align_desc;
        if(style->getAlign() & Style::ALIGN_LEFT) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "LEFT";
        }
        if(style->getAlign() & Style::ALIGN_RIGHT) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "RIGHT";
        }
        if(style->getAlign() & Style::ALIGN_CENTER_HORIZONTAL) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "H_CENTER";
        }
        if(style->getAlign() & Style::ALIGN_TOP) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "TOP";
        }
        if(style->getAlign() & Style::ALIGN_BOTTOM) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "BOTTOM";
        }
        if(style->getAlign() & Style::ALIGN_CENTER_VERTICAL) {
            if(!align_desc.empty()) align_desc += "|";
            align_desc += "V_CENTER";
        }
        out << "Align:" << align_desc;
    }
    out << "}";
    return out;
}

inline std::ostream& operator<< (std::ostream& out, const Style::SharedPtr style)
{
    out << "Style {";
    out << simpleStyle(style);
    out << ", (direction=" << style->getDirection() << ", richText=" << style->getRichText() << ", lineHeight=" << style->getLineHeight() << ", indent=" << style->getIndent() <<", maxLines=" << style->getMaxLines() << ", wrap=" << style->getWrap() << "), ";
    out << "(" << style->getShadow() << "), (" << style->getPadding() << "), (" << style->getTextTruncation() << "), (" << style->getEdgeEffect() << "), (" << style->getTextCursor() << ")";
    out << "}";

    return out;
}

}} // namespace netflix::gibbon

#endif // STYLE_H

