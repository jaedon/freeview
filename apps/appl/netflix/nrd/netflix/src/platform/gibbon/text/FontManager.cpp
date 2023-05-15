/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "FontManager.h"
#include "UnicodeBlocks.h"
#include <GibbonConfiguration.h>
#include <GibbonApplication.h>
#include <GibbonBridge.h>
#include <GibbonFreetype.h>

#include <nrd/AppLog.h>
#include <nrdbase/Configuration.h>
#include <nrdbase/ConfigData.h>
#include <nrdbase/Dumper.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/StringUtils.h>

#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include <bitset>

#ifdef BUILD_DEBUG // code to get a font's versionString field
# include <ft2build.h>
# include FT_FREETYPE_H
# include FT_SFNT_NAMES_H
# include FT_TRUETYPE_IDS_H
# include FT_TRUETYPE_TABLES_H
# include <unicode/unistr.h>
#endif

using namespace netflix;
using namespace netflix::gibbon;

#define FMDEBUG(s) NTRACES(TRACE_GIBBON_FONTS,s)

FontManager::FontRegistry FontManager::mFontRegistry;
FontManager::FontCache FontManager::mFontCache;
FontManager::FontAliases FontManager::mFontAliases;
std::set<unsigned long> FontManager::sReported;
bool FontManager::sHasReported = false;

static Mutex sFontManagerFontCacheMutex(netflix::UNTRACKED_MUTEX, "GibbonFontManagerFontCacheMutex");
extern bool pathAbsolute(const std::string &path);

std::vector<std::string> split(const std::string &str, const std::string &delim)
{
    std::string::size_type start = 0;
    std::string::size_type end;
    std::vector<std::string> v;

    while((end = str.find(delim, start)) != std::string::npos) {
        v.push_back(str.substr(start, end-start));
        start = end + delim.length();
    }
    v.push_back(str.substr(start));
    return v;
}

//-----------------------------------------------------------------------------------------------------------

namespace netflix {
namespace gibbon {
class FontManagerDumper : public Dumper
{
public:
    FontManagerDumper() : mMode(Normal) { }

    enum Mode { Normal, Languages, Blocks };
    void setMode(Mode m) { mMode = m; }

protected:
    virtual std::vector<std::string> consoleEntry(const unsigned int idx, const std::string& section, const Variant& value);
    virtual std::vector<std::string> consolePostProcess(const Variant& values);

private:
    Mode mMode;
};

std::vector<std::string> FontManagerDumper::consoleEntry(const unsigned int, const std::string&, const Variant& value)
{
    const std::string textFontPath = GibbonConfiguration::textFontPath();
    std::string filename = value["filename"].string();
    if(filename.substr(0, textFontPath.length()) == textFontPath)
        filename = filename.substr(textFontPath.length() + 1, std::string::npos);

    switch (mMode) {
    case Normal: {
        std::stringstream text;
        text << (value["bold"].boolean() ? "B" : "-");
        text << (value["italic"].boolean() ? "I" : "-");
        text << " ";

        const int attributes = value["attributes"].value<int>();
        if(attributes & FontManager::Synthesize)
            text << "Syn ";
        else
            text << "--- ";

        if(attributes & FontManager::Embolden)
            text << "Emb ";
        else
            text << "--- ";

        if(attributes & FontManager::Slant)
            text << "Sla ";
        else
            text << "--- ";

        if(attributes & FontManager::Monospace)
            text << "Mon ";
        else
            text << "--- ";


        text << " [";

        if(attributes & FontManager::Hinting)
            text << "H";
        else
            text << "-";

        if(attributes & FontManager::Autohinter)
            text << "A";
        else
            text << "-";

        if(attributes & FontManager::HintLight)
            text << "L";
        else if(attributes & FontManager::HintNormal)
            text << "N";
        else if(attributes & FontManager::HintMono)
            text << "S";
        else if(attributes & FontManager::HintLcd)
            text << "C";
        else if(attributes & FontManager::HintLcdV)
            text << "V";
        else
            text << "-";

        text << "] (" << std::setw(3) << value["priority"].value<int>() << ")";

        std::string url = value["downloaded"]["request"]["url"].value<std::string>();
        if(!url.empty())
            filename = url;

        return one(StringFormatter::sformat(" %30s: %s %s [%s]", value["family"].string().c_str(),
                                            text.str().c_str(), filename.c_str(), value["version"].string().c_str())); }
    case Languages: {
        std::vector<std::string> str;
        const Variant& langs = value["languages"];
        if(langs.isArray()) {
            str.push_back(" ");
            str.push_back(StringFormatter::sformat(" %s (%s):", value["family"].string().c_str(),
                                                   filename.c_str()));
            Variant::ArrayIterator it = langs.arrayBegin();
            const Variant::ArrayIterator end = langs.arrayEnd();
            while(it != end) {
                str.push_back(StringFormatter::sformat(" - %s", it->string().c_str()));
                ++it;
            }
        }
        return str; }
    case Blocks: {
        std::vector<std::string> str;
        const Variant& blocks = value["blocks"];
        if(blocks.isArray()) {
            str.push_back(" ");
            str.push_back(StringFormatter::sformat(" %s (%s):", value["family"].string().c_str(),
                                                   filename.c_str()));
            Variant::ArrayIterator it = blocks.arrayBegin();
            const Variant::ArrayIterator end = blocks.arrayEnd();
            while(it != end) {
                std::string name;
                const unsigned int start = it->mapValue<unsigned int>("start");
                if(const UnicodeBlocks::Range *range = UnicodeBlocks::find(start))
                    name = " (" + std::string(range->name) + ")";
                str.push_back(StringFormatter::sformat(" - U+%06X-U+%06X%s", start,
                                                       it->mapValue<unsigned int>("end"),
                                                       name.c_str()));
                ++it;
            }
        }
        return str; }
    }
    return std::vector<std::string>();
}

std::vector<std::string> FontManagerDumper::consolePostProcess(const Variant& values)
{
    std::vector<std::string> str;
    str.push_back(" ");
    str.push_back("Loaded fonts:");
    const Variant cache = values["cache"];
    if(cache.isArray()) {
        Variant::ArrayIterator it = cache.arrayBegin();
        const Variant::ArrayIterator end = cache.arrayEnd();
        while(it != end) {
            str.push_back(StringFormatter::sformat(" %s - References: %d",
                                                   (*it)["loaded"].string().c_str(),
                                                   (*it)["references"].value<int>()));
            ++it;
        }
    }
    return str;
}
}} // namespace netflix::gibbon

shared_ptr<FontManagerDumper> FontManager::dumper(const DumpInfo& dump)
{
    NRDP_UNUSED(dump);

    shared_ptr<FontManagerDumper> dumper(new FontManagerDumper);
    {
        dumper->keys() << "bold" << "italic" << "attributes" << "priority" << "family" << "filename" << "version" << "downloaded" << "languages" << "blocks";

        FontRegistry::const_iterator it = mFontRegistry.begin();
        FontRegistry::const_iterator end = mFontRegistry.end();
        for(; it != end; ++it) {
            Variant lang, blocks;
            for(std::vector<Language>::size_type i = 0; i < it->second.languages.size(); ++i) {
                lang.push_back(it->second.languages[i].toString());
            }
            for(std::vector<UnicodeBlock>::size_type i = 0; i < it->second.unicodeBlocks.size(); ++i) {
                Variant block;
                block["start"] = it->second.unicodeBlocks[i].start;
                block["end"] = it->second.unicodeBlocks[i].end;
                blocks.push_back(block);
            }
            dumper->values() << (it->second.weight == Font::WEIGHT_BOLD)
                             << (it->second.style == Font::STYLE_ITALIC)
                             << it->second.attributes
                             << it->second.priority
                             << it->first
                             << it->second.fileName
                             << it->second.version
                             << it->second.downloaded
                             << lang << blocks;
        }
    }

    {
        dumper->next("cache");
        dumper->keys() << "loaded" << "references";
        FontCache::const_iterator it = mFontCache.begin();
        FontCache::const_iterator end = mFontCache.end();

        for(; it != end; ++it) {
            if(it->second.use_count() != 0)
                dumper->values() << it->first << it->second.use_count();
        }
    }

    return dumper;
}

Variant FontManager::fontInfo()
{
    const shared_ptr<FontManagerDumper> d = dumper();
    return d->variant();
}

//-----------------------------------------------------------------------------------------------------------

FontManager::Language::Language(const std::string &languageString)
{
    std::vector<std::string> v = split(languageString, "-");
    if(v.size()>=3)
        country = v[2];
    if(v.size()>=2)
        script = v[1];
    if(v.size()>=1)
        language = v[0];
}

std::string FontManager::Language::toString() const
{
    std::ostringstream is;
    is << language;
    if(!script.empty()) {
        is << "-" << script;
        if(!country.empty()) {
            is << "-" << country;
        }
    }

    return is.str();
}

//-----------------------------------------------------------------------------------------------------------

int FontManager::FontRegistryItem::supportsLanguage(std::string language) const
{
    lowerCase(language);
    Language search(language);

    int score = 0; // No match
    for(std::vector<Language>::size_type i = 0; i < languages.size(); i++) {
        int candidate = 0;
        Language l = languages[i];
        lowerCase(l.language);
        lowerCase(l.script);
        lowerCase(l.country);

        if(l.language == "*" || search.language == l.language) {
            candidate += 1;

            if(l.script == "*" || search.script == l.script) {
                candidate += 2;

                if(l.country == "*" || search.country == l.country) {
                    candidate += 4;
                }
            }
        }
        if(candidate == 7)
            return 7; //Direct match
        else if(candidate > score)
            score = candidate;
    }
    return score;
}

//-----------------------------------------------------------------------------------------------------------

// This is the index for a font registry item
std::string FontManager::getFontName(const std::string& family, Font::Weight weight, Font::Style style, uint32_t size)
{
    std::ostringstream str;

    str << family;

    switch(weight) {
    case Font::WEIGHT_NORMAL:
        break;
    case Font::WEIGHT_BOLD:
        str << " Bold";
        break;
    default:
        break;
    }

    switch(style) {
    case Font::STYLE_NORMAL:
        break;
    case Font::STYLE_ITALIC:
        str << " Italic";
        break;
    default:
        break;
    }

    if(size != 0)
        str << " " << size;

    return str.str();
}

//-----------------------------------------------------------------------------------------------------------

bool FontManager::registerFont(const FontRegistryItem& item)
{
    sHasReported = false;
    sReported.clear();

    std::string fontDesc = getFontName(item.family, item.weight, item.style, 0);
    FMDEBUG("Registering font " << fontDesc << " (" << item.fileName << "): " << item.fileName << (item.downloaded.empty() ? "" : "@" + item.downloaded.toJSON()));

    if(item.ttf.isEmpty() && !Configuration::resourceExists(item.fileName)) {
        Log::info(TRACE_UI_ENGINE) << "Font file for " << fontDesc << " (" << item.fileName << ") not found, ignoring...";
        return false;
    }

    const std::string patchDesc = item.patch.empty() ? std::string() : getFontName(item.patch, item.weight, item.style, 0);

    if (!mFontRegistry.insert(FontRegistryEntry(fontDesc, item)).second) {
        Log::info(TRACE_UI_ENGINE) << "Font " << fontDesc << " already exists";
        return false;
    }
    for(std::vector<UnicodeBlock>::size_type i = 0; i < item.unicodeBlocks.size(); i++) {
        if(UnicodeBlocks::Range *r = UnicodeBlocks::find(item.unicodeBlocks[i].start))
            r->fonts.push_back(fontDesc);
    }

    if(!patchDesc.empty())
        patchFonts(patchDesc, fontDesc, item.fileName, item.unicodeBlocks);

    return true;
}

bool FontManager::unregisterFont(const std::string& family, netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style)
{
    std::string fontDesc = getFontName(family, weight, style, 0);
    FMDEBUG("Unregistering font " << fontDesc);

    FontRegistry::iterator it = mFontRegistry.find(fontDesc);
    if(it == mFontRegistry.end()) {
        Log::error(TRACE_UI_ENGINE, "Font not found '%s' (%s)", fontDesc.c_str(), family.c_str());
        return false;
    }
    FontRegistryItem& item = it->second;
    if(item.downloaded.empty()) {
        Log::error(TRACE_UI_ENGINE, "Font not a downloaded font '%s' (%s)", fontDesc.c_str(), family.c_str());
        return false;
    }
    for(std::vector<UnicodeBlock>::size_type i = 0; i < item.unicodeBlocks.size(); i++) {
        if(UnicodeBlocks::Range *r = UnicodeBlocks::find(item.unicodeBlocks[i].start)) {
            UnicodeBlocks::Fonts::iterator it = r->fonts.begin();
            while (it != r->fonts.end()) {
                if (*it == fontDesc) {
                    it = r->fonts.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    if(!item.patch.empty())
        unpatchFonts(item.patch, fontDesc, item.fileName);

    Font::unregister(item.fileName, item.attributes);
    mFontRegistry.erase(it);

    return true;
}

void FontManager::patchFonts(const std::string& from, const std::string& to, const std::string& fn, const std::vector<UnicodeBlock> &blocks)
{
    FontRegistry::iterator it = mFontRegistry.find(from);
    if(it != mFontRegistry.end())
        it->second.patches[to] = blocks;
    updatePatches(from, fn);
}

void FontManager::unpatchFonts(const std::string& from, const std::string& to, const std::string& fn)
{
    FontRegistry::iterator it = mFontRegistry.find(from);
    if(it != mFontRegistry.end())
        it->second.patches.erase(to);
    updatePatches(from, fn);
}

void FontManager::updatePatches()
{
    FontRegistry::const_iterator it = mFontRegistry.begin();
    const FontRegistry::const_iterator end = mFontRegistry.end();
    while (it != end) {
        const FontRegistryItem& item = it->second;
        if (!item.patch.empty()) {
            updatePatches(item.patch, item.fileName);
        }
        ++it;
    }
}

void FontManager::updatePatches(const std::string& from, const std::string& fn)
{
    std::map<long unsigned int, long unsigned int> ranges;
    {
        const FontRegistry::iterator it = mFontRegistry.find(from);
        if(it != mFontRegistry.end()) {
            std::map<std::string, std::vector<UnicodeBlock> >::const_iterator p = it->second.patches.begin();
            while(p != it->second.patches.end()) {
                std::vector<UnicodeBlock>::const_iterator u = p->second.begin();
                while(u != p->second.end()) {
                    ranges[u->start] = std::max(u->end, ranges[u->start]);
                    ++u;
                }
                ++p;
            }
        }
    }

    // for all the fonts in a family
    FontCache::iterator f = mFontCache.lower_bound(from);
    while(f != mFontCache.end()) {
        if (!startsWith(f->first, from))
            break;
        if(Font::SharedPtr font = f->second.lock()) {
            if (font->getFileName() == fn) {
                ++f;
                continue;
            }
            font->setExceptions(ranges);
        }
        ++f;
    }
}

void FontManager::clearDownloadedFonts(unsigned int mode)
{
    FontRegistry::iterator it = mFontRegistry.begin();
    while (it != mFontRegistry.end()) {
        const std::string& fontDesc = it->first;
        const FontRegistryItem& item = it->second;
        if ((!item.downloaded.isNull()) && (mode & All || ((mode & Persisted) && item.persist) || ((mode & NonPersisted) && !item.persist))) {
            for(std::vector<UnicodeBlock>::size_type i = 0; i < item.unicodeBlocks.size(); i++) {
                if(UnicodeBlocks::Range *r = UnicodeBlocks::find(item.unicodeBlocks[i].start)) {
                    UnicodeBlocks::Fonts::iterator uit = r->fonts.begin();
                    while (uit != r->fonts.end()) {
                        if (*uit == fontDesc) {
                            uit = r->fonts.erase(uit);
                        } else {
                            ++uit;
                        }
                    }
                }
            }
            if (!item.patch.empty())
                unpatchFonts(item.patch, fontDesc, item.fileName);
            mFontRegistry.erase(it++);
        } else {
            ++it;
        }
    }
    if (shared_ptr<GibbonBridge> bridge = gibbonApp()->getBridge())
        bridge->fontsChanged();
}

bool FontManager::shouldReportMissing(unsigned long codePoint)
{
    if (sReported.find(codePoint) != sReported.end())
        return false;
    sHasReported = true;
    sReported.insert(codePoint);
    return true;
}

void FontManager::reportMissing(unsigned long codePoint, const std::string& familyName)
{
    // Do not report missing the control characters:
    //  U+00A0 (NON-BREAKING SPACE)
    //  U+200B (ZERO WIDTH SPACE)
    //  U+200C (ZERO WIDTH NON-JOINER)
    //  U+200D (ZERO WIDTH JOINER)
    //  U+200E (LEFT-TO-RIGHT MARK)
    //  U+200F (RIGHT-TO-LEFT MARK)
    //  U+FEFF (ZERO WIDTH NO-BREAK SPACE)

    if(shouldReportMissing(codePoint) && codePoint != 0x00a0 && codePoint != 0x200b
       && codePoint != 0x200c && codePoint != 0x200d && codePoint != 0x200e
       && codePoint != 0x200f && codePoint != 0xfeff) {
        Log::error(TRACE_GIBBON_TEXT) << " No glyph for code point U+" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << codePoint;
    }

    if(shared_ptr<GibbonBridge> bridge = gibbonApp()->getBridge()) {
        bridge->reportCodepointMissing(familyName, codePoint);
    }
}

Font::SharedPtr FontManager::getFontForCodePoint(unsigned long codePoint, const std::string& currentFamilyName, uint32_t size,
                                                 netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style,
                                                 std::string locale, unsigned long* rangeStart, unsigned long* rangeEnd)
{
    if (sHasReported && sReported.find(codePoint) != sReported.end())
        return Font::SharedPtr();
    if (codePoint < 32) {
        FMDEBUG("Requesting control character. Not a hit.");
        return Font::SharedPtr();
    }

    // Do not search for non-printable/control characters

    if (codePoint == 0x2029 || // PARAGRAPH SEPARATOR
        codePoint == 0x200e || // LEFT-TO-RIGHT MARK
        codePoint == 0x200f || // RIGHT-TO-LEFT MARK
        codePoint == 0x202a || // LEFT-TO-RIGHT EMBEDDING
        codePoint == 0x202b || // RIGHT-TO-LEFT EMBEDDING
        codePoint == 0x202c || // POP DIRECTIONAL FORMATTING
        codePoint == 0x202d || // LEFT-TO-RIGHT OVERRIDE
        codePoint == 0x202e || // RIGHT-TO-LEFT OVERRIDE
        codePoint == 0x2067 || // RIGHT-TO-LEFT ISOLATE
        codePoint == 0x2068 || // FIRST STRONG ISOLATE
        codePoint == 0x2069 || // POP DIRECTIONAL ISOLATE

        codePoint == 0x200b || // ZERO WIDTH SPACE
        codePoint == 0x200c || // ZERO WIDTH NON-JOINER
        codePoint == 0x200d || // ZERO WIDTH JOINER
        codePoint == 0xfeff    // ZERO WIDTH NO-BREAK SPACE
    ) {
        return Font::SharedPtr();
    }

    std::vector<Match> r = getFontsForCodePoint(codePoint, currentFamilyName, weight, style, locale, rangeStart, rangeEnd);
    if(r.empty())
        return Font::SharedPtr();

    std::vector<Match>::const_iterator it = r.begin();
    std::vector<Match>::const_iterator end = r.end();

    for(; it != end; it++) {
        FMDEBUG("Checking if code point is present in " << *it );
        Font::SharedPtr font = getFontByIndex(it->index, size);
        if(font->hasCodepoint(codePoint)) {
            FMDEBUG("Hit! Returning");
            return font;
        } else {
            FMDEBUG("Not a hit");
        }
    }

    reportMissing(codePoint, currentFamilyName);
    return Font::SharedPtr(); // TODO: Possibility of last resort font here.
}


bool sortMatches(const FontManager::Match &first, const FontManager::Match &second)
{
    // Sort by score. If two scores are the same, look at priority.
    if(first.score != second.score)
        return first.score > second.score;
    return first.priority > second.priority;
}

std::vector<FontManager::Match> FontManager::getFontsForCodePoint(unsigned long codePoint, const std::string& currentFamilyName,
                                                                  netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style,
                                                                  std::string locale, unsigned long* rangeStart, unsigned long* rangeEnd)
{
    FMDEBUG("Requesting code point " << codePoint << " (U+" << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << codePoint << ")" << getFontName("", weight, style, 0) << " - W:" << (int)weight << " S:" << (int)style << " Language (global): " << GibbonConfiguration::defaultLocale() << " Language (widget): " << locale);

    std::vector<Match> matches;

    UnicodeBlocks::Range *r = UnicodeBlocks::find(codePoint);
    if(!r || r->fonts.size() == 0) {
        reportMissing(codePoint, currentFamilyName);
        return matches;
    }

    FMDEBUG("Found " << r->fonts.size() << " font(s)");
#if defined(BUILD_DEBUG)
    FMDEBUG(" Range '" << r->name << "' (U+" << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << r->start << "-U+" << std::setw(6) << r->end << ")");
    for(std::vector<UnicodeBlocks::Fonts>::size_type i = 0; i < r->fonts.size(); i++) {
        FMDEBUG(" -" << r->fonts[i]);
    }
#endif

    std::string searchLocale = locale;
    if(searchLocale.empty())
        searchLocale = GibbonConfiguration::defaultLocale();

    // Add weight to each matching property so we gauge the matching level
    const int NoMatch = 0;
    const int Weight = 1;
    const int Style = 2;
    //const int Language = 4;
    //const int Script = 8;
    //const int Country = 16;
    // Language, Script and Country are returned by supportsLanguage, we just shift the value and add it to the score

    UnicodeBlocks::Fonts::size_type candidateIndex = 9999;
    int candidateScore = 0;
    int candidatePriority = 0;

    for(UnicodeBlocks::Fonts::size_type i = 0; i < r->fonts.size(); i++) {
        int score = NoMatch;

        const FontRegistryItem match = mFontRegistry[r->fonts[i]];
        score += match.supportsLanguage(searchLocale) * 4;

        if(match.weight == weight)
            score += Weight;
        if(match.style == style)
            score += Style;

#if defined(BUILD_DEBUG)
        std::bitset<8> value(score);
        std::stringstream text;
        text << "Candidate: " << r->fonts[i] << ": " << value;

        text << " - Score = [" << score << "." << match.priority << "] - Current = " << candidateScore << "." << candidatePriority;

        FMDEBUG(text.str());
#endif
        if(score >= candidateScore && match.priority > candidatePriority) {
            candidateScore = score;
            candidateIndex = i;
            candidatePriority = match.priority;
            if(rangeStart)
                *rangeStart = r->start;
            if(rangeEnd)
                *rangeEnd = r->end;
            FMDEBUG(" " << r->fonts[i] << " selected");
        }

        //if(score > 0) {
            Match m = Match(getFontName(match.family, match.weight, match.style, 0), score, match.priority);
            matches.push_back(m);
        //}
    }

    std::sort(matches.begin(), matches.end(), sortMatches);
    return matches;
}

Font::SharedPtr FontManager::getFontByIndex(const std::string& index, uint32_t size)
{
    FontRegistry::const_iterator it = mFontRegistry.find(index);
    if(it == mFontRegistry.end()) {
        return Font::SharedPtr(new Font());
    } else {
        return getFont(it->second.family, size, it->second.weight, it->second.style);
    }
}

Font::SharedPtr FontManager::getFont(const std::string& family, uint32_t size, Font::Weight weight, Font::Style style)
{
    //FMDEBUG("Requesting " << getFontName(family, weight, style, size));

    Font::SharedPtr result;

    std::string reqFontDesc = getFontName(family, weight, style, size);

    {
        ScopedMutex mutex(sFontManagerFontCacheMutex);
        {
            // See if the requested font and size is already in the cache
            FontCache::iterator it = mFontCache.begin();
            FontCache::const_iterator end = mFontCache.end();

            for(; it != end; ++it) {
                //FMDEBUG("Looking for " << reqFontDesc << " - cache: " << it->first);
                if(reqFontDesc == it->first) {
                    result = it->second.lock();
                    if(result) {
                        //FMDEBUG("Returning " << reqFontDesc << " from the font cache");
                        result->ensureCached();
                        return result;
                    } else {
                        mFontCache.erase(it);
                        break;
                    }
                }
            }
        }

        {
            // Create a new font, return it and add it to the cache
            std::istringstream is(family);

            std::string _family;
            std::string _fileName;
            Font::Weight _weight = weight;
            Font::Style _style = style;
            DataBuffer _ttf;
            FontValues _fontValues;
            int _attributes = FontManager::None;
            Rect _bbox;
            int _cmap = -1;

            std::string fontDesc, patchDesc;

            FontRegistry::const_iterator it;
            FontRegistry::const_iterator end = mFontRegistry.end();

            while(std::getline(is, _family, ',')) {
                _family.erase(_family.begin(), std::find_if(_family.begin(), _family.end(), std::not1(std::ptr_fun<int, int>(std::isspace)))); //ltrim

                fontDesc = getFontName(_family, weight, style, 0);

                it = mFontRegistry.find(fontDesc);
                if(it == end) {
                    FMDEBUG("'" << fontDesc << "' not found, trying aliases");
                    const FontAliases::const_iterator aliasIt = mFontAliases.find(_family);
                    if(aliasIt != mFontAliases.end()) {
                        FMDEBUG(" Alias found: " << _family << " is " << aliasIt->second);
                        _family = aliasIt->second;
                        fontDesc = getFontName(_family, weight, style, 0);
                        it = mFontRegistry.find(fontDesc);
                    } else {
                        FMDEBUG(" Aliases not found for '" << _family << "'");
                    }
                }

                if(it == end) {
                    FMDEBUG(" Requested '" << fontDesc << "' not found");

                    // We didn't get an exact match. See if we can synthesize it
                    if((weight == Font::WEIGHT_BOLD) && (style == Font::STYLE_ITALIC)) {
                        // If the request is for Bold+Italic, see if we can embolden an italic style or slant a bold one
                        FMDEBUG("  Looking for '" << _family << " Italic'+Embolden");
                        fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_ITALIC);
                        it = mFontRegistry.find(fontDesc);
                        if(it != end && (it->second.attributes & FontManager::Synthesize)) {
                            _fileName = it->second.fileName;
                            _weight = Font::WEIGHT_BOLD;
                            _style = Font::STYLE_ITALIC;
                            _attributes = it->second.attributes | FontManager::Embolden;
                            _cmap = it->second.cmap;
                            _bbox = it->second.bbox;
                            _fontValues = it->second.fontValues;
                            _ttf = it->second.ttf;
                            patchDesc = it->second.patch;
                        } else {
                            FMDEBUG("  Looking for '" << _family << " Bold'+Slant");
                            fontDesc = getFontName(_family, Font::WEIGHT_BOLD, Font::STYLE_NORMAL);
                            it = mFontRegistry.find(fontDesc);
                            if(it != end && (it->second.attributes & FontManager::Synthesize)) {
                                _fileName = it->second.fileName;
                                _weight = Font::WEIGHT_BOLD;
                                _style = Font::STYLE_ITALIC;
                                _attributes = it->second.attributes | FontManager::Slant;
                                _cmap = it->second.cmap;
                                _bbox = it->second.bbox;
                                _fontValues = it->second.fontValues;
                                _ttf = it->second.ttf;
                                patchDesc = it->second.patch;
                            } else {
                                FMDEBUG("  Looking for '" << _family << "'+Slant+Embolden");
                                fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_NORMAL);
                                it = mFontRegistry.find(fontDesc);
                                if(it != end && (it->second.attributes & FontManager::Synthesize)) {
                                    _fileName = it->second.fileName;
                                    _weight = Font::WEIGHT_BOLD;
                                    _style = Font::STYLE_ITALIC;
                                    _attributes = it->second.attributes | (FontManager::Slant | FontManager::Embolden);
                                    _cmap = it->second.cmap;
                                    _bbox = it->second.bbox;
                                    _fontValues = it->second.fontValues;
                                    _ttf = it->second.ttf;
                                    patchDesc = it->second.patch;
                                } else {
                                    FMDEBUG("  Looking for '" << _family << " Italic'");
                                    fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_ITALIC);
                                    it = mFontRegistry.find(fontDesc);
                                    if(it != end) {
                                        _fileName = it->second.fileName;
                                        _weight = Font::WEIGHT_NORMAL;
                                        _style = Font::STYLE_ITALIC;
                                        _attributes = it->second.attributes;
                                        _cmap = it->second.cmap;
                                        _bbox = it->second.bbox;
                                        _fontValues = it->second.fontValues;
                                        _ttf = it->second.ttf;
                                        patchDesc = it->second.patch;
                                    } else {
                                        FMDEBUG("  Looking for '" << _family << " Bold'");
                                        fontDesc = getFontName(_family, Font::WEIGHT_BOLD, Font::STYLE_NORMAL);
                                        it = mFontRegistry.find(fontDesc);
                                        if(it != end) {
                                            _fileName = it->second.fileName;
                                            _weight = Font::WEIGHT_BOLD;
                                            _style = Font::STYLE_NORMAL;
                                            _attributes = it->second.attributes;
                                            _cmap = it->second.cmap;
                                            _bbox = it->second.bbox;
                                            _fontValues = it->second.fontValues;
                                            _ttf = it->second.ttf;
                                            patchDesc = it->second.patch;
                                        } else {
                                            it = end;
                                        }
                                    }
                                }
                            }
                        }

                    } else if(weight == Font::WEIGHT_BOLD) {
                        FMDEBUG("  Looking for '" << _family << "'+Synthesize to embolden");
                        fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_NORMAL);
                        it = mFontRegistry.find(fontDesc);
                        if(it != end && (it->second.attributes & FontManager::Synthesize)) {
                            _fileName = it->second.fileName;
                            _weight = Font::WEIGHT_BOLD;
                            _style = Font::STYLE_NORMAL;
                            _attributes = it->second.attributes | FontManager::Embolden;
                            _cmap = it->second.cmap;
                            _bbox = it->second.bbox;
                            _fontValues = it->second.fontValues;
                            _ttf = it->second.ttf;
                            patchDesc = it->second.patch;
                        } else {
                            it = end;
                        }

                    } else if(style == Font::STYLE_ITALIC) {
                        FMDEBUG("  Looking for '" << _family << "'+Synthesize to slant");
                        fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_NORMAL);
                        it = mFontRegistry.find(fontDesc);
                        if(it != end && (it->second.attributes & FontManager::Synthesize)) {
                            _fileName = it->second.fileName;
                            _weight = Font::WEIGHT_NORMAL;
                            _style = Font::STYLE_ITALIC;
                            _attributes = it->second.attributes | FontManager::Slant;
                            _cmap = it->second.cmap;
                            _bbox = it->second.bbox;
                            _fontValues = it->second.fontValues;
                            _ttf = it->second.ttf;
                            patchDesc = it->second.patch;
                        } else {
                            it = end;
                        }
                    }
                } else {
                    FMDEBUG("Found '" << fontDesc << "'");

                    _family = it->second.family;
                    _fileName = it->second.fileName;
                    _weight = it->second.weight;
                    _style = it->second.style;
                    _attributes = it->second.attributes;
                    _cmap = it->second.cmap;
                    _bbox = it->second.bbox;
                    _fontValues = it->second.fontValues;
                    _ttf = it->second.ttf;
                    patchDesc = it->second.patch;
                }
            }

            if(it == end) {
                if(weight == Font::WEIGHT_BOLD || style == Font::STYLE_ITALIC) {
                    fontDesc = getFontName(_family, Font::WEIGHT_NORMAL, Font::STYLE_NORMAL, 0);
                    if(weight == Font::WEIGHT_BOLD) {
                        FMDEBUG(" " << _family << " Bold not found, trying " << _family << " Regular");
                    } else {
                        FMDEBUG(" " << _family << " Italic not found, trying " << _family << " Regular");
                    }
                    it = mFontRegistry.find(fontDesc);
                }

                if(it == end) {
                    fontDesc = getFontName("Arial_for_Netflix", weight, style, 0);
                    FMDEBUG("Font not found, trying the default family: " << fontDesc);
                    it = mFontRegistry.find(fontDesc);

                    if(it == end) {
                        fontDesc = getFontName("Arial_for_Netflix", Font::WEIGHT_NORMAL, Font::STYLE_NORMAL, 0);
                        FMDEBUG("Font still not found, reverting to the base font: " << fontDesc);
                        it = mFontRegistry.find(fontDesc);

                        if(it == end ){
                            Log::error(TRACE_GIBBON_TEXT, "Can't find the default font!");
                            result.reset(new Font("", "", 0, Font::WEIGHT_NORMAL, Font::STYLE_NORMAL, 0, FontValues()));
                            return result;
                        }
                    }
                }

                _family = it->second.family;
                _fileName = it->second.fileName;
                _weight = it->second.weight;
                _style = it->second.style;
                _attributes = it->second.attributes;
                _cmap = it->second.cmap;
                _bbox = it->second.bbox;
                _fontValues = it->second.fontValues;
                _ttf = it->second.ttf;
                patchDesc = it->second.patch;
            }

            std::ostringstream fdesc;
            fdesc << getFontName(_family, _weight, _style, size);
            fontDesc = fdesc.str();

            FMDEBUG(": Returning " << fontDesc);

            result.reset(new Font(_family, _fileName, size, _weight, _style, _attributes, _fontValues, _cmap, _bbox, _ttf));
            mFontCache.insert(FontCacheItem(fontDesc, weak_ptr<Font>(result)));
            updatePatches();
            FMDEBUG(" Adding " << fontDesc << " to the font cache");

            return result;
        }
    }
}

static inline void setBit(int &flags, const int flag, const bool value)
{
    if(value)
        flags |= flag;
    else
        flags &= ~(flag);
}


static int getAttributes(const Variant& settings, const int previousFlags = FontManager::None)
{
    bool ok = false;
    int result = previousFlags;

    bool value = settings["hinting"].value<bool>(&ok, true);
    if(ok) {
        setBit(result, FontManager::Hinting, value);
    }

    value = settings["autohinter"].value<bool>(&ok, true);
    if(ok) {
        setBit(result, FontManager::Autohinter, value);
    }

    value = settings["embolden"].value<bool>(&ok, true);
    if(ok) {
        setBit(result, FontManager::Embolden, value);
    }

    value = settings["slant"].value<bool>(&ok, true);
    if(ok) {
        setBit(result, FontManager::Slant, value);
    }

    value = settings["monospace"].value<bool>(&ok, true);
    if(ok) {
        setBit(result, FontManager::Monospace, value);
    }

    value = settings["synthesize"].value<bool>(&ok, false);
    if(ok) {
        setBit(result, FontManager::Synthesize, value);
    }

    const std::string v = settings["hinting_mode"].value<std::string>();
    if(!v.empty()) {
        setBit(result, FontManager::HintNormal, false);
        setBit(result, FontManager::HintLight, false);
        setBit(result, FontManager::HintMono, false);
        setBit(result, FontManager::HintLcd, false);
        setBit(result, FontManager::HintLcdV, false);

        if(v == "normal")
            result |= FontManager::HintNormal;
        if(v == "light")
            result |= FontManager::HintLight;
        if(v == "mono" || v == "strong")
            result |= FontManager::HintMono;
        if(v == "lcd")
            result |= FontManager::HintLcd;
        if(v == "lcdv")
            result |= FontManager::HintLcdV;
    }

    return result;
}

FontValues getFontValues(const Variant &values, const FontValues &previousValues)
{
    FontValues result = previousValues;

    bool ok = false;
    double embolden = values["embolden_factor"].value<double>(&ok, 0);
    if(ok) {
        result.emboldenFactor = embolden;
    }

    double slant = values["slant_factor"].value<double>(&ok, 0);
    if(ok) {
        result.slantFactor = slant;
    }

    return result;
}

#ifdef BUILD_DEBUG
void fromUni16be(FT_Byte* string, FT_UInt string_len, std::string &utf8)
{
    icu::UnicodeString str(string_len/2, 0 ,0);
    UChar32 ch = 0;

    for (FT_UInt i = 0; i < string_len; i += 2) {
        ch = (string[i] << 8) | string[i + 1];
        str.append(ch);
    }

    str.toUTF8String(utf8);
}
#endif

void FontManager::addFontsFromDirectory(const std::string &path)
{
    ConfigData config;
    const std::string configFile = path + "/fonts.xml";

#ifdef BUILD_DEBUG
    FT_Error ftError;
    FT_Library ftLibrary;
    FT_Face ftFace;

    ftError = FT_Init_FreeType(&ftLibrary);
    if (ftError) {
        Log::error(TRACE_GIBBON_FONTS, "FreeType: Unable to initialize freetype: %s", GibbonFreetype::freetypeErrorString(ftError));
        return;
    }
#endif

    if(!config.init(configFile))
        Log::error(TRACE_GIBBON_FONTS, "Unable to parse fonts configuration file: %s", configFile.c_str());

    Variant settings = config.values()["fontManager"]["settings"];
    int defaultAttributes = getAttributes(settings);

    Variant values = config.values()["fontManager"]["settings"];
    const FontValues defaultValues = getFontValues(values, FontValues());

    Variant fonts = config.values()["fontManager"]["fonts"];
    const Variant::StringMapIterator begin = fonts.stringMapBegin();
    const Variant::StringMapIterator end = fonts.stringMapEnd();

    if(begin == end) {
        Log::error(TRACE_UI_ENGINE, "No fonts defined in %s", configFile.c_str());
    }

    struct styleMap {
        std::string styleName;
        Font::Weight weight;
        Font::Style style;
    };

    styleMap sm[5] = {
        { "regular", Font::WEIGHT_NORMAL, Font::STYLE_NORMAL },
        { "bold", Font::WEIGHT_BOLD, Font::STYLE_NORMAL },
        { "italic", Font::WEIGHT_NORMAL, Font::STYLE_ITALIC },
        { "boldItalic", Font::WEIGHT_BOLD, Font::STYLE_ITALIC },
        { "", Font::WEIGHT_NORMAL, Font::STYLE_NORMAL }
    };

    int fontCount = 0;
    for(Variant::StringMapIterator it = begin; it != end; ++it) {
        const std::pair<std::string, Variant> &node = *it;
        const Variant settings = node.second["settings"];
        int fontAttributes = getAttributes(settings, defaultAttributes);
        FontValues fontValues = getFontValues(settings, defaultValues);

        std::string aliases = settings["aliases"].value<std::string>();
        if(!aliases.empty()) {
            std::istringstream is(aliases);
            std::string alias;
            while(std::getline(is, alias, ',')) {
                alias.erase(alias.begin(), std::find_if(alias.begin(), alias.end(), std::not1(std::ptr_fun<int, int>(std::isspace)))); //ltrim
                mFontAliases[alias] = node.first;
            }
        }

        int i = 0;
        while(!sm[i].styleName.empty()) {
            const Variant& font = node.second[sm[i].styleName];
            const Variant& settings = font["settings"];
            const Rect bbox = Rect::fromString(settings["bbox"].string());
            const int cmap = settings["cmap"].value<int>(0, -1);
            const int variantAttributes = getAttributes(settings, fontAttributes);
            const FontValues variantValues = getFontValues(settings, fontValues);
            std::string file = font["file"].string();
            if(!file.empty()) {
                std::vector<UnicodeBlock> unicodeBlocks;
                std::vector<Language> languages;
                std::string version;

                if(!pathAbsolute(file))
                    file = Configuration::dataReadPath() + "/" + file;

                const std::string& languagesString = font["languages"].value<std::string>();
                if(!languagesString.empty()) {
                    std::vector<std::string> v = split(languagesString, ",");

                    for(std::vector<std::string>::const_iterator it = v.begin(); it != v.end(); ++it) {
                        languages.push_back(Language(*it));
                    }
                }

                const int priority = font["priority"].value<int>(0, 100);

                const Variant& blocks = font["blocks"];
                const Variant::StringMapIterator bBegin = blocks.stringMapBegin();
                const Variant::StringMapIterator bEnd = blocks.stringMapEnd();

                if(bBegin != bEnd) {
                    for(Variant::StringMapIterator bIt = bBegin; bIt != bEnd; ++bIt) {
                        const std::pair<std::string, Variant> &block = *bIt;
                        std::string value = block.second.value<std::string>();
                        if(!value.empty()) {
                            size_t pos = value.find("-");
                            if(pos != std::string::npos) {
                                std::string start = value.substr(0, pos);
                                std::string end = value.substr(pos + 1);
                                unsigned long blockStart;
                                unsigned long blockEnd;
                                sscanf(start.c_str(), "%lx", &blockStart);
                                sscanf(end.c_str(), "%lx", &blockEnd);
                                unicodeBlocks.push_back(UnicodeBlock(blockStart, blockEnd));
                            }
                        }
                    }
                }

#ifdef BUILD_DEBUG
                ftError = FT_New_Face(ftLibrary, file.c_str(), 0, &ftFace);
                if (ftError) {
                    Log::error(TRACE_GIBBON_FONTS, "Unable to open font file %s (%s)\n", file.c_str(), GibbonFreetype::freetypeErrorString(ftError));
                } else {
                    FT_SfntName name;
                    FT_UInt num_names, i;

                    num_names = FT_Get_Sfnt_Name_Count(ftFace);
                    for (i = 0; i < num_names; i++) {
                        ftError = FT_Get_Sfnt_Name(ftFace, i, &name);
                        if (ftError == FT_Err_Ok) {
                            if (name.name_id == TT_NAME_ID_VERSION_STRING) {
                                switch (name.platform_id) {
                                case TT_PLATFORM_APPLE_UNICODE:
                                    switch (name.encoding_id) {
                                        case TT_APPLE_ID_DEFAULT:
                                        case TT_APPLE_ID_UNICODE_1_1:
                                        case TT_APPLE_ID_ISO_10646:
                                        case TT_APPLE_ID_UNICODE_2_0:
                                            fromUni16be(name.string, name.string_len, version);
                                            break;
                                        default:
                                            break;
                                    }
                                    break;

                                case TT_PLATFORM_MACINTOSH:
                                    if (name.encoding_id == TT_MAC_ID_ROMAN) {
                                        version = std::string(reinterpret_cast<const char*>(name.string), name.string_len);
                                    } else {
                                        break;
                                    }
                                    break;

                                case TT_PLATFORM_ISO:
                                    switch (name.encoding_id) {
                                        case TT_ISO_ID_7BIT_ASCII:
                                        case TT_ISO_ID_8859_1:
                                            version = std::string(reinterpret_cast<const char*>(name.string), name.string_len);
                                            break;
                                        case TT_ISO_ID_10646:
                                            fromUni16be(name.string, name.string_len, version);
                                            break;
                                        default:
                                            break;
                                    }
                                    break;

                                case TT_PLATFORM_MICROSOFT:
                                    switch (name.encoding_id) {
                                        case TT_MS_ID_SYMBOL_CS:
                                        case TT_MS_ID_UNICODE_CS:
                                            fromUni16be(name.string, name.string_len, version);
                                            break;
                                        default:
                                            break;
                                    }
                                    break;

                                default:
                                    break;

                                }
                                if (!version.empty())
                                    break;
                            }
                        }
                    }

                    FT_Done_Face(ftFace);
                }
#endif

                FontRegistryItem item(file.c_str(), node.first, sm[i].weight, sm[i].style, variantAttributes, priority, unicodeBlocks, languages, variantValues, version, cmap, bbox);
                FontManager::registerFont(item);
                ++fontCount;
            }
            ++i;
        }
    }

#ifdef BUILD_DEBUG
    FT_Done_FreeType(ftLibrary);
#endif

    if(!fontCount)
        Log::error(TRACE_UI_ENGINE, "No fonts present in %s", path.c_str());
    else if(shared_ptr<GibbonBridge> bridge = gibbonApp()->getBridge())
        bridge->fontsChanged();
}

void FontManager::dumpFontLinkingTree(unsigned long codePoint, Font::Weight weight, Font::Style style, std::string locale)
{
    if(codePoint == 0) {
        Log::warn(TRACE_UI_ENGINE, "Font Linking tree:");
        UnicodeBlocks::dumpFonts();
    } else {
        std::stringstream text;

        text << "Getting font for U+" << std::hex << std::uppercase << std::setw(6) << std::setfill('0') << codePoint << " W:";
        if(weight == Font::WEIGHT_NORMAL) {
            text << "N";
        } else {
            text << "B";
        }

        text << " S:";

        if(style == Font::STYLE_NORMAL) {
            text << "N";
        } else {
            text << "I";
        }

        text << " locale: ";

        std::string l = locale;

        if(l.empty()) {
            l = GibbonConfiguration::defaultLocale();
            text << "system:";
        }

        text << l;

        Log::warn(TRACE_UI_ENGINE) << text.str();

        Log::warn(TRACE_UI_ENGINE) << getFontsForCodePoint(codePoint, "", weight, style, l, 0, 0);
        //Font::SharedPtr font = getFontForCodePoint(codePoint, 0, weight, style, l, 0, 0);
        //Log::warn(TRACE_UI_ENGINE) << font->getFamily() << " [" << font->getWeight() << " " << font->getStyle() << "]";
    }
}

void FontManager::dumpFontStatistics(bool verbose)
{
    std::string textFontPath = GibbonConfiguration::textFontPath();
    Log::warn(TRACE_UI_ENGINE, "Available fonts (%s):", textFontPath.c_str());
    const shared_ptr<FontManagerDumper> d = dumper();

    std::vector<std::string> data = d->toConsole();
    for(std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
        Log::warn(TRACE_GIBBON_SURFACE, "%s", it->c_str());
    }

    if(verbose) {
        d->setMode(FontManagerDumper::Languages);
        data = d->toConsole();
        Log::warn(TRACE_UI_ENGINE, " ");
        Log::warn(TRACE_UI_ENGINE, "Languages supported:");
        for(std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
            Log::warn(TRACE_GIBBON_SURFACE, "%s", it->c_str());
        }

        d->setMode(FontManagerDumper::Blocks);
        data = d->toConsole();
        Log::warn(TRACE_UI_ENGINE, " ");
        Log::warn(TRACE_UI_ENGINE, "Unicode blocks supported:");
        for(std::vector<std::string>::const_iterator it = data.begin(); it != data.end(); ++it) {
            Log::warn(TRACE_GIBBON_SURFACE, "%s", it->c_str());
        }
    }
}

