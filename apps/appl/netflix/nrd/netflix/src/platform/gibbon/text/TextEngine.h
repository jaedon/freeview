/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights. Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#ifndef TEXTENGINE_H_
#define TEXTENGINE_H_

#include "Rect.h"
#include <nrd/AppLog.h>

#include <vector>
#include <map>
#include <stack>
#include <stdint.h>

namespace netflix {
namespace gibbon {

struct tiLayout;
struct TextLayout;
class GraphicsEngine;
struct TextItemizerData;

class TextEngine
{
public:
    enum Diff
    {
        DIFF_LAYOUT = 0x01,
        DIFF_RENDER = 0x02,
        DIFF_NONE   = 0x00
    };

    // Initialization functions
    static void init() {};
    static void cleanup();

    // Layout functions
    static void layoutText(tiLayout &layout, const TextItemizerData &data, const netflix::gibbon::Rect &r, int firstLine = 0);

    // Drawing functions
    static void drawLayout(netflix::gibbon::GraphicsEngine *e, const tiLayout &layout, const netflix::gibbon::Rect &dstRec);

    // Cursor handling functions
    static netflix::gibbon::Rect getCursorRect(const tiLayout &layout, uint32_t charIndex, uint32_t &actualCharIndex);
    static int getCharIndex(const tiLayout &layout, const netflix::gibbon::Point &point);
    static int moveCursor(const tiLayout &layout, int current, int lines, int columns);
};


}} // namespace netflix::gibbon

#endif /* TEXTENGINE_H_ */
