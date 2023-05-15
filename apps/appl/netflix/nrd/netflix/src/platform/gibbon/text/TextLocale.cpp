/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include "GibbonConfiguration.h"
#include "TextLocale.h"

#include <vector>
#include <sstream>
#include <ostream>
#include <string.h>

using namespace netflix::gibbon;

static std::vector<std::string> split(const std::string &str, const std::string &delim)
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

Locale::Locale(const std::string &languageString)
{
    setLanguage(languageString.empty() ? GibbonConfiguration::defaultLocale() : languageString);
}

std::string Locale::toString() const
{
    std::ostringstream is;
    is << language;
    if (!script.empty()) {
        is << "-" << script;
        if (!country.empty()) {
            is << "-" << country;
        }
    }

    return is.str();
}

int Locale::getDirection()
{
    const char* scr = script.c_str();

    if (strncmp(scr, "Arab", 4) == 0 || // Arabic (1.1)
        strncmp(scr, "Hebr", 4) == 0 || // Hebrew (1.1)
        strncmp(scr, "Syrc", 4) == 0 || // Syriac (3.0)
        strncmp(scr, "Thaa", 4) == 0 || // Thaana (3.0)
        strncmp(scr, "Cprt", 4) == 0 || // Cypriot (4.0)
        strncmp(scr, "Khar", 4) == 0 || // Kharoshthi (4.1)
        strncmp(scr, "Phnx", 4) == 0 || // Phoenician (5.0)
        strncmp(scr, "Nkoo", 4) == 0 || // Nko (5.0)
        strncmp(scr, "Lydi", 4) == 0 || // Lydian (5.1)
        strncmp(scr, "Avst", 4) == 0 || // Avestan (5.2)
        strncmp(scr, "Armi", 4) == 0 || // Imperial Aramaic (5.2)
        strncmp(scr, "Phli", 4) == 0 || // Inscriptional Pahlavi (5.2)
        strncmp(scr, "Prti", 4) == 0 || // Inscriptional Parthian (5.2)
        strncmp(scr, "Sarb", 4) == 0 || // Old South Arabian (5.2)
        strncmp(scr, "Orkh", 4) == 0 || // Old Turkic (5.2)
        strncmp(scr, "Samr", 4) == 0 || // Samaritan (5.2)
        strncmp(scr, "Mand", 4) == 0 || // Mandaic (6.0)
        strncmp(scr, "Cakm", 4) == 0 || // Chakma (6.1)
        strncmp(scr, "Merc", 4) == 0 || // Meroitic Cursive (6.1)
        strncmp(scr, "Mero", 4) == 0 || // Meroitic Gieroglyphs (6.1)
        strncmp(scr, "Mani", 4) == 0 || // Manichaean (7.0)
        strncmp(scr, "Mend", 4) == 0 || // Mende Kikakui (7.0)
        strncmp(scr, "Nbat", 4) == 0 || // Nabatean (7.0)
        strncmp(scr, "Narb", 4) == 0 || // Old North Arabian (7.0)
        strncmp(scr, "Palm", 4) == 0 || // Palmyrene (7.0)
        strncmp(scr, "Phlp", 4) == 0)   // Psalter Pahlavi (7.0)
    {
        return 1; // Right to left
    } else {
        // Key on languages, just in case the script has not been specified
        // TODO: Fill the script automatically based on the language, using
        // ICU or some other Unicode library
        const char* lng = language.c_str();
        if (strncmp(lng, "ar", 2) == 0 ||  // Arabic
            strncmp(lng, "dv", 2) == 0 ||  // Dhivehi
            strncmp(lng, "fa", 2) == 0 ||  // Persian
            strncmp(lng, "he", 2) == 0 ||  // Hebrew
            strncmp(lng, "iw", 2) == 0 ||  // Hebrew (legacy identifier)
            strncmp(lng, "ku", 2) == 0 ||  // Kurdish
            strncmp(lng, "ps", 2) == 0 ||  // Pashto
            strncmp(lng, "sd", 2) == 0 ||  // Sindhi
            strncmp(lng, "ug", 2) == 0 ||  // Uyghur
            strncmp(lng, "ur", 2) == 0 ||  // Urdu
            strncmp(lng, "yi", 2) == 0 ||  // Yiddish

            strncmp(lng, "arc", 3) == 0 || // Aramaic
            strncmp(lng, "bcc", 3) == 0 || // Southern Balochi
            strncmp(lng, "bqi", 3) == 0 || // Bakthiari
            strncmp(lng, "ckb", 3) == 0 || // Sorani
            strncmp(lng, "glk", 3) == 0 || // Gilaki
            strncmp(lng, "mzn", 3) == 0 || // Mazanderani
            strncmp(lng, "pnb", 3) == 0)   // Western Punjabi
        {
            return 1; // Right to left
        } else {
            return 0; // Left to right
        }
    }
}

void Locale::setLanguage(const std::string &languageString)
{
    std::vector<std::string> v = split(languageString, "-");
    if (v.size()>=3)
        country = v[2];
    if (v.size()>=2) {
        if (v[1].length() == 4)
            script = v[1];
        else
            country = v[1];
    }
    if (v.size()>=1)
        language = v[0];

    // TODO: Fill this with a proper list
    if (script.empty()) {
        if (language == "he" || language == "yd" || language == "iw") {
            script = "Hebr";
        } else if (language == "ar" || language == "fa" ||
                   language == "ps" || language == "ur") {
            script = "Arab";
        }
    }
}
