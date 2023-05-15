/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TextFontItemizer_H
#define TextFontItemizer_H

#include "TextItemizerCommon.h"
#include "TextAttributesItemizer.h"
#include <Style.h>

namespace netflix {
namespace gibbon {

class TextFontItemizer {
public:
    static void processFonts(icu::UnicodeString &text, TextAttributeRuns &textRuns);
};

} /* namespace gibbon */
} /* namespace netflix */

#endif /* TextFontItemizer_H */
