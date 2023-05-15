/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */


#include "TextLocaleItemizer.h"

using namespace netflix::gibbon;
using namespace icu;

void TextLocaleRuns::addRun(uint32_t start, uint32_t end, const std::string &locale)
{
    Base::add(TextLocaleRun::SharedPtr(new TextLocaleRun(start, end, locale)));
}

void TextLocaleItemizer::processLocales(TextLocaleRuns &textRuns, const UnicodeString &text, TextAttributeRuns &attr)
{
    TextAttributeRuns::TextAttributeRunsList::iterator it = attr.get().begin();
    //TextAttributeRuns::TextAttributeRunsList::iterator prevIt = attr.get().begin();

    std::string currentLocale;
    std::string currentScript;
    bool add = true;

    while(it != attr.get().end()) {
        //TIDEBUG(it->get()->start);

        const std::string locale = it->get()->style->getLocale();
        const std::string script = it->get()->style->getScript();
        assert(!locale.empty());
        if(currentLocale != locale) {
            currentLocale = locale;
            add = true;
        }

        if(!script.empty() && currentScript != script) {
            currentScript = script;
            std::string newLocale;

            // Scripts for which we know ICU has dictionary based segmentation,
            // so we force the locale if not set
            // Updated for ICU 5.4
            if(script == "Khmr") { // Khmer
                newLocale = "km-KH";
            } else if(script == "Mymr") { // Burmese
                newLocale = "my-MM";
            } else if(script == "Thai") { // Thai
                newLocale = "th-TH";
            } else if(script == "Laoo") { // Lao
                newLocale = "lo-LA";
            }

            // Create a new run only if the previous and new locale are completely different
            // Consider them the same locale if the language code is the same
            if(!newLocale.empty() && (newLocale != currentLocale)) {
                if((newLocale[0] != currentLocale[0]) || (newLocale[1] != currentLocale[1])) {
                    currentLocale = newLocale;
                    add = true;
                }
            }
        }

        if(add) {
            if(textRuns.get().size() != 0)
                textRuns.get().back().get()->end = it->get()->start - 1;
            textRuns.addRun(it->get()->start, text.length(), currentLocale);
            add = false;
        }

        ++it;
    }
}

