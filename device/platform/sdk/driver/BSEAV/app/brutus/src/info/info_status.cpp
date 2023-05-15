/***************************************************************************
 *     (c)2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: info_status.cpp $
 * $brcm_Revision: 40 $
 * $brcm_Date: 10/25/12 2:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: X:/win_views/97231/BSEAV/app/brutus/src/info/info_status.cpp $
 * 
 * 40   10/25/12 2:18p vishk
 * SW3461-206: Fix signal strength to set it as units of dBm for Ofdm.
 * 
 * 39   9/7/12 11:51a erickson
 * SW7435-349: add standard header
 * 
 * 38   5/18/12 8:02p bselva
 * SW7360-19: Add support for 7360 platform
 * 
 * 37   3/7/12 3:23p vishk
 * SW7552-183: Update postRsBer correctly.
 * 
 * 37   3/7/12 1:05p vishk
 * SW7552-183: Update postRsBer correctly.
 * 
 * 36   1/5/12 3:49p xhuang
 * SW7552-156: Add signal strength menu
 * 
 * 35   1/2/12 2:57p erickson
 * SW7358-88: refactor BCHP_CHIP list to feature macro
 * 
 * 34   8/3/11 5:43p bselva
 * SW7358-88: Added 7358 chip to display the input power
 * 
 * 33   4/27/11 4:43p jrubio
 * SW7346-158: fix frontpanel for 7346
 * 
 * 32   1/27/11 11:30a mward
 * SW7125-726:  Add QAM Annex C.
 * 
 * 31   1/21/11 5:35p nitinb
 * SW7550-526: Merge to main branch
 * 
 * SW7550-526/1   8/18/10 10:39a pinglong
 * SW7550-526: SNR on Brutus status page is incorrect for OFDM
 * 
 * 30   6/7/10 9:35a erickson
 * SW7550-236: merge
 * 
 * SW7550-236/3   5/21/10 3:06p pinglong
 * SW7550-236: move the postRsBer calculation to info_status.cpp
 * 
 * SW7550-236/2   4/16/10 8:11p pinglong
 * SW7550-236: It's not right for some item for status page of brutus
 * 
 * 29   8/10/09 1:54p jrubio
 * PR55232: add 7340/7342
 * 
 * 28   2/4/09 10:30a jrubio
 * PR51629: add 7336 support
 * 
 * 27   1/10/09 5:00p agin
 * PR50937: Fixed wording of correctable/uncorrectable blocks.
 * 
 * 26   12/18/08 9:47p agin
 * PR49983: Incorrect channel status reported for power, BER, SNR for
 * 97335 AST.
 * 
 * 25   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 * 
 * 24   2/26/08 7:33a jrubio
 * PR39363: fix compile warnings
 * 
 * 23   2/1/08 4:59p katrep
 * PR39180: Added support for ofdm/DVB-T THD tuner
 * 
 * 22   9/4/07 11:23a yrajakar
 * PR34578: Adding support for RTP FEC
 * 
 * 21   4/19/07 6:45p jrubio
 * PR30010: added tcp fields where needed
 * 
 * 20   3/26/07 1:48a erickson
 * PR28741: fix PVR_SUPPORT=n support
 * 
 * 19   2/27/07 7:10p erickson
 * PR28005: allow status even if no channel in channelmap
 * 
 * 18   1/29/07 11:52a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 18   1/29/07 11:18a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 * 
 * 17   1/26/07 11:12a erickson
 * PR27149: info panel displays frontend status for timeshifting playback.
 * Control extended to lookup AVStream.
 * 
 * 16   12/7/06 5:56p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 * 
 * 15   9/29/06 11:32a arbisman
 * PR24289: Back-out changes
 * 
 * 12   7/25/06 5:23p jjordan
 * PR22906: Integrate liveMedia open-source for RTP and RTCP support
 * 
 * 11   6/19/06 3:47p erickson
 * PR19526: added ChannelMapEntry::setType to force initialization of
 * Settop API frontend params structures when type changes
 * 
 * 10   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/6   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 9   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/5   4/14/06 8:04p tokushig
 * moved info screen location to avoid overscan cutoff
 * 
 * SanDiego_Brutus_Skin/4   3/30/06 10:52a tokushig
 * remove prerendering of icon if magnum based.  since we are using the
 * blitter to display pixmaps, any pixmap with alpha values cannot be
 * displayed properly.  legacy platforms will operate as usual
 * 
 * SanDiego_Brutus_Skin/3   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 * 
 * SanDiego_Brutus_Skin/2   2/15/06 10:50a tokushig
 * add bevel style for mlabel to info skin
 * 
 * SanDiego_Brutus_Skin/1   2/13/06 4:37p tokushig
 * added partial skin ability.  note that this is primarily for cable show
 * demo purposes and will be expanded to be fully skinnable in the
 * future.
 * 
 * 7   8/12/05 10:42a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 * 
 * 6   5/23/05 3:44p dlwin
 * PR 15503: Updated display of VSB information based on recommendation by
 * HW folks for Corrected/Uncorrected counters.
 * 
 * 5   5/20/05 9:15a dlwin
 * PR 15503: Added IF Offset information for VSB.
 * 
 * 4   5/20/05 8:53a dlwin
 * PR 15503: Add support getting and displaying addition status
 * information for VSB frontend
 * 
 * 3   4/18/05 10:56a erickson
 * PR14593: prerender images for faster draw time on slow platforms
 * 
 * 2   3/28/05 4:58p erickson
 * PR11759: added multiple linein support
 * 
 * 1   2/7/05 8:10p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/38   2/4/05 10:48a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 * 
 * Irvine_BSEAVSW_Devel/37   11/8/04 2:54p erickson
 * PR12857: if no tuner, we still need to update the fields
 * 
 * Irvine_BSEAVSW_Devel/36   11/5/04 9:57a erickson
 * PR12988: added analog present field
 * 
 * Irvine_BSEAVSW_Devel/35   11/4/04 2:30p erickson
 * PR13169: Fix units on AGC levels
 * 
 * Irvine_BSEAVSW_Devel/34   10/27/04 12:22p erickson
 * PR12857: only frontend panels require current channel too
 * 
 * Irvine_BSEAVSW_Devel/33   10/21/04 4:51p erickson
 * PR12857: only frontend stats require tuner
 * 
 * Irvine_BSEAVSW_Devel/32   10/15/04 12:03p erickson
 * PR12857: updated AGC label
 * 
 * Irvine_BSEAVSW_Devel/31   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 * Irvine_BSEAVSW_Devel/30   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/29   8/9/04 3:48p erickson
 * PR12104: extended SDS to handle DSS_PES and DSS_ES, along with STREAMER
 * and INPUTBAND
 *
 * Irvine_BSEAVSW_Devel/28   6/30/04 11:34a erickson
 * PR11135: need to clear FEC cor/uncorr when changing channel, toggling
 * windows
 *
 * Irvine_BSEAVSW_Devel/27   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/26   3/23/04 5:36p erickson
 * PR9465: added VSB status to brutus info panel
 *
 * Irvine_BSEAVSW_Devel/25   2/17/04 2:28p erickson
 * PR9465: sds and vsb on 7038 need more delay, but qam doesn't. so it
 * needs to be dynamic.
 *
 * Irvine_BSEAVSW_Devel/24   2/13/04 12:05p erickson
 * PR9465: updated channelmgr naming and comments
 * 
 * Irvine_BSEAVSW_Devel/23   2/12/04 7:34p erickson
 * PR9465: reworked ChannelManager to support multiple tuners per
 * downstream
 * 
 * Irvine_BSEAVSW_Devel/22   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/21   12/10/03 10:26p erickson
 * PR8813: no need to hack in QAM1024 values. If you get lock but don't
 * have channels, you can always program in some bogus pids using PROGRAM
 * PIDS
 *
 * Irvine_BSEAVSW_Devel/20   12/4/03 9:39a erickson
 * PR8813: complete rework of info UI to support vertical bar
 *
 ****************************************************************************/
#include "bsettop.h"
#include "channelmgr.h"
#include "control.h"
#include "cfgsettings.h"

#include "info_status.h"
#include "info_constellation.h"
#include "info_decodepage.h"
#include "info_playbackbuffer.h"

#include "bstd.h"
BDBG_MODULE(info_status);

#define MEGA_F 1000000.0

static char *g_SdsSpecInv[] = {
    (char *) "Scan", // input only
    (char *) "I Inv",
    (char *) "Q Inv",
    (char *) "Normal"
    };

#define PIXMAPPATH (MString(infoBar->control()->cfg()->get(Config::IMAGES_PATH)) + "/info/")
#define PANELRECT MRect(230,25,350,350)

class FrontEndPage : public InfoPanelStatusPage {
public:
    FrontEndPage(InfoPanel *parent, const MRect &rect);
    void set(btuner_t tuner);
    void update();

protected:
    btuner_stream_type _type;
    btuner_t _tuner;
    void setPageType(btuner_stream_type type);
};

FrontEndPage::FrontEndPage(InfoPanel *parent, const MRect &rect) :
    InfoPanelStatusPage(parent, rect)
{
    _type = btuner_sds_stream; // force the refresh
    setPageType(btuner_qam_stream);
}

void FrontEndPage::setPageType(btuner_stream_type type)
{
    if ((type == _type))
        return;
    _type = type;

    clearLabels();
    switch (_type) {
    case btuner_sds_stream:
        addLabel(0, "Freq:");
        addLabel(2, "Mode:");
        addLabel(3, "CodeRate:");
        addLabel(1, "SpecInv:");
        addLabel(5, "SymRate:");
        addLabel(6, "CarrErr:");
        addLabel(8, "SNR:");
        addLabel(10, "InputPower:");
        addLabel(13, "FEC corrected:");
        addLabel(14, "FEC uncorrected:");
        addLabel(21, "FEC clean:");
        addLabel(15, "Locked:");
        addLabel(16, "Unlocked:");
        addLabel(20, "Channel:");
        break;
    case btuner_analog_stream:
        addLabel(0, "Freq:");
        addLabel(1, "Source:");
        addLabel(2, "Format:");
        addLabel(3, "Analog Present:");
        break;
    case btuner_qam_stream:
        addLabel(0, "Freq:");
        addLabel(2, "Mode:");
        addLabel(3, "QAMAnnex:");
        addLabel(4, "FECLock:");
        addLabel(5, "QAMLock:");
        addLabel(6, "SymRate:");
        addLabel(7, "SNR:");
        addLabel(8, "AGC:");
        addLabel(9, "BER:");
        addLabel(10, "FrqErr:");
        addLabel(11, "PhaseErr:");
        addLabel(12, "FEC_cor:");
        addLabel(13, "FEC_uncor:");
        addLabel(14, "signal strength:");		
        break;
    case btuner_vsb_stream:
        addLabel(0, "Freq:");
        addLabel(1, "Mode:");
        addLabel(2, "Locked:");
        addLabel(3, "Unlocked:");
        addLabel(4, "SymRate:");
        addLabel(5, "IF:");
        addLabel(6, "SNR:");
        addLabel(7, "AGC:");
        addLabel(8, "IF Error:");
        addLabel(9, "SymErr:");
        addLabel(10, "Bit_cor:");
        addLabel(11, "FEC_uncor:");
        break;
    case btuner_ofdm_stream:
        addLabel(0, "Freq:");
        addLabel(1, "Mode:");
        addLabel(2, "Bandwidth:");
        addLabel(3, "Priority:");
        addLabel(4, "CCI:");
        addLabel(5, "SNR:");
        addLabel(6, "RS_cor:");
        addLabel(7, "RS_uncor:");
        addLabel(8, "Acq count:");
        addLabel(9, "Locked:");
        addLabel(10, "Unlocked:");
        addLabel(11, "signal strength:");		
        break;
    case btuner_ip_stream:
        addLabel(0, "IP Address:");
        addLabel(1, "Protocol:");
        addLabel(2, "Port:");
        break;
    default:
        addLabel(0, "No Status");
    }
}

#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360)
#define BCM73XX_INPUT_POWER 1
#endif

#if BCM73XX_INPUT_POWER
void bcm73xx_get_input_power(uint32_t rfagc, 
                             uint32_t ifagc, 
                             uint32_t tuner_freq, 
                             uint32_t int_config,
                             uint32_t ext_config,
                             uint32_t version,
                             uint32_t agc0,
                             uint32_t agc1,
                             uint32_t channel,
                             float *pPower)
{
#define LDAIT_THRESH (0xFFFF0000)

   static float lna_gain_0[64] =
   {
      (float)18.52128105,
      (float)18.49890976,
      (float)18.47153549,
      (float)18.4376667,
      (float)18.3860424,
      (float)18.32747536,
      (float)18.19055252,
      (float)18.10708694,
      (float)17.99657326,
      (float)17.86715584,
      (float)17.69694214,
      (float)17.51351242,
      (float)17.32871323,
      (float)17.05899315,
      (float)16.78234177,
      (float)16.51082687,
      (float)16.2005928,
      (float)15.90700226,
      (float)15.58021622,
      (float)15.25473671,
      (float)14.91493111,
      (float)14.58075218,
      (float)14.24549179,
      (float)13.86191254,
      (float)13.5504158,
      (float)13.19491653,
      (float)12.85526352,
      (float)12.51637344,
      (float)12.15071564,
      (float)11.81063919,
      (float)11.40886383,
      (float)10.9570549,
      (float)10.48106079,
      (float)10.0015686,
      (float)9.450173187,
      (float)8.874081421,
      (float)8.24601059,
      (float)7.575627136,
      (float)6.888356018,
      (float)6.152416992,
      (float)5.438557434,
      (float)4.696274567,
      (float)3.989422607,
      (float)3.269135284,
      (float)2.580582428,
      (float)1.938179779,
      (float)1.344006348,
      (float)0.780266571,
      (float)0.262810516,
      (float)-0.24341507,
      (float)-0.708350372,
      (float)-1.180190277,
      (float)-1.629843903,
      (float)-2.059775543,
      (float)-2.582392883,
      (float)-3.020091248,
      (float)-3.449237061,
      (float)-3.8992836,
      (float)-4.349417877,
      (float)-4.769541931,
      (float)-5.192805481,
      (float)-5.668547821,
      (float)-6.070815277,
      (float)-6.523020935
   };

   static float lna_gain_1[64] =
   {
      (float)18.50371628,
      (float)18.43415909,
      (float)18.34189873,
      (float)18.25750237,
      (float)18.11298065,
      (float)17.97500877,
      (float)17.82687073,
      (float)17.58118515,
      (float)17.36043053,
      (float)17.12426262,
      (float)16.81940346,
      (float)16.51476936,
      (float)16.23875504,
      (float)15.90388947,
      (float)15.56721001,
      (float)15.26126747,
      (float)14.93567162,
      (float)14.57109718,
      (float)14.23564224,
      (float)13.87808304,
      (float)13.57791977,
      (float)13.19830017,
      (float)12.88682632,
      (float)12.46267776,
      (float)12.07420807,
      (float)11.62433701,
      (float)11.1628006,
      (float)10.66158752,
      (float)10.11655502,
      (float)9.551731873,
      (float)8.938088226,
      (float)8.330128479,
      (float)7.663777161,
      (float)6.931649017,
      (float)6.228138733,
      (float)5.532807159,
      (float)4.749226379,
      (float)3.96424942,
      (float)3.328621674,
      (float)2.596588898,
      (float)1.997566986,
      (float)1.3883255,
      (float)0.85374527,
      (float)0.389157104,
      (float)-0.104338837,
      (float)-0.510932159,
      (float)-0.984130096,
      (float)-1.472476196,
      (float)-1.896323395,
      (float)-2.363707733,
      (float)-2.7936203,
      (float)-3.218924713,
      (float)-3.702536774,
      (float)-4.14865036,
      (float)-4.584147644,
      (float)-5.000064087,
      (float)-5.442527008,
      (float)-5.941153717,
      (float)-6.371207428,
      (float)-6.81473465,
      (float)-7.220576477,
      (float)-7.665530396,
      (float)-8.154948425,
      (float)-8.764987183
   };

   static float POWER_COEF_LP[4] =
   {
      (float)(-0.00071424),
      (float)(-20.5163), 
      (float)(0.0057254),
      (float)(-47.5023)
   };
   static float POWER_COEF_MP[4] =
   {
      (float)(-0.0014882),
      (float)(-26.2103),
      (float)(0.0045247),
      (float)(-34.3448)
   };
   static float POWER_COEF_HP[4] =
   {
      (float)(0.00040586),
      (float)(-32.6998),
      (float)(0.0039915),
      (float)(-11.6626)
   };
    
   float t, b, x, m, *pCoef, *pGain;
   uint32_t gain;
   uint8_t lna_agc;

   {
      if ((version != 0) && (ext_config != 0x0F) && (int_config != 0x0F))
      {
         /* BCM3445 was detected and it is controlled by BCM73XX */
         if (channel)
         {
            /* SDS1 */
            lna_agc = agc1;
            pGain = lna_gain_1;
         }
         else
         {
            /* SDS0 */
            lna_agc = agc0;
            pGain = lna_gain_0;
         }

         if (lna_agc)
         {
            /* compute input power from BCM3445 */
            *pPower = (float)(-15.0 - pGain[lna_agc] - (10.0 * lna_agc / 64.0));
            return;
         }
      }
   }

   /* compute input power from BCM73XX */
   gain = ifagc;
   if (rfagc == LDAIT_THRESH)
   {
      pCoef = POWER_COEF_LP; /* low power */
   }
   else if (rfagc == 0)
   {
      pCoef = POWER_COEF_HP; /* high power */
   }
   else
   {
      pCoef = POWER_COEF_MP; /* mid power */
      gain = rfagc; 
   }

   gain = gain >> 8;
   x = (float)((float)gain / 16777216.0);
   t = (float)((float)(tuner_freq - 950000000) / 1000000.0);
   m = (pCoef[0] * t) + pCoef[1];
   b = (pCoef[2] * t) + pCoef[3];
   *pPower =  b + (x * m);
}
#endif

int int_log10(float f) {
    int log = 0;
    if (f <= 0) {
        return 1; /* special value */
    }
    else if (f < 1.0) {
        while (f < 1.0) {
            f *= 10.0;
            log--;
        }
    }
    else {
        int n = (int)f;
        while (n >= 10) {
            n /= 10;
            log++;
        }
    }
    return log;
}

void FrontEndPage::update()
{
    ChannelMap::Channel channel;
    
    /* NOTE: If you don't have any program information, but want to see frontend
    status, just add in a PROGRAM PIDS line to fake out the channel scan.
    Unfortunately, at the present time, this isn't even going to be called because
    to TODO later on in this file. */

#if PVR_SUPPORT     
    PlayEngine *pvr = control()->playEngine();
    if (pvr) {
        AVStream *timeshift_stream = control()->getLiveStream(pvr);
        if (!timeshift_stream) {
            setPageType(btuner_streamer_stream); // no frontend status if doing playback
            return;
        }
        /* get the channel that is being recorded */
        if (control()->channelMap()->getChannel(timeshift_stream->ch(), &channel))
            return;
    }
    else 
#endif
    {
        if (control()->channelMap()->getChannel(control()->channel(), &channel))
            return;
    }
    
    btuner_stream_type type = channel.entry.getType();
    setPageType(type);

    switch (type) {
    case btuner_sds_stream:
        {
        btuner_sds_status status;
        
        /* don't depend on status */
        setLabelValue(0, "%0.2f MHz", channel.entry.freq);
        setLabelValue(2, "%s", btuner_sds_str(channel.entry.sds.params.mode,
            channel.entry.sds.mpeg_type));
        setLabelValue(5, "%0.2f Msps", channel.entry.sds.params.symbol_rate / MEGA_F);

        /* require sds to be locked */
        if (!_tuner || btuner_get_sds_status(_tuner, &status))
            return;
        setLabelValue(3, "%d/%d", status.coderate.num, status.coderate.denom);
        setLabelValue(1, "%s", g_SdsSpecInv[status.spectral_inversion]);
        setLabelValue(6, "%d Hz", status.carrier_error);
        setLabelValue(8, "%0.2f dB", status.snr_estimate/256.0);
        if (status.ber_estimate < 1)
            setLabelValue(9, "10 ^ %d", status.ber_estimate);
        else
            setLabelValue(9, "N/A");
        setLabelValue(20, "SDS%d", status.channel);
        #if BCM73XX_INPUT_POWER
        {
            float power;
            bcm73xx_get_input_power(status.rf_agc, status.if_agc, 
                                 status.tuner_freq, 
                                 status.lnaStatus.int_config,
                                 status.lnaStatus.ext_config,
                                 status.lnaStatus.version,
                                 status.lnaStatus.agc0,
                                 status.lnaStatus.agc1,
                                 status.channel,
                                 &power);
            setLabelValue(10, "%0.2f dBm", power);
        }
        #endif
        setLabelValue(13, "%d", status.fec_corr);
        setLabelValue(14, "%d", status.fec_uncorr);
        setLabelValue(21, "%d", status.fec_clean);
        setLabelValue(15, "%s%s%s",
            status.rs_locked?"FEC ":"",
             status.vit_locked?"PHY ":"",
            status.bert_locked?"BERT":"");
        setLabelValue(16, "%s%s%s",
            !status.rs_locked?"FEC ":"",
            !status.vit_locked?"PHY ":"",
            !status.bert_locked?"BERT":"");            
        }
        break;

    case btuner_qam_stream:
        {
        btuner_qam_status status;
        float postRsBer = 0;
        if (!_tuner || btuner_get_qam_status(_tuner, &status))
            return;
        setLabelValue(0, "%0.2f MHz", channel.entry.freq);
        setLabelValue(2, "%s", btuner_qam_str(channel.entry.qam.mode));
        setLabelValue(3, btuner_qam_annex_str(channel.entry.qam.annex));
        setLabelValue(4, "%c", status.fec_lock?'Y':'N');
        setLabelValue(5, "%c", status.qam_lock?'Y':'N');
        setLabelValue(6, "%f", status.symbol_rate / MEGA_F);
        setLabelValue(7, "%0.2f dB", status.snr_estimate / 256.0);
        setLabelValue(8, "IF=%0.1f%% RF=%0.1f%%",
            status.int_agc_level/10.0, status.ext_agc_level/10.0);
        setLabelValue(14, "%0.2f dB", status.dsChannelPower / 256.0);
        postRsBer = status.postRsBer;
        if(postRsBer > 0)
            setLabelValue(9, "10 ^ %d", int_log10(postRsBer));
        else
            setLabelValue(9, "0");

        setLabelValue(10, "%0.2f KHz", status.freq_err / 1000.0);
        setLabelValue(11, "%0.2f KHz", status.freq_er2 / 1000.0);
        setLabelValue(12, "%d", status.fec_corr_cnt);
        setLabelValue(13, "%d", status.fec_uncorr_cnt);
        }
        break;

    case btuner_vsb_stream:
        {
        btuner_vsb_status status;
        if (!_tuner || btuner_get_vsb_status(_tuner, &status))
            return;
        setLabelValue(0, "%0.2f MHz", channel.entry.freq);
        setLabelValue(1, "%s", btuner_vsb_str(channel.entry.vsb.mode));
        setLabelValue(2, "%s%s%s",
            status.receiver_lock?"RCV ":"",
            status.fec_lock?"FEC ":"",
            status.opll_lock?"PLL":"");
        setLabelValue(3, "%s%s%s",
            !status.receiver_lock?"RCV ":"",
            !status.fec_lock?"FEC ":"",
            !status.opll_lock?"PLL":"");
        setLabelValue(4, "%f", status.symbol_rate / MEGA_F);
        setLabelValue(5, "%f", status.if_freq / MEGA_F);
        setLabelValue(6, "%0.2f dB", status.snr_estimate / 256.0);
        setLabelValue(7, "IF=%0.1f%% RF=%0.1f%%", 
            status.int_agc_level/10.0, status.ext_agc_level/10.0);
        setLabelValue(8, "%0.2f KHz", status.if_freq_error / 1000.0 );
        setLabelValue(9, "%d Hz", status.symbol_rate_error );
        setLabelValue(10, "%d", status.err_corr );
        setLabelValue(11, "%d", status.blk_uncorr );
        }
        break;

    case btuner_ofdm_stream:
        {
        btuner_ofdm_status status;
        if (!_tuner || btuner_get_ofdm_status(_tuner, &status))
            return;
        setLabelValue(0, "%0.2f MHz", channel.entry.freq);
        setLabelValue(1, "%s", btuner_ofdm_str());
        setLabelValue(2, "%d Mhz", channel.entry.ofdm.bandwidth);
        setLabelValue(3, "%s", channel.entry.ofdm.priority==btuner_ofdm_priority_high?"High":"Low");
        setLabelValue(4, "%s", channel.entry.ofdm.cci_mode==btuner_ofdm_cci_mode_none?"Off":"Auto");
        setLabelValue(5, "%d dB", (int)status.snr/100);
        setLabelValue(6, "%d ", status.blk_corr);
        setLabelValue(7, "%d ", status.blk_uncorr);
        setLabelValue(8, "%d ", status.acq_count);
        setLabelValue(9, "%s%s",status.receiver_lock?"RCV ":"",status.fec_lock?"FEC ":"");
        setLabelValue(10, "%s%s",!status.receiver_lock?"RCV ":"",!status.fec_lock?"FEC ":"");
        setLabelValue(11, "%d dBm", (int)(status.signal_strength/10) - 49);		
        }
        break;

    case btuner_analog_stream:
        {
        btuner_analog_status status;
        
        if (!_tuner || btuner_get_analog_status(_tuner, &status))
            return;
        
        if (channel.entry.analog.linein >= 0)
            setLabelValue(0, "");
        else
            setLabelValue(0, "%0.2f MHz", channel.entry.freq);
        setLabelValue(1, "%s", btuner_analog_str(&channel.entry));
        setLabelValue(2, "%s", btuner_analog_format_str(channel.entry.analog.params.video_format));
        setLabelValue(3, "%s", status.video_present?"yes":"no");
        }
        break;

    case btuner_ip_stream:
        setLabelValue(0, "%s", channel.entry.ip.params.open_params.ip_addr);
        setLabelValue(1, "%s",  (channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_http) ? "HTTP" :
                                (channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtp)  ? "RTP"  :
                                (channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_udp)  ? "UDP"  :
                                (channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtp_fec)  ? "FEC"  :
                                (channel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp) ? "RTSP" : 
                      "Unknown");
        setLabelValue(2, "%d", channel.entry.ip.params.open_params.port);
        break;
    default:
        // no frontend
        BDBG_ERR(("Unsupported frontend"));
        break;
    }
}

void FrontEndPage::set(btuner_t t) {
    _tuner = t;
}

///////////////////////////////////

StatusInfoPanel::StatusInfoPanel(InfoBar *infoBar, const MRect &rect) :
    InfoPanel(infoBar, rect)
{
    control()->addControlEventHandler(this);

    _const = new Constellation(this);
    _const->setGeometry(infoClientRect());
    _const->hide();
// TODO: add back in IP STB equivalent of constellation later (eg network jitter, packet loss, etc)
    addPage(_const, "Const", 30);

    _status = new FrontEndPage(this, infoClientRect());
    _status->hide();
    addPage(_status, "Frontend", 2000);

    _bars = new PlaybackBufferPanel(this, infoClientRect());
    _bars->hide();
    addPage(_bars, "Buffers", 333);

    _decode = new DecodePage(this, infoClientRect());
    _decode->hide();
    addPage(_decode, "PIDs", 2000);

    Skin * pSkin = infoBar->control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        //get list box attributes from skin
        MString         screenColor;
        MString         imgFocus;
        MString         textColor;
        MString         textColorFocus;
        MString         bevelStyle;
        uint32_t        bevelWidth       = 0;

        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_COLOR,              screenColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_IMAGE_FOCUS,        imgFocus);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR,         textColor);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_TEXT_COLOR_FOCUS,   textColorFocus);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_WIDTH,        bevelWidth);
        pSkin->getAttrValue(XML_SCREEN_TV, 
                            XML_ATT_INFO_BEVEL_STYLE,        bevelStyle);

        //set panel background color if necessary
        if (!screenColor.isEmpty())
        {
            setBackgroundColor(pSkin->getColorValue(screenColor.s()));
        }
 
        //set focus image if necessary
        if (!imgFocus.isEmpty())
        {
            //setImageFocus(pSkin->getImage(imgFocus.s()));
        }

        //set text color if necessary
        if (!textColor.isEmpty())
        {
            setTextColor(pSkin->getColorValue(textColor.s()));
            _status->setTextColor(pSkin->getColorValue(textColor.s()));
            _bars->setTextColor(pSkin->getColorValue(textColor.s()));
            _decode->setTextColor(pSkin->getColorValue(textColor.s()));
        }
 
        //set focus text color if necessary
        if (!textColorFocus.isEmpty())
        {
            //setTextColorFocus(pSkin->getColorValue(textColorFocus.s()));
        }
 
        //set bevel if necessary
        if ((bevelWidth > 0) && (!bevelStyle.isEmpty()))
        {
            MPainter::BevelStyle bevelType = MPainter::bsRaised;
            if (bevelStyle == XML_BEVEL_SUNKEN)
            {
                bevelType = MPainter::bsSunken;
            }
            if (bevelStyle == XML_BEVEL_UPDOWN)
            {
                bevelType = MPainter::bsUpDown;
            }

            setBevel(bevelWidth, bevelType);
        }
    }
}

StatusInfoPanel::~StatusInfoPanel()
{
    control()->removeControlEventHandler(this);
}

void StatusInfoPanel::onControlEvent(Control *control, Control::Event event)
{
    switch (event) {
    case Control::evChangeChannel:
    case Control::evMoveWindowControl:
        _const->repaint();
        update();
        break;
    default: break;
    }
}

void StatusInfoPanel::update()
{
    ChannelMap::Channel channel;
    btuner_t tuner;
    int timer = 30;
    bool has_channel = false;
    
    /* TODO: brutus's AVManager only returns an AVStream if you can lock (i.e. acquire).
    Therefore you can't watch frontend status unless you lock. This is fine for
    an app, but not good for frontend debug. */
    tuner = control()->tuner();
    
    if (tuner && !control()->channelMap()->getChannel(control()->channel(), &channel)) {
        // has_channel means the channel variable is valid
        has_channel = true;
    }
    
    // TODO: just because the current channel is IP doesn't mean we're not doing HDD playback. this should be fixed.
    if (has_channel && channel.entry.getType() == btuner_ip_stream)
    {
        switch (_currentPage) {
        case 0:
            // TODO: add back in IP STB equivalent of constellation later (eg network jitter, packet loss, etc)
            _status->set(tuner);
            _status->update();
            break;
        case 1:
            _bars->update();
            break;
        case 2:
            _decode->update();
            break;
        }
    }
    else
    {
        switch (_currentPage) {
        case 0:
            if (has_channel) {
                /* Set the page time based on the type of tuner for some platforms.
                Specifically, any tuner which is controlled via I2C cannot be polled
                as frequently because the UI will sieze up. This code can be removed
                when all settop api tuner calls are made from a separate thread. */
#if defined BCM7319_FLAVOR
                timer = 2000;
#elif BCHP_CHIP == 7038
                if (channel.entry.getType() == btuner_sds_stream)
                    timer = 300;
                if (channel.entry.getType() == btuner_vsb_stream)
                    timer = 300;
#endif
                BDBG_MSG(("Setting page time to %d", timer));
                setPageTimer(_currentPage, timer);
                
                if (tuner) {
                    _const->set(tuner);
                    _const->update();
                }
            }
            break;
        case 1:
            _status->set(tuner);
            _status->update();
            break;
        case 2:
            _bars->update();
            break;
        case 3:
            _decode->update();
            break;
        }
    }
}

///////////////////////////////////////////

StatusInfoBarPlugin::StatusInfoBarPlugin(InfoBar *infoBar) :
    InfoBarPlugin(infoBar, "status_plugin")
{
    infoBar->control()->addControlEventHandler(this);
    setText("Status");
    _linkedWidget = _panel = new StatusInfoPanel(infoBar, PANELRECT);
    _panel->hide();
    _icon = new MImage(app(), PIXMAPPATH + "frontend.png");
}

StatusInfoBarPlugin::~StatusInfoBarPlugin()
{
    infoBar()->control()->removeControlEventHandler(this);
    delete _icon;
}

void StatusInfoBarPlugin::onControlEvent(Control *control, Control::Event event)
{
    if (event == Control::evChangeChannel) {
        _panel->repaint();
    }
}

bool StatusInfoBarPlugin::keyDown(int ascii, int modifiers, int scancode)
{
    switch (ascii) {
    case MWKEY_LEFT: _panel->incPage(-1); return true;
    case MWKEY_RIGHT: _panel->incPage(1); return true;
    default: return MLabel::keyDown(ascii, modifiers, scancode);
    }
}
