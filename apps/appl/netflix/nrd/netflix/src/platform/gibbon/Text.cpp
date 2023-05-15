/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "Text.h"
#include "Debug.h"
#include "Widget.h"

#define DEBUG_TEXT_LAYOUT(fmt, ...) if(NRDP_UNLIKELY(Debug::DebugTextLayout)) Log::warn(TRACE_GIBBON_WIDGET, "TEXT_LAYOUT:" fmt, __VA_ARGS__)

using namespace netflix;
using namespace netflix::gibbon;

Mutex Text::sCursorLock(UNTRACKED_MUTEX, "GibbonTextCursor");

NRDP_DEFINE_OBJECTCOUNT(GibbonText);

namespace netflix {
namespace gibbon {

static inline int calculateRenderFirstLine(const tiLayout &layout, const int firstLine)
{
    int result = 0;
    if(!layout.style->getWrap() || firstLine < 0)
        result = 0;
    else if(firstLine > layout.getTotalLines() - 1)
        result = layout.getTotalLines() > 0 ? layout.getTotalLines() - 1 : 0;
    else
        result = firstLine;
    return result;
}

class CursorTimer : public Application::Timer
{
public:
    CursorTimer(Widget::SharedPtr widget) : Application::Timer(-1), mState(true), mWidget(widget) { }

    virtual std::string describe() const { return "CursorTimer"; }
    void timerFired()
    {
        PERFORMANCE_MARKER_SCOPED("cursortimer.fired");
        mState = !mState;
        ScopedMutex _lock(Widget::lock());
        if(Widget::SharedPtr widget = mWidget.lock())
            widget->needsRender(widget->getText()->mTextCache.cursor.bounds);
    }
    bool getState() const { return mState; }
    void setState(bool value) { mState = value; restart(); }
private:
    bool mState;
    Widget::WeakPtr mWidget;
};

}} // netflix::applicaton::gibbon

Text::Text(shared_ptr<Widget> widget) : mWidget(widget),
                                        mTextStyle(new Style),
                                        mTextFirstLine(0), mTextCursorPosition(0),
                                        mRenderTextFirstLine(0),
                                        mNeedsRenderTextFirstLine(false)
{
    NRDP_OBJECTCOUNT_REF(GibbonText, this);
}

Text::~Text()
{
    if(mTextCursorTimer) {
        mTextCursorTimer->stop();
        mTextCursorTimer.reset();
    }
    NRDP_OBJECTCOUNT_DEREF(GibbonText, this);
}

Size Text::sizeContent(const Size &maxSize) const
{
    if(hasText()) {
        const tiLayout &layout = getTextLayout(maxSize);
        return layout.bounds.size();
    }
    return Size(0, 0);
}

void Text::render(GraphicsEngine *graphics, const Rect &globalRect)
{
    const Size scaledSize(globalRect.width, globalRect.height);
    const tiLayout& layout = getTextLayout(scaledSize);
    mTextCache.rect = layout.bounds;
    if(getWidget()->getClipEnabled()) { // Clip text rectangle
        if(mTextCache.rect.height > globalRect.height)
            mTextCache.rect.height = globalRect.height;
        if(mTextCache.rect.width > globalRect.width)
            mTextCache.rect.width = globalRect.width;
    }
    graphics->setCompositionMode(GraphicsEngine::CompositionMode_SourceOver);

    DisplayList::SharedPtr displayList = mTextCache.displayList;
    Rect textRect(0, 0, globalRect.width, globalRect.height);
    if(!displayList) {
        mTextCache.displayList.reset(new DisplayList(textRect.width, textRect.height));
        displayList = mTextCache.displayList;
        GraphicsEngine textEngine(displayList);
        TextEngine::drawLayout(&textEngine, layout, textRect);
    }
    assert(displayList);
    graphics->apply(displayList, textRect, globalRect);

    if(mNeedsRenderTextFirstLine) {
        mNeedsRenderTextFirstLine = false;
        if(mTextStyle->getWrap())
            propertiesChanged_internal(Text::Property_RenderFirstLine);
    }

    if(hasTextCursor()) {
        updateCursor();
        if(!mTextCache.cursor.bounds.empty() && (!mTextCursorTimer || mTextCursorTimer->getState()))
            graphics->fill(mTextCache.cursor.bounds + globalRect.origin(), mTextStyle->getTextCursor().color);
    }
}

void Text::setText(const std::string& text)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(text != mText) {
        mText = text;
        needsItemize();
    }
}

const tiLayout& Text::getTextLayout(const Size &size) const
{
    PERFORMANCE_MARKER_SCOPED("text.gettextlayout");
    bool layoutInCache = false;

    if(!(mTextCache.layout.dirty & TextCache::DIRTY_MAXLAYOUT) && size.width == CoordinateMax && size.height == CoordinateMax)
        return mTextCache.layout.maxLayout;

    if(!(mTextCache.layout.dirty & TextCache::DIRTY_LAYOUT) && mTextStyle->diff(mTextCache.layout.style) == TextEngine::DIFF_NONE) {
        int checkLine = 0;
        if(!mTextCache.layout.style->getTextTruncation().empty() && mTextCache.layout.layout.wasTruncated() &&
           (mTextCache.layout.cachedSize.height > size.height && mTextCache.layout.cachedSize.height != CoordinateMax)) {
            checkLine = __LINE__;
            layoutInCache = false;
        } else {
            checkLine = __LINE__;
            layoutInCache = ((mTextCache.layout.cachedSize.width == size.width ||
                              mTextCache.layout.layout.bounds.width == size.width ||
                              (!mTextStyle->getWrap() && mTextCache.layout.cachedSize.width == CoordinateMax)) &&
                             (mTextCache.layout.cachedSize.height >= size.height ||
                              mTextCache.layout.layout.bounds.height <= size.height ||
                              mTextCache.layout.cachedSize.height == CoordinateMax));
        }

        static int hits = 0, misses = 0;
        if(layoutInCache) {
            ++mTextCache.layout.hits;
            ++hits;
        } else {
            ++mTextCache.layout.misses;
            ++misses;
        }
        DEBUG_TEXT_LAYOUT("%s(%d):%d/%d(%d/%d): %s::%s %s[%s]", layoutInCache ? "HIT" : "MISS", checkLine,
                                mTextCache.layout.hits, mTextCache.layout.misses, hits, misses,
                                size.toString().c_str(), mTextCache.layout.cachedSize.toString().c_str(), getWidget()->describe().c_str(), mText.c_str());
    }

    if(layoutInCache && mTextFirstLine != mTextCache.layout.layout.getFirstLine()) {
        if (mTextCache.layout.layout.isTruncating()) {
            // If we are truncating and the first text line has changed, we need to relayout the text
            layoutInCache = false;
        } else {
            // If we are not truncating it's enough with setting the firstLine property of the already calculated layout
            mTextCache.displayList.reset();
            mTextCache.layout.layout.setFirstLine(mTextFirstLine);
            mRenderTextFirstLine = calculateRenderFirstLine(mTextCache.layout.layout, mTextFirstLine);
        }
    }

    if(!layoutInCache) {
        mTextCache.layout.cachedSize = size;
        int firstLine = mTextFirstLine;

        // If we are not truncating, calculate the layout from line 0. We keep it in the cache and change first line only at draw time.
        if(mTextStyle->getTextTruncation().empty() && mTextStyle->getMaxLines() == 0)
            firstLine = 0;

        ++mTextCache.layout.count;
        mTextCache.layout.layout.clear();
        const char *desc = "LAYOUT";
        if(mTextCache.layout.dirty & TextCache::DIRTY_ITEMIZE) {
            mTextCache.layout.itemized = TextItemizer::itemize(mText.c_str(), mTextStyle);
            mTextCache.layout.dirty &= ~TextCache::DIRTY_ITEMIZE;
            ++mTextCache.layout.itemizes;
            desc = "ITEMIZE";
        }
        TextEngine::layoutText(mTextCache.layout.layout, mTextCache.layout.itemized, Rect(0, 0, size), firstLine);
        mTextCache.layout.dirty &= ~TextCache::DIRTY_LAYOUT;
        mTextCache.displayList.reset();

        if(mTextStyle->getTextTruncation().empty() && mTextStyle->getMaxLines() == 0) {
            mTextCache.layout.layout.setFirstLine(mTextFirstLine);
            mRenderTextFirstLine = calculateRenderFirstLine(mTextCache.layout.layout, mTextFirstLine);
        } else {
            mRenderTextFirstLine = mTextCache.layout.layout.getFirstLine();
        }

        mNeedsRenderTextFirstLine = true;
        mTextCache.layout.style = mTextStyle;

        if(mTextStyle->getWrap())
            propertiesChanged_internal(Text::Property_RenderLines);
        propertiesChanged_internal(Text::Property_RenderPadding);
        needsCursor();

        DEBUG_TEXT_LAYOUT("%s: %s::%s %s(%s)", desc, size.toString().c_str(), mTextCache.layout.layout.bounds.toString().c_str(), getWidget()->describe().c_str(), mText.c_str());
    } else if(mTextCache.layout.layout.realign(size)) {
        mTextCache.layout.style = mTextStyle;
        mTextCache.displayList.reset();
        needsCursor();
        DEBUG_TEXT_LAYOUT("REALIGN: %s::%s %s(%s)", size.toString().c_str(), mTextCache.layout.layout.bounds.toString().c_str(), getWidget()->describe().c_str(), mText.c_str());
    }

    if(!(mTextCache.layout.dirty & TextCache::DIRTY_MAXLAYOUT) && size.width == CoordinateMax && size.height && size.height == CoordinateMax) {
        mTextCache.layout.maxLayout.bounds = mTextCache.layout.layout.bounds;
        mTextCache.layout.dirty &= ~TextCache::DIRTY_MAXLAYOUT;
        return mTextCache.layout.maxLayout;
    }
    return mTextCache.layout.layout;
}



void Text::setTextCursorPosition(int position)
{
    if(position == mTextCursorPosition)
        return;

    Widget::SharedPtr widget = getWidget();
    if(widget)
        widget->parentNeedsRender();
    mTextCursorPosition = position;
    if(widget)
        widget->needsRect();
    needsCursor();
    needsRender();
}

void Text::setTextFirstLine(int position)
{
    assert(MutexStack::locked(Widget::lock().name()));

    if(position != 0 && !mTextStyle->getTextCursor().empty()) {
        Log::error(TRACE_GIBBON_TEXT) << "textFirstLine can't be used if the cursor is enabled";
        return;
    }

    if(hasText() && position != mTextFirstLine) {
        mTextFirstLine = position;
        mNeedsRenderTextFirstLine = mTextStyle->getWrap();
        needsCursor();
        needsRender();
    }
}

void Text::updateCursor() const
{
    if(hasTextCursor()) {
        if(mTextCache.cursor.bounds.empty()) {
            Widget::SharedPtr widget = getWidget();
            const Rect renderRect = widget->getTransformedRenderRect();
            const Rect contentRect(widget->getLeftPadding(), widget->getTopPadding(),
                                   renderRect.width - widget->getLeftPadding() + widget->getRightPadding(),
                                   renderRect.height - widget->getTopPadding() + widget->getBottomPadding());
            mTextCache.cursor.bounds = TextEngine::getCursorRect(getTextLayout(contentRect.size()),
                                                                 mTextCache.cursor.position, mTextCache.cursor.position);

            if(!mTextCache.cursor.bounds.empty()) {
                propertiesChanged_internal(Text::Property_RenderCursorPosition);

                if(mTextStyle->getTextCursor().interval > 0) {
                    if(!mTextCursorTimer) {
                        mTextCursorTimer.reset(new CursorTimer(getWidget()));
                        mTextCursorTimer->restart(mTextStyle->getTextCursor().interval);
                    } else {
                        mTextCursorTimer->setState(true);
                    }
                }
            } else if(mTextCursorTimer) {
                mTextCursorTimer->stop();
                mTextCursorTimer.reset();
            }
        }
    } else if(mTextCursorTimer) {
        mTextCursorTimer->stop();
        mTextCursorTimer.reset();
    }

}

void Text::needsLayout(bool itemize) const
{
    if(Widget::SharedPtr widget = getWidget()) {
        DEBUG_TEXT_LAYOUT("INVALIDATE:%d: %s(%s)", itemize, widget->describe().c_str(), mText.c_str());
        widget->parentNeedsRender();
        mTextCache.layout.dirty |= TextCache::DIRTY_MAXLAYOUT | TextCache::DIRTY_LAYOUT;
        if(itemize || 1)
            mTextCache.layout.dirty |= TextCache::DIRTY_ITEMIZE;
        widget->needsRect();
        widget->needsRender();
    }
}

void Text::needsRender() const
{
    if(Widget::SharedPtr widget = getWidget()) {
        widget->parentNeedsRender();
        widget->needsRender();
    }
}

void Text::needsCursor() const
{
    mTextCache.cursor.position = mTextCursorPosition;
    mTextCache.cursor.bounds.clear();

    if(mTextCursorTimer && mTextCursorTimer->interval() != mTextStyle->getTextCursor().interval) {
        if(mTextStyle->getTextCursor().interval <= 0) {
            mTextCursorTimer->stop();
            mTextCursorTimer.reset();
        } else {
            mTextCursorTimer->restart(mTextStyle->getTextCursor().interval);
        }
    }
}

unsigned int Text::purge()
{
    mTextCache.displayList.reset();
    // recreate the text object with the existing family
    mTextStyle->dirtyFont();
    needsLayout();
    return 0; //Should we let the displaylist tell us? --Sam
}

void Text::setFontFamily(std::string family)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(family != mTextStyle->getFontFamily()) {
        mTextStyle->setFontFamily(family);
        if(hasText())
            needsItemize();
    }
}

void Text::setLineHeight(Coordinate lineHeight)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(lineHeight != mTextStyle->getLineHeight()) {
        mTextStyle->setLineHeight(lineHeight);
        if(hasText())
            needsLayout();
    }
}

void Text::setIndent(Coordinate indent)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(indent != mTextStyle->getIndent()) {
        mTextStyle->setIndent(indent);
        if(hasText())
            needsLayout();
    }
}

void Text::setFontPixelSize(uint32_t pxSize)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(pxSize != mTextStyle->getFontPixelSize()) {
        mTextStyle->setFontPixelSize(pxSize);
        if(hasText())
            needsItemize();
    }
}

void Text::setFontWeight(Font::Weight weight)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(weight != mTextStyle->getFontWeight()) {
        mTextStyle->setFontWeight(weight);
        if(hasText())
            needsItemize();
    }
}

void Text::setFontStyle(Font::Style style)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(style != mTextStyle->getFontStyle()) {
        mTextStyle->setFontStyle(style);
        if(hasText())
            needsItemize();
    }
}

void Text::setRichText(bool richText)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(richText != mTextStyle->getRichText()) {
        mTextStyle->setRichText(richText);
        if(hasText())
            needsItemize();
    }
}

void Text::setColor(const Color &color)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(color != mTextStyle->getColor()) {
        mTextStyle->setColor(color);
        if(hasText()) {
            if(mTextStyle->getRichText()) {
                needsLayout();
            } else {
                mTextCache.layout.layout.setColor(color);
                mTextCache.displayList.reset();
                needsRender();
            }
        }
    }
}

void Text::setBackgroundColor(const Color &color)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(color != mTextStyle->getBackgroundColor()) {
        mTextStyle->setBackgroundColor(color);
        if(hasText()) {
            if(mTextStyle->getRichText()) {
                needsLayout();
            } else {
                mTextCache.layout.layout.setBackgroundColor(color);
                mTextCache.displayList.reset();
                needsRender();
            }
        }
    }
}

void Text::setShadow(const Shadow &s)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(const uint8_t diff = mTextStyle->getShadow().diff(s)) {
        mTextStyle->setShadow(s);
        if(hasText()) {
            if((diff & TextEngine::DIFF_LAYOUT) || mTextStyle->getRichText()) {
                needsLayout();
            } else {
                mTextCache.layout.layout.setShadow(s);
                mTextCache.displayList.reset();
                needsRender();
            }
        }
    }
}

void Text::setPadding(const Padding &padding)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(padding != mTextStyle->getPadding()) {
        mTextStyle->setPadding(padding);
        if(hasText())
            needsItemize();
    }
}

void Text::setTypography(const Typography &typography)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(typography != mTextStyle->getTypography()) {
        mTextStyle->setTypography(typography);
        if(hasText())
            needsItemize();
    }
}

void Text::setUnderline(bool underline)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(underline != mTextStyle->getUnderline()) {
        mTextStyle->setUnderline(underline);
        if(hasText())
            needsLayout();
    }
}

void Text::setStrikethrough(bool strike)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(strike != mTextStyle->getStrikethrough()) {
        mTextStyle->setStrikethrough(strike);
        if(hasText())
            needsLayout();
    }
}

void Text::setTextTruncation(const TextTruncation &truncation)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(truncation != mTextStyle->getTextTruncation()) {
        mTextStyle->setTextTruncation(truncation);
        if(hasText())
            needsLayout();
    }
}

void Text::setTextCursor(const TextCursor &c)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(const uint8_t diff = mTextStyle->getTextCursor().diff(c)) {
        mTextStyle->setTextCursor(c);
        if(hasText()) {
            if((diff & TextEngine::DIFF_LAYOUT) || mTextStyle->getRichText()) {
                needsLayout();
            } else {
                mTextCache.layout.layout.setTextCursor(c);
                mTextCache.displayList.reset();
                needsRender();
            }
        }
    }
}

void Text::setEdgeEffect(const EdgeEffect &e)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(const uint8_t diff = mTextStyle->getEdgeEffect().diff(e)) {
        mTextStyle->setEdgeEffect(e);
        if(hasText()) {
            if((diff & TextEngine::DIFF_LAYOUT) || mTextStyle->getRichText()) {
                needsItemize();
            } else {
                mTextCache.layout.layout.setEdgeEffect(e);
                mTextCache.displayList.reset();
                needsRender();
            }
        }
    }
}

void Text::setAlign(uint8_t align)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(align != mTextStyle->getAlign()) {
        mTextStyle->setAlign(align);
        if(hasText())
            needsLayout();
    }
}

void Text::setWrap(bool wrap)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(wrap != mTextStyle->getWrap()) {
        mTextStyle->setWrap(wrap);
        if(hasText()) {
            if(mRenderTextFirstLine && !wrap) {
                mNeedsRenderTextFirstLine = false;
                mRenderTextFirstLine = 0;
                propertiesChanged_internal(Text::Property_RenderFirstLine);
            }
            needsItemize();
        }
    }
}

void Text::setMaxLines(unsigned maxLines)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(maxLines != mTextStyle->getMaxLines()) {
        mTextStyle->setMaxLines(maxLines);
        if(hasText())
            needsLayout();
    }
}

void Text::setLocale(const std::string &locale)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(locale != mTextStyle->getLocale()) {
        mTextStyle->setLocale(locale);
        if(hasText())
            needsItemize();
    }
}

void Text::setDirection(int direction)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(direction != mTextStyle->getDirection()) {
        mTextStyle->setDirection(direction);
        if(hasText())
            needsItemize();
    }
}

void Text::setScript(const std::string &script)
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(script != mTextStyle->getScript()) {
        mTextStyle->setScript(script);
        if(hasText())
            needsItemize();
    }
}

void Text::propertiesChanged_internal(unsigned int properties) const
{
    assert(MutexStack::locked(Widget::lock().name()));
    if(properties) {
        for(Text::ListenerContainer::const_iterator it = mListeners.begin(); it != mListeners.end(); ++it)
            (*it)->onPropertiesChanged(properties);
    }
}

void Text::addListener(const Listener::SharedPtr &listener)
{
    mListeners.insert(listener);
}

void Text::removeListener(const Listener::SharedPtr &listener)
{
    mListeners.erase(listener);
}

Variant Text::toVariant() const
{
    Variant data;
    data["layout"] = mTextCache.layout.layout.toVariant();
    data["itemizes"] = mTextCache.layout.itemizes;
    data["hits"] = mTextCache.layout.hits;
    data["misses"] = mTextCache.layout.misses;
    data["count"] = mTextCache.layout.count;
    return data;
}

std::map<std::string, std::vector<Rect> > Text::dumpRectangles(int line, int run, bool all, int what, const std::string& key)
{
    std::map<std::string, std::vector<Rect> > result;
    tiLayout* layout = &mTextCache.layout.layout;

    switch (what) {
    // runs
    case 0: {

        int startLine;
        int endLine;

        if (line == -1) {
            startLine = 0;
            endLine = static_cast<int>(layout->lines.size());
        } else {
            if (line < 0 || line > static_cast<int>(layout->lines.size()) - 1) {
                Log::warn(TRACE_UI_ENGINE) << "The text layout does not have a line " << line;
                return result;
            }
            startLine = line;
            endLine = line + 1;
        }

        for(int i = startLine; i < endLine; i++) {
            if (run == -1 && !all) {
                result["rects"].push_back(layout->lines[i].logicalBounds);
            } else {
                if (all) {
                    std::vector<tiRun>::const_iterator it = layout->lines[i].runs.begin();
                    std::vector<tiRun>::const_iterator end = layout->lines[i].runs.end();

                    for (; it != end; it++) {
                        result["rects"].push_back(it->logicalBounds);
                    }
                } else {
                    if (run < 0 || run > static_cast<int>(mTextCache.layout.layout.lines.size()) - 1) {
                        Log::warn(TRACE_UI_ENGINE) << "Line " << line << " does not have a run " << run;
                        return result;
                    }
                    result["rects"].push_back(layout->lines[i].runs[run].logicalBounds);
                }
            }
        }
        break;
    }

    // padding
    case 1: {
        Rect l = layout->bounds;
        Rect p = Rect(l.x, l.y, layout->hpad, l.height);
        result["rects"].push_back(p);
        p = Rect(l.x + l.width - layout->hpad + 1, l.y, layout->hpad, l.height);
        result["rects"].push_back(p);
        p = Rect(l.x, l.y, l.width, layout->vpad);
        result["rects"].push_back(p);
        p = Rect(l.x, l.y + l.height - layout->vpad + 1, l.width, layout->vpad);
        result["rects"].push_back(p);
        break;
    }

    //style properties
    case 2: {
        std::vector<tiLine>::const_iterator lineIt = layout->lines.begin();
        const std::vector<tiLine>::const_iterator lineItEnd = layout->lines.end();

        while (lineIt != lineItEnd) {
            std::vector<tiRun>::const_iterator runIt = lineIt->runs.begin();
            const std::vector<tiRun>::const_iterator runItEnd = lineIt->runs.end();

            while (runIt != runItEnd) {
                Style::SharedPtr style = runIt->item->attribute->style;

                if (key == "fonts") {
                    result[style->getFont()->describe()].push_back(runIt->logicalBounds);
                } else if (key == "scripts") {
                    result[style->getScript()].push_back(runIt->logicalBounds);
                } else if (key == "direction") {
                    if (style->getDirection() == 0) {
                        result["ltr"].push_back(runIt->logicalBounds);
                    } else {
                        result["rtl"].push_back(runIt->logicalBounds);
                    }
                } else if (key == "locales") {
                    result[style->getLocale()].push_back(runIt->logicalBounds);
                }

                ++runIt;
            }
            ++lineIt;
        }

        if (all) {
            Log::warn(TRACE_UI_ENGINE) << key;

            for (std::map<std::string, std::vector<Rect> >::iterator mit = result.begin(); mit != result.end(); ++mit) {
                std::vector<Rect> v = mit->second;

                Log::warn(TRACE_UI_ENGINE) << "  " << mit->first << ":";

                for (std::vector<Rect>::size_type i = 0; i < v.size(); ++i) {
                    Log::warn(TRACE_UI_ENGINE) << "    " << v[i];

                }
            }
        }

        if (key == "fonts")
            dump(Dump_Attributes);
        else if (key == "direction")
            dump(Dump_Direction);
        else if (key == "scripts")
            dump(Dump_Scripts);
        else if (key == "locales")
            dump(Dump_Locale);

        break;
    }
    }

    return result;
}

void Text::dump(unsigned int flags)
{
#ifdef BUILD_DEBUG
    if (flags & Dump_Attributes)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.attributeRuns;

    if (flags & Dump_Direction)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.directionRuns;

    if (flags & Dump_Scripts)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.scriptRuns;

    if (flags & Dump_Locale)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.localeRuns;

    if (flags & Dump_WordWrap)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.wordBreaks;

    if (flags & Dump_Final)
        Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.itemized.finalRuns;

    if (flags != 0)
        return;
#else
    NRDP_UNUSED(flags);
    Log::warn(TRACE_UI_ENGINE) << "Itemizer information is only available in debug builds";
#endif

    Log::warn(TRACE_UI_ENGINE) << mTextCache.layout.layout;
    Log::warn(TRACE_UI_ENGINE, "Cache Reuse: %d/%d (%d/%d)", mTextCache.layout.count,
              mTextCache.layout.itemizes, mTextCache.layout.hits, mTextCache.layout.misses);
    if(mTextCache.displayList)
        mTextCache.displayList->dump();
}

std::string Text::describe() const
{
    std::ostringstream stream;
    stream << "txt";
    stream << ":'" << getText() << "'";
    if(int l = getTextFirstLine())
        stream << "@" << l;
    stream << ": txtStyle:" << mTextStyle->describe();
    stream << " txtCache:" << mTextCache.layout.count << "/" << mTextCache.layout.itemizes
           << "(" << mTextCache.layout.hits << "," << mTextCache.layout.misses << "):"
           << mTextCache.displayList.get() << "@" << mTextCache.rect;
    stream << " ";
    return stream.str();
}
