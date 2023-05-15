/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TYPOGRAPHY_H_
#define TYPOGRAPHY_H_

namespace netflix {
namespace gibbon {

#include <nrdbase/Variant.h>

class Typography
{
public:
    Typography() : tracking(0.), kerning(true) {}
    ~Typography() {}

    inline bool operator==(const Typography& t) const
    {
        return tracking == t.tracking && kerning == t.kerning && fontFeatures == t.fontFeatures;
    }

    inline bool operator!=(const Typography& t) const
    {
        return !(*this == t);
    }

    inline std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Typography (";
            out << "Tracking: " << tracking;
            out << "Kerning: " << kerning;
            out << "Features: " << fontFeatures;
            out << ")";
        } else {
            out << tracking << ":" << kerning << ":" << fontFeatures;
        }
        return out.str();
    }

    Variant toVariant() const
    {
        Variant data;
        data["tracking"] = tracking;
        data["kerning"] = kerning;
        data["fontFeatures"] = fontFeatures;
        return data;
    }

    double tracking;
    bool kerning;
    std::string fontFeatures;
};

inline std::ostream &operator<< (std::ostream &out, const Typography &t)
{
    return out << t.toString(true);
}

}} // namespace netflix::gibbon



#endif /* TYPOGRAPHY_H_ */
