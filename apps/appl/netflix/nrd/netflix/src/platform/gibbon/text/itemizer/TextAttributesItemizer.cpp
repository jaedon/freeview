/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "TextAttributesItemizer.h"

#include <string>
#include <stack>
#include <string.h>

#include <nrdbase/SaxParser.h>
#include <nrd/AppLog.h>
#include <GibbonConfiguration.h>

using namespace netflix::gibbon;
using namespace icu;

void TextAttributeRuns::addAttribute(uint32_t start, uint32_t end, const Style::SharedPtr &style, int visualOrder, int sequence)
{
    Base::add(TextAttributeRun::SharedPtr(new TextAttributeRun(start, end, style, visualOrder, sequence)));
}

void TextAttributeRuns::addAttribute(uint32_t start, uint32_t end, const Style::SharedPtr &style)
{
    addAttribute(start, end, style, 0, 0);
}

void TextAttributeRuns::swap(TextAttributeRuns& target)
{
    m_vector.swap(target.m_vector);
}

struct parseState {
    parseState(UnicodeString& _text, TextAttributeRuns &_runs, const Style::SharedPtr &_style) : _offset(0), text(_text), runs(_runs), sourceText(0), addToPrevious(false), needsBottomLayer(false) {
        style.push(_style);
    }
    uint32_t _offset;

    UnicodeString& text;
    TextAttributeRuns& runs;
    std::stack<Style::SharedPtr> style;
    const char* sourceText;
    bool addToPrevious;
    bool needsBottomLayer;
};


// XML parsing
//------------------------------------------------------------------------------
#ifdef BUILD_DEBUG
# define RICHTEXT_WARN_SEVERITY warn
#else
# define RICHTEXT_WARN_SEVERITY error
#endif

#define RICHTEXT_WARN(s) if(GibbonConfiguration::richTextWarnings()) {netflix::Log::RICHTEXT_WARN_SEVERITY(TRACE_GIBBON_TEXT) << s;}

static inline bool getBoolean(const char *text)
{
    return strcasecmp(text, "true") == 0 || strcasecmp(text, "on") == 0 || strcmp(text, "1") == 0 || strcasecmp(text, "yes") == 0 || strcasecmp(text, "si") == 0 || strcasecmp(text, "oui") == 0;
}


inline void setRise(Style::SharedPtr& newStyle, double size, double offset)
{
    // Set the size
    uint32_t newPixelSize = newStyle->getFontPixelSize() * size;
    newStyle->setFontPixelSize(newPixelSize);

    // Set the baseline offset
    Coordinate ascent = newStyle->getFont()->getAscent(newStyle);
    newStyle->setRise(static_cast<int>(ascent * offset));
}

inline void setSubscript(Style::SharedPtr& newStyle)
{
    // Defaults: size: 58%, position: -33% (from LibreOffice)
    const double subscriptSize = 0.58;
    const double subscriptPosition = -0.33;

    if (newStyle->getRise() != 0) {
        RICHTEXT_WARN("Nested super/subscripts are not allowed");
        return;
    } else {
        setRise(newStyle, subscriptSize, subscriptPosition);
    }
}

inline void setSuperscript(Style::SharedPtr& newStyle)
{
    // Defaults: size: 58%, position: 33% (from LibreOffice)
    // The position is 100% - 33%
    const double superscriptSize = 0.58;
    const double superscriptPosition = 0.66;

    if (newStyle->getRise() != 0) {
        RICHTEXT_WARN("Nested super/subscripts are not allowed");
        return;
    } else {
        setRise(newStyle, superscriptSize, superscriptPosition);
    }
}


static void charDataHandler(void *userData, const char *s, int len)
{
    parseState *ps = static_cast<parseState*>(userData);

    // Assuming the expat library was built with its default utf-8 setting
    // Maybe check if XML_UNICODE is defined and accept text in UTF-16 in that case?
    UnicodeString text = UnicodeString::fromUTF8(StringPiece(s, len));

    // From here we need to use the UnicodeString's length (or countChar32 if we want to know the codepoints),
    // as we are now in UTF-16's space. Note that the number of codepoints is not the length / 2 as we might
    // have surrogate pairs.
    // Example: U+1D11E -> from UTF8 0xF0 0x9D 0x84 0x9E to UTF16 0xD834 0xDD1E

    // Keep text with the same style together
    if(ps->runs.size() != 0 && ((ps->addToPrevious) || (ps->style.top()->equals(ps->runs.get().back()->style)))) {
        ps->runs.get().back()->end += text.length();
    } else {
        ps->runs.addAttribute(ps->_offset, ps->_offset + text.length(), ps->style.top());
    }
    ps->text.append(text);
    ps->_offset += text.length();
    ps->addToPrevious = false;
}

static bool checkTag(const char *attribute, const char *name, const parseState *ps)
{
    if(*attribute == '\0') {
        RICHTEXT_WARN ("No value set for the '" << name << "' element of the <span> tag. Text: " << ps->sourceText)
        return false;
    }
    return true;
}

static void startElementHandler(void *userData, const char *name, const char **attributes)
{
    parseState *ps = reinterpret_cast<parseState*>(userData);
    Style::SharedPtr newStyle = ps->style.top()->clone();
    newStyle->setNeedsPDI(false);
    //int charIndex = 0;

    if(strcasecmp(name, "text") == 0) {
        // Do nothing, this is the base tag we insert.

    // Convenience tags
    } else if(strcasecmp(name, "b") == 0) {
        newStyle->setFontWeight(Font::WEIGHT_BOLD);

    } else if(strcasecmp(name, "i") == 0) {
        newStyle->setFontStyle(Font::STYLE_ITALIC);

    } else if(strcasecmp(name, "u") == 0) {
        newStyle->setUnderline(true);

    } else if(strcasecmp(name, "s") == 0) {
        newStyle->setStrikethrough(true);

    } else if(strcasecmp(name, "sup") == 0) {
        setSuperscript(newStyle);

    } else if(strcasecmp(name, "sub") == 0) {
        setSubscript(newStyle);

    } else if((strcasecmp(name, "bigger") == 0) || (strcasecmp(name, "big") == 0)) {
        if (newStyle->getRise() != 0) {
            RICHTEXT_WARN("Invalid <" << name << "> tag in a super/subscript");
        } else {
            newStyle->setFontPixelSize(int(roundf(newStyle->getFontPixelSize() * 1.2)));
        }

    } else if((strcasecmp(name, "smaller") == 0) || (strcasecmp(name, "small") == 0)){
        if (newStyle->getRise() != 0) {
            RICHTEXT_WARN("Invalid <" << name << "> tag in a super/subscript");
        } else {
            newStyle->setFontPixelSize(int(roundf(newStyle->getFontPixelSize() / 1.2)));
        }

    } else if(((strcasecmp(name, "br") == 0) || (strcasecmp(name, "p") == 0))) { // Only support new lines if we are word wrapping
        if (newStyle->getRise() != 0) {
            RICHTEXT_WARN("Invalid <" << name << "> tag in a super/subscript");
        } else if(!ps->style.top()->getWrap()) {
            RICHTEXT_WARN("Processing of <" << name << "> tags is only done when word wrapping is enabled");
        } else {
            // Insert a paragraph separator, which creates a new BiDi context.
            const unsigned char PSEP[3] = { 0xE2, 0x80, 0xA9 }; // U+2029 PARAGRAPH SEPARATOR (PSEP) UTF8: 0xE2 0x80 0xA9
            ps->addToPrevious = true;
            charDataHandler(userData, reinterpret_cast<const char*>(PSEP), 3);
        }

    } else if(strcasecmp(name, "bdi") == 0) { // Bidirectional Isolation Element
        if (newStyle->getRise() != 0) {
            RICHTEXT_WARN("Invalid <" << name << "> tag in a super/subscript");
            return;
        }
        int dir = 2; // auto by default
        const unsigned char LRI[3] = { 0xE2, 0x81, 0xA6 }; // U+2066 LEFT-TO-RIGHT ISOLATE (LRI) UTF8: 0xE2 0x81 0xA6
        const unsigned char RLI[3] = { 0xE2, 0x81, 0xA7 }; // U+2067 RIGHT-TO-LEFT ISOLATE (RLI) UTF8: 0xE2 0x81 0xA7
        const unsigned char FSI[3] = { 0xE2, 0x81, 0xA8 }; // U+2068 FIRST STRONG ISOLATE  (FSI) UTF8: 0xE2 0x81 0xA8
        const char* code = 0;

        for (int i = 0; attributes[i]; i += 2) {
            if((strcasecmp(attributes[i], "dir") == 0) || strcasecmp(attributes[i], "direction") == 0) {
                if(strcasecmp(attributes[i + 1], "rtl") == 0)
                    dir = 1;
                else if(strcasecmp(attributes[i + 1], "ltr") == 0)
                    dir = 0;
            } else {
                RICHTEXT_WARN("Unknown attribute '" << attributes[i] << "' inside <bdi> element. Text: " << ps->text)
            }
        }
        switch (dir) {
        case 0: // ltr
            code = reinterpret_cast<const char*>(LRI);
            break;
        case 1: // rtl
            code = reinterpret_cast<const char*>(RLI);
            break;
        default: // auto/other
            code = reinterpret_cast<const char*>(FSI);
            break;
        }
        // Inject the isolate mark in the text stream
        charDataHandler(userData, code, 3); //TODO: Pass this as UTF-16 so we don't have to convert from UTF-8

    } else if(strcasecmp(name, "bdo") == 0) { // Bidirectional Override Element
        if (newStyle->getRise() != 0) {
            RICHTEXT_WARN("Invalid <" << name << "> tag in a super/subscript");
            return;
        }
        int dir = 2; // auto by default
        const unsigned char LRO[3] = { 0xE2, 0x80, 0xAD }; // U+202D LEFT-TO-RIGHT OVERRIDE (LRO) UTF8: 0xE2 0x80 0xAD
        const unsigned char RLO[3] = { 0xE2, 0x80, 0xAE }; // U+2067 RIGHT-TO-LEFT OVERRIDE (RLO) UTF8: 0xE2 0x80 0xAE
        const char* code = 0;

        for (int i = 0; attributes[i]; i += 2) {
            if((strcasecmp(attributes[i], "dir") == 0) || strcasecmp(attributes[i], "direction") == 0) {
                if(strcasecmp(attributes[i + 1], "rtl") == 0)
                    dir = 1;
                else if(strcasecmp(attributes[i + 1], "ltr") == 0)
                    dir = 0;
            } else {
                RICHTEXT_WARN("Unknown attribute '" << attributes[i] << "' inside <bdo> element. Text: " << ps->text);
            }
        }

        switch (dir) {
        case 0:
            code = reinterpret_cast<const char*>(LRO);
            break;
        case 1:
            code = reinterpret_cast<const char*>(RLO);
            break;
        default:
            break;
            // We don't do anything on the "auto" case. The direction itemizer will handle it.
        }
        if(code)
            charDataHandler(userData, code, 3); //TODO: Pass this as UTF-16 so we don't have to convert from UTF-8

    } else if(strcasecmp(name, "span") == 0) { // Parse attributes for <span> tag
        for (int i = 0; attributes[i]; i += 2) {
            if((strcasecmp(attributes[i], "fgcolor") == 0) || (strcasecmp(attributes[i], "color") == 0)) {
                if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setColor(Color(attributes[i + 1]));
                }

            } else if((strcasecmp(attributes[i], "bgcolor") == 0) || (strcasecmp(attributes[i], "background") == 0)) {
                if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setBackgroundColor(Color(attributes[i + 1]));
                    ps->needsBottomLayer = true;
                }

            } else if(strcasecmp(attributes[i], "bold") == 0) {
                if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setFontWeight(getBoolean(attributes[i + 1]) ? Font::WEIGHT_BOLD : Font::WEIGHT_NORMAL);
                }

            } else if(strcasecmp(attributes[i], "italic") == 0) {
                if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setFontStyle(getBoolean(attributes[i + 1]) ? Font::STYLE_ITALIC : Font::STYLE_NORMAL);
                }

            } else if(strcasecmp(attributes[i], "underline") == 0) {
                if (newStyle->getRise() != 0) {
                    RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                } else if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setUnderline(getBoolean(attributes[i + 1]));
                }

            } else if((strcasecmp(attributes[i], "strikethrough") == 0) || (strcasecmp(attributes[i], "strike")) == 0) {
                if (newStyle->getRise() != 0) {
                    RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                } else if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setStrikethrough(getBoolean(attributes[i + 1]));
                }

            } else if(((strcasecmp(attributes[i], "superscript") == 0) || (strcasecmp(attributes[i], "super") == 0)) && getBoolean(attributes[i + 1])) {
                setSuperscript(newStyle);

            } else if(((strcasecmp(attributes[i], "subscript") == 0) || (strcasecmp(attributes[i], "sub") == 0)) && getBoolean(attributes[i + 1])) {
                setSubscript(newStyle);

            } else if((strcasecmp(attributes[i], "locale") == 0) || (strcasecmp(attributes[i], "lang") == 0) || (strcasecmp(attributes[i], "language") == 0)){
                if (newStyle->getRise() != 0) {
                    RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                } else if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setLocale(attributes[i + 1]);
                }

            } else if(strcasecmp(attributes[i], "fontfeatures") == 0) {
                if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    Typography t = newStyle->getTypography();
                    t.fontFeatures = attributes[i + 1];
                    newStyle->setTypography(t);
                }

            } else if((strcasecmp(attributes[i], "font_face") == 0) || (strcasecmp(attributes[i], "font_family") == 0)
                        || (strcasecmp(attributes[i], "face") == 0) || (strcasecmp(attributes[i], "family") == 0)
                        || (strcasecmp(attributes[i], "font") == 0) || (strcasecmp(attributes[i], "font_name") == 0) ) {

                if (newStyle->getRise() != 0) {
                    RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                } else if(checkTag(attributes[i + 1], attributes[i], ps)) {
                    newStyle->setFontFamily(attributes[i + 1]);
                }

            } else if((strcasecmp(attributes[i], "dir") == 0) || strcasecmp(attributes[i], "direction") == 0) { // According to the HTML5 spec, the direction attribute forces an isolate
                if (newStyle->getRise() != 0) {
                    RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                } else {
                    const unsigned char LRI[3] = { 0xE2, 0x81, 0xA6 }; // U+2066 LEFT-TO-RIGHT ISOLATE (LRI) UTF8: 0xE2 0x81 0xA6
                    const unsigned char RLI[3] = { 0xE2, 0x81, 0xA7 }; // U+2067 RIGHT-TO-LEFT ISOLATE (RLI) UTF8: 0xE2 0x81 0xA7
                    const unsigned char FSI[3] = { 0xE2, 0x81, 0xA8 }; // U+2068 FIRST STRONG ISOLATE  (FSI) UTF8: 0xE2 0x81 0xA8
                    const char* code = 0;

                    if(strcasecmp(attributes[i + 1], "ltr") == 0) {
                        code = reinterpret_cast<const char*>(LRI);
                    } else if(strcasecmp(attributes[i + 1], "rtl") == 0) {
                        code = reinterpret_cast<const char*>(RLI);
                    } else {
                        code = reinterpret_cast<const char*>(FSI);
                    }
                    charDataHandler(userData, code, 3);
                    newStyle->setNeedsPDI(true);
                }

            } else if(strcasecmp(attributes[i], "size") == 0 ) {
                if(strcasecmp(attributes[i + 1], "smaller") == 0)
                    if (newStyle->getRise() != 0) {
                        RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                    } else {
                        newStyle->setFontPixelSize(int(roundf(newStyle->getFontPixelSize() / 1.2)));
                    }
                else if(strcasecmp(attributes[i + 1], "bigger") == 0)
                    if (newStyle->getRise() != 0) {
                        RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                    } else {
                        newStyle->setFontPixelSize(int(roundf(newStyle->getFontPixelSize() * 1.2)));
                    }
                else {
                    if (newStyle->getRise() != 0) {
                        RICHTEXT_WARN("Invalid attribute '" << attributes[i] << "' in a super/subscript");
                    } else {
                        int size = atoi(attributes[i + 1]);
                        if(size > 0)
                            newStyle->setFontPixelSize(atoi(attributes[i + 1]));
                    }
                }
            } else {
                RICHTEXT_WARN("Unknown attribute '" << attributes[i] << "' inside <span> element. Text: " << ps->sourceText)
            }
        }
    } else {
        RICHTEXT_WARN("Unknown rich text tag '" << name << "'. Text: " << ps->sourceText)
    }

    ps->style.push(newStyle);
}

static void endElementHandler(void *userData, const char *name)
{
    parseState *ps = static_cast<parseState*>(userData);

    if(strcasecmp(name, "bdi") == 0 || ps->style.top()->getNeedsPDI()) {
        const unsigned char PDI[3] = { 0xE2, 0x81, 0xA9 }; // U+2069 POP DIRECTIONAL ISOLATE (PDI) UTF8: 0xE2 0x81 0xA9
        charDataHandler(userData, reinterpret_cast<const char*>(PDI), 3);

    } else if(strcasecmp(name, "bdo") == 0) {
        const unsigned char PDF[3] = { 0xE2, 0x80, 0xAC }; // U+202C POP DIRECTIONAL FORMATTING (PDF) UTF8: 0xE2 0x80 0xAC
        charDataHandler(userData, reinterpret_cast<const char*>(PDF), 3);
    }

    ps->style.pop();
}


// Text attributes itemizer
// Generate a UTF-16 representation of the text, without the markup, and create
// a collection of text runs with the same style attributes
// Input comes from expat, which is utf-8.
//------------------------------------------------------------------------------

void TextAttributesItemizer::processAttributes(TextAttributeRuns &textRuns, UnicodeString &text, const char *utf8Text, const netflix::gibbon::Style::SharedPtr &baseStyle)
{
    if(baseStyle->getRichText()) {
        SaxParser expat;

        parseState ps(text, textRuns, baseStyle);
        ps.sourceText = utf8Text;

        expat.setUserData(&ps);
        expat.setCharDataHandler(charDataHandler);
        expat.setElementHandler(startElementHandler, endElementHandler);

        std::string xmlText = "<text>";
        xmlText.append(utf8Text).append("</text>");
        bool ex = expat.parse(xmlText);

        if(ps.needsBottomLayer)
            textRuns.hasBottomLayer = true;

        if(ex) {
            if(textRuns.size() == 0) {
                RICHTEXT_WARN("Parsing rich text resulted in an empty string");
                RICHTEXT_WARN(" String: |" << xmlText << "|");
            }
            return;
        }

        Log::error(TRACE_GIBBON_TEXT) << "Error parsing rich text string: " << expat.errorString();
        Log::error(TRACE_GIBBON_TEXT) << " String: |" << xmlText << "|";
        Log::error(TRACE_GIBBON_TEXT) << " Offending code:\n" << Log::hexDump(xmlText.substr(expat.curByteOffset(), 30));
        if(textRuns.size())
            return;
    }
    text.append(UnicodeString::fromUTF8(StringPiece(utf8Text, strlen(utf8Text))));
    textRuns.addAttribute(0, text.length(), baseStyle);

}



