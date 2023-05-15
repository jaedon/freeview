/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef __TEXT_H__
#define __TEXT_H__

#include <Rect.h>
#include <TextEngine.h>
#include <DisplayList.h>
#include <GraphicsEngine.h>
#include <itemizer/TextItemizer.h>

#include <nrdbase/tr1.h>
#include <nrdbase/Mutex.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class Widget;
class CursorTimer;

class Text : public enable_shared_from_this<Text>
{
public:
    friend class CursorTimer;

    typedef shared_ptr<Text> SharedPtr;
    typedef weak_ptr<Text> WeakPtr;

    enum Property
    {
        Property_RenderLines          = 0x00010000,
        Property_RenderFirstLine      = 0x00020000,
        Property_RenderCursorPosition = 0x00040000,
        Property_RenderPadding        = 0x00080000,

        Property_Unknown              = 0x00000000
    };

    Text(shared_ptr<Widget> widget = shared_ptr<Widget>());
    ~Text();

    inline shared_ptr<Widget> getWidget() const { return mWidget.lock(); }

    class Listener
    {
    public:
        typedef shared_ptr<Listener> SharedPtr;
        Listener() {}
        virtual ~Listener() {}

        virtual void onAnimationFinished(Property, const AnimationFinish &) { }
        virtual void onPropertiesChanged(unsigned int) { }
    };
    void addListener(const Listener::SharedPtr &listener);
    void removeListener(const Listener::SharedPtr &listener);

    Size sizeContent(const Size &maxSize) const;

    void render(GraphicsEngine *graphics, const Rect &globalRect);

    unsigned int purge();

    inline bool hasText() const { return mTextStyle && !mText.empty(); }
    inline bool hasTextCursor() const { return mTextStyle && mTextStyle->isVisible() &&
            getRenderTextCursorPosition() != INT_MAX &&
            !mTextStyle->getTextCursor().empty(); }
    inline bool hasContent() const { return (hasText() && mTextStyle->isVisible()) || hasTextCursor(); }

    std::string describe() const;

    void setText(const std::string& text);
    inline std::string getText() const { return mText; }

    inline Style::SharedPtr getTextStyle() const { return mTextStyle; }
    inline const tiLayout& getTextLayout() const { return mTextCache.layout.layout; }
    const tiLayout& getTextLayout(const Size& size) const;

    void updateCursor() const;
    inline int getRenderTextCursorPosition() const { return mTextCache.cursor.position; }
    inline int getTextCursorPosition() const { return mTextCursorPosition; }
    void setTextCursorPosition(int position);
    inline Rect getTextCursorBounds() const { return mTextCache.cursor.bounds; }

    inline int getRenderTextFirstLine() const { return mRenderTextFirstLine; }
    inline int getRenderTextLines() const { return getTextLayout().lines.size(); }

    inline int getTextFirstLine() const { return mTextFirstLine; }
    void setTextFirstLine(int position);

    void setFontFamily(std::string family);
    inline std::string getFontFamily() const { return mTextStyle->getFontFamily(); }

    void setLineHeight(Coordinate lineHeight);
    inline Coordinate getLineHeight() const { return mTextStyle->getLineHeight(); }

    void setIndent(Coordinate indent);
    inline Coordinate getIndent() const { return mTextStyle->getIndent(); }

    void setFontPixelSize(uint32_t pxSize);
    inline uint32_t getFontPixelSize() const { return mTextStyle->getFontPixelSize(); }

    void setFontWeight(Font::Weight weight);
    inline Font::Weight getFontWeight() const { return mTextStyle->getFontWeight(); }

    void setFontStyle(Font::Style style);
    inline Font::Style getFontStyle() const { return mTextStyle->getFontStyle(); }

    void setRichText(bool richText);
    inline bool getRichText() const { return mTextStyle->getRichText(); }

    void setColor(const Color& c);
    inline Color getColor() const { return mTextStyle->getColor(); }

    void setBackgroundColor(const Color& c);
    inline Color getBackgroundColor() const { return mTextStyle->getBackgroundColor(); }

    void setShadow(const Shadow& s);
    inline Shadow getShadow() const { return mTextStyle->getShadow(); }

    void setPadding(const Padding& s);
    inline Padding getPadding() const { return mTextStyle->getPadding(); }

    void setTypography(const Typography& t);
    inline Typography getTypography() const { return mTextStyle->getTypography(); }

    void setUnderline(bool underline);
    inline bool getUnderline() const { return mTextStyle->getUnderline(); }

    void setStrikethrough(bool strike);
    inline bool getStrikethrough() const { return mTextStyle->getStrikethrough(); }

    void setTextTruncation(const TextTruncation& t);
    inline TextTruncation getTextTruncation() const { return mTextStyle->getTextTruncation(); }

    void setTextCursor(const TextCursor& c);
    inline TextCursor getTextCursor() const{ return mTextStyle->getTextCursor(); }

    void setEdgeEffect(const EdgeEffect& e);
    inline EdgeEffect getEdgeEffect() const { return mTextStyle->getEdgeEffect(); }

    void setAlign(uint8_t align);
    inline uint8_t getAlign() const { return mTextStyle->getAlign(); }

    void setWrap(bool wrap);
    inline bool getWrap() const { return mTextStyle->getWrap(); }

    void setMaxLines(unsigned maxLines);
    inline unsigned getMaxLines() const { return mTextStyle->getMaxLines(); }

    void setLocale(const std::string& locale);
    inline const std::string& getLocale() const { return mTextStyle->getLocale(); }

    void setDirection(int direction);
    inline unsigned getDirection() const { return mTextStyle->getDirection(); }

    void setScript(const std::string& direction);
    inline const std::string& getScript() const { return mTextStyle->getScript(); }

    enum DumpItems
    {
        Dump_Default                  = 1 << 0,
        Dump_Attributes               = 1 << 1,
        Dump_Direction                = 1 << 2,
        Dump_Scripts                  = 1 << 3,
        Dump_Locale                   = 1 << 4,
        Dump_WordWrap                 = 1 << 5,
        Dump_Final                    = 1 << 6,
        Dump_All                      = 0xffffffff
    };

    void dump(unsigned int flags=0);
    std::map<std::string, std::vector<Rect> > dumpRectangles(int line, int column, bool all, int type, const std::string& key);

    Variant toVariant() const;

protected:
    void needsCursor() const;
    void needsRender() const;
    void needsItemize() const { needsLayout(true); }
    void needsLayout(bool itemize=false) const;
    void propertiesChanged_internal(unsigned int properties) const;

    typedef std::set<Listener::SharedPtr> ListenerContainer;
    ListenerContainer mListeners;

    weak_ptr<Widget> mWidget;
    mutable struct TextCache {
        enum { DIRTY_ITEMIZE = 0x01, DIRTY_LAYOUT = 0x02, DIRTY_MAXLAYOUT = 0x04 };
        mutable struct TextLayoutCache {
            uint8_t dirty;
            TextItemizerData itemized;
            tiLayout layout, maxLayout;
            Style::SharedPtr style;
            Size cachedSize;
            int hits, misses, count, itemizes;
            inline TextLayoutCache() : dirty(DIRTY_ITEMIZE|DIRTY_LAYOUT|DIRTY_MAXLAYOUT), hits(0), misses(0), count(0), itemizes(0) {}
        } layout;
        mutable struct TextCursorCache {
            uint32_t position;
            Rect bounds;
            inline TextCursorCache() : position(INT_MAX) {}
        } cursor;
        Rect rect;
        DisplayList::SharedPtr displayList;
    } mTextCache;
    std::string mText;
    Style::SharedPtr mTextStyle;
    mutable shared_ptr<CursorTimer> mTextCursorTimer;
    int mTextFirstLine;
    int mTextCursorPosition;
    mutable int mRenderTextFirstLine;
    mutable bool mNeedsRenderTextFirstLine;

    static Mutex sCursorLock;
};

}} // namespace netflix::gibbon

#endif /* __TEXT_H__ */
