/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: buttonlistbox.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/26/10 5:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/buttonlistbox.cpp $
 * 
 * 2   3/26/10 5:22p erickson
 * SW7405-3625: refactor constructor params for ButtonListBox to allow for
 * more generic use
 *
 * 1   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   8/21/08 5:21p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 *
 *
 ***************************************************************************/
#include "buttonlistbox.h"

void ButtonListBox::layout()
{
    int y = 0 + 20;
    int tmpwidth = width();
    int tmpheight = height() - 40;

#ifdef NO_LINES
    int yinc = _items.total() ? (tmpheight-4) / _items.total() : 10;
#else
    int yinc = _items.total() ? tmpheight / _items.total() : 10;
#endif
    int w    = tmpwidth;
    if (_bar)
        w -= _bar->width();
    for (MListItem *item = _items.first(); item; item = _items.next()) {
        item->setGeometry(MRect(0,y,w,yinc-buttonVSpacing()));
        y += yinc;
    }
}

void ButtonListBox::draw(const MRect &cliprect)
{
    MPainter ptr(this, clientRect());

    drawArrows(ptr, clientRect());

    MColumnListBox::draw(cliprect);
}

void ButtonListBox::drawArrows(MPainter &ptr, const MRect &rect)
{
    int midx = rect.midX();
    //int midx = rect.x() + rect.width() - 7;
    int midy = rect.y() + 7;
    int totalRows = _values.total();
    int maxVisibleRows = _items.total();

    if (_bar)
    {
        midx -= _bar->width() / 2;
    }

    if (totalRows > maxVisibleRows)
    {
        /* scrolling arrows are needed */

        if (topIndex() > 0)
        {
            /* show top scroll arrow */
            int width = 0;
            int y     = rect.y() + 5;
            int yOrig = y;
            int x     = midx;
            for (x=midx; x>=midx-7; x--)
            {
                ptr.setPen(_arrowFillColor);
                ptr.drawLine(x, y, midx+width, y);

                ptr.setPen(textColor());
                ptr.point(x-1, y);
                ptr.point(x-2, y);
                ptr.point(midx+width+1, y);
                ptr.point(midx+width+2, y);

                width++;
                y++;
            }

            /* draw top line */
            ptr.drawLine(midx-1, yOrig - 1, midx+2, yOrig - 1);
            ptr.drawLine(midx-1, yOrig, midx+2, yOrig);
            /* draw bottom line */
            ptr.drawLine(x-1, y, midx+width+2, y);
            ptr.drawLine(x-1, y+1, midx+width+2, y+1);
        }

        if (totalRows - topIndex() > maxVisibleRows)
        {
            /* show bottom scroll arrow */
            int width = 7;
            int y     = _items.last()->y() + _items.last()->height() + 5;
            int yOrig = y;
            int x     = midx;
            for (x=midx-7; x<=midx; x++)
            {
                ptr.setPen(_arrowFillColor);
                ptr.drawLine(x, y, midx+width, y);

                ptr.setPen(textColor());
                ptr.point(x-1, y);
                ptr.point(x-2, y);
                ptr.point(midx+width+1, y);
                ptr.point(midx+width+2, y);

                width--;
                y++;
            }

            /* draw bottom line */
            ptr.drawLine(midx-1, y-1, midx+2, y-1);
            ptr.drawLine(midx-1, y, midx+2, y);
            /* draw top line */
            ptr.drawLine(midx-7-2, yOrig - 1, midx+7+3, yOrig - 1);
            ptr.drawLine(midx-7-2, yOrig, midx+7+3, yOrig);
        }
    }
}

void ButtonListBox::insert(int index, const char *text, MImage *icon)
{
    if (icon)
    {
        _buttonIconList.insert(index, icon);
    }

    MColumnListBox::insert(index, text);
}

void ButtonListBox::drawIcon(MPainter &ptr, const MRect &rect, const int index)
{
    MImage * pCurrentImage = _buttonIconList.get(index);

    if (pCurrentImage)
    {
        ptr.drawImage(*pCurrentImage, rect.x(), rect.y() + 2, rect.width(), rect.height());
    }
}

void ButtonListBox::ButtonListItem::drawColumn(
    MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
    MPainter::Alignment al, MPainter::VAlignment val)
{
    ButtonListBox *box = (ButtonListBox*)parent();

    if ((col == 0) && text) {
        _buttonListBox->drawIcon(ptr, rect, atoi(text));
    }
    else
    if (text) {
        if (box->buttonsVisible())
        {
            int bevel = 5;

            // draw face
            ptr.setPen(style()->color(MStyle::BUTTON_FACE, enabled()));
            ptr.setBrush(style()->color(MStyle::BUTTON_FACE, enabled()));
            ptr.fillRect(MRect(bevel,bevel,clientRect().width()-2*bevel,clientRect().height()-2*bevel));

            // draw button bevel
            ptr.drawBevel(clientRect(), bevel,
                style()->color(MStyle::BUTTON_TOPBEVEL, enabled()),
                style()->color(MStyle::BUTTON_LEFTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_RIGHTBEVEL, enabled()),
                style()->color(MStyle::BUTTON_BOTTOMBEVEL, enabled()),
                style()->prop(MStyle::BUTTON_ROUNDING));
        }

        /* call the base class */
        MColumnListItem::drawColumn(ptr, col, text, textlen, rect, al, val);
    }
}

MListBox::MListItem *ButtonListBox::createItem() {
    return new ButtonListItem(this, "buttonlistitem");
}
