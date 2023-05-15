/***************************************************************************
 *     Copyright (c) 2010-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgrc_packet_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/192 $
 * $brcm_Date: 8/24/12 1:40p $
 *
 * Module Description: GRC Packet Private API
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/grc/7435/bgrc_packet_priv.c $
 * 
 * Hydra_Software_Devel/192   8/24/12 1:40p nissen
 * SW7445-22: Added support for the dual M2MCs on the 7445.
 * 
 * Hydra_Software_Devel/191   8/22/12 5:29p nissen
 * SWVC4-380: Fixed support for WRGB1565 surfaces.
 * 
 * Hydra_Software_Devel/190   8/21/12 6:03p nissen
 * SWVC4-380: Fixed support for using a secondary surface.
 * 
 * Hydra_Software_Devel/189   8/21/12 12:30p nissen
 * SW7425-3625: Switched to using cached memory access.
 * 
 * Hydra_Software_Devel/188   8/2/12 2:31p nissen
 * SW7563-13: Added support for the 7563.
 * 
 * Hydra_Software_Devel/187   7/26/12 5:46p nissen
 * SWVC4-380: Added support for WRGB1565 surfaces using SourceFeeders
 * packet.
 * 
 * Hydra_Software_Devel/186   7/5/12 3:33p nissen
 * SW7231-850: Added code to reverse src stripes when overlapping scaled
 * blits.
 * 
 * Hydra_Software_Devel/185   6/28/12 6:25p nissen
 * SW7425-2480: Added SCALER_CTRL reset to fill blit.
 * 
 * Hydra_Software_Devel/184   6/28/12 2:41p nissen
 * SW7425-2480: Added filter reset to sync blit.
 * 
 * Hydra_Software_Devel/183   6/27/12 4:00p nissen
 * SW7425-2480: Added alpha premultiply reset to sync blit.
 * 
 * Hydra_Software_Devel/182   6/26/12 6:25p nissen
 * SW7425-2480: Added dest colorkey reset to sync blit.
 * 
 * Hydra_Software_Devel/181   6/18/12 4:44p nissen
 * SW7425-3196: Added code to clear stripe num field when scaling.
 * 
 * Hydra_Software_Devel/180   6/13/12 8:09p nissen
 * SW7425-3196: Further fix for fixed scaling.
 * 
 * Hydra_Software_Devel/179   6/13/12 6:46p nissen
 * SW7425-3196: Adding fix for fixed scaling when the shift is not using
 * the full precision.
 * 
 * Hydra_Software_Devel/178   6/8/12 3:43p nissen
 * SW7425-3196: Switched to generating stripes for blits with vertical
 * down scales larger than 4x.
 * 
 * Hydra_Software_Devel/177   6/7/12 11:23a nissen
 * SW7435-27: Removed cache flush when clearing the sync blit's pixel.
 * 
 * Hydra_Software_Devel/176   6/5/12 6:00p nissen
 * SW7435-27: Added cache flush when clearing the sync blit's pixel.
 * 
 * Hydra_Software_Devel/175   5/9/12 1:17p nissen
 * SW7420-2303: Added code to verify that palette is not in MEMC1 for the
 * 7420.
 * 
 * Hydra_Software_Devel/174   5/1/12 12:58p nissen
 * SW7584-11: Added support or 7584.
 * 
 * Hydra_Software_Devel/173   4/5/12 4:35p nissen
 * SW7425-1413: Added fix to make sure last output stripe is less pixels
 * than the others.
 * 
 * Hydra_Software_Devel/172   3/16/12 4:09p nissen
 * SW7425-2480: Added rop reset to sync blit.
 * 
 * Hydra_Software_Devel/171   3/9/12 5:39p nissen
 * SW7038-3520: Temporarily removing veritcal phase fix when the
 * horizontal fixed scale  factor is 0 for 7038.
 * 
 * Hydra_Software_Devel/170   3/7/12 5:51p nissen
 * SW7038-3520: Added another condition to prevent possible divide by
 * zero.
 * 
 * Hydra_Software_Devel/169   3/7/12 12:35p nissen
 * SW7038-3520: Fixed condition to prevent output stripe width from
 * becoming zero.
 * 
 * Hydra_Software_Devel/168   3/2/12 12:42p nissen
 * SW7435-13 : Added fix to 7435 A0 for null src and dst feeders.
 * 
 * Hydra_Software_Devel/167   3/1/12 3:31p nissen
 * SW7420-2097: Removed vertical initial phase fix for greater than 4x
 * down scales.
 * 
 * Hydra_Software_Devel/166   2/17/12 1:30p nissen
 * SW7425-1413: Fixing problem with last output stripe size for M2MCs with
 * split fifo.
 * 
 * Hydra_Software_Devel/165   2/15/12 8:15p nissen
 * SW7038-3520: Fixed condition for ver phase fix.
 * 
 * Hydra_Software_Devel/164   2/15/12 1:15p nissen
 * SW7038-3520: Added veritcal phase fix when the horizontal fixed scale
 * factor is 0 for 7038.
 * 
 * Hydra_Software_Devel/163   2/10/12 3:47p nissen
 * SW7420-1150: Fixed build error.
 * 
 * Hydra_Software_Devel/162   2/10/12 1:37p nissen
 * SW7420-1150: Fixed look up bit for palette bilts.
 * 
 * Hydra_Software_Devel/161   2/9/12 2:37p nissen
 * SW7038-3520: Added more conditions to prevent mod with zero.
 * 
 * Hydra_Software_Devel/160   1/9/12 5:19p nissen
 * SW7425-1207: Fixed UpdateScale packet.
 * 
 * Hydra_Software_Devel/159   1/6/12 1:40p nissen
 * SW7435-13 : Fixed support for 7435 A0.
 * 
 * Hydra_Software_Devel/158   1/5/12 1:54p nissen
 * SW7038-3520: Fixed build warning.
 * 
 * Hydra_Software_Devel/157   1/5/12 11:51a nissen
 * SW7038-3520: Fixed condition to prevent mod with zero when fixed scale
 * is zero.
 * 
 * Hydra_Software_Devel/156   1/3/12 4:37p nissen
 * SW7425-1921: Changed support for dest colorkeying.
 * 
 * Hydra_Software_Devel/155   12/21/11 5:15p nissen
 * SW7360-5: Added support for the 7360 A0.
 * 
 * Hydra_Software_Devel/154   12/21/11 1:42p nissen
 * SW7425-1915: Added code to prevent down scale from exceeding the max.
 * 
 * Hydra_Software_Devel/153   12/20/11 5:35p nissen
 * SW7231-540: Fixed sync and scaling problems with mirroring.
 * 
 * Hydra_Software_Devel/152   12/20/11 3:18p nissen
 * SW7435-13: Fixed build error.
 * 
 * Hydra_Software_Devel/151   12/20/11 12:37p nissen
 * SW7435-13: Fixing another build error
 * 
 * Hydra_Software_Devel/150   12/19/11 7:21p nissen
 * SW7435-13: Fixing build error
 * 
 * Hydra_Software_Devel/149   12/19/11 5:17p nissen
 * SW7435-13: Fixing build error
 * 
 * Hydra_Software_Devel/148   12/19/11 4:47p nissen
 * SW7038-3520: Added condition to prevent mod with zero when fixed scale
 * is zero.
 * 
 * Hydra_Software_Devel/147   12/19/11 4:34p nissen
 * SW7435-13 : Fixing build
 * 
 * Hydra_Software_Devel/146   12/16/11 6:48p nissen
 * SW7038-3520: Added flag to force fixed scale factor of 0.
 * 
 * Hydra_Software_Devel/145   12/15/11 6:58p nissen
 * SW7435-13 : Added support for 7435 A0.
 * 
 * Hydra_Software_Devel/144   12/13/11 3:41p nissen
 * SW7231-535: Fixed surface rectangle verification after sync blit.
 * 
 * Hydra_Software_Devel/143   12/9/11 7:38p nissen
 * SW7425-1207: Fixed phase with UpdateScalePacket.
 * 
 * Hydra_Software_Devel/142   12/8/11 5:11p nissen
 * SW7038-3520: Added fix so that a scaler step of 0 can be used when the
 * source size is equal to 1.
 * 
 * Hydra_Software_Devel/141   12/1/11 5:10p nissen
 * SW7429-4: Added SourceNone fix for 7429.
 * 
 * Hydra_Software_Devel/140   11/30/11 4:29p nissen
 * SW7425-1830: Fixed warning in non debug build.
 * 
 * Hydra_Software_Devel/139   11/25/11 5:21p nissen
 * SW7231-477 : Fixed minimum vertical initial phase for sources that are
 * 4096 pixels or greater.
 * 
 * Hydra_Software_Devel/138   11/23/11 5:49p nissen
 * SW7425-1830: Fixed rectangle verification.
 * 
 * Hydra_Software_Devel/137   11/22/11 5:40p nissen
 * SW7425-1830: Added code to verify rectangle sizes don't exceed the max
 * of 8191.
 * 
 * Hydra_Software_Devel/136   11/10/11 6:03p nissen
 * SW7425-1413: Disabled split fifo for 7425 B0.
 * 
 * Hydra_Software_Devel/135   11/10/11 2:05p nissen
 * SW7405-5534: Added code to verify down scale factor.
 * 
 * Hydra_Software_Devel/134   11/8/11 11:35a nissen
 * SW7425-1413: Fixed build error.
 * 
 * Hydra_Software_Devel/133   11/7/11 8:09p nissen
 * SW7425-1413: Fixed build errors.
 * 
 * Hydra_Software_Devel/132   11/7/11 6:45p nissen
 * SW7425-1413: Fixed problem with last output stripe size for 7425 B0.
 * 
 * Hydra_Software_Devel/131   11/3/11 4:42p nissen
 * SWBLURAY-27238: Added support for 2nd M2MC for 7640 B0.
 * 
 * Hydra_Software_Devel/130   10/24/11 6:57p nissen
 * SW7420-2097: Adjusted minimum vertical initial phase for greater than
 * 4x down scales.
 * 
 * Hydra_Software_Devel/129   10/21/11 5:25p nissen
 * SW7038-3518: Removed odd source x 422 pixel fix for the 7038.
 * 
 * Hydra_Software_Devel/128   10/21/11 5:09p nissen
 * SW7420-2097: Setting a minimum vertical initial phase when vertically
 * down scaling more than 4x to prevent a hang.
 * 
 * Hydra_Software_Devel/127   10/18/11 4:36p nissen
 * SW7038-3518: Added condition to fix odd source x when source format is
 * 422.
 * 
 * Hydra_Software_Devel/126   10/12/11 2:25p nissen
 * SW7425-1413: Disabled the split fifo for the 7425 B0.
 * 
 * Hydra_Software_Devel/125   10/6/11 12:08p nissen
 * SW7420-2082: Added debug prints for FixedScale
 * 
 * Hydra_Software_Devel/124   10/5/11 1:01p nissen
 * SW7405-3671: Fixed width adjustment for odd x value.
 * 
 * Hydra_Software_Devel/123   10/3/11 6:43p nissen
 * SW7405-3671: Fixed odd x value for single stripes.
 * 
 * Hydra_Software_Devel/122   10/3/11 12:05p nissen
 * SW7420-2079: Fixed problem with writing non-blit packets.
 * 
 * Hydra_Software_Devel/121   9/30/11 9:43p nissen
 * SW7405-3671: Fixed first stripe when it's x value is odd for 422
 * formats.
 * 
 * Hydra_Software_Devel/120   9/29/11 6:13p nissen
 * SW7405-3671: Fixed scale order for SW striped scales.
 * 
 * Hydra_Software_Devel/119   9/29/11 12:21p nissen
 * SW7401-4480: Changed 7400 to use SW assisted 420 destriping.
 * 
 * Hydra_Software_Devel/118   9/24/11 1:34p nissen
 * SW7405-3671: Fixed scale order when 420 destriping.
 * 
 * Hydra_Software_Devel/117   9/16/11 1:04p nissen
 * SW7425-1207: Added support for UpdateScaleBlit packet.
 * 
 * Hydra_Software_Devel/116   9/13/11 4:35p nissen
 * SW7420-1575: Fixed surface rectangle verification, and added support
 * for surface format verification.
 * 
 * Hydra_Software_Devel/115   9/13/11 3:35p nissen
 * SW7038-3512: Removed print statements.
 * 
 * Hydra_Software_Devel/114   9/13/11 1:24p nissen
 * SW7038-3512: Fixed bilinear filter table for 7038.
 * 
 * Hydra_Software_Devel/113   9/9/11 12:49p nissen
 * SW7038-3513: Reduced padding between stripes by one pixel for the 7038.
 * 
 * Hydra_Software_Devel/112   9/7/11 5:01p nissen
 * SW7405-3671: Changed 420 fixed scale destriping.
 * 
 * Hydra_Software_Devel/111   9/7/11 2:38p nissen
 * SW7405-3671: Added another fix for chroma width when destriping 420.
 * 
 * Hydra_Software_Devel/110   9/6/11 8:59p nissen
 * SW7038-3513: Fixed problem with forced striping 422 formats.
 * 
 * Hydra_Software_Devel/109   9/5/11 5:28p nissen
 * SW7405-3671: Added fix for chroma width when destriping 420.
 * 
 * Hydra_Software_Devel/108   8/30/11 2:30p nissen
 * SW7401-4509: Fixed setting scaling order when using fixed scaling.
 * 
 * Hydra_Software_Devel/107   8/8/11 12:44p nissen
 * SW7405-3671: Added initial support for fixed scaling with 420
 * destriping on the 7401.
 * 
 * Hydra_Software_Devel/106   7/28/11 4:12p nissen
 * SW7405-3671: Another further fix for final 420 stripe.
 * 
 * Hydra_Software_Devel/105   7/28/11 3:33p nissen
 * SW7405-3671: Further fix for final 420 stripe.
 * 
 * Hydra_Software_Devel/104   7/27/11 5:47p nissen
 * SW7405-3671: Added support for fixed scaling with 420 destriping.
 * 
 * Hydra_Software_Devel/103   7/21/11 2:18p nissen
 * SW7405-3671: Fixed problem with final 420 stripe.
 * 
 * Hydra_Software_Devel/102   7/20/11 3:36p nissen
 * SW7405-3671: Fixed problem with final 420 stripe being less than 2
 * pixels.
 * 
 * Hydra_Software_Devel/101   7/14/11 3:15p nissen
 * SW7420-1974: Writing dest addr and format registers in DestinationColor
 * packet to prevent hang.
 * 
 * Hydra_Software_Devel/100   6/29/11 3:02p nissen
 * SW7405-3671: Removed default initial phase adjustment when using fixed
 * scaling.
 * 
 * Hydra_Software_Devel/99   6/28/11 2:29p nissen
 * SW7420-1893: Changed Sync so that it continues to try it's blit instead
 * of reporting out of memory.
 * 
 * Hydra_Software_Devel/98   6/28/11 11:58a nissen
 * SW7405-3671: Fixed hang when using odd x position for 422 sources.
 * 
 * Hydra_Software_Devel/97   6/23/11 10:23a nissen
 * SW7420-1893: Prevented sync blit frin over writting packet memory.
 * 
 * Hydra_Software_Devel/96   6/9/11 6:19p nissen
 * SW7405-3671: Fixed forced striping while scaling.
 * 
 * Hydra_Software_Devel/95   6/7/11 1:15p nissen
 * SW7405-3671: Added code to store group header when running out of
 * memory.
 * 
 * Hydra_Software_Devel/94   5/27/11 12:35p nissen
 * SW7405-3671: Fixed problem with rectangle registers after doing a
 * striped scale blit.
 * 
 * Hydra_Software_Devel/93   5/26/11 6:14p nissen
 * SW7335-916: Fixed linear destripe for SourceControl packet.
 * 
 * Hydra_Software_Devel/92   5/20/11 7:09p nissen
 * SW7335-1213: Fixing sync blit.
 * 
 * Hydra_Software_Devel/91   5/20/11 5:13p nissen
 * SW7335-1213: Fixing sync blit.
 * 
 * Hydra_Software_Devel/90   5/19/11 5:09p nissen
 * SW7038-3509: Fixed setting filter table for 7038.
 * 
 * Hydra_Software_Devel/89   5/11/11 1:22p nissen
 * SW7401-4501: Added L8 pixel format.
 * 
 * Hydra_Software_Devel/88   5/6/11 1:05p nissen
 * SW7038-3500: Fixed color matrix add shift.
 * 
 * Hydra_Software_Devel/87   5/6/11 11:55a nissen
 * SW7425-19: Fixing SourceNone for new cores.
 * 
 * Hydra_Software_Devel/86   5/2/11 4:55p nissen
 * SW7425-19: Fixing SourceNone for 7552
 * 
 * Hydra_Software_Devel/85   5/2/11 4:23p nissen
 * SW7425-19: Fixed build error.
 * 
 * Hydra_Software_Devel/84   5/2/11 2:38p nissen
 * SW7425-19: Fixing SourceNone for 7358.
 * 
 * Hydra_Software_Devel/83   5/1/11 4:38p nissen
 * SW7038-3500: Fixed filter tables.
 * 
 * Hydra_Software_Devel/82   4/27/11 2:27p nissen
 * SW7425-19: Using SourceNone fix for 7358.
 * 
 * Hydra_Software_Devel/81   4/25/11 5:08p nissen
 * SW7630-104: Added more support for Cb8 and Cr8 formats.
 * 
 * Hydra_Software_Devel/80   4/25/11 3:54p nissen
 * SW7630-104: Added support for A8_Y8, Cb8 and Cr8 formats.
 * 
 * Hydra_Software_Devel/79   3/18/11 7:41p nissen
 * SW7038-3500: Fixing header size.
 * 
 * Hydra_Software_Devel/78   3/18/11 1:26p nissen
 * SW7420-1575: Added new width and height fields for surface planes.
 * 
 * Hydra_Software_Devel/77   3/17/11 4:47p nissen
 * SW7405-3671: Fixed new luma pixel formats.
 * 
 * Hydra_Software_Devel/76   3/17/11 4:39p nissen
 * SW7344-32: Fixed changes to reset state.
 * 
 * Hydra_Software_Devel/75   3/16/11 5:20p nissen
 * SW7344-32: Fixed reset state to use default output feeder and
 * rectangle.
 * 
 * Hydra_Software_Devel/74   3/15/11 3:33p nissen
 * SW7405-5183: Fixed SourceColor packet.
 * 
 * Hydra_Software_Devel/73   3/10/11 4:44p nissen
 * SW7420-1575: Fixed surface rectangle verification.
 * 
 * Hydra_Software_Devel/72   3/8/11 1:21p nissen
 * SW7422-256: Switched to SW strip setup when vertical scale down max
 * exceeded.
 * 
 * Hydra_Software_Devel/71   3/7/11 3:51p nissen
 * SW7420-1575: Added support for surface rectangle verification.
 * 
 * Hydra_Software_Devel/70   3/7/11 2:07p nissen
 * SW7038-3500: Switched header size back.
 * 
 * Hydra_Software_Devel/69   3/5/11 4:55p nissen
 * SW7038-3500: Changed header size to fit filter table.
 * 
 * Hydra_Software_Devel/68   3/5/11 4:21p nissen
 * SW7401-4480: Set 7401 so that SW sets up destripe blits.
 * 
 * Hydra_Software_Devel/67   3/4/11 5:18p nissen
 * SW7422-256: Fixed surface scaling for large surfaces.
 * 
 * Hydra_Software_Devel/66   3/4/11 1:43p nissen
 * SW7038-3500: Fixed header size.
 * 
 * Hydra_Software_Devel/65   3/3/11 6:15p nissen
 * SW7401-4480: Forcing even destripe source rectangle values.
 * 
 * Hydra_Software_Devel/64   3/3/11 4:51p nissen
 * SW7038-3500: Fixed build error for 7038.
 * 
 * Hydra_Software_Devel/63   3/3/11 3:01p nissen
 * SW7038-3500: Fixed more build errors.
 * 
 * Hydra_Software_Devel/62   3/2/11 7:47p nissen
 * SW7038-3500: Fixed more build errors.
 * 
 * Hydra_Software_Devel/61   3/2/11 7:12p nissen
 * SW7038-3500: Fixed build errors on 7038.
 * 
 * Hydra_Software_Devel/60   3/2/11 5:01p nissen
 * SW7405-3671: Added new luma pixel formats.
 * 
 * Hydra_Software_Devel/59   2/27/11 1:38p nissen
 * SW7422-287: Fixed memory read violation at address 0 when doing a fill.
 * 
 * Hydra_Software_Devel/58   2/25/11 4:56p nissen
 * SW7038-3500: Added initial support for 7038.
 * 
 * Hydra_Software_Devel/57   12/15/10 1:18p nissen
 * SW7425-19: Fixed problem with SourceNone packet.
 * 
 * Hydra_Software_Devel/56   12/10/10 1:18p nissen
 * SW7425-19: Changed format to 1-bit when using SourceColor or SourceNone
 * packets.
 * 
 * Hydra_Software_Devel/55   12/10/10 12:35p nissen
 * SW7425-19: Changed format to 4-bit when using SourceColor or SourceNone
 * packets.
 * 
 * Hydra_Software_Devel/54   12/9/10 3:32p nissen
 * SW7425-19: Enabling source feeder in SourceNone packet to prevent hang
 * on 7422.
 * 
 * Hydra_Software_Devel/53   12/6/10 12:29p nissen
 * SW7420-1138: Added support for validating memory bounds used by blits.
 * 
 * Hydra_Software_Devel/52   12/3/10 1:15a nissen
 * SW7420-1150: Fixed bug in palette blits.
 * 
 * Hydra_Software_Devel/51   11/30/10 3:38p nissen
 * SW7335-916: Fixed macro warnings.
 * 
 * Hydra_Software_Devel/50   11/30/10 3:05p nissen
 * SW7420-1236: Fixed source and destination colorkey.
 * 
 * Hydra_Software_Devel/49   11/30/10 2:21p nissen
 * SW7420-1236: Fixed destination colorkey.
 * 
 * Hydra_Software_Devel/48   11/25/10 2:36p nissen
 * SW7335-916: Added more debug print parameters to FixedScale packet.
 * 
 * Hydra_Software_Devel/47   11/25/10 1:15p nissen
 * SW7420-1150: Added fix for palette blits.
 * 
 * Hydra_Software_Devel/46   11/24/10 5:59p nissen
 * SW7425-19: Enabled source feeder and set format for SourceNone packet
 * to prevent hangs on 7422.
 * 
 * Hydra_Software_Devel/44   11/22/10 12:51p nissen
 * SW7425-19: Disabled feeders for none packets.
 * 
 * Hydra_Software_Devel/43   11/18/10 7:52p nissen
 * SW7425-19: Fixed None packets.
 * 
 * Hydra_Software_Devel/42   11/18/10 5:56p nissen
 * SW7425-19: Fixed destination format registers for null dest on the
 * 7422.
 * 
 * Hydra_Software_Devel/41   11/18/10 5:45p nissen
 * SW7425-19: Fixed source format registers for null source on the 7422.
 * 
 * Hydra_Software_Devel/40   11/16/10 2:38p nissen
 * SW7335-916: Fixed support for FixedScale packet.
 * 
 * Hydra_Software_Devel/39   11/15/10 3:08p nissen
 * SW7420-1150: Fixed palette to palette blits.
 * 
 * Hydra_Software_Devel/38   11/11/10 1:34p nissen
 * SW7335-916: Added support for FixedScale packet.
 * 
 * Hydra_Software_Devel/37   11/1/10 10:32a nissen
 * SW7420-1200: Fixed problem with checking for available M2MC FIFO
 * memory.
 * 
 * Hydra_Software_Devel/36   10/27/10 12:20p nissen
 * SW7420-1150: Fixed support for palette to palatte blits.
 * 
 * Hydra_Software_Devel/35   10/20/10 7:17p nissen
 * SW7420-1193: Fixed blend equation when using subtraction.
 * 
 * Hydra_Software_Devel/34   10/6/10 2:29p nissen
 * SW7405-3671: Added DestinationColor packet.
 * 
 * Hydra_Software_Devel/33   9/27/10 5:51p nissen
 * SW7405-3671: Fixed problem destriping odd width surfaces.
 * 
 * Hydra_Software_Devel/32   9/23/10 2:01p nissen
 * SW7405-4892: Fixed BGRC_PACKET_P_ConvertFilterCoeffs when a size
 * parameter is 0.
 * 
 * Hydra_Software_Devel/31   9/20/10 3:38p nissen
 * SW7405-4869: Fixed Rop selection.
 * 
 * Hydra_Software_Devel/30   9/20/10 2:04p nissen
 * SW7405-3671: Added support for Source, Destination, and Output Control
 * packets.
 * 
 * Hydra_Software_Devel/29   8/19/10 12:11p nissen
 * SW7405-3671: Fixed x/y position of destripe blit.
 * 
 * Hydra_Software_Devel/28   8/18/10 3:27p nissen
 * SW7405-3671: Checking if user overwrote submited packet memory.
 * 
 * Hydra_Software_Devel/27   8/11/10 2:33p nissen
 * SW7405-3671: Fixed anisotropic and sharp filter tables.
 * 
 * Hydra_Software_Devel/26   8/9/10 10:33p nissen
 * SW7405-3671: Forcing SW striping for YCbCr422 source blits.
 * 
 * Hydra_Software_Devel/25   8/9/10 6:44p nissen
 * SW7405-3671: Added fix for odd stripe width for YCbCr422 surfaces.
 * 
 * Hydra_Software_Devel/24   8/9/10 5:09p nissen
 * SW7405-3671: Removed pixel formats that are not in PXL.
 * 
 * Hydra_Software_Devel/23   8/9/10 11:30a nissen
 * SW7405-3671: Fixed YUV420 one pixel wide blit, and added code for SW
 * destriping.
 * 
 * Hydra_Software_Devel/22   8/4/10 8:36p nissen
 * SW7405-3671: Fixed problem with over allocating hw fifo when dealing
 * with advance interrupts.
 * 
 * Hydra_Software_Devel/21   8/4/10 7:13p nissen
 * SW7405-3671: Changed dither default to false.
 * 
 * Hydra_Software_Devel/20   8/4/10 6:16p nissen
 * SW7405-3671: Fixed problem with missing advanced interrupt.
 * 
 * Hydra_Software_Devel/19   8/3/10 5:17p nissen
 * SW7405-3671: Forced palette address to be non-null.
 * 
 * Hydra_Software_Devel/18   8/3/10 5:03p nissen
 * SW7405-3671: Fixed reset state and palette packets.
 * 
 * Hydra_Software_Devel/17   8/2/10 3:00p nissen
 * SW7405-3671: Removed reset state packet from sync blit.
 * 
 * Hydra_Software_Devel/16   8/1/10 3:37p nissen
 * SW7405-3671: Added support for horizontal mirroring for destination and
 * output.
 * 
 * Hydra_Software_Devel/15   7/31/10 5:13p nissen
 * SW7405-3671: Fixed HW fifo wrapping and sync interrupt.
 * 
 * Hydra_Software_Devel/14   7/30/10 11:20a nissen
 * SW7405-3671: Fixed advance interrupt on context.
 * 
 * Hydra_Software_Devel/13   7/29/10 2:01p nissen
 * SW7405-3671: Fixed advance interrupt.
 * 
 * Hydra_Software_Devel/12   7/28/10 5:53p nissen
 * SW7405-3671: Fixed Ax pixel formats.
 * 
 * Hydra_Software_Devel/11   7/28/10 4:01p nissen
 * SW7405-3671: Fixed group header problem with packets that don't include
 * an execute.
 * 
 * Hydra_Software_Devel/10   7/27/10 2:36p nissen
 * SW7405-3671: Fixed advance interrupt.
 * 
 * Hydra_Software_Devel/9   7/23/10 8:06p nissen
 * SW7405-3671: Fixed problem with main interrupt handling.
 * 
 * Hydra_Software_Devel/8   7/23/10 4:40p nissen
 * SW7405-3671: Fixed converting filter tables, and blend blit packet.
 * 
 * Hydra_Software_Devel/7   7/23/10 3:59p nissen
 * SW7405-3671: Added control entries to group mask array.
 * 
 * Hydra_Software_Devel/6   7/21/10 3:09p nissen
 * SW7405-3671: Added fields to device group header mask array.
 * 
 * Hydra_Software_Devel/5   7/21/10 11:35a nissen
 * SW7405-3671: Fixed define in device group sizes.
 * 
 * Hydra_Software_Devel/4   7/18/10 4:11p nissen
 * SW7405-3671: Fixed reset state and sync.
 * 
 * Hydra_Software_Devel/3   7/17/10 3:32p nissen
 * SW7405-3671: Fixed memory alignment for sync packet.
 * 
 * Hydra_Software_Devel/2   7/15/10 9:10p nissen
 * SW7405-3671: Added more interrupt support.
 * 
 * Hydra_Software_Devel/1   7/12/10 1:25p nissen
 * SW7405-3671: Initial version.
 * 
 ***************************************************************************/

#if (BCHP_CHIP==7435) || (BCHP_CHIP==7445)
#include "bchp_m2mc.h"
#include "bchp_m2mc1.h"
#else
#include "bchp_m2mc.h"
#endif

#include "bchp_common.h"

#include "bgrc.h"
#include "bgrc_packet.h"
#include "bgrc_packet_priv.h"
#include "bgrc_private.h"

BDBG_MODULE(BGRC);

/***************************************************************************/
void BGRC_PACKET_P_PrintRegisters( BGRC_Handle hGrc )
{
	BSTD_UNUSED(hGrc);
	BDBG_ERR(( "LIST_CTRL                       %08x", BGRC_P_ReadReg32( hGrc->hRegister, LIST_CTRL ) ));
	BDBG_ERR(( "LIST_STATUS                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, LIST_STATUS ) ));
	BDBG_ERR(( "LIST_FIRST_PKT_ADDR             %08x", BGRC_P_ReadReg32( hGrc->hRegister, LIST_FIRST_PKT_ADDR ) ));
	BDBG_ERR(( "LIST_CURR_PKT_ADDR              %08x", BGRC_P_ReadReg32( hGrc->hRegister, LIST_CURR_PKT_ADDR ) ));
	BDBG_ERR(( "BLIT_STATUS                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_STATUS ) ));
	BDBG_ERR(( "BLIT_SRC_ADDRESS                %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_ADDRESS ) ));
	BDBG_ERR(( "BLIT_DEST_ADDRESS               %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_DEST_ADDRESS ) ));
	BDBG_ERR(( "BLIT_OUTPUT_ADDRESS             %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_OUTPUT_ADDRESS ) ));
	BDBG_ERR(( "BLIT_MEM_HI                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_MEM_HI ) ));
	BDBG_ERR(( "BLIT_MEM_LO                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_MEM_LO ) ));
	BDBG_ERR(( "SRC_FEEDER_ENABLE               %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_FEEDER_ENABLE ) ));
	BDBG_ERR(( "SRC_SURFACE_ADDR_0              %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "SRC_SURFACE_STRIDE_0            %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "SRC_SURFACE_ADDR_1              %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_STRIDE_1            %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_1        %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_2        %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "SRC_SURFACE_FORMAT_DEF_3        %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_FORMAT_DEF_3 ) ));
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_1      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_1_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_2      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_1_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "SRC_SURFACE_1_FORMAT_DEF_3      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_SURFACE_1_FORMAT_DEF_3 ) ));
#endif
	BDBG_ERR(( "SRC_W_ALPHA                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_W_ALPHA ) ));
	BDBG_ERR(( "SRC_CONSTANT_COLOR              %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CONSTANT_COLOR ) ));
	BDBG_ERR(( "DEST_FEEDER_ENABLE              %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_FEEDER_ENABLE ) ));
	BDBG_ERR(( "DEST_SURFACE_ADDR_0             %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "DEST_SURFACE_STRIDE_0           %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "DEST_SURFACE_ADDR_1             %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_STRIDE_1           %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_1       %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_2       %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "DEST_SURFACE_FORMAT_DEF_3       %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_SURFACE_FORMAT_DEF_3 ) ));
	BDBG_ERR(( "DEST_W_ALPHA                    %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_W_ALPHA ) ));
	BDBG_ERR(( "DEST_CONSTANT_COLOR             %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_CONSTANT_COLOR ) ));
	BDBG_ERR(( "OUTPUT_FEEDER_ENABLE            %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_FEEDER_ENABLE ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_ADDR_0           %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_ADDR_0 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_STRIDE_0         %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_STRIDE_0 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_ADDR_1           %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_ADDR_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_STRIDE_1         %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_STRIDE_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_1     %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_FORMAT_DEF_1 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_2     %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_FORMAT_DEF_2 ) ));
	BDBG_ERR(( "OUTPUT_SURFACE_FORMAT_DEF_3     %08x", BGRC_P_ReadReg32( hGrc->hRegister, OUTPUT_SURFACE_FORMAT_DEF_3 ) ));
	BDBG_ERR(( "BLIT_HEADER                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_HEADER ) ));
	BDBG_ERR(( "BLIT_SRC_TOP_LEFT               %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_SRC_SIZE                   %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_SIZE ) ));
#if defined(BCHP_M2MC_BLIT_SRC_TOP_LEFT_1)
	BDBG_ERR(( "BLIT_SRC_TOP_LEFT_1             %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_TOP_LEFT_1 ) ));
	BDBG_ERR(( "BLIT_SRC_SIZE_1                 %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_SIZE_1 ) ));
#endif
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
	BDBG_ERR(( "BLIT_SRC_STRIPE_HEIGHT_WIDTH_0  %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_STRIPE_HEIGHT_WIDTH_0 ) ));
	BDBG_ERR(( "BLIT_SRC_STRIPE_HEIGHT_WIDTH_1  %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_SRC_STRIPE_HEIGHT_WIDTH_1 ) ));
#endif
	BDBG_ERR(( "BLIT_DEST_TOP_LEFT              %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_DEST_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_DEST_SIZE                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_DEST_SIZE ) ));
	BDBG_ERR(( "BLIT_OUTPUT_TOP_LEFT            %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_OUTPUT_TOP_LEFT ) ));
	BDBG_ERR(( "BLIT_OUTPUT_SIZE                %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_OUTPUT_SIZE ) ));
	BDBG_ERR(( "BLIT_INPUT_STRIPE_WIDTH         %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_INPUT_STRIPE_WIDTH ) ));
#if defined(BCHP_M2MC_BLIT_INPUT_STRIPE_WIDTH_1)
	BDBG_ERR(( "BLIT_INPUT_STRIPE_WIDTH_1       %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_INPUT_STRIPE_WIDTH_1 ) ));
#endif
	BDBG_ERR(( "BLIT_OUTPUT_STRIPE_WIDTH        %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_OUTPUT_STRIPE_WIDTH ) ));
	BDBG_ERR(( "BLIT_STRIPE_OVERLAP             %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_STRIPE_OVERLAP ) ));
#if defined(BCHP_M2MC_BLIT_STRIPE_OVERLAP_1)
	BDBG_ERR(( "BLIT_STRIPE_OVERLAP_1           %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_STRIPE_OVERLAP_1 ) ));
#endif
	BDBG_ERR(( "BLIT_CTRL                       %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLIT_CTRL ) ));
	BDBG_ERR(( "SCALER_CTRL                     %08x", BGRC_P_ReadReg32( hGrc->hRegister, SCALER_CTRL ) ));
	BDBG_ERR(( "HORIZ_AVERAGER_COUNT            %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_AVERAGER_COUNT ) ));
	BDBG_ERR(( "HORIZ_AVERAGER_COEFF            %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_AVERAGER_COEFF ) ));
	BDBG_ERR(( "VERT_AVERAGER_COUNT             %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_AVERAGER_COUNT ) ));
	BDBG_ERR(( "VERT_AVERAGER_COEFF             %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_AVERAGER_COEFF ) ));
	BDBG_ERR(( "HORIZ_SCALER_INITIAL_PHASE      %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_SCALER_INITIAL_PHASE ) ));
	BDBG_ERR(( "HORIZ_SCALER_STEP               %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_SCALER_STEP ) ));
#if defined(BCHP_M2MC_HORIZ_SCALER_1_INITIAL_PHASE)
	BDBG_ERR(( "HORIZ_SCALER_1_INITIAL_PHASE    %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_SCALER_1_INITIAL_PHASE ) ));
	BDBG_ERR(( "HORIZ_SCALER_1_STEP             %08x", BGRC_P_ReadReg32( hGrc->hRegister, HORIZ_SCALER_1_STEP ) ));
#endif
	BDBG_ERR(( "VERT_SCALER_INITIAL_PHASE       %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_SCALER_INITIAL_PHASE ) ));
	BDBG_ERR(( "VERT_SCALER_STEP                %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_SCALER_STEP ) ));
#if defined(BCHP_M2MC_VERT_SCALER_1_INITIAL_PHASE)
	BDBG_ERR(( "VERT_SCALER_1_INITIAL_PHASE     %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_SCALER_1_INITIAL_PHASE ) ));
	BDBG_ERR(( "VERT_SCALER_1_STEP              %08x", BGRC_P_ReadReg32( hGrc->hRegister, VERT_SCALER_1_STEP ) ));
#endif
	BDBG_ERR(( "BLEND_COLOR_OP                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLEND_COLOR_OP ) ));
	BDBG_ERR(( "BLEND_ALPHA_OP                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLEND_ALPHA_OP ) ));
	BDBG_ERR(( "BLEND_CONSTANT_COLOR            %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLEND_CONSTANT_COLOR ) ));
	BDBG_ERR(( "BLEND_COLOR_KEY_ACTION          %08x", BGRC_P_ReadReg32( hGrc->hRegister, BLEND_COLOR_KEY_ACTION ) ));
	BDBG_ERR(( "ROP_OPERATION                   %08x", BGRC_P_ReadReg32( hGrc->hRegister, ROP_OPERATION ) ));
	BDBG_ERR(( "ROP_PATTERN_TOP                 %08x", BGRC_P_ReadReg32( hGrc->hRegister, ROP_PATTERN_TOP ) ));
	BDBG_ERR(( "ROP_PATTERN_BOTTOM              %08x", BGRC_P_ReadReg32( hGrc->hRegister, ROP_PATTERN_BOTTOM ) ));
	BDBG_ERR(( "ROP_PATTERN_COLOR_0             %08x", BGRC_P_ReadReg32( hGrc->hRegister, ROP_PATTERN_COLOR_0 ) ));
	BDBG_ERR(( "ROP_PATTERN_COLOR_1             %08x", BGRC_P_ReadReg32( hGrc->hRegister, ROP_PATTERN_COLOR_1 ) ));
	BDBG_ERR(( "SRC_COLOR_KEY_HIGH              %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_COLOR_KEY_HIGH ) ));
	BDBG_ERR(( "SRC_COLOR_KEY_LOW               %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_COLOR_KEY_LOW ) ));
	BDBG_ERR(( "SRC_COLOR_KEY_MASK              %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_COLOR_KEY_MASK ) ));
	BDBG_ERR(( "SRC_COLOR_KEY_REPLACEMENT       %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_COLOR_KEY_REPLACEMENT ) ));
	BDBG_ERR(( "SRC_COLOR_KEY_REPLACEMENT_MASK  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_COLOR_KEY_REPLACEMENT_MASK ) ));
	BDBG_ERR(( "DEST_COLOR_KEY_HIGH             %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_COLOR_KEY_HIGH ) ));
	BDBG_ERR(( "DEST_COLOR_KEY_LOW              %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_COLOR_KEY_LOW ) ));
	BDBG_ERR(( "DEST_COLOR_KEY_MASK             %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_COLOR_KEY_MASK ) ));
	BDBG_ERR(( "DEST_COLOR_KEY_REPLACEMENT      %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_COLOR_KEY_REPLACEMENT ) ));
	BDBG_ERR(( "DEST_COLOR_KEY_REPLACEMENT_MASK %08x", BGRC_P_ReadReg32( hGrc->hRegister, DEST_COLOR_KEY_REPLACEMENT_MASK ) ));
	BDBG_ERR(( "SRC_CM_C00_C01                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C00_C01 ) ));
	BDBG_ERR(( "SRC_CM_C02_C03                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C02_C03 ) ));
	BDBG_ERR(( "SRC_CM_C04                      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C04 ) ));
	BDBG_ERR(( "SRC_CM_C10_C11                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C10_C11 ) ));
	BDBG_ERR(( "SRC_CM_C12_C13                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C12_C13 ) ));
	BDBG_ERR(( "SRC_CM_C14                      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C14 ) ));
	BDBG_ERR(( "SRC_CM_C20_C21                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C20_C21 ) ));
	BDBG_ERR(( "SRC_CM_C22_C23                  %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C22_C23 ) ));
	BDBG_ERR(( "SRC_CM_C24                      %08x", BGRC_P_ReadReg32( hGrc->hRegister, SRC_CM_C24 ) ));
	BDBG_ERR(( "" ));
}

/***************************************************************************/
#define M2MC_FT_Default       (0L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_WRGB1555      (1L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_WRGB1565      (2L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_Palette       (3L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_YCbCr422      (4L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_Alpha         (5L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_YCbCr422_10   (6L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))
#define M2MC_FT_YCbCr444_10   (8L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT))

#ifdef BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK
#define M2MC_FT_YCbCr420      (BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_1_FORMAT_TYPE_MASK) | BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK))
#else
#define M2MC_FT_YCbCr420      BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_MASK)
#endif

#define M2MC_DISABLE_SHIFT    BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_3_CH0_DISABLE_SHIFT)
#define M2MC_CHANNEL_MASK     ((1L << BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_SHIFT)) - 1)

/***************************************************************************/
static uint32_t s_BGRC_PACKET_P_DevicePixelFormats[][3] = 
{
	{ 0x00000000, 0x00000000, 0x00000000 }, /* Unknown */
	{ 0x00000565 | M2MC_FT_Default,     0x000B0500, 0x00000008 << M2MC_DISABLE_SHIFT }, /* R5_G6_B5 */
	{ 0x00000565 | M2MC_FT_Default,     0x0000050B, 0x00000008 << M2MC_DISABLE_SHIFT }, /* B5_G6_R5 */
	{ 0x00001555 | M2MC_FT_Default,     0x0F0A0500, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A1_R5_G5_B5 */
	{ 0x00001555 | M2MC_FT_Default,     0x0F0A0500, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X1_R5_G5_B5 */
	{ 0x00001555 | M2MC_FT_Default,     0x0F00050A, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A1_B5_G5_R5 */
	{ 0x00001555 | M2MC_FT_Default,     0x0F00050A, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X1_B5_G5_R5 */
	{ 0x00001555 | M2MC_FT_Default,     0x000B0601, 0x00000000 << M2MC_DISABLE_SHIFT }, /* R5_G5_B5_A1 */
	{ 0x00001555 | M2MC_FT_Default,     0x000B0601, 0x00000008 << M2MC_DISABLE_SHIFT }, /* R5_G5_B5_X1 */
	{ 0x00001555 | M2MC_FT_Default,     0x0001060B, 0x00000000 << M2MC_DISABLE_SHIFT }, /* B5_G5_R5_A1 */
	{ 0x00001555 | M2MC_FT_Default,     0x0001060B, 0x00000008 << M2MC_DISABLE_SHIFT }, /* B5_G5_R5_X1 */
	{ 0x00004444 | M2MC_FT_Default,     0x0C080400, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A4_R4_G4_B4 */
	{ 0x00004444 | M2MC_FT_Default,     0x0C080400, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X4_R4_G4_B4 */
	{ 0x00004444 | M2MC_FT_Default,     0x0C000408, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A4_B4_G4_R4 */
	{ 0x00004444 | M2MC_FT_Default,     0x0C000408, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X4_B4_G4_R4 */
	{ 0x00004444 | M2MC_FT_Default,     0x000C0804, 0x00000000 << M2MC_DISABLE_SHIFT }, /* R4_G4_B4_A4 */
	{ 0x00004444 | M2MC_FT_Default,     0x000C0804, 0x00000008 << M2MC_DISABLE_SHIFT }, /* R4_G4_B4_X4 */
	{ 0x00004444 | M2MC_FT_Default,     0x0004080C, 0x00000000 << M2MC_DISABLE_SHIFT }, /* B4_G4_R4_A4 */
	{ 0x00004444 | M2MC_FT_Default,     0x0004080C, 0x00000008 << M2MC_DISABLE_SHIFT }, /* B4_G4_R4_X4 */
	{ 0x00008888 | M2MC_FT_Default,     0x18100800, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A8_R8_G8_B8 */
	{ 0x00008888 | M2MC_FT_Default,     0x18100800, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X8_R8_G8_B8 */
	{ 0x00008888 | M2MC_FT_Default,     0x18000810, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A8_B8_G8_R8 */
	{ 0x00008888 | M2MC_FT_Default,     0x18000810, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X8_B8_G8_R8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00181008, 0x00000000 << M2MC_DISABLE_SHIFT }, /* R8_G8_B8_A8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00181008, 0x00000008 << M2MC_DISABLE_SHIFT }, /* R8_G8_B8_X8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00081018, 0x00000000 << M2MC_DISABLE_SHIFT }, /* B8_G8_R8_A8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00081018, 0x00000008 << M2MC_DISABLE_SHIFT }, /* B8_G8_R8_X8 */
	{ 0x00008000 | M2MC_FT_Alpha,       0x00000000, 0x00000007 << M2MC_DISABLE_SHIFT }, /* A8 */
	{ 0x00004000 | M2MC_FT_Alpha,       0x00000000, 0x00000007 << M2MC_DISABLE_SHIFT }, /* A4 */
	{ 0x00002000 | M2MC_FT_Alpha,       0x00000000, 0x00000007 << M2MC_DISABLE_SHIFT }, /* A2 */
	{ 0x00001000 | M2MC_FT_Alpha,       0x00000000, 0x00000007 << M2MC_DISABLE_SHIFT }, /* A1 */
	{ 0x00001000 | M2MC_FT_WRGB1565,    0x00000000, 0x00000007 << M2MC_DISABLE_SHIFT }, /* W1 */
	{ 0x00008008 | M2MC_FT_Palette,     0x08000000, 0x00000006 << M2MC_DISABLE_SHIFT }, /* A8_P8 */
	{ 0x00000008 | M2MC_FT_Palette,     0x00000000, 0x0000000E << M2MC_DISABLE_SHIFT }, /* P8 */
	{ 0x00000004 | M2MC_FT_Palette,     0x00000000, 0x0000000E << M2MC_DISABLE_SHIFT }, /* P4 */
	{ 0x00000002 | M2MC_FT_Palette,     0x00000000, 0x0000000E << M2MC_DISABLE_SHIFT }, /* P2 */
	{ 0x00000001 | M2MC_FT_Palette,     0x00000000, 0x0000000E << M2MC_DISABLE_SHIFT }, /* P1 */
	{ 0x00000808 | M2MC_FT_Palette,     0x00080000, 0x0000000A << M2MC_DISABLE_SHIFT }, /* Y8_P8 */
	{ 0x00000800 | M2MC_FT_YCbCr420,    0x00000000, 0x0000000B << M2MC_DISABLE_SHIFT }, /* Y8 */
	{ 0x00008800 | M2MC_FT_YCbCr420,    0x08000000, 0x00000003 << M2MC_DISABLE_SHIFT }, /* A8_Y8 */
	{ 0x00000088 | M2MC_FT_YCbCr420,    0x00000800, 0x0000000C << M2MC_DISABLE_SHIFT }, /* Cb8_Cr8 */
	{ 0x00000088 | M2MC_FT_YCbCr420,    0x00000008, 0x0000000C << M2MC_DISABLE_SHIFT }, /* Cr8_Cb8 */
	{ 0x00000080 | M2MC_FT_Default,     0x00000000, 0x0000000D << M2MC_DISABLE_SHIFT }, /* Cb8 */
	{ 0x00000008 | M2MC_FT_Default,     0x00000000, 0x0000000E << M2MC_DISABLE_SHIFT }, /* Cr8 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x18081000, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y08_Cb8_Y18_Cr8 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x18080010, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y08_Cr8_Y18_Cb8 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x08181000, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y18_Cb8_Y08_Cr8 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x08180010, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y18_Cr8_Y08_Cb8 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x10001808, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cb8_Y08_Cr8_Y18 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x00101808, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cb8_Y18_Cr8_Y08 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x00100818, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cr8_Y18_Cb8_Y08 */
	{ 0x00008888 | M2MC_FT_YCbCr422,    0x10000818, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cr8_Y08_Cb8_Y18 */
	{ 0x00002AAA | M2MC_FT_YCbCr444_10, 0x1E0A0014, 0x00000008 << M2MC_DISABLE_SHIFT }, /* X2_Cr10_Y10_Cb10 */
	{ 0x00008888 | M2MC_FT_Default,     0x18100800, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A8_Y8_Cb8_Cr8 */
	{ 0x00008888 | M2MC_FT_Default,     0x18000810, 0x00000000 << M2MC_DISABLE_SHIFT }, /* A8_Cr8_Cb8_Y8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00081018, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cr8_Cb8_Y8_A8 */
	{ 0x00008888 | M2MC_FT_Default,     0x00181008, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y8_Cb8_Cr8_A8 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x1E0A1400, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y010_Cb10_Y110_Cr10 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x1E0A0014, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y010_Cr10_Y110_Cb10 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x0A1E1400, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y110_Cb10_Y010_Cr10 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x0A1E0014, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Y110_Cr10_Y010_Cb10 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x14001E0A, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cb10_Y010_Cr10_Y110 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x00141E0A, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cb10_Y110_Cr10_Y010 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x00140A1E, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cr10_Y110_Cb10_Y010 */
	{ 0x0000AAAA | M2MC_FT_YCbCr422_10, 0x14000A1E, 0x00000000 << M2MC_DISABLE_SHIFT }, /* Cr10_Y010_Cb10_Y110 */
	{ 0x00000008 | M2MC_FT_Default,     0x00000000, 0x00000000 << M2MC_DISABLE_SHIFT }, /* L8 */
	{ 0x00004004 | M2MC_FT_Default,     0x00000004, 0x00000000 << M2MC_DISABLE_SHIFT }, /* L4_A4 */
	{ 0x00008008 | M2MC_FT_Default,     0x00000008, 0x00000000 << M2MC_DISABLE_SHIFT }, /* L8_A8 */
	{ 0x00006055 | M2MC_FT_Default,     0x00000B06, 0x00000000 << M2MC_DISABLE_SHIFT }, /* L15_L05_A6 */
	{ 0x00000000, 0x00000000, 0x00000000 }, /* Max */
};

/***************************************************************************/
#define BGRC_PACKET_P_DEVICE_GROUP_HEADER_FULL ( \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_OUTPUT_FEEDER_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLEND_PARAM_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_ROP_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_COLOR_KEY_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_COLOR_KEY_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_COEF_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_COLOR_MATRIX_GRP_CNTRL_MASK | \
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_CLUT_GRP_CNTRL_MASK)

/***************************************************************************/
static uint32_t s_BGRC_PACKET_P_DeviceGroupHeaderMasks[] = 
{
	0,                                                              /* EndOfBuffer */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK,       /* SourceFeeder */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK,       /* SourceFeeders */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK,       /* SourceColor */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK,       /* SourceNone */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK,       /* SourceControl */
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK,       /* DestinationFeeder */
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK,       /* DestinationColor */
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK,       /* DestinationNone */
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK,       /* DestinationControl */
	BCHP_M2MC_LIST_PACKET_HEADER_1_OUTPUT_FEEDER_GRP_CNTRL_MASK,    /* OutputFeeder */
	BCHP_M2MC_LIST_PACKET_HEADER_1_OUTPUT_FEEDER_GRP_CNTRL_MASK,    /* OutputControl */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLEND_PARAM_GRP_CNTRL_MASK,      /* Blend */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLEND_PARAM_GRP_CNTRL_MASK,      /* BlendColor */
	BCHP_M2MC_LIST_PACKET_HEADER_1_ROP_GRP_CNTRL_MASK,              /* Rop */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_COLOR_KEY_GRP_CNTRL_MASK,    /* SourceColorkey */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* SourceColorkeyEnable */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLEND_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_DST_COLOR_KEY_GRP_CNTRL_MASK,    /* DestinationColorkey */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* DestinationColorkeyEnable */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLEND_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_COEF_GRP_CNTRL_MASK,       /* Filter */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,      /* FilterEnable */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* FilterControl */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_COLOR_MATRIX_GRP_CNTRL_MASK, /* SourceColorMatrix */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* SourceColorMatrixEnable */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SRC_CLUT_GRP_CNTRL_MASK,         /* SourcePalette */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,      /* AlphaPremultiply */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* AlphaDemultiply */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* DestAlphaPremultiply */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* Mirror */
	0,                                                              /* FixedScale */
	0,                                                              /* DestripeFixedScale */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* FillBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* CopyBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK,             /* BlendBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* ScaleBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* ScaleBlendBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* UpdateScaleBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,
	BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK |            /* DestripeBlit */
	BCHP_M2MC_LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK,
	BGRC_PACKET_P_DEVICE_GROUP_HEADER_FULL,                         /* Reset */
	0,                                                              /* Save */
	0                                                               /* Restore */
};

/***************************************************************************/
#if defined(BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01)
#else
#define BCHP_M2MC_HORIZ_FIR_0_COEFF_PHASE0_01   BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_01
#endif

/***************************************************************************/
static uint32_t s_BGRC_PACKET_P_DeviceGroupSizes[] = 
{
	0,
	sizeof (uint32_t),
	0,
	0,
	BGRC_M2MC(SRC_CM_C34) - BGRC_M2MC(SRC_CM_C00_C01) + sizeof (uint32_t),
	BGRC_M2MC(SRC_CM_C00_C01) - BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01),
	BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01) - BGRC_M2MC(DEST_COLOR_KEY_HIGH),
	BGRC_M2MC(DEST_COLOR_KEY_HIGH) - BGRC_M2MC(SRC_COLOR_KEY_HIGH),
	BGRC_M2MC(SRC_COLOR_KEY_HIGH) - BGRC_M2MC(ROP_OPERATION),
	BGRC_M2MC(ROP_OPERATION) - BGRC_M2MC(BLEND_COLOR_OP),
	BGRC_M2MC(BLEND_COLOR_OP) - BGRC_M2MC(SCALER_CTRL),
	BGRC_M2MC(SCALER_CTRL) - BGRC_M2MC(BLIT_HEADER),
	BGRC_M2MC(BLIT_HEADER) - BGRC_M2MC(OUTPUT_FEEDER_ENABLE),
	BGRC_M2MC(OUTPUT_FEEDER_ENABLE) - BGRC_M2MC(DEST_FEEDER_ENABLE),
	BGRC_M2MC(DEST_FEEDER_ENABLE) - BGRC_M2MC(SRC_FEEDER_ENABLE)
};

/***************************************************************************/
static uint32_t s_BGRC_PACKET_P_DeviceRegisterOffsets[] = 
{
	0,
	BGRC_M2MC(SRC_CLUT_ENTRY_i_ARRAY_BASE) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	0,
	0,
	BGRC_M2MC(SRC_CM_C00_C01) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(DEST_COLOR_KEY_HIGH) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(SRC_COLOR_KEY_HIGH) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(ROP_OPERATION) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(BLEND_COLOR_OP) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(SCALER_CTRL) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(BLIT_HEADER) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(OUTPUT_FEEDER_ENABLE) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(DEST_FEEDER_ENABLE) - BGRC_M2MC(SRC_FEEDER_ENABLE),
	BGRC_M2MC(SRC_FEEDER_ENABLE) - BGRC_M2MC(SRC_FEEDER_ENABLE)
};

/***************************************************************************/
static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_PointSample = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0000, 0x0100 }, { 0x0000, 0x0000, 0x0100 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x000, 0x00000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_Bilinear = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0000, 0x0100 }, { 0x0000, 0x0000, 0x0080 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0200, 0x0E00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0C00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0600, 0x0A00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0800, 0x0800, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0A00, 0x0600, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0C00, 0x0400, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0E00, 0x0200, 0x0000, 0x0000, 0x0000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_Anisotropic[] = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { { 0x0000, 0x0000, 0x0100 }, { 0x0000, 0x0000, 0x0080 } } },     /* 1 to 1 */
	{ { { 0x0000, 0x0000, 0x0100 }, { 0x0000, 0x0000, 0x0080 } } },     /* 2 to 1 */
	{ { { 0x0000, 0x0055, 0x0056 }, { 0x0000, 0x002b, 0x0055 } } },     /* 3 to 1 */
	{ { { 0x0000, 0x0055, 0x0056 }, { 0x0000, 0x002b, 0x0055 } } },     /* 4 to 1 */
	{ { { 0x0033, 0x0033, 0x0034 }, { 0x001a, 0x0033, 0x0033 } } },     /* 5 to 1 */
	{ { { 0x0033, 0x0033, 0x0034 }, { 0x002a, 0x002b, 0x002b } } },     /* 6 to 1 */
	{ { { 0x0033, 0x0033, 0x0034 }, { 0x002a, 0x002b, 0x002b } } },     /* 7 to 1 */
	{ { { 0x0033, 0x0033, 0x0034 }, { 0x002a, 0x002b, 0x002b } } }      /* 8 to 1 */
#else
{ {
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000, 0x0000 }, /* 1 to 1 */
	{ 0x0000, 0x0000, 0x0000, 0x0200, 0x0E00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0C00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0600, 0x0A00, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0800, 0x0800, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0A00, 0x0600, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0C00, 0x0400, 0x0000, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0E00, 0x0200, 0x0000, 0x0000, 0x0000 }
} },{ {
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0800, 0x0800, 0x0000, 0x0000 }, /* 2 to 1 */
	{ 0x0000, 0x0000, 0x0000, 0x0100, 0x0800, 0x0700, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0200, 0x0800, 0x0600, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0300, 0x0800, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0800, 0x0300, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0600, 0x0800, 0x0200, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0700, 0x0800, 0x0100, 0x0000, 0x0000 }
} },{ {
	{ 0x0000, 0x0000, 0x0000, 0x0554, 0x0558, 0x0554, 0x0000, 0x0000 }, /* 3 to 1 */
	{ 0x0000, 0x0000, 0x00AC, 0x0554, 0x0554, 0x04AC, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0154, 0x0554, 0x0558, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0200, 0x0554, 0x0558, 0x0354, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x02AC, 0x0554, 0x0554, 0x02AC, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0354, 0x0554, 0x0558, 0x0200, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0400, 0x0554, 0x0558, 0x0154, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x04AC, 0x0554, 0x0554, 0x00AC, 0x0000, 0x0000 }
} },{ {
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0400, 0x0400, 0x0400, 0x0000 }, /* 4 to 1 */
	{ 0x0000, 0x0000, 0x0080, 0x0400, 0x0400, 0x0400, 0x0380, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0100, 0x0400, 0x0400, 0x0400, 0x0300, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0180, 0x0400, 0x0400, 0x0400, 0x0280, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0200, 0x0400, 0x0400, 0x0400, 0x0200, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0280, 0x0400, 0x0400, 0x0400, 0x0180, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0300, 0x0400, 0x0400, 0x0400, 0x0100, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0380, 0x0400, 0x0400, 0x0400, 0x0080, 0x0000 }
} },{ {
	{ 0x0000, 0x0000, 0x0334, 0x0334, 0x0334, 0x0334, 0x0330, 0x0000 }, /* 5 to 1 */
	{ 0x0000, 0x0068, 0x0334, 0x0334, 0x0334, 0x0334, 0x02C8, 0x0000 }, 
	{ 0x0000, 0x00CC, 0x0334, 0x0334, 0x0334, 0x0334, 0x0264, 0x0000 }, 
	{ 0x0000, 0x0134, 0x0334, 0x0334, 0x0334, 0x0334, 0x01FC, 0x0000 }, 
	{ 0x0000, 0x0198, 0x0334, 0x0334, 0x0334, 0x0334, 0x0198, 0x0000 }, 
	{ 0x0000, 0x0200, 0x0334, 0x0334, 0x0334, 0x0334, 0x0134, 0x0000 }, 
	{ 0x0000, 0x0264, 0x0334, 0x0334, 0x0334, 0x0334, 0x00CC, 0x0000 }, 
	{ 0x0000, 0x02C8, 0x0334, 0x0334, 0x0334, 0x0334, 0x0068, 0x0000 }
} },{ {
	{ 0x0000, 0x0000, 0x02A8, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02A8 }, /* 6 to 1 */
	{ 0x0000, 0x0054, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x0250 }, 
	{ 0x0000, 0x00A8, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x01FC }, 
	{ 0x0000, 0x00FC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x01A8 }, 
	{ 0x0000, 0x0154, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x0150 }, 
	{ 0x0000, 0x01A8, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x00FC }, 
	{ 0x0000, 0x01FC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x00A8 }, 
	{ 0x0000, 0x0250, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x02AC, 0x0054 }
} },{ {
	{ 0x0000, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x0248 }, /* 7 to 1 */
	{ 0x0048, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x0200 }, 
	{ 0x0094, 0x0248, 0x0248, 0x0248, 0x024C, 0x0248, 0x0248, 0x01B8 }, 
	{ 0x00DC, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x016C }, 
	{ 0x0124, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x0124 }, 
	{ 0x016C, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x00DC }, 
	{ 0x01B8, 0x0248, 0x0248, 0x0248, 0x024C, 0x0248, 0x0248, 0x0094 }, 
	{ 0x0200, 0x0248, 0x0248, 0x024C, 0x024C, 0x0248, 0x0248, 0x0048 }
} },{ {
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, /* 8 to 1 */
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }, 
	{ 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200, 0x0200 }
} }
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_Sharp[] = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { { 0x000f, 0x0041, 0x0060 }, { 0x0006, 0x0024, 0x0056 } } },     /* 8 to 1 */
	{ { { 0x000b, 0x0040, 0x006a }, { 0x0004, 0x0020, 0x005c } } },     /* 8 to 2 */
	{ { { 0x0006, 0x003f, 0x0076 }, { 0x0000, 0x001a, 0x0066 } } },     /* 8 to 3 */
	{ { { 0x0000, 0x003b, 0x008a }, {-0x0002, 0x0010, 0x0072 } } },     /* 8 to 4 */
	{ { {-0x0005, 0x0033, 0x00a4 }, {-0x0003, 0x0003, 0x0080 } } },     /* 8 to 5 */
	{ { {-0x0007, 0x0026, 0x00c2 }, {-0x0001,-0x0009, 0x008a } } },     /* 8 to 6 */
	{ { {-0x0004, 0x0014, 0x00e0 }, { 0x0002,-0x0012, 0x0090 } } },     /* 8 to 7 */
	{ { {-0x0001, 0x0000, 0x0100 }, { 0x0002,-0x0015, 0x0093 } } },     /* 8 to 8 */
	{ { { 0x0000,-0x0001, 0x0102 }, { 0x0002,-0x0015, 0x0093 } } },     /* 8 to 9 */
	{ { {-0x0006, 0x001f, 0x00ce }, { 0x0000,-0x000d, 0x008d } } }      /* 1 to N */
#else
{ {
	{ 0x0000, 0x009C, 0x01E8, 0x036C, 0x041C, 0x036C, 0x01E8, 0x009C }, /* 8 to 1 */
	{ 0x0038, 0x00B8, 0x0214, 0x0388, 0x0408, 0x0338, 0x01B0, 0x0080 }, 
	{ 0x0038, 0x00DC, 0x0248, 0x03A8, 0x0400, 0x030C, 0x0180, 0x006C }, 
	{ 0x0040, 0x0100, 0x027C, 0x03C8, 0x03F4, 0x02DC, 0x0154, 0x0058 }, 
	{ 0x004C, 0x0128, 0x02AC, 0x03E0, 0x03E0, 0x02AC, 0x0128, 0x004C }, 
	{ 0x0058, 0x0154, 0x02DC, 0x03F4, 0x03C8, 0x027C, 0x0100, 0x0040 }, 
	{ 0x006C, 0x0180, 0x030C, 0x0400, 0x03A8, 0x0248, 0x00DC, 0x0038 }, 
	{ 0x0080, 0x01B0, 0x0338, 0x0408, 0x0388, 0x0214, 0x00B8, 0x0038 }
} },{ {
	{ 0x0000, 0x0048, 0x0198, 0x03B8, 0x04D4, 0x03B8, 0x0198, 0x0048 }, /* 8 to 2 */
	{ 0x0004, 0x0060, 0x01D4, 0x03F4, 0x04CC, 0x0378, 0x015C, 0x0034 }, 
	{ 0x0008, 0x007C, 0x0218, 0x042C, 0x04C0, 0x0334, 0x0124, 0x0024 }, 
	{ 0x000C, 0x009C, 0x0260, 0x045C, 0x04A8, 0x02EC, 0x00F0, 0x0018 }, 
	{ 0x0010, 0x00C4, 0x02A4, 0x0488, 0x0488, 0x02A4, 0x00C4, 0x0010 }, 
	{ 0x0018, 0x00F0, 0x02EC, 0x04A8, 0x045C, 0x0260, 0x009C, 0x000C }, 
	{ 0x0024, 0x0124, 0x0334, 0x04C0, 0x042C, 0x0218, 0x007C, 0x0008 }, 
	{ 0x0034, 0x015C, 0x0378, 0x04CC, 0x03F4, 0x01D4, 0x0060, 0x0004 }
} },{ {
	{ 0x0000, 0x3FE4, 0x00F0, 0x041C, 0x0620, 0x041C, 0x00F0, 0x3FE4 }, /* 8 to 3 */
	{ 0x3FE8, 0x3FE8, 0x0140, 0x048C, 0x0620, 0x03B4, 0x00B0, 0x3FE0 }, 
	{ 0x3FE8, 0x3FF8, 0x0198, 0x04F0, 0x0604, 0x0340, 0x0074, 0x3FE0 }, 
	{ 0x3FE4, 0x0004, 0x01FC, 0x054C, 0x05D4, 0x02D0, 0x0048, 0x3FE0 }, 
	{ 0x3FE4, 0x0020, 0x0264, 0x0598, 0x0598, 0x0264, 0x0020, 0x3FE4 }, 
	{ 0x3FE0, 0x0048, 0x02D0, 0x05D4, 0x054C, 0x01FC, 0x0004, 0x3FE4 }, 
	{ 0x3FE0, 0x0074, 0x0340, 0x0604, 0x04F0, 0x0198, 0x3FF8, 0x3FE8 }, 
	{ 0x3FE0, 0x00B0, 0x03B4, 0x0620, 0x048C, 0x0140, 0x3FE8, 0x3FE8 }
} },{ {
	{ 0x0000, 0x3FB0, 0x0000, 0x0454, 0x07F4, 0x0454, 0x0000, 0x3FB0 }, /* 8 to 4 */
	{ 0x3FFC, 0x3FA0, 0x004C, 0x050C, 0x07E4, 0x03A0, 0x3FC8, 0x3FC0 }, 
	{ 0x3FF8, 0x3F90, 0x00AC, 0x05BC, 0x07B0, 0x02F0, 0x3FA4, 0x3FD0 }, 
	{ 0x3FF0, 0x3F88, 0x0120, 0x065C, 0x075C, 0x0244, 0x3F90, 0x3FDC }, 
	{ 0x3FE8, 0x3F84, 0x01AC, 0x06E8, 0x06E8, 0x01AC, 0x3F84, 0x3FE8 }, 
	{ 0x3FDC, 0x3F90, 0x0244, 0x075C, 0x065C, 0x0120, 0x3F88, 0x3FF0 }, 
	{ 0x3FD0, 0x3FA4, 0x02F0, 0x07B0, 0x05BC, 0x00AC, 0x3F90, 0x3FF8 }, 
	{ 0x3FC0, 0x3FC8, 0x03A0, 0x07E4, 0x050C, 0x004C, 0x3FA0, 0x3FFC }
} },{ {
	{ 0x0000, 0x3FE4, 0x3F14, 0x0408, 0x0A04, 0x0408, 0x3F14, 0x3FE4 }, /* 8 to 5 */
	{ 0x0018, 0x3FC8, 0x3F38, 0x0518, 0x09D4, 0x02F8, 0x3F0C, 0x3FF8 }, 
	{ 0x0018, 0x3FA4, 0x3F80, 0x062C, 0x0978, 0x0204, 0x3F18, 0x0004 }, 
	{ 0x0018, 0x3F7C, 0x3FE8, 0x0734, 0x08E4, 0x012C, 0x3F34, 0x000C }, 
	{ 0x0014, 0x3F58, 0x0074, 0x0820, 0x0820, 0x0074, 0x3F58, 0x0014 }, 
	{ 0x000C, 0x3F34, 0x012C, 0x08E4, 0x0734, 0x3FE8, 0x3F7C, 0x0018 }, 
	{ 0x0004, 0x3F18, 0x0204, 0x0978, 0x062C, 0x3F80, 0x3FA4, 0x0018 }, 
	{ 0x3FF8, 0x3F0C, 0x02F8, 0x09D4, 0x0518, 0x3F38, 0x3FC8, 0x0018 }
} },{ {
	{ 0x0000, 0x0038, 0x3EB4, 0x0314, 0x0C00, 0x0314, 0x3EB4, 0x0038 }, /* 8 to 6 */
	{ 0x0008, 0x002C, 0x3E8C, 0x048C, 0x0BC8, 0x01C0, 0x3EF0, 0x0038 }, 
	{ 0x0010, 0x0018, 0x3E88, 0x0618, 0x0B2C, 0x009C, 0x3F38, 0x0034 }, 
	{ 0x0018, 0x3FF4, 0x3EB4, 0x079C, 0x0A3C, 0x3FBC, 0x3F80, 0x002C }, 
	{ 0x0020, 0x3FC0, 0x3F18, 0x0908, 0x0908, 0x3F18, 0x3FC0, 0x0020 }, 
	{ 0x002C, 0x3F80, 0x3FBC, 0x0A3C, 0x079C, 0x3EB4, 0x3FF4, 0x0018 }, 
	{ 0x0034, 0x3F38, 0x009C, 0x0B2C, 0x0618, 0x3E88, 0x0018, 0x0010 }, 
	{ 0x0038, 0x3EF0, 0x01C0, 0x0BC8, 0x048C, 0x3E8C, 0x002C, 0x0008 }
} },{ {
	{ 0x0000, 0x004C, 0x3F14, 0x01A8, 0x0DEC, 0x01A8, 0x3F14, 0x004C }, /* 8 to 7 */
	{ 0x3FE8, 0x0064, 0x3EA0, 0x0378, 0x0DB4, 0x002C, 0x3F88, 0x0030 }, 
	{ 0x3FEC, 0x0074, 0x3E3C, 0x0580, 0x0CD0, 0x3F14, 0x3FE8, 0x0018 }, 
	{ 0x3FF0, 0x0074, 0x3E04, 0x0798, 0x0B68, 0x3E60, 0x0030, 0x0004 }, 
	{ 0x3FFC, 0x0060, 0x3E08, 0x099C, 0x099C, 0x3E08, 0x0060, 0x3FFC }, 
	{ 0x0004, 0x0030, 0x3E60, 0x0B68, 0x0798, 0x3E04, 0x0074, 0x3FF0 }, 
	{ 0x0018, 0x3FE8, 0x3F14, 0x0CD0, 0x0580, 0x3E3C, 0x0074, 0x3FEC }, 
	{ 0x0030, 0x3F88, 0x002C, 0x0DB4, 0x0378, 0x3EA0, 0x0064, 0x3FE8 }
} },{ {
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000, 0x0000 }, /* 8 to 8 */
	{ 0x3FF8, 0x0024, 0x3F6C, 0x01F8, 0x0F94, 0x3E94, 0x006C, 0x3FE8 }, 
	{ 0x3FEC, 0x0054, 0x3EC0, 0x0468, 0x0E50, 0x3DC0, 0x00AC, 0x3FDC }, 
	{ 0x3FE4, 0x0088, 0x3E1C, 0x071C, 0x0C50, 0x3D74, 0x00C0, 0x3FD8 }, 
	{ 0x3FDC, 0x00B0, 0x3DA0, 0x09D4, 0x09D4, 0x3DA0, 0x00B0, 0x3FDC }, 
	{ 0x3FD8, 0x00C0, 0x3D74, 0x0C50, 0x071C, 0x3E1C, 0x0088, 0x3FE4 }, 
	{ 0x3FDC, 0x00AC, 0x3DC0, 0x0E50, 0x0468, 0x3EC0, 0x0054, 0x3FEC }, 
	{ 0x3FE8, 0x006C, 0x3E94, 0x0F94, 0x01F8, 0x3F6C, 0x0024, 0x3FF8 }
} },{ {
	{ 0x0000, 0x3FB4, 0x00EC, 0x3E54, 0x1214, 0x3E54, 0x00EC, 0x3FB4 }, /* 8 to 9 */
	{ 0x0018, 0x3FC0, 0x0084, 0x0040, 0x1158, 0x3D40, 0x010C, 0x3FC0 }, 
	{ 0x0010, 0x3FE0, 0x3FD4, 0x02EC, 0x0F98, 0x3CF4, 0x00F4, 0x3FD0 }, 
	{ 0x0008, 0x0014, 0x3EF4, 0x0628, 0x0CE4, 0x3D48, 0x00B0, 0x3FE8 }, 
	{ 0x3FFC, 0x0060, 0x3E08, 0x099C, 0x099C, 0x3E08, 0x0060, 0x3FFC }, 
	{ 0x3FE8, 0x00B0, 0x3D48, 0x0CE4, 0x0628, 0x3EF4, 0x0014, 0x0008 }, 
	{ 0x3FD0, 0x00F4, 0x3CF4, 0x0F98, 0x02EC, 0x3FD4, 0x3FE0, 0x0010 }, 
	{ 0x3FC0, 0x010C, 0x3D40, 0x1158, 0x0040, 0x0084, 0x3FC0, 0x0018 }
} },{ {
	{ 0x0000, 0x0030, 0x3EE0, 0x0320, 0x0CE0, 0x01A0, 0x3F20, 0x0030 }, /* 1 to N */
	{ 0x0000, 0x0030, 0x3EA0, 0x04D0, 0x0C60, 0x0070, 0x3F70, 0x0020 }, 
	{ 0x0000, 0x0030, 0x3E80, 0x06A0, 0x0B60, 0x3F80, 0x3FB0, 0x0020 }, 
	{ 0x0010, 0x0010, 0x3E90, 0x0860, 0x0A10, 0x3EE0, 0x3FF0, 0x0010 }, 
	{ 0x0010, 0x3FF0, 0x3EE0, 0x0A10, 0x0860, 0x3E90, 0x0010, 0x0010 }, 
	{ 0x0020, 0x3FB0, 0x3F80, 0x0B60, 0x06A0, 0x3E80, 0x0030, 0x0000 }, 
	{ 0x0020, 0x3F70, 0x0070, 0x0C60, 0x04D0, 0x3EA0, 0x0030, 0x0000 }, 
	{ 0x0030, 0x3F20, 0x01A0, 0x0CE0, 0x0320, 0x3EE0, 0x0030, 0x0000 }
} }
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_Sharper = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0000, 0x0100 }, { 0x0003,-0x0015, 0x0092 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x1000, 0x0000, 0x0000, 0x0000 }, 
	{ 0x3FD0, 0x0060, 0x3F30, 0x0220, 0x0FC0, 0x3E60, 0x00B0, 0x3FB0 }, 
	{ 0x3FA0, 0x00D0, 0x3E50, 0x04B0, 0x0EA0, 0x3D50, 0x0130, 0x3F70 }, 
	{ 0x3F70, 0x0130, 0x3D80, 0x0780, 0x0CC0, 0x3CE0, 0x0170, 0x3F50 }, 
	{ 0x3F60, 0x0170, 0x3CF0, 0x0A40, 0x0A40, 0x3CF0, 0x0170, 0x3F60 }, 
	{ 0x3F50, 0x0170, 0x3CE0, 0x0CC0, 0x0780, 0x3D80, 0x0130, 0x3F70 }, 
	{ 0x3F70, 0x0130, 0x3D50, 0x0EA0, 0x04B0, 0x3E50, 0x00D0, 0x3FA0 }, 
	{ 0x3FB0, 0x00B0, 0x3E60, 0x0FC0, 0x0220, 0x3F30, 0x0060, 0x3FD0 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_Blurry = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0055, 0x0056 }, { 0x0000, 0x002b, 0x0055 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0554, 0x0558, 0x0554, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x00AC, 0x0554, 0x0554, 0x04AC, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0154, 0x0554, 0x0558, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0200, 0x0554, 0x0558, 0x0354, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x02AC, 0x0554, 0x0554, 0x02AC, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0354, 0x0554, 0x0558, 0x0200, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0400, 0x0554, 0x0558, 0x0154, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x04AC, 0x0554, 0x0554, 0x00AC, 0x0000, 0x0000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_AntiFlutter = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0040, 0x0080 }, { 0x0000, 0x0000, 0x0080 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0400, 0x0800, 0x0400, 0x0000, 0x0000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_AntiFlutterScale = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0040, 0x0080 }, { 0x0000, 0x0018, 0x0068 } }
#else
{
	{ 0x0000, 0x0000, 0x0030, 0x04C0, 0x07B0, 0x0360, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0090, 0x0560, 0x0760, 0x02B0, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x00F0, 0x0600, 0x0710, 0x0200, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0178, 0x0688, 0x0688, 0x0170, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0200, 0x0710, 0x0600, 0x00F0, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x02B0, 0x0760, 0x0560, 0x0090, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0360, 0x07B0, 0x04C0, 0x0030, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x01C8, 0x0638, 0x0638, 0x01C8, 0x0000, 0x0000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_AntiFlutterBlurry = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0050, 0x0060 }, { 0x0000, 0x0020, 0x0060 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0500, 0x0600, 0x0500, 0x0000, 0x0000 }
}
#endif
};

static BM2MC_PACKET_FilterCoeffs s_BGRC_PACKET_P_DeviceFilter_AntiFlutterSharp = 
{
#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
	{ { 0x0000, 0x0028, 0x00b0 }, { 0x0000, 0x0000, 0x0080 } }
#else
{
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }, 
	{ 0x0000, 0x0000, 0x0000, 0x0280, 0x0B00, 0x0280, 0x0000, 0x0000 }
}
#endif
};

/***************************************************************************/
void BGRC_PACKET_P_Isr( void *pvParam1, int iParam2 )
{
	BGRC_Handle hGrc = (BGRC_Handle) pvParam1;
	BSTD_UNUSED(iParam2);
	hGrc->advance_interrupt = false;
	(*hGrc->callback_isr)( hGrc, hGrc->callback_data );
}

/***************************************************************************/
#define BGRC_PACKET_P_OUT_OF_FIFO_MEMORY( alloc_offset, m2mc_offset ) \
	(((alloc_offset) <= (m2mc_offset)) && ((alloc_offset) + BGRC_PACKET_P_BLIT_GROUP_SIZE_MAX > (m2mc_offset)))

#define BGRC_PACKET_P_ALIGN_MEMORY( addr ) \
	(((uint32_t) (addr) + BGRC_PACKET_P_MEMORY_ALIGN_MASK) & (~BGRC_PACKET_P_MEMORY_ALIGN_MASK))

/***************************************************************************/
static bool BGRC_PACKET_P_AvailableFifoMemory(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext )
{
	/* align memory */
	hGrc->curr_fifo_addr = (uint32_t *) BGRC_PACKET_P_ALIGN_MEMORY(hGrc->curr_fifo_addr);
	hGrc->curr_fifo_offset = BGRC_PACKET_P_ALIGN_MEMORY(hGrc->curr_fifo_offset);

	/* check if memory is available */
	if( BGRC_PACKET_P_OUT_OF_FIFO_MEMORY(hGrc->curr_fifo_offset, hContext->m2mc_offset) )
	{
		hContext->m2mc_offset = BGRC_P_ReadReg32( hGrc->hRegister, LIST_CURR_PKT_ADDR );
		if( BGRC_PACKET_P_OUT_OF_FIFO_MEMORY(hGrc->curr_fifo_offset, hContext->m2mc_offset) )
			return false;
	}

	/* check if fifo memory is reaching the end */
	if( hGrc->curr_fifo_offset + BGRC_PACKET_P_BLIT_GROUP_SIZE_MAX > hGrc->fifo_base_offset + hGrc->fifo_total_size )
	{
		/* check if m2mc has been done a blit before */
		if( hContext->m2mc_offset == 0 )
			return false;
		
		/* check again if memory can be wrapped */
		if( BGRC_PACKET_P_OUT_OF_FIFO_MEMORY(hGrc->fifo_base_offset, hContext->m2mc_offset) )
		{
			hContext->m2mc_offset = BGRC_P_ReadReg32( hGrc->hRegister, LIST_CURR_PKT_ADDR );
			if( BGRC_PACKET_P_OUT_OF_FIFO_MEMORY(hGrc->fifo_base_offset, hContext->m2mc_offset) )
				return false;
		}

		/* reset to base */
		hGrc->end_flush_addr = hGrc->curr_fifo_addr;
		hGrc->curr_fifo_addr = hGrc->fifo_base_addr;
		hGrc->curr_fifo_offset = hGrc->fifo_base_offset;
	}

	return true;
}

/***************************************************************************/
static void BGRC_PACKET_P_FlushFifoMemory( BGRC_Handle hGrc )
{
#if ORIGINAL_FLUSH
	if( hGrc->curr_fifo_addr > hGrc->start_flush_addr )
	{
		/* flush unwrapped memory */
		int flush_size = (uint8_t *) hGrc->curr_fifo_addr -  (uint8_t *) hGrc->start_flush_addr;
		BMEM_FlushCache( hGrc->hMemory, hGrc->start_flush_addr, flush_size+4 );
	}
	else if( hGrc->curr_fifo_addr < hGrc->start_flush_addr )
	{
		/* flush wrapped memory */
		int flush_size =  hGrc->fifo_total_size - ((uint8_t *) hGrc->start_flush_addr - (uint8_t *)hGrc->fifo_cached_addr);
		BMEM_FlushCache( hGrc->hMemory, hGrc->start_flush_addr, flush_size+4);

		flush_size = (uint8_t *) hGrc->curr_fifo_addr - (uint8_t *) hGrc->fifo_cached_addr;
		BMEM_FlushCache( hGrc->hMemory, hGrc->fifo_cached_addr, flush_size+4);
	}
#else
	BMEM_FlushCache( hGrc->hMemory, hGrc->fifo_cached_addr, hGrc->fifo_total_size);
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_WriteFifoMemory(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	uint32_t group_header )
{
	uint32_t *curr_fifo_addr = hGrc->curr_fifo_addr;
	uint32_t curr_fifo_offset = hGrc->curr_fifo_offset;

	int groups = sizeof s_BGRC_PACKET_P_DeviceGroupSizes / sizeof (uint32_t);
	int ii, jj;

	/* store context's last fifo offset used */
	hContext->last_fifo_offset = hGrc->curr_fifo_offset;

	/* write group header */
	BGRC_PACKET_P_DEBUG_PRINT( "** REGISTERS          " );
	BGRC_PACKET_P_WRITE_DATA( *hGrc->curr_fifo_addr++, BCHP_M2MC_LIST_PACKET_HEADER_0_LAST_PKT_IND_MASK );
	BGRC_PACKET_P_WRITE_DATA( *hGrc->curr_fifo_addr++, group_header );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
	hGrc->curr_fifo_offset += 2 * sizeof (uint32_t);

	/* write groups */
	for( ii = groups - 1; ii >= 0; --ii )
	{
		uint32_t group_mask = group_header & (1 << ii);
		if( group_mask )
		{
			int count = s_BGRC_PACKET_P_DeviceGroupSizes[ii] / sizeof (uint32_t);
			int index = s_BGRC_PACKET_P_DeviceRegisterOffsets[ii] / sizeof (uint32_t);

			/* store last blit header to possibly set interrupt later */
			if( group_mask & BGRC_M2MC(LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK) )
			{
				hContext->last_blitheader_addr = hGrc->curr_fifo_addr;
				hContext->last_blitheader = BGRC_PACKET_P_GET_REG_FROM_INDEX(index);
			}

			/* write group registers */
			BGRC_PACKET_P_DEBUG_PRINT( "**                    " );
			for( jj = 0; jj < count; ++jj )
				BGRC_PACKET_P_WRITE_REG( *hGrc->curr_fifo_addr++, index + jj );
			BGRC_PACKET_P_DEBUG_PRINT( "\n" );

			hGrc->curr_fifo_offset += count * sizeof (uint32_t);
		}
	}

	/* write previous group's next offset */
	if( hGrc->prev_fifo_addr )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- NEXT               " );
		BGRC_PACKET_P_WRITE_DATA( *hGrc->prev_fifo_addr, curr_fifo_offset );
		BGRC_PACKET_P_DEBUG_PRINT( "\n" );
	}
	hGrc->prev_fifo_addr = curr_fifo_addr;
}

/***************************************************************************/
static void BGRC_PACKET_P_InitiateBlits(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext,
	uint32_t *fifo_addr,
	uint32_t fifo_offset,
	bool advance_interrupt,
	bool sync_interrupt )
{
	hContext->advance_interrupt = hContext->advance_interrupt || advance_interrupt;

	/* prepare for advance interrupt */
    BKNI_EnterCriticalSection();
	advance_interrupt = advance_interrupt && !hGrc->advance_interrupt;
	if( advance_interrupt )
	{
		/* start advance event */
		hGrc->advance_interrupt = true;

		/* enable blitheader interrupt field */
		if( hContext->last_blitheader_addr )
			BGRC_PACKET_P_WRITE_DATA( *hContext->last_blitheader_addr, hContext->last_blitheader | BGRC_M2MC(BLIT_HEADER_INTERRUPT_ENABLE_MASK) );
	}
	BKNI_LeaveCriticalSection();

	/* prepare for sync interrupt */
	if( sync_interrupt )
	{
		/* reset sync event */
		hContext->eSyncState = BGRC_PACKET_P_SyncState_eQueuedInHw;
	}

	/* flush memory */
#if ORIGINAL_FLUSH
	if( hGrc->fifo_base_addr == hGrc->fifo_cached_addr )
	{
		hGrc->start_flush_addr = fifo_addr;
		BGRC_PACKET_P_FlushFifoMemory( hGrc );
	}
#else
	BSTD_UNUSED(fifo_addr);
	BGRC_PACKET_P_FlushFifoMemory( hGrc );
#endif

	/* init blits */
	if( hGrc->last_fifo_addr )
	{
		/* write next offset from previous blits */
		BGRC_PACKET_P_DEBUG_PRINT( "** NEXT               " );
		BGRC_PACKET_P_WRITE_DATA( *hGrc->last_fifo_addr, fifo_offset );
		BGRC_PACKET_P_DEBUG_PRINT( "\n" );

#if ORIGINAL_FLUSH
		/* flush next offset from previous blits */
		if( hGrc->fifo_base_addr == hGrc->fifo_cached_addr )
			BMEM_FlushCache( hGrc->hMemory, hGrc->last_fifo_addr, sizeof (uint32_t) );
#else
        BGRC_PACKET_P_FlushFifoMemory( hGrc );
#endif

		BGRC_P_WriteReg32( hGrc->hRegister, LIST_CTRL,
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, WAKE, WakeUp ) |
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, RUN, Run ) |
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, WAKE_MODE, ResumeFromLast ) );
	}
	else
	{
		BGRC_P_WriteReg32( hGrc->hRegister, LIST_FIRST_PKT_ADDR, fifo_offset );
		BGRC_P_WriteReg32( hGrc->hRegister, LIST_CTRL,
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, WAKE, Ack ) |
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, RUN, Run ) |
			BCHP_FIELD_ENUM( M2MC_LIST_CTRL, WAKE_MODE, ResumeFromFirst ) );
	}

	hGrc->last_fifo_addr = hGrc->prev_fifo_addr;
	hGrc->prev_fifo_addr = 0;
}

/***************************************************************************/
#if defined(BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_C_SHIFT)
#define BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3 ( \
	(8L << BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_C_SHIFT)) | \
	(8L << BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_3_RANGE_EXP_MAP_SCALE_FACTOR_Y_SHIFT)) | \
	BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_DONT_LOOKUP))
#elif defined(BGRC_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_DONT_LOOKUP)
#define BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3 \
	BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_3_PALETTE_BYPASS_DONT_LOOKUP
#else
#define BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3    0
#endif

#define BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK ( \
	BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_3_ZERO_PAD_MASK) | BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_MASK))
#define BGRC_PACKET_P_DEST_SURFACE_FORMAT_DEF_3_MASK ( \
	BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_ZERO_PAD_MASK) | BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_MASK))
#define BGRC_PACKET_P_OUTPUT_SURFACE_FORMAT_DEF_3_MASK ( \
	BGRC_M2MC(OUTPUT_SURFACE_FORMAT_DEF_3_DITHER_ENABLE_MASK) | BGRC_M2MC(OUTPUT_SURFACE_FORMAT_DEF_3_CHROMA_FILTER_MASK))

#define BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS( sur ) ( \
	BGRC_M2MC(sur##_FORMAT_DEF_3_CH0_DISABLE_MASK) | BGRC_M2MC(sur##_FORMAT_DEF_3_CH1_DISABLE_MASK) | \
	BGRC_M2MC(sur##_FORMAT_DEF_3_CH2_DISABLE_MASK) | BGRC_M2MC(sur##_FORMAT_DEF_3_CH3_DISABLE_MASK))

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceFeederPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceFeeder *packet = (BM2MC_PACKET_PacketSourceFeeder *) header;
	uint32_t format00 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][0];
	uint32_t format01 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][1];
	uint32_t format02 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][2] | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	uint32_t format12 = BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS(SRC_SURFACE) | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
#endif
	uint32_t palette_lookup = 0;

	hContext->src_format0 = format00;
	if( (hContext->src_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
	{
		if( (hContext->out_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
			format00 &= M2MC_CHANNEL_MASK;
		else
			palette_lookup = BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_MASK;
	}

	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcFeeder          " );
	BGRC_PACKET_P_STORE_REG( SRC_FEEDER_ENABLE, packet->plane.address ? 1 : 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, packet->plane.address );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_0, packet->plane.pitch );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_1, 0 );

	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, format00 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_2, format01 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_FORMAT_DEF_3, format02 | palette_lookup, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_2, 0 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_1_FORMAT_DEF_3, format12, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );
#endif

	BGRC_PACKET_P_STORE_REG( SRC_W_ALPHA, packet->color >> 24 );
	BGRC_PACKET_P_STORE_REG( SRC_CONSTANT_COLOR, packet->color );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->SRC_surface_width = packet->plane.width;
	hContext->SRC_surface_height = packet->plane.height;
	hContext->SRC_surface_format = packet->plane.format;
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceFeedersPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceFeeders *packet = (BM2MC_PACKET_PacketSourceFeeders *) header;
	uint32_t format01 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane0.format][1];
	uint32_t format02 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane0.format][2] | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	uint32_t format10 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane1.format][0];
	uint32_t format11 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane1.format][1];
	uint32_t format12 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane1.format][2] | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
#endif
	hContext->src_format0 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane0.format][0];

#ifdef BCHP_M2MC_SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK
	if( hContext->linear_destripe )
	{
		hContext->src_format0 &= ~BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK);
#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
		format10 &= ~BGRC_M2MC(SRC_SURFACE_0_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK);
#endif
	}
#endif

	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcFeeders         " );
	BGRC_PACKET_P_STORE_REG( SRC_FEEDER_ENABLE, packet->plane0.address ? 1 : 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, packet->plane0.address );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_0, packet->plane0.pitch );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_1, packet->plane1.address );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_1, packet->plane1.pitch );

	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, hContext->src_format0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_2, format01 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_FORMAT_DEF_3, format02, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_1, format10 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_2, format11 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_1_FORMAT_DEF_3, format12, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );
#endif

	BGRC_PACKET_P_STORE_REG( SRC_W_ALPHA, (packet->color >> 24) | 0xFF0000 );
	BGRC_PACKET_P_STORE_REG( SRC_CONSTANT_COLOR, packet->color );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->SRC_surface_width = packet->plane0.width;
	hContext->SRC_surface_height = packet->plane0.height;
	hContext->SRC_surface_format = packet->plane0.format;
#endif
}

/***************************************************************************/
#if (BCHP_CHIP==7422) || (BCHP_CHIP==7425) || (BCHP_CHIP==7435) || (BCHP_CHIP==7445) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7429) || \
	(BCHP_CHIP==7231) || (BCHP_CHIP==7358) || (BCHP_CHIP==7360) || (BCHP_CHIP==7552) || (BCHP_CHIP==7584) || (BCHP_CHIP==35233) || (BCHP_CHIP==7563)
#define BGRC_PACKET_P_SRC_FEEDER_ENABLE_DISABLE     1
#define BGRC_PACKET_P_SRC_SURFACE_ADDR_0            hContext->sync_output_offset
#define BGRC_PACKET_P_SURFACE_FORMAT_DEF_1_DISABLE  s_BGRC_PACKET_P_DevicePixelFormats[BM2MC_PACKET_PixelFormat_eA1][0]
#define BGRC_PACKET_P_SURFACE_FORMAT_DEF_2_DISABLE  s_BGRC_PACKET_P_DevicePixelFormats[BM2MC_PACKET_PixelFormat_eA1][1]
#else
#define BGRC_PACKET_P_SRC_FEEDER_ENABLE_DISABLE     0
#define BGRC_PACKET_P_SRC_SURFACE_ADDR_0            0
#define BGRC_PACKET_P_SURFACE_FORMAT_DEF_1_DISABLE  0
#define BGRC_PACKET_P_SURFACE_FORMAT_DEF_2_DISABLE  0
#endif
/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceColorPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceColor *packet = (BM2MC_PACKET_PacketSourceColor *) header;
	uint32_t format2 = BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS(SRC_SURFACE) | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
	hContext->src_format0 = 0;

	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcColor           " );
	BGRC_PACKET_P_STORE_REG( SRC_FEEDER_ENABLE, 1 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, BGRC_PACKET_P_SRC_SURFACE_ADDR_0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_0, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_1, 0 );

	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, BGRC_PACKET_P_SURFACE_FORMAT_DEF_1_DISABLE );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_2, BGRC_PACKET_P_SURFACE_FORMAT_DEF_2_DISABLE );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_FORMAT_DEF_3, format2, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_2, 0 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_1_FORMAT_DEF_3, format2, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );
#endif

	BGRC_PACKET_P_STORE_REG( SRC_W_ALPHA, packet->color >> 24 );
	BGRC_PACKET_P_STORE_REG( SRC_CONSTANT_COLOR, packet->color );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceNonePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	uint32_t format2 = BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS(SRC_SURFACE) | BGRC_PACKET_P_DEFAULT_SOURCE_SURFACE_FORMAT_DEF_3;
	hContext->src_format0 = 0;
	BSTD_UNUSED(header);
	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcNone            " );
	BGRC_PACKET_P_STORE_REG( SRC_FEEDER_ENABLE, BGRC_PACKET_P_SRC_FEEDER_ENABLE_DISABLE );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, BGRC_PACKET_P_SRC_SURFACE_ADDR_0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_0, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_STRIDE_1, 0 );

	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, BGRC_PACKET_P_SURFACE_FORMAT_DEF_1_DISABLE );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_2, BGRC_PACKET_P_SURFACE_FORMAT_DEF_2_DISABLE );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_FORMAT_DEF_3, format2, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );

#if defined(BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1)
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_1, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_SURFACE_1_FORMAT_DEF_2, 0 );
	BGRC_PACKET_P_STORE_REG_MASK( SRC_SURFACE_1_FORMAT_DEF_3, format2, BGRC_PACKET_P_SRC_SURFACE_FORMAT_DEF_3_MASK );
#endif

	BGRC_PACKET_P_STORE_REG( SRC_W_ALPHA, 0 );
	BGRC_PACKET_P_STORE_REG( SRC_CONSTANT_COLOR, 0 );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceControlPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceControl *packet = (BM2MC_PACKET_PacketSourceControl *) header;

	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcControl            " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SRC_SURFACE_FORMAT_DEF_3, ZERO_PAD, REPLICATE, ZERO_PAD, packet->zero_pad );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SRC_SURFACE_FORMAT_DEF_3, CHROMA_FILTER, REPLICATE, FILTER, packet->chroma_filter );

#ifdef BCHP_M2MC_SRC_SURFACE_1_FORMAT_DEF_1_STRIPED_IMAGE_FORMAT_MASK
	BGRC_PACKET_P_STORE_REG_FIELD_DATA( SRC_SURFACE_0_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, packet->linear_destripe ? 0 : 1 );
	BGRC_PACKET_P_STORE_REG_FIELD_DATA( SRC_SURFACE_1_FORMAT_DEF_1, STRIPED_IMAGE_FORMAT, packet->linear_destripe ? 0 : 1 );
#endif
	hContext->linear_destripe = packet->linear_destripe;
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationFeederPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestinationFeeder *packet = (BM2MC_PACKET_PacketDestinationFeeder *) header;
	uint32_t format0 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][0];
	uint32_t format2 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][2];
	uint32_t palette_lookup = 0;

	hContext->dst_format0 = format0;
	if( (hContext->dst_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
	{
		if( (hContext->out_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
			format0 &= M2MC_CHANNEL_MASK;
		else
			palette_lookup = BCHP_M2MC_SRC_SURFACE_FORMAT_DEF_3_PALETTE_BYPASS_MASK;
	}

	BGRC_PACKET_P_DEBUG_PRINT( "-- DstFeeder          " );
	BGRC_PACKET_P_STORE_REG( DEST_FEEDER_ENABLE, packet->plane.address ? 1 : 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_0, packet->plane.address );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_0, packet->plane.pitch );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_1, format0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_2, s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][1] );
	BGRC_PACKET_P_STORE_REG_MASK( DEST_SURFACE_FORMAT_DEF_3, format2 | palette_lookup, BGRC_PACKET_P_DEST_SURFACE_FORMAT_DEF_3_MASK );
	BGRC_PACKET_P_STORE_REG( DEST_W_ALPHA, packet->color >> 24 );
	BGRC_PACKET_P_STORE_REG( DEST_CONSTANT_COLOR, packet->color );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->DEST_surface_width = packet->plane.width;
	hContext->DEST_surface_height = packet->plane.height;
	hContext->DEST_surface_format = packet->plane.format;
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationColorPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestinationColor *packet = (BM2MC_PACKET_PacketDestinationColor *) header;
	uint32_t format2 = BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS(DEST_SURFACE);
	hContext->dst_format0 = 0;
	BGRC_PACKET_P_DEBUG_PRINT( "-- DstColor           " );
	BGRC_PACKET_P_STORE_REG( DEST_FEEDER_ENABLE, 1 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_0, hContext->sync_output_offset );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_0, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_1, s_BGRC_PACKET_P_DevicePixelFormats[BM2MC_PACKET_PixelFormat_eA1][0] );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_2, s_BGRC_PACKET_P_DevicePixelFormats[BM2MC_PACKET_PixelFormat_eA1][1] );
	BGRC_PACKET_P_STORE_REG_MASK( DEST_SURFACE_FORMAT_DEF_3, format2, BGRC_PACKET_P_DEST_SURFACE_FORMAT_DEF_3_MASK );
	BGRC_PACKET_P_STORE_REG( DEST_W_ALPHA, packet->color >> 24 );
	BGRC_PACKET_P_STORE_REG( DEST_CONSTANT_COLOR, packet->color );

#if defined(BCHP_M2MC_DEST_SURFACE_ADDR_0_MSB_ADDR_MSB_MASK)
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_0_MSB, 0 );
#endif

	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationNonePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	uint32_t format2 = BGRC_PACKET_P_DISABLE_SURFACE_CHANNELS(DEST_SURFACE);
	hContext->dst_format0 = 0;
	BSTD_UNUSED(header);
	BGRC_PACKET_P_DEBUG_PRINT( "-- DstNone            " );
	BGRC_PACKET_P_STORE_REG( DEST_FEEDER_ENABLE, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_0, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_0, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_STRIDE_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_1, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_2, 0 );
	BGRC_PACKET_P_STORE_REG_MASK( DEST_SURFACE_FORMAT_DEF_3, format2, BGRC_PACKET_P_DEST_SURFACE_FORMAT_DEF_3_MASK );
	BGRC_PACKET_P_STORE_REG( DEST_W_ALPHA, 0 );
	BGRC_PACKET_P_STORE_REG( DEST_CONSTANT_COLOR, 0 );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationControlPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestinationControl *packet = (BM2MC_PACKET_PacketDestinationControl *) header;

	BGRC_PACKET_P_DEBUG_PRINT( "-- DstControl            " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( DEST_SURFACE_FORMAT_DEF_3, ZERO_PAD, REPLICATE, ZERO_PAD, packet->zero_pad );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( DEST_SURFACE_FORMAT_DEF_3, CHROMA_FILTER, REPLICATE, FILTER, packet->chroma_filter );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitOutputFeederPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketOutputFeeder *packet = (BM2MC_PACKET_PacketOutputFeeder *) header;
	uint32_t out_format0 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][0];
	uint32_t out_format2 = s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][2];
	uint32_t format_type = out_format0 & BGRC_M2MC(OUTPUT_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_MASK);
	uint32_t format0 = ((format_type == M2MC_FT_Palette) || (format_type == M2MC_FT_YCbCr420)) ? out_format0 & M2MC_CHANNEL_MASK : out_format0;

	if( (format0 & BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_MASK)) == M2MC_FT_WRGB1565 )
		format0 &= ~BGRC_M2MC(SRC_SURFACE_FORMAT_DEF_1_FORMAT_TYPE_MASK);

	hContext->out_format0 = out_format0;
	BGRC_PACKET_P_DEBUG_PRINT( "-- OutFeeder          " );
	BGRC_PACKET_P_STORE_REG( OUTPUT_FEEDER_ENABLE, packet->plane.address ? 1 : 0 );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_ADDR_0, packet->plane.address );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_STRIDE_0, packet->plane.pitch );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_ADDR_1, 0 );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_STRIDE_1, 0 );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_FORMAT_DEF_1, format0 );
	BGRC_PACKET_P_STORE_REG( OUTPUT_SURFACE_FORMAT_DEF_2, s_BGRC_PACKET_P_DevicePixelFormats[packet->plane.format][1] );
	BGRC_PACKET_P_STORE_REG_MASK( OUTPUT_SURFACE_FORMAT_DEF_3, out_format2, BGRC_PACKET_P_OUTPUT_SURFACE_FORMAT_DEF_3_MASK );

	/* check if source is palette */
	if( (hContext->src_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
	{
		uint32_t def1 = BGRC_PACKET_P_GET_REG(SRC_SURFACE_FORMAT_DEF_1);
		uint32_t def3 = BGRC_PACKET_P_GET_REG(SRC_SURFACE_FORMAT_DEF_3);

		if( (hContext->out_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
		{
			BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, hContext->src_format0 & M2MC_CHANNEL_MASK );
			BGRC_PACKET_P_STORE_REG_FIELD( SRC_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, DONT_LOOKUP );
		}
		else
		{
			BGRC_PACKET_P_STORE_REG( SRC_SURFACE_FORMAT_DEF_1, hContext->src_format0 );
			BGRC_PACKET_P_STORE_REG_FIELD( SRC_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP );
		}

		if( (def1 != BGRC_PACKET_P_GET_REG(SRC_SURFACE_FORMAT_DEF_1)) || (def3 != BGRC_PACKET_P_GET_REG(SRC_SURFACE_FORMAT_DEF_3)) )
			hContext->group_header |= BGRC_M2MC(LIST_PACKET_HEADER_1_SRC_FEEDER_GRP_CNTRL_MASK);
	}

	/* check if dest is palette */
	if( (hContext->dst_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
	{
		uint32_t def1 = BGRC_PACKET_P_GET_REG(DEST_SURFACE_FORMAT_DEF_1);
		uint32_t def3 = BGRC_PACKET_P_GET_REG(DEST_SURFACE_FORMAT_DEF_3);

		if( (hContext->out_format0 & M2MC_FT_Palette) == M2MC_FT_Palette )
		{
			BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_1, hContext->dst_format0 & M2MC_CHANNEL_MASK );
			BGRC_PACKET_P_STORE_REG_FIELD( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, DONT_LOOKUP );
		}
		else
		{
			BGRC_PACKET_P_STORE_REG( DEST_SURFACE_FORMAT_DEF_1, hContext->dst_format0 );
			BGRC_PACKET_P_STORE_REG_FIELD( DEST_SURFACE_FORMAT_DEF_3, PALETTE_BYPASS, LOOKUP );
		}

		if( (def1 != BGRC_PACKET_P_GET_REG(DEST_SURFACE_FORMAT_DEF_1)) || (def3 != BGRC_PACKET_P_GET_REG(DEST_SURFACE_FORMAT_DEF_3)) )
			hContext->group_header |= BGRC_M2MC(LIST_PACKET_HEADER_1_DST_FEEDER_GRP_CNTRL_MASK);
	}

	BGRC_PACKET_P_DEBUG_PRINT( "\n" );

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->OUTPUT_surface_width = packet->plane.width;
	hContext->OUTPUT_surface_height = packet->plane.height;
	hContext->OUTPUT_surface_format = packet->plane.format;
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitOutputControlPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketOutputControl *packet = (BM2MC_PACKET_PacketOutputControl *) header;

	BGRC_PACKET_P_DEBUG_PRINT( "-- OutControl            " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( OUTPUT_SURFACE_FORMAT_DEF_3, DITHER_ENABLE, DISABLE, ENABLE, packet->dither );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( OUTPUT_SURFACE_FORMAT_DEF_3, CHROMA_FILTER, FIRST, FILTER, packet->chroma_filter );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static uint32_t s_BGRC_PACKET_P_BlendOps[] = 
{
	BGRC_M2MC(BLEND_COLOR_OP_OP_D_ZERO),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_HALF),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_ZERO) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_COLOR),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_COLOR) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_ALPHA),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_SOURCE_ALPHA) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_COLOR),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_COLOR) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_ALPHA),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_DEST_ALPHA) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_COLOR),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_COLOR) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_ALPHA),
	BGRC_M2MC(BLEND_COLOR_OP_OP_A_CONSTANT_ALPHA) | BGRC_M2MC(BLEND_COLOR_OP_OP_A_INV_MASK)
};

/***************************************************************************/
#define BGRC_PACKET_P_CONVERT_BLEND( equation ) ( \
	(s_BGRC_PACKET_P_BlendOps[(equation).a] << BGRC_M2MC(BLEND_COLOR_OP_OP_A_SHIFT)) | \
	(s_BGRC_PACKET_P_BlendOps[(equation).b] << BGRC_M2MC(BLEND_COLOR_OP_OP_B_SHIFT)) | \
	((equation).sub_cd << BGRC_M2MC(BLEND_COLOR_OP_SUBTRACT_CD_SHIFT)) | \
	(s_BGRC_PACKET_P_BlendOps[(equation).c] << BGRC_M2MC(BLEND_COLOR_OP_OP_C_SHIFT)) | \
	(s_BGRC_PACKET_P_BlendOps[(equation).d] << BGRC_M2MC(BLEND_COLOR_OP_OP_D_SHIFT)) | \
	((equation).sub_e << BGRC_M2MC(BLEND_COLOR_OP_SUBTRACT_E_SHIFT)) | \
	(s_BGRC_PACKET_P_BlendOps[(equation).e] << BGRC_M2MC(BLEND_COLOR_OP_OP_E_SHIFT)))

#define BGRC_PACKET_P_SRC_COLOR_KEY_ACTION ( \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_SHIFT)))

#define BGRC_PACKET_P_DST_COLOR_KEY_ACTION ( \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_SHIFT)))

#define BGRC_PACKET_P_BLEND_COLOR_KEY_ACTION ( \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_TAKE_DEST) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_NOT_SRC_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_NOT_DEST_SHIFT)) | \
	(BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_TAKE_ROP) << BGRC_M2MC(BLEND_COLOR_KEY_ACTION_ACTION_SRC_DEST_SHIFT)))

#define BGRC_PACKET_P_SET_COLORKEY_ACTION() \
{ \
	if( BGRC_PACKET_P_GET_REG_FIELD(BLIT_HEADER, SRC_COLOR_KEY_ENABLE) && \
		BGRC_PACKET_P_GET_REG_FIELD(BLIT_HEADER, DEST_COLOR_KEY_ENABLE) ) \
	{ \
		BGRC_PACKET_P_STORE_REG( BLEND_COLOR_KEY_ACTION, BGRC_PACKET_P_BLEND_COLOR_KEY_ACTION ); \
	} \
	else if( BGRC_PACKET_P_GET_REG_FIELD(BLIT_HEADER, DEST_COLOR_KEY_ENABLE) ) \
	{ \
		BGRC_PACKET_P_STORE_REG( BLEND_COLOR_KEY_ACTION, BGRC_PACKET_P_DST_COLOR_KEY_ACTION ); \
	} \
	else \
	{ \
		BGRC_PACKET_P_STORE_REG( BLEND_COLOR_KEY_ACTION, BGRC_PACKET_P_SRC_COLOR_KEY_ACTION ); \
	} \
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitBlendPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketBlend *packet = (BM2MC_PACKET_PacketBlend *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- Blend              " );
	BGRC_PACKET_P_STORE_REG( BLEND_COLOR_OP, BGRC_PACKET_P_CONVERT_BLEND(packet->color_blend) );
	BGRC_PACKET_P_STORE_REG( BLEND_ALPHA_OP, BGRC_PACKET_P_CONVERT_BLEND(packet->alpha_blend) );
	BGRC_PACKET_P_STORE_REG( BLEND_CONSTANT_COLOR, packet->color );

	BGRC_PACKET_P_SET_COLORKEY_ACTION();
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitBlendColorPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketBlendColor *packet = (BM2MC_PACKET_PacketBlendColor *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- BlendColor         " );
	BGRC_PACKET_P_STORE_REG( BLEND_CONSTANT_COLOR, packet->color );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitRopPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketRop *packet = (BM2MC_PACKET_PacketRop *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- Rop                " );
	BGRC_PACKET_P_STORE_REG( ROP_OPERATION, packet->rop );
	BGRC_PACKET_P_STORE_REG( ROP_PATTERN_TOP, packet->pattern0 );
	BGRC_PACKET_P_STORE_REG( ROP_PATTERN_BOTTOM, packet->pattern1 );
	BGRC_PACKET_P_STORE_REG( ROP_PATTERN_COLOR_0, packet->color0 );
	BGRC_PACKET_P_STORE_REG( ROP_PATTERN_COLOR_1, packet->color1 );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceColorkeyPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceColorkey *packet = (BM2MC_PACKET_PacketSourceColorkey *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- SrcColorkey        " );
	BGRC_PACKET_P_STORE_REG( SRC_COLOR_KEY_HIGH, packet->high );
	BGRC_PACKET_P_STORE_REG( SRC_COLOR_KEY_LOW, packet->low );
	BGRC_PACKET_P_STORE_REG( SRC_COLOR_KEY_MASK, packet->mask );
	BGRC_PACKET_P_STORE_REG( SRC_COLOR_KEY_REPLACEMENT, packet->replacement );
	BGRC_PACKET_P_STORE_REG( SRC_COLOR_KEY_REPLACEMENT_MASK, packet->replacement_mask );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceColorkeyEnablePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceColorkeyEnable *packet = (BM2MC_PACKET_PacketSourceColorkeyEnable *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- SrcColorkeyEnable  " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_COLOR_KEY_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- SrcColorkeyDisable " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_COLOR_KEY_ENABLE, DISABLE );
	}

	BGRC_PACKET_P_SET_COLORKEY_ACTION();
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationColorkeyPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestinationColorkey *packet = (BM2MC_PACKET_PacketDestinationColorkey *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- DstColorkey        " );
	BGRC_PACKET_P_STORE_REG( DEST_COLOR_KEY_HIGH, packet->high );
	BGRC_PACKET_P_STORE_REG( DEST_COLOR_KEY_LOW, packet->low );
	BGRC_PACKET_P_STORE_REG( DEST_COLOR_KEY_MASK, packet->mask );
	BGRC_PACKET_P_STORE_REG( DEST_COLOR_KEY_REPLACEMENT, packet->replacement );
	BGRC_PACKET_P_STORE_REG( DEST_COLOR_KEY_REPLACEMENT_MASK, packet->replacement_mask );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestinationColorkeyEnablePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestinationColorkeyEnable *packet = (BM2MC_PACKET_PacketDestinationColorkeyEnable *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- DstColorkeyEnable  " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, DEST_COLOR_KEY_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- DstColorkeyDisable " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, DEST_COLOR_KEY_ENABLE, DISABLE );
	}

	BGRC_PACKET_P_SET_COLORKEY_ACTION();
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

#if defined(BCHP_M2MC_HORIZ_FIR_COEFF_PHASE0_2_COEFF_2_SHIFT)
/***************************************************************************/
#define BGRC_PACKET_P_CONVERT_COEFFS_1( coeff ) \
	((((uint32_t) (coeff)) << BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_2_COEFF_2_SHIFT)) & BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_2_COEFF_2_MASK))

#define BGRC_PACKET_P_CONVERT_COEFFS_2( coeff0, coeff1 ) \
	((((uint32_t) (coeff0)) << BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_0_SHIFT)) & BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_0_MASK)) | \
	((((uint32_t) (coeff1)) << BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_1_SHIFT)) & BGRC_M2MC(HORIZ_FIR_0_COEFF_PHASE0_01_COEFF_1_MASK))

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFilterPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketFilter *packet = (BM2MC_PACKET_PacketFilter *) header;
	uint32_t hor_coeffs0 = BGRC_PACKET_P_CONVERT_COEFFS_2(packet->hor.coeffs[0][0], packet->hor.coeffs[0][1]);
	uint32_t hor_coeffs1 = BGRC_PACKET_P_CONVERT_COEFFS_1(packet->hor.coeffs[0][2]);
	uint32_t hor_coeffs2 = BGRC_PACKET_P_CONVERT_COEFFS_2(packet->hor.coeffs[1][0], packet->hor.coeffs[1][1]);
	uint32_t hor_coeffs3 = BGRC_PACKET_P_CONVERT_COEFFS_1(packet->hor.coeffs[1][2]);
	uint32_t ver_coeffs0 = BGRC_PACKET_P_CONVERT_COEFFS_2(packet->ver.coeffs[0][0], packet->ver.coeffs[0][1]);
	uint32_t ver_coeffs1 = BGRC_PACKET_P_CONVERT_COEFFS_1(packet->ver.coeffs[0][2]);
	uint32_t ver_coeffs2 = BGRC_PACKET_P_CONVERT_COEFFS_2(packet->ver.coeffs[1][0], packet->ver.coeffs[1][1]);
	uint32_t ver_coeffs3 = BGRC_PACKET_P_CONVERT_COEFFS_1(packet->ver.coeffs[1][2]);
	BGRC_PACKET_P_DEBUG_PRINT( "-- Filter             " );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_0_COEFF_PHASE0_01, hor_coeffs0 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_0_COEFF_PHASE0_2,  hor_coeffs1 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_0_COEFF_PHASE1_01, hor_coeffs2 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_0_COEFF_PHASE1_2,  hor_coeffs3 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_1_COEFF_PHASE0_01, hor_coeffs0 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_1_COEFF_PHASE0_2,  hor_coeffs1 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_1_COEFF_PHASE1_01, hor_coeffs2 );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_1_COEFF_PHASE1_2,  hor_coeffs3 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_0_COEFF_PHASE0_01, ver_coeffs0 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_0_COEFF_PHASE0_2,  ver_coeffs1 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_0_COEFF_PHASE1_01, ver_coeffs2 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_0_COEFF_PHASE1_2,  ver_coeffs3 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_1_COEFF_PHASE0_01, ver_coeffs0 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_1_COEFF_PHASE0_2,  ver_coeffs1 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_1_COEFF_PHASE1_01, ver_coeffs2 );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_1_COEFF_PHASE1_2,  ver_coeffs3 );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}
#else
/***************************************************************************/
#define BGRC_PACKET_P_CONVERT_COEFFS( coeff0, coeff1 ) \
	(((uint32_t) (coeff0) << 16) & BGRC_M2MC(HORIZ_FIR_COEFF_PHASE0_01_COEFF_0_MASK)) | \
	(((uint32_t) (coeff1)) & BGRC_M2MC(HORIZ_FIR_COEFF_PHASE0_01_COEFF_1_MASK))

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFilterPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketFilter *packet = (BM2MC_PACKET_PacketFilter *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- Filter             " );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE0_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[0][0], packet->hor.coeffs[0][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE0_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[0][2], packet->hor.coeffs[0][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE0_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[0][4], packet->hor.coeffs[0][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE0_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[0][6], packet->hor.coeffs[0][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE1_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[1][0], packet->hor.coeffs[1][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE1_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[1][2], packet->hor.coeffs[1][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE1_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[1][4], packet->hor.coeffs[1][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE1_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[1][6], packet->hor.coeffs[1][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE2_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[2][0], packet->hor.coeffs[2][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE2_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[2][2], packet->hor.coeffs[2][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE2_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[2][4], packet->hor.coeffs[2][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE2_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[2][6], packet->hor.coeffs[2][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE3_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[3][0], packet->hor.coeffs[3][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE3_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[3][2], packet->hor.coeffs[3][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE3_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[3][4], packet->hor.coeffs[3][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE3_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[3][6], packet->hor.coeffs[3][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE4_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[4][0], packet->hor.coeffs[4][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE4_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[4][2], packet->hor.coeffs[4][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE4_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[4][4], packet->hor.coeffs[4][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE4_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[4][6], packet->hor.coeffs[4][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE5_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[5][0], packet->hor.coeffs[5][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE5_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[5][2], packet->hor.coeffs[5][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE5_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[5][4], packet->hor.coeffs[5][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE5_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[5][6], packet->hor.coeffs[5][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE6_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[6][0], packet->hor.coeffs[6][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE6_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[6][2], packet->hor.coeffs[6][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE6_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[6][4], packet->hor.coeffs[6][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE6_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[6][6], packet->hor.coeffs[6][7]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE7_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[7][0], packet->hor.coeffs[7][1]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE7_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[7][2], packet->hor.coeffs[7][3]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE7_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[7][4], packet->hor.coeffs[7][5]) );
	BGRC_PACKET_P_STORE_REG( HORIZ_FIR_COEFF_PHASE7_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->hor.coeffs[7][6], packet->hor.coeffs[7][7]) );

	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE0_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[0][0], packet->ver.coeffs[0][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE0_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[0][2], packet->ver.coeffs[0][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE0_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[0][4], packet->ver.coeffs[0][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE0_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[0][6], packet->ver.coeffs[0][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE1_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[1][0], packet->ver.coeffs[1][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE1_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[1][2], packet->ver.coeffs[1][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE1_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[1][4], packet->ver.coeffs[1][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE1_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[1][6], packet->ver.coeffs[1][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE2_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[2][0], packet->ver.coeffs[2][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE2_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[2][2], packet->ver.coeffs[2][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE2_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[2][4], packet->ver.coeffs[2][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE2_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[2][6], packet->ver.coeffs[2][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE3_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[3][0], packet->ver.coeffs[3][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE3_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[3][2], packet->ver.coeffs[3][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE3_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[3][4], packet->ver.coeffs[3][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE3_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[3][6], packet->ver.coeffs[3][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE4_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[4][0], packet->ver.coeffs[4][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE4_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[4][2], packet->ver.coeffs[4][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE4_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[4][4], packet->ver.coeffs[4][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE4_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[4][6], packet->ver.coeffs[4][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE5_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[5][0], packet->ver.coeffs[5][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE5_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[5][2], packet->ver.coeffs[5][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE5_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[5][4], packet->ver.coeffs[5][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE5_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[5][6], packet->ver.coeffs[5][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE6_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[6][0], packet->ver.coeffs[6][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE6_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[6][2], packet->ver.coeffs[6][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE6_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[6][4], packet->ver.coeffs[6][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE6_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[6][6], packet->ver.coeffs[6][7]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE7_01, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[7][0], packet->ver.coeffs[7][1]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE7_23, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[7][2], packet->ver.coeffs[7][3]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE7_45, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[7][4], packet->ver.coeffs[7][5]) );
	BGRC_PACKET_P_STORE_REG( VERT_FIR_COEFF_PHASE7_67, BGRC_PACKET_P_CONVERT_COEFFS(packet->ver.coeffs[7][6], packet->ver.coeffs[7][7]) );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}
#endif

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFilterEnablePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
#if defined(BCHP_M2MC_SCALER_CTRL_CLUT_SCALE_MODE_MASK)
	BM2MC_PACKET_PacketFilterEnable *packet = (BM2MC_PACKET_PacketFilterEnable *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- FilterEnable       \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, CLUT_SCALE_MODE, DISABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- FilterDisable      \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, CLUT_SCALE_MODE, ENABLE );
	}
#else
	BSTD_UNUSED(hContext);
	BSTD_UNUSED(header);
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFilterControlPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketFilterControl *packet = (BM2MC_PACKET_PacketFilterControl *) header;
	BSTD_UNUSED(hContext);

#if defined(BCHP_M2MC_SCALER_CTRL_ROUNDING_MODE_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, ROUNDING_MODE, TRUNCATE, NEAREST, packet->round );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_OFFSET_EN_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, ALPHA_PRE_MULTIPLY_OFFSET_EN, DISABLE, ENABLE, packet->adjust_color );
#endif

#if defined(BCHP_M2MC_SCALER_CTRL_OFFSET_ADJUST_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, OFFSET_ADJUST, DISABLE, ENABLE, packet->sub_alpha );
#endif

#if defined(BCHP_M2MC_BLIT_HEADER_CBAR_SRC_COLOR_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_DATA( BLIT_HEADER, CBAR_SRC_COLOR, packet->order );
#endif

#if defined(BCHP_M2MC_BLIT_CTRL_DEST_ALPHA_PRE_MULTIPLY_OFFSET_EN_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, DEST_ALPHA_PRE_MULTIPLY_OFFSET_EN, DISABLE, ENABLE, packet->adjust_color );
#endif

#if defined(BCHP_M2MC_BLIT_CTRL_DEST_CBAR_SELECT_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, DEST_CBAR_SELECT, CKEY_THEN_ALPHA_PREMULT, ALPHA_PREMULT_THEN_CKEY, packet->order );
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceColorMatrixPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceColorMatrix *packet = (BM2MC_PACKET_PacketSourceColorMatrix *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- ColorMatrix        " );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C00_C01, packet->matrix.m[0][1] | (packet->matrix.m[0][0] << BGRC_M2MC(SRC_CM_C00_C01_CM_C00_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C02_C03, packet->matrix.m[0][3] | (packet->matrix.m[0][2] << BGRC_M2MC(SRC_CM_C02_C03_CM_C02_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C04,     packet->matrix.m[0][4] );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C10_C11, packet->matrix.m[1][1] | (packet->matrix.m[1][0] << BGRC_M2MC(SRC_CM_C10_C11_CM_C10_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C12_C13, packet->matrix.m[1][3] | (packet->matrix.m[1][2] << BGRC_M2MC(SRC_CM_C12_C13_CM_C12_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C14,     packet->matrix.m[1][4] );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C20_C21, packet->matrix.m[2][1] | (packet->matrix.m[2][0] << BGRC_M2MC(SRC_CM_C20_C21_CM_C20_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C22_C23, packet->matrix.m[2][3] | (packet->matrix.m[2][2] << BGRC_M2MC(SRC_CM_C22_C23_CM_C22_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C24,     packet->matrix.m[2][4] );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C30_C31, packet->matrix.m[3][1] | (packet->matrix.m[3][0] << BGRC_M2MC(SRC_CM_C30_C31_CM_C30_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C32_C33, packet->matrix.m[3][3] | (packet->matrix.m[3][2] << BGRC_M2MC(SRC_CM_C32_C33_CM_C32_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( SRC_CM_C34,     packet->matrix.m[3][4] );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourceColorMatrixEnablePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourceColorMatrixEnable *packet = (BM2MC_PACKET_PacketSourceColorMatrixEnable *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- ColorMatrixEnable  \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_COLOR_MATRIX_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- ColorMatrixDisable \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_COLOR_MATRIX_ENABLE, DISABLE );
	}
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSourcePalettePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketSourcePalette *packet = (BM2MC_PACKET_PacketSourcePalette *) header;
#if (BCHP_CHIP==7420)
	if( packet->address >= 0x60000000 )
		BDBG_ERR(("7420 M2MC cannot access palette on MEMC1"));
#endif
	BGRC_PACKET_P_DEBUG_PRINT( "-- Palette            " );
	BGRC_PACKET_P_STORE_REG( SRC_CLUT_ENTRY_i_ARRAY_BASE, packet->address ? packet->address : hContext->sync_output_offset );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitMirrorPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketMirror *packet = (BM2MC_PACKET_PacketMirror *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- Mirror             " );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->src_hor );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->src_ver );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->dst_hor );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->dst_ver );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->out_hor );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->out_ver );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, SRC_H_DIRECTION, LEFT_TO_RIGHT, RIGHT_TO_LEFT, packet->src_hor );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, SRC_V_DIRECTION, TOP_TO_BOTTOM, BOTTOM_TO_TOP, packet->src_ver );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, DEST_H_DIRECTION, LEFT_TO_RIGHT, RIGHT_TO_LEFT, packet->dst_hor );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, DEST_V_DIRECTION, TOP_TO_BOTTOM, BOTTOM_TO_TOP, packet->dst_ver );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, OUTPUT_H_DIRECTION, LEFT_TO_RIGHT, RIGHT_TO_LEFT, packet->out_hor );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, OUTPUT_V_DIRECTION, TOP_TO_BOTTOM, BOTTOM_TO_TOP, packet->out_ver );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFixedScalePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketFixedScale *packet = (BM2MC_PACKET_PacketFixedScale *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- FixedScale         " );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->hor_phase );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->ver_phase );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->hor_step );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->ver_step );
	BGRC_PACKET_P_DEBUG_PRINT_VALUE( packet->shift );
	hContext->fixed_scale = *packet;
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestripeFixedScalePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketDestripeFixedScale *packet = (BM2MC_PACKET_PacketDestripeFixedScale *) header;
	BGRC_PACKET_P_DEBUG_PRINT( "-- DestripeFixedScale         " );
	hContext->destripe_fixed_scale = *packet;
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitAlphaPremultiplyPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
#if defined(BCHP_M2MC_SCALER_CTRL_ALPHA_PRE_MULTIPLY_ENABLE_MASK)
	BM2MC_PACKET_PacketAlphaPremultiply *packet = (BM2MC_PACKET_PacketAlphaPremultiply *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- AlphaPremulEnable  \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, ALPHA_PRE_MULTIPLY_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- AlphaPremulDisable \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, ALPHA_PRE_MULTIPLY_ENABLE, DISABLE );
	}
#else
	BSTD_UNUSED(hContext);
	BSTD_UNUSED(header);
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitAlphaDemultiplyPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
#if defined(BCHP_M2MC_BLIT_CTRL_ALPHA_DE_MULTIPLY_ENABLE_MASK)
	BM2MC_PACKET_PacketAlphaPremultiply *packet = (BM2MC_PACKET_PacketAlphaPremultiply *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- AlphaDemulEnable   \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, ALPHA_DE_MULTIPLY_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- AlphaDemulDisable  \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, ALPHA_DE_MULTIPLY_ENABLE, DISABLE );
	}
#else
	BSTD_UNUSED(hContext);
	BSTD_UNUSED(header);
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestAlphaPremultiplyPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
#if defined(BCHP_M2MC_BLIT_CTRL_DEST_ALPHA_PRE_MULTIPLY_ENABLE_MASK)
	BM2MC_PACKET_PacketAlphaPremultiply *packet = (BM2MC_PACKET_PacketAlphaPremultiply *) header;
	if( packet->enable )
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- DestAlphaPreEnable \n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, DEST_ALPHA_PRE_MULTIPLY_ENABLE, ENABLE );
	}
	else
	{
		BGRC_PACKET_P_DEBUG_PRINT( "-- DestAlphaPreDisable\n" );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, DEST_ALPHA_PRE_MULTIPLY_ENABLE, DISABLE );
	}
#else
	BSTD_UNUSED(hContext);
	BSTD_UNUSED(header);
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitFillBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketFillBlit *packet = (BM2MC_PACKET_PacketFillBlit *) header;
	uint32_t pos = packet->rect.x | ((uint32_t) packet->rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t size = packet->rect.height | ((uint32_t) packet->rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

	BGRC_PACKET_P_DEBUG_PRINT( "-- FillBlit           " );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_RECT_REGS( pos, size, 0, 0, pos, size, pos, size );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
#if ((BCHP_CHIP==7400) && (BCHP_VER == BCHP_VER_A0)) || \
	((BCHP_CHIP==7550) && (BCHP_VER == BCHP_VER_A0)) || \
	(BCHP_CHIP==7401) || (BCHP_CHIP==7403) || (BCHP_CHIP==7038) || (BCHP_CHIP==3573) || \
	(BCHP_CHIP==3548) || (BCHP_CHIP==7358) || (BCHP_CHIP==7552)
#define BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos )
#else
#if 1
#define BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos )
#else
#define BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos ) \
{ \
	/* if the source format is 422 and the source x is odd, then adjust source x and source address to make it even */ \
	if( BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422 ) \
	{ \
		if( src_pos & 1 ) \
		{ \
			src_pos &= ~1; \
			BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, BGRC_PACKET_P_GET_REG(SRC_SURFACE_ADDR_0) + 2 ); \
		} \
		else \
		{ \
			BGRC_PACKET_P_STORE_REG( SRC_SURFACE_ADDR_0, BGRC_PACKET_P_GET_REG(SRC_SURFACE_ADDR_0) & (~3) ); \
		} \
	} \
}
#endif
#endif

#if 0/*def BGRC_M2MC(BLIT_CTRL_BLOCK_AUTO_SPLIT_FIFO_MASK)*/
#define BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_1_SRC( x0, w0 ) \
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, BLOCK_AUTO_SPLIT_FIFO, DISABLE, ENABLE, \
		(x0 & 7) && (w0 >= 57) && (w0 <= 64) )
#define BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_2_SRC( x0, w0, x1, w1 ) \
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, BLOCK_AUTO_SPLIT_FIFO, DISABLE, ENABLE, ( \
		(x0 & 7) && (w0 >= 57) && (w0 <= 64)) || ( \
		(x1 & 7) && (w1 >= 57) && (w1 <= 64)) )
#else
#define BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_1_SRC( x0, w0 )
#define BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_2_SRC( x0, w0, x1, w1 )
#endif

/***************************************************************************/
static void BGRC_PACKET_P_SubmitCopyBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketCopyBlit *packet = (BM2MC_PACKET_PacketCopyBlit *) header;
	uint32_t src_pos = packet->src_rect.x | ((uint32_t) packet->src_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t out_pos = packet->out_point.x | ((uint32_t) packet->out_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t size = packet->src_rect.height | ((uint32_t) packet->src_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

	BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos );

	BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_1_SRC( packet->src_rect.x, packet->src_rect.width );

	BGRC_PACKET_P_DEBUG_PRINT( "-- CopyBlit           " );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_RECT_REGS( src_pos, size, src_pos, size, out_pos, size, out_pos, size );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitBlendBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketBlendBlit *packet = (BM2MC_PACKET_PacketBlendBlit *) header;
	uint32_t src_pos = packet->src_rect.x | ((uint32_t) packet->src_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t out_pos = packet->out_point.x | ((uint32_t) packet->out_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t dst_pos = packet->dst_point.x | ((uint32_t) packet->dst_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t size = packet->src_rect.height | ((uint32_t) packet->src_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

	BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos );

	BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_2_SRC( packet->src_rect.x, packet->src_rect.width, packet->dst_point.x, packet->src_rect.width );

	BGRC_PACKET_P_DEBUG_PRINT( "-- BlendBlit          " );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, DISABLE );
	BGRC_PACKET_P_STORE_RECT_REGS( src_pos, size, src_pos, size, dst_pos, size, out_pos, size );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
#if defined(BCHP_M2MC_HORIZ_SCALER_0_STEP_reserved0_SHIFT)
#else
#define BCHP_M2MC_HORIZ_SCALER_0_STEP_reserved0_SHIFT   BCHP_M2MC_HORIZ_SCALER_STEP_reserved0_SHIFT
#endif

#define BGRC_PACKET_P_SCALER_STEP_FRAC_BITS       (BGRC_M2MC(HORIZ_SCALER_0_STEP_reserved0_SHIFT) - 4)
#define BGRC_PACKET_P_SCALER_STEP_FRAC_ONE        (1 << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS)
#define BGRC_PACKET_P_SCALER_STEP_FRAC_MASK       (BGRC_PACKET_P_SCALER_STEP_FRAC_ONE - 1)
#define BGRC_PACKET_P_SCALER_STRIPE_MAX           128
#define BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT  (BGRC_PACKET_P_SCALER_STEP_FRAC_BITS - 16)
#define BGRC_PACKET_P_SCALER_WIDTH_TO_INT_SHIFT   (BGRC_PACKET_P_SCALER_STEP_FRAC_BITS - BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT)
#define BGRC_PACKET_P_SCALER_HALF_SIZE            4095

#define BGRC_PACKET_P_FIXED_SHIFT_RIGHT( data, shift ) \
	(((data) >> (shift)) | (((data) & 0x80000000) ? ~(0xFFFFFFFF >> (shift)) : 0))

#if (BCHP_CHIP==7038)
#define BGRC_PACKET_P_SCALER_STRIPE_OVERLAP       2
#else
#define BGRC_PACKET_P_SCALER_STRIPE_OVERLAP       3
#endif
#define BGRC_PACKET_P_SCALER_STRIPE_OVERLAP_BASE  4

#define BGRC_PACKET_P_NEG(a)                      (~(a) + 1)

/***************************************************************************/
static void BGRC_PACKET_P_SetScaler( BGRC_PacketContext_Handle hContext, BGRC_PACKET_P_Scaler *scaler,
	BM2MC_PACKET_Rectangle *src_rect, BM2MC_PACKET_Rectangle *out_rect )
{
	uint32_t h_step, abs_h_phase;
	uint32_t hor_shift_adjust = (src_rect->width > BGRC_PACKET_P_SCALER_HALF_SIZE) ? 1 : 0;
	uint32_t ver_shift_adjust = (src_rect->height > BGRC_PACKET_P_SCALER_HALF_SIZE) ? 1 : 0;

	BSTD_UNUSED(hContext);

	/* calculate scaler steps and initial phases */
	/* HW limit 1: abs(phase) < src_step */
	h_step = (((uint32_t) src_rect->width << (BGRC_PACKET_P_SCALER_STEP_FRAC_BITS - hor_shift_adjust)) / out_rect->width) << hor_shift_adjust;
	abs_h_phase = (h_step >= BGRC_PACKET_P_SCALER_STEP_FRAC_ONE)?
		(h_step - BGRC_PACKET_P_SCALER_STEP_FRAC_ONE) >> 1:
		(BGRC_PACKET_P_SCALER_STEP_FRAC_ONE - h_step) >> 1;
	scaler->hor_phase =
		BGRC_PACKET_P_FIXED_SHIFT_RIGHT(h_step - BGRC_PACKET_P_SCALER_STEP_FRAC_ONE, 1);
	scaler->hor_chroma_phase = 0;

	scaler->ver_step = (((uint32_t) src_rect->height << (BGRC_PACKET_P_SCALER_STEP_FRAC_BITS - ver_shift_adjust)) / out_rect->height) << ver_shift_adjust;
	scaler->ver_phase =
		BGRC_PACKET_P_FIXED_SHIFT_RIGHT(scaler->ver_step - BGRC_PACKET_P_SCALER_STEP_FRAC_ONE, 1);
	scaler->ver_chroma_phase = 0;

	/* check if striping is required */
	if( (src_rect->width > BGRC_PACKET_P_SCALER_STRIPE_MAX) && (out_rect->width > BGRC_PACKET_P_SCALER_STRIPE_MAX) )
	{
		uint32_t overlap_base, stripe_overlap, stripe_w, in_stripe_w, out_stripe_w, last_stripe_w;
		uint32_t  scale_factor;

		/* calculate stripe overlap */
		if(h_step <= 0x2*BGRC_PACKET_P_SCALER_STEP_FRAC_ONE) overlap_base = 4;
		else if (h_step <= 0x4*BGRC_PACKET_P_SCALER_STEP_FRAC_ONE) overlap_base = 3;
		else if (h_step <= 0x6*BGRC_PACKET_P_SCALER_STEP_FRAC_ONE) overlap_base = 2;
		else /* if (h_step > 0x6*BGRC_PACKET_P_SCALER_STEP_FRAC_ONE) */ overlap_base = 1;
		stripe_overlap =
			(abs_h_phase >> BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) + overlap_base;

		/* calculate stripe width: adjust them so that HW limits are satisfied */
		stripe_w = BGRC_PACKET_P_SCALER_STRIPE_MAX - stripe_overlap * 2;
		out_stripe_w = ( src_rect->width >= out_rect->width )? stripe_w :
			(stripe_w << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) / h_step;
		out_stripe_w &= 0xFFFFFFFE;
		in_stripe_w = (out_stripe_w * h_step) >> BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT;
		last_stripe_w = ((uint32_t)src_rect->width << 16) % in_stripe_w;
		scale_factor = (BGRC_PACKET_P_SCALER_STEP_FRAC_ONE << 2) / h_step + 1;
		while( (in_stripe_w) && (out_stripe_w > 2) &&
			   (((((uint32_t)src_rect->width<<16) + in_stripe_w - 1) / in_stripe_w !=
				 (out_rect->width + out_stripe_w - 1) / out_stripe_w) ||
#ifdef BCHP_M2MC_BLIT_CTRL_BLOCK_AUTO_SPLIT_FIFO_MASK
				/* the following 2 limits are from Glenn's code, test shows that
				 * 7445 does not need any of them, but 7231 really needs them */
				((out_rect->width % out_stripe_w) < 4) ||
				/* glenn: make sure last stripe is less pixels than the rest */
				((out_rect->width % out_stripe_w) >= out_stripe_w - scale_factor) ||
#endif
				(last_stripe_w && (last_stripe_w < (stripe_overlap <<16)))) )
		{
			out_stripe_w -= 2;
			in_stripe_w = (out_stripe_w * h_step) >> BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT;
			last_stripe_w = ((uint32_t)src_rect->width << 16) % in_stripe_w;
		}

		scaler->output_stripe_width = out_stripe_w;
		scaler->input_stripe_width = in_stripe_w;
		scaler->stripe_overlap = stripe_overlap;
		scaler->stripe_count = (0 == out_stripe_w)? 1 : (out_rect->width + out_stripe_w - 1) / out_stripe_w;

#if (BDBG_DEBUG_BUILD)
		if (in_stripe_w < (110<<16) || out_stripe_w < 110)
		{
			BDBG_WRN(("info: in_w %d, in_stripe_w %p(%d), out_w %d, out_stripe_w %d",
				src_rect->width , in_stripe_w, in_stripe_w>>16, out_rect->width, out_stripe_w));
			if ((((uint32_t)src_rect->width<<16) + in_stripe_w - 1) / in_stripe_w !=
				(out_rect->width + out_stripe_w - 1) / out_stripe_w)
				BDBG_WRN(("# of in stripes != # of out stripes: in_w %d, in_stripe_w %p(%d), out_w %d, out_stripe_w %d",
					src_rect->width , in_stripe_w, in_stripe_w>>16, out_rect->width, out_stripe_w));
			if (last_stripe_w && (last_stripe_w < (stripe_overlap <<16)))
				BDBG_WRN(("last stripe in_w too small: in_w %d, in_stripe_w %p(%d), out_w %d, out_stripe_w %d, last_stripe_in %d, min %d",
					src_rect->width , in_stripe_w, in_stripe_w>>16, out_rect->width, out_stripe_w, last_stripe_w, stripe_overlap <<16));
			if ((out_rect->width % out_stripe_w) < 4)
				BDBG_WRN(("last stripe out_w < 4: in_w %d, in_stripe_w %p(%d), out_w %d, out_stripe_w %d, last_stripe_out %d",
					src_rect->width , in_stripe_w, in_stripe_w>>16, out_rect->width, out_stripe_w, out_rect->width % out_stripe_w));
			if ((out_rect->width % out_stripe_w) >= out_stripe_w - scale_factor)
				BDBG_WRN(("last stripe out_w too big: in_w %d, in_stripe_w %p(%d), out_w %d, out_stripe_w %d, last_stripe_out %d >= %d",
					src_rect->width , in_stripe_w, in_stripe_w>>16, out_rect->width, out_stripe_w, out_rect->width % out_stripe_w, out_stripe_w - scale_factor));
		}
#endif /* (BDBG_DEBUG_BUILD) */
	}
	else
	{
		scaler->stripe_overlap = 0;
		scaler->output_stripe_width = 0;
		scaler->input_stripe_width = 0;
		scaler->stripe_count = 1;
	}
	scaler->hor_step = h_step;
	scaler->stripe_num = 0;
}

/***************************************************************************/
static void BGRC_PACKET_P_SetStriping( BGRC_PacketContext_Handle hContext, 
	BM2MC_PACKET_Rectangle *src_rect, BM2MC_PACKET_Rectangle *out_rect,  BM2MC_PACKET_Point *dst_point,
	BGRC_PACKET_P_Scaler *scaler, BGRC_PACKET_P_Stripe *stripe, uint32_t stripe_num )
{
	stripe->hor_phase = scaler->hor_phase;
	stripe->src_x = src_rect->x;
	stripe->dst_x = dst_point->x;
	stripe->out_x = out_rect->x;

	if( scaler->stripe_count <= 1 )
	{
		stripe->src_width = src_rect->width;
		stripe->out_width = out_rect->width;
	}
	else
	{
		/* adjust phase and positions for all but the first stripe */
		if( stripe_num > 0 )
		{
			uint32_t input_stripe_start = scaler->input_stripe_width * stripe_num;
			uint32_t output_stripe_start = scaler->output_stripe_width * stripe_num;
			uint32_t fixed_phase = BGRC_PACKET_P_FIXED_SHIFT_RIGHT(stripe->hor_phase, BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT);
			uint32_t fixed_left = fixed_phase + input_stripe_start;
			uint32_t pad_left = BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;

			stripe->src_x += BGRC_PACKET_P_FIXED_SHIFT_RIGHT(fixed_left, BGRC_PACKET_P_SCALER_WIDTH_TO_INT_SHIFT) - BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;
			stripe->dst_x += output_stripe_start;
			stripe->out_x += output_stripe_start;

			stripe->hor_phase += input_stripe_start << BGRC_PACKET_P_SCALER_STEP_TO_WIDTH_SHIFT;
			stripe->hor_phase &= BGRC_PACKET_P_SCALER_STEP_FRAC_MASK;
			stripe->hor_phase += pad_left << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS;
		}
	
		/* adjust widths */
		if( stripe_num < scaler->stripe_count - 1 )
		{
			stripe->src_width = (scaler->input_stripe_width >> BGRC_PACKET_P_SCALER_WIDTH_TO_INT_SHIFT) + BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;
			if( stripe_num > 0 )
				stripe->src_width += BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;
			stripe->out_width = scaler->output_stripe_width;
		}
		else
		{
			stripe->src_width = src_rect->width - (stripe->src_x - src_rect->x);
			stripe->out_width = out_rect->width - scaler->output_stripe_width * (scaler->stripe_count - 1);
		}
		/* adjust source x for horizontal source mirroring */
		if( BGRC_PACKET_P_GET_REG(BLIT_CTRL) & BGRC_M2MC(BLIT_CTRL_SRC_H_DIRECTION_MASK) )
		{
			stripe->src_x = src_rect->x;
			if( stripe_num < scaler->stripe_count - 1 )
				stripe->src_x += src_rect->width - (scaler->input_stripe_width * (scaler->stripe_count - 1) >> BGRC_PACKET_P_SCALER_WIDTH_TO_INT_SHIFT) - BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;
			if( stripe_num < scaler->stripe_count - 2 )
				stripe->src_x += (scaler->input_stripe_width * (scaler->stripe_count - stripe_num - 2) >> BGRC_PACKET_P_SCALER_WIDTH_TO_INT_SHIFT) - BGRC_PACKET_P_SCALER_STRIPE_OVERLAP;
		}

		/* adjust output x for horizontal output mirroring */
		if( BGRC_PACKET_P_GET_REG(BLIT_CTRL) & BGRC_M2MC(BLIT_CTRL_OUTPUT_H_DIRECTION_MASK) )
		{
			stripe->out_x = out_rect->x;
			if( stripe_num < scaler->stripe_count - 1 )
				stripe->out_x += out_rect->width - scaler->output_stripe_width * (scaler->stripe_count - 1);
			if( stripe_num < scaler->stripe_count - 2 )
				stripe->out_x += scaler->output_stripe_width * (scaler->stripe_count - stripe_num - 2);
		}

		/* adjust destination x for horizontal destination mirroring */
		if( BGRC_PACKET_P_GET_REG(BLIT_CTRL) & BGRC_M2MC(BLIT_CTRL_DEST_H_DIRECTION_MASK) )
		{
			stripe->dst_x = dst_point->x;
			if( stripe_num < scaler->stripe_count - 1 )
				stripe->dst_x += out_rect->width - scaler->output_stripe_width * (scaler->stripe_count - 1);
			if( stripe_num < scaler->stripe_count - 2 )
				stripe->dst_x += scaler->output_stripe_width * (scaler->stripe_count - stripe_num - 2);
		}
	}

	/* need even stripe width for YCbCr422 surfaces */
	if( (stripe->src_x & 1) && (BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) )
	{
		stripe->src_x--;
		stripe->hor_phase += 1L << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS;
		stripe->src_width++;
	}

    BDBG_ASSERT(stripe->src_x + stripe->src_width <= src_rect->x + src_rect->width);
    BDBG_ASSERT(stripe->out_x + stripe->out_width <= out_rect->x + out_rect->width);
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitScaleBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketScaleBlit *packet = (BM2MC_PACKET_PacketScaleBlit *) header;
	uint32_t src_pos = packet->src_rect.x | ((uint32_t) packet->src_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t out_pos = packet->out_rect.x | ((uint32_t) packet->out_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t src_size = packet->src_rect.height | ((uint32_t) packet->src_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
	uint32_t out_size = packet->out_rect.height | ((uint32_t) packet->out_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

/*	BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect, 
		hContext->fixed_scale.hor_phase, 0, hContext->fixed_scale.ver_phase, 0, 
		hContext->fixed_scale.hor_step, hContext->fixed_scale.ver_step, hContext->fixed_scale.shift );
*/	hContext->scaler_header = header;

	BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos );

	BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_1_SRC( packet->src_rect.x, packet->src_rect.width );

	BGRC_PACKET_P_DEBUG_PRINT( "-- ScaleBlit          " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, STRIPE_ENABLE, DISABLE, ENABLE, hContext->scaler.stripe_count > 1 );
	BGRC_PACKET_P_STORE_RECT_REGS( src_pos, src_size, src_pos, src_size, out_pos, out_size, out_pos, out_size );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH, hContext->scaler.input_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH_1, hContext->scaler.input_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_STRIPE_WIDTH, hContext->scaler.output_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP_1, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, packet->src_rect.width >= packet->out_rect.width );
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, hContext->scaler.hor_phase, hContext->scaler.hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, hContext->scaler.hor_phase, hContext->scaler.hor_step );
#endif
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#endif
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitStripeBlitPacket( BGRC_PacketContext_Handle hContext,
	BM2MC_PACKET_Rectangle *src_rect, BM2MC_PACKET_Rectangle *out_rect,  BM2MC_PACKET_Point *dst_point )
{
	BGRC_PACKET_P_Stripe stripe;
	BGRC_PACKET_P_SetStriping( hContext, src_rect, out_rect, dst_point, &hContext->scaler, &stripe, hContext->scaler.stripe_num );
	hContext->scaler.stripe_num++;

	{
		uint32_t src_pos = stripe.src_x | ((uint32_t) src_rect->y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t out_pos = stripe.out_x | ((uint32_t) out_rect->y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t dst_pos = stripe.dst_x | ((uint32_t) dst_point->y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t src_size = src_rect->height | ((uint32_t) stripe.src_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t out_size = out_rect->height | ((uint32_t) stripe.out_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

		BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos );

		BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_2_SRC( stripe.src_x, stripe.src_width, stripe.dst_x, stripe.out_width );

		BGRC_PACKET_P_DEBUG_PRINT( "-- StripeBlit         " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
		BGRC_PACKET_P_STORE_RECT_REGS( src_pos, src_size, src_pos, src_size, dst_pos, out_size, out_pos, out_size );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, stripe.src_width >= stripe.out_width );
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, stripe.hor_phase, hContext->scaler.hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, stripe.hor_phase, hContext->scaler.hor_step );
#endif
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#endif
		BGRC_PACKET_P_DEBUG_PRINT( "\n" );
	}
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitScaleBlendBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketScaleBlendBlit *packet = (BM2MC_PACKET_PacketScaleBlendBlit *) header;

	if( hContext->scaler.stripe_num == 0 )
	{
		BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect );
		hContext->scaler_header = header;
	}

	/* syang: why we can not use BGRC_PACKET_P_SubmitScaleBlitPacket( hContext, header ); */
	BGRC_PACKET_P_SubmitStripeBlitPacket( hContext, &packet->src_rect, &packet->out_rect, &packet->dst_point );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitUpdateScaleBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BM2MC_PACKET_PacketUpdateScaleBlit *packet = (BM2MC_PACKET_PacketUpdateScaleBlit *) header;
	uint32_t shift = BGRC_PACKET_P_SCALER_STEP_FRAC_BITS;

	uint32_t hor_step = ((uint32_t) packet->src_rect.width << shift) / packet->out_rect.width;
	uint32_t ver_step = ((uint32_t) packet->src_rect.height << shift) / packet->out_rect.height;

	uint32_t offset_x = packet->update_rect.x * hor_step;
	uint32_t offset_y = packet->update_rect.y * ver_step;

	uint32_t src_x0 = packet->src_rect.x;
	uint32_t src_y0 = packet->src_rect.y;
	uint32_t src_x1 = packet->src_rect.x + packet->src_rect.width;
	uint32_t src_y1 = packet->src_rect.y + packet->src_rect.height;

	uint32_t scale_x0 = ((uint32_t) packet->update_rect.x * hor_step) >> shift;
	uint32_t scale_y0 = ((uint32_t) packet->update_rect.y * ver_step) >> shift;
	uint32_t scale_x1 = ((uint32_t) (packet->update_rect.x + packet->update_rect.width) * hor_step) >> shift;
	uint32_t scale_y1 = ((uint32_t) (packet->update_rect.y + packet->update_rect.height) * ver_step) >> shift;

	uint32_t pad = 2;
	uint32_t pad_x0 = ((scale_x0 > src_x0 + pad) ? scale_x0 - pad : src_x0);
	uint32_t pad_y0 = ((scale_y0 > src_y0 + pad) ? scale_y0 - pad : src_y0);
	uint32_t pad_x1 = ((scale_x1 + pad < src_x1) ? scale_x1 + pad : src_x1);
	uint32_t pad_y1 = ((scale_y1 + pad < src_y1) ? scale_y1 + pad : src_y1);

	uint32_t hor_phase = (((uint32_t) scale_x0 - pad_x0) << shift) + (offset_x & BGRC_PACKET_P_SCALER_STEP_FRAC_MASK) +
		BGRC_PACKET_P_FIXED_SHIFT_RIGHT(hor_step - BGRC_PACKET_P_SCALER_STEP_FRAC_ONE, 1);
	uint32_t ver_phase = (((uint32_t) scale_y0 - pad_y0) << shift) + (offset_y & BGRC_PACKET_P_SCALER_STEP_FRAC_MASK) +
		BGRC_PACKET_P_FIXED_SHIFT_RIGHT(ver_step - BGRC_PACKET_P_SCALER_STEP_FRAC_ONE, 1);

	bool adjust_ver = (packet->update_rect.height == 1) && ((ver_phase >> BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) >= 3);
	uint32_t new_pad_y0 = pad_y0 + (adjust_ver ? 1 : 0);

	uint32_t src_pos = pad_x0 | (new_pad_y0 << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t out_pos = packet->update_rect.x | ((uint32_t) packet->update_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t src_size = (pad_y1 - new_pad_y0) | ((pad_x1 - pad_x0) << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
	uint32_t out_size = packet->update_rect.height | ((uint32_t) packet->update_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));

	BM2MC_PACKET_Rectangle src_rect;
	src_rect.x = pad_x0;
	src_rect.y = pad_y0;
	src_rect.width = pad_x1 - pad_x0;
	src_rect.height = pad_y1 - new_pad_y0;

	BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &src_rect, &packet->update_rect );
	hContext->scaler_header = header;

	hContext->scaler.input_stripe_width += 0x7FFF;
	hContext->scaler.input_stripe_width &= ~0x7FFF;

	BGRC_PACKET_P_422_ODD_POSITION_FIX( src_pos );

	BGRC_PACKET_P_BLOCK_SPLIT_FIFO_FIX_2_SRC( pad_x, pad_w, packet->update_rect.x, packet->update_rect.width );

	BGRC_PACKET_P_DEBUG_PRINT( "-- UpdateScaleBlit    " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, STRIPE_ENABLE, DISABLE, ENABLE, hContext->scaler.stripe_count > 1 );
	BGRC_PACKET_P_STORE_RECT_REGS( src_pos, src_size, src_pos, src_size, out_pos, out_size, out_pos, out_size );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH, hContext->scaler.input_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH_1, hContext->scaler.input_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_STRIPE_WIDTH, hContext->scaler.output_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP_1, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, packet->src_rect.width >= packet->out_rect.width );
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, hor_phase, hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, hor_phase, hor_step );
#endif
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, ver_phase, ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, ver_phase, ver_step );
#endif
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
}

/***************************************************************************/
#if (BCHP_CHIP==7401) || (BCHP_CHIP==7400)
#define BGRC_PACKET_P_SW_DESTRIPE
#endif

/***************************************************************************/
static void BGRC_PACKET_P_SubmitDestripeBlitPacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
#if defined(BCHP_M2MC_BLIT_SRC_STRIPE_HEIGHT_WIDTH_0)
#ifdef BGRC_PACKET_P_SW_DESTRIPE

#if 1

	BM2MC_PACKET_PacketDestripeBlit *packet = (BM2MC_PACKET_PacketDestripeBlit *) header;
	BGRC_PACKET_P_Stripe stripe;

	if( hContext->scaler.stripe_num == 0 )
	{
		BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect );
		hContext->scaler_header = header;
	}

	BGRC_PACKET_P_SetStriping( hContext, &packet->src_rect, &packet->out_rect, &packet->dst_point, 
		&hContext->scaler, &stripe, hContext->scaler.stripe_num );
	hContext->scaler.stripe_num++;

	{
		uint32_t src_pos0 = stripe.src_x | ((uint32_t) packet->src_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t src_pos1 = (stripe.src_x / 2) | ((uint32_t) (packet->src_rect.y / 2) << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t out_pos = stripe.out_x | ((uint32_t) packet->out_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t dst_pos = stripe.dst_x | ((uint32_t) packet->dst_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t src_size0 = packet->src_rect.height | ((uint32_t) stripe.src_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t src_size1 = ((packet->src_rect.height + 1) / 2) | ((uint32_t) ((stripe.src_width + 1) / 2) << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t out_size = packet->out_rect.height | ((uint32_t) stripe.out_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t stripe_width = packet->source_stripe_width >> 7;

		BGRC_PACKET_P_DEBUG_PRINT( "-- DestripeBlit       " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
		BGRC_PACKET_P_STORE_RECT_REGS( src_pos0, src_size0, src_pos1, src_size1, dst_pos, out_size, out_pos, out_size );
		BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, stripe_width |
			(packet->luma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_HEIGHT_SHIFT)) );
		BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, stripe_width |
			(packet->chroma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_1_STRIPE_HEIGHT_SHIFT)) );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, stripe.src_width >= stripe.out_width );
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, stripe.hor_phase, hContext->scaler.hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, stripe.hor_phase / 2, hContext->scaler.hor_step / 2 );
#endif
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, hContext->scaler.ver_phase / 2, hContext->scaler.ver_step / 2 );
#endif
		BGRC_PACKET_P_DEBUG_PRINT( "\n" );
	}

#else

	BM2MC_PACKET_PacketDestripeBlit *packet = (BM2MC_PACKET_PacketDestripeBlit *) header;
	BGRC_PACKET_P_Stripe luma_stripe;
	BGRC_PACKET_P_Stripe chroma_stripe;
	BGRC_PACKET_P_Scaler chroma_scaler;
	BM2MC_PACKET_Rectangle chroma_rect;

	if( hContext->destripe_fixed_scale.chroma_rect.width )
	{
		chroma_rect = hContext->destripe_fixed_scale.chroma_rect;
	}
	else
	{
		chroma_rect.x = packet->src_rect.x / 2;
		chroma_rect.y = packet->src_rect.y / 2;
		chroma_rect.width = (packet->src_rect.width + 1) / 2;
		chroma_rect.height = (packet->src_rect.height + 1) / 2;
	}

	if( hContext->scaler.stripe_num == 0 )
	{
		BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect );
		BGRC_PACKET_P_SetScaler( hContext, &chroma_scaler, &chroma_rect, &packet->out_rect );
		chroma_scaler.input_stripe_width /= 2;
		chroma_scaler.stripe_count = hContext->scaler.stripe_count;

		hContext->scaler_header = header;
	}

	BGRC_PACKET_P_SetStriping( hContext, &packet->src_rect, &packet->out_rect, &packet->dst_point, 
		&hContext->scaler, &luma_stripe, hContext->scaler.stripe_num );
	BGRC_PACKET_P_SetStriping( hContext, &chroma_rect, &packet->out_rect, &packet->dst_point, 
		&chroma_scaler, &chroma_stripe, hContext->scaler.stripe_num );
	hContext->scaler.stripe_num++;

	{
		uint32_t src_pos0 = luma_stripe.src_x | ((uint32_t) packet->src_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t src_pos1 = chroma_stripe.src_x | ((uint32_t) chroma_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t out_pos = luma_stripe.out_x | ((uint32_t) packet->out_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t dst_pos = luma_stripe.dst_x | ((uint32_t) packet->dst_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
		uint32_t src_size0 = packet->src_rect.height | ((uint32_t) luma_stripe.src_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t src_size1 = chroma_rect.height | ((uint32_t) chroma_stripe.src_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t out_size = packet->out_rect.height | ((uint32_t) luma_stripe.out_width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
		uint32_t stripe_width = packet->source_stripe_width >> 7;

		BGRC_PACKET_P_DEBUG_PRINT( "-- DestripeBlit       " );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, STRIPE_ENABLE, DISABLE );
		BGRC_PACKET_P_STORE_RECT_REGS( src_pos0, src_size0, src_pos1, src_size1, dst_pos, out_size, out_pos, out_size );
		BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, stripe_width |
			(packet->luma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_HEIGHT_SHIFT)) );
		BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, stripe_width |
			(packet->chroma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_1_STRIPE_HEIGHT_SHIFT)) );
		BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
		BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, luma_stripe.src_width >= luma_stripe.out_width );
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, luma_stripe.hor_phase, hContext->scaler.hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, chroma_stripe.hor_phase, hContext->scaler.hor_step / 2 );
#endif
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
		BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, hContext->scaler.ver_phase / 2, hContext->scaler.ver_step / 2 );
#endif
		BGRC_PACKET_P_DEBUG_PRINT( "\n" );
	}

#endif

#else
	BM2MC_PACKET_PacketDestripeBlit *packet = (BM2MC_PACKET_PacketDestripeBlit *) header;
	uint32_t src_width_chroma = hContext->destripe_fixed_scale.chroma_rect.width;
	uint32_t src_height_chroma = hContext->destripe_fixed_scale.chroma_rect.height;
	uint32_t src_x0 = packet->src_rect.x;
	uint32_t src_y0 = packet->src_rect.y;
	uint32_t src_x1 = src_width_chroma ? hContext->destripe_fixed_scale.chroma_rect.x : (src_x0 / 2);
	uint32_t src_y1 = src_height_chroma ? hContext->destripe_fixed_scale.chroma_rect.y : (src_y0 / 2);
	uint32_t src_width0 = packet->src_rect.width;
	uint32_t src_height0 = packet->src_rect.height;
	uint32_t src_width1 = src_width_chroma ? src_width_chroma : ((src_width0 > 1) ? src_width0 / 2 : 1);
	uint32_t src_height1 = src_height_chroma ? src_height_chroma : ((src_height0 > 1) ? src_height0 / 2 : 1);
	uint32_t src_pos0 = src_x0 | (src_y0 << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t src_pos1 = src_x1 | (src_y1 << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t out_pos = packet->out_rect.x | ((uint32_t) packet->out_rect.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t dst_pos = packet->dst_point.x | ((uint32_t) packet->dst_point.y << BGRC_M2MC(BLIT_OUTPUT_TOP_LEFT_TOP_SHIFT));
	uint32_t src_size0 = src_height0 | (src_width0 << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
	uint32_t src_size1 = src_height1 | (src_width1 << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
	uint32_t out_size = packet->out_rect.height | ((uint32_t) packet->out_rect.width << BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT));
	uint32_t stripe_width = packet->source_stripe_width >> 7;

	BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect );

	BGRC_PACKET_P_DEBUG_PRINT( "-- DestripeBlit       " );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( BLIT_CTRL, STRIPE_ENABLE, DISABLE, ENABLE, hContext->scaler.stripe_count > 1 );
	BGRC_PACKET_P_STORE_RECT_REGS( src_pos0, src_size0, src_pos1, src_size1, dst_pos, out_size, out_pos, out_size );
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_0, stripe_width |
		(packet->luma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_0_STRIPE_HEIGHT_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( BLIT_SRC_STRIPE_HEIGHT_WIDTH_1, stripe_width |
		(packet->chroma_stripe_height << BGRC_M2MC(BLIT_SRC_STRIPE_HEIGHT_WIDTH_1_STRIPE_HEIGHT_SHIFT)) );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH, hContext->scaler.input_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_INPUT_STRIPE_WIDTH_1, hContext->scaler.input_stripe_width / 2 );
	BGRC_PACKET_P_STORE_REG( BLIT_OUTPUT_STRIPE_WIDTH, hContext->scaler.output_stripe_width );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG( BLIT_STRIPE_OVERLAP_1, hContext->scaler.stripe_overlap );
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_HEADER, SRC_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_DEBUG_PRINT( "\n                      " );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, HORIZ_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD( SCALER_CTRL, VERT_SCALER_ENABLE, ENABLE );
	BGRC_PACKET_P_STORE_REG_FIELD_COMP( SCALER_CTRL, SCALER_ORDER, VERT_THEN_HORIZ, HORIZ_THEN_VERT, packet->src_rect.width >= packet->out_rect.width );
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 0, hContext->scaler.hor_phase, hContext->scaler.hor_step );
#if defined(BCHP_M2MC_HORIZ_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( HORIZ, 1, 
		src_width_chroma ? hContext->scaler.hor_chroma_phase : (hContext->scaler.hor_phase / 2), hContext->scaler.hor_step / 2 );
#endif
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 0, hContext->scaler.ver_phase, hContext->scaler.ver_step );
#if defined(BCHP_M2MC_VERT_FIR_1_COEFF_PHASE0_01)
	BGRC_PACKET_P_STORE_SCALE_REGS( VERT, 1, 
		src_height_chroma ? hContext->scaler.ver_chroma_phase : (hContext->scaler.ver_phase / 2), hContext->scaler.ver_step / 2 );
#endif
	BGRC_PACKET_P_DEBUG_PRINT( "\n" );
#endif
#endif
}

/***************************************************************************/
#define BGRC_PACKET_P_RESET_COLOR_BLEND { \
	BM2MC_PACKET_BlendFactor_eSourceColor, BM2MC_PACKET_BlendFactor_eOne, false, \
	BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero }

#define BGRC_PACKET_P_RESET_ALPHA_BLEND { \
	BM2MC_PACKET_BlendFactor_eSourceAlpha, BM2MC_PACKET_BlendFactor_eOne, false, \
	BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero }

#define BGRC_PACKET_P_RESET_ROP_VECTOR    0xCC

/***************************************************************************/
void BGRC_PACKET_P_ResetState( BGRC_PacketContext_Handle hContext )
{
	BM2MC_PACKET_PacketSourceNone src_packet;
	BM2MC_PACKET_PacketDestinationNone dst_packet;
	BM2MC_PACKET_PacketOutputFeeder out_packet;
	BM2MC_PACKET_PacketBlend blend_packet;
	BM2MC_PACKET_PacketRop rop_packet;
	BM2MC_PACKET_PacketFilter filter_packet;
	BM2MC_PACKET_PacketSourcePalette palette_packet;
	BM2MC_PACKET_PacketFillBlit fill_packet;
	BM2MC_PACKET_Blend color_blend = BGRC_PACKET_P_RESET_COLOR_BLEND;
	BM2MC_PACKET_Blend alpha_blend = BGRC_PACKET_P_RESET_ALPHA_BLEND;

	/* clear registers */
	BGRC_PACKET_P_DEBUG_PRINT( "-- Reset              \n" );
	BKNI_Memset( hContext->stored_registers, 0, sizeof (uint32_t) * BGRC_PACKET_P_REGISTER_COUNT );

	/* set default surfaces */
	BM2MC_PACKET_INIT( &src_packet, SourceNone, false );
	BGRC_PACKET_P_SubmitSourceNonePacket( hContext, (BM2MC_PACKET_Header *) &src_packet );

	BM2MC_PACKET_INIT( &dst_packet, DestinationNone, false );
	BGRC_PACKET_P_SubmitDestinationNonePacket( hContext, (BM2MC_PACKET_Header *) &dst_packet );

	BM2MC_PACKET_INIT( &out_packet, OutputFeeder, false );
	out_packet.plane.address = hContext->sync_output_offset;
	out_packet.plane.pitch = 4;
	out_packet.plane.format = BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8;
	out_packet.plane.width = 1;
	out_packet.plane.height = 1;
	BGRC_PACKET_P_SubmitOutputFeederPacket( hContext, (BM2MC_PACKET_Header *) &out_packet );

	/* set default blend - source pixel */
	BM2MC_PACKET_INIT( &blend_packet, Blend, false );
	blend_packet.color_blend = color_blend;
	blend_packet.alpha_blend = alpha_blend;
	blend_packet.color = 0;
	BGRC_PACKET_P_SubmitBlendPacket( hContext, (BM2MC_PACKET_Header *) &blend_packet );

	/* set default ROP - source copy */
	BM2MC_PACKET_INIT( &rop_packet, Rop, false );
	rop_packet.rop = BGRC_PACKET_P_RESET_ROP_VECTOR;
	rop_packet.pattern0 = 0;
	rop_packet.pattern1 = 0;
	rop_packet.color0 = 0;
	rop_packet.color1 = 0;
	BGRC_PACKET_P_SubmitRopPacket( hContext, (BM2MC_PACKET_Header *) &rop_packet );

	/* set default filter - bilinear */
	BM2MC_PACKET_INIT( &filter_packet, Filter, false );
	filter_packet.hor = s_BGRC_PACKET_P_DeviceFilter_Bilinear;
	filter_packet.ver = s_BGRC_PACKET_P_DeviceFilter_Bilinear;
	BGRC_PACKET_P_SubmitFilterPacket( hContext, (BM2MC_PACKET_Header *) &filter_packet );

	/* set default palette */
	BM2MC_PACKET_INIT( &palette_packet, SourcePalette, false );
	palette_packet.address = 0;
	BGRC_PACKET_P_SubmitSourcePalettePacket( hContext, (BM2MC_PACKET_Header *) &palette_packet );

	/* set default rectangle*/
	BM2MC_PACKET_INIT( &fill_packet, FillBlit, false );
	fill_packet.rect.x = 0;
	fill_packet.rect.y = 0;
	fill_packet.rect.width = 1;
	fill_packet.rect.height = 1;
	BGRC_PACKET_P_SubmitFillBlitPacket( hContext, (BM2MC_PACKET_Header *) &fill_packet );

	BKNI_Memset( &hContext->destripe_fixed_scale, 0, sizeof (BM2MC_PACKET_PacketDestripeFixedScale) );
	BKNI_Memset( &hContext->fixed_scale, 0, sizeof (BM2MC_PACKET_PacketFixedScale) );

	hContext->src_format0 = 0;
	hContext->out_format0 = 0;
	hContext->linear_destripe = false;
	hContext->reset_state = true;

#if defined(BCHP_M2MC_BLIT_CTRL_BLOCK_AUTO_SPLIT_FIFO_MASK)
	BGRC_PACKET_P_STORE_REG_FIELD( BLIT_CTRL, BLOCK_AUTO_SPLIT_FIFO, ENABLE );
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitResetStatePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BSTD_UNUSED(header);
	BGRC_PACKET_P_ResetState( hContext );
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitSaveStatePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BSTD_UNUSED(header);
	BKNI_Memcpy( hContext->saved_registers, hContext->stored_registers, BGRC_PACKET_P_REGISTER_COUNT * sizeof (uint32_t) );
	hContext->saved_destripe_fixed_scale = hContext->destripe_fixed_scale;
	hContext->saved_fixed_scale = hContext->fixed_scale;
	hContext->saved_src_format0 = hContext->src_format0;
	hContext->saved_out_format0 = hContext->out_format0;
	hContext->saved_linear_destripe = hContext->linear_destripe;

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->saved_SRC_surface_width = hContext->SRC_surface_width;
	hContext->saved_SRC_surface_height = hContext->SRC_surface_height;
	hContext->saved_SRC_surface_format = hContext->SRC_surface_format;
	hContext->saved_DEST_surface_width = hContext->DEST_surface_width;
	hContext->saved_DEST_surface_height = hContext->DEST_surface_height;
	hContext->saved_DEST_surface_format = hContext->DEST_surface_format;
	hContext->saved_OUTPUT_surface_width = hContext->OUTPUT_surface_width;
	hContext->saved_OUTPUT_surface_height = hContext->OUTPUT_surface_height;
	hContext->saved_OUTPUT_surface_format = hContext->OUTPUT_surface_format;
#endif
}

/***************************************************************************/
static void BGRC_PACKET_P_SubmitRestoreStatePacket( BGRC_PacketContext_Handle hContext, BM2MC_PACKET_Header *header )
{
	BSTD_UNUSED(header);
	BKNI_Memcpy( hContext->stored_registers, hContext->saved_registers, BGRC_PACKET_P_REGISTER_COUNT * sizeof (uint32_t) );
	hContext->destripe_fixed_scale = hContext->saved_destripe_fixed_scale;
	hContext->fixed_scale = hContext->saved_fixed_scale;
	hContext->src_format0 = hContext->saved_src_format0;
	hContext->out_format0 = hContext->saved_out_format0;
	hContext->linear_destripe = hContext->saved_linear_destripe;
	hContext->reset_state = true;

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
	hContext->SRC_surface_width = hContext->saved_SRC_surface_width;
	hContext->SRC_surface_height = hContext->saved_SRC_surface_height;
	hContext->SRC_surface_format = hContext->saved_SRC_surface_format;
	hContext->DEST_surface_width = hContext->saved_DEST_surface_width;
	hContext->DEST_surface_height = hContext->saved_DEST_surface_height;
	hContext->DEST_surface_format = hContext->saved_DEST_surface_format;
	hContext->OUTPUT_surface_width = hContext->saved_OUTPUT_surface_width;
	hContext->OUTPUT_surface_height = hContext->saved_OUTPUT_surface_height;
	hContext->OUTPUT_surface_format = hContext->saved_OUTPUT_surface_format;
#endif
}

/***************************************************************************/
static uint32_t *BGRC_PACKET_P_SubmitPacket(
	BGRC_PacketContext_Handle hContext,
	BM2MC_PACKET_Header *header,
	bool *force_striping )
{
	uint32_t *packet = (uint32_t *) header;

	switch( header->type )
	{
	case 0:
		BGRC_PACKET_P_DEBUG_PRINT( "-- EndOfBuffer        \n" );
		packet = (uint32_t *) ((uint8_t *) hContext->buffer_base_addr - header->size * sizeof (uint32_t));
		break;
	case BM2MC_PACKET_PacketType_eSourceFeeder:
		BGRC_PACKET_P_SubmitSourceFeederPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceFeeders:
		BGRC_PACKET_P_SubmitSourceFeedersPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceColor:
		BGRC_PACKET_P_SubmitSourceColorPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceNone:
		BGRC_PACKET_P_SubmitSourceNonePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceControl:
		BGRC_PACKET_P_SubmitSourceControlPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationFeeder:
		BGRC_PACKET_P_SubmitDestinationFeederPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationColor:
		BGRC_PACKET_P_SubmitDestinationColorPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationNone:
		BGRC_PACKET_P_SubmitDestinationNonePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationControl:
		BGRC_PACKET_P_SubmitDestinationControlPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eOutputFeeder:
		BGRC_PACKET_P_SubmitOutputFeederPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eOutputControl:
		BGRC_PACKET_P_SubmitOutputControlPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eBlend:
		BGRC_PACKET_P_SubmitBlendPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eBlendColor:
		BGRC_PACKET_P_SubmitBlendColorPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eRop:
		BGRC_PACKET_P_SubmitRopPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceColorkey:
		BGRC_PACKET_P_SubmitSourceColorkeyPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceColorkeyEnable:
		BGRC_PACKET_P_SubmitSourceColorkeyEnablePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationColorkey:
		BGRC_PACKET_P_SubmitDestinationColorkeyPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestinationColorkeyEnable:
		BGRC_PACKET_P_SubmitDestinationColorkeyEnablePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eFilter:
		BGRC_PACKET_P_SubmitFilterPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eFilterEnable:
		BGRC_PACKET_P_SubmitFilterEnablePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eFilterControl:
		BGRC_PACKET_P_SubmitFilterControlPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceColorMatrix:
		BGRC_PACKET_P_SubmitSourceColorMatrixPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourceColorMatrixEnable:
		BGRC_PACKET_P_SubmitSourceColorMatrixEnablePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSourcePalette:
		BGRC_PACKET_P_SubmitSourcePalettePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eMirror:
		BGRC_PACKET_P_SubmitMirrorPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eFixedScale:
		BGRC_PACKET_P_SubmitFixedScalePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestripeFixedScale:
		BGRC_PACKET_P_SubmitDestripeFixedScalePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eAlphaPremultiply:
		BGRC_PACKET_P_SubmitAlphaPremultiplyPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eAlphaDemultiply:
		BGRC_PACKET_P_SubmitAlphaDemultiplyPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eDestAlphaPremultiply:
		BGRC_PACKET_P_SubmitDestAlphaPremultiplyPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eFillBlit:
	    {
			BM2MC_PACKET_PacketFillBlit *packet = (BM2MC_PACKET_PacketFillBlit *) header;
			BGRC_PACKET_P_OUT_PXL_422_ALIGN(packet->rect.x, packet->rect.width);
		BGRC_PACKET_P_SubmitFillBlitPacket( hContext, header );
		break;
		}
	case BM2MC_PACKET_PacketType_eCopyBlit:
	    {
			BM2MC_PACKET_PacketCopyBlit *packet = (BM2MC_PACKET_PacketCopyBlit *) header;
			BGRC_PACKET_P_SRC_OUT_PXL_422_ALIGN(packet->src_rect.x, packet->src_rect.width, packet->out_point.x);
		BGRC_PACKET_P_SubmitCopyBlitPacket( hContext, header );
		break;
		}
	case BM2MC_PACKET_PacketType_eBlendBlit:
	    {
			BM2MC_PACKET_PacketBlendBlit *packet = (BM2MC_PACKET_PacketBlendBlit *) header;
			BGRC_PACKET_P_SRC_DST_OUT_PXL_422_ALIGN(packet->src_rect.x, packet->src_rect.width, packet->dst_point.x, packet->out_point.x);
		BGRC_PACKET_P_SubmitBlendBlitPacket( hContext, header );
		break;
		}
	case BM2MC_PACKET_PacketType_eScaleBlit:
		{
			BM2MC_PACKET_PacketScaleBlit *packet = (BM2MC_PACKET_PacketScaleBlit *) header;
			BGRC_PACKET_P_SRC_PXL_422_ALIGN(packet->src_rect.x, packet->src_rect.width);
			BGRC_PACKET_P_OUT_PXL_422_ALIGN(packet->out_rect.x, packet->out_rect.width);
			BGRC_PACKET_P_SetScaler( hContext, &hContext->scaler, &packet->src_rect, &packet->out_rect );

			/* use sw striping if scale down max exceeded */
			if( (BGRC_PACKET_P_GET_REG_FIELD(SRC_SURFACE_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) ||
				(BGRC_PACKET_P_GET_REG(BLIT_CTRL) & BGRC_M2MC(BLIT_CTRL_OUTPUT_H_DIRECTION_MASK)) ||
				(hContext->scaler.ver_step > (4 << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS)) )
			{
				BGRC_PACKET_P_SubmitStripeBlitPacket( hContext, &packet->src_rect, &packet->out_rect, 
					(BM2MC_PACKET_Point *) (void *) &packet->out_rect );

				hContext->scaler_header = header;
				*force_striping = true;
			}
			else
			{
				BGRC_PACKET_P_SubmitScaleBlitPacket( hContext, header );
			}
		}
		break;
	case BM2MC_PACKET_PacketType_eScaleBlendBlit:
	{
		BM2MC_PACKET_PacketScaleBlendBlit *packet = (BM2MC_PACKET_PacketScaleBlendBlit *) header;
		BGRC_PACKET_P_SRC_PXL_422_ALIGN(packet->src_rect.x, packet->src_rect.width);
		BGRC_PACKET_P_DST_PXL_422_ALIGN(packet->dst_point.x, packet->out_rect.width);
		BGRC_PACKET_P_OUT_PXL_422_ALIGN(packet->out_rect.x, packet->out_rect.width);
		BGRC_PACKET_P_SubmitScaleBlendBlitPacket( hContext, header );
		break;
	}
	case BM2MC_PACKET_PacketType_eUpdateScaleBlit:
	{
		BM2MC_PACKET_PacketUpdateScaleBlit *packet = (BM2MC_PACKET_PacketUpdateScaleBlit *) header;
		BGRC_PACKET_P_SRC_PXL_422_ALIGN(packet->src_rect.x, packet->src_rect.width);
		BGRC_PACKET_P_OUT_PXL_422_ALIGN(packet->out_rect.x, packet->out_rect.width);
		BGRC_PACKET_P_SubmitUpdateScaleBlitPacket( hContext, header );
		break;
	}
	case BM2MC_PACKET_PacketType_eDestripeBlit:
		/* not 4:2:2, no need to do 4:2:2 align */
		BGRC_PACKET_P_SubmitDestripeBlitPacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eResetState:
		BGRC_PACKET_P_SubmitResetStatePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eSaveState:
		BGRC_PACKET_P_SubmitSaveStatePacket( hContext, header );
		break;
	case BM2MC_PACKET_PacketType_eRestoreState:
		BGRC_PACKET_P_SubmitRestoreStatePacket( hContext, header );
		break;
	default:
		break;
	}

	return (uint32_t *) ((uint8_t *) packet + header->size * sizeof (uint32_t));
}

/***************************************************************************/
#define BGRC_PACKET_P_GET_SURFACE_BPP( surface ) (( \
	BGRC_PACKET_P_GET_REG_FIELD_SHIFT(surface##_FORMAT_DEF_1, CH0_NUM_BITS) + \
	BGRC_PACKET_P_GET_REG_FIELD_SHIFT(surface##_FORMAT_DEF_1, CH1_NUM_BITS) + \
	BGRC_PACKET_P_GET_REG_FIELD_SHIFT(surface##_FORMAT_DEF_1, CH2_NUM_BITS) + \
	BGRC_PACKET_P_GET_REG_FIELD_SHIFT(surface##_FORMAT_DEF_1, CH3_NUM_BITS)) / \
	((BGRC_PACKET_P_GET_REG_FIELD(surface##_FORMAT_DEF_1, FORMAT_TYPE) == M2MC_FT_YCbCr422) ? 2 : 1))

/***************************************************************************/
#define BGRC_PACKET_P_VERIFY_RECTANGLE_SCALE() \
{ \
	if( hContext->scaler.hor_step > (BGRC_P_SCALE_DOWN_MAX << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) ) \
	{ \
		BDBG_ERR(( "Horizontal down scale factor larger than %dx (%d->%d)", BGRC_P_SCALE_DOWN_MAX, \
			BGRC_PACKET_P_GET_REG(BLIT_SRC_SIZE) >> BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT), \
			BGRC_PACKET_P_GET_REG(BLIT_OUTPUT_SIZE) >> BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT) )); \
		hContext->scaler.hor_step = 0; \
		BGRC_PACKET_P_STORE_REG( HORIZ_SCALER_STEP, BGRC_P_SCALE_DOWN_MAX << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS ); \
/*		violation = true; */\
	} \
	if( hContext->scaler.ver_step > (BGRC_P_SCALE_DOWN_MAX_Y << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) ) \
	{ \
		BDBG_ERR(( "Vertical down scale factor larger than %dx (%d->%d)", BGRC_P_SCALE_DOWN_MAX_Y, \
			BGRC_PACKET_P_GET_REG(BLIT_SRC_SIZE) & 0xFFFF, BGRC_PACKET_P_GET_REG(BLIT_OUTPUT_SIZE) & 0xFFFF)); \
		hContext->scaler.ver_step = 0; \
		BGRC_PACKET_P_STORE_REG( VERT_SCALER_STEP, BGRC_P_SCALE_DOWN_MAX_Y << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS ); \
/*		violation = true; */\
	} \
	if( BGRC_PACKET_P_GET_REG(BLIT_SRC_SIZE) & (~(BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_MASK) | BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_HEIGHT_MASK))) ) \
	{ \
		BDBG_ERR(( "Source surface rectangle is larger than 8191 (%dx%d)", \
			BGRC_PACKET_P_GET_REG(BLIT_SRC_SIZE) >> BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT), \
			BGRC_PACKET_P_GET_REG(BLIT_SRC_SIZE) & 0xFFFF )); \
		violation = true; \
	} \
	if( BGRC_PACKET_P_GET_REG(BLIT_OUTPUT_SIZE) & (~(BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_MASK) | BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_HEIGHT_MASK))) ) \
	{ \
		BDBG_ERR(( "Output surface rectangle is larger than 8191 (%dx%d)", \
			BGRC_PACKET_P_GET_REG(BLIT_OUTPUT_SIZE) >> BGRC_M2MC(BLIT_OUTPUT_SIZE_SURFACE_WIDTH_SHIFT), \
			BGRC_PACKET_P_GET_REG(BLIT_OUTPUT_SIZE) & 0xFFFF )); \
		violation = true; \
	} \
}

/***************************************************************************/
#define BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_BOUNDS( surface, num, pos, dim, mask ) \
{ \
	if( BGRC_PACKET_P_GET_REG(surface##_FEEDER_ENABLE) ) \
	{ \
		uint32_t surface_offset = BGRC_PACKET_P_GET_REG(surface##_SURFACE_ADDR_0); \
		if( surface_offset != hContext->sync_output_offset ) \
		{ \
			if( surface_offset ) \
			{ \
				uint32_t bpp = BGRC_PACKET_P_GET_SURFACE_BPP(surface##_##num); \
				uint32_t pitch = BGRC_PACKET_P_GET_REG(surface##_SURFACE_STRIDE_0); \
				uint32_t x = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_##pos, LEFT); \
				uint32_t y = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_##pos, TOP); \
				uint32_t w = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_##dim, SURFACE_WIDTH); \
				uint32_t h = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_##dim, SURFACE_HEIGHT); \
				uint32_t start_offset = surface_offset + (x * bpp) / 8 + y * pitch; \
				uint32_t end_offset = surface_offset + ((x + w) * bpp) / 8 + (y + h - 1) * pitch; \
\
				if( start_offset < hContext->create_settings.memory_bounds.offset ) \
				{ \
					BDBG_ERR(( "%6s context memory violation (bounds=%08x-%08x start=%08x)   sur (off=%08x x=%4d y=%4d w=%4d h=%4d)", \
						#surface, hContext->create_settings.memory_bounds.offset, \
						hContext->create_settings.memory_bounds.offset + hContext->create_settings.memory_bounds.size, \
						start_offset, surface_offset, x, y, w, h )); \
					violation = true; \
				} \
\
				if( end_offset > hContext->create_settings.memory_bounds.offset + hContext->create_settings.memory_bounds.size ) \
				{ \
					BDBG_ERR(( "%6s context memory violation (bounds=%08x-%08x   end=%08x)   sur (off=%08x x=%4d y=%4d w=%4d h=%4d)", \
						#surface, hContext->create_settings.memory_bounds.offset, \
						hContext->create_settings.memory_bounds.offset + hContext->create_settings.memory_bounds.size, \
						end_offset, surface_offset, x, y, w, h )); \
					violation = true; \
				} \
			} \
			else if( (mask) ) \
			{ \
				if( (BGRC_PACKET_P_GET_REG(surface##_SURFACE_FORMAT_DEF_3) & (mask)) != (mask) ) \
				{ \
					BDBG_ERR(( "%6s feeder violates memory at offset 0." )); \
					violation = true; \
				} \
			} \
		} \
	} \
}

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
/***************************************************************************/
#define BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_SURFACE( surface ) \
{ \
	if( BGRC_PACKET_P_GET_REG(surface##_FEEDER_ENABLE) ) \
	{ \
		uint32_t surface_offset = BGRC_PACKET_P_GET_REG(surface##_SURFACE_ADDR_0); \
		if( surface_offset != hContext->sync_output_offset ) \
		{ \
			if( surface_offset ) \
			{ \
				uint32_t bpp = BGRC_PACKET_P_GET_SURFACE_BPP(surface##_SURFACE); \
				uint32_t pitch = BGRC_PACKET_P_GET_REG(surface##_SURFACE_STRIDE_0); \
				uint32_t x = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_TOP_LEFT, LEFT); \
				uint32_t y = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_TOP_LEFT, TOP); \
				uint32_t w = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_SIZE, SURFACE_WIDTH); \
				uint32_t h = BGRC_PACKET_P_GET_REG_FIELD_SHIFT(BLIT_##surface##_SIZE, SURFACE_HEIGHT); \
				uint32_t end_offset = surface_offset + ((x + w) * bpp) / 8 + (y + h - 1) * pitch; \
				uint32_t surface_size = hContext->surface##_surface_height * pitch; \
\
				if( surface_size && (end_offset > surface_offset + surface_size) ) \
				{ \
					BDBG_ERR(( "%6s surface memory violation (bounds=%08x-%08x   end=%08x)   sur (off=%08x x=%4d y=%4d w=%4d h=%4d bpp=%d) %08x %08x", \
						#surface, surface_offset, surface_offset + surface_size, end_offset, surface_offset, x, y, w, h, bpp, BGRC_PACKET_P_GET_REG(BLIT_##surface##_SIZE), BGRC_PACKET_P_GET_REG(BLIT_##surface##_TOP_LEFT) )); \
					violation = true; \
				} \
\
				if( hContext->surface##_surface_format >= BM2MC_PACKET_PixelFormat_eMax ) \
				{ \
					BDBG_ERR(( "%6s surface format invalid", \
						#surface, surface_offset, surface_offset + surface_size, end_offset, surface_offset, x, y, w, h )); \
					violation = true; \
				} \
			} \
		} \
	} \
}
#endif

/***************************************************************************/
void BGRC_PACKET_P_SubmitPackets(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext )
{
	/* get first packet and group header */
	uint32_t *packet = (uint32_t *) ((uint8_t *) hContext->buffer_base_addr + hContext->buffer_submit_offset);
	uint32_t *next_packet = packet;
	uint32_t group_header = 0;
	bool execute = false;
	bool sync_interrupt = false;
	bool advance_interrupt = false;
	bool first_blit = true;

	uint32_t *start_fifo_addr = 0;
	uint32_t start_fifo_offset = 0;
	uint32_t ii;

	/* check if context changing */
	if( hGrc->hContext != hContext )
		hContext->group_header = BGRC_PACKET_P_DEVICE_GROUP_HEADER_FULL;

	/* get m2mc's current offset */
	hContext->m2mc_offset = BGRC_P_ReadReg32( hGrc->hRegister, LIST_CURR_PKT_ADDR );

	/* clear scaler's stripe counts */
	hContext->scaler.stripe_count = 0;
	hContext->scaler.stripe_num = 0;

	/* loop through packets */
	while( hContext->buffer_submit_size )
	{
		BM2MC_PACKET_Header *header = (BM2MC_PACKET_Header *) packet;
		bool force_striping = false;

		/* set group header */
		hContext->group_header |= hContext->reset_state ? BGRC_PACKET_P_DEVICE_GROUP_HEADER_FULL : s_BGRC_PACKET_P_DeviceGroupHeaderMasks[header->type];
		hContext->reset_state = false;
		group_header = hContext->group_header;

		if( header->execute )
		{
			/* exit loop if out of memory */
			if( !BGRC_PACKET_P_AvailableFifoMemory( hGrc, hContext ) ) 
			{
				advance_interrupt = first_blit && (!hGrc->advance_interrupt);

				/* allow allocation for sync blit and for first blit if no advance interrupt pending */
				if( (!header->sync) && (!advance_interrupt) )
					break;
			}

			/* store first blit offset */
			if( start_fifo_offset == 0 )
			{
				start_fifo_addr = hGrc->curr_fifo_addr;
				start_fifo_offset = hGrc->curr_fifo_offset;
			}
		}

		/* submit packet */
		next_packet = BGRC_PACKET_P_SubmitPacket( hContext, header, &force_striping );

		/* decrease size of buffer submitted */
		hContext->buffer_submit_size -= header->size * sizeof (uint32_t);

		/* store last header type for a blit packet */
		if( (header->type >= BM2MC_PACKET_PacketType_eFillBlit) && (header->type <= BM2MC_PACKET_PacketType_eDestripeBlit) )
			hContext->last_blit_type = header->type;

		if( header->execute )
		{
			static bool violation = false;

			BGRC_PACKET_P_VERIFY_RECTANGLE_SCALE();

			/* bounds check memory */
			if( hContext->create_settings.memory_bounds.offset && hContext->create_settings.memory_bounds.size )
			{
				static uint32_t disable_mask =
					BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_CH0_DISABLE_MASK) | BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_CH1_DISABLE_MASK) |
					BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_CH2_DISABLE_MASK) | BGRC_M2MC(DEST_SURFACE_FORMAT_DEF_3_CH3_DISABLE_MASK);

				BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_BOUNDS( SRC, SURFACE, TOP_LEFT, SIZE, disable_mask );
				BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_BOUNDS( DEST, SURFACE, TOP_LEFT, SIZE, disable_mask );
				BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_BOUNDS( OUTPUT, SURFACE, TOP_LEFT, SIZE, 0 );
			}

#ifdef BGRC_PACKET_P_VERIFY_SURFACE_RECTANGLE
			BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_SURFACE( SRC );
			BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_SURFACE( DEST );
			BGRC_PACKET_P_VERIFY_RECTANGLE_WITH_SURFACE( OUTPUT );
#endif
			if( !violation )
			{
				first_blit = false;

				/* handle blit without any register groups */
				if( hContext->group_header == 0 )
					hContext->group_header = BCHP_M2MC_LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK;

				/* write blits */
				BGRC_PACKET_P_WriteFifoMemory( hGrc, hContext, hContext->group_header );
				execute = true;
				group_header = 0;

				/* write scaler stripe blits */
				if( force_striping || 
#ifdef BGRC_PACKET_P_SW_DESTRIPE
					(hContext->last_blit_type == BM2MC_PACKET_PacketType_eDestripeBlit) || 
#endif
					(hContext->last_blit_type == BM2MC_PACKET_PacketType_eScaleBlendBlit) )
				{
					for( ii = 1; ii < hContext->scaler.stripe_count; ++ii )
					{
						hGrc->curr_fifo_addr = (uint32_t *) BGRC_PACKET_P_ALIGN_MEMORY( hGrc->curr_fifo_addr );
						hGrc->curr_fifo_offset = (uint32_t ) BGRC_PACKET_P_ALIGN_MEMORY( hGrc->curr_fifo_offset );

						if( hContext->scaler_header->type == BM2MC_PACKET_PacketType_eScaleBlendBlit )
						{
							BM2MC_PACKET_PacketScaleBlendBlit *scale_packet = (BM2MC_PACKET_PacketScaleBlendBlit *) hContext->scaler_header;
							BGRC_PACKET_P_SubmitStripeBlitPacket( hContext, &scale_packet->src_rect, &scale_packet->out_rect, &scale_packet->dst_point );
						}
#ifdef BGRC_PACKET_P_SW_DESTRIPE
						else if( hContext->scaler_header->type == BM2MC_PACKET_PacketType_eDestripeBlit )
						{
							BGRC_PACKET_P_SubmitDestripeBlitPacket( hContext, hContext->scaler_header );
						}
#endif
						else
						{
							BM2MC_PACKET_PacketScaleBlit *scale_packet = (BM2MC_PACKET_PacketScaleBlit *) hContext->scaler_header;
							BGRC_PACKET_P_SubmitStripeBlitPacket( hContext, &scale_packet->src_rect, &scale_packet->out_rect, 
								(BM2MC_PACKET_Point*) (void *) &scale_packet->out_rect );
						}

						BGRC_PACKET_P_WriteFifoMemory( hGrc, hContext, 
							BGRC_M2MC(LIST_PACKET_HEADER_1_BLIT_GRP_CNTRL_MASK) | BGRC_M2MC(LIST_PACKET_HEADER_1_SCALE_PARAM_GRP_CNTRL_MASK) );
					}
				}

				/* enable blitheader interrupt field */
				if( header->sync && (BGRC_PACKET_P_SyncState_eQueuedInSw == hContext->eSyncState) )
				{
					hContext->sync_pkt_offset = (hGrc->prev_fifo_addr - hGrc->fifo_base_addr) * 4 + hGrc->fifo_base_offset;
					sync_interrupt = true;
					BGRC_PACKET_P_WRITE_DATA( *hContext->last_blitheader_addr, hContext->last_blitheader | BGRC_M2MC(BLIT_HEADER_INTERRUPT_ENABLE_MASK) );
				}

				/* clear group header */
				hContext->group_header = 0;
			}

			/* clear scaler's stripe counts */
			hContext->scaler.stripe_count = 0;
			hContext->scaler.stripe_num = 0;
		}

		packet = next_packet;
	}

	/* adjust buffer offset */
	hContext->buffer_submit_offset = (uint8_t *) packet - (uint8_t *) hContext->buffer_base_addr;

	/* initiate blits */
	if( execute )
	{
		BGRC_PACKET_P_InitiateBlits( hGrc, hContext, start_fifo_addr, start_fifo_offset, 
			hContext->buffer_submit_size || advance_interrupt, sync_interrupt );

		/* save current context */
		hGrc->hContext = hContext;
		hContext->group_header = group_header;
	}
}

/***************************************************************************/
#define BGRC_PACKET_P_SYNC_COLOR_BLEND { \
	BM2MC_PACKET_BlendFactor_eConstantColor, BM2MC_PACKET_BlendFactor_eOne, false, \
	BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero }

#define BGRC_PACKET_P_SYNC_ALPHA_BLEND { \
	BM2MC_PACKET_BlendFactor_eConstantAlpha, BM2MC_PACKET_BlendFactor_eOne, false, \
	BM2MC_PACKET_BlendFactor_eZero, BM2MC_PACKET_BlendFactor_eZero, false, BM2MC_PACKET_BlendFactor_eZero }

/***************************************************************************/
BERR_Code BGRC_PACKET_P_SyncBlit(
	BGRC_Handle hGrc,
	BGRC_PacketContext_Handle hContext )
{
	void *buffer;
	size_t size_out;
	int rc = BGRC_PACKET_MSG_PACKETS_INCOMPLETE;

	if (BGRC_PACKET_P_SyncState_eRequested == hContext->eSyncState)
	{
		rc = BGRC_Packet_GetPacketMemory( hGrc, hContext, &buffer, &size_out, BGRC_PACKET_P_SYNC_PACKET_SIZE );
		if (rc) return BERR_TRACE(rc);
		if (!buffer) return BERR_OUT_OF_SYSTEM_MEMORY;
		else
		{
			BM2MC_PACKET_PacketSaveState *save_packet = (BM2MC_PACKET_PacketSaveState *) ((uint8_t *) buffer);
			BM2MC_PACKET_PacketSourceNone *src_packet = (BM2MC_PACKET_PacketSourceNone *) (save_packet + 1);
			BM2MC_PACKET_PacketDestinationNone *dst_packet = (BM2MC_PACKET_PacketDestinationNone *) (src_packet + 1);
			BM2MC_PACKET_PacketOutputFeeder *out_packet = (BM2MC_PACKET_PacketOutputFeeder *) (dst_packet + 1);
			BM2MC_PACKET_PacketBlend *blend_packet = (BM2MC_PACKET_PacketBlend *) (out_packet + 1);
			BM2MC_PACKET_PacketRop *rop_packet = (BM2MC_PACKET_PacketRop *) (blend_packet + 1);
			BM2MC_PACKET_PacketSourceColorkeyEnable *src_key_packet = (BM2MC_PACKET_PacketSourceColorkeyEnable *) (rop_packet + 1);
			BM2MC_PACKET_PacketDestinationColorkeyEnable *dst_key_packet = (BM2MC_PACKET_PacketDestinationColorkeyEnable *) (src_key_packet + 1);
			BM2MC_PACKET_PacketFilterEnable *filter_packet = (BM2MC_PACKET_PacketFilterEnable *) (dst_key_packet + 1);
			BM2MC_PACKET_PacketFilterControl *control_packet = (BM2MC_PACKET_PacketFilterControl *) (filter_packet + 1);
			BM2MC_PACKET_PacketSourceColorMatrixEnable *matrix_packet = (BM2MC_PACKET_PacketSourceColorMatrixEnable *) (control_packet + 1);
			BM2MC_PACKET_PacketAlphaPremultiply *premult_packet = (BM2MC_PACKET_PacketAlphaPremultiply *) (matrix_packet + 1);
			BM2MC_PACKET_PacketAlphaDemultiply *demult_packet = (BM2MC_PACKET_PacketAlphaDemultiply *) (premult_packet + 1);
			BM2MC_PACKET_PacketDestAlphaPremultiply *destpremult_packet = (BM2MC_PACKET_PacketDestAlphaPremultiply *) (demult_packet + 1);
			BM2MC_PACKET_PacketMirror *mirror_packet = (BM2MC_PACKET_PacketMirror *) (destpremult_packet + 1);
			BM2MC_PACKET_PacketFillBlit *fill_packet = (BM2MC_PACKET_PacketFillBlit *) (mirror_packet + 1);
			BM2MC_PACKET_PacketRestoreState *restore_packet = (BM2MC_PACKET_PacketRestoreState *) (fill_packet + 1);
			BM2MC_PACKET_Blend color_blend = BGRC_PACKET_P_SYNC_COLOR_BLEND;
			BM2MC_PACKET_Blend alpha_blend = BGRC_PACKET_P_SYNC_ALPHA_BLEND;

			BDBG_ASSERT(size_out >= BGRC_PACKET_P_SYNC_PACKET_SIZE);

			/* clear the output pixel of "sync blit" to 0, "sync blit" will fill it with 0xFFFFFFFF,
			 * this will be checked by BGRC_Packet_GetContextStatus, to decide if the "sync blit" is executed */
			hContext->sync_output_cache_addr[0] = 0;
			BMEM_FlushCache( hGrc->hMemory, hContext->sync_output_cache_addr, 4 );

			/* write packets */
			BM2MC_PACKET_INIT( save_packet, SaveState, false );
			BM2MC_PACKET_INIT( src_packet, SourceNone, false );
			BM2MC_PACKET_INIT( dst_packet, DestinationNone, false );

			BM2MC_PACKET_INIT( out_packet, OutputFeeder, false );
			out_packet->plane.address = hContext->sync_output_offset;
			out_packet->plane.pitch = sizeof (uint32_t);
			out_packet->plane.format = BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8;
			out_packet->plane.width = 1;
			out_packet->plane.height = 1;

			BM2MC_PACKET_INIT( blend_packet, Blend, false );
			blend_packet->color_blend = color_blend;
			blend_packet->alpha_blend = alpha_blend;
			blend_packet->color = 0xFFFFFFFF;

			BM2MC_PACKET_INIT( rop_packet, Rop, false );
			rop_packet->rop = BGRC_PACKET_P_RESET_ROP_VECTOR;
			rop_packet->pattern0 = 0;
			rop_packet->pattern1 = 0;
			rop_packet->color0 = 0;
			rop_packet->color1 = 0;

			BM2MC_PACKET_INIT( src_key_packet, SourceColorkeyEnable, false );
			src_key_packet->enable = false;
			BM2MC_PACKET_INIT( dst_key_packet, DestinationColorkeyEnable, false );
			dst_key_packet->enable = false;
			BM2MC_PACKET_INIT( filter_packet, FilterEnable, false );
			filter_packet->enable = true;

			BM2MC_PACKET_INIT( control_packet, FilterControl, false );
			control_packet->round = true;
			control_packet->adjust_color = false;
			control_packet->sub_alpha = false;
			control_packet->order = false;

			BM2MC_PACKET_INIT( matrix_packet, SourceColorMatrixEnable, false );
			matrix_packet->enable = false;
			BM2MC_PACKET_INIT( premult_packet, AlphaPremultiply, false );
			premult_packet->enable = false;
			BM2MC_PACKET_INIT( demult_packet, AlphaDemultiply, false );
			demult_packet->enable = false;
			BM2MC_PACKET_INIT( destpremult_packet, DestAlphaPremultiply, false );
			destpremult_packet->enable = false;

			BM2MC_PACKET_INIT( mirror_packet, Mirror, false );
			mirror_packet->src_hor = false;
			mirror_packet->src_ver = false;
			mirror_packet->dst_hor = false;
			mirror_packet->dst_ver = false;
			mirror_packet->out_hor = false;
			mirror_packet->out_ver = false;

			BM2MC_PACKET_INIT( fill_packet, FillBlit, true );
			fill_packet->header.sync = true;
			fill_packet->rect.x = 0;
			fill_packet->rect.y = 0;
			fill_packet->rect.width = 1;
			fill_packet->rect.height = 1;

			BM2MC_PACKET_INIT( restore_packet, RestoreState, false );

			hContext->eSyncState = BGRC_PACKET_P_SyncState_eQueuedInSw;
			hContext->last_size_out = BGRC_PACKET_P_SYNC_PACKET_SIZE;
			rc = BGRC_Packet_SubmitPackets( hGrc, hContext, BGRC_PACKET_P_SYNC_PACKET_SIZE );
			if (rc && rc != BGRC_PACKET_MSG_PACKETS_INCOMPLETE) return BERR_TRACE(rc);
		}
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
BM2MC_PACKET_PixelFormat BGRC_PACKET_P_ConvertPixelFormat( BPXL_Format format )
{
	switch( format )
	{
	case 0:                           return BM2MC_PACKET_PixelFormat_eUnknown;
	case BPXL_eR5_G6_B5:              return BM2MC_PACKET_PixelFormat_eR5_G6_B5;
	case BPXL_eB5_G6_R5:              return BM2MC_PACKET_PixelFormat_eB5_G6_R5;
	case BPXL_eA1_R5_G5_B5:           return BM2MC_PACKET_PixelFormat_eA1_R5_G5_B5;
	case BPXL_eX1_R5_G5_B5:           return BM2MC_PACKET_PixelFormat_eX1_R5_G5_B5;
	case BPXL_eA1_B5_G5_R5:           return BM2MC_PACKET_PixelFormat_eA1_B5_G5_R5;
	case BPXL_eX1_B5_G5_R5:           return BM2MC_PACKET_PixelFormat_eX1_B5_G5_R5;
	case BPXL_eR5_G5_B5_A1:           return BM2MC_PACKET_PixelFormat_eR5_G5_B5_A1;
	case BPXL_eR5_G5_B5_X1:           return BM2MC_PACKET_PixelFormat_eR5_G5_B5_X1;
	case BPXL_eB5_G5_R5_A1:           return BM2MC_PACKET_PixelFormat_eB5_G5_R5_A1;
	case BPXL_eB5_G5_R5_X1:           return BM2MC_PACKET_PixelFormat_eB5_G5_R5_X1;
	case BPXL_eA4_R4_G4_B4:           return BM2MC_PACKET_PixelFormat_eA4_R4_G4_B4;
	case BPXL_eX4_R4_G4_B4:           return BM2MC_PACKET_PixelFormat_eX4_R4_G4_B4;
	case BPXL_eA4_B4_G4_R4:           return BM2MC_PACKET_PixelFormat_eA4_B4_G4_R4;
	case BPXL_eX4_B4_G4_R4:           return BM2MC_PACKET_PixelFormat_eX4_B4_G4_R4;
	case BPXL_eR4_G4_B4_A4:           return BM2MC_PACKET_PixelFormat_eR4_G4_B4_A4;
	case BPXL_eR4_G4_B4_X4:           return BM2MC_PACKET_PixelFormat_eR4_G4_B4_X4;
	case BPXL_eB4_G4_R4_A4:           return BM2MC_PACKET_PixelFormat_eB4_G4_R4_A4;
	case BPXL_eB4_G4_R4_X4:           return BM2MC_PACKET_PixelFormat_eB4_G4_R4_X4;
	case BPXL_eA8_R8_G8_B8:           return BM2MC_PACKET_PixelFormat_eA8_R8_G8_B8;
	case BPXL_eX8_R8_G8_B8:           return BM2MC_PACKET_PixelFormat_eX8_R8_G8_B8;
	case BPXL_eA8_B8_G8_R8:           return BM2MC_PACKET_PixelFormat_eA8_B8_G8_R8;
	case BPXL_eX8_B8_G8_R8:           return BM2MC_PACKET_PixelFormat_eX8_B8_G8_R8;
	case BPXL_eR8_G8_B8_A8:           return BM2MC_PACKET_PixelFormat_eR8_G8_B8_A8;
	case BPXL_eR8_G8_B8_X8:           return BM2MC_PACKET_PixelFormat_eR8_G8_B8_X8;
	case BPXL_eB8_G8_R8_A8:           return BM2MC_PACKET_PixelFormat_eB8_G8_R8_A8;
	case BPXL_eB8_G8_R8_X8:           return BM2MC_PACKET_PixelFormat_eB8_G8_R8_X8;
	case BPXL_eA8:                    return BM2MC_PACKET_PixelFormat_eA8;
	case BPXL_eA4:                    return BM2MC_PACKET_PixelFormat_eA4;
	case BPXL_eA2:                    return BM2MC_PACKET_PixelFormat_eA2;
	case BPXL_eA1:                    return BM2MC_PACKET_PixelFormat_eA1;
	case BPXL_eW1:                    return BM2MC_PACKET_PixelFormat_eW1;
	case BPXL_eA8_P8:                 return BM2MC_PACKET_PixelFormat_eA8_P8;
	case BPXL_eP8:                    return BM2MC_PACKET_PixelFormat_eP8;
	case BPXL_eP4:                    return BM2MC_PACKET_PixelFormat_eP4;
	case BPXL_eP2:                    return BM2MC_PACKET_PixelFormat_eP2;
	case BPXL_eP1:                    return BM2MC_PACKET_PixelFormat_eP1;
	case BPXL_eY8_P8:                 return BM2MC_PACKET_PixelFormat_eY8_P8;
	case BPXL_eY8:                    return BM2MC_PACKET_PixelFormat_eY8;
	case BPXL_eA8_Y8:                 return BM2MC_PACKET_PixelFormat_eA8_Y8;
	case BPXL_eCb8_Cr8:               return BM2MC_PACKET_PixelFormat_eCb8_Cr8;
	case BPXL_eCr8_Cb8:               return BM2MC_PACKET_PixelFormat_eCr8_Cb8;
	case BPXL_eCb8:                   return BM2MC_PACKET_PixelFormat_eCb8;
	case BPXL_eCr8:                   return BM2MC_PACKET_PixelFormat_eCr8;
	case BPXL_eY08_Cb8_Y18_Cr8:       return BM2MC_PACKET_PixelFormat_eY08_Cb8_Y18_Cr8;
	case BPXL_eY08_Cr8_Y18_Cb8:       return BM2MC_PACKET_PixelFormat_eY08_Cr8_Y18_Cb8;
	case BPXL_eY18_Cb8_Y08_Cr8:       return BM2MC_PACKET_PixelFormat_eY18_Cb8_Y08_Cr8;
	case BPXL_eY18_Cr8_Y08_Cb8:       return BM2MC_PACKET_PixelFormat_eY18_Cr8_Y08_Cb8;
	case BPXL_eCb8_Y08_Cr8_Y18:       return BM2MC_PACKET_PixelFormat_eCb8_Y08_Cr8_Y18;
	case BPXL_eCb8_Y18_Cr8_Y08:       return BM2MC_PACKET_PixelFormat_eCb8_Y18_Cr8_Y08;
	case BPXL_eCr8_Y18_Cb8_Y08:       return BM2MC_PACKET_PixelFormat_eCr8_Y18_Cb8_Y08;
	case BPXL_eCr8_Y08_Cb8_Y18:       return BM2MC_PACKET_PixelFormat_eCr8_Y08_Cb8_Y18;
	case BPXL_eX2_Cr10_Y10_Cb10:      return BM2MC_PACKET_PixelFormat_eX2_Cr10_Y10_Cb10;
	case BPXL_eA8_Y8_Cb8_Cr8:         return BM2MC_PACKET_PixelFormat_eA8_Y8_Cb8_Cr8;
	case BPXL_eA8_Cr8_Cb8_Y8:         return BM2MC_PACKET_PixelFormat_eA8_Cr8_Cb8_Y8;
	case BPXL_eCr8_Cb8_Y8_A8:         return BM2MC_PACKET_PixelFormat_eCr8_Cb8_Y8_A8;
	case BPXL_eY8_Cb8_Cr8_A8:         return BM2MC_PACKET_PixelFormat_eY8_Cb8_Cr8_A8;
	case BPXL_eY010_Cb10_Y110_Cr10:   return BM2MC_PACKET_PixelFormat_eY010_Cb10_Y110_Cr10;
	case BPXL_eY010_Cr10_Y110_Cb10:   return BM2MC_PACKET_PixelFormat_eY010_Cr10_Y110_Cb10;
	case BPXL_eY110_Cb10_Y010_Cr10:   return BM2MC_PACKET_PixelFormat_eY110_Cb10_Y010_Cr10;
	case BPXL_eY110_Cr10_Y010_Cb10:   return BM2MC_PACKET_PixelFormat_eY110_Cr10_Y010_Cb10;
	case BPXL_eCb10_Y010_Cr10_Y110:   return BM2MC_PACKET_PixelFormat_eCb10_Y010_Cr10_Y110;
	case BPXL_eCb10_Y110_Cr10_Y010:   return BM2MC_PACKET_PixelFormat_eCb10_Y110_Cr10_Y010;
	case BPXL_eCr10_Y110_Cb10_Y010:   return BM2MC_PACKET_PixelFormat_eCr10_Y110_Cb10_Y010;
	case BPXL_eCr10_Y010_Cb10_Y110:   return BM2MC_PACKET_PixelFormat_eCr10_Y010_Cb10_Y110;
	case BPXL_eL15_L05_A6:            return BM2MC_PACKET_PixelFormat_eL15_L05_A6;
	default: return 0;
	}
}

/***************************************************************************/
void BGRC_PACKET_P_ConvertFilterCoeffs( BM2MC_PACKET_FilterCoeffs *coeffs, BGRC_FilterCoeffs filter, size_t src_size, size_t out_size )
{
	static BM2MC_PACKET_FilterCoeffs *pcoeffs = NULL;
	int iIndex;
	int iIndexMax;

	switch( filter )
	{
		case BGRC_FilterCoeffs_ePointSample:
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_PointSample;
			break;

		case BGRC_FilterCoeffs_eBilinear:
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_Bilinear;
			break;

		case BGRC_FilterCoeffs_eAnisotropic:
			iIndexMax = (sizeof s_BGRC_PACKET_P_DeviceFilter_Anisotropic / sizeof (BM2MC_PACKET_FilterCoeffs)) - 1;
			iIndex = out_size ? ((src_size << BGRC_PACKET_P_SCALER_STEP_FRAC_BITS) / out_size) >> BGRC_PACKET_P_SCALER_STEP_FRAC_BITS : 0;
			iIndex = (iIndex > iIndexMax) ? iIndexMax : iIndex;
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_Anisotropic[iIndex];
			break;

		case BGRC_FilterCoeffs_eSharp:
			iIndexMax = (sizeof s_BGRC_PACKET_P_DeviceFilter_Sharp / sizeof (BM2MC_PACKET_FilterCoeffs)) - 1;
			iIndex = src_size ? (out_size * 8) / src_size : 0;
			iIndex = (iIndex > iIndexMax) ? iIndexMax : iIndex;
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_Sharp[iIndex];
			break;

		case BGRC_FilterCoeffs_eSharper:
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_Sharper;
			break;

		case BGRC_FilterCoeffs_eBlurry:
			pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_Blurry;
			break;

		case BGRC_FilterCoeffs_eAntiFlutter:
			if( src_size == out_size )
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutter;
			else
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutterScale;
			break;

		case BGRC_FilterCoeffs_eAntiFlutterBlurry:
			if( src_size == out_size )
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutterBlurry;
			else
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutterScale;
			break;

		case BGRC_FilterCoeffs_eAntiFlutterSharp:
			if( src_size == out_size )
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutterSharp;
			else
				pcoeffs = &s_BGRC_PACKET_P_DeviceFilter_AntiFlutterScale;
			break;
	}

	if( coeffs && pcoeffs )
		BKNI_Memcpy( coeffs, pcoeffs, sizeof (BM2MC_PACKET_FilterCoeffs) );
}

/***************************************************************************/
#define BGRC_PACKET_P_MATRIX_FRAC_BITS          10

#if (BCHP_CHIP!=7038)
#define BGRC_PACKET_P_MATRIX_FRAC_ADD_BITS      4
#else
#define BGRC_PACKET_P_MATRIX_FRAC_ADD_BITS      0
#endif

#define BGRC_PACKET_P_GET_MATRIX_ENTRY( entry, left, right ) (int16_t) \
	((((((entry) * (((entry) < 0) ? -1 : 1)) << (left)) >> (right)) * (((entry) < 0) ? -1 : 1)) & BGRC_M2MC(SRC_CM_C00_C01_CM_C01_MASK))

/***************************************************************************/
void BGRC_PACKET_P_ConvertColorMatrix( BM2MC_PACKET_ColorMatrix *matrix_out, const int32_t *matrix_in, size_t shift )
{
	int ii;
	for( ii = 0; ii < 4; ++ii )
	{
		matrix_out->m[ii][0] = BGRC_PACKET_P_GET_MATRIX_ENTRY(matrix_in[ii*5+0], BGRC_PACKET_P_MATRIX_FRAC_BITS, shift);
		matrix_out->m[ii][1] = BGRC_PACKET_P_GET_MATRIX_ENTRY(matrix_in[ii*5+1], BGRC_PACKET_P_MATRIX_FRAC_BITS, shift);
		matrix_out->m[ii][2] = BGRC_PACKET_P_GET_MATRIX_ENTRY(matrix_in[ii*5+2], BGRC_PACKET_P_MATRIX_FRAC_BITS, shift);
		matrix_out->m[ii][3] = BGRC_PACKET_P_GET_MATRIX_ENTRY(matrix_in[ii*5+3], BGRC_PACKET_P_MATRIX_FRAC_BITS, shift);
		matrix_out->m[ii][4] = BGRC_PACKET_P_GET_MATRIX_ENTRY(matrix_in[ii*5+4], BGRC_PACKET_P_MATRIX_FRAC_ADD_BITS, shift);
	}
}
