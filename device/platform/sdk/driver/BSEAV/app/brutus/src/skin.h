/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: skin.h $
 * $brcm_Revision: 19 $
 * $brcm_Date: 3/21/12 1:53p $
 *
 * Module Description: Parsing, conversion and storage of XML skin data
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/skin.h $
 * 
 * 19   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 *
 * 18   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/18/09 4:33p chengli
 * SW7550-64 : enable channel map screen even B_HAS_IP is not defined
 *
 * 17   5/11/09 7:41p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * PR53373_DeepColor/1   4/16/09 3:57p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 16   2/23/09 6:14p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 *
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:56p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 *
 * 15   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/3   8/21/08 4:06p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 *
 * BRUTUS_97405_97043/2   8/4/08 9:44a tokushig
 * PR43426: remove duplicate entry for showExtServerWin
 *
 * 14   8/20/08 5:49p lwhite
 * PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
 *
 * 13   7/30/08 8:09p shyam
 * PR43426 : Add dynamic toggling between transcode and decode
 *
 * 11   8/20/07 11:34a tokushig
 * PR23101: do not attempt to load skin xml file if option is not
 * specified
 *
 * 10   8/10/07 1:22p ssood
 * PR33786: Net DMA & IP Streamer refactoring: replace STREAMER_SUPPORT
 * variable by NETACCEL_SUPPORT
 *
 * 9   5/1/07 5:05p jrubio
 * PR30010: reuse the drm-nd skin+button
 *
 * 8   4/24/07 7:02p piyushg
 * PR28588: Add DRM-NDR blue skin support
 *
 * 7   4/24/07 12:52p jrubio
 * PR30010: added channel server screen for ip server
 *
 * 6   3/9/07 2:13p piyushg
 * PR28588: Add DRM-NDR support in brutus
 *
 * 5   8/25/06 1:26p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 *
 * SanDiego_Brutus_Skin/23   8/23/06 5:07p tokushig
 * added nothumb icon attribute for picturesscreen
 *
 * SanDiego_Brutus_Skin/22   8/17/06 5:05p tokushig
 * merge from main
 *
 * 4   6/6/06 11:48a tokushig
 * PR20349: ensure all dynamically allocated bwin framebuffers are
 * destroyed before deleting display
 *
 * SanDiego_Brutus_Skin/21   6/6/06 11:01a tokushig
 * clean up dynamic memory allocation in destructor
 *
 * 3   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/20   5/22/06 10:32a tokushig
 * add progress indicator image tag to xml
 *
 * SanDiego_Brutus_Skin/19   5/16/06 10:17a tokushig
 * Merge from main
 *
 * SanDiego_Brutus_Skin/18   4/27/06 11:05a tokushig
 * add border color for main window
 *
 * 1   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/17   3/13/06 5:26p tokushig
 * read pixel format as attribute in skin tag
 *
 * SanDiego_Brutus_Skin/16   2/28/06 1:27p tokushig
 * add dropshadow handling
 *
 * SanDiego_Brutus_Skin/15   2/23/06 10:06a tokushig
 * added button icon and focus text color attributes to skin
 *
 * SanDiego_Brutus_Skin/14   2/22/06 3:55p tokushig
 * added ability to modify pip window geometry in skin
 *
 * SanDiego_Brutus_Skin/12   2/16/06 11:46a piyushg
 * Add skin for the audio screen
 *
 * SanDiego_Brutus_Skin/12   2/16/06 11:37a piyushg
 * Add skin for the audio screen
 *
 * SanDiego_Brutus_Skin/9   2/14/06 5:28p tokushig
 * add skin attributes for info screen banner color
 *
 * SanDiego_Brutus_Skin/8   2/14/06 12:52p tokushig
 * added info screen button image attributes
 *
 * SanDiego_Brutus_Skin/7   2/13/06 4:43p tokushig
 * added new skin attribute values for info screens and record screen
 *
 * SanDiego_Brutus_Skin/6   2/8/06 1:14p tokushig
 * added tv screen define...not used yet
 *
 * SanDiego_Brutus_Skin/5   2/1/06 6:09p tokushig
 * changed xml screen names to match control class _screenname
 *
 * SanDiego_Brutus_Skin/4   2/1/06 3:22p tokushig
 * added support for dynamic label update for platform name and
 * description
 *
 * SanDiego_Brutus_Skin/3   1/26/06 9:57a tokushig
 * added ability to handle xml specified actions.
 * buffer used to read xml skin file now dynamically allocated/freed.
 *
 * SanDiego_Brutus_Skin/2   1/11/06 11:23a tokushig
 * add handling for pixmaps in addition to images.
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
 * SanDiego_BSEAVSW_Devel/1   12/09/05 1:54p tokushige
 * Implemented basic XML parsing, conversion, storage for XML skin data
 ***************************************************************************/
#ifndef SKIN_H__
#define SKIN_H__

#include "control.h"
#include "mhash.h"
#include "mxmlparser.h"
#include "mstringhash.h"
#include "genericscreen.h"

#define MAXBUF 1024*20

//xml tags
#define XML_TAG_BUTTON                      "button"
#define XML_TAG_IMAGE                       "img"
#define XML_TAG_LABEL                       "label"
#define XML_TAG_PIXMAP                      "pixmap"
#define XML_TAG_SCREEN                      "screen"
#define XML_TAG_SKIN                        "skin"

//xml attributes
#define XML_ATT_ACTION                      "action"
#define XML_ATT_ALIGN_H                     "halign"
#define XML_ATT_ALIGN_V                     "valign"
#define XML_ATT_BEVEL                       "bevel"
#define XML_ATT_BEVEL_WIDTH                 "bevelwidth"
#define XML_ATT_BEVEL_COLOR                 "bevelcolor"
#define XML_ATT_BEVEL_LIST_STYLE            "listbevelstyle"
#define XML_ATT_BEVEL_LIST_WIDTH            "listbevelwidth"
#define XML_ATT_BKGND_IMAGE                 "bkgndimage"
#define XML_ATT_BORDER                      "border"
#define XML_ATT_BORDER_COLOR                "bordercolor"
#define XML_ATT_COLOR                       "color"
#define XML_ATT_COLOR_TEXT                  "textcolor"
#define XML_ATT_COLOR_TEXT_FOCUS            "textcolorfocus"
#define XML_ATT_DROP_SHADOW                 "dropshadow"
#define XML_ATT_FOCUS                       "focus"
#define XML_ATT_FONT                        "font"
#define XML_ATT_GEOM_H                      "h"
#define XML_ATT_GEOM_W                      "w"
#define XML_ATT_GEOM_X                      "x"
#define XML_ATT_GEOM_Y                      "y"
#define XML_ATT_GEOM_LIST_H                 "listh"
#define XML_ATT_GEOM_LIST_W                 "listw"
#define XML_ATT_GEOM_LIST_X                 "listx"
#define XML_ATT_GEOM_LIST_Y                 "listy"
#define XML_ATT_GEOM_PIP_H                  "piph"
#define XML_ATT_GEOM_PIP_W                  "pipw"
#define XML_ATT_GEOM_PIP_X                  "pipx"
#define XML_ATT_GEOM_PIP_Y                  "pipy"
#define XML_ATT_GEOM_TV_H                   "tvh"
#define XML_ATT_GEOM_TV_W                   "tvw"
#define XML_ATT_GEOM_TV_X                   "tvx"
#define XML_ATT_GEOM_TV_Y                   "tvy"
#define XML_ATT_ICON_WIDTH                  "iconwidth"
#define XML_ATT_ICON_BORDER                 "iconborder"
#define XML_ATT_IMAGE                       "image"
#define XML_ATT_IMAGE_ON                    "imageOn"
#define XML_ATT_IMAGE_OFF                   "imageOff"
#define XML_ATT_IMAGE_DOWN                  "imageDown"
#define XML_ATT_IMAGE_ICON                  "imageIcon"
#define XML_ATT_INFO_BANNER_COLOR           "infobannercolor"
#define XML_ATT_INFO_BANNER_BEVEL_STYLE     "infobannerbevelstyle"
#define XML_ATT_INFO_BANNER_BEVEL_WIDTH     "infobannerbevelwidth"
#define XML_ATT_INFO_BEVEL_WIDTH            "infobevelwidth"
#define XML_ATT_INFO_BEVEL_STYLE            "infobevelstyle"
#define XML_ATT_INFO_BUTTON_IMAGE_ON        "infobuttonimageOn"
#define XML_ATT_INFO_BUTTON_IMAGE_OFF       "infobuttonimageOff"
#define XML_ATT_INFO_BUTTON_IMAGE_DOWN      "infobuttonimageDown"
#define XML_ATT_INFO_COLOR                  "infocolor"
#define XML_ATT_INFO_IMAGE                  "infoimage"
#define XML_ATT_INFO_IMAGE_FOCUS            "infoimagefocus"
#define XML_ATT_INFO_TEXT_COLOR             "infotextcolor"
#define XML_ATT_INFO_TEXT_COLOR_FOCUS       "infotextcolorfocus"
#define XML_ATT_LIST_COLUMNS                "listcolumns"
#define XML_ATT_LIST_DROP_SHADOW            "listdropshadow"
#define XML_ATT_LIST_FOCUS_IMAGE            "listfocusimage"
#define XML_ATT_LIST_ICON_NOTHUMB           "listnothumbimage"
#define XML_ATT_LIST_IMAGE                  "listimage"
#define XML_ATT_LIST_SCROLLBAR_IMAGE        "listscrollbarimage"
#define XML_ATT_LIST_SCROLLIND_IMAGE        "listscrollindimage"
#define XML_ATT_LIST_VISIBLE_ROWS           "listrows"
#define XML_ATT_LIST_TEXT_ALIGN             "listtextalign"
#define XML_ATT_LIST_TEXT_COLOR             "listtextcolor"
#define XML_ATT_LIST_TEXT_COLOR_FOCUS       "listtextcolorfocus"
#define XML_ATT_LIST_BUTTON_V_SPACING       "listbuttonvspacing"
#define XML_ATT_NAME                        "name"
#define XML_ATT_PIXEL_FORMAT                "pixelformat"
#define XML_ATT_PIXMAP                      "pixmap"
#define XML_ATT_PROGRESS_IND_IMAGE          "progressindimage"
#define XML_ATT_PVRBAR_COLOR                "pvrbarcolor"
#define XML_ATT_SCALE                       "scale"
#define XML_ATT_WRAP                        "wrap"

//xml attribute values
//XML_SCREEN_XXX names must match control::_screenname
#define XML_SCREEN_MENU                     "menu"
#define XML_SCREEN_PLAYBACK                 "playback"
#define XML_SCREEN_PICTURES                 "pictures"
#define XML_SCREEN_VIDEO                    "video"
#define XML_SCREEN_TV                       "tv"
#define XML_SCREEN_RECORD                   "record"
#define XML_SCREEN_AUDIO                    "audio"
#define XML_SCREEN_CHAN_MAP                 "chan_map"
#define XML_BEVEL_RAISED                    "raised"
#define XML_BEVEL_SUNKEN                    "sunken"
#define XML_BEVEL_UPDOWN                    "updown"
#define XML_SCALE_STRETCH                   "stretch"
#define XML_SCALE_SINGLE                    "single"
#define XML_SCALE_MAX                       "max"
#define XML_SCALE_DOWN                      "down"
#define XML_ALIGN_LEFT                      "left"
#define XML_ALIGN_RIGHT                     "right"
#define XML_ALIGN_CENTER                    "center"
#define XML_ALIGN_TOP                       "top"
#define XML_ALIGN_BOTTOM                    "bottom"
#define XML_WRAP_WORD                       "word"
#define XML_WRAP_NEWLINE                    "newline"
#define XML_WRAP_NONE                       "none"

#ifdef B_HAS_DRM_ND
#define XML_SCREEN_SERVER                   "extServer"
#endif
//xml actions (additions to this list must correspond to eSkinActions enum below
//as well as strAction2enum()
#define XML_ACTION_INVALID                  "showNone"
#define XML_ACTION_SHOW_ADMIN_WIN           "showAdminWin"
#define XML_ACTION_SHOW_AUDIO_WIN           "showAudioWin"
#define XML_ACTION_SHOW_INFO_WIN            "showInfoWin"
#define XML_ACTION_SHOW_MENU_WIN            "showMenuWin"
#define XML_ACTION_SHOW_PHOTO_WIN           "showPhotoWin"
#define XML_ACTION_SHOW_VIDEO_WIN           "showVideoWin"
#define XML_ACTION_SHOW_PLAYBACK_WIN        "showPlaybackWin"
#define XML_ACTION_SHOW_HDMI_WIN            "showHdmiWin"
#if defined  B_HAS_DRM_ND || defined B_HAS_IP
#define XML_ACTION_SHOW_EXTSERVER_WIN        "showExtServerWin"
#endif
#define XML_ACTION_SHOW_RECORD_WIN          "showRecordWin"
#define XML_ACTION_SHOW_CHAN_WIN            "showChanWin"
#define XML_ACTION_PLAY_VIDEO               "playVideo"
#define XML_ACTION_PLAY_VIDEO_TRANSCODE     "playVideoTranscode"
#define XML_ACTION_RESUME_VIDEO             "resumeVideo"
#define XML_ACTION_RENAME_VIDEO             "renameVideo"
#define XML_ACTION_DELETE_VIDEO             "deleteVideo"
#define XML_ACTION_PLATFORM_NAME            "platformName"
#define XML_ACTION_PLATFORM_NAME_LONG       "platformNameLong"
#define XML_ACTION_PLATFORM_DESC            "platformDesc"
#define XML_ACTION_RECORD                   "record"
#define XML_ACTION_SAVE                     "save"
#define XML_ACTION_VIDEO_TITLE              "videoTitle"
#define XML_ACTION_VIDEO_DATE               "videoDate"
#define XML_ACTION_VIDEO_FILENAME           "videoFilename"
#define XML_ACTION_VIDEO_SIZE               "videoSize"
#define XML_ACTION_VIDEO_LENGTH             "videoLength"
#define XML_ACTION_VIDEO_INDEX              "videoIndex"
#define XML_ACTION_VIDEO_VFORMAT            "videoVFormat"
#define XML_ACTION_VIDEO_AFORMAT            "videoAFormat"
#define XML_ACTION_VIDEO_VPID               "videoVPid"
#define XML_ACTION_VIDEO_APID               "videoAPid"
#define XML_ACTION_VIDEO_ENCRYPTION         "videoEncryption"
#define XML_ACTION_AUDIO_PLAY               "playAudio"
#define XML_ACTION_AUDIO_PLAYALL            "playAllAudio"
#define XML_ACTION_AUDIO_STOP               "stopAudio"
#define XML_ACTION_TRANSCODE                "transcode"

//changes to this enum must also be reflected in #defines above
//as well as strAction2enum()
typedef enum
{
    eActionInvalid,
    eActionShowAdminWin,
    eActionShowAudioWin,
    eActionShowInfoWin,
    eActionShowMenuWin,
    eActionShowPhotoWin,
    eActionShowVideoWin,
    eActionShowPlaybackWin,
    eActionShowHdmiWin,
#if defined B_HAS_DRM_ND || defined B_HAS_IP
    eActionShowExtServerWin,
#endif
    eActionShowRecordWin,
    eActionShowChanWin,
    eActionPlayVideo,
    eActionPlayVideoTranscode,
    eActionResumeVideo,
    eActionTranscodePlaybackVideo,
    eActionRenameVideo,
    eActionDeleteVideo,
    eActionPlatformName,
    eActionPlatformNameLong,
    eActionPlatformDesc,
    eActionRecord,
    eActionSave,
    eActionVideoTitle,
    eActionVideoDate,
    eActionVideoFilename,
    eActionVideoSize,
    eActionVideoLength,
    eActionVideoIndex,
    eActionVideoVFormat,
    eActionVideoAFormat,
    eActionVideoVPid,
    eActionVideoAPid,
    eActionVideoEncryption,
    eActionAudioPlay,
    eActionAudioPlayAll,
    eActionAudioStop,
    eActionTranscode
} eSkinAction;

//english color definitions
#define XML_COLOR_RED                       "red"
#define XML_COLOR_GREEN                     "green"
#define XML_COLOR_BLUE                      "blue"
#define XML_COLOR_MAGENTA                   "magenta"
#define XML_COLOR_CYAN                      "cyan"
#define XML_COLOR_YELLOW                    "yellow"
#define XML_COLOR_BLACK                     "black"
#define XML_COLOR_WHITE                     "white"

//errors
#define ERROR_TEXT_NOT_AVAIL                "Text Not Available"


class Control;

class Skin
{
public:
    Skin(Control * acontrol);
    ~Skin();

    void      initialize();
    MPixmap * getPixmap(const char * strName);
    MImage  * getImage(const char * strName);
    void      parse(const char * strFilename);
    void      createImagesPixmaps();
    void      createWidgets(const char * strScreenName, GenericScreen * screen);
    void      destroyWidgets(const char * strScreenName);
    bool      isValid(const char * strScreen = NULL);
    void      setControl(Control * c) { control = c; }

    inline MString getName() { return strName; };
    inline int32_t getBorderColor() { return nBorderColor; };

    int32_t getColorValue(const char * strColor);
    int32_t changeAlpha(int32_t nColor, unsigned char byte);

    MRect getPIPWindowSize(const char        * strScreenName,
                           bdisplay_settings * displaySettings);
    MRect getTVWindowSize(const char        * strScreenName,
                          bdisplay_settings * displaySettings);

    MWidget * getWidget(const char * strName);

    void getSkinAttrValue(const char * strName,
                          MString & strValue);
    void getSkinAttrValue(const char * strName,
                          uint32_t & uValue);
    void getAttrValue(const char * strScreen,
                      const char * strName,
                      MString & strValue);
    void getAttrValue(const char * strScreen,
                      const char * strName,
                      uint32_t & uValue);

    void processAction(MWidget * widget, int action);
    void processAction(MWidget * widget);
    void processActionLabel(const char * strScreen, eSkinAction action);

protected:
    void getAttrValue(MXmlElement * elem, const char * strName, MString & strValue);
    void getAttrValue(MXmlElement * elem, const char * strName, uint32_t & uValue);
    void getTagValue(MXmlElement * elem, MString & strValue);

    MXmlElement * findTagAttrValue(const char * strTag,
                                   const char * strAttr,
                                   const char * strAttrValue,
                                   MXmlElement * elem);

    void createImagesR(MXmlElement * elem);
    void createWidgetsR(MXmlElement * elem, GenericScreen * screen);
    void processActionLabelR(MXmlElement * elem, eSkinAction action);

    eSkinAction strAction2enum(const char * strAction);

    MXmlElement           *  xmlElemTop;
    MXmlElement           *  xmlElemSkin;
    MHash<MPixmap>           mapPixmaps;
    MHash<MImage>            mapImages;
    MHash<MWidget>           mapWidgets;
    MHash<MXmlElement>       mapScreenElem;
    MString                  strName;
    int32_t                  nBorderColor;
    Control               *  control;
    bool                     bValid;
};

#endif

