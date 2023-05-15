/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <GraphicsEngine.h>
#include <TextLocale.h>
#include <GibbonFreetype.h>
#include <GibbonHarfbuzz.h>
#include <hb.h>

#if defined(GIBBON_GRAPHICSENGINE_NOVA)
    #include <GibbonApplication.h>
    #include <Screen.h>
#endif

using namespace netflix;
using namespace netflix::gibbon;

SERIALIZE_ALIAS(Font::Weight, uint8_t)
SERIALIZE_ALIAS(Font::Style, uint8_t)
SERIALIZE_ALIAS(EdgeEffect::Effect, uint8_t)
SERIALIZE_ALIAS(TextTruncation::Position, uint8_t)
SERIALIZE_ALIAS(TextCursor::CursorStyle, uint8_t)

bool GraphicsEngine::sDefaultSmoothScale = false;

class SetCompositionModeCommand : public DisplayList::Command
{
public:
    inline SetCompositionModeCommand() : Command(Type_SetCompositionMode), mode(GraphicsEngine::CompositionMode_Source) {}
    inline SetCompositionModeCommand(const GraphicsEngine::CompositionMode &_mode) : Command(Type_SetCompositionMode), mode(_mode) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "SetCompositionMode(" << mode << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.SetCompMode");
        graphics->setCompositionMode(mode);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << mode;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> mode;
    }

    GraphicsEngine::CompositionMode mode;
};

void GraphicsEngine::setCompositionMode_dl(CompositionMode mode)
{
    mDisplayList->appendCommand(new SetCompositionModeCommand(mode));
}

class SetOpacityCommand : public DisplayList::Command
{
public:
    inline SetOpacityCommand() : Command(Type_SetOpacity), opacity(1.) {}
    inline SetOpacityCommand(float _opacity) : Command(Type_SetOpacity), opacity(_opacity) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "SetOpacity(" << opacity << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.SetOpacity");
        graphics->setOpacity(state->opacity*opacity);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << opacity;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> opacity;
    }

    float opacity;
};

void GraphicsEngine::setOpacity_dl(float opacity)
{
    mDisplayList->appendCommand(new SetOpacityCommand(opacity));
}

class SetScaleCommand : public DisplayList::Command
{
public:
    inline SetScaleCommand() : Command(Type_SetScale), scale(1.) {}
    inline SetScaleCommand(float _scale) : Command(Type_SetScale), scale(_scale) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "SetScale(" << scale << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.Scale");
        graphics->setScale(state->scale*scale);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << scale;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> scale;
    }

    float scale;
};

void GraphicsEngine::setScale_dl(float scale)
{
    mDisplayList->appendCommand(new SetScaleCommand(scale));
}

class SetClipCommand : public DisplayList::Command
{
public:
    inline SetClipCommand() : Command(Type_SetClip) {}
    inline SetClipCommand(const Rect &_rect) : Command(Type_SetClip), rect(_rect) {}

    virtual Command::SharedPtr duplicate(State *state) {
        if(!gibbon_float_compare(state->scale, 1.0))
            return Command::SharedPtr(new SetClipCommand(rect.scaled(state->scale, state->scale)));
        return DisplayList::Command::duplicate(state);
    }

    virtual std::string describe() const {
        std::ostringstream s;
        s << "SetClip(" << rect << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.SetClip");
        graphics->setClip(state->transform(rect).intersected(state->clip));
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << rect;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> rect;
    }

    Rect rect;
};

class ClearClipCommand : public DisplayList::Command
{
public:
    inline ClearClipCommand() : Command(Type_ClearClip) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "ClearClip()";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.ClearClip");
        graphics->setClip(state->clip);
    }
    virtual void write(DisplayList::Writer &) const { }
    virtual void read(DisplayList::Reader &) { }
};

void GraphicsEngine::setClip_dl(const Rect *rect)
{
    assert(isDisplayList());
    if(rect)
        mDisplayList->appendCommand(new SetClipCommand(*rect));
    else
        mDisplayList->appendCommand(new ClearClipCommand);
}

class SetSmoothScaleCommand : public DisplayList::Command
{
public:
    inline SetSmoothScaleCommand() : Command(Type_SetSmoothScale), smooth(false) {}
    inline SetSmoothScaleCommand(bool _smooth) : Command(Type_SetSmoothScale), smooth(_smooth) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "SetSmoothScale(" << smooth << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.SetSmoothScale");
        graphics->setSmoothScale(smooth);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << smooth;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> smooth;
    }

    bool smooth;
};

void GraphicsEngine::setSmoothScale_dl(bool smooth)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new SetSmoothScaleCommand(smooth));
}

class ClearRectCommand : public DisplayList::Command
{
public:
    inline ClearRectCommand() : Command(Type_ClearRect) {}
    inline ClearRectCommand(const Rect &_rect) : Command(Type_ClearRect), rect(_rect) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "Clear(" << rect << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.ClearRect");
        graphics->clear(state->transform(rect));
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << rect;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> rect;
    }

    Rect rect;
};

void GraphicsEngine::clear_dl(const Rect &rect)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new ClearRectCommand(rect));
}

class FillRectCommand : public DisplayList::Command
{
public:
    inline FillRectCommand() : Command(Type_FillRect) {}
    inline FillRectCommand(const Rect &_rect, const Color &_color) : Command(Type_FillRect), rect(_rect), color(_color) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "Fill(" << rect << ", " << color << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.FillRect");
        graphics->fill(state->transform(rect), color);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << rect;
        writer << color;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> rect;
        reader >> color;
    }

    Rect rect;
    Color color;
};

void GraphicsEngine::fill_dl(const Rect& rect, const Color& color)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new FillRectCommand(rect, color));
}

class DrawRectCommand : public DisplayList::Command
{
public:
    inline DrawRectCommand() : Command(Type_DrawRect) {}
    inline DrawRectCommand(const Rect &_rect, const Color &_color) : Command(Type_DrawRect), rect(_rect), color(_color) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "DrawRect(" << rect << ", " << color << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.DrawRect");
        graphics->drawRect(state->transform(rect), color);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << rect;
        writer << color;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> rect;
        reader >> color;
    }

    Rect rect;
    Color color;
};

void GraphicsEngine::drawRect_dl(const Rect& rect, const Color& color)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new DrawRectCommand(rect, color));
}

class BlitCommand : public DisplayList::Command
{
public:
    inline BlitCommand() : Command(Type_Blit), useColor(false) {}
    inline BlitCommand(const Surface::SharedPtr &_srcSurface, const Rect &_srcRect, const Rect &_dstRect, const Color *_color=0) :
        Command(Type_Blit), surface(_srcSurface), srcRect(_srcRect), dstRect(_dstRect), color(_color ? *_color : Color()), useColor(_color) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "Blit(" << (surface ? surface->describe() : "N/A") << ", " << srcRect << ", " << dstRect << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.Blit");
        if(useColor)
            graphics->blit(surface, srcRect, state->transform(dstRect), color);
        else
            graphics->blit(surface, srcRect, state->transform(dstRect));
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << srcRect;
        writer << dstRect;
        writer << color;
        writer << useColor;
        writer << surface;
    }
    virtual void read(DisplayList::Reader &reader) {
        reader >> srcRect;
        reader >> dstRect;
        reader >> color;
        reader >> useColor;
        reader >> surface;
    }

    Surface::SharedPtr surface;
    Rect srcRect, dstRect;
    Color color;
    bool useColor;
};

void GraphicsEngine::blit_dl(const Surface::SharedPtr &srcSurface, const Rect &srcRect, const Rect &dstRect, const Color *color)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new BlitCommand(srcSurface, srcRect, dstRect, color));
}

class BatchBlitCommand : public DisplayList::Command
{
public:
    inline BatchBlitCommand() : Command(Type_BatchBlit), useColor(false) {}
    inline BatchBlitCommand(const Surface::SharedPtr& _surface, const std::vector<Rect>& _src,
                            const std::vector<Rect>& _dst, const Color *_color)
        : Command(Type_BatchBlit), surface(_surface), src(_src), dst(_dst), color(_color ? *_color : Color()), useColor(_color) {}

    virtual std::string describe() const {
        std::ostringstream s;
        s << "BatchBlit()";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.BatchBlit");
        if(useColor)
            graphics->batchBlit(surface, src, state->transform(dst), color);
        else
            graphics->batchBlit(surface, src, state->transform(dst));
    }
    virtual void write(DisplayList::Writer &writer) const {
        assert(src.size() == dst.size());
        writer << src.size();
        for(size_t idx = 0; idx < src.size(); ++idx) {
            writer << src[idx];
            writer << dst[idx];
        }
        writer << color;
        writer << useColor;
        writer << surface;
    }
    virtual void read(DisplayList::Reader &reader) {
        Rect rect;
        uint32_t rects;
        reader >> rects;
        for(uint32_t i = 0; i < rects; ++i) {
            reader >> rect;
            src.push_back(rect);
            reader >> rect;
            dst.push_back(rect);
        }
        reader >> color;
        reader >> useColor;
        reader >> surface;
    }

    Surface::SharedPtr surface;
    std::vector<Rect> src, dst;
    Color color;
    bool useColor;
};

void GraphicsEngine::batchBlit_dl(const Surface::SharedPtr& surface, const std::vector<Rect>& src,
                                  const std::vector<Rect>& dst, const Color *color)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new BatchBlitCommand(surface, src, dst, color));
}

void GraphicsEngine::drawText(const char *text, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style)
{
    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, strlen(text), 0, -1);

    netflix::gibbon::Locale locale(style->getLocale());
    if (locale.language.empty()) {
        assert(false);
        locale.language = "en"; // Default to English (ie, Latin shaper) if no language is specified (shouldn't happen)
    }

    hb_buffer_set_script(buf, HB_SCRIPT_INVALID);
    hb_buffer_set_language(buf, hb_language_from_string(locale.language.c_str(), locale.language.size()));
    hb_buffer_set_direction(buf, style->getDirection() ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
    hb_buffer_guess_segment_properties(buf);

    const Typography& typography = style->getTypography();
    std::string feature_settings = typography.fontFeatures;
    const GibbonFreetype::SharedPtr ft = style->getFont()->getFreetype();
    ft->harfbuzz()->setKerning(typography.kerning);
    if (feature_settings.empty()) {
        hb_shape(ft->harfbuzzFont(), buf, 0, 0);
    } else {
        std::stringstream fs(feature_settings);
        std::string item;
        std::vector<hb_feature_t> features;
        while (std::getline(fs, item, ',')) {
            hb_feature_t feature;
            if (hb_feature_from_string(item.c_str(), item.size(), &feature)) {
                features.push_back(feature);
            }
        }
        hb_shape(ft->harfbuzzFont(), buf, &features[0], features.size());
    }

    drawText(buf, 0, -1, rect, dstRect, style);

    hb_buffer_destroy(buf);
}

class DrawTextCommand : public DisplayList::Command
{
public:
    inline DrawTextCommand() : Command(Type_DrawText), text(0), length(0) {}
    inline DrawTextCommand(hb_buffer_t *_text, int _start, int _end,
                           const Rect &_rect, const Rect &_dstRect, const Style::SharedPtr &_style)
        : Command(Type_DrawText), text(_text), start(_start), end(_end), rect(_rect), dstRect(_dstRect), style(_style)
    {
        hb_buffer_reference(text);
        length = hb_buffer_get_length(text);
    }
    ~DrawTextCommand()
    {
        hb_buffer_destroy(text);
    }

    virtual std::string describe() const {
        std::ostringstream s;
        s << "DrawText(" << text << ", " << rect << ", " << dstRect << ", " << style << ")";
        return s.str();
    }
    virtual void apply(GraphicsEngine *graphics, State *state) const {
        PERFORMANCE_MARKER_SCOPED("Cmd.DrawText");
        Rect scaledRect = rect, scaledDstRect = dstRect;
        const float scale = graphics->scale();
        if(!gibbon_float_compare(scale, 1.0f)) {
            scaledRect.x *= scale;
            scaledRect.y *= scale;
            scaledDstRect.x *= scale;
            scaledDstRect.y *= scale;
        }
        graphics->drawText(text, start, end, state->transform(scaledRect), state->transform(scaledDstRect), style);
    }
    virtual void write(DisplayList::Writer &writer) const {
        writer << start << end << length;
        {
            unsigned int len;
            std::string buf;
            buf.resize(8192);
            hb_buffer_serialize_glyphs(text, 0, length, &buf[0], buf.size(), &len, 0,
                                       HB_BUFFER_SERIALIZE_FORMAT_TEXT, HB_BUFFER_SERIALIZE_FLAG_DEFAULT);
            buf.resize(len);
            writer << buf;
        }

        writer << rect;
        writer << dstRect;

        writer << style->getWrap();
        writer << style->getFontFamily();
        writer << style->getFontPixelSize();
        writer.writeCoordinate(style->getLineHeight());
        writer << style->getFontWeight();
        writer << style->getFontStyle();
        writer << style->getColor();
        writer << style->getBackgroundColor();
        writer << style->getAlign();
        writer << style->getMaxLines();
        writer << style->getUnderline();
        writer << style->getStrikethrough();
        writer << style->getRichText();
        writer.writeCoordinate(style->getRise());
        writer << style->getLocale();

        const Shadow &styleShadow = style->getShadow();
        writer << styleShadow.offsetX;
        writer << styleShadow.offsetY;
        writer << styleShadow.color;

        const Typography &styleTypography = style->getTypography();
        writer << styleTypography.tracking;
        writer << styleTypography.kerning;

        const TextTruncation &styleTruncation = style->getTextTruncation();
        writer << styleTruncation.position;
        writer << styleTruncation.ellipsis;

        const TextCursor &styleCursor = style->getTextCursor();
        writer << styleCursor.visible;
        writer << styleCursor.interval;
        writer << styleCursor.color;
        writer << styleCursor.style;
        writer << styleCursor.width;

        const EdgeEffect &styleEdgeEffect = style->getEdgeEffect();
        writer << styleEdgeEffect.color1;
        writer << styleEdgeEffect.color2;
        writer << styleEdgeEffect.width;
        writer << styleEdgeEffect.type;
    }

    virtual void read(DisplayList::Reader &reader) {
        reader >> start >> end >> length;
        {
            text = hb_buffer_create();
            std::string buf;
            reader >> buf;
            hb_buffer_deserialize_glyphs(text, buf.c_str(), buf.size(), 0, 0, HB_BUFFER_SERIALIZE_FORMAT_TEXT);
            assert(hb_buffer_get_length(text) == length);
        }
        reader >> rect;
        reader >> dstRect;

        style.reset(new Style);
#define STYLE_DESERIALIZE(t, n) { t n; reader >> n; style->n(n); }
#define STYLE_DESERIALIZE_COORD(n) { Coordinate n; reader.readCoordinate(n); style->n(n); }
        STYLE_DESERIALIZE(bool, setWrap);
        STYLE_DESERIALIZE(std::string, setFontFamily);
        STYLE_DESERIALIZE(uint32_t, setFontPixelSize);
        STYLE_DESERIALIZE_COORD(setLineHeight);
        STYLE_DESERIALIZE(Font::Weight, setFontWeight);
        STYLE_DESERIALIZE(Font::Style, setFontStyle);
        STYLE_DESERIALIZE(Color, setColor);
        STYLE_DESERIALIZE(Color, setBackgroundColor);
        STYLE_DESERIALIZE(uint8_t, setAlign);
        STYLE_DESERIALIZE(unsigned int, setMaxLines);
        STYLE_DESERIALIZE(bool, setUnderline);
        STYLE_DESERIALIZE(bool, setStrikethrough);
        STYLE_DESERIALIZE(bool, setRichText);
        STYLE_DESERIALIZE_COORD(setRise);
        STYLE_DESERIALIZE(std::string, setLocale);
#undef STYLE_DESERIALIZE

        Shadow styleShadow;
        reader >> styleShadow.offsetX;
        reader >> styleShadow.offsetY;
        reader >> styleShadow.color;
        style->setShadow(styleShadow);

        Typography styleTypography;
        reader >> styleTypography.tracking;
        reader >> styleTypography.kerning;
        style->setTypography(styleTypography);

        TextTruncation styleTruncation;
        reader >> styleTruncation.position;
        reader >> styleTruncation.ellipsis;
        style->setTextTruncation(styleTruncation);

        TextCursor styleCursor;
        reader >> styleCursor.visible;
        reader >> styleCursor.interval;
        reader >> styleCursor.color;
        reader >> styleCursor.style;
        reader >> styleCursor.width;
        style->setTextCursor(styleCursor);

        EdgeEffect styleEdgeEffect;
        reader >> styleEdgeEffect.color1;
        reader >> styleEdgeEffect.color2;
        reader >> styleEdgeEffect.width;
        reader >> styleEdgeEffect.type;
        style->setEdgeEffect(styleEdgeEffect);
    }

    hb_buffer_t* text;
    unsigned int length;
    int start, end;
    Rect rect, dstRect;
    Style::SharedPtr style;
};

void GraphicsEngine::drawText_dl_internal(hb_buffer_t *text, int start, int end, const Rect &rect, const Rect &dstRect, const Style::SharedPtr &style)
{
    assert(isDisplayList());
    mDisplayList->appendCommand(new DrawTextCommand(text, start, end, rect, dstRect, style));
}

void GraphicsEngine::initCommands()
{
    DisplayList::initCommands();
    DisplayList::registerFactory<SetCompositionModeCommand>(DisplayList::Command::Type_SetCompositionMode);
    DisplayList::registerFactory<SetOpacityCommand>(DisplayList::Command::Type_SetOpacity);
    DisplayList::registerFactory<SetScaleCommand>(DisplayList::Command::Type_SetScale);
    DisplayList::registerFactory<SetClipCommand>(DisplayList::Command::Type_SetClip);
    DisplayList::registerFactory<ClearClipCommand>(DisplayList::Command::Type_ClearClip);
    DisplayList::registerFactory<SetSmoothScaleCommand>(DisplayList::Command::Type_SetSmoothScale);
    DisplayList::registerFactory<ClearRectCommand>(DisplayList::Command::Type_ClearRect);
    DisplayList::registerFactory<FillRectCommand>(DisplayList::Command::Type_FillRect);
    DisplayList::registerFactory<DrawRectCommand>(DisplayList::Command::Type_DrawRect);
    DisplayList::registerFactory<BlitCommand>(DisplayList::Command::Type_Blit);
    DisplayList::registerFactory<BatchBlitCommand>(DisplayList::Command::Type_BatchBlit);
    DisplayList::registerFactory<DrawTextCommand>(DisplayList::Command::Type_DrawText);
    initCommands_sys();
}

namespace netflix {
namespace gibbon {
class GraphicsTextData : public GibbonFreetype::TextData
{
public:
    GraphicsTextData(GraphicsEngine* e) : engine(e) { }
    ~GraphicsTextData();

    struct Data
    {
        std::vector<Rect> regularSrc, regularDst;
        std::vector<Rect> outlineSrc, outlineDst;
        Surface::SharedPtr surface;
        bool hasRegularColor, hasOutlineColor;
        Color regularColor, outlineColor;
        enum Scale {
            NotSet,
            Scaled,
            NotScaled
        } scale;
        Rect scaleRect;
        float scaleFactor;

        bool isEmpty() const
        {
            return regularDst.empty() && regularSrc.empty() && outlineDst.empty() && outlineSrc.empty();
        }
    };
    GraphicsEngine* engine;
    std::vector<Data> datas;

    std::vector<ftcache::Glyph::SharedPtr> glyphs;

    void addEntry(Type type, const ftcache::Glyph::SharedPtr& glyph, int tx, int ty, const Surface::SharedPtr& surface, const Color& color)
    {
        glyphs.push_back(glyph);

        Data* d = datas.empty() ? 0 : &datas.back();
        const rectpacker::Rect& adjusted = GibbonFreetype::adjustRect(glyph->glyphNode->rect);
        if (datas.empty() || d->surface != surface
            || (type == GraphicsTextData::Regular && d->hasRegularColor && d->regularColor != color)
            || (type == GraphicsTextData::Outline && d->hasOutlineColor && d->outlineColor != color)) {
            Data nd;
            nd.surface = surface;
            if (type == GraphicsTextData::Regular) {
                nd.hasOutlineColor = false;
                nd.hasRegularColor = true;
                nd.regularColor = color;
            } else {
                nd.hasRegularColor = false;
                nd.hasOutlineColor = true;
                nd.outlineColor = color;
            }
            nd.scale = Data::NotSet;
            datas.push_back(nd);
            d = &datas.back();
        } else if (type == GraphicsTextData::Regular && !d->hasRegularColor) {
            d->hasRegularColor = true;
            d->regularColor = color;
        } else if (type == GraphicsTextData::Outline && !d->hasOutlineColor) {
            d->hasOutlineColor = true;
            d->outlineColor = color;
        }

        const Rect srcRect(adjusted.x, adjusted.y, (adjusted.right - adjusted.x) + 1, (adjusted.bottom - adjusted.y) + 1);
        const Rect dstRect(tx, ty, srcRect.width, srcRect.height);
        if(type == GibbonFreetype::TextData::Regular) {
            d->regularSrc.push_back(srcRect);
            d->regularDst.push_back(dstRect);
        } else {
            d->outlineSrc.push_back(srcRect);
            d->outlineDst.push_back(dstRect);
        }
    }
    void clear()
    {
        datas.clear();
    }
    void updateScale(bool scale, const Rect& rect, float factor)
    {
        if (datas.empty())
            return;
        Data&d = datas.back();
        if (d.scale == Data::NotSet) {
            d.scale = (scale ? Data::Scaled : Data::NotScaled);
            d.scaleFactor = factor;
            d.scaleRect = rect;
        } else if ((scale && d.scale == Data::NotScaled)
                   || (scale && !gibbon_float_compare(factor, d.scaleFactor))
                   || (scale && rect != d.scaleRect)
                   || (!scale && d.scale == Data::Scaled)) {
            if (d.isEmpty()) { // reuse
                d.scale = (scale ? Data::Scaled : Data::NotScaled);
                d.scaleFactor = factor;
                d.scaleRect = rect;
            } else { // create new
                Data nd;
                nd.surface = d.surface;
                nd.hasRegularColor = d.hasRegularColor;
                nd.regularColor = d.regularColor;
                nd.hasOutlineColor = d.hasOutlineColor;
                nd.outlineColor = d.outlineColor;
                nd.scale = (scale ? Data::Scaled : Data::NotScaled);
                nd.scaleFactor = factor;
                nd.scaleRect = rect;
                datas.push_back(nd);
            }
        }
    }
};

GraphicsTextData::~GraphicsTextData()
{
    std::vector<GraphicsTextData::Data>::const_iterator d = datas.begin();
    const std::vector<GraphicsTextData::Data>::const_iterator end = datas.end();
    while (d != end) {
        if (d->scale == Data::Scaled) {
            const Rect& r = d->scaleRect;
            if(!d->outlineSrc.empty()) {
                const size_t sz = d->outlineSrc.size();
                for(size_t i = 0; i < sz; ++i) {
                    const Rect& sr = d->outlineSrc.at(i);
                    Rect dr = d->outlineDst.at(i);
                    dr.x = ((dr.x - r.x) * d->scaleFactor) + r.x;
                    dr.y = ((dr.y - r.y) * d->scaleFactor) + r.y;
                    engine->blit(d->surface, sr, dr, d->outlineColor);
                }
            }
            if(!d->regularSrc.empty()) {
                const size_t sz = d->regularSrc.size();
                for(size_t i = 0; i < sz; ++i) {
                    const Rect& sr = d->regularSrc.at(i);
                    Rect dr = d->regularDst.at(i);
                    dr.x = ((dr.x - r.x) * d->scaleFactor) + r.x;
                    dr.y = ((dr.y - r.y) * d->scaleFactor) + r.y;
                    engine->blit(d->surface, sr, dr, d->regularColor);
                }
            }
        } else {
            if(!d->outlineSrc.empty()) {
                assert(d->outlineSrc.size() == d->outlineDst.size());
                engine->batchBlit(d->surface, d->outlineSrc, d->outlineDst, d->outlineColor);
            }
            if (!d->regularSrc.empty()) {
                assert(d->regularSrc.size() == d->regularDst.size());
                engine->batchBlit(d->surface, d->regularSrc, d->regularDst, d->regularColor);
            }
        }
        ++d;
    }
}
}} // namespace netflix::gibbon

void GraphicsEngine::startText_internal()
{
    assert(!mTextData);
    mTextData = new GraphicsTextData(this);
}

void GraphicsEngine::endText_internal()
{
    assert(mTextData);
    delete mTextData;
    mTextData = 0;
}

void GraphicsEngine::drawText_internal(hb_buffer_t *text, int start, int end, const Rect &r, const Rect &dstRect, const Style::SharedPtr &style)
{
    NRDP_UNUSED(dstRect);

    const bool del = (mTextData == 0);
    if (del)
        mTextData = new GraphicsTextData(this);
    const bool scale = !gibbon_float_compare(mState.scale, 1.0);
    GibbonFreetype::SharedPtr freetype = style->getFont()->getFreetype();
    freetype->prepareText(text, start, end, r, style, mTextData);
#if defined(GIBBON_GRAPHICSENGINE_NOVA)
    GibbonApplication::instance()->getScreen()->addToLockedGlyphs(mTextData->glyphs);
#endif
    mTextData->updateScale(scale, r, mState.scale);
    if (del) {
        delete mTextData;
        mTextData = 0;
    }
}
