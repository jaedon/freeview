/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "TextBridge.h"
#include "Debug.h"
#include "Widget.h"
#include "Screen.h"
#include "WidgetBridge.h"
#include "GibbonConfiguration.h"

#include <nrd/Request.h>
#include <nrdbase/StringUtils.h>

using namespace netflix::gibbon;
using namespace netflix;

static const unsigned int sRenderProperties = Text::Property_RenderLines|Text::Property_RenderCursorPosition|
                                              Text::Property_RenderFirstLine|Text::Property_RenderPadding;

#define TextBridgePropertyList(Property)                        \
    Property(align, NfObject::ReadWrite|DumpProperty)           \
    Property(backgroundColor, NfObject::ReadWrite|DumpProperty) \
    Property(color, NfObject::ReadWrite|DumpProperty)           \
    Property(contents, NfObject::ReadWrite|DumpProperty)        \
    Property(cursor, NfObject::ReadWrite|DumpProperty)          \
    Property(cursorPosition, NfObject::ReadWrite|DumpProperty)  \
    Property(direction, NfObject::ReadWrite|DumpProperty)       \
    Property(edgeEffect, NfObject::ReadWrite|DumpProperty)      \
    Property(family, NfObject::ReadWrite|DumpProperty)          \
    Property(firstLine, NfObject::ReadWrite|DumpProperty)       \
    Property(indent, NfObject::ReadWrite|DumpProperty)          \
    Property(lineHeight, NfObject::ReadWrite|DumpProperty)      \
    Property(locale, NfObject::ReadWrite|DumpProperty)          \
    Property(maxLines, NfObject::ReadWrite|DumpProperty)        \
    Property(padding, NfObject::ReadWrite|DumpProperty)         \
    Property(renderCursorPosition, NfObject::ReadOnly)          \
    Property(renderFirstLine, NfObject::ReadOnly)               \
    Property(renderLines, NfObject::ReadOnly)                   \
    Property(renderPadding, NfObject::ReadOnly)                 \
    Property(richText, NfObject::ReadWrite|DumpProperty)        \
    Property(script, NfObject::ReadWrite|DumpProperty)          \
    Property(sendRenderProperties, NfObject::ReadWrite)         \
    Property(sendTextLoaded, NfObject::ReadWrite)               \
    Property(shadow, NfObject::ReadWrite|DumpProperty)          \
    Property(size, NfObject::ReadWrite|DumpProperty)            \
    Property(strikethrough, NfObject::ReadWrite|DumpProperty)   \
    Property(style, NfObject::ReadWrite|DumpProperty)           \
    Property(truncation, NfObject::ReadWrite|DumpProperty)      \
    Property(typography, NfObject::ReadWrite|DumpProperty)      \
    Property(underline, NfObject::ReadWrite|DumpProperty)       \
    Property(variant, NfObject::ReadWrite|DumpProperty)         \
    Property(weight, NfObject::ReadWrite|DumpProperty)          \
    Property(wrap, NfObject::ReadWrite|DumpProperty)

PropertyTable(TextBridge, TextBridgePropertyList, properties)

#define TextBridgeMethodList(Method)            \
    Method(destroy)                             \
    Method(textInfo)

MethodTable(TextBridge, TextBridgeMethodList);

static inline Padding convertPadding(const Variant& value)
{
    Padding padding;
    if (value.isArray()) {
        padding.horizontal = value[0].value<int>(0, -1);
        if (value.size() == 1)
            padding.vertical = padding.horizontal;
        else
            padding.vertical = value[1].value<int>(0, -1);
    } else if (value.isStringMap()) {
        padding.horizontal = value.mapValue<int>("horizontal", 0, -1);
        padding.vertical = value.mapValue<int>("vertical", 0, -1);
    } else if (value.isNumber()) {
        padding.horizontal = padding.vertical = value.value<int>(0, -1);
    }
    return padding;
}

static Variant convertShadow(const Shadow &shadow)
{
    Variant value;
    value["offsetX"] = shadow.offsetX;
    value["offsetY"] = shadow.offsetY;
    value["color"] = convertColor(shadow.color);
    return value;
}

static Shadow convertShadow(const Variant &value)
{
    Shadow shadow;
    if(value.isArray()) {
        if(value.size() >= 1)
            shadow.offsetX = value[0].value<int>(0, 0);
        if(value.size() >= 2)
            shadow.offsetY = value[1].value<int>(0, 0);
        if(value.size() >= 3)
            shadow.color = convertColor(value[2]);
    } else {
        shadow.offsetX = value["offsetX"].value<int>(0, 0);
        shadow.offsetY = value["offsetY"].value<int>(0, 0);
        shadow.color = convertColor(value["color"]);
    }
    return shadow;
}

static Variant convertTypography(const Typography &typography)
{
    Variant value;
    value["tracking"] = typography.tracking;
    value["kerning"] = typography.kerning;
    value["fontFeatures"] = typography.fontFeatures;
    return value;
}

static Typography convertTypography(const Variant &value)
{
    Typography typography;
    typography.tracking = value["tracking"].value<int>(0, 0);
    typography.kerning = value["kerning"].value<bool>(0, true);
    typography.fontFeatures = value["fontFeatures"].value<std::string>();
    return typography;
}

static Variant convertEdgeEffect(const EdgeEffect &edgeEffect)
{
    Variant value;
    switch(edgeEffect.type) {
    case EdgeEffect::EDGE_EFFECT_NONE:
        value["type"] = "none";
        break;
    case EdgeEffect::EDGE_EFFECT_RAISED:
        value["type"] = "raised";
        break;
    case EdgeEffect::EDGE_EFFECT_DEPRESSED:
        value["type"] = "depressed";
        break;
    case EdgeEffect::EDGE_EFFECT_OUTLINE:
        value["type"] = "outline";
        break;
    }

    value["width"] = edgeEffect.width;
    value["color1"] = convertColor(edgeEffect.color1);
    value["color2"] = convertColor(edgeEffect.color2);
    return value;
}

static EdgeEffect::Effect convertEdgeEffectType(const std::string &v)
{
    if(v == "raised")
        return EdgeEffect::EDGE_EFFECT_RAISED;
    else if(v == "depressed")
        return EdgeEffect::EDGE_EFFECT_DEPRESSED;
    else if(v == "outline")
        return EdgeEffect::EDGE_EFFECT_OUTLINE;
    return EdgeEffect::EDGE_EFFECT_NONE;
}

static EdgeEffect convertEdgeEffect(const Variant &value)
{
    EdgeEffect edgeEffect;
    if(value.isArray()) {
        if(value.size() >= 1)
            edgeEffect.type = convertEdgeEffectType(value[0].value<std::string>());
        if(value.size() >= 2)
            edgeEffect.width = value[1].value<int>(0, 1);
        if(value.size() >= 3)
            edgeEffect.color1 = convertColor(value[2]);
        if(value.size() >= 4)
            edgeEffect.color2 = convertColor(value[3]);
    } else {
        edgeEffect.type = convertEdgeEffectType(value["type"].value<std::string>());
        edgeEffect.width = value["width"].value<int>(0, 1);
        edgeEffect.color1 = convertColor(value["color1"]);
        edgeEffect.color2 = convertColor(value["color2"]);
    }
    return edgeEffect;
}

static Variant convertTruncation(const TextTruncation &textTruncation)
{
    Variant value;
    switch(textTruncation.position) {
    case TextTruncation::POSITION_START:
        value["position"] = "start";
        break;
    case TextTruncation::POSITION_MIDDLE:
        value["position"] = "middle";
        break;
    case TextTruncation::POSITION_END:
        value["position"] = "end";
        break;
    case TextTruncation::POSITION_NONE:
        value["position"] = "none";
        break;
    }
    value["ellipsis"] = textTruncation.ellipsis;
    return value;
}

static TextTruncation::Position convertTruncationPosition(const std::string &v)
{
    if(v == "left" || v == "start")
        return TextTruncation::POSITION_START;
    else if(v == "center" || v == "middle")
        return TextTruncation::POSITION_MIDDLE;
    else if(v == "right" || v == "end")
        return TextTruncation::POSITION_END;
    return TextTruncation::POSITION_NONE;
}

static TextTruncation convertTruncation(const Variant &value)
{
    TextTruncation textTruncation;
    if(value.isArray()) {
        if(value.size() >= 1)
            textTruncation.position = convertTruncationPosition(value[0].value<std::string>());
        if(value.size() >= 2)
            textTruncation.ellipsis = value[1].value<std::string>(0, TextTruncation::defaultEllipsis());
    } else {
        textTruncation.position = convertTruncationPosition(value["position"].value<std::string>());
        textTruncation.ellipsis = value["ellipsis"].value<std::string>(0, TextTruncation::defaultEllipsis());
    }
    return textTruncation;
}

static TextCursor::CursorStyle convertCursorStyle(const Variant &value)
{
    if(value.isString()) {
        const std::string str = value.string();
        if(str == "" || str == "none")
            return TextCursor::CURSOR_NONE;
    }
    return TextCursor::CURSOR_LINE;
}

static unsigned convertDirection(const Variant &value)
{
    if(value.isNull())
        return convertDirection(GibbonConfiguration::defaultDirection());
    else if(value.isString())
        return value.string() == "rtl";
    return 0;
}

static std::string convertDirection(const unsigned value)
{
    if (value == 1)
        return "rtl";
    else
        return "ltr";
}

static Variant convertCursor(const TextCursor &textCursor)
{
    Variant value;
    switch(textCursor.style) {
    case TextCursor::CURSOR_LINE:
        value["style"] = "line";
        break;
    case TextCursor::CURSOR_NONE:
        value["style"] = "none";
        break;
    }
    value["interval"] = textCursor.interval;
    value["color"] = convertColor(textCursor.color);
    value["visible"] = bool(textCursor.visible);
    value["width"] = textCursor.width;
    return value;
}

static TextCursor convertCursor(const Variant &value)
{
    TextCursor textCursor;
    if(value.isArray()) {
        if(value.size() >= 1)
            textCursor.style = convertCursorStyle(value[0]);
        if(value.size() >= 2)
            textCursor.visible = value[1].value<bool>(0, false);
        if(value.size() >= 3)
            textCursor.interval = value[2].value<int>(0, 500);
        if(value.size() >= 4)
            textCursor.width = value[3].value<int>(0, 1);
        if(value.size() >= 5)
            textCursor.color = convertColor(value[4]);
    } else {
        textCursor.style = convertCursorStyle(value["style"]);
        textCursor.interval = value["interval"].value<int>(0, 500);
        textCursor.visible = value["visible"].value<bool>(0, false);
        textCursor.color = convertColor(value["color"]);
        textCursor.width = value["width"].value<int>(0, 1);
    }
    return textCursor;
}

TextBridge::TextBridge(const Text::SharedPtr &text)
    : GibbonBaseBridge("text", Properties::TextBridge::properties, Properties::TextBridge::PropertyCount,
                       Methods::TextBridge::methods, Methods::TextBridge::MethodCount, Flag_NoPropertySync),
      mSendRenderProperties(false), mSendTextLoaded(false), mText(text)
{
}

TextBridge::~TextBridge()
{
    ScopedMutex _lock(Widget::lock());
    mText.reset();
    clearChildren();
}

void TextBridge::init()
{
    mText->addListener(Text::Listener::SharedPtr(new TextListener(static_pointer_cast<TextBridge>(shared_from_this()))));
}

void TextBridge::handlePropertiesChanged(unsigned int properties)
{
    if(!mSendRenderProperties)
        return;
    const unsigned int renderProperties = properties & sRenderProperties;
    if(renderProperties)
        propertiesChanged(renderProperties);
}

bool TextBridge::getProperty(int index, Variant *result) const
{
    switch(index) {
    case Properties::TextBridge::sendRenderProperties:
        *result = mSendRenderProperties;
        break;
    case Properties::TextBridge::sendTextLoaded:
        *result = mSendTextLoaded;
        break;
    case Properties::TextBridge::cursorPosition: {
        const int val = mText->getTextCursorPosition();
        if(val == INT_MAX)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::TextBridge::renderCursorPosition: {
        const int val = mText->getRenderTextCursorPosition();
        if(val == INT_MAX)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::TextBridge::renderLines: {
        const int val = mText->getRenderTextLines();
        if(val == -1)
            *result = Variant();
        else
            *result = val;
        break; }
    case Properties::TextBridge::contents:
        *result = mText->getText();
        break;
    case Properties::TextBridge::renderFirstLine:
        *result = mText->getRenderTextFirstLine();
        break;
    case Properties::TextBridge::renderPadding: {
        const tiLayout& layout = mText->getTextLayout();
        Variant padding;
        padding["horizontal"] = layout.hpad;
        padding["vertical"] = layout.vpad;
        *result = padding;
        break; }
    case Properties::TextBridge::firstLine:
        *result = mText->getTextFirstLine();
        break;
    case Properties::TextBridge::family: {
        *result = mText->getFontFamily();
        break; }
    case Properties::TextBridge::size: {
        *result = mText->getFontPixelSize();
        break; }
    case Properties::TextBridge::weight: {
        *result = mText->getFontWeight() == Font::WEIGHT_BOLD ? "bold" : "normal";
        break; }
    case Properties::TextBridge::variant: {
        *result = mText->getFontStyle() == Font::STYLE_ITALIC ? "italic" : "normal";
        break; }
    case Properties::TextBridge::padding: {
        *result = mText->getPadding().toVariant();
        break; }
    case Properties::TextBridge::align: {
        *result = convertAlignment(mText->getAlign());
        break; }
    case Properties::TextBridge::color: {
        *result = convertColor(mText->getColor());
        break; }
    case Properties::TextBridge::backgroundColor: {
        *result = convertColor(mText->getBackgroundColor());
        break; }
    case Properties::TextBridge::shadow: {
        *result = convertShadow(mText->getShadow());
        break; }
    case Properties::TextBridge::typography: {
        *result = convertTypography(mText->getTypography());
        break; }
    case Properties::TextBridge::wrap: {
        *result = mText->getWrap();
        break; }
    case Properties::TextBridge::truncation: {
        *result = convertTruncation(mText->getTextTruncation());
        break; }
    case Properties::TextBridge::edgeEffect: {
        *result = convertEdgeEffect(mText->getEdgeEffect());
        break; }
    case Properties::TextBridge::maxLines: {
        *result = mText->getMaxLines();
        break; }
    case Properties::TextBridge::lineHeight: {
        const int lines = (int)mText->getLineHeight();
        if(lines != INT_MAX)
            *result = lines;
        break; }
    case Properties::TextBridge::indent: {
        *result = mText->getIndent();
        break; }
    case Properties::TextBridge::underline: {
        *result = mText->getUnderline();
        break; }
    case Properties::TextBridge::strikethrough: {
        *result = mText->getStrikethrough();
        break; }
    case Properties::TextBridge::richText: {
        *result = mText->getRichText();
        break; }
    case Properties::TextBridge::locale: {
        *result = mText->getLocale();
        break; }
    case Properties::TextBridge::cursor: {
        *result = convertCursor(mText->getTextCursor());
        break; }
    case Properties::TextBridge::direction: {
        *result = convertDirection(mText->getDirection());
        break; }
    case Properties::TextBridge::script: {
        *result = mText->getScript();
        break; }
    default:
        return false;
    }
    return true;
}

bool TextBridge::setProperty(int index, const Variant &value)
{
    GibbonBaseBridge::setHaveProperty(index);
    if(Debug::DebugWidgetProperties)
        Log::error(TRACE_LOG, "[%s:%d]: %s(text) (%s->%s)", __func__, __LINE__, mText->getWidget()->describe().c_str(),
                                  propertyName(index), value.toJSON().c_str());
    switch(index) {
    case Properties::TextBridge::sendRenderProperties: {
        const bool old = mSendRenderProperties;
        mSendRenderProperties = value.value<bool>();
        if(mSendRenderProperties && !old && mText->hasText())
            handlePropertiesChanged(sRenderProperties);
        break; }
    case Properties::TextBridge::sendTextLoaded: {
        mSendTextLoaded = value.value<bool>();
        break; }
    case Properties::TextBridge::contents:
        mText->setText(value.value<std::string>());
        break;
    case Properties::TextBridge::cursorPosition: {
        if(value.isStringMap()) {
            const int lines = value.mapValue<int>("lines", 0, 0);
            const int columns = value.mapValue<int>("columns", 0, 0);
            mText->setTextCursorPosition(TextEngine::moveCursor(mText->getTextLayout(), mText->getTextCursorPosition(), lines, columns));
        } else {
            const int position = std::max(value.value<int>(0, INT_MAX), 0);
            mText->setTextCursorPosition(position);
        }
        break; }
    case Properties::TextBridge::firstLine: {
        const int line = value.value<int>(0, 0);
        mText->setTextFirstLine(line);
        break; }
    case Properties::TextBridge::family: {
        const std::string family = value.value<std::string>(0, std::string());
        mText->setFontFamily(family.length() ? family : "Arial_for_Netflix");
        break; }
    case Properties::TextBridge::size: {
        if(const int size = value.value<int>())
            mText->setFontPixelSize(int(size * GibbonApplication::instance()->getScreen()->getCoordinateScale()));
        break; }
    case Properties::TextBridge::weight: {
        const std::string weight = value.value<std::string>();
        mText->setFontWeight(strcasecmp(weight.c_str(), "normal") == 0 ||
                             strcasecmp(weight.c_str(), "regular") == 0 ||
                             strcasecmp(weight.c_str(), "plain") == 0 ? Font::WEIGHT_NORMAL : Font::WEIGHT_BOLD);
        break; }
    case Properties::TextBridge::variant: {
        const std::string variant = value.value<std::string>();
        mText->setFontStyle(strcasecmp(variant.c_str(), "italic") == 0 ||
                            strcasecmp(variant.c_str(), "oblique") == 0 ? Font::STYLE_ITALIC : Font::STYLE_NORMAL);
        break; }
    case Properties::TextBridge::padding: {
        mText->setPadding(convertPadding(value));
        break; }
    case Properties::TextBridge::align: {
        mText->setAlign(convertAlignment(value));
        break; }
    case Properties::TextBridge::color: {
        mText->setColor(convertColor(value));
        break; }
    case Properties::TextBridge::backgroundColor: {
        mText->setBackgroundColor(convertColor(value));
        break; }
    case Properties::TextBridge::shadow: {
        mText->setShadow(convertShadow(value));
        break; }
    case Properties::TextBridge::typography: {
        mText->setTypography(convertTypography(value));
        break; }
    case Properties::TextBridge::wrap: {
        mText->setWrap(value.value<bool>(0, false));
        break; }
    case Properties::TextBridge::truncation: {
        mText->setTextTruncation(convertTruncation(value));
        break; }
    case Properties::TextBridge::edgeEffect: {
        mText->setEdgeEffect(convertEdgeEffect(value));
        break; }
    case Properties::TextBridge::maxLines: {
        mText->setMaxLines(value.value<int>(0, 0));
        break; }
    case Properties::TextBridge::lineHeight: {
        bool ok;
        Coordinate lineHeight = value.value<Coordinate>(&ok,CoordinateMax);
        if(ok)
            lineHeight *= GibbonApplication::instance()->getScreen()->getCoordinateScale();
        mText->setLineHeight(lineHeight);
        break; }
    case Properties::TextBridge::indent: {
        Coordinate indent = value.value<Coordinate>(0, 0);
        mText->setIndent(indent);
        break; }
    case Properties::TextBridge::underline: {
        mText->setUnderline(value.value<bool>(0, false));
        break; }
    case Properties::TextBridge::richText: {
        mText->setRichText(value.value<bool>(0, false));
        break; }
    case Properties::TextBridge::locale: {
        mText->setLocale(value.value<std::string>(0, std::string()));
        break; }
    case Properties::TextBridge::cursor: {
        mText->setTextCursor(convertCursor(value));
        break; }
    case Properties::TextBridge::strikethrough: {
        mText->setStrikethrough(value.value<bool>(0, false));
        break; }
    case Properties::TextBridge::direction: {
        mText->setDirection(convertDirection(value));
        break; }
    case Properties::TextBridge::script: {
        const std::string script = value.value<std::string>(0, std::string());
        if(!script.empty())
            mText->setScript(script);
        break; }
    default:
        return false;
    }
    return true;
}

Variant TextBridge::invoke(int method, const Variant &data)
{
    switch (method) {
    case Methods::TextBridge::destroy:
        setParent(shared_ptr<NfObject>());
        break;
    case Methods::TextBridge::textInfo: {
        bool ok;
        const int id = data.mapValue<int>("id", &ok);
        if(!ok) {
            invalidArgumentError(method, "id");
            return false;
        }

        Variant ret;
        ret["info"] = mText->toVariant();
        ret["id"] = id;
        sendEvent("textInfo", ret);
        break; }
    default:
        return false;
    }
    return Variant();
}

void TextBridge::syncPendingProperties(unsigned int properties)
{
    std::vector<int> updated;
    if(properties & Text::Property_RenderLines)
        updated.push_back(Properties::TextBridge::renderLines);
    if(properties & Text::Property_RenderCursorPosition)
        updated.push_back(Properties::TextBridge::renderCursorPosition);
    if(properties & Text::Property_RenderFirstLine)
        updated.push_back(Properties::TextBridge::renderFirstLine);
    if(properties & Text::Property_RenderPadding)
        updated.push_back(Properties::TextBridge::renderPadding);
    if(updated.size())
        propertiesUpdated(&updated[0], updated.size());
}
