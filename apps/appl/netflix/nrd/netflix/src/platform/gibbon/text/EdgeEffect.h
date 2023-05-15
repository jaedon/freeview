/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef EDGEEFFECT_H_
#define EDGEEFFECT_H_

#include <Color.h>
#include <TextEngine.h>
#include <nrdbase/Variant.h>

namespace netflix {
namespace gibbon {

class EdgeEffect
{
public:

    enum Effect {
        EDGE_EFFECT_NONE,
        EDGE_EFFECT_DEPRESSED,
        EDGE_EFFECT_RAISED,
        EDGE_EFFECT_OUTLINE
    };

    inline EdgeEffect() : color1(0, 0, 0, 255), color2(0, 0, 0, 255), width(0), type(EDGE_EFFECT_NONE) { }
    inline ~EdgeEffect() {}

    Color color1;
    Color color2;
    int width;
    Effect type;

    inline uint8_t diff(const EdgeEffect &e) const {
        if(e.type != type || e.width != width)
            return TextEngine::DIFF_LAYOUT|TextEngine::DIFF_RENDER;
        if(e.color1 != color1 || e.color2 != color2)
            return TextEngine::DIFF_RENDER;
        return TextEngine::DIFF_NONE;
    }

    inline bool empty() const
    {
        return (type == EDGE_EFFECT_NONE || width == 0);
    }

    inline bool operator!() const
    {
        return empty();
    }

    inline bool operator==(const EdgeEffect& e) const
    {
        return type == e.type && width == e.width && color1 == e.color1 && color2 == e.color2;
    }

    inline bool operator!=(const EdgeEffect& e) const
    {
        return !(*this == e);
    }

    inline std::string toString(bool verbose = false) const
    {
        std::stringstream out;
        if (verbose) {
            out << "Edge effect (";
            switch (type) {
            case EdgeEffect::EDGE_EFFECT_NONE:
                out << "none";
                break;
            case EdgeEffect::EDGE_EFFECT_DEPRESSED:
                out << "depressed";
                break;
            case EdgeEffect::EDGE_EFFECT_RAISED:
                out << "raised";
                break;
            case EdgeEffect::EDGE_EFFECT_OUTLINE:
                out << "outline";
                break;
            default:
                break;
            }

            out << ", width=" << width << ", " << color1 << ", " << color2;
            out << ")";

        } else {
            out << (int)type << ":" << width << ":" << color1.toString(false) << ":" << color2.toString(false);
        }
        return out.str();
    }

    inline Variant toVariant() const
    {
        Variant data;
        data["color1"] = color1.toVariant();
        data["color2"] = color2.toVariant();
        data["width"] = width;
        data["type"] = type;
        return data;
    }
};

inline std::ostream &operator<< (std::ostream &out, const EdgeEffect &e)
{
    return out << e.toString(true);
}


}} // namespace netflix::gibbon

#endif /* EDGEEFFECT_H_ */
