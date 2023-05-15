/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef LOCALE_H_
#define LOCALE_H_

#include <string>

namespace netflix {
namespace gibbon {

class Locale
{
public:
    std::string language;
    std::string script;
    std::string country;
    Locale(const std::string &languageString);
    virtual ~Locale() {}
    std::string toString() const;

    void setLanguage(const std::string &languageString);

    int getDirection();
};

}
}

#endif /* _LOCALE_H_ */
