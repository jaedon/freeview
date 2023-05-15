/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef FONTMANAGER_H_
#define FONTMANAGER_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>
#include <nrdbase/tr1.h>
#include <nrdbase/DumpInfo.h>
#include <Font.h>
#include "Style.h"
#include <Rect.h>
#include <FontManagerValues.h>

namespace netflix {
namespace gibbon {
class FontManagerDumper;

class FontManager
{
public:
    enum FontAttributes {
        None        = 0x00000000,
        Embolden    = 0x00000001,
        Slant       = 0x00000002,
        Monospace   = 0x00000004,
        Synthesize  = 0x00000008,   // Automatically create the bold or italic variants without having to add them to fonts.xml

        Hinting     = 0x00000100,
        Autohinter  = 0x00000200,
        HintNormal  = 0x00000400,
        HintLight   = 0x00000800,
        HintMono    = 0x00001000,
        HintLcd     = 0x00002000,
        HintLcdV    = 0x00004000
    };

    struct UnicodeBlock
    {
        unsigned long start;
        unsigned long end;
        UnicodeBlock() : start(0), end(0) {}
        UnicodeBlock(unsigned long _start, unsigned long _end) : start(_start), end(_end) {}
    };

    struct Language
    {
        std::string language;
        std::string script;
        std::string country;
        Language(const std::string &languageString);
        std::string toString() const;
    };

    struct FontRegistryItem {
        std::string fileName;
        std::string family;
        netflix::gibbon::Font::Weight weight;
        netflix::gibbon::Font::Style style;
        int attributes;
        int priority;
        std::vector<UnicodeBlock> unicodeBlocks;
        std::vector<Language> languages;
        netflix::gibbon::FontValues fontValues;
        DataBuffer ttf;
        std::string version;
        int cmap;
        netflix::gibbon::Rect bbox;
        std::string patch;
        Variant downloaded;
        bool persist;
        std::map<std::string, std::vector<UnicodeBlock> > patches;

        FontRegistryItem(const std::string &_fileName, const std::string& _family, const netflix::gibbon::Font::Weight _weight, const netflix::gibbon::Font::Style _style, const int _attributes, const int _priority, const std::vector<UnicodeBlock> &_unicodeBlocks, const std::vector<Language> &_languages, const netflix::gibbon::FontValues &_fontValues, const std::string &_version, int _cmap = -1, const netflix::gibbon::Rect& _bbox = netflix::gibbon::Rect(), const DataBuffer& _ttf = DataBuffer(), const std::string &_patch = std::string(), const Variant &_downloaded = Variant(), bool _persist = false)
            : fileName(_fileName), family(_family), weight(_weight), style(_style), attributes(_attributes), priority(_priority), unicodeBlocks(_unicodeBlocks), languages(_languages), fontValues(_fontValues), ttf(_ttf), version(_version), cmap(_cmap), bbox(_bbox), patch(_patch), downloaded(_downloaded), persist(_persist) { }
        FontRegistryItem() : weight(Font::WEIGHT_NORMAL), style(Font::STYLE_NORMAL), attributes(0), priority(100) { }
        int supportsLanguage(std::string language) const;
    };

    struct Match {
        Match(const std::string& _index, int _score, int _priority) : index(_index), score(_score), priority(_priority) {}
        std::string index;
        int score;
        int priority;
    };

    static void addFontsFromDirectory(const std::string &path);
    static bool registerFont(const FontRegistryItem& item);
    static bool unregisterFont(const std::string& family, netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style);
    static netflix::gibbon::Font::SharedPtr getFont(const std::string& family, uint32_t size, netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style);
    static netflix::gibbon::Font::SharedPtr getFontForCodePoint(unsigned long codePoint, const std::string& currentFamilyName, uint32_t size, netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style, std::string locale, unsigned long* rangeStart, unsigned long* rangeEnd);
    static std::vector<Match> getFontsForCodePoint(unsigned long codePoint, const std::string& currentFamilyName, netflix::gibbon::Font::Weight weight, netflix::gibbon::Font::Style style, std::string locale, unsigned long* rangeStart, unsigned long* rangeEnd);

    static void reportMissing(unsigned long codePoint, const std::string& familyName);

    static netflix::gibbon::Font::SharedPtr getFontByIndex(const std::string& index, uint32_t size);
    static void clear() { mFontRegistry.clear(); }

    enum ClearMode {
        Persisted = 0x1,
        NonPersisted = 0x2,
        All = Persisted | NonPersisted
    };
    static void clearDownloadedFonts(unsigned int mode = All);

    static void dumpFontStatistics(bool verbose = false);
    static void dumpFontLinkingTree(unsigned long codePoint = 0, Font::Weight = Font::WEIGHT_NORMAL, Font::Style = Font::STYLE_NORMAL, std::string locale = "");

    typedef std::map<std::string, FontRegistryItem> FontRegistry;
    typedef std::pair<std::string, FontRegistryItem> FontRegistryEntry;

    typedef std::map< std::string, weak_ptr<Font> > FontCache;
    typedef std::pair <std::string, weak_ptr<Font> > FontCacheItem;

    typedef std::map<std::string, std::string> FontAliases;

    static FontRegistry& getFontRegistry() { return mFontRegistry; }
    static std::string getFontName(const std::string& family, Font::Weight weight, Font::Style style, uint32_t size = 0);

    static Variant fontInfo();

private:
    static void populateUnicodeRanges();
    static void patchFonts(const std::string& from, const std::string& to, const std::string& fn, const std::vector<UnicodeBlock> &blocks);
    static void unpatchFonts(const std::string& from, const std::string& to, const std::string& fn);
    static void updatePatches(const std::string& from, const std::string& fn);
    static void updatePatches();
    static bool shouldReportMissing(unsigned long codePoint);

    static shared_ptr<FontManagerDumper> dumper(const DumpInfo& dump = DumpInfo());

private:
    static FontRegistry mFontRegistry;
    static FontCache mFontCache;
    static FontAliases mFontAliases;
    static std::set<unsigned long> sReported;
    static bool sHasReported;
};

inline std::ostream &operator<< (std::ostream &out, const FontManager::FontRegistryItem& font) {
    out << FontManager::getFontName(font.family, font.weight, font.style, 0) << std::endl;
    return out;
}


inline std::ostream &operator<< (std::ostream &out, const FontManager::FontRegistry& registry)
{
    FontManager::FontRegistry::const_iterator it = registry.begin();
    FontManager::FontRegistry::const_iterator end = registry.end();

    out << "Font registry entries:" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << it->second << std::endl;
        ++i;
        out.flush();
    }

    return out;
}

inline std::ostream &operator<< (std::ostream &out, const FontManager::Match& match) {
    out << match.index << " (" << match.score << "." << match.priority << ")";
    return out;
}

inline std::ostream &operator<< (std::ostream &out, const std::vector<FontManager::Match>& matches)
{
    std::vector<FontManager::Match>::const_iterator it = matches.begin();
    std::vector<FontManager::Match>::const_iterator end = matches.end();

    out << "Font matches:" << std::endl;
    int i = 0;
    for (; it != end; it++) {
        out << " [" << std::setfill('0') << std::setw(2) << i << "] " << *it << std::endl;
        ++i;
        out.flush();
    }
    return out;
}


}} // namespace netflix::gibbon

#endif /* FONTMANAGER_H_ */
