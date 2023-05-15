/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef GREP_H
#define GREP_H

#include <string>
#include <vector>
#include <nrdbase/tr1.h>
#include <nrdbase/config.h>
#include <nrdbase/Console.h>
#include <nrdbase/StringTokenizer.h>
#include <nrdbase/StringUtils.h>
#include <nrdbase/Configuration.h>
#define HAVE_REGEX
#ifdef HAVE_REGEX
#include <regex.h>
#endif
namespace netflix {

class Grep : public Console::Filter
{
public:
    Grep(const std::string &pattern, unsigned int flags = 0);
    Grep(const std::vector<std::string> &args = std::vector<std::string>(), unsigned int flags = 0);
    void clear();
    bool isValid() const;
    bool init(const std::string &pattern, unsigned int flags = 0, Configuration::Color color=Configuration::Invalid);
    bool init(const std::vector<std::string> &args, unsigned int flags = 0);
    bool match(const std::string &line) const;
    bool match(const char *line, int *start = 0, int *length = 0) const;

    virtual bool filter(std::string &text, const Configuration::ColorScheme &colorScheme) const;
    virtual std::string describe() const;

    static bool match(const std::vector<Grep> &grep, const std::string &line);
    enum Flag {
        None = 0x00,
        Valid = 0x01,
        Inverted = 0x02,
        IgnoreCase = 0x04,
        NoColor = 0x08,
        Highlight = 0x10
    };
private:
    struct Data {
        Data(const std::string &p, unsigned int f, Configuration::Color c)
            : pattern(p), flags(f), color(c)
        {}

#ifdef HAVE_REGEX
        ~Data()
        {
            if (flags) {
                regfree(&regexp);
            }
        }
        regex_t regexp;
#endif
        std::string pattern;
        unsigned int flags;
        Configuration::Color color;
    };
    shared_ptr<Data> mData;
};
}

#endif
