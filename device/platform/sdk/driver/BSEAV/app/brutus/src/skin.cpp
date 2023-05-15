/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: skin.cpp $
 * $brcm_Revision: 33 $
 * $brcm_Date: 3/21/12 1:53p $
 *
 * Module Description: Parsing, conversion, and storage of XML skin data
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/skin.cpp $
 * 
 * 33   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 * 
 * 32   2/23/11 5:17p katrep
 * SW7405-5146:do not repeat channel map screen for IP
 * 
 * 31   2/23/11 4:06p katrep
 * SW7405-5146:do not repeat channel map screen for IP
 * 
 * 30   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 * 
 * Refsw_7550/1   11/18/09 4:31p chengli
 * SW7550-64 : enable channel map screen even B_HAS_IP is not defined
 * 
 * 29   8/10/09 1:39p jrubio
 * PR55232: add 7340/7342
 * 
 * 28   5/11/09 7:41p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * PR53373_DeepColor/1   4/16/09 3:57p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 * 
 * 27   2/23/09 6:14p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 * 
 * 26   2/12/09 12:12p katrep
 * PR52047: Fixed MSDRM_PD_SUPPORT build issue
 * 
 * 25   2/4/09 10:27a jrubio
 * PR51629: add 7336 support
 * 
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:56p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 * 
 * 24   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   7/31/08 5:38p tokushig
 * PR45076: added button scrolling on homescreen and blue skin support.
 * 
 * 23   8/20/08 5:48p lwhite
 * PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
 * 
 * 22   5/28/08 7:38p jrubio
 * PR42353: add 7325
 * 
 * 21   3/6/08 11:58a tokushig
 * PR20349: Caution must be taken when using the MHash class because it
 * relies on MAutoList,  Do not add static objects or dynamically
 * allocated objects that are deleted elsewhere, to an MHash.  Fixed
 * cases where this occurs in the skin.  Also searched thorough entire
 * brutus/mlibs source to ensure a similar problem does not exist.
 * MAutoList (and any class that use it) is a very dangerous class to use
 * unless you have full knowledge of it's limitations.  this fix will
 * correct all of the seemingly random segmentation faults that occur
 * when quitting brutus.
 * 
 * 20   12/4/07 5:20p katrep
 * PR37217: Added support for 7335.
 * 
 * 19   8/20/07 5:28p tokushig
 * PR23101: add member pointer initialization to skin constructor
 * initialization list to ensure premature delete of xmlElemTop does not
 * occur when constructor is called.  this will fix the sometimes random
 * crashes that occur in the skin when the brutus starts up.
 * 
 * 18   8/20/07 11:34a tokushig
 * PR23101: do not attempt to load skin xml file if option is not
 * specified
 * 
 * 17   8/10/07 1:22p ssood
 * PR33786: Net DMA & IP Streamer refactoring: replace STREAMER_SUPPORT
 * variable by NETACCEL_SUPPORT
 * 
 * 16   5/31/07 3:13p katrep
 * PR27643:Added support for 7405
 * 
 * 15   5/14/07 5:58p rjlewis
 * PR29784: crashes VxWorks on quit when using blue skin.
 * 
 * 14   5/2/07 3:22p jrubio
 * PR30010: fix the photo button when STREAMER_SUPPORT is used
 * 
 * 13   5/1/07 5:03p jrubio
 * PR30010: reuse the drm-nd skin+button
 * 
 * 12   4/24/07 7:01p piyushg
 * PR28588: Add DRM-NDR blue skin support
 * 
 * 11   4/24/07 12:52p jrubio
 * PR30010: added channel server screen for ip server
 * 
 * 10   3/14/07 10:16a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 * 
 * 9   9/18/06 1:49p tokushig
 * PR23446: handled brutus.cfg options in skin.  note that restricting
 * options when using a skin will not remove the associated button.  it
 * will only restrict functionality and in many cases display a popup to
 * notify the user that the features is disabled.
 * 
 * SanDiego_Brutus_Skin/24   9/18/06 12:52p tokushig
 * skin restricts/allows functionality for PLAYBACK_ENABLED,
 * RECORD_ENABLED, PICTURES_ENABLED, MP3_ENABLED, and ADMIN brutus.cfg flags
 * 
 * 8   9/14/06 4:26p tokushig
 * PR23444: show info window when information button in skin is pressed
 * 
 * SanDiego_Brutus_Skin/23   9/14/06 3:49p tokushig
 * correct info button to display info window when using skin
 * 
 * SanDiego_Brutus_Skin/22   8/17/06 5:04p tokushig
 * merge from main
 * 
 * 7   6/6/06 11:48a tokushig
 * PR20349: ensure all dynamically allocated bwin framebuffers are
 * destroyed before deleting display
 * 
 * SanDiego_Brutus_Skin/21   6/6/06 11:00a tokushig
 * clean up dynamic memory allocation in destructor
 * 
 * 6   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/20   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/19   4/27/06 11:04a tokushig
 * added border color for main window to skin
 * 
 * 2   4/17/06 7:47p jgarrett
 * PR 20951: Removing nonstandard ESC sequence
 * 
 * 1   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/18   4/5/06 1:32p tokushig
 * removed printout for found images/pixmaps
 * 
 * SanDiego_Brutus_Skin/17   3/13/06 5:25p tokushig
 * read pixel format as attribute in skin tag
 * 
 * SanDiego_Brutus_Skin/16   3/10/06 11:47a tokushig
 * moved printf of xml action to Skin::processAction() and converted it to
 * BDBG_MSG()
 * 
 * SanDiego_Brutus_Skin/15   3/9/06 11:16a tokushig
 * changed printf calls to BDBG_MSG calls
 * 
 * SanDiego_Brutus_Skin/14   2/28/06 1:27p tokushig
 * add dropshadow handling
 * 
 * SanDiego_Brutus_Skin/13   2/23/06 4:15p tokushig
 * updated platform description for menu screen
 * 
 * SanDiego_Brutus_Skin/12   2/23/06 10:06a tokushig
 * added button icon and focus text color attributes to skin
 * 
 * SanDiego_Brutus_Skin/11   2/22/06 3:55p tokushig
 * added ability to modify pip window geometry in skin
 * 
 * SanDiego_Brutus_Skin/10   2/16/06 11:37a piyushg
 * Add skin for the audio screen
 * 
 * SanDiego_Brutus_Skin/9   2/13/06 4:42p tokushig
 * added pixmap attribute to xml.  added record and save actions.
 * 
 * SanDiego_Brutus_Skin/8   2/3/06 4:43p tokushig
 * add drop shadow capability to tvwindow
 * 
 * SanDiego_Brutus_Skin/7   2/1/06 6:09p tokushig
 * changed xml screen names to match control class _screenname
 * 
 * SanDiego_Brutus_Skin/6   2/1/06 3:22p tokushig
 * added support for dynamic label update for platform name and
 * description
 * 
 * SanDiego_Brutus_Skin/5   1/26/06 9:56a tokushig
 * added ability to handle xml specified actions.
 * buffer used to read xml skin file now dynamically allocated/freed.
 * 
 * SanDiego_Brutus_Skin/4   1/19/06 12:52p tokushig
 * update for merges from main branch
 * 
 * SanDiego_Brutus_Skin/3   1/18/06 4:07p tokushig
 * change was tv window coords are calculated and add overscan adjustment.
 * this code assumes we are scaling from a full screen tv window.
 * 
 * SanDiego_Brutus_Skin/2   1/11/06 11:22a tokushig
 * add handling for pixmaps in addtion to images.
 * add mechanism by which individual screen code can quickly access xml
 * skin data.
 * 
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 * 
 * SanDiego_BSEAVSW_Devel/1   12/09/04 3:45p tokushige
 * Implemented basic XML parsing, conversion, storage for XML skin data
 ***************************************************************************/
#include "skin.h"
#include <unistd.h> //file i/o
#include <fcntl.h>
#include "cfgsettings.h"
#include "mlabel.h"
#include "mmessagebox.h"
#include "tvscreen.h"

#include "videoscreen.h"


#include "bstd.h"
BDBG_MODULE(brutus_skin);

//Constructor
Skin::Skin(Control * acontrol) :
    control(acontrol),
    xmlElemTop(NULL),
    xmlElemSkin(NULL),
    mapScreenElem(false), /* this map merely contains shortcuts into the xmlElemTop tree */
    mapWidgets(false) /* widget data will be auto deleted by parent window */
{
    initialize();
}

Skin::~Skin()
{
    if (xmlElemTop)
    {
        delete xmlElemTop;
        xmlElemTop = NULL;
        xmlElemSkin = NULL;
        bValid = false;
    }
}

//Clears out any previously store parsed data
void Skin::initialize()
{
    if (xmlElemTop)
    {
        delete xmlElemTop;
        xmlElemTop = NULL;
    }

    nBorderColor = 0;
    strName.clear();
    mapImages.clear();
    mapPixmaps.clear();
    mapScreenElem.clear();
    mapWidgets.clear();

    bValid = false;
}

//Parse() will open the given XML file and parse it for skin data.  XML data
//will be stored in xmlElemSkin tree structure for later processing.
void Skin::parse(const char * strFilename)
{
    initialize();

    int           fd       = open(strFilename, O_RDONLY, 0);
    char     *    buf      = NULL;
    int           nBufSize = 0;

    if (!fd)
    {
        BDBG_ERR(("Failed to open file '%s'!", strFilename));
        return;
    }

    buf = (char *)malloc(sizeof(char) * MAXBUF);
    if (!buf)
    {
        BDBG_ERR(("cannot malloc skin parse buffer!"));
        close(fd);
        return;
    }

    if (fd >= 0)
    {
        memset(buf, 0, sizeof(buf));
        nBufSize = read(fd, buf, MAXBUF);
        close(fd);
    }

    BDBG_MSG(("----------------------------------------------------------"));
    BDBG_MSG(("BUF:\n%s", buf));
    
    if (nBufSize <= 0)
    {
        BDBG_WRN(("Cannot open XML Skin file: %s", strFilename));
        if (buf)
        {
            free(buf);
            buf = NULL;
        }
        return;
    }

    MXmlParser    xmlParser;
    xmlElemTop = xmlParser.parse(buf);

    //BDBG_MSG("----------------------------------------------------------");
    //BDBG_MSG("xmlElemTop:");
    //xmlElemTop->print();

    if (!xmlElemTop)
    {
        //parse failed
        BDBG_ERR(("XML syntax error in: %s", strFilename));
        if (buf)
        {
            free(buf);
            buf = NULL;
        }
        return;
    }
    
    xmlElemSkin = xmlElemTop->findChild(XML_TAG_SKIN);
    if (xmlElemSkin)
    {
        //save skin name
        strName = xmlElemSkin->attrValue(XML_ATT_NAME);

        //save main window border color
        MString strBorderColor = xmlElemSkin->attrValue(XML_ATT_BORDER_COLOR, "");
        if (!strBorderColor.isEmpty())
        {
            nBorderColor = getColorValue(strBorderColor.s());
        }
    }

    bValid = true;

    if (buf)
    {
        free(buf);
        buf = NULL;
    }
}

//traverses XML tree and create all found images and pixmaps
//parse() must be called before createImagesPixmaps()
void Skin::createImagesPixmaps()
{
    if (xmlElemSkin)
    {
        //search for defined images and create resources - images will
        //be shared by all screens.
        createImagesR(xmlElemSkin);

#if 0
        BDBG_MSG(("----------------------------------------------------------"));
        BDBG_MSG(("found images:"));
        mapImages.print();
        BDBG_MSG(("----------------------------------------------------------"));
        BDBG_MSG(("found pixmaps:"));
        mapPixmaps.print();
#endif
    }
}

//returns true if given screen is found in xml skin.  returns false otherwise.
void Skin::createWidgets(const char * strScreenName, GenericScreen * screen)
{
    MXmlElement * xmlElemCurrent = NULL;

    if (!isValid())
    {
        return;
    }

    if (!strScreenName)
    {
        return;
    }

    if (!screen)
    {
        return;
    }

    //search xml tree to find screen tag with name attribute that matches given 
    //strScreenName
    xmlElemCurrent = 
        findTagAttrValue(XML_TAG_SCREEN, XML_ATT_NAME, strScreenName, xmlElemSkin);

    if (xmlElemCurrent)
    {
        BDBG_MSG(("creating %s screen widget", strScreenName));

        /*********************/
        /*** SCREEN WIDGET ***/
        /*********************/
        MString         strColor;
        MString         strImage;
        MString         strPixmap;
        MString         strBevelColor;
        uint32_t        x            = 0;
        uint32_t        y            = 0;
        uint32_t        width        = 0;
        uint32_t        height       = 0;
        uint32_t        tvX          = 0;
        uint32_t        tvY          = 0;
        uint32_t        tvWidth      = 0;
        uint32_t        tvHeight     = 0;
        uint32_t        dropShadow   = 0;
        uint32_t        bevelWidth   = 0;

        getAttrValue(xmlElemCurrent, XML_ATT_COLOR,       strColor);
        getAttrValue(xmlElemCurrent, XML_ATT_IMAGE,       strImage);
        getAttrValue(xmlElemCurrent, XML_ATT_PIXMAP,      strPixmap);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_H,      height);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_W,      width);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_X,      x);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_Y,      y);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_H,   tvHeight);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_W,   tvWidth);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_X,   tvX);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_Y,   tvY);
        getAttrValue(xmlElemCurrent, XML_ATT_DROP_SHADOW, dropShadow);
        getAttrValue(xmlElemCurrent, XML_ATT_BEVEL_WIDTH, bevelWidth);
        getAttrValue(xmlElemCurrent, XML_ATT_BEVEL_COLOR, strBevelColor);

        //set color if necessary
        if (!strColor.isEmpty())
        {
            screen->setBackgroundColor(getColorValue(strColor.s()));
        }
        
        //set background image if necessary
        if (!strImage.isEmpty())
        {
            screen->setBackgroundImage(getImage(strImage.s()));
        }
        
        //set background pixmap if necessary
        if (!strPixmap.isEmpty())
        {
            screen->setBackgroundPixmap(getPixmap(strPixmap.s()));
        }

        //set geometry if necessary
        if ((width > 0) || (height > 0))
        {
            screen->setGeometry(x, y, width, height);
        }

        //set tvwindow geometry if necessary
        if ((tvWidth > 0) && (tvHeight > 0))
        {
            screen->setGeometryTV(MRect(tvX, tvY, tvWidth, tvHeight));
        }

        //set dropshadow if necessary
        if (dropShadow > 0)
        {
            control->cfg()->set(Config::TVSCREEN_MESSAGE_FONT_DROPSHADOW, true);
        }

        //set tvwindow bevel width if necessary
        if (bevelWidth > 0)
        {
            //update config data so tvwindow bevel is drawn correctly
            control->cfg()->set(Config::TVBEVEL_WIDTH, (int)bevelWidth);
        }

        //set tvwindow bevel color if necessary
        if (!strBevelColor.isEmpty())
        {
            //update config setting so tvwindow bevel is drawn with correct color
            control->cfg()->set(Config::TVBEVEL_COLOR, 
                                (int)getColorValue(strBevelColor.s()));
        }

        //store xml tree screen element pointer for later fast lookup.  screen
        //specific code will use this to lookup screen attributes without having
        //to search the entire xml tree.  this will also serve to hide the 
        //storage types used in the xml tree from screen specific code.
        const char * strScreenName = xmlElemCurrent->attrValue(XML_ATT_NAME);
        mapScreenElem.add(strScreenName, xmlElemCurrent);
        screen->setName(strScreenName);

        //continue to search xml tree for defined widgets and create them
        createWidgetsR(xmlElemCurrent, screen);
    }
    else
    {
        BDBG_WRN(("screen name not found in XML skin: %s", strScreenName));
    }
}

//recursive routine to find handled widgets and create them
void Skin::createWidgetsR(MXmlElement * elem, GenericScreen * screen)
{
    if (!elem)
    {
        return;
    }

    if (!screen)
    {
        return;
    }

    BDBG_MSG(("tag:%s", elem->tag().s()));

    MWidget     *   pWidget     = NULL;
    MString         strText;
    MString         strColorText;
    MString         strColorTextFocus;
    MString         strColor;
    MString         strBkgndImage;
    MString         strFont;
    MString         strAction;
    uint32_t        x           = 0;
    uint32_t        y           = 0;
    uint32_t        width       = 0;
    uint32_t        height      = 0;
    uint32_t        dropshadow  = 0;

    //get attributes inheirited from MWidget class
    getTagValue(elem, strText);

    getAttrValue(elem, XML_ATT_ACTION,            strAction);
    getAttrValue(elem, XML_ATT_BKGND_IMAGE,       strBkgndImage);
    getAttrValue(elem, XML_ATT_COLOR,             strColor);
    getAttrValue(elem, XML_ATT_COLOR_TEXT,        strColorText);
    getAttrValue(elem, XML_ATT_COLOR_TEXT_FOCUS,  strColorTextFocus);
    getAttrValue(elem, XML_ATT_FONT,              strFont);
    getAttrValue(elem, XML_ATT_GEOM_H,            height);
    getAttrValue(elem, XML_ATT_GEOM_W,            width);
    getAttrValue(elem, XML_ATT_GEOM_X,            x);
    getAttrValue(elem, XML_ATT_GEOM_Y,            y);
    getAttrValue(elem, XML_ATT_DROP_SHADOW,       dropshadow);

    if (elem->tag() == XML_TAG_LABEL)
    {
        /***************************/
        /*** CREATE LABEL WIDGET ***/
        /***************************/
        
        MString         strName;
        MString         strImage;
        MString         strScale;
        MString         strBevel;
        uint32_t        bevelWidth  = 0;
        uint32_t        border      = 0;
        MString         strHAlign;
        MString         strVAlign;
        MString         strWrap;

        getAttrValue(elem, XML_ATT_ALIGN_H,     strHAlign);
        getAttrValue(elem, XML_ATT_ALIGN_V,     strVAlign);
        getAttrValue(elem, XML_ATT_BEVEL,       strBevel);
        getAttrValue(elem, XML_ATT_BEVEL_WIDTH, bevelWidth);
        getAttrValue(elem, XML_ATT_BORDER,      border);
        getAttrValue(elem, XML_ATT_IMAGE,       strImage);
        getAttrValue(elem, XML_ATT_NAME,        strName);
        getAttrValue(elem, XML_ATT_SCALE,       strScale);
        getAttrValue(elem, XML_ATT_WRAP,        strWrap);

        if (!strName.isEmpty())
        {
            MLabel * label = NULL;

            BDBG_MSG(("create label: %s", strName.s()));

            label = new MLabel(screen, strName.s());
            pWidget = label;

            if (label)
            {
                //set bevel if necessary
                if ((bevelWidth > 0) && (!strBevel.isEmpty()))
                {
                    MPainter::BevelStyle bevelType = MPainter::bsRaised;
                    if (strBevel == XML_BEVEL_SUNKEN)
                    {
                        bevelType = MPainter::bsSunken;
                    }
                    if (strBevel == XML_BEVEL_UPDOWN)
                    {
                        bevelType = MPainter::bsUpDown;
                    }

                    label->setBevel(bevelWidth, bevelType);
                }

                //set image if necessary
                if (!strImage.isEmpty())
                {
                    MPainter::DrawImageMode scaleMode = MPainter::eSingle;

                    if (!strScale.isEmpty())
                    {
                        if (strScale == XML_SCALE_STRETCH)
                        {
                            scaleMode = MPainter::eStretch;
                        }
                        else
                        if (strScale == XML_SCALE_MAX)
                        {
                            scaleMode = MPainter::eMaximize;
                        }
                        else
                        if (strScale == XML_SCALE_DOWN)
                        {
                            scaleMode = MPainter::eScaleDownToMax;
                        }
                    }

                    label->setImage(getImage(strImage.s()), scaleMode);
                }

                //set border if necessary
                if (border > 0)
                {
                    label->setBorder(border);
                }

                //set horizontal alignment if necessary
                if (!strHAlign.isEmpty())
                {
                    MPainter::Alignment align = MPainter::alLeft;

                    if (strHAlign == XML_ALIGN_RIGHT)
                    {
                        align = MPainter::alRight;
                    }
                    else
                    if (strHAlign == XML_ALIGN_CENTER)
                    {
                        align = MPainter::alCenter;
                    }

                    label->setAlignment(align);
                }

                //set vertical alignment if necessary
                if (!strVAlign.isEmpty())
                {
                    MPainter::VAlignment align = MPainter::valTop;

                    if (strHAlign == XML_ALIGN_BOTTOM)
                    {
                        align = MPainter::valBottom;
                    }
                    else
                    if (strHAlign == XML_ALIGN_CENTER)
                    {
                        align = MPainter::valCenter;
                    }

                    label->setVAlignment(align);
                }

                //set text wrap mode if necessary
                if (!strWrap.isEmpty())
                {
                    MLabel::WrapMode wrap = MLabel::NoWrap;

                    if (strWrap == XML_WRAP_WORD)
                    {
                        wrap = MLabel::WordWrap;
                    }
                    else
                    if (strWrap == XML_WRAP_NEWLINE)
                    {
                        wrap = MLabel::NewLineWrap;
                    }

                    label->setWrapMode(wrap);
                }

                mapWidgets.add(strName.s(), (MWidget *)label);
            }
        }
    }
    else
    if (elem->tag() == XML_TAG_BUTTON)
    {
        /****************************/
        /*** CREATE BUTTON WIDGET ***/
        /****************************/

        MString         strName;
        uint32_t        bevelWidth  = 0;
        MString         strHAlign;
        MString         strVAlign;
        MString         strImageOn;
        MString         strImageOff;
        MString         strImageDown;
        MString         strImageIcon;

        getAttrValue(elem, XML_ATT_ALIGN_H,     strHAlign);
        getAttrValue(elem, XML_ATT_ALIGN_V,     strVAlign);
        getAttrValue(elem, XML_ATT_BEVEL_WIDTH, bevelWidth);
        getAttrValue(elem, XML_ATT_IMAGE_ON,    strImageOn);
        getAttrValue(elem, XML_ATT_IMAGE_OFF,   strImageOff);
        getAttrValue(elem, XML_ATT_IMAGE_DOWN,  strImageDown);
        getAttrValue(elem, XML_ATT_IMAGE_ICON,  strImageIcon);
        getAttrValue(elem, XML_ATT_NAME,        strName);

        if (!strName.isEmpty())
        {
            MPushButton * button = NULL;

            BDBG_MSG(("create pushbutton: %s", strName.s()));

            button  = new MPushButton(screen, 
                                      MRect(x, y, width, height),
                                      strText.s(), 
                                      strName.s());

            button->setImages(!strImageOff.isEmpty()  ? getImage(strImageOff.s()) : NULL,
                              !strImageDown.isEmpty() ? getImage(strImageDown.s()) : NULL,
                              !strImageOn.isEmpty()   ? getImage(strImageOn.s()) : NULL);

            button->setIcon(!strImageIcon.isEmpty() ? getImage(strImageIcon.s()) : NULL);

            pWidget = button;

            if (button)
            {
                //set horizontal alignment if necessary
                if (!strHAlign.isEmpty())
                {
                    MPainter::Alignment align = MPainter::alLeft;

                    if (strHAlign == XML_ALIGN_RIGHT)
                    {
                        align = MPainter::alRight;
                    }
                    else
                    if (strHAlign == XML_ALIGN_CENTER)
                    {
                        align = MPainter::alCenter;
                    }

                    button->setAlignment(align);
                }

                //set vertical alignment if necessary
                if (!strVAlign.isEmpty())
                {
                    MPainter::VAlignment align = MPainter::valTop;

                    if (strHAlign == XML_ALIGN_BOTTOM)
                    {
                        align = MPainter::valBottom;
                    }
                    else
                    if (strHAlign == XML_ALIGN_CENTER)
                    {
                        align = MPainter::valCenter;
                    }

                    button->setVAlignment(align);
                }

                button->addHandler(screen);

                mapWidgets.add(strName.s(), (MWidget *)button);
            }
        }
    }

    //set attributes inheirited from MWidget class
    if (pWidget)
    {
        pWidget->raise();

        //set text if necessary
        if (!strText.isEmpty())
        {
            pWidget->setText(strText.s());
        }

        //set font if necessary
        if (!strFont.isEmpty())
        {
            strFont = control->cfg()->get(Config::FONTS_PATH) + ("/" + strFont);
            pWidget->setFont(MFont(screen->app(), strFont.s()));
            BDBG_MSG(("using font file: %s", strFont.s()));
        }

        //set dropshadow if necessary
        if (dropshadow)
        {
            pWidget->setDropShadow(true);
        }

        //set geometry if necessary
        if ((width > 0) || (height > 0))
        {
            pWidget->setGeometry(x, y, width, height);
        }

        //set background image if necessary
        if (!strBkgndImage.isEmpty())
        {
            pWidget->setBackgroundPixmap(getPixmap(strBkgndImage.s()));
        }

        //set text color if necessary
        if (!strColorText.isEmpty())
        {
            pWidget->setTextColor(getColorValue(strColorText.s()));
        }

        //set focus text color if necessary
        if (!strColorTextFocus.isEmpty())
        {
            pWidget->setTextColorFocus(getColorValue(strColorTextFocus.s()));
        }

        //set color if necessary - note that background image
        //take precedence over color if specified
        if (!strColor.isEmpty())
        {
            pWidget->setBackgroundColor(getColorValue(strColor.s()));
        }

        //bind action to widget if necessary
        //screen specific actions are
        //handled by the associated window.  generic actions 
        //such as 'showing screens' are handled by the skin to
        //allow control placement on any window thru the xml skin file.
        pWidget->setAction(strAction2enum(strAction.s()));
        pWidget->setActionData((void *)screen);
    }

    if (elem->totalChildren() == 0)
    {
        return;
    }

    if ((elem->totalChildren() == 1) && (elem->firstChild()->tag().isNull()))
    {
        return;
    }

    for (MXmlElement * elemChild = elem->firstChild();
         elemChild;
         elemChild = elem->nextChild())
    {
        createWidgetsR(elemChild, screen);
    }
}

void Skin::destroyWidgets(const char * strScreenName)
{
    //TODO: destroy created widgets!
}

static int checkfile(const char *filename)
{
    int64_t size;
    if (filename && b_get_file_size(filename, &size)) {
        /* can't use debug interface at this point */
        fprintf(stderr, "*** '%s' does not exist\n", filename);
        return 0;
    }
    return 1;
}

//find all images - if the skin file has image tags with duplicate names, 
//only the last one will be valid.  found images will have MImage 
//widgets automatically created for them.  these images are created globally
//so they can be shared between screens.
void Skin::createImagesR(MXmlElement * elem)
{
    MXmlAttribute * attr = NULL;

    if (!elem)
    {
        return;
    }
    BDBG_MSG(("tag:%s", elem->tag().s()));

    if (elem->tag() == XML_TAG_IMAGE)
    {
        MString strName;
        MString strPath;

        getAttrValue(elem, XML_ATT_NAME, strName);
        getTagValue(elem, strPath);
        strPath = control->cfg()->get(Config::SKINS_PATH) + ("/" + getName()) + "/images/" + strPath;
        BDBG_MSG(("image path: %s", strPath.s()));

        if (!strName.isEmpty())
        {
            BDBG_MSG(("create image: %s:%s", strName.s(), strPath.s()));
            if (checkfile(strPath.s()))
            {
                mapImages.add(strName.s(), 
                              new MImage(control->fb()->app(), strPath.s()));
            }
        }
    }
    else
    if (elem->tag() == XML_TAG_PIXMAP)
    {
        MString strName;
        MString strPath;

        getAttrValue(elem, XML_ATT_NAME, strName);
        getTagValue(elem, strPath);
        strPath = control->cfg()->get(Config::SKINS_PATH) + ("/" + getName()) + "/images/" + strPath;
        BDBG_MSG(("pixmap path: %s", strPath.s()));

        if (!strName.isEmpty())
        {
            BDBG_MSG(("create pixmap: %s:%s", strName.s(), strPath.s()));
            if (checkfile(strPath.s()))
            {
                MImage * pImage = new MImage(control->fb()->app(), strPath.s());
                mapPixmaps.add(strName.s(), new MPixmap(control->fb(), *pImage));

                delete pImage;
                pImage = NULL;
            }
        }
    }
    
    if (elem->totalChildren() == 0)
    {
        return;
    }

    if ((elem->totalChildren() == 1) && (elem->firstChild()->tag().isNull()))
    {
        return;
    }

    for (MXmlElement * elemChild = elem->firstChild();
         elemChild;
         elemChild = elem->nextChild())
    {
        createImagesR(elemChild);
    }
}

MImage * Skin::getImage(const char * strName)
{
    MImage * pRet = NULL;

    if (!strName)
    {
        return NULL;
    }

    MImage * pImg = mapImages.get(strName);
    if (pImg)
    {
        pRet = pImg;
    }

    return pRet;
}

MPixmap * Skin::getPixmap(const char * strName)
{
    MPixmap * pRet = NULL;

    if (!strName)
    {
        return NULL;
    }

    MPixmap * pImg = mapPixmaps.get(strName);
    if (pImg)
    {
        pRet = pImg;
    }

    return pRet;
}

int32_t Skin::getColorValue(const char * color)
{
    MString strColor = color;
    int32_t nColor   = -1;

    if (!strColor)
    {
        return nColor;
    }

    if (strColor[0] == '#')
    {
        //colors that begin with '#' are in hex
        strColor.removeChar(0);

        if (strColor.length() == 6)
        {
            //specified value does NOT have alpha value
            //so assume no transparency
            strColor = "0xFF" + strColor;
        }
        else
        {
            //specified color already has alpha value
            strColor = "0x" + strColor;
        }

        nColor   = strColor.toInt();
    }
    else
    {
        //translate english color names
        //taken from http://www.visibone.com/colorlab/
        if (strColor == XML_COLOR_BLUE)
        {
            nColor = 0xFF0000FF;
        }
        else
        if (strColor == XML_COLOR_GREEN)
        {
            nColor = 0xFF00FF00;
        }
        else
        if (strColor == XML_COLOR_RED)
        {
            nColor = 0xFFFF0000;
        }
        else
        if (strColor == XML_COLOR_MAGENTA)
        {
            nColor = 0xFFFF00FF;
        }
        else
        if (strColor == XML_COLOR_CYAN)
        {
            nColor = 0xFF00FFFF;
        }
        else
        if (strColor == XML_COLOR_YELLOW)
        {
            nColor = 0xFFFFFF00;
        }
        else
        if (strColor == XML_COLOR_BLACK)
        {
            nColor = 0xFF000000;
        }
        else
        if (strColor == XML_COLOR_WHITE)
        {
            nColor = 0xFFFFFFFF;
        }
    }

    return nColor;
}

//returns the given color value with the given changed alpha value
int32_t Skin::changeAlpha(int32_t nColor, unsigned char byte)
{
    uint32_t alphaMask = byte;
    return  ((nColor & 0x00ffffff) | ((alphaMask << 24) & 0xff000000));
}

//returns the pip window size adjusted by the current display settings
MRect Skin::getPIPWindowSize(const char        * strScreenName, 
                            bdisplay_settings * displaySettings)
{
    MXmlElement * xmlElemCurrent = NULL;
    MRect         rectPIPWin(0,0,0,0);

    if (!strScreenName)
    {    
        return MRect(0,0,0,0);
    }

    if (!isValid())
    {
        return MRect(0,0,0,0);
    }

    //search xml tree to find screen tag with name that matches given 
    //strScreenName
    xmlElemCurrent = findTagAttrValue(XML_TAG_SCREEN, XML_ATT_NAME, strScreenName, xmlElemSkin);

    if (xmlElemCurrent)
    {
        //determine specified screen size
        uint32_t        pipX          = 0;
        uint32_t        pipY          = 0;
        uint32_t        pipWidth      = 0;
        uint32_t        pipHeight     = 0;

        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_PIP_H,   pipHeight);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_PIP_W,   pipWidth);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_PIP_X,   pipX);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_PIP_Y,   pipY);

        if (pipHeight == 0 || pipWidth == 0)
        {
            //no pip attribute specified so use tvwindow attributes instead
            return getTVWindowSize(strScreenName, displaySettings);
        }

        uint32_t screenX = control->cfg()->getInt(Config::SCREEN_X);
        uint32_t screenY = control->cfg()->getInt(Config::SCREEN_Y);
    
        int32_t nOverscan = -3;
        if (displaySettings)
        {
            //this case is used to determine the size/position of the 
            //pip video window itself...shrink slightly so graphics overlaps 
            //edge of video
            bvideo_format_settings formatSettings;
            bdisplay_get_video_format_settings(displaySettings, &formatSettings);

            rectPIPWin.set((pipX + screenX) * formatSettings.width / 720 - nOverscan, 
                           (pipY + screenY) * formatSettings.height / 480 - nOverscan, 
                           pipWidth * formatSettings.width / 720 + (nOverscan * 2),
                           pipHeight * formatSettings.height / 480 + (nOverscan * 2));
        }
        else
        {
            //this case is used to determine the size/position of the 
            //yellow/black pip outline 
            rectPIPWin.set(pipX, pipY, pipWidth, pipHeight);
        }
    }

    return rectPIPWin;
}

//returns the tvwindow size adjusted by the current display settings
MRect Skin::getTVWindowSize(const char        * strScreenName, 
                            bdisplay_settings * displaySettings)
{
    MXmlElement * xmlElemCurrent = NULL;
    MRect         rectTVWin(0,0,0,0);

    if (!strScreenName)
    {    
        return MRect(0,0,0,0);
    }

    if (!isValid())
    {
        return MRect(0,0,0,0);
    }

    //search xml tree to find screen tag with name that matches given 
    //strScreenName
    xmlElemCurrent = findTagAttrValue(XML_TAG_SCREEN, XML_ATT_NAME, strScreenName, xmlElemSkin);

    if (xmlElemCurrent)
    {
        //determine specified screen size
        uint32_t        tvX          = 0;
        uint32_t        tvY          = 0;
        uint32_t        tvWidth      = 0;
        uint32_t        tvHeight     = 0;

        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_H,   tvHeight);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_W,   tvWidth);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_X,   tvX);
        getAttrValue(xmlElemCurrent, XML_ATT_GEOM_TV_Y,   tvY);

        bvideo_format_settings formatSettings;
        bdisplay_get_video_format_settings(displaySettings, &formatSettings);

        if (tvHeight == 0 || tvWidth == 0)
        {
            //no tvwindow attribute specified so use max size
            return MRect(0, 0, formatSettings.width, formatSettings.height);
        }

        uint32_t screenX = control->cfg()->getInt(Config::SCREEN_X);
        uint32_t screenY = control->cfg()->getInt(Config::SCREEN_Y);
    
        int32_t nOverscan = 5;
        if (displaySettings)
        {
            //this is used to get the size of the tv window...so we'll enlarge it
            //slightly so the overlayed graphics covers the edges.
            //
            //we'll add some overscan 
            rectTVWin.set((tvX + screenX) * formatSettings.width / 720 - nOverscan, 
                          (tvY + screenY) * formatSettings.height / 480 - nOverscan, 
                          tvWidth * formatSettings.width / 720 + (nOverscan * 2),
                          tvHeight * formatSettings.height / 480 + (nOverscan * 2));
        }
        else
        {
            //this case is used to determine the size/position of the tvwindow
            //outline
            rectTVWin.set(tvX, tvY, tvWidth, tvHeight);
        }
    }

    return rectTVWin;
}

//get string attribute from given XML tree element
void Skin::getAttrValue(MXmlElement * elem, const char * strName, MString & strValue)
{
    strValue = "";

    if (!elem)
    {
        return;
    }

    MXmlAttribute * attr = NULL;
    attr = elem->findAttr(strName);
    if (attr)
    {
        strValue = attr->value();
    }
}

//get unsigned attribute from given XML tree element
void Skin::getAttrValue(MXmlElement * elem, const char * strName, uint32_t & uValue)
{
    uValue = 0;

    if (!elem)
    {
        return;
    }

    MXmlAttribute * attr = NULL;
    attr = elem->findAttr(strName);
    if (attr)
    {
        uValue = attr->value().toLong();
    }
}

//get tag string name from given XML tree element
void Skin::getTagValue(MXmlElement * elem, MString & strValue)
{
    strValue = "";

    if (!elem)
    {
        return;
    }

    strValue = elem->childData();
}

//get string attribute value from skin tag 
void Skin::getSkinAttrValue(const char * strName, MString & strValue)
{
    strValue.clear();
    if (xmlElemSkin)
    {
        getAttrValue(xmlElemSkin, strName, strValue);
    }
}

//get unsigned attribute value from skin tag 
void Skin::getSkinAttrValue(const char * strName, uint32_t & uValue)
{
    uValue = 0;
    if (xmlElemSkin)
    {
        getAttrValue(xmlElemSkin, strName, uValue);
    }
}

//get string attribute value from given screen - note that this works only for 
//screen based attributes
void Skin::getAttrValue(const char * strScreen, const char * strName, MString & strValue)
{
    getAttrValue(mapScreenElem.get(strScreen), strName, strValue);
}

//get unsigned attribute value from given screen - note that this works only for 
// screen based attributes
void Skin::getAttrValue(const char * strScreen, const char * strName, uint32_t & uValue)
{
    getAttrValue(mapScreenElem.get(strScreen), strName, uValue);
}


//returns the XML tree element with the given tag, attribute, and value.  
//returns NULL otherwise.
MXmlElement * Skin::findTagAttrValue(const char * strTag, const char * strAttr, const char * strAttrValue, MXmlElement * elem)
{
    if (!strTag)
    {
        return NULL;
    }
    if (!strAttr)
    {
        return NULL;
    }
    if (!strAttrValue)
    {
        return NULL;
    }
    if (!elem)
    {
        return NULL;
    }

    if (elem->tag() == strTag)
    {
        MString strName;
        getAttrValue(elem, strAttr, strName);
        if (strName == strAttrValue)
        {
            return elem;
        }
    }

    if (elem->totalChildren() == 0)
    {
        return NULL;
    }

    if ((elem->totalChildren() == 1) && (elem->firstChild()->tag().isNull()))
    {
        return NULL;
    }

    for (MXmlElement * elemChild = elem->firstChild();
         elemChild;
         elemChild = elem->nextChild())
    {
        MXmlElement * elemTemp = NULL;
        elemTemp = findTagAttrValue(strTag, strAttr, strAttrValue, elemChild);
        if (elemTemp)
        {
            return elemTemp;
        }
    }

    return NULL;
}

//if strScreen is not specified by the caller, we will return true if the skin object
//has been initialized, and false otherwise.
//if strScreen is specified by the caller, we will attempt to find that screen tag
//in the skin's XML tree.  returns true if it is found, false otherwise.
bool Skin::isValid(const char * strScreen)
{
    bool bRet = false;

    if (!strScreen)
    {
        return bValid;
    }

    if (findTagAttrValue(XML_TAG_SCREEN, XML_ATT_NAME, strScreen, xmlElemSkin))
    {
        bRet = true;
    }

    return bRet;
}

//execute skin action 
void Skin::processAction(MWidget * widget, int action)
{
    BDBG_MSG(("process XML action:%d", action));
    switch (action) 
    {
    case eActionShowInfoWin:
        control->showScreen(Control::eTV);
        ((TVScreen*)control->screen(Control::eTV))->showMessage(1);
        break;

    case eActionShowAdminWin:
        if (control->cfg()->getBool(Config::ADMIN))
        {
            control->showScreen(Control::eAdmin);
        }
        else
        {
        if (control->verifyAdminPassword())
            {
                control->showScreen(Control::eAdmin);
            }
        }
        break;

    case eActionShowAudioWin:
        if (control->cfg()->getBool(Config::MP3_ENABLED))
        {
        control->showScreen(Control::eAudio);
        }
        else
        {
            MMessageBox::info(control->fb(), "Feature Disabled");
        }
        break;
        
    case eActionShowPhotoWin:

        if (control->cfg()->getBool(Config::PICTURES_ENABLED))
        {
        control->showScreen(Control::ePictures);
        }
        else
        {
            MMessageBox::info(control->fb(), "Feature Disabled");
        }
        break;

    case eActionShowPlaybackWin:
        if (control->cfg()->getBool(Config::PLAYBACK_ENABLED))
        {
            control->showScreen(Control::ePlayback);
        }
        else
        {
            MMessageBox::info(control->fb(), "Feature Disabled");
        }
        break;
        
    case eActionShowHdmiWin:
        control->showScreen(Control::eHdmi);
        break;

#if defined B_HAS_DRM_ND  
    case eActionShowExtServerWin:
        /* channel map is already available below, does IP still need a channnel map?? */
        control->showScreen(Control::eExtserver);
        break;
#endif

    case eActionShowRecordWin:
        if (control->cfg()->getBool(Config::RECORD_ENABLED))
        {
        control->showScreen(Control::eRecord);
        }
        else
        {
            MMessageBox::info(control->fb(), "Feature Disabled");
        }
        break;

    case eActionShowVideoWin:
        if (control->cfg()->getBool(Config::PLAYBACK_ENABLED))
        {
        control->showScreen(Control::eVideo);
        }
        else
        {
            MMessageBox::info(control->fb(), "Feature Disabled");
        }
        break;

    case eActionShowChanWin:
        control->showScreen(Control::eChanMap);
        break;

    case eActionPlatformName:
    {
        MString strBoardName = BCM_BOARD;
        widget->setText(strBoardName.s());
    }
        break;

    case eActionPlatformNameLong:
    {
        MString strBoardName(BCM_BOARD);
        strBoardName = "BROADCOM " + strBoardName;
        widget->setText(strBoardName.s());
    }
        break;

    case eActionPlatformDesc:
    {
        MString strBoardDesc;
        if (BCM_BOARD == 97401)
        {
            strBoardDesc = "AVC/VC-1/HD/PVR STB Reference Platform";
        }
        else
        if (BCM_BOARD == 97400)
        {
            strBoardDesc = "Dual AVC/VC-1/MPEG-2 HD Media Center";
        }
        else
        if (BCM_BOARD == 97398)
        {
            strBoardDesc = "Dual AVC/HD/PVR STB Reference Platform";
        }
        else
        if (BCM_BOARD == 97038)
        {
            strBoardDesc = "Dual HD/PVR STB Reference Platform";
        }
        else
        if (BCM_BOARD == 97405)
        {
            strBoardDesc = "AVC/VC-1/HD/PVR IP STB Reference Platform";
        }
        else
        if (BCM_BOARD == 97335)
        {
            strBoardDesc = "AVC/VC-1/HD/PVR Satellite/IP STB Reference Platform";
        }
        if (BCM_BOARD == 97342)
        {
            strBoardDesc = "AVC/VC-1/HD/PVR Satellite/IP STB Reference Platform";
        }
        if (BCM_BOARD == 97325)
        {
            strBoardDesc = "AVC/VC-1/HD Satellite/IP STB Reference Platform";
        }
        if (BCM_BOARD == 97340)
        {
            strBoardDesc = "AVC/VC-1/HD Satellite/IP STB Reference Platform";
        }
        else
        {
            strBoardDesc = "STB Reference Platform";
        }

        widget->setText(strBoardDesc.s());
    }
        break;

    case eActionVideoTitle:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            widget->setText(pScreen->video()->title);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoDate:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            char rec_time[32];
            memset(rec_time, 0, sizeof(rec_time));
            b_print_calendar_time(rec_time, 32, pScreen->video()->recordtime);
            widget->setText(rec_time);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoFilename:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            widget->setText(pScreen->video()->mediafile);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoSize:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            char vid_size[32];
            memset(vid_size, 0, sizeof(vid_size));
            b_print_size(vid_size, 32, pScreen->video()->size());
            widget->setText(vid_size);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoLength:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            char vid_length[32];
            memset(vid_length, 0, sizeof(vid_length));
            b_print_time_period(vid_length, 32, pScreen->video()->length(), 
                                B_TIME_PERIOD_FORMAT_COMPACT);
            widget->setText(vid_length);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoIndex:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            if (pScreen->video()->indexFormat() == bindex_format_bcm)
            {
                widget->setText("Broadcom (NAV)");
            }
            else
            if (pScreen->video()->indexFormat() == bindex_format_sct)
            {
                widget->setText("Start Code");
            }
            else
            {
                widget->setText("None");
            }
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoVFormat:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            widget->setText(b_vidxpt_str(pScreen->video()->mpeg.video[0].format, 
                         pScreen->video()->mpeg.mpeg_type));
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoAFormat:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            widget->setText(b_audtype_str(pScreen->video()->mpeg.audio[0].format));
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoVPid:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            char vid_vpid[32];
            memset(vid_vpid, 0, sizeof(vid_vpid));
            
           snprintf(vid_vpid, sizeof(vid_vpid), "0x%x", 
                         pScreen->video()->mpeg.video[0].pid);
           widget->setText(vid_vpid);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoAPid:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            char vid_apid[32];
            memset(vid_apid, 0, sizeof(vid_apid));
            
            snprintf(vid_apid, sizeof(vid_apid), "0x%x", 
                         pScreen->video()->mpeg.audio[0].pid);
            widget->setText(vid_apid);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    case eActionVideoEncryption:
    {
        //TODO: check widget->className() to ensure actionData() holds the
        //      proper type of widget.
        VideoScreen * pScreen = (VideoScreen *)widget->actionData();
        if (pScreen)
        {
            widget->setText(g_encryptionStr[pScreen->video()->mpeg.encryption.type]);
        }
        else
        {
            widget->setText(ERROR_TEXT_NOT_AVAIL);
        }
    }
        break;

    default:
        break;
    }
}

//execute action associated with given widget
void Skin::processAction(MWidget * widget)
{
    processAction(widget, widget->action());
}

//execute action on all widgets associated with that action on the given screen 
void Skin::processActionLabel(const char * strScreen, eSkinAction action)
{
    if (!strScreen)
    {
        //TODO: if screen is NULL, then do for all screens in skin
        return;
    }

    processActionLabelR(mapScreenElem.get(strScreen), action);
}

//recursive routine to find widgets with the given action 
//(starting at the element: elem) and then execute that action
void Skin::processActionLabelR(MXmlElement * elem, eSkinAction action)
{
    if (!elem)
    {
        return;
    }

    if (action == eActionInvalid)
    {
        return;
    }

    MString strAction;
    getAttrValue(elem, XML_ATT_ACTION,      strAction);
    if (strAction2enum(strAction.s()) == action)
    {
        //found label with matching action - execute action
        MString strName;
        getAttrValue(elem, XML_ATT_NAME, strName);
        processAction(getWidget(strName));
    }

    if (elem->totalChildren() == 0)
    {
        return;
    }

    if ((elem->totalChildren() == 1) && (elem->firstChild()->tag().isNull()))
    {
        return;
    }

    for (MXmlElement * elemChild = elem->firstChild();
         elemChild;
         elemChild = elem->nextChild())
    {
        processActionLabelR(elemChild, action);
    }
}

MWidget * Skin::getWidget(const char * strName)
{
    if (!strName)
    {
        return NULL;
    }

    return mapWidgets.get(strName);
}

eSkinAction Skin::strAction2enum(const char * action)
{
    if (!action)
    {
        return(eActionInvalid);
    }

    MString strAction = action;
    if (!strAction.isEmpty())
    {
        if (strAction == XML_ACTION_SHOW_ADMIN_WIN)
        {
            return(eActionShowAdminWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_AUDIO_WIN)
        {
            return(eActionShowAudioWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_INFO_WIN)
        {
            return(eActionShowInfoWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_MENU_WIN)
        {
            return(eActionShowMenuWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_PHOTO_WIN)
        {
            return(eActionShowPhotoWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_VIDEO_WIN)
        {
            return(eActionShowVideoWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_PLAYBACK_WIN)
        {
            return(eActionShowPlaybackWin);
        }
        else
        if (strAction == XML_ACTION_SHOW_HDMI_WIN)
        {
            return(eActionShowHdmiWin);
        }
#if defined B_HAS_DRM_ND || defined B_HAS_IP
        else
        if (strAction == XML_ACTION_SHOW_EXTSERVER_WIN)
        {
#ifdef B_HAS_IP
            return(eActionShowChanWin);
#else
            return(eActionShowExtServerWin);
#endif
        }
#endif
        else
        if (strAction == XML_ACTION_SHOW_RECORD_WIN)
        {
            return(eActionShowRecordWin);
        }
        else

        if (strAction == XML_ACTION_SHOW_CHAN_WIN)
        {
          return(eActionShowChanWin);
        }
        else

        if (strAction == XML_ACTION_PLAY_VIDEO)
        {
            return(eActionPlayVideo);
        }
        else
        if (strAction == XML_ACTION_PLAY_VIDEO_TRANSCODE)
        {
            return(eActionPlayVideoTranscode);
        }
        else
        if (strAction == XML_ACTION_RESUME_VIDEO)
        {
            return(eActionResumeVideo);
        }
        else
        if (strAction == XML_ACTION_RENAME_VIDEO)
        {
            return(eActionRenameVideo);
        }
        else
        if (strAction == XML_ACTION_DELETE_VIDEO)
        {
            return(eActionDeleteVideo);
        }
        else
        if (strAction == XML_ACTION_PLATFORM_NAME)
        {
            return(eActionPlatformName);
        }
        else
        if (strAction == XML_ACTION_PLATFORM_NAME_LONG)
        {
            return(eActionPlatformNameLong);
        }
        else
        if (strAction == XML_ACTION_PLATFORM_DESC)
        {
            return(eActionPlatformDesc);
        }
        else
        if (strAction == XML_ACTION_RECORD)
        {
            return(eActionRecord);
        }
        else
        if (strAction == XML_ACTION_SAVE)
        {
            return(eActionSave);
        }
        else
        if (strAction == XML_ACTION_VIDEO_TITLE)
        {
            return(eActionVideoTitle);
        }
        else
        if (strAction == XML_ACTION_VIDEO_DATE)
        {
            return(eActionVideoDate);
        }
        else
        if (strAction == XML_ACTION_VIDEO_FILENAME)
        {
            return(eActionVideoFilename);
        }
        else
        if (strAction == XML_ACTION_VIDEO_SIZE)
        {
            return(eActionVideoSize);
        }
        else
        if (strAction == XML_ACTION_VIDEO_LENGTH)
        {
            return(eActionVideoLength);
        }
        else
        if (strAction == XML_ACTION_VIDEO_INDEX)
        {
            return(eActionVideoIndex);
        }
        else
        if (strAction == XML_ACTION_VIDEO_VFORMAT)
        {
            return(eActionVideoVFormat);
        }
        else
        if (strAction == XML_ACTION_VIDEO_AFORMAT)
        {
            return(eActionVideoAFormat);
        }
        else
        if (strAction == XML_ACTION_VIDEO_VPID)
        {
            return(eActionVideoVPid);
        }
        else
        if (strAction == XML_ACTION_VIDEO_APID)
        {
            return(eActionVideoAPid);
        }
        else
        if (strAction == XML_ACTION_VIDEO_ENCRYPTION)
        {
            return(eActionVideoEncryption);
        }
        else
        if (strAction == XML_ACTION_AUDIO_PLAY)
        {
            return(eActionAudioPlay);
        }
        else
        if (strAction == XML_ACTION_AUDIO_PLAYALL)
        {
            return(eActionAudioPlayAll);
        }
        else
        if (strAction == XML_ACTION_AUDIO_STOP)
        {
            return(eActionAudioStop);
        }
        else
        if (strAction == XML_ACTION_TRANSCODE)
        {
            return(eActionTranscode);
        }
    }
    
    return(eActionInvalid);
}

