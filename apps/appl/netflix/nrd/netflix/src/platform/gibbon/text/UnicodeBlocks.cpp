/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "UnicodeBlocks.h"

#include <nrd/AppLog.h>

#include <stdio.h>
#include <iomanip>

using namespace netflix;
using namespace netflix::gibbon;

// Taken from http://www.unicode.org/Public/UNIDATA/Blocks.txt
/*
  #!/bin/awk

  function ltrim(s) { sub(/^[ \t\r\n]+/, "", s); return s }
  function rtrim(s) { sub(/[ \t\r\n]+$/, "", s); return s }
  function trim(s)  { return rtrim(ltrim(s)); }

  BEGIN {
  while ((getline line < "input.txt") > 0) {
  split(line,a,";")
  split(a[1],b,".")
  #printf "%s -> %s = |%s|\n", b[1], b[3], trim(a[2])
  printf "UnicodeBlock(0x%s, 0x%s, \"%s\"),\n", b[1], b[3], trim(a[2])
  }
  close("input.txt")
  }
*/

static UnicodeBlocks::Range sUnicodeRanges[] = {
    UnicodeBlocks::Range(0x0000, 0x007F, "Basic Latin"),
    UnicodeBlocks::Range(0x0080, 0x00FF, "Latin-1 Supplement"),
    UnicodeBlocks::Range(0x0100, 0x017F, "Latin Extended-A"),
    UnicodeBlocks::Range(0x0180, 0x024F, "Latin Extended-B"),
    UnicodeBlocks::Range(0x0250, 0x02AF, "IPA Extensions"),
    UnicodeBlocks::Range(0x02B0, 0x02FF, "Spacing Modifier Letters"),
    UnicodeBlocks::Range(0x0300, 0x036F, "Combining Diacritical Marks"),
    UnicodeBlocks::Range(0x0370, 0x03FF, "Greek and Coptic"),
    UnicodeBlocks::Range(0x0400, 0x04FF, "Cyrillic"),
    UnicodeBlocks::Range(0x0500, 0x052F, "Cyrillic Supplement"),
    UnicodeBlocks::Range(0x0530, 0x058F, "Armenian"),
    UnicodeBlocks::Range(0x0590, 0x05FF, "Hebrew"),
    UnicodeBlocks::Range(0x0600, 0x06FF, "Arabic"),
    UnicodeBlocks::Range(0x0700, 0x074F, "Syriac"),
    UnicodeBlocks::Range(0x0750, 0x077F, "Arabic Supplement"),
    UnicodeBlocks::Range(0x0780, 0x07BF, "Thaana"),
    UnicodeBlocks::Range(0x07C0, 0x07FF, "NKo"),
    UnicodeBlocks::Range(0x0800, 0x083F, "Samaritan"),
    UnicodeBlocks::Range(0x0840, 0x085F, "Mandaic"),
    UnicodeBlocks::Range(0x08A0, 0x08FF, "Arabic Extended-A"),
    UnicodeBlocks::Range(0x0900, 0x097F, "Devanagari"),
    UnicodeBlocks::Range(0x0980, 0x09FF, "Bengali"),
    UnicodeBlocks::Range(0x0A00, 0x0A7F, "Gurmukhi"),
    UnicodeBlocks::Range(0x0A80, 0x0AFF, "Gujarati"),
    UnicodeBlocks::Range(0x0B00, 0x0B7F, "Oriya"),
    UnicodeBlocks::Range(0x0B80, 0x0BFF, "Tamil"),
    UnicodeBlocks::Range(0x0C00, 0x0C7F, "Telugu"),
    UnicodeBlocks::Range(0x0C80, 0x0CFF, "Kannada"),
    UnicodeBlocks::Range(0x0D00, 0x0D7F, "Malayalam"),
    UnicodeBlocks::Range(0x0D80, 0x0DFF, "Sinhala"),
    UnicodeBlocks::Range(0x0E00, 0x0E7F, "Thai"),
    UnicodeBlocks::Range(0x0E80, 0x0EFF, "Lao"),
    UnicodeBlocks::Range(0x0F00, 0x0FFF, "Tibetan"),
    UnicodeBlocks::Range(0x1000, 0x109F, "Myanmar"),
    UnicodeBlocks::Range(0x10A0, 0x10FF, "Georgian"),
    UnicodeBlocks::Range(0x1100, 0x11FF, "Hangul Jamo"),
    UnicodeBlocks::Range(0x1200, 0x137F, "Ethiopic"),
    UnicodeBlocks::Range(0x1380, 0x139F, "Ethiopic Supplement"),
    UnicodeBlocks::Range(0x13A0, 0x13FF, "Cherokee"),
    UnicodeBlocks::Range(0x1400, 0x167F, "Unified Canadian Aboriginal Syllabics"),
    UnicodeBlocks::Range(0x1680, 0x169F, "Ogham"),
    UnicodeBlocks::Range(0x16A0, 0x16FF, "Runic"),
    UnicodeBlocks::Range(0x1700, 0x171F, "Tagalog"),
    UnicodeBlocks::Range(0x1720, 0x173F, "Hanunoo"),
    UnicodeBlocks::Range(0x1740, 0x175F, "Buhid"),
    UnicodeBlocks::Range(0x1760, 0x177F, "Tagbanwa"),
    UnicodeBlocks::Range(0x1780, 0x17FF, "Khmer"),
    UnicodeBlocks::Range(0x1800, 0x18AF, "Mongolian"),
    UnicodeBlocks::Range(0x18B0, 0x18FF, "Unified Canadian Aboriginal Syllabics Extended"),
    UnicodeBlocks::Range(0x1900, 0x194F, "Limbu"),
    UnicodeBlocks::Range(0x1950, 0x197F, "Tai Le"),
    UnicodeBlocks::Range(0x1980, 0x19DF, "New Tai Lue"),
    UnicodeBlocks::Range(0x19E0, 0x19FF, "Khmer Symbols"),
    UnicodeBlocks::Range(0x1A00, 0x1A1F, "Buginese"),
    UnicodeBlocks::Range(0x1A20, 0x1AAF, "Tai Tham"),
    UnicodeBlocks::Range(0x1B00, 0x1B7F, "Balinese"),
    UnicodeBlocks::Range(0x1B80, 0x1BBF, "Sundanese"),
    UnicodeBlocks::Range(0x1BC0, 0x1BFF, "Batak"),
    UnicodeBlocks::Range(0x1C00, 0x1C4F, "Lepcha"),
    UnicodeBlocks::Range(0x1C50, 0x1C7F, "Ol Chiki"),
    UnicodeBlocks::Range(0x1CC0, 0x1CCF, "Sundanese Supplement"),
    UnicodeBlocks::Range(0x1CD0, 0x1CFF, "Vedic Extensions"),
    UnicodeBlocks::Range(0x1D00, 0x1D7F, "Phonetic Extensions"),
    UnicodeBlocks::Range(0x1D80, 0x1DBF, "Phonetic Extensions Supplement"),
    UnicodeBlocks::Range(0x1DC0, 0x1DFF, "Combining Diacritical Marks Supplement"),
    UnicodeBlocks::Range(0x1E00, 0x1EFF, "Latin Extended Additional"),
    UnicodeBlocks::Range(0x1F00, 0x1FFF, "Greek Extended"),
    UnicodeBlocks::Range(0x2000, 0x206F, "General Punctuation"),
    UnicodeBlocks::Range(0x2070, 0x209F, "Superscripts and Subscripts"),
    UnicodeBlocks::Range(0x20A0, 0x20CF, "Currency Symbols"),
    UnicodeBlocks::Range(0x20D0, 0x20FF, "Combining Diacritical Marks for Symbols"),
    UnicodeBlocks::Range(0x2100, 0x214F, "Letterlike Symbols"),
    UnicodeBlocks::Range(0x2150, 0x218F, "Number Forms"),
    UnicodeBlocks::Range(0x2190, 0x21FF, "Arrows"),
    UnicodeBlocks::Range(0x2200, 0x22FF, "Mathematical Operators"),
    UnicodeBlocks::Range(0x2300, 0x23FF, "Miscellaneous Technical"),
    UnicodeBlocks::Range(0x2400, 0x243F, "Control Pictures"),
    UnicodeBlocks::Range(0x2440, 0x245F, "Optical Character Recognition"),
    UnicodeBlocks::Range(0x2460, 0x24FF, "Enclosed Alphanumerics"),
    UnicodeBlocks::Range(0x2500, 0x257F, "Box Drawing"),
    UnicodeBlocks::Range(0x2580, 0x259F, "Block Elements"),
    UnicodeBlocks::Range(0x25A0, 0x25FF, "Geometric Shapes"),
    UnicodeBlocks::Range(0x2600, 0x26FF, "Miscellaneous Symbols"),
    UnicodeBlocks::Range(0x2700, 0x27BF, "Dingbats"),
    UnicodeBlocks::Range(0x27C0, 0x27EF, "Miscellaneous Mathematical Symbols-A"),
    UnicodeBlocks::Range(0x27F0, 0x27FF, "Supplemental Arrows-A"),
    UnicodeBlocks::Range(0x2800, 0x28FF, "Braille Patterns"),
    UnicodeBlocks::Range(0x2900, 0x297F, "Supplemental Arrows-B"),
    UnicodeBlocks::Range(0x2980, 0x29FF, "Miscellaneous Mathematical Symbols-B"),
    UnicodeBlocks::Range(0x2A00, 0x2AFF, "Supplemental Mathematical Operators"),
    UnicodeBlocks::Range(0x2B00, 0x2BFF, "Miscellaneous Symbols and Arrows"),
    UnicodeBlocks::Range(0x2C00, 0x2C5F, "Glagolitic"),
    UnicodeBlocks::Range(0x2C60, 0x2C7F, "Latin Extended-C"),
    UnicodeBlocks::Range(0x2C80, 0x2CFF, "Coptic"),
    UnicodeBlocks::Range(0x2D00, 0x2D2F, "Georgian Supplement"),
    UnicodeBlocks::Range(0x2D30, 0x2D7F, "Tifinagh"),
    UnicodeBlocks::Range(0x2D80, 0x2DDF, "Ethiopic Extended"),
    UnicodeBlocks::Range(0x2DE0, 0x2DFF, "Cyrillic Extended-A"),
    UnicodeBlocks::Range(0x2E00, 0x2E7F, "Supplemental Punctuation"),
    UnicodeBlocks::Range(0x2E80, 0x2EFF, "CJK Radicals Supplement"),
    UnicodeBlocks::Range(0x2F00, 0x2FDF, "Kangxi Radicals"),
    UnicodeBlocks::Range(0x2FF0, 0x2FFF, "Ideographic Description Characters"),
    UnicodeBlocks::Range(0x3000, 0x303F, "CJK Symbols and Punctuation"),
    UnicodeBlocks::Range(0x3040, 0x309F, "Hiragana"),
    UnicodeBlocks::Range(0x30A0, 0x30FF, "Katakana"),
    UnicodeBlocks::Range(0x3100, 0x312F, "Bopomofo"),
    UnicodeBlocks::Range(0x3130, 0x318F, "Hangul Compatibility Jamo"),
    UnicodeBlocks::Range(0x3190, 0x319F, "Kanbun"),
    UnicodeBlocks::Range(0x31A0, 0x31BF, "Bopomofo Extended"),
    UnicodeBlocks::Range(0x31C0, 0x31EF, "CJK Strokes"),
    UnicodeBlocks::Range(0x31F0, 0x31FF, "Katakana Phonetic Extensions"),
    UnicodeBlocks::Range(0x3200, 0x32FF, "Enclosed CJK Letters and Months"),
    UnicodeBlocks::Range(0x3300, 0x33FF, "CJK Compatibility"),
    UnicodeBlocks::Range(0x3400, 0x4DBF, "CJK Unified Ideographs Extension A"),
    UnicodeBlocks::Range(0x4DC0, 0x4DFF, "Yijing Hexagram Symbols"),
    UnicodeBlocks::Range(0x4E00, 0x9FFF, "CJK Unified Ideographs"),
    UnicodeBlocks::Range(0xA000, 0xA48F, "Yi Syllables"),
    UnicodeBlocks::Range(0xA490, 0xA4CF, "Yi Radicals"),
    UnicodeBlocks::Range(0xA4D0, 0xA4FF, "Lisu"),
    UnicodeBlocks::Range(0xA500, 0xA63F, "Vai"),
    UnicodeBlocks::Range(0xA640, 0xA69F, "Cyrillic Extended-B"),
    UnicodeBlocks::Range(0xA6A0, 0xA6FF, "Bamum"),
    UnicodeBlocks::Range(0xA700, 0xA71F, "Modifier Tone Letters"),
    UnicodeBlocks::Range(0xA720, 0xA7FF, "Latin Extended-D"),
    UnicodeBlocks::Range(0xA800, 0xA82F, "Syloti Nagri"),
    UnicodeBlocks::Range(0xA830, 0xA83F, "Common Indic Number Forms"),
    UnicodeBlocks::Range(0xA840, 0xA87F, "Phags-pa"),
    UnicodeBlocks::Range(0xA880, 0xA8DF, "Saurashtra"),
    UnicodeBlocks::Range(0xA8E0, 0xA8FF, "Devanagari Extended"),
    UnicodeBlocks::Range(0xA900, 0xA92F, "Kayah Li"),
    UnicodeBlocks::Range(0xA930, 0xA95F, "Rejang"),
    UnicodeBlocks::Range(0xA960, 0xA97F, "Hangul Jamo Extended-A"),
    UnicodeBlocks::Range(0xA980, 0xA9DF, "Javanese"),
    UnicodeBlocks::Range(0xAA00, 0xAA5F, "Cham"),
    UnicodeBlocks::Range(0xAA60, 0xAA7F, "Myanmar Extended-A"),
    UnicodeBlocks::Range(0xAA80, 0xAADF, "Tai Viet"),
    UnicodeBlocks::Range(0xAAE0, 0xAAFF, "Meetei Mayek Extensions"),
    UnicodeBlocks::Range(0xAB00, 0xAB2F, "Ethiopic Extended-A"),
    UnicodeBlocks::Range(0xABC0, 0xABFF, "Meetei Mayek"),
    UnicodeBlocks::Range(0xAC00, 0xD7AF, "Hangul Syllables"),
    UnicodeBlocks::Range(0xD7B0, 0xD7FF, "Hangul Jamo Extended-B"),
    UnicodeBlocks::Range(0xD800, 0xDB7F, "High Surrogates"),
    UnicodeBlocks::Range(0xDB80, 0xDBFF, "High Private Use Surrogates"),
    UnicodeBlocks::Range(0xDC00, 0xDFFF, "Low Surrogates"),
    UnicodeBlocks::Range(0xE000, 0xF8FF, "Private Use Area"),
    UnicodeBlocks::Range(0xF900, 0xFAFF, "CJK Compatibility Ideographs"),
    UnicodeBlocks::Range(0xFB00, 0xFB4F, "Alphabetic Presentation Forms"),
    UnicodeBlocks::Range(0xFB50, 0xFDFF, "Arabic Presentation Forms-A"),
    UnicodeBlocks::Range(0xFE00, 0xFE0F, "Variation Selectors"),
    UnicodeBlocks::Range(0xFE10, 0xFE1F, "Vertical Forms"),
    UnicodeBlocks::Range(0xFE20, 0xFE2F, "Combining Half Marks"),
    UnicodeBlocks::Range(0xFE30, 0xFE4F, "CJK Compatibility Forms"),
    UnicodeBlocks::Range(0xFE50, 0xFE6F, "Small Form Variants"),
    UnicodeBlocks::Range(0xFE70, 0xFEFF, "Arabic Presentation Forms-B"),
    UnicodeBlocks::Range(0xFF00, 0xFFEF, "Halfwidth and Fullwidth Forms"),
    UnicodeBlocks::Range(0xFFF0, 0xFFFF, "Specials"),
    UnicodeBlocks::Range(0x10000, 0x1007F, "Linear B Syllabary"),
    UnicodeBlocks::Range(0x10080, 0x100FF, "Linear B Ideograms"),
    UnicodeBlocks::Range(0x10100, 0x1013F, "Aegean Numbers"),
    UnicodeBlocks::Range(0x10140, 0x1018F, "Ancient Greek Numbers"),
    UnicodeBlocks::Range(0x10190, 0x101CF, "Ancient Symbols"),
    UnicodeBlocks::Range(0x101D0, 0x101FF, "Phaistos Disc"),
    UnicodeBlocks::Range(0x10280, 0x1029F, "Lycian"),
    UnicodeBlocks::Range(0x102A0, 0x102DF, "Carian"),
    UnicodeBlocks::Range(0x10300, 0x1032F, "Old Italic"),
    UnicodeBlocks::Range(0x10330, 0x1034F, "Gothic"),
    UnicodeBlocks::Range(0x10380, 0x1039F, "Ugaritic"),
    UnicodeBlocks::Range(0x103A0, 0x103DF, "Old Persian"),
    UnicodeBlocks::Range(0x10400, 0x1044F, "Deseret"),
    UnicodeBlocks::Range(0x10450, 0x1047F, "Shavian"),
    UnicodeBlocks::Range(0x10480, 0x104AF, "Osmanya"),
    UnicodeBlocks::Range(0x10800, 0x1083F, "Cypriot Syllabary"),
    UnicodeBlocks::Range(0x10840, 0x1085F, "Imperial Aramaic"),
    UnicodeBlocks::Range(0x10900, 0x1091F, "Phoenician"),
    UnicodeBlocks::Range(0x10920, 0x1093F, "Lydian"),
    UnicodeBlocks::Range(0x10980, 0x1099F, "Meroitic Hieroglyphs"),
    UnicodeBlocks::Range(0x109A0, 0x109FF, "Meroitic Cursive"),
    UnicodeBlocks::Range(0x10A00, 0x10A5F, "Kharoshthi"),
    UnicodeBlocks::Range(0x10A60, 0x10A7F, "Old South Arabian"),
    UnicodeBlocks::Range(0x10B00, 0x10B3F, "Avestan"),
    UnicodeBlocks::Range(0x10B40, 0x10B5F, "Inscriptional Parthian"),
    UnicodeBlocks::Range(0x10B60, 0x10B7F, "Inscriptional Pahlavi"),
    UnicodeBlocks::Range(0x10C00, 0x10C4F, "Old Turkic"),
    UnicodeBlocks::Range(0x10E60, 0x10E7F, "Rumi Numeral Symbols"),
    UnicodeBlocks::Range(0x11000, 0x1107F, "Brahmi"),
    UnicodeBlocks::Range(0x11080, 0x110CF, "Kaithi"),
    UnicodeBlocks::Range(0x110D0, 0x110FF, "Sora Sompeng"),
    UnicodeBlocks::Range(0x11100, 0x1114F, "Chakma"),
    UnicodeBlocks::Range(0x11180, 0x111DF, "Sharada"),
    UnicodeBlocks::Range(0x11680, 0x116CF, "Takri"),
    UnicodeBlocks::Range(0x12000, 0x123FF, "Cuneiform"),
    UnicodeBlocks::Range(0x12400, 0x1247F, "Cuneiform Numbers and Punctuation"),
    UnicodeBlocks::Range(0x13000, 0x1342F, "Egyptian Hieroglyphs"),
    UnicodeBlocks::Range(0x16800, 0x16A3F, "Bamum Supplement"),
    UnicodeBlocks::Range(0x16F00, 0x16F9F, "Miao"),
    UnicodeBlocks::Range(0x1B000, 0x1B0FF, "Kana Supplement"),
    UnicodeBlocks::Range(0x1D000, 0x1D0FF, "Byzantine Musical Symbols"),
    UnicodeBlocks::Range(0x1D100, 0x1D1FF, "Musical Symbols"),
    UnicodeBlocks::Range(0x1D200, 0x1D24F, "Ancient Greek Musical Notation"),
    UnicodeBlocks::Range(0x1D300, 0x1D35F, "Tai Xuan Jing Symbols"),
    UnicodeBlocks::Range(0x1D360, 0x1D37F, "Counting Rod Numerals"),
    UnicodeBlocks::Range(0x1D400, 0x1D7FF, "Mathematical Alphanumeric Symbols"),
    UnicodeBlocks::Range(0x1EE00, 0x1EEFF, "Arabic Mathematical Alphabetic Symbols"),
    UnicodeBlocks::Range(0x1F000, 0x1F02F, "Mahjong Tiles"),
    UnicodeBlocks::Range(0x1F030, 0x1F09F, "Domino Tiles"),
    UnicodeBlocks::Range(0x1F0A0, 0x1F0FF, "Playing Cards"),
    UnicodeBlocks::Range(0x1F100, 0x1F1FF, "Enclosed Alphanumeric Supplement"),
    UnicodeBlocks::Range(0x1F200, 0x1F2FF, "Enclosed Ideographic Supplement"),
    UnicodeBlocks::Range(0x1F300, 0x1F5FF, "Miscellaneous Symbols And Pictographs"),
    UnicodeBlocks::Range(0x1F600, 0x1F64F, "Emoticons"),
    UnicodeBlocks::Range(0x1F680, 0x1F6FF, "Transport And Map Symbols"),
    UnicodeBlocks::Range(0x1F700, 0x1F77F, "Alchemical Symbols"),
    UnicodeBlocks::Range(0x20000, 0x2A6DF, "CJK Unified Ideographs Extension B"),
    UnicodeBlocks::Range(0x2A700, 0x2B73F, "CJK Unified Ideographs Extension C"),
    UnicodeBlocks::Range(0x2B740, 0x2B81F, "CJK Unified Ideographs Extension D"),
    UnicodeBlocks::Range(0x2F800, 0x2FA1F, "CJK Compatibility Ideographs Supplement"),
    UnicodeBlocks::Range(0xE0000, 0xE007F, "Tags"),
    UnicodeBlocks::Range(0xE0100, 0xE01EF, "Variation Selectors Supplement"),
    UnicodeBlocks::Range(0xF0000, 0xFFFFF, "Supplementary Private Use Area-A"),
    UnicodeBlocks::Range(0x100000, 0x10FFFF, "Supplementary Private Use Area-B")
};

static int
compare_unicode_range(const void *m1, const void *m2)
{
    const uint32_t c = *((uint32_t*)m1);
    const UnicodeBlocks::Range *range = (const UnicodeBlocks::Range*)m2;
    if(c < range->start)
        return -1;
    else if(c > range->end)
        return 1;
    return 0;
}

UnicodeBlocks::Range *UnicodeBlocks::find(uint32_t character)
{
    return (UnicodeBlocks::Range*)bsearch(&character, sUnicodeRanges, sizeof(sUnicodeRanges)/sizeof(sUnicodeRanges[0]),
                                          sizeof(sUnicodeRanges[0]), compare_unicode_range);
}

void UnicodeBlocks::dumpFonts()
{
    for(size_t i = 0; i < sizeof(sUnicodeRanges)/sizeof(sUnicodeRanges[0]); ++i) {
        Range *range = sUnicodeRanges+i;
        if(range->fonts.size() != 0) {
            Log::warn(TRACE_UI_ENGINE) << range->name << " (U+" << std::hex << std::uppercase << std::setw(6)
                                       << std::setfill('0') << range->start << "-U+" << std::setw(6) << range->end << ")";
            for(Fonts::size_type i = 0; i < range->fonts.size(); i++)
                Log::warn(TRACE_UI_ENGINE) << " - " << range->fonts[i];
        }
    }
}
