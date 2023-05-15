/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_window_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1512 $
 * $brcm_Date: 10/15/12 2:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_window_priv.c $
 * 
 * Hydra_Software_Devel/PROD_HUMAX_URSR/3   5/29/15 5:47p scho
 * SW7425-4944: Video flicker while transition full scren to PiG window.
 * Complete transfer destination clipping to source from SW7346-1200
 * 
 * Hydra_Software_Devel/PROD_HUMAX_URSR/2   10/17/13 3:40p scho
 * SW7425-4944: optimize minsrc for deinterlacer from SW7425-5186
 * 
 * Hydra_Software_Devel/PROD_HUMAX_URSR/1   7/2/13 3:36p dongho
 * SW7425-4944:Patch for mosaic service issues in URSR
 * 
 * Hydra_Software_Devel/1567   6/13/13 1:47p pntruong
 * SW7425-4882: Corrected initialization of mosaic colorspace to sd.
 * 
 * Hydra_Software_Devel/1561   5/10/13 4:42p yuxiaz
 * SW7425-4785: More optimization for mosaic zorder sort.
 * 
 * Hydra_Software_Devel/1560   5/9/13 5:07p yuxiaz
 * SW7425-4785: Clear rect is not shown for all mosaic windows. Optimize
 * mosaic zorder code.
 * 
 * Hydra_Software_Devel/1551   4/2/13 3:00p pntruong
 * SW7425-4579: Keep track of mosaic on a per window context.
 * 
 * Hydra_Software_Devel/1512   10/15/12 2:55p pntruong
 * SW7231-1033: Correctly toggle dirty bit for clearrect/mosaic function.
 * Isolated the SW7425-2236 work-around for mosaic only.
 *
 * Hydra_Software_Devel/1511   10/5/12 4:45p yuxiaz
 * SW7425-3928: Added synclock status in window callback.
 *
 * Hydra_Software_Devel/1510   10/4/12 3:42p vanessah
 * SW7435-386: use the source aspect ratio for encode aspect ratio bypass
 * mode.
 *
 * Hydra_Software_Devel/1509   9/24/12 6:22p syang
 * SW7425-3977: add aspect-ratio handling for 3D video src and display
 *
 * Hydra_Software_Devel/1508   9/20/12 4:49p yuxiaz
 * SW7425-1835, SW7425-3928: RDC timestamps are always valid.
 *
 * Hydra_Software_Devel/1507   9/13/12 4:04p syang
 * SW7425-3896: pass ulOrigPTS for mfd/vfd gfx
 *
 * Hydra_Software_Devel/1506   9/12/12 2:30p syang
 * SW7425-2172: EnforceMinSizeLimit for SclCut
 *
 * Hydra_Software_Devel/1505   8/16/12 2:45p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/1504   8/16/12 2:03p tdo
 * SW7445-8: Fix assertion error due to new source
 *
 * Hydra_Software_Devel/1503   8/15/12 6:21p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/1502   8/10/12 3:26p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/1501   8/9/12 5:24p tdo
 * SW7420-2321 : Force to use SCL before CAP when bw ratio is equal to 1
 * in asymetric mode.
 *
 * Hydra_Software_Devel/1500   8/9/12 11:20a syang
 * SW7425-2172: add SetSurface support for MFD gfx feeding
 *
 * Hydra_Software_Devel/1499   8/8/12 4:23p vanessah
 * SW7425-3704: add gfx xcode channelchange flag
 *
 * Hydra_Software_Devel/1498   8/7/12 5:30p tdo
 * SW7420-2321: Add adjDstCut for mute path
 *
 * Hydra_Software_Devel/1497   7/31/12 4:56p yuxiaz
 * SW7425-1835: Use RDC slot timestamp for capture and playback timestamp
 * in multi-buffering algo.
 *
 * Hydra_Software_Devel/1496   7/27/12 3:46p vanessah
 * SW7425-3150: adding Bypass Video Processing support
 *
 * Hydra_Software_Devel/1495   7/17/12 9:30a vanessah
 * SW7425-3359: fix hddvi error
 *
 * Hydra_Software_Devel/1494   7/13/12 11:27a tdo
 * SW7563-9: Add support for VDC
 *
 * Hydra_Software_Devel/1493   7/12/12 11:33p vanessah
 * SW7358-354: min Src/Dsp Fmt support for 3D orientation
 *
 * Hydra_Software_Devel/1492   6/29/12 3:31p vanessah
 * SW7425-3359: FNRT flags support to ViCE MBox
 *
 * Hydra_Software_Devel/1491   6/14/12 3:44p syang
 * SW7425-2172: VFD/GFX with only cmp0 need 6 scratch regs for gfx;
 * prepare to add SetSurface support to MFD
 *
 * Hydra_Software_Devel/1490   6/12/12 6:50p darnstein
 * SW7425-3226: fix Coverity errors involving suspicious tests for NULL
 * pointer.
 *
 * Hydra_Software_Devel/1489   6/7/12 3:27p yuxiaz
 * SW7425-3026: Optimize 3D video support.
 *
 * Hydra_Software_Devel/1488   6/6/12 4:51p tdo
 * SW7552-279: video(color or brightness) is abnormal after change channel
 * to NTSC SD program when brightness/contrast/saturation/Hue is not
 * default value
 *
 * Hydra_Software_Devel/1487   6/6/12 11:39a vanessah
 * SW7425-2643:  complete the case for hd/sd simul
 *
 * Hydra_Software_Devel/1486   6/5/12 10:02a syang
 * SW7346-842: update scale factor rouding to honor user src clipping
 *
 * Hydra_Software_Devel/1485   6/5/12 8:56a vanessah
 * SW7425-2643: fix coverity error
 *
 * Hydra_Software_Devel/1484   6/4/12 10:22p vanessah
 * SW7425-2643:  reduce the iteration time
 *
 * Hydra_Software_Devel/1483   6/4/12 5:11p vanessah
 * SW7425-2643: pass bar type as invalid for invalid bar value
 *
 * Hydra_Software_Devel/1482   6/2/12 12:24p vanessah
 * SW7425-2643: fix coverity error
 *
 * Hydra_Software_Devel/1481   5/31/12 4:05p vanessah
 * SW7425-2643: adding bar data support for scaling etc
 *
 * Hydra_Software_Devel/1480   5/31/12 3:25p syang
 * SW7425-2093: alloc scratch reg for a playback's timeStamp if the vfd is
 * marked "false" for source creation during BVDC_Open
 *
 * Hydra_Software_Devel/1480   5/31/12 3:18p syang
 * SW7425-2093: alloc scratch reg for a playback's timeStamp if the vfd is
 * marked "false" for source creation during BVDC_Open
 *
 * Hydra_Software_Devel/1479   5/30/12 6:13p tdo
 * SW7425-1210: SCL: Veritical Block Averaging theshold should be base on
 * scaler's veritical tap.
 *
 * Hydra_Software_Devel/1478   5/30/12 4:54p syang
 * SW7425-2093: extra ApplyChange with 1st SetSurface no longer needed;
 * vfd/cap time stamp related scratch registers now share with vfd's
 * surface manager's; added vfd resource check; more clean up
 *
 * Hydra_Software_Devel/1477   5/25/12 11:27a vanessah
 * SW7435-199: using the display fmt aspect ratio when "full" window
 *
 * Hydra_Software_Devel/1476   5/25/12 11:04a yuxiaz
 * SW7408-332: Enable BVDC_P_SCL_V_STEP_SIZE_WORKAROUND on 7408.
 *
 * Hydra_Software_Devel/1475   5/23/12 6:48p syang
 * SW7425-2093: refactor vfd gfx feeding support for deep cleanup and to
 * use gfxsurface sub-module
 *
 * Hydra_Software_Devel/1473   5/21/12 2:52p yuxiaz
 * SW7425-3010: Added support of 3D korean format in BVN.
 *
 * Hydra_Software_Devel/1472   5/9/12 10:54a syang
 * SW7425-2093: refactor gfxfeeder code for deep cleanup and potential
 * error fix with GetSurface; use shared gfxsurface to manage gfx surface
 *
 * Hydra_Software_Devel/1471   5/7/12 9:48p vanessah
 * SW7418-96: fix the testfeature propogation error
 *
 * Hydra_Software_Devel/1470   5/7/12 6:24p vanessah
 * SW7418-96: fix the testfeature propogation error
 *
 * Hydra_Software_Devel/1469   5/3/12 9:24p vanessah
 * SW7425-2873: transcode path fmt stress test
 *
 * Hydra_Software_Devel/1468   5/1/12 3:05p vanessah
 * SW7425-2643: add bar data support
 *
 * Hydra_Software_Devel/1467   4/27/12 4:03p tdo
 * SW7584-21: Add PI/vdc support for 7584
 *
 * Hydra_Software_Devel/1466   4/11/12 10:58a pntruong
 * SW7420-2286: Default enabled pq enhancement.
 *
 * Hydra_Software_Devel/1465   4/9/12 1:48p yuxiaz
 * SW7231-753: Video freeze on composite when doing 1080p60 HD and 576i
 * SD.
 *
 * Hydra_Software_Devel/1464   4/5/12 11:07a vanessah
 * SW7552-221: no subrul win id setting for invalid window or inactive
 * reader
 *
 * Hydra_Software_Devel/1463   4/4/12 7:47p vanessah
 * SW7552-221: roll back the change due to potential bug
 *
 * Hydra_Software_Devel/1462   4/4/12 3:00p vanessah
 * SW7552-221: no subrul win id setting for invalid window
 *
 * Hydra_Software_Devel/1461   3/31/12 10:01p tdo
 * SW7346-760: Fix segfault with non pep windows
 *
 * Hydra_Software_Devel/1460   3/30/12 5:08p tdo
 * SW7346-760: Kernel oops during BVDC_P_Pep_BuildRul_isr.  Avoid read
 * HIST stats during vnet reconfig or comp reset
 *
 * Hydra_Software_Devel/1459   3/30/12 3:35p pntruong
 * SW7420-2286: Added tri-state option for pq policies for mad.
 *
 * Hydra_Software_Devel/1458   3/26/12 2:55p vanessah
 * SW7425-2608: removal the non-cmp0 restriction on force capture decision
 *
 * Hydra_Software_Devel/1457   3/26/12 1:54p yuxiaz
 * SW7405-5571: Enable bInvalid bit if vnetmode is not set
 *
 * Hydra_Software_Devel/1456   3/26/12 10:57a yuxiaz
 * SW7425-2753: Fixed BVN errors for 3d conversion.
 *
 * Hydra_Software_Devel/1455   3/23/12 9:44a pntruong
 * SW7231-736: Handle mute by removing reader portion from vnet.
 *
 * Hydra_Software_Devel/1454   3/5/12 4:08p yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/1453   3/1/12 7:54p tdo
 * SW7429-71: Init color space for mosaic
 *
 * Hydra_Software_Devel/1452   2/27/12 3:53p pntruong
 * SW7358-180, SW7125-1254: Fixed fullsize display format allocation.
 *
 * Hydra_Software_Devel/1451   2/27/12 10:46a pntruong
 * SW7358-180: Fixed initialization of minfmt.
 *
 * Hydra_Software_Devel/1450   2/24/12 3:31p vanessah
 * SW7425-2327: add Bar data left and top pixel shift
 *
 * Hydra_Software_Devel/1449   2/22/12 7:10p albertl
 * SW7420-339: Temporarily disabled const color due to stress test issue.
 *
 * Hydra_Software_Devel/1448   2/22/12 2:38p albertl
 * SW7420-339: Fixed build errors for platforms with 1 video widnow.
 *
 * Hydra_Software_Devel/1447   2/21/12 3:54p albertl
 * SW7420-339: Re-enabled constant color for supported chips.
 *
 * Hydra_Software_Devel/1446   2/17/12 2:13p yuxiaz
 * SW7231-635: Use BVDC_P_SCB_BURST_SIZE.
 *
 * Hydra_Software_Devel/1445   2/17/12 11:19a vanessah
 * SW7425-2327: add Bar data handling to VDC
 *
 * Hydra_Software_Devel/1444   2/15/12 3:47p pntruong
 * SW7231-587: Correctly propagate the original pts value to get buffer.
 *
 * Hydra_Software_Devel/1443   2/15/12 10:00a vanessah
 * SW7425-2020: fix the compile error
 *
 * Hydra_Software_Devel/1442   2/14/12 1:37p vanessah
 * SW7425-2020: add meta data delay when deinterlacer on/off
 *
 * Hydra_Software_Devel/1441   2/13/12 11:20a yuxiaz
 * SW7231-632: DTG Test Video window position Failure
 *
 * Hydra_Software_Devel/1440   2/9/12 5:15p vanessah
 * SW7425-1705: add the workaround to 7358/7552
 *
 * Hydra_Software_Devel/1439   2/8/12 10:05a pntruong
 * SW7360-3: Initial support.
 *
 * Hydra_Software_Devel/1438   2/7/12 4:49p tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/1437   2/7/12 12:43p tdo
 * SW7420-2248: VDC GFX error when running Brutus in kernel mode
 *
 * Hydra_Software_Devel/1436   2/6/12 9:36p vanessah
 * SW7425-2296: use scaler to do Orientation conversion
 *
 * Hydra_Software_Devel/1435   2/6/12 5:19p pntruong
 * SW7425-2236: Used colorkey to mask out mosaic old buffers.  Make sure
 * capture rectangles and clear rectangle are synced.
 *
 * Hydra_Software_Devel/1434   2/1/12 2:48p tdo
 * SW7231-479: Fix tearing problem with psf
 *
 * Hydra_Software_Devel/1433   1/31/12 5:47p vanessah
 * SW7425-2080: enable GFD visibility
 *
 * Hydra_Software_Devel/1432   1/30/12 4:06p vanessah
 * SW7425-2239: add  bChannelChange in VCE Mbox
 *
 * Hydra_Software_Devel/1431   1/27/12 11:19p vanessah
 * SW7358-219:  vnet configration backward compatible
 *
 * Hydra_Software_Devel/1430   1/24/12 4:34p vanessah
 * SW7358-219: adjust the MCVP delay
 *
 * Hydra_Software_Devel/1429   1/19/12 12:58p yuxiaz
 * SW7358-219: Use stVnetMode to calculate MAD delay.
 *
 * Hydra_Software_Devel/1428   1/16/12 11:13a vanessah
 * SW7425-2084: add bLast flage to VCD Mbox
 *
 * Hydra_Software_Devel/1427   1/13/12 3:33p vanessah
 * SW7425-2095: fix the failure for mosaic mode
 *
 * Hydra_Software_Devel/1426   1/12/12 5:43p vanessah
 * SW7425-2162: SW7425-2095: change 7425 MADR QM buffer allocation/settop-
 * box crashed due to non-invalid mute color buffer
 *
 * Hydra_Software_Devel/1425   1/11/12 6:59p vanessah
 * SW7358-180: fix mindspfmt typo
 *
 * Hydra_Software_Devel/1424   12/27/11 11:09p vanessah
 * SW7425-1992: fix close/open window buffer allocation assert problem.
 *
 * Hydra_Software_Devel/1423   12/22/11 10:02a vanessah
 * SW7425-2020: roll back the mad delay changes first for release
 *
 * Hydra_Software_Devel/1422   12/20/11 11:38p vanessah
 * SW7425-1992: dynamic allocate mcvp buffer according to its mode
 *
 * Hydra_Software_Devel/1421   12/19/11 4:34p vanessah
 * SW7425-2020: Meta data include deinterlacer delay
 *
 * Hydra_Software_Devel/1420   12/16/11 4:53p vanessah
 * SW7425-1992: fix mcvp segfault
 *
 * Hydra_Software_Devel/1419   12/16/11 10:28a hongtaoz
 * SW7425-1169: count and resume the dropped non-ignore pics during BVN
 * reconfig to keep the NRT transcode AV sync;
 *
 * Hydra_Software_Devel/1418   12/13/11 4:58p yuxiaz
 * SW7425-1875: Added support for source buffer selection in 3d mode.
 *
 * Hydra_Software_Devel/1417   12/9/11 4:28p pntruong
 * SW7231-122: Added support for DUAL HD Display capabilities.  This mode
 * likely require special customized rts.
 *
 * Hydra_Software_Devel/1416   12/1/11 4:34p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 *
 * Hydra_Software_Devel/1415   11/30/11 5:37p yuxiaz
 * SW7425-1182: Set default 8bit 422 format based on LE or BE.
 *
 * Hydra_Software_Devel/1414   11/29/11 11:18a vanessah
 * SW7425-1783: add user data support in VDC PI
 *
 * Hydra_Software_Devel/1413   11/28/11 2:27p pntruong
 * SW7425-973: Corrected for the output lines per picture.
 *
 * Hydra_Software_Devel/1412   11/23/11 10:57p vanessah
 * SW7358-180: fix bDeinterlaceAllocateFull buffer requirement
 *
 * Hydra_Software_Devel/1411   11/23/11 4:24p tdo
 * SW7435-9: Add support for CMP4-5, GFD4-5, MFD3, VFD5
 *
 * Hydra_Software_Devel/1410   11/23/11 11:31a tdo
 * SW7435-9: add support for 7435A0 in VDC
 *
 * Hydra_Software_Devel/1409   11/18/11 11:39a vanessah
 * SW7425-1743: apply Mosaic mode x_offset  even number restriction to all
 * platforms
 *
 * Hydra_Software_Devel/1408   11/17/11 2:32p vanessah
 * SW7425-1705: add Mosaic mode workaround for 7425/7344/7346
 *
 * Hydra_Software_Devel/1407   11/16/11 11:39a vanessah
 * SW7358-180: fix the MCVP buffer cfg.
 *
 * Hydra_Software_Devel/1406   11/15/11 12:59p vanessah
 * SW7358-180: minSrcFmt minDspFmt  size and interlace alignment
 *
 * Hydra_Software_Devel/1405   11/12/11 11:45a hongtaoz
 * SW7425-1115: MAD rect size is independent of minimum display format
 * since SCL is always after MAD;
 *
 * Hydra_Software_Devel/1404   11/11/11 5:08p vanessah
 * SW7425-1115: disable Mcvp for mosaic mode
 *
 * Hydra_Software_Devel/1403   11/10/11 9:20p vanessah
 * SW7425-1115: add MCVP/MADR seamless transaction
 *
 * Hydra_Software_Devel/1402   10/27/11 4:56p yuxiaz
 * SW7405-5520: SD display will show wrong full screen flash when toggling
 * size and visibility with simultaneous HD display.
 *
 * Hydra_Software_Devel/1401   10/27/11 4:28p syang
 * SW7425-1429: move per vsync call back from window to display
 *
 * Hydra_Software_Devel/1400   10/24/11 4:27p syang
 * SW7425-1429: check in after debug
 *
 * Hydra_Software_Devel/1399   10/21/11 11:16a syang
 * SW7425-1429: added GetBuff_isr and ReturnBuff_isr and per vsync window
 * call back
 *
 * Hydra_Software_Devel/1398   10/19/11 1:48p vanessah
 * SW7425-1488: MADR RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/1397   10/14/11 11:25p vanessah
 * SW7425-1027: add gcd calculation
 *
 * Hydra_Software_Devel/1396   10/6/11 4:12p pntruong
 * SW7429-16: Initial support.
 *
 * Hydra_Software_Devel/1395   9/27/11 5:21p yuxiaz
 * SW7425-967: Bring up MRE on 7425 B0.
 *
 * Hydra_Software_Devel/1394   9/26/11 6:12p pntruong
 * SW7425-969: Allowed pip's path resource scl/vfd/cap to be on 2nd
 * transcode path.
 *
 * Hydra_Software_Devel/1393   9/23/11 5:21p pntruong
 * SW7231-383: Correct programming of game mode and delay callback
 * reporting for madr/mcvp.
 *
 * Hydra_Software_Devel/1392   9/22/11 3:33p pntruong
 * SW3548-3090: Pruned ununsed code.
 *
 * Hydra_Software_Devel/1391   9/14/11 3:51p yuxiaz
 * SW7425-1276: Fixed switch between 3DLR and 3DOU.
 *
 * Hydra_Software_Devel/1390   9/8/11 10:21p tdo
 * SW7425-922: VDC bringup for B0
 *
 * Hydra_Software_Devel/1389   9/7/11 12:53p tdo
 * SW7425-1210: Undo previous check in, not working with old SCL yet.
 *
 * Hydra_Software_Devel/1387   9/1/11 3:53p pntruong
 * SW7425-973: Make sure all dirty bits are handled and cleared.
 *
 * Hydra_Software_Devel/1386   8/31/11 5:39p pntruong
 * SW7425-973:  Implement bandwidth restriction for madr.
 *
 * Hydra_Software_Devel/1385   8/31/11 12:11p syang
 * SW7425-1170: pxlAspRatio passed to ViCE2 is corrected to format x<<16 |
 * y
 *
 * Hydra_Software_Devel/1383   8/31/11 10:48a hongtaoz
 * SW7425-483: avoid repeated window vsync callback for NRT mode;
 *
 * Hydra_Software_Devel/1382   8/30/11 11:58p hongtaoz
 * SW7425-483: return zero vsync delay if the window display is in NRT
 * mode transcode since VDC doesn't provide non-ignored zero fill
 * pictures to NRT mode transcoder;
 *
 * Hydra_Software_Devel/1381   8/30/11 8:52p yuxiaz
 * SW7425-1132, SW7420-2017: Addded  7405 and 7231 to list  of chips needs
 * SCL vertical step size workaround.
 *
 * Hydra_Software_Devel/1380   8/27/11 8:35p hongtaoz
 * SW7425-1132, SW7420-2017: honor window source clip setting if possible
 * for muted source; limit the SCL vertical step size workaround to
 * specific legacy chips with the hw bug since it has quality side
 * effect;
 *
 * Hydra_Software_Devel/1379   8/26/11 6:08p syang
 * SW7425-1170: put STG related util func under macro's control
 *
 * Hydra_Software_Devel/1378   8/26/11 5:39p syang
 * SW7425-1170: merge from branch SW7572-1170
 *
 * Hydra_Software_Devel/SW7425-1170/3   8/26/11 4:22p syang
 * SW7425-1170: refactor pixel aspect ratio related code
 *
 * Hydra_Software_Devel/SW7425-1170/2   8/24/11 6:56p vanessah
 * SW7425-1170: ASP code clean
 *
 * Hydra_Software_Devel/SW7425-1170/1   8/23/11 6:17p vanessah
 * SW7425-1170: Aspect ratio cleanup
 *
 * Hydra_Software_Devel/1377   8/25/11 11:03a pntruong
 * SW7425-1190, SW7425-630: Improved documentation and renamed parameter
 * to better fit functionality.
 *
 * Hydra_Software_Devel/1376   8/22/11 1:39p pntruong
 * SW7425-1175: Optimized deringing/dejagging defaults.
 *
 * Hydra_Software_Devel/1375   8/15/11 4:16p tdo
 * SW7420-2017: Video displays static image. Audio decodes correctly.  Add
 * SW work-around to VERT Step calculation.
 *
 * Hydra_Software_Devel/1374   8/3/11 2:53p tdo
 * SW7425-1050: BVDC_P_MAD_OUTPUT_RATE macro generating floating point
 * code.
 *
 * Hydra_Software_Devel/1373   8/2/11 7:45p vanessah
 * SW7425-465: optimization of delay counter
 *
 * Hydra_Software_Devel/1372   7/29/11 11:40a vanessah
 * SW7425-465: add MAD game mode delay concern into fmt delay vsync
 *
 * Hydra_Software_Devel/1371   7/28/11 5:23p vanessah
 * SW7425-465: add MADR delay to CMP fmt delay
 *
 * Hydra_Software_Devel/1370   7/22/11 11:13p pntruong
 * SW7425-983: Corrected deinterlacer's hscaler threshold.
 *
 * Hydra_Software_Devel/1369   7/22/11 10:59a yuxiaz
 * SW7420-1959: Back out support of non-full content mode for PIP, use
 * mosaic APIs instead.
 *
 * Hydra_Software_Devel/1368   7/18/11 5:31p pntruong
 * SW7231-284, SW7346-295, SW7405-5339: Refactored master framerate
 * tracking, so that it can also handle validation.
 *
 * Hydra_Software_Devel/1367   7/18/11 11:22a vanessah
 * SW7425-835: SW7425-923: fix ViCE2 channel id bug + B0 STG
 *
 * Hydra_Software_Devel/1366   7/11/11 2:17p tdo
 * SW7420-1971: Video Pause seen when VEC alignment is going on.  Add flag
 * to keep BVN connected while doing alignment.
 *
 * Hydra_Software_Devel/1365   6/30/11 6:51p yuxiaz
 * SW7420-1959: Added support of non-full content mode for PIP.
 *
 * Hydra_Software_Devel/1364   6/29/11 11:35a vanessah
 * SW7425-784: BVN error for 480p MAD mode.
 *
 * Hydra_Software_Devel/1363   6/28/11 4:59p vanessah
 * SW7425-686: change Srcpolarity when XVD and BVN refreshrate mismatch
 *
 * Hydra_Software_Devel/1362   6/28/11 9:41a vanessah
 * SW7425-686: Get rid of black frame
 *
 * Hydra_Software_Devel/1361   6/27/11 7:58p pntruong
 * SW7425-772: Fine tuned deinterlacer policy.
 *
 * Hydra_Software_Devel/1360   6/24/11 8:23p vanessah
 * SW7425-772: turn off MADR on some exception cases
 *
 * Hydra_Software_Devel/1359   6/24/11 4:31p vanessah
 * SW7425-761: decide Scaler position due to fmt instead of compositor id
 *
 * Hydra_Software_Devel/1358   6/24/11 3:54p albertl
 * SW7420-1966: Cleaned up and reorganized colorspace enums and indicies
 * in display, vdec, and cmp.
 *
 * Hydra_Software_Devel/1356   6/23/11 1:48p vanessah
 * SW7425-762: fix cmp surface configuration error
 *
 * Hydra_Software_Devel/1355   6/23/11 12:46p vanessah
 * SW7425-761: fix math calculation error in scaling
 *
 * Hydra_Software_Devel/1354   6/22/11 3:07p yuxiaz
 * SW7425-592: Added API to get and set display sample aspect ratio
 * settings.
 *
 * Hydra_Software_Devel/1353   6/21/11 11:02p pntruong
 * SW7425-337: Temporary restricted 2nd deinterlacer (madr) for encoder
 * path only to match 7425 usages.
 *
 * Hydra_Software_Devel/1352   6/21/11 3:15p yuxiaz
 * SW7552-47: Added software workaround for MADR alignment in
 * BVDC_P_Window_TryXferHrzSclToHscl_isr.
 *
 * Hydra_Software_Devel/1351   6/20/11 5:50p tdo
 * SW7425-104: Rename the old repeat field flag to match avc structure and
 * use the new added bRepeatField flag for ANR.
 *
 * Hydra_Software_Devel/1350   6/16/11 6:19p vanessah
 * SW7425-686: fix size mismatch and STG init
 *
 * Hydra_Software_Devel/1349   6/13/11 10:56a vanessah
 * SW7425-704: fix compile error for 7405
 *
 * Hydra_Software_Devel/1348   6/10/11 5:49p hongtaoz
 * SW7425-704: add NRT mode support to VDC; moved the STG meta data
 * passing from window writer isr to reader isr; added bStallStc flag
 * support;
 *
 * Hydra_Software_Devel/1347   6/8/11 9:11a vanessah
 * SW7425-465: fix compatibility for 7405/7038
 *
 * Hydra_Software_Devel/1346   6/7/11 6:11p vanessah
 * SW7425-465: fix fmt switch error for 7420
 *
 * Hydra_Software_Devel/1345   6/2/11 11:28a vanessah
 * SW7425-630: Further fix API and setDisplaySize CurInfo NewInfo
 * confusion.
 *
 * Hydra_Software_Devel/1344   6/2/11 9:00a vanessah
 * SW7425-630: add boundary check for 3D right window
 *
 * Hydra_Software_Devel/1343   6/2/11 8:41a vanessah
 * SW7425-630: fix delayed apply change for GFX
 *
 * Hydra_Software_Devel/1342   5/27/11 6:37p vanessah
 * SW7425-465: fix video transcoding resolution switch transition artifact
 *
 * Hydra_Software_Devel/1341   5/27/11 3:37p yuxiaz
 * SW7425-415: Clean up buffer allocation and orientation settings for 3d
 * video support.
 *
 * Hydra_Software_Devel/1340   5/27/11 10:57a pntruong
 * SW7425-658: Allowed scale factor rounding to co-exist with non-linear
 * scaling.
 *
 * Hydra_Software_Devel/1339   5/27/11 9:45a syang
 * SW7572-193:  use accurate bDeinterlace info to calculate ulCapFdrRate
 *
 * Hydra_Software_Devel/1338   5/24/11 5:53p hongtaoz
 * SW7425-348: use BDBG_FILE_MODULE to demux the debug messages;
 *
 * Hydra_Software_Devel/1337   5/18/11 9:43a vanessah
 * SW7425-404: fix compile error for 7405
 *
 * Hydra_Software_Devel/1336   5/17/11 5:14p vanessah
 * SW7425-404: further code clean-up on GFX Display format update
 *
 * Hydra_Software_Devel/1335   5/17/11 4:12p vanessah
 * SW7425-404: update GFD display orientation correctly.
 *
 * Hydra_Software_Devel/1334   5/17/11 2:08p pntruong
 * SW3548-2828: Use the correct window id enumeration.
 *
 * Hydra_Software_Devel/1333   5/16/11 6:34p vanessah
 * SW7425-404: update horizontal and verital scale flag
 *
 * Hydra_Software_Devel/1332   5/13/11 11:23a yuxiaz
 * SW7425-267: Fixed the problem MADR hsize is not multiple of 4.
 *
 * Hydra_Software_Devel/1331   5/12/11 4:25p yuxiaz
 * SW7231-74: Added software workaround for MADR alignment hung.
 *
 * Hydra_Software_Devel/1330   5/11/11 3:14p vanessah
 * SW7425-404: set GFD orientation  to default fmt orientation if no
 * overrideOrientation provided.
 *
 * Hydra_Software_Devel/1329   5/5/11 6:24p pntruong
 * SW7425-465: Video transcoder dynamic resolution change has black frames
 * artifact.  Allow specifying minimum buffer to not trigger re-
 * allocation.
 *
 * Hydra_Software_Devel/1328   5/3/11 3:03p tdo
 * SW7425-486, SW7422-142: Checking against new fmt vs. current
 *
 * Hydra_Software_Devel/1327   5/3/11 2:58p tdo
 * SW7425-486, SW7422-142: Add window size validation check for 3D half
 * res cases
 *
 * Hydra_Software_Devel/1326   4/27/11 12:08p yuxiaz
 * SW7425-348, SW7231-105: Fixed comment.
 *
 * Hydra_Software_Devel/1325   4/26/11 3:19p yuxiaz
 * SW7425-348, SW7231-105: Increase fraction bits to 8 for MADR alignment
 * workaround.
 *
 * Hydra_Software_Devel/1324   4/25/11 3:50p yuxiaz
 * SW7425-348: Added software workaround for MADR alignment.
 *
 * Hydra_Software_Devel/1323   4/21/11 6:46p yuxiaz
 * SW7425-411: Temporary fix for the MVC playback segfault. Need to clean
 * up later.
 *
 * Hydra_Software_Devel/1322   4/12/11 11:18a pntruong
 * SW7425-236: Removed build warnings.
 *
 * Hydra_Software_Devel/1321   4/8/11 1:56p yuxiaz
 * SW7425-236: Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/1320   4/5/11 5:35p albertl
 * SW7405-5138: Fixed potential divide by error when src ratios are wildly
 * invalid.
 *
 * Hydra_Software_Devel/1319   3/31/11 3:31p pntruong
 * SW7425-267: MADR input needs to be multiple of 4.
 *
 * Hydra_Software_Devel/1318   3/29/11 1:42p yuxiaz
 * SW7422-143, SW7425-236: Added new source dirty bit for orientation
 * change. Fixed flashing when source orientation changes.
 *
 * Hydra_Software_Devel/1317   3/15/11 1:32p yuxiaz
 * SW7420-1186: Fixed field inversion on SD in vdc_test.
 *
 * Hydra_Software_Devel/1316   3/3/11 2:11p pntruong
 * SW7400-3008: Need to release unused/acquired resources that does not go
 * thru vnet shutdown path.
 *
 * Hydra_Software_Devel/1315   2/24/11 4:33p albertl
 * SW7405-4434: Fixed ApplyChanges taking more than one vsync on sync
 * locked windows.
 *
 * Hydra_Software_Devel/1314   2/16/11 5:15p tdo
 * SW7346-85, SW7422-51 : Software TN2TH Setting
 *
 * Hydra_Software_Devel/1313   2/9/11 5:04p tdo
 * SW7340-84: MAD in PIG mode has incorrect CAP/SCL orientation.
 *
 * Hydra_Software_Devel/1312   2/8/11 5:25p vanessah
 * SW7425-46: STG-Mbox programming
 *
 * Hydra_Software_Devel/1311   2/7/11 5:48p vanessah
 * SW7425-46: add ViCE2 meta-data Mbox register
 *
 * Hydra_Software_Devel/1310   2/7/11 5:25p vanessah
 * SW7425-46: add MFD mute to the skip picture to start quality comparison
 *
 * Hydra_Software_Devel/1309   1/27/11 4:01p albertl
 * SW7125-762: Removed extra conditions for resetting last capture
 * polarity when handling cadence.
 *
 * Hydra_Software_Devel/1308   1/20/11 5:03p tdo
 * SW7422-42:  Update compositor 3D programming
 *
 * Hydra_Software_Devel/1307   1/18/11 4:53p vanessah
 * SW7422-43:  3D graphics support
 *
 * Hydra_Software_Devel/1306   1/13/11 3:14p yuxiaz
 * SW7422-53: Added mosaic support to PIP.
 *
 * Hydra_Software_Devel/1305   1/12/11 4:36p yuxiaz
 * SW7335-1133: Added more debug messages for VDC heap debugging.
 *
 * Hydra_Software_Devel/1304   1/7/11 4:47p yuxiaz
 * SW7422-174: Added DCX support to MCVP and MADR.
 *
 * Hydra_Software_Devel/1303   1/6/11 9:48a pntruong
 * SW7422-174: Initialized madr with compression enabled to match up with
 * default rts for reference usage.
 *
 * Hydra_Software_Devel/1302   1/4/11 4:09p yuxiaz
 * SW7422-174: Refactor DCX macros.
 *
 * Hydra_Software_Devel/1301   1/4/11 2:29p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case.  Fix
 * segfault.
 *
 * Hydra_Software_Devel/1300   12/29/10 3:47p pntruong
 * SW7422-174: Initial support for siob in madr.
 *
 * Hydra_Software_Devel/1299   12/28/10 4:52p pntruong
 * SW7231-2: Corrected the resource allocation for 2nd display pip.
 *
 * Hydra_Software_Devel/1299   12/28/10 4:49p pntruong
 * SW7231-2: Corrected the resource allocation for 2nd display pip.
 *
 * Hydra_Software_Devel/1298   12/28/10 3:05p vanessah
 * SW7422-52:
 * Two deinterlacers support.
 *
 * Hydra_Software_Devel/1297   12/23/10 5:13p tdo
 * SW7422-148: Refactor MCVP to address chips w/wo MCTF
 *
 * Hydra_Software_Devel/1296   12/21/10 11:05a tdo
 * SW7422-28: Adjust to 3D format size change
 *
 * Hydra_Software_Devel/1295   12/20/10 5:47p yuxiaz
 * SW7422-34: Added BVDC_Source_SetOrientation and
 * BVDC_Source_GetOrientation.
 *
 * Hydra_Software_Devel/1294   12/16/10 11:39p pntruong
 * SW3548-2987: Added 1080p@30hz 3DOU_AS hdmi input support.
 *
 * Hydra_Software_Devel/1293   12/13/10 2:26p yuxiaz
 * SW7422-141: Rework ANR buffer allocation for 1080p 3D input case.
 *
 * Hydra_Software_Devel/1292   12/10/10 4:45p yuxiaz
 * SW7422-39: Always use dual pointer mode for 3D in CAP and VFD.
 *
 * Hydra_Software_Devel/1291   12/9/10 2:38p yuxiaz
 * SW7422-39: Set bReDetVnet if user changes display orientation
 *
 * Hydra_Software_Devel/1290   12/8/10 5:44p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/1289   12/8/10 5:09p yuxiaz
 * SW7422-36: Double the buffer size for MAD and ANR for 3D display on
 * 7422.
 *
 * Hydra_Software_Devel/1288   12/6/10 5:42p yuxiaz
 * SW7422-36: Need to double the buffer size for MAD and ANR for 3D on
 * 7422.
 *
 * Hydra_Software_Devel/1287   12/1/10 6:10p tdo
 * SW7422-44: Handle 3D changes for display
 *
 * Hydra_Software_Devel/1286   11/29/10 10:58p hongtaoz
 * SW7425-29: enforce digital format size for digital master display;
 *
 * Hydra_Software_Devel/1285   11/29/10 10:10p hongtaoz
 * SW7425-29: fixed a typo;
 *
 * Hydra_Software_Devel/1284   11/29/10 9:54p hongtaoz
 * SW7425-29: enforce full-screen digital master display window size to be
 * digital size;
 *
 * Hydra_Software_Devel/1283   11/24/10 1:25p tdo
 * SW7422-52: Bring-up MAD-R
 *
 * Hydra_Software_Devel/1282   11/23/10 2:20p yuxiaz
 * SW7422-35: Added 3D support in HD_DVI.
 *
 * Hydra_Software_Devel/1281   11/19/10 3:06p tdo
 * SW7422-13: Revert back the resource feature table change for 7400.
 *
 * Hydra_Software_Devel/1280   11/16/10 6:50p tdo
 * SW7422-92: Need display 3D orientation override function for VDC
 *
 * Hydra_Software_Devel/1279   11/15/10 6:01p pntruong
 * SW7231-2: Initial support to make 7231 compile.
 *
 * Hydra_Software_Devel/1278   11/12/10 3:56p pntruong
 * SW7425-31: Takes bvn yuv into account for hddvi input.  Fixed bad debug
 * register read on non-mfd source.  Updated scratch registers
 * availabliity.
 *
 * Hydra_Software_Devel/1277   11/11/10 7:33p albertl
 * SW7125-364: Fixed BVDC_P_CbIsDirty and added assert to check bitfields
 * in dirty bits fit within union integer representation.  Fixed naming
 * of dirty bits.
 *
 * Hydra_Software_Devel/1276   11/11/10 7:13p hongtaoz
 * SW7425-51: reduced combo trigger use in VDC to 1 per MFD source;
 *
 * Hydra_Software_Devel/1275   11/11/10 11:19a tdo
 * SW7422-13: window acquires wrong MCVP id due to the cap in feature
 * table doesn't match with resource table
 *
 * Hydra_Software_Devel/1274   11/10/10 4:37p albertl
 * SW7550-610: Extended BVDC_Window_SetCoefficientIndex to work for
 * graphics windows.
 *
 * Hydra_Software_Devel/1273   11/8/10 4:59p yuxiaz
 * SW7552-7: Add initial support for 7552.
 *
 * Hydra_Software_Devel/1272   11/3/10 10:25p tdo
 * SW7420-1224: Add trick mode setting to MCDI
 *
 * Hydra_Software_Devel/1271   10/29/10 3:02p tdo
 * SW7422-40: Update GetBuffer() capability in VDC with 3D case
 *
 * Hydra_Software_Devel/1270   10/29/10 10:59a yuxiaz
 * SW7422-31, SW7422-32, SW7422-39: More for 3D Video support.
 *
 * Hydra_Software_Devel/1269   10/28/10 6:15p tdo
 * SW7422-42:  Update compositor 3D programming.
 *
 * Hydra_Software_Devel/1268   10/22/10 7:02p vanessah
 * SW7425-46:  ViCE2 meta data mailbox register
 *
 * Hydra_Software_Devel/1267   10/21/10 2:04p pntruong
 * SW7408-117: Added support for 7408B0 CMP changes.
 *
 * Hydra_Software_Devel/1266   10/18/10 11:09p tdo
 * SW7422-52: Fix segfault when toggling anr when there is no mad for 7420
 *
 * Hydra_Software_Devel/1265   10/15/10 3:47p hongtaoz
 * SW7425-13: added shared SCL3 support between c1v1 and c2v0 windows for
 * 7420/7422/7425;
 *
 * Hydra_Software_Devel/1264   10/15/10 12:42p tdo
 * SW7420-1157, SW7425-33 : Take DCX workaround into account for PIP mem
 * allocation. Fix resource sharing table for 7420/7422/7425.
 *
 * Hydra_Software_Devel/1263   10/11/10 1:02p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/1262   10/7/10 2:53p syang
 * SW7405-4239, SW7400-2813: ccase didn't merge, manually merge in version
 * 1260
 *
 * Hydra_Software_Devel/1261   10/7/10 12:38p syang
 * SW7405-4239, SW7400-2813: besides uint64_t, added back uint32_t
 * calculation for aspect ratio correction with a compile option -D
 * BVDC_UINT32_ONLY=1
 *
 * Hydra_Software_Devel/1260   10/7/10 11:46a yuxiaz
 * SW7422-38: Handle VDC changes to memory allocation for 3D.
 *
 * Hydra_Software_Devel/1259   10/4/10 5:25p hongtaoz
 * SW7425-33: added the 4th compositor support;
 *
 * Hydra_Software_Devel/1258   9/29/10 5:23p yuxiaz
 * SW7422-39: Added 3D video support in CAP and VFD.
 *
 * Hydra_Software_Devel/1257   9/28/10 11:35p tdo
 * SW7425-33: Remove compiling warning
 *
 * Hydra_Software_Devel/1256   9/28/10 10:56p tdo
 * SW7425-33: VDC: Add the 4th Compositor Support for 7422/7425
 *
 * Hydra_Software_Devel/1255   9/23/10 10:30a vanessah
 * SW7425-29:  STG support for transcoding
 *
 * Hydra_Software_Devel/1254   9/21/10 6:11p yuxiaz
 * SW7422-31, SW7422-32: Handle single and dual buffer 3D cases in MFD.
 *
 * Hydra_Software_Devel/1253   9/15/10 11:22a yuxiaz
 * SW7422-30: Handle forced capture modes and orientation programming in
 * MFD.
 *
 * Hydra_Software_Devel/1252   9/14/10 3:44p yuxiaz
 * SW7422-28: Use new 3D formats
 *
 * Hydra_Software_Devel/1251   9/13/10 4:35p yuxiaz
 * SW7358-4: Added initial VDC support for 7358.
 *
 * Hydra_Software_Devel/1250   9/3/10 2:21p vanessah
 * SW7422-52:  MAD + MCVP on the same chip, coverity and check error
 *
 * Hydra_Software_Devel/1247   8/27/10 3:36p vanessah
 * SW7420-965:  Get rid of coverity warning.
 *
 * Hydra_Software_Devel/1246   8/26/10 8:38p albertl
 * SW7405-4556, SW7405-4515: Fixed newline build warning.
 *
 * Hydra_Software_Devel/1243   8/26/10 5:28p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/1242   8/26/10 2:37p vanessah
 * SW7125-556: SW7420-965:  Support for MAD 4 fields 1 delay and MCVP game
 * mode
 *
 * Hydra_Software_Devel/SW7420-965/2   8/20/10 1:55p vanessah
 * SW7420-965:  Eliminate Warning and error for chips earlier than 7420
 *
 * Hydra_Software_Devel/SW7420-965/1   8/20/10 1:30p vanessah
 * SW7420-965:  MCVP game mode support
 *
 * Hydra_Software_Devel/SW7420-588/1   8/9/10 7:24p vanessah
 * SW7420-588:  panscan to zoom video loss
 *
 * Hydra_Software_Devel/1240   8/3/10 5:15p rpan
 * SW7420-895: Back out the "Report buffer delay based on writer and
 * reader node gap" change.
 *
 * Hydra_Software_Devel/1239   8/2/10 11:15a rpan
 * SW7420-895: Report buffer delay based on writer and reader node gap.
 *
 * Hydra_Software_Devel/1238   7/13/10 11:27a yuxiaz
 * SW3548-2987: Added new 1080p@30hz support for 3DTV.
 *
 * Hydra_Software_Devel/1237   6/30/10 2:45p yuxiaz
 * SW7405-4507: solid fill the destination buffer before scaling
 *
 * Hydra_Software_Devel/1236   6/22/10 11:39a rpan
 * SW7420-856: Address another Coverity error.
 *
 * Hydra_Software_Devel/1235   6/21/10 5:26p rpan
 * SW7420-856: Attempt to fix a Coverity error.
 *
 * Hydra_Software_Devel/1234   6/18/10 8:52p rpan
 * SW7400-2763: Fixed compile errors.
 *
 * Hydra_Software_Devel/1233   6/18/10 4:57p yuxiaz
 * SW7405-4437: Fixed scaling ratio when source scan out is smaller than
 * minimal size.
 *
 * Hydra_Software_Devel/1232   6/18/10 4:38p rpan
 * SW7400-2763: Fixed compile warming/error.
 *
 * Hydra_Software_Devel/1231   6/18/10 4:25p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/1230   6/17/10 5:38p hongtaoz
 * SW3548-2912: added support for pseudo-1080i input for 3DTV; allow API
 * to disable SCL vertical phase shift;
 *
 * Hydra_Software_Devel/1229   5/7/10 7:21p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/1228   5/3/10 4:58p syang
 * SW7405-4239: 1). use uint64_t for aspect ratio numbers; 2). avoid even
 * pixel rounding before aspect ratio correction calculation
 *
 * Hydra_Software_Devel/1227   4/23/10 6:00p tdo
 * SW7401-3828: Support AFD=2, 3 for asia market. Add the AFD clipping for
 * SAR that has equivalent DAR of 4x3 and 16x9.
 *
 * Hydra_Software_Devel/1226   4/23/10 3:50p yuxiaz
 * SW7405-4234: Kernel crash while playback 480p in PIP and 720p in Main
 * and switch display format from 1080i to 1080p30.
 *
 * Hydra_Software_Devel/1225   4/19/10 10:25p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/1224   4/15/10 5:11p hongtaoz
 * SW3548-2678: removed modulo sw algorithm to match unique delay target;
 * this may be slower to converge;
 *
 * Hydra_Software_Devel/1223   4/14/10 3:49p yuxiaz
 * SW3548-2883: Video broken on some source formats in config 15.
 *
 * Hydra_Software_Devel/1222   4/7/10 11:38a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/1221   4/5/10 4:18p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/1220   4/2/10 2:57p yuxiaz
 * SW7405-3965: Mosaic decoder can't be attached to the window of the 2nd
 * display.
 *
 * Hydra_Software_Devel/1219   4/1/10 4:49p syang
 * SW7405-4090: ignore asp-ratio correction in mosaic mode
 *
 * Hydra_Software_Devel/1218   3/30/10 5:35p hongtaoz
 * SW3548-2828: only alternate the 2160i windows for 3D 1080p input on
 * legacy 3DTV chipsets; default window game delay callback off and
 * cleaned up drift delay, game delay computation based on source refresh
 * rate instead of hard-coded frame time;
 *
 * Hydra_Software_Devel/1217   3/25/10 3:36p yuxiaz
 * SW7405-3965, SW7405-3135: Fixed channel change and HDSD simul mode in
 * mosaic mode.
 *
 * Hydra_Software_Devel/1216   3/17/10 12:05p syang
 * SW7405-4046: set canvas ctrl after sur ctrl; handle gfx win vnet the
 * same as video; ensure that reader and writer vnetState match in sync-
 * locked case; aligned width up to even for stride in feeder and cap;
 * assert mosaic mode doesn't co-exist with dest cut;
 *
 * Hydra_Software_Devel/1215   3/11/10 5:35p albertl
 * SW3548-2700: Added float debug messages.  Fixed incorrect value for Mc
 * when doing RGB attenuation with user matrices.
 *
 * Hydra_Software_Devel/1214   2/25/10 4:09p yuxiaz
 * SW7405-1601: VDC should expose adjusted scale-out rects after aspect
 * ratio cut-out
 *
 * Hydra_Software_Devel/1213   2/24/10 3:05p albertl
 * SW7125-143: Set bMosaicMode to false on init.
 *
 * Hydra_Software_Devel/1212   2/24/10 1:33p albertl
 * SW7125-143: Fixed bad checkin.
 *
 * Hydra_Software_Devel/1211   2/23/10 6:22p albertl
 * SW7125-143: Restored changes lost in bad merge.
 *
 * Hydra_Software_Devel/1210   2/23/10 6:10p albertl
 * SW7125-143: Added mosaic configuration defaults to window init.
 *
 * Hydra_Software_Devel/1209   2/23/10 3:48p yuxiaz
 * SW7405-3890: Set clipping parameters for minimal size scan out.
 *
 * Hydra_Software_Devel/1208   2/16/10 2:49p yuxiaz
 * SW7405-3890: video has corrupted on clipping.
 *
 * Hydra_Software_Devel/1207   2/11/10 11:01a pntruong
 * SW7405-3829: Moved shrinkwidth to mad settings.
 *
 * Hydra_Software_Devel/1206   2/4/10 11:31a pntruong
 * SW7405-3829: Restored the mad pig mode for chipsets that allow it.
 *
 * Hydra_Software_Devel/1205   2/1/10 6:44p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added HDMI1.4 3D formats support
 * for 3548; added multi-buffer low delay mode support for 1to2 and 2to5
 * rate gap cases; reduced capture buffer allocation for interlaced
 * pulldown case;
 *
 * Hydra_Software_Devel/1204   1/28/10 2:54p pntruong
 * SW7405-3829: Fixed another build error.
 *
 * Hydra_Software_Devel/1203   1/28/10 2:02p pntruong
 * SW7405-3829: fixed build error.
 *
 * Hydra_Software_Devel/1202   1/28/10 12:13p syang
 * SW7405-3829: fixed a problem with cap bufHeapId due to mad width adj
 *
 * Hydra_Software_Devel/1201   1/28/10 11:12a syang
 * SW7405-3829: add option for app to shrink src width to use deinterlace
 *
 * Hydra_Software_Devel/BCM3D/6   1/29/10 6:57p hongtaoz
 * SW3548-2680, SW3548-2681: removed 1470p custom formats since they are
 * the same as 720p3D formats; added macro to wrap the 3548 specific
 * legacy 3DTV format; reduced window buffer allocation needed for
 * pulldown case; disable hddvi DE if invalid HVstart;
 *
 * Hydra_Software_Devel/BCM3D/5   1/29/10 11:27a hongtaoz
 * SW3548-2680, SW3548-2681: reduce window buffer allocation needs as
 * early as possible to fit 6x2HD buffer for 1080p3D sw workaround;
 *
 * Hydra_Software_Devel/BCM3D/4   1/15/10 6:35p hongtaoz
 * SW3548-2680, SW3548-2681: added custom formats to support HDMI 1.4 3D
 * 720p50/60 input passed-thru to external panel processor; reanmed 2160i
 * to custom DVI format;
 *
 * Hydra_Software_Devel/BCM3D/3   1/15/10 4:53p hongtaoz
 * SW3548-2680, SW3548-2681: added HDMI1.4 3D format 720p50 input support;
 *
 * Hydra_Software_Devel/BCM3D/2   1/14/10 5:18p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: fixed build error due to the
 * changes on other mainline VDC code
 *
 * Hydra_Software_Devel/1200   1/13/10 9:34p pntruong
 * SW3556-967: Corrected the 3dcomb delay reports in callback for pal
 * formats.
 *
 * Hydra_Software_Devel/1199   1/13/10 8:49p albertl
 * SW3548-2700: Fixed RGB attenuation for 7420.
 *
 * Hydra_Software_Devel/1198   1/13/10 11:53a rpan
 * SW7400-2278: Always store picture's display polarity to help the case
 * that reader node moved by writer ISR.
 *
 * Hydra_Software_Devel/1197   1/12/10 8:43p albertl
 * SW3548-2700: Changed RGB attenuation to use DVO matrix as YCbCr->RGB
 * matrix and Matrix C as RGB->YCbCr matrix.
 *
 * Hydra_Software_Devel/1196   1/7/10 9:16a pntruong
 * SW3556-886, SW3548-2701: Fixed typo.
 *
 * Hydra_Software_Devel/1195   1/6/10 6:17p pntruong
 * SW3556-886, SW3548-2701: Don't clip in vfd if there are compression
 * enabled.
 *
 * Hydra_Software_Devel/1194   12/23/09 2:04p tdo
 * SW7408-26: Add new TNT support
 *
 * Hydra_Software_Devel/1193   12/22/09 5:15p tdo
 * SW7401-3828: Add AFD support
 *
 * Hydra_Software_Devel/1192   12/21/09 10:36a yuxiaz
 * SW7405-3633: Fix Coverity warings on 7325.
 *
 * Hydra_Software_Devel/1191   12/18/09 10:30a yuxiaz
 * SW7405-3633: Fix Coverity warings in VDC
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:10p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added game mode delay support
 * for 1to2 and 2to5 src/disp rate ratioes; added HDMI1.4 3D DTV format
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/BCM3D/1   12/17/09 6:10p hongtaoz
 * SW3548-2678, SW3548-2680, SW3548-2681: added game mode delay support
 * for 1to2 and 2to5 src/disp rate ratioes; added HDMI1.4 3D DTV format
 * handling for 11-bit BVN;
 *
 * Hydra_Software_Devel/1190   12/16/09 5:12p pntruong
 * SW3556-886: Fixed typo on unnecessary cliping in scl on writer case.
 *
 * Hydra_Software_Devel/1189   12/15/09 8:28p pntruong
 * SW3556-886: TTX operation is wrong in SECAM signal.  Dynamically turn
 * on/off secam filter in sop/eop. Also use vfd clipping for this case.
 *
 * Hydra_Software_Devel/1188   12/9/09 3:52p pntruong
 * SW3556-880: Need to re-evaluate maaf filter on input change that may
 * not trigger bvn reconfiguration.
 *
 * Hydra_Software_Devel/1187   12/4/09 11:37a yuxiaz
 * SW7405 -3542: Garbage video shows when format change in mosaic mode.
 *
 * Hydra_Software_Devel/1186   12/2/09 3:05p yuxiaz
 * SW7405-3120: Added support for overlapping mosaic videos with z-
 * ordering
 *
 * Hydra_Software_Devel/1185   11/30/09 4:51p tdo
 * CR7125-160: HIST window wrong for 1080i <-> 1080p
 *
 * Hydra_Software_Devel/1184   11/24/09 5:47p yuxiaz
 * SW7420-462: Added new tuning functions for Tuning Database.
 *
 * Hydra_Software_Devel/1183   11/19/09 10:16a pntruong
 * SW7408-13: Initial check in to get 7408 build.
 *
 * Hydra_Software_Devel/1182   11/17/09 2:44p rpan
 * SW7468-20: 7468 work.
 *
 * Hydra_Software_Devel/1181   11/16/09 5:04p yuxiaz
 * SW7405-3406: Clean up bMosaicMode and  bMosaicClearOutside.
 *
 * Hydra_Software_Devel/1180   11/12/09 2:54p pntruong
 * SW7340-84, SW7405-3310: Need to keep mad off for pig mode until full
 * rts analysis has been done.
 *
 * Hydra_Software_Devel/1179   11/10/09 4:32p tdo
 * SW7400-2574: Adjust TNT based on input and output format
 *
 * Hydra_Software_Devel/1178   11/6/09 12:05p yuxiaz
 * SW7405-3355: Always use scl-cap bias for bScalerFirst.
 *
 * Hydra_Software_Devel/1177   11/2/09 2:04p rpan
 * SW7405-3310: Added restriction on "enable deinterlacer for PIG on
 * legacy platforms" back 7400.
 *
 * Hydra_Software_Devel/1176   10/29/09 1:42p hongtaoz
 * SW7405-3310: enable deinterlacer for PIG on legacy platforms;
 *
 * Hydra_Software_Devel/1175   10/27/09 10:53p albertl
 * SW7420-30: Changed mosaic matrices to be loaded statically with xvYcc
 * matrix loaded dynamically as necessary.
 *
 * Hydra_Software_Devel/1174   10/26/09 4:44p pntruong
 * SW3548-1649, SW7420-22: Used new scl-cap bias function, and corrected
 * default for force capture to match rts doc.
 *
 * Hydra_Software_Devel/1173   10/26/09 4:00p syang
 * SW7340-60: add CapOut, VfdOut and WinIn into madDeleay handlling
 *
 * Hydra_Software_Devel/1172   10/22/09 10:50a hongtaoz
 * SW7043-233: added MAD file log tool to help debug sw algorithm; window
 * writer should not set repeat flag;
 *
 * Hydra_Software_Devel/1171   10/16/09 5:20p darnstein
 * SW7405-3191: Back out all changes in pixel frequency handling. New
 * design coming soon.
 *
 * Hydra_Software_Devel/1170   10/15/09 4:49p darnstein
 * SW7405-3191: Pixel frequency is now defined as both a bitmask, and an
 * enum. The bitmask option is DEPRECATED.
 *
 * Hydra_Software_Devel/1169   10/14/09 8:35p pntruong
 * SW3548-2503: Repeated switch between 1080i RGB and 1080p24 YCbCr inputs
 * on HDMI results in a jerky dazzle video condition.  Correctly update
 * anr's splitbuf state flag.
 *
 * Hydra_Software_Devel/1168   10/12/09 4:56p tdo
 * SW7420-381: HIST window size sometimes incorrect
 *
 * Hydra_Software_Devel/1167   10/12/09 4:37p rpan
 * SW7405-3159: Moved writer/reader time stamp register from CAP/VFD to
 * RDC scratch registers.
 *
 * Hydra_Software_Devel/1166   10/9/09 5:30p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/1165   10/5/09 11:21a yuxiaz
 * SW7405-3068: Fixed a coverity issue.
 *
 * Hydra_Software_Devel/1164   10/2/09 4:51p yuxiaz
 * SW7405-3068: BVN hang in mosaic mode.
 *
 * Hydra_Software_Devel/1163   9/29/09 4:39p syang
 * SW7405-3046: 1). calc NrmHrz/VrtSrcStep in InitMuteRec_isr before
 * calling to AutoAdjRelatedToSclRatio_isr; 2). use AdjDstRec for cap/buf
 * size if scl before cap
 *
 * Hydra_Software_Devel/1162   9/29/09 1:52p yuxiaz
 * SW7405-3085: Fixed field inversion on SD output in HD/SD mosaic mode
 * when all mosaic channels are progressive.
 *
 * Hydra_Software_Devel/1161   9/29/09 12:41p yuxiaz
 * SW7405-3094: Garbage video shown on left edge in mosaic mode.
 *
 * Hydra_Software_Devel/1160   9/14/09 4:13p hongtaoz
 * SW3548-2471: added window API to allow smooth transition for MAD game
 * modes; updated MAD 3-field game modes QM constellations;
 *
 * Hydra_Software_Devel/1159   9/14/09 2:08p pntruong
 * SW3548-2439: Improved debug messages.
 *
 * Hydra_Software_Devel/1158   9/11/09 10:08a rpan
 * SW3548-2439: Roll back change "Refer to a BVN module's information only
 * if it is being used.".
 *
 * Hydra_Software_Devel/1157   9/3/09 10:33p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/1156   9/3/09 3:05p rpan
 * SW3548-2439: Trigger a rectangle reconfiguration when reconfiguring
 * VNET.
 *
 * Hydra_Software_Devel/1155   9/2/09 9:35p rpan
 * SW3548-2439: Fixed a cut-n-paste error.
 *
 * Hydra_Software_Devel/1154   9/2/09 6:01p rpan
 * SW3548-2439: Refer to a BVN module's information only if it is being
 * used.
 *
 * Hydra_Software_Devel/1153   9/1/09 5:22p tdo
 * SW7420-303: Cleanup unused parameters in contrast stretch structure
 *
 * Hydra_Software_Devel/1152   8/28/09 2:36p hongtaoz
 * SW7342-25: flush MAD delay buffers when display format switch;
 *
 * Hydra_Software_Devel/1151   8/25/09 7:29p albertl
 * SW7125-10: Initial 7125 support.
 *
 * Hydra_Software_Devel/1150   8/25/09 11:57a darnstein
 * SW7340-30: initialize bSclCapSymmetric the same way for all the new
 * settop chips 7420, 7340, 7342, 7550, 7125.
 *
 * Hydra_Software_Devel/1149   8/19/09 2:01p rpan
 * PR57853: Played some tricks to make Coverity silent.
 *
 * Hydra_Software_Devel/1148   8/18/09 4:15p rpan
 * PR57098: Flush buffers when no video detected. This solves the HDMI
 * garbage issue.
 *
 * Hydra_Software_Devel/1147   8/18/09 11:56a darnstein
 * PR55812: Handle resource table for 7550 in same way as for 7340 and
 * 7342.
 *
 * Hydra_Software_Devel/1146   8/13/09 4:16p syang
 * PR 55812: handle non-supported PEP the same way as 7405
 *
 * Hydra_Software_Devel/1145   8/13/09 3:54p tdo
 * PR55225: Initial VDC/Appframework support for 7342
 *
 * Hydra_Software_Devel/1144   8/11/09 6:02p rpan
 * PR57584: Correct alignment handling for mosaic mode.
 *
 * Hydra_Software_Devel/1143   8/6/09 5:00p pntruong
 * PR56800, PR56802, PR57098, PR57149: Synchonized the test feature
 * settings with source pending.
 *
 * Hydra_Software_Devel/1142   8/4/09 4:05p yuxiaz
 * PR56674: More fix for DCX alignment.
 *
 * Hydra_Software_Devel/1141   7/30/09 5:23p tdo
 * PR57236: Remove CSC update when new dynamic contrast is enabled
 *
 * Hydra_Software_Devel/1140   7/21/09 2:44p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/1139   7/20/09 5:08p yuxiaz
 * PR56674: Fixed HSCL/ANR/MAD alignment for DCX.
 *
 * Hydra_Software_Devel/1138   7/15/09 5:20p pntruong
 * PR55237: Added options to avoid black flashes on dst and/or src change.
 *
 * Hydra_Software_Devel/1137   7/15/09 12:13p syang
 * PR 56605: even cap/vfd/win width align in the case of cap after scl
 *
 * Hydra_Software_Devel/1136   7/9/09 10:54a syang
 * PR 56526: 1). not-align up sclOut.width up if 444 supported; 2). align
 * hsclOut / anrIn/Out / MadIn/Out width up to even number
 *
 * Hydra_Software_Devel/1135   7/8/09 6:37p tdo
 * PR56150: refine the condition of when to turn on/off mad
 *
 * Hydra_Software_Devel/1134   7/7/09 3:08p syang
 * PR 56605: fix a div by 0 (*pulNrmSrcStep) problem
 *
 * Hydra_Software_Devel/1133   7/7/09 12:55p syang
 * PR 56605: re-org asp ratio correction msg
 *
 * Hydra_Software_Devel/1132   7/6/09 3:46p syang
 * PR 56605:  added handle for sclOut expanding due to sclRatio rounding
 *
 * Hydra_Software_Devel/1131   7/6/09 10:31a syang
 * PR 56605: xfer dstCut to src-clipping before aspect ratio correction;
 * add  adjFlags to indicate specific item to be adjusted, for accuracy
 * in later apply
 *
 * Hydra_Software_Devel/1130   6/19/09 6:29p darnstein
 * PR55225: make some adjustments for 7342.
 *
 * Hydra_Software_Devel/1129   6/18/09 5:54p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/1128   6/15/09 5:17p pntruong
 * PR56026: Don't restrict MAD to tie to CMP_0 only.
 *
 * Hydra_Software_Devel/1127   6/11/09 4:01p darnstein
 * PR55878: fill in some details for 7340 chipset.
 *
 * Hydra_Software_Devel/1126   6/8/09 12:18p syang
 * PR 55510: ensure pre-mad alignment don't go beyong fullSize
 *
 * Hydra_Software_Devel/1125   6/5/09 3:09p pntruong
 * PR54615: [M+T][LCD][VIDEO] Pink gargabe blinks on DTV.  Ensured that
 * the window shutdown process go thru upon destroy.
 *
 * Hydra_Software_Devel/1124   6/4/09 2:00p yuxiaz
 * PR55330, PR55323, PR53197, PR55158, PR55604: Need to use separate CAP
 * and VFD buffers in ANR when DCX is enabled.
 *
 * Hydra_Software_Devel/1123   6/2/09 3:04p syang
 * PR 55646:  smoothly filtering  in field inversion cases
 *
 * Hydra_Software_Devel/1122   5/28/09 2:20p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/1121   5/28/09 1:55p hongtaoz
 * PR55439, PR53688: MAD trick mode repeat doesn't take effect
 * occasionally;
 *
 * Hydra_Software_Devel/1120   5/21/09 4:55p rpan
 * PR55160: Swapped the calling sequence of function
 * BVDC_P_Window_DecideBufsCfgs_isr() and
 * BVDC_P_Window_SetCadenceHandling_isr() in BVDC_P_Writer_isr() so that
 * we get accurate information for picture capture polarity
 * determination.
 *
 * Hydra_Software_Devel/1119   5/20/09 11:47p pntruong
 * PR55332: Improved error msg.  Should not report error testfeature if
 * it's not enabled.
 *
 * Hydra_Software_Devel/1118   5/15/09 6:47p pntruong
 * PR53654: Avoid no flash on digital source changes up to 720p.  Need
 * compile option BVDC_NO_FLASH=1.
 *
 * Hydra_Software_Devel/1117   5/13/09 5:22p syang
 * PR53688: flush content or cutting-before-mad change for mad trick mode
 *
 * Hydra_Software_Devel/1116   5/12/09 5:31p tdo
 * PR54944: Allow bypass of LAB_SAT_GAIN even when DynCont is enabled
 *
 * Hydra_Software_Devel/1115   5/12/09 9:11a syang
 * PR53688: rewrite AdjustPicRepeatBit_isr for syc-lock win
 *
 * Hydra_Software_Devel/1114   5/11/09 2:31p rpan
 * PR47337: Make BVDC_ENABLE_50HZ_60HZ_FRAME_CAPTURE a compile flag which
 * can be controlled by makefile.
 *
 * Hydra_Software_Devel/1113   5/6/09 4:20p syang
 * PR53688:  more minor cleanup in AdjustForMadDelay_isr
 *
 * Hydra_Software_Devel/1112   5/6/09 11:59a syang
 * PR53688: take off unneeded msg
 *
 * Hydra_Software_Devel/1111   5/6/09 11:23a syang
 * PR53037, PR53038, PR53687, PR53688: cleanup AdjustForMadDelay_isr for
 * readabilty
 *
 * Hydra_Software_Devel/1110   4/28/09 1:42p rpan
 * PR47621: Minor user capture buffer implementation clean-up.
 *
 * Hydra_Software_Devel/1109   4/27/09 6:43p syang
 * PR 54447: add back dnr disabling code and add BuildWinId usage
 *
 * Hydra_Software_Devel/1108   4/27/09 6:42p rpan
 * PR54630, PR47621: Added matching user specified ePixelFmt into the
 * valid user capture buffer criteria. Cleaned up dirty bits related to
 * user capture buffer handling a bit.
 *
 * Hydra_Software_Devel/1107   4/27/09 2:21p rpan
 * PR54630: Assign the most current pixel format for user capture buffer.
 *
 * Hydra_Software_Devel/1106   4/24/09 6:40p tdo
 * PR54181: Take into account user clipping parameters in source scanout
 * rect calculation.
 *
 * Hydra_Software_Devel/1105   4/24/09 9:08a syang
 * PR 51176: cleanup and add some comments
 *
 * Hydra_Software_Devel/1104   4/24/09 12:06a tdo
 * PR45740: Also take into account the delta value.  Refactor code to
 * avoid duplicate logics.
 *
 * Hydra_Software_Devel/1103   4/22/09 12:03p rpan
 * PR54379: Added support for the case that XVD DM scans out frame
 * pictures when doing 60->50Hz conversion.
 *
 * Hydra_Software_Devel/1102   4/21/09 1:57p tdo
 * PR45740: Modify 7420 bandwidth equation to take into account the
 * progressive or interlaced picture size, also user scaler bias
 * selection.
 *
 * Hydra_Software_Devel/1101   4/21/09 9:58a syang
 * PR  53484, PR 52037: clean up AlignPreMadRect_isr
 *
 * Hydra_Software_Devel/1100   4/20/09 1:36p albertl
 * PR54196: Backed out incorrect coverity fix.  Fix already in latest
 * code.
 *
 * Hydra_Software_Devel/1099   4/17/09 7:04p albertl
 * PR54196: Fixed coverity issue.
 *
 * Hydra_Software_Devel/1098   4/17/09 2:04p yuxiaz
 * PR54062: Fixed Compiler warning with DEBUG=n
 *
 * Hydra_Software_Devel/1097   4/17/09 11:56a pntruong
 * PR54064: Refactored common code for defered callback til bvn shutdown.
 * Improved readability and extensibility of dirty bits.
 *
 * Hydra_Software_Devel/1096   4/14/09 2:10p yuxiaz
 * PR54096: Video and OSD both disappear if Input source is HDMI or
 * Component of 1080I.
 *
 * Hydra_Software_Devel/1095   4/10/09 4:16p syang
 * PR 53484: set BVDC_P_DST_CUT_TOP_THRESH back to 0 for 3548, otherwise
 * PR 53484 still fails
 *
 * Hydra_Software_Devel/1094   4/8/09 5:11p yuxiaz
 * PR53942: Added pixel format in error message.
 *
 * Hydra_Software_Devel/1093   4/7/09 6:54p tdo
 * PR53037, PR53038, PR53687, PR53688: Use proper MAD hardstart querry for
 * 7420
 *
 * Hydra_Software_Devel/1092   4/6/09 7:17p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: reset sw MAD delay pipeline in
 * window context when display format switch and MAD is at vnet reader
 * side to prevent BVN error;
 *
 * Hydra_Software_Devel/1091   4/6/09 5:20p albertl
 * PR52810: Fixed display size not matching surface size when
 * BVDC_P_Window_WinOutSetEnable_isr is called.
 *
 * Hydra_Software_Devel/1090   4/6/09 4:34p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: rewrote the
 * BVDC_P_Window_AdjustForMadDelay_isr to truely pipeline the picture
 * info to adapt to various deinterlacer delays;
 *
 * Hydra_Software_Devel/1089   4/6/09 3:42p syang
 * PR  53484 , PR 52037: cleanup (rect, scl-ratio...) rounding code and
 * AdjustRect code; add 1 line delay to TV chip "early trigger"; change
 * BVDC_P_DST_CUT_TOP_THRESH to 4 for 3548;
 *
 * Hydra_Software_Devel/1088   4/3/09 8:03p albertl
 * PR52810: Fixed clear rect code to apply only to the window calling the
 * function.
 *
 * Hydra_Software_Devel/1087   4/3/09 4:35p yuxiaz
 * PR53460: Optimal 444/422 conversion settings
 *
 * Hydra_Software_Devel/1086   4/3/09 3:32p yuxiaz
 * PR52821: Fixed overflow in throughput calculation.
 *
 * Hydra_Software_Devel/1085   4/2/09 7:58p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: fixed writer vnet shutdown order
 * issue;
 *
 * Hydra_Software_Devel/1084   4/2/09 1:38p hongtaoz
 * PR53037, PR53038, PR53687, PR53688: reduced window state checking in
 * build RUL isr since it's never hit;
 *
 * Hydra_Software_Devel/1083   4/2/09 10:04a hongtaoz
 * PR53037, PR53038, PR53687, PR53688: fixed BVN errors during vnet
 * reconfig; video window writer shutdown as soon as possible so window
 * shutdown would take 2 vsyncs less than before;
 *
 * Hydra_Software_Devel/1082   3/30/09 4:12p rpan
 * PR53677: Add support for XVD NTSC->PAL frame drop mode.
 *
 * Hydra_Software_Devel/1081   3/30/09 2:22p hongtaoz
 * PR53037, PR53038: Implement the VEC Locking/Alignments For Memory
 * Reduction; Implement Double-Capture-Buffer Algorithm Under VEC Locking
 * Scheme
 *
 * Hydra_Software_Devel/1080   3/27/09 5:51p tdo
 * PR53629: Turn off MAD if input and output format matched
 *
 * Hydra_Software_Devel/1079   3/27/09 4:58p albertl
 * PR52810: Changed stNewInfo to stCurInfo in BVDC_P_Window_Reader_isr.
 *
 * Hydra_Software_Devel/1078   3/27/09 4:26p albertl
 * PR52810: Constant color fixed during format changes and re-enabled.
 *
 * Hydra_Software_Devel/1077   3/25/09 12:56p tdo
 * PR53553: Force vnet reconfigure when anr/mad change for 7420 to resolve
 * the problem of MCVP hangs due to lack of hard start
 *
 * Hydra_Software_Devel/1076   3/20/09 11:16a yuxiaz
 * PR52770: Fixed BVDC_P_OPTIMIZE_MEM_USAGE usage.
 *
 * Hydra_Software_Devel/1075   3/20/09 10:36a pntruong
 * PR53429: Corrected the mad/anr mem saving flag.
 *
 * Hydra_Software_Devel/1074   3/19/09 11:36p pntruong
 * PR53429: Corrected the anr/mad/hscl vnet connection.  Removed ununsed
 * bAnrMadCombo flags.
 *
 * Hydra_Software_Devel/1073   3/19/09 4:39p yuxiaz
 * PR52770: Optimize memory usage for MAD and ANR.
 *
 * Hydra_Software_Devel/1072   3/19/09 3:39p tdo
 * PR45785, PR45789: Fix problem with 720p source playback
 *
 * Hydra_Software_Devel/1071   3/19/09 1:40p rpan
 * PR53289: Make sure writer and reader not point to the same buffer node
 * when in source repeated mute moe.
 *
 * Hydra_Software_Devel/1070   3/19/09 9:51a rpan
 * PR53289: Stop advancing writer node when in the source repeated mute
 * mode.
 *
 * Hydra_Software_Devel/1069   3/18/09 10:17a rpan
 * PR53104: Fixed an issue which caused non-zero value being written to
 * the reserved bits of CMP_x_G0_SURFACE_CTRL register.  VEC could be
 * killed by this at certain case.
 *
 * Hydra_Software_Devel/1068   3/18/09 9:52a yuxiaz
 * PR52840, PR53325: Fixed Coverity Defect issues.
 *
 * Hydra_Software_Devel/1067   3/17/09 5:20p syang
 * PR 53334:   writer shut-down at same time as reader; setBuildWinId for
 * shutting down writer
 *
 * Hydra_Software_Devel/1066   3/16/09 10:52p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/24   3/16/09 1:02p tdo
 * PR45785, PR45789: merge from main branch on 3/16/09
 *
 * Hydra_Software_Devel/1065   3/13/09 5:29p syang
 * PR 52376, PR 41481, PR 44041:
 * 1). exec user buf returning to hBuffer before shut-down code in
 * writer_isr; 2). clean up mad-delay handling code and add handling to
 * all settings in pic-node that are delayed by mad; 3). 444 SCL no-
 * longer uses BVN_IN to align-up for half-band; 4). add pic-node rects
 * dump feature as setting lbox_0_scratch to 2 (1 still printing mpeg pic
 * info).
 *
 * Hydra_Software_Devel/7420_mcvp/23   3/13/09 11:15a tdo
 * PR45785, PR45789: merge from main branch on 3/13/09
 *
 * Hydra_Software_Devel/1064   3/12/09 2:15p albertl
 * PR52358: Fixed bug causing csc not to update during format changes.
 *
 * Hydra_Software_Devel/7420_mcvp/22   3/4/09 5:24p tdo
 * PR45785, PR45789: merge from main branch on 3/04/09
 *
 * Hydra_Software_Devel/1063   3/4/09 4:10p tdo
 * PR51106: Support table base dynamic contrast algorithm.  Change
 * precision of level threshold for histogram data.
 *
 * Hydra_Software_Devel/7420_mcvp/21   3/2/09 4:22p tdo
 * PR 45785, PR 45789: merge from main branch on 3/02/09
 *
 * Hydra_Software_Devel/1062   3/2/09 3:47p yuxiaz
 * PR52287: More update on 422-444 and 444-422 converters.
 *
 * Hydra_Software_Devel/1061   2/27/09 8:18p pntruong
 * PR52425: Aspect Ratio calculation problem when playing some divx files.
 *
 * Hydra_Software_Devel/7420_mcvp/20   2/26/09 11:18p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/1060   2/26/09 6:12p yuxiaz
 * PR52287: 422-444 and 444-422 converters should use decimate/duplicate.
 *
 * Hydra_Software_Devel/7420_mcvp/19   2/26/09 2:55p tdo
 * PR 45785, PR 45789: merge from main branch on 2/26/09
 *
 * Hydra_Software_Devel/1059   2/26/09 2:40p tdo
 * PR52555: always calculate BVN total delay
 *
 * Hydra_Software_Devel/1058   2/25/09 11:35p pntruong
 * PR47734, PR50316: Fixed typo by node 1021.
 *
 * Hydra_Software_Devel/7420_mcvp/18   2/25/09 10:00a tdo
 * PR 45785, PR 45789: merge from main branch on 2/25/09
 *
 * Hydra_Software_Devel/1057   2/23/09 3:05p yuxiaz
 * PR51004: Fixed floating point calculation. Use ulVertFreq instread of
 * ulPxlFreq.
 *
 * Hydra_Software_Devel/1056   2/23/09 1:51p yuxiaz
 * PR51004: Implement new bandwidth equation for MPEG source
 *
 * Hydra_Software_Devel/7420_mcvp/17   2/23/09 12:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/23/09
 *
 * Hydra_Software_Devel/1055   2/20/09 8:27p albertl
 * PR52009: Enabled window constant color for 7420.  Disabled for all
 * other current platforms.
 *
 * Hydra_Software_Devel/1054   2/20/09 2:50p yuxiaz
 * PR51178: Change DCX bpp requires hard start for MAD.
 *
 * Hydra_Software_Devel/7420_mcvp/16   2/20/09 8:17p albertl
 * PR52009: Enabled window constant color for 7420.  Disabled for all
 * other current platforms.
 *
 * Hydra_Software_Devel/7420_mcvp/15   2/18/09 7:03p syang
 * PR 45785, PR 45789: fix 3556 compile error
 *
 * Hydra_Software_Devel/7420_mcvp/14   2/18/09 12:41p syang
 * PR52030: update mcvp code to use new MadPixelHeapId
 *
 * Hydra_Software_Devel/7420_mcvp/13   2/18/09 11:55a syang
 * PR 45785, PR 45789: merge from main branch on 2/18/09
 *
 * Hydra_Software_Devel/1053   2/13/09 11:15a yuxiaz
 * PR52030: MAD QM do not use DCX logic.
 *
 * Hydra_Software_Devel/7420_mcvp/12   2/12/09 6:16p tdo
 * PR 45785, PR 45789: Use correct delay for MCDI
 *
 * Hydra_Software_Devel/7420_mcvp/11   2/11/09 9:50p tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/1052   2/11/09 8:31p pntruong
 * PR52037: Get green screen when updating scalar clip/dest coordinates.
 *
 * Hydra_Software_Devel/7420_mcvp/10   2/11/09 3:01p tdo
 * PR 45785, PR 45789:  Fix source polarity when MCVP is at writer
 *
 * Hydra_Software_Devel/7420_mcvp/9   2/11/09 11:29a tdo
 * PR 45785, PR 45789: merge from main branch on 2/11/09
 *
 * Hydra_Software_Devel/1051   2/11/09 11:05a tdo
 * PR50391: Add smoothscaling for 7420
 *
 * Hydra_Software_Devel/1050   2/10/09 7:41p albertl
 * PR45798: Fixed compile error.
 *
 * Hydra_Software_Devel/1049   2/10/09 7:08p albertl
 * PR45798: Changed mosaic mode to assign HD, Unknown, or first available
 * colorspace to rect if there are too many unique colorspaces for mosaic
 * cscs to handle.
 *
 * Hydra_Software_Devel/1048   2/10/09 11:16a yuxiaz
 * PR51614: VDC global symbol naming convention violations
 *
 * Hydra_Software_Devel/7420_mcvp/8   2/9/09 1:17p tdo
 * PR 45785, PR 45789: merge from main branch on 2/9/09
 *
 * Hydra_Software_Devel/1047   2/7/09 2:54p pntruong
 * PR51916: VDC still needs to better handle missing aspect_ratio.
 *
 * Hydra_Software_Devel/7420_mcvp/7   2/6/09 2:22p tdo
 * PR 45785, PR 45789: merge from main branch on 2/6/09
 *
 * Hydra_Software_Devel/1046   2/5/09 7:42p pntruong
 * PR51815: [GDVR-KR] Video Blinking happens in DTV 1280x720P at a
 * specific ClipRect Size.
 *
 * Hydra_Software_Devel/1045   2/5/09 4:50p yuxiaz
 * PR51160, PR51327: Add Hsize Alignment for DCX Workaround
 *
 * Hydra_Software_Devel/7420_mcvp/6   2/5/09 4:32p syang
 * PR 45785, PR 45789:  remove a bad assert in BVDC_P_Window_Reader_isr
 *
 * Hydra_Software_Devel/7420_mcvp/5   2/5/09 10:53a syang
 * PR 45785, PR 45789: merge from main branch on 2/5/09
 *
 * Hydra_Software_Devel/1044   2/3/09 7:16p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/7420_mcvp/4   2/5/09 9:53a syang
 * PR 45785, PR 45789:   get rid of BVDC_Source_Get/SetAnrConfiguration
 *
 * Hydra_Software_Devel/7420_mcvp/3   2/3/09 7:23p syang
 * PR 45785, PR 45789: merge from main branch on 2/3/09
 *
 * Hydra_Software_Devel/1043   2/2/09 9:07p albertl
 * PR45798: Fixed window timeout.
 *
 * Hydra_Software_Devel/1042   2/2/09 1:24a albertl
 * PR45798: Fixed compiler warning.
 *
 * Hydra_Software_Devel/1040   1/31/09 4:33a albertl
 * PR45798: Fixed compile error on non chips without mosaic cscs.
 *
 * Hydra_Software_Devel/1039   1/31/09 3:31a albertl
 * PR45798: Fixed bugs and enabled mosaic mode csc implementation.
 *
 * Hydra_Software_Devel/1038   1/30/09 11:53a rpan
 * PR47337: Do pull down for any case that display vertical rate is faster
 * than source. This feature is turned off by default.
 *
 * Hydra_Software_Devel/1037   1/30/09 5:19a pntruong
 * PR51345: Fixed build errors.
 *
 * Hydra_Software_Devel/1036   1/29/09 11:22p albertl
 * PR51345:  Fixed coverity issue with null hMad32.
 *
 * Hydra_Software_Devel/7420_mcvp/2   1/29/09 5:01p syang
 * PR 45785, PR 45789: merge from main branch on 1/29/09
 *
 * Hydra_Software_Devel/1035   1/27/09 8:58p tdo
 * PR51627: add VDC 7336 PI support
 *
 * Hydra_Software_Devel/1034   1/27/09 3:54p tdo
 * PR47776: Enforce height minimum HW limitation in consider with
 * interlace vs. progressive source and destination
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:32p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/1033   1/20/09 4:42p yuxiaz
 * PR50833: PIP swap results in Error BVDC_ERR_RESOURCE_NOT_AVAILABLE for
 * MAD.
 *
 * Hydra_Software_Devel/1032   1/16/09 4:20p tdo
 * PR46529, PR51020: Use correct current writer node to store histogram
 * and completely freeze APL when source mute
 *
 * Hydra_Software_Devel/1031   1/16/09 11:43a yuxiaz
 * PR51160: Updated throughput calculation for MAD/ANR.
 *
 * Hydra_Software_Devel/1030   1/15/09 1:15p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/1029   1/12/09 1:38p albertl
 * PR45798: Added CMP Mosaic CSC implementation for 7420.
 *
 * Hydra_Software_Devel/1028   1/9/09 2:24p syang
 * PR 50758: call Anr_BuildRul_SrcInit_isr if anr internal PxlFmt changes,
 * rather than according to pPicComRulInfo->PicDirty
 *
 * Hydra_Software_Devel/1027   1/9/09 11:22a syang
 * PR 50876:  disabling lbox now releases the lbox-forced capture
 *
 * Hydra_Software_Devel/1026   1/8/09 3:52p jessem
 * PR 50316: Added stringent conditional for interpolating sharpness.
 *
 * Hydra_Software_Devel/1025   1/6/09 11:47a rpan
 * PR49634: Use the trick mode flag passed by decoder instead of trying to
 * detect it.
 *
 * Hydra_Software_Devel/1024   1/6/09 9:56a rpan
 * PR50759: Take the capture buffer held by user into count when re-
 * allocating buffers.
 *
 * Hydra_Software_Devel/1023   12/31/08 2:23p rpan
 * PR50629: No forced alternative capturing for 50Hz source to 60Hz
 * progressive display.
 *
 * Hydra_Software_Devel/1022   12/23/08 10:44p tdo
 * PR50391: Properly define resource table for 7420
 *
 * Hydra_Software_Devel/1021   12/23/08 9:46a jessem
 * PR 50316: Backed out changes to previous version. Need further debug.
 *
 * Hydra_Software_Devel/1020   12/23/08 8:53a jessem
 * PR 50316: Added conditional to check for bResume when calling
 * BVDC_P_Tnt_InterpolateSharpness.
 *
 * Hydra_Software_Devel/1019   12/18/08 9:55p syang
 * PR 50002: invoke ReleasePictureNodes_isr after FrcCap turnt off during
 * srcPending
 *
 * Hydra_Software_Devel/1018   12/18/08 8:46p pntruong
 * PR50502: Used correct display/source rate for computing the number of
 * buffers.
 *
 * Hydra_Software_Devel/1017   12/15/08 2:42p rpan
 * PR50374: Always update window flags such as "bFrameCapture" during
 * format switch.
 *
 * Hydra_Software_Devel/1016   12/15/08 1:49p yuxiaz
 * PR49865: Fixed vnetmode calculation.
 *
 * Hydra_Software_Devel/1015   12/10/08 2:04p albertl
 * PR45808: Updated 7420 CSC support and reworked CMP CSC versioning.
 *
 * Hydra_Software_Devel/1014   12/9/08 8:52p tdo
 * PR45740: Support New BVN Bandwidth Equation for 7420
 *
 * Hydra_Software_Devel/1013   12/4/08 6:37p pntruong
 * PR49979: For chips with vdb let app dictate wheather to capture or not
 * for mpeg, since vdc does not have this information.
 *
 * Hydra_Software_Devel/1012   12/3/08 5:26p jessem
 * PR 48992: Removed extraneous code.
 *
 * Hydra_Software_Devel/1011   12/3/08 5:04p jessem
 * PR 48992: Added Chroma Histogram API support.
 *
 * Hydra_Software_Devel/1010   12/3/08 4:39p rpan
 * PR45804: Initial effort for the orthogonal VEC support.
 *
 * Hydra_Software_Devel/1009   12/3/08 3:14p yuxiaz
 * PR49370, PR48313: Coverity Defect ID:13270 UNINIT
 *
 * Hydra_Software_Devel/1008   11/25/08 1:41p rpan
 * PR47337: Downgrade trick mode debug message type from BDBG_WRN to
 * BDBG_MSG.
 *
 * Hydra_Software_Devel/1007   11/25/08 11:13a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/1006   11/24/08 4:24p albertl
 * PR49415: Fixed FORWARD_NULL coverity issue.
 *
 * Hydra_Software_Devel/1005   11/24/08 1:46p rpan
 * PR47337: Reset stored last capture and source polarity only during vnet
 * reconfig.
 *
 * Hydra_Software_Devel/1004   11/24/08 11:03a yuxiaz
 * PR49598: Use bandwidth equation to calculate throughput.
 *
 * Hydra_Software_Devel/1003   11/20/08 2:51p rpan
 * PR47337: Added trick mode detection. Replaced feedback based capture
 * polarity prediction mechanism with a non-feedback based mechanism.
 * This would help reduce latency.
 *
 * Hydra_Software_Devel/1002   11/20/08 9:31a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/1001   11/19/08 7:14p tdo
 * PR49488: Add condition checking for HW support hist before accessing
 * Hist handle to avoid segfault.
 *
 * Hydra_Software_Devel/1000   11/18/08 6:58p albertl
 * PR48855:  Fixed bug preventing static initial CSCs from loading
 * properly on chips without non-linear CSCs.
 *
 * Hydra_Software_Devel/999   11/13/08 2:51p syang
 * PR 48970:  added back bShuDown setting in ApplyChanges for win destroy
 *
 * Hydra_Software_Devel/998   11/13/08 2:06p tdo
 * PR48866: Correctly guarding FGT mem usage to make graphic surface
 * display via MPEG feeder works for vdc_test
 *
 * Hydra_Software_Devel/997   11/12/08 3:14p tdo
 * PR48642: Provide clipping rect for histo luma region
 *
 * Hydra_Software_Devel/996   11/10/08 5:02p syang
 * PR 41898: remove un-needed bools from win-context and dirty bits
 *
 * Hydra_Software_Devel/995   11/7/08 5:45p albertl
 * PR48740: Fixed compiler warnings when debug is turned off.
 *
 * Hydra_Software_Devel/994   11/7/08 4:46p syang
 * PR 41898:  1).  VnetMode is no-longer decided in ApplyChanges_isr (now
 * only decided in writer_isr, and  DetermineVnetMode ->  DecideCapture);
 * 2). writer_isr  is refined for readability (buf related duplicated
 * code are put in DecideBufsCfgs_isr, shut-down code into
 * WriterShutDown); 3). simplified some dirty bits usage
 *
 * Hydra_Software_Devel/993   11/4/08 7:10p pntruong
 * PR48029: Added control for gain/horz/core weight factor for dejagging.
 * Enforced dejagging on only if passed threshold.
 *
 * Hydra_Software_Devel/992   11/3/08 3:47p yuxiaz
 * PR46305: More on testfeature1.
 *
 * Hydra_Software_Devel/991   10/29/08 12:38p tdo
 * PR48513: Round up SCL output width to even number in
 * BVDC_P_Window_InitMuteRec_isr.
 *
 * Hydra_Software_Devel/990   10/28/08 9:15p pntruong
 * PR48028, PR48029: Allow controlling of scaler's dejagging and
 * deringing.
 *
 * Hydra_Software_Devel/989   10/28/08 12:17a hongtaoz
 * PR48454: fixed the OSD blinking issue by removing the CMP drain;
 *
 * Hydra_Software_Devel/988   10/23/08 5:54p albertl
 * PR47318, PR47814: Fixed overflow when calculating RGB attenuation
 * offsets.  Rewrote portions of RGB attenuation code and fixed various
 * offset related issues.
 *
 * Hydra_Software_Devel/987   10/23/08 1:59p tdo
 * PR46529: Implement a consistent delay for the dynamic contrast
 * algorithm
 *
 * Hydra_Software_Devel/986   10/16/08 8:09p tdo
 * PR48009: enable Color_conv for MA, MB, MC for 3548 B0
 *
 * Hydra_Software_Devel/985   10/16/08 3:06p syang
 * PR 47896:  fix the addr error for V1 sur register
 *
 * Hydra_Software_Devel/984   10/15/08 2:14p jessem
 * PR 46489: Added check to new Sharpness setting when interpolating
 * sharpness.
 *
 * Hydra_Software_Devel/983   10/15/08 1:24p jessem
 * PR 46489: Fixed bug in storing interpolated sharpness. Changed
 * BVDC_P_Tnt_StoreInterpolatedSharpnessValues to
 * BVDC_P_Tnt_InterpolateSharpness.
 *
 * Hydra_Software_Devel/982   10/9/08 5:30p syang
 * PR 46891: add _isr to the name of func used in _isr context
 *
 * Hydra_Software_Devel/981   10/9/08 2:51p syang
 * PR 42693:  perform even numebr align for SCL-1/2 according to eId (for
 * 3548 B0)
 *
 * Hydra_Software_Devel/980   10/8/08 12:18p pntruong
 * PR46735: B0: Need SECAM Dr/Db adjustment implemented.
 *
 * Hydra_Software_Devel/979   10/7/08 4:05p syang
 * PR 41898: 1). move video sur enabling (in RUL) from cmp to win together
 * with other modules in  readerBuilRul;  2). get rid of muteReader; 3).
 * consolidate sur constant color usage impl with vnet start/shut-down
 *
 * Hydra_Software_Devel/978   10/7/08 9:54a jessem
 * PR 47304, PR 46489: Added TNT interpolation table support and private
 * functions.
 *
 * Hydra_Software_Devel/977   10/6/08 4:43p rpan
 * PR47593: When returning a user capture buffer, search the user used
 * buffer nodes  for matching the returned picture node.
 *
 * Hydra_Software_Devel/976   10/6/08 1:12p pntruong
 * PR47595, PR47072: Fixed pixel formats for cap/mad.
 *
 * Hydra_Software_Devel/975   10/3/08 11:59a syang
 * PR42693:  1).  always make width even; 2). improve bypass win
 * validation in AdjustRectangles
 *
 * Hydra_Software_Devel/974   10/3/08 11:01a yuxiaz
 * PR46305, PR46307, PR46309: Use new TestFeature1 function from BSUR.
 * Change BVDC_P_SUPPORT_VIDEO_TestFeature1 to
 * BVDC_P_SUPPORT_VIDEO_TESTFEATURE1.
 *
 * Hydra_Software_Devel/973   10/3/08 9:25a yuxiaz
 * PR46305, PR46307, PR46309: More Video TestFeature1 support.
 *
 * Hydra_Software_Devel/972   10/2/08 4:02p tdo
 * PR45790: Implement a Tuning Interface for HSCL Down-Scaling Threshold
 * Size
 *
 * Hydra_Software_Devel/971   10/1/08 8:33p pntruong
 * PR47303, PR46309: Reused the TestFeature1 type in surface.
 *
 * Hydra_Software_Devel/970   9/30/08 4:56p yuxiaz
 * PR46305, PR46307: Video TestFeature1 support: Fix throughput
 * calculation, combine MAD and ANR set TestFeature1 function.
 *
 * Hydra_Software_Devel/969   9/30/08 12:35p jessem
 * PR 46489: Re-added TN2T support using tighter compiler directive to
 * compile only when BVDC_P_SUPPORT_TNT_VER >= 5.
 *
 * Hydra_Software_Devel/968   9/29/08 6:37p pntruong
 * PR 46489: Rollbacked all the build errors introduced.  Todo: need to
 * properly factored out the code for tnt and tnt new.
 *
 * Hydra_Software_Devel/966   9/29/08 12:57p pntruong
 * PR47072: Moving 3563 vdc dedicated branch.
 *
 * Hydra_Software_Devel/965   9/26/08 3:25p yuxiaz
 * PR46305, PR46307: More video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/964   9/25/08 3:52p albertl
 * PR46493: Fixed to build for chips without constant color.
 *
 * Hydra_Software_Devel/963   9/25/08 2:56p albertl
 * PR46493:  Re-added constant color output when no video is detected for
 * 3548 B0.
 *
 * Hydra_Software_Devel/962   9/25/08 2:26p tdo
 * PR46529: Implement a consistent delay for histogram data for the
 * dynamic contrast algorithm
 *
 * Hydra_Software_Devel/961   9/25/08 10:46a rpan
 * PR45319: 1) Add 2 more cases that writer can skip one field;
 * 2) Changed Writer(Reader)RateGap to WriterVsReaderRateCode enum for
 * better readablity.
 *
 * Hydra_Software_Devel/960   9/24/08 6:19p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/959   9/24/08 10:03a syang
 * PR 46742, PR 46739:  add maaf, add drain in anr when mad is not used
 *
 * Hydra_Software_Devel/958   9/23/08 7:55p pntruong
 * PR47160: Window freeing memory needed to be brought to baseline before
 * realloc.
 *
 * Hydra_Software_Devel/957   9/23/08 4:02p tdo
 * PR42693: BVN_SCL: even number for picture width and horiz offset of
 * 4:2:2 SCL_1 and SCL_2 for 3548 B0
 *
 * Hydra_Software_Devel/956   9/23/08 3:16p rpan
 * PR46865: Flag to indicate whether doing frame capture is now a field of
 * window context.
 *
 * Hydra_Software_Devel/955   9/22/08 9:30p pntruong
 * PR47160: Added additional case for capture as frame.  Refactored logic
 * and trigger computation on change.
 *
 * Hydra_Software_Devel/954   9/22/08 4:53p yuxiaz
 * PR46305, PR46307, PR46309: Added video TestFeature1 support for 3548 B0.
 *
 * Hydra_Software_Devel/953   9/22/08 12:34p syang
 * PR 41898: always re-compute vnetMode during vnet reconfigure
 *
 * Hydra_Software_Devel/952   9/22/08 10:22a rpan
 * PR46986: Restore the mute mode functionality which was accidently
 * removed.
 *
 * Hydra_Software_Devel/951   9/19/08 3:56p tdo
 * PR46481: Add dejagging to SCL implementation for 3548 B0
 *
 * Hydra_Software_Devel/950   9/19/08 2:38p syang
 * PR 41898: 1). removed usage of RulVnetMode; 2).  avoid more shut down
 * if vdc is still waiting for resuming after src-pending;  3).  added
 * reader auto-shut-down in the case that pic is muted before vnet-
 * reconfigure (likely signal lost and before lock again).
 *
 * Hydra_Software_Devel/949   9/18/08 3:06p pntruong
 * PR46477: B0: Remove workaround dealing with VDEC/BVB hookup.
 *
 * Hydra_Software_Devel/948   9/16/08 6:45p albertl
 * PR46491: Updated MC_COLOR_CONV_ENABLE field for 3548 B0.
 *
 * Hydra_Software_Devel/947   9/15/08 1:39p rpan
 * PR46865: Multi-buffering writer's skip step (1 frame or 2 fields) now
 * bases on whether pictures are captured as frame.
 *
 * Hydra_Software_Devel/946   9/12/08 10:44a rpan
 * PR46794: Improved handling of increasing from 3 to 5 buffers or
 * decreasing from 5 to 3 buffers.
 *
 * Hydra_Software_Devel/945   9/10/08 4:48p rpan
 * PR45319: Initialize window object's current reader and writer node to
 * buffer's current reader and write node.
 *
 * Hydra_Software_Devel/944   9/9/08 9:03p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/943   9/9/08 4:05p syang
 * PR 46411: re-alloc anr buf (so reconfig vnet)  if bSrcInterlaced
 * changes
 *
 * Hydra_Software_Devel/942   9/8/08 9:51a rpan
 * PR46100: Added more checks to make sure interlaced display won't be
 * affected.
 *
 * Hydra_Software_Devel/941   9/5/08 4:45p tdo
 * PR46484: Bringup appframework for7420
 *
 * Hydra_Software_Devel/940   9/5/08 4:41p rpan
 * PR46100: Reset flag bBufferCntDecrementedForPullDown when determining
 * VNET mode.
 *
 * Hydra_Software_Devel/939   9/5/08 4:28p rpan
 * PR46100: Re-implemented 3 buffers for 1080p24/25/30 source to
 * 1080p48/50/60 display to match the algorithm.
 *
 * Hydra_Software_Devel/938   9/5/08 2:16p rpan
 * PR46100: 3 buffers for 1080p24/25/30 source to 1080p48/50/60 display.
 *
 * Hydra_Software_Devel/937   9/4/08 8:30p pntruong
 * PR46502: Moving 7403 vdc dedicated branch.
 *
 * Hydra_Software_Devel/936   9/4/08 2:28p rpan
 * PR45319: Further cleaned up multi-buffering stuff from other modules.
 *
 * Hydra_Software_Devel/935   9/4/08 1:25p rpan
 * PR45319: Moved multi-buffering related stuff from window to buffer.
 *
 * Hydra_Software_Devel/934   9/2/08 6:06p pntruong
 * PR46314, PR46316: Updated 3dcomb memory allocation, and added buffers
 * alloc debug messages.
 * Hydra_Software_Devel/933   9/2/08 11:53a rpan
 * PR45319: Covert picture node flags to bit-fields to reduce structure
 * size.
 *
 * Hydra_Software_Devel/932   8/28/08 4:11p tdo
 * PR45896: Add new dynamic back-light callback
 *
 * Hydra_Software_Devel/931   8/22/08 1:32p rpan
 * PR45979: Set display size and surface size at the same spot.
 *
 * Hydra_Software_Devel/930   8/22/08 10:29a rpan
 * PR45979: Remove VBI passthru support.
 *
 * Hydra_Software_Devel/929   8/21/08 10:09a rpan
 * PR45620: Like adding vsync delay buffer, no flushing buffer queue
 * needed for adding/removing user capture buffers.
 *
 * Hydra_Software_Devel/928   8/20/08 2:59p rpan
 * PR45381: Reworked the logic of returning user capture buffer node.
 *
 * Hydra_Software_Devel/927   8/14/08 4:26p hongtaoz
 * PR45601: HSCL may be used to support 1080i down-scaling for ANR-only
 * mode as well to reduce bandwidth;
 *
 * Hydra_Software_Devel/926   8/13/08 7:09p pntruong
 * PR45658: [PQ] Need to relax the ANR for 1080p@60Hz mode.
 *
 * Hydra_Software_Devel/925   8/13/08 4:41p yuxiaz
 * PR45588: Video is moving up and down when clipping video window in
 * HD/SD dual mode.
 *
 * Hydra_Software_Devel/924   8/12/08 10:15a rpan
 * PR45381: 1) Added more restriction on which picture node can be
 * returned as user capture buffer; 2) Adding/Removing user capture
 * buffer should be affect vsync delay.
 *
 * Hydra_Software_Devel/923   7/30/08 11:37a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 * Hydra_Software_Devel/922   7/29/08 7:50p pntruong
 * PR40213: Used correct alignment macro.
 *
 * Hydra_Software_Devel/921   7/28/08 1:01p pntruong
 * PR44664: Need to account for the csc compensation before applying csc
 * adjustment.
 *
 * Hydra_Software_Devel/PR44237/2   7/28/08 2:05p mlei
 * PR44664: move the 2x compensation before CMP color matrix adjustments,
 * add 2x compensation for Y offset as well
 *
 * Hydra_Software_Devel/920   7/25/08 3:49p pntruong
 * PR39163: Fixed delay affect of xvycc toggle.
 *
 * Hydra_Software_Devel/919   7/25/08 3:30p yuxiaz
 * PR44041: Fix SclCut settings when both MAD and LBOX are enabled.
 *
 * Hydra_Software_Devel/918   7/24/08 6:42p albertl
 * PR39163:  Hacked HDMI xvYcc controls to affect N2L and L2N bits only.
 *
 * Hydra_Software_Devel/917   7/16/08 9:58a pntruong
 * PR44664: compensate scart csc in compositor.
 *
 * Hydra_Software_Devel/916   7/11/08 4:47p rpan
 * PR43614: Bandwidth equation API change.
 *
 * Hydra_Software_Devel/915   7/11/08 1:14p yuxiaz
 * PR44759: VDC compiler warning in nexus kernel mode builds
 *
 * Hydra_Software_Devel/914   6/30/08 1:48p tdo
 * PR44253: Properly set vnet for HIST and CMP block for analog source
 *
 * Hydra_Software_Devel/913   6/25/08 9:09p pntruong
 * PR39163: Added support xvYcc transfer characteristic from hddvi source.
 *
 * Hydra_Software_Devel/912   6/25/08 4:52p albertl
 * PR44071:  Fixed BVDC_P_Csc_ApplyColorTemp to use pPriCsc.
 *
 * Hydra_Software_Devel/911   6/25/08 1:41p yuxiaz
 * PR43785: Captured CRCs don't match golden CRCs on 3548. Only Shut down
 * BVN if capture is needed.
 *
 * Hydra_Software_Devel/910   6/24/08 2:34p hongtaoz
 * PR43460, PR39154: unify ANR vnet mode decision at isr only and removed
 * user thread decision;
 *
 * Hydra_Software_Devel/909   6/23/08 7:54p tdo
 * PR39331: More tuning for dynamic contrast initial setting
 *
 * Hydra_Software_Devel/908   6/20/08 3:43p tdo
 * PR43662 , PR43645, PR38841: Fix mosaic and SD to HD channel change due
 * to adjust rectangle being ignored when picture is muted.
 *
 * Hydra_Software_Devel/907   6/19/08 6:27p syang
 * PR35549:  avoid to disable Hscaler / mad32 when the handle is NULL
 *
 * Hydra_Software_Devel/906   6/18/08 4:21p tdo
 * PR39331: More tuning for dynamic contrast
 *
 * Hydra_Software_Devel/905   6/18/08 3:07p syang
 * PR35549: 1). src-pending when new win is created; 2). don't do
 * BufferHeap_AllocateBuffers as bSrcPending
 *
 * Hydra_Software_Devel/904   6/18/08 1:46p albertl
 * PR39163:  Fixed demo bit being written on second compositor v0 due to
 * difference in bit layout from first compositor v0 by handling non-
 * linear and linear windows differently.
 *
 * Hydra_Software_Devel/903   6/17/08 5:29p tdo
 * PR39333: Connect HIST at reader side for VDEC source as a work around
 *
 * Hydra_Software_Devel/902   6/17/08 10:59a rpan
 * PR43614: Moved bandwidth equation parameters to BVDC_P_Window_info
 * structure. Changed factor to 1000.
 *
 * Hydra_Software_Devel/901   6/16/08 5:00p rpan
 * PR43614: Added API for user to set/get bandwidth equation parameters.
 *
 * Hydra_Software_Devel/900   6/11/08 11:45p tdo
 * PR39331: Bringup dynamic contrast for 3548/3556
 *
 * Hydra_Software_Devel/899   6/11/08 4:25p hongtaoz
 * PR43460: add HSCL for ANR or MAD usage mode; HSCL seems to shield BVB
 * error going to ANR;
 *
 * Hydra_Software_Devel/898   6/9/08 8:17p tdo
 * PR39333: Hist bringup
 *
 * Hydra_Software_Devel/897   6/5/08 5:04p hongtaoz
 * PR39807: 3548A0 bringup; put SCL in always_enable mode; separate
 * anr+mad combo mode and memory saving mode in macroes;
 *
 * Hydra_Software_Devel/896   6/3/08 9:27a pntruong
 * PR39335: Fixed cm3d initialization for other windows.
 *
 * Hydra_Software_Devel/895   5/30/08 1:39p tdo
 * PR38362, PR38678, PR38841, PR38842: More fine tuning of the BVN HW
 * limitation implementation.
 *
 * Hydra_Software_Devel/894   5/30/08 10:35a rpan
 * PR43051: Obtain the correct buffer type for frame capturing.
 *
 * Hydra_Software_Devel/893   5/29/08 6:55p hongtaoz
 * PR29244: put left/right edges of mosaic rectangles on the even pixels
 * to avoid noisy edges for legacy mosaic hw;
 *
 * Hydra_Software_Devel/892   5/29/08 5:49p tdo
 * PR38362, PR38678, PR38841, PR38842: Undo transfer dest cut
 *
 * Hydra_Software_Devel/891   5/29/08 1:30p tdo
 * PR38362, PR38678, PR38841, PR38842: Enforce Hardware Minimum Size
 * Limitation for BVN Blocks and Partition Between Destination and Source
 * Clipping To Support 2x1 Window Size
 *
 * Hydra_Software_Devel/890   5/28/08 7:06p hongtaoz
 * PR43082: MPEG source interrupt field swap logic needs to consider total
 * BVN delay for lipsync purpose;
 *
 * Hydra_Software_Devel/889   5/27/08 7:05p hongtaoz
 * PR42248, PR42494: added display alignment API;
 *
 * Hydra_Software_Devel/888   5/23/08 3:57p rpan
 * PR43006: At least five buffers required if a window is sync slip, 50Hz
 * source ->60 Hz interlaced display, and feeder goes directly to
 * compositor and VEC.
 *
 * Hydra_Software_Devel/887   5/22/08 5:00p rpan
 * PR42965: Abstract rate gap calculation into a function.
 *
 * Hydra_Software_Devel/886   5/22/08 1:19p pntruong
 * PR42475, PR41898: Rollback pr41898. Need re-revaluation of resource
 * releasing.
 *
 * Hydra_Software_Devel/885   5/21/08 1:55p albertl
 * PR39163:  Updated CMP csc tables to use 10 bit offset values.
 * Corrected to use proper csc tables in conversions involving ntsc, pal,
 * and smpte 240M.  Moved colorclip from window to compositor and changed
 * BVDC_Window_Set/GetColorClip to BVDC_Compositor_Set/GetColorClip.
 *
 * Hydra_Software_Devel/884   5/19/08 6:34p pntruong
 * PR42736: Coverity Defect ID:7838 DEADCODE bvdc_window_priv.c
 * Product=97405nexus.
 *
 * Hydra_Software_Devel/883   5/19/08 10:48a rpan
 * PR37429: Enhanced the forced capturing as frame criteria.
 *
 * Hydra_Software_Devel/882   5/19/08 9:49a rpan
 * PR42656: Reconfig vnet if new buffers come from different heap.
 *
 * Hydra_Software_Devel/881   5/14/08 4:10p syang
 * PR35549: change src pending related msg from err to msg
 *
 * Hydra_Software_Devel/880   5/12/08 2:54p jessem
 * PR 39335: Removed bCm3dEnable and added bCm3dRegionEnable and
 * bCm3dOverlapRegionEnable in BVDC_P_Window_Info. Added CM3D demo
 * support to BVDC_P_Window_ValidateChanges.
 *
 * Hydra_Software_Devel/879   5/12/08 10:08a pntruong
 * PR35549: Adding support for source pending.
 *
 * Hydra_Software_Devel/878   5/5/08 6:25p albertl
 * PR42395:  Fixed warning and using BVDC_P_WIN_IS_GFX_WINDOW on invalid
 * context.
 *
 * Hydra_Software_Devel/877   5/5/08 5:44p albertl
 * PR42395:  Fixed initial settings for rgb attenuation to handle GFD and
 * CMP cscs properly.
 *
 * Hydra_Software_Devel/876   4/29/08 1:53p yuxiaz
 * PR41888: video CRCs not generated correctly due to recent change of
 * reference SW.
 *
 * Hydra_Software_Devel/875   4/25/08 1:50p pntruong
 * PR39333: Updated with feedbacks from DavidE.
 *
 * Hydra_Software_Devel/874   4/25/08 10:26a rpan
 * PR37429: Restore the forced capturing as frame logic.
 *
 * Hydra_Software_Devel/873   4/21/08 4:35p hongtaoz
 * PR35398, PR40895: some compiler might generate floating point
 * instructions;
 *
 * Hydra_Software_Devel/872   4/21/08 3:38p hongtaoz
 * PR35398, PR40895: some compiler generated floating point value in
 * comparison statement;
 *
 * Hydra_Software_Devel/871   4/18/08 1:43p pntruong
 * PR41898: Need better synchronization of window states.  Synchronize the
 * releases of resource and vnetmode together in writer task.
 *
 * Hydra_Software_Devel/870   4/17/08 3:07p rpan
 * PR37429: Deleted the forced capturing as frame logic.
 *
 * Hydra_Software_Devel/869   4/17/08 2:54p yuxiaz
 * PR40957: Refactor out the dynamics capture pixel format computation
 * code.
 *
 * Hydra_Software_Devel/868   4/15/08 2:38p yuxiaz
 * PR29569: Use stRulVnetMode when building RUL for FGT.
 *
 * Hydra_Software_Devel/867   4/11/08 3:06p tdo
 * PR39333: Do not reconfigure vnet when hist is toggled if hist hw is not
 * available
 *
 * Hydra_Software_Devel/866   4/11/08 2:12p tdo
 * PR39333: Properly handle Hist in vnet
 *
 * Hydra_Software_Devel/865   4/10/08 1:16p tdo
 * PR39333: Set hist vnet mode when dirty bit is set.  Passing pointer
 * instead of structure to avoid unnecessary data copying.
 *
 * Hydra_Software_Devel/864   4/4/08 4:50p jessem
 * PR 39335: Removed  bCm3dOvlpRegionChange and bCm3dRegionChange in
 * BVDC_P_Window_Context. Applied setting of dirty bits in "Set"
 * function. Corrected implementation of CM3D RULs.
 *
 * Hydra_Software_Devel/863   4/3/08 11:23a pntruong
 * PR41286: Enable COLOR_CONV_ENABLE field in CMP_0_V0_SURFACE_CTRL.
 *
 * Hydra_Software_Devel/862   4/2/08 4:28p tdo
 * PR39333: Remove warnings
 *
 * Hydra_Software_Devel/861   3/31/08 2:39p jessem
 * PR 39335: Added CM3D support.
 *
 * Hydra_Software_Devel/860   3/27/08 7:41p albertl
 * PR39163:  Implemented non-linear get matrix functions.
 *
 * Hydra_Software_Devel/859   3/27/08 5:10p albertl
 * PR39163:  Implemented non-linear xvYCC csc support for 3548.
 *
 * Hydra_Software_Devel/858   3/26/08 1:15p hongtaoz
 * PR35398, PR40895: added ulVsyncRate and removed bGameModeDelayLags in
 * the window lipsync callback structure; added BFMT_FREQ_FACTOR to unify
 * Vsync rate and pixel frequency calculation;
 *
 * Hydra_Software_Devel/857   3/24/08 4:30p tdo
 * PR39333: Passing Hist handle instead of its pointer to follow the
 * standard.  Using the interlace flag from source instead of compositor
 * when setting the size.
 *
 * Hydra_Software_Devel/856   3/21/08 5:09p tdo
 * PR39333: More Hist implementation for 3548
 *
 * Hydra_Software_Devel/855   3/21/08 12:21p hongtaoz
 * PR39154, PR39202: updated MAD revision ID and validation for 3548;
 *
 * Hydra_Software_Devel/854   3/21/08 12:11p tdo
 * PR39333: More implementation of HIST block
 *
 * Hydra_Software_Devel/853   3/21/08 11:11a hongtaoz
 * PR39154, PR39202: added HSCL support;
 *
 * Hydra_Software_Devel/852   3/21/08 10:42a yuxiaz
 * PR38929: Set default capture pixel format to
 * BVDC_P_CAP_PIXEL_FORMAT_8BIT422 on 3548.
 *
 * Hydra_Software_Devel/851   3/20/08 6:37p pntruong
 * PR39333: Removed build warnings.
 *
 * Hydra_Software_Devel/850   3/20/08 6:28p pntruong
 * PR39333: Fixed checking for hist on new chip.
 *
 * Hydra_Software_Devel/849   3/20/08 11:14a pntruong
 * PR39333: Don't set histogram dirty bit if chipset does not has it.
 * Removed redundant checks of hist present.  Init current info as well
 * so that the get functions can be call prior to first apply to get
 * initial value.
 *
 * Hydra_Software_Devel/848   3/19/08 7:48p hongtaoz
 * PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
 * on 3548; implemented MAD specific change; updated SCL precision for
 * 3548;
 *
 * Hydra_Software_Devel/847   3/19/08 5:43p tdo
 * PR39333: Implement HIST core
 *
 * Hydra_Software_Devel/846   3/13/08 4:12p yuxiaz
 * PR38929: Initial VDC bringup: Added CSC settings for CMP.
 *
 * Hydra_Software_Devel/845   3/10/08 9:28p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/844   3/7/08 9:14p pntruong
 * PR37836, PR39921: Added more helpful info for out of vdc heap to ease
 * debugging.
 *
 * Hydra_Software_Devel/843   3/7/08 10:45a yuxiaz
 * PR40364: fixed KLOCWORK message.
 *
 * Hydra_Software_Devel/842   3/5/08 1:42p tdo
 * PR39417: VDC PI support 3556
 *
 * Hydra_Software_Devel/841   2/28/08 6:23p hongtaoz
 * PR39154, PR36273, PR39202: added HSCL block; enabled MAD/ANR for 3548;
 * fixed non-linear scaling when MAD is on;
 *
 * Hydra_Software_Devel/840   2/27/08 1:54p pntruong
 * PR39294: Removed hallucinating tabs.
 *
 * Hydra_Software_Devel/839   2/26/08 2:24p yuxiaz
 * PR39927: Assertion when swapping main and PIP with Brutus.
 *
 * Hydra_Software_Devel/838   2/25/08 4:11p jessem
 * PR 39294: Used most current hDisplay->stCurInfo.ulVertFreq for
 * ulDstVertRate in BVDC_P_Buffer_GetNextReaderNode_isr. Reset pWindow-
 * >bBufferCntDecremented in BVDC_P_Window_DetermineVnetMode.
 *
 * Hydra_Software_Devel/837   2/21/08 6:17p hongtaoz
 * PR39828: return correct ulVsyncDelay callback value for progressive
 * format and forced capture modes now;
 *
 * Hydra_Software_Devel/836   2/21/08 5:02p pntruong
 * PR39244: Need drain debugging hooked up to HD_DVI, VDEC, and 656in.
 * Improved code readability.
 *
 * Hydra_Software_Devel/835   2/21/08 2:14p hongtaoz
 * PR39154: added a new flag to signal MAD user changes;
 *
 * Hydra_Software_Devel/834   2/20/08 4:16p hongtaoz
 * PR39154: added window API to toggle ANR; prepare for combo ANR/MAD
 * feature on 3548;
 *
 * Hydra_Software_Devel/833   2/20/08 3:01p rpan
 * PR38929: Fixed a cut-n-paste error in bcm3548 resource feature table.
 *
 * Hydra_Software_Devel/832   2/19/08 4:32p pntruong
 * PR38929: Updated resource table for windows.
 *
 * Hydra_Software_Devel/831   2/14/08 5:57p tdo
 * PR39331, PR39332: Remove link error
 *
 * Hydra_Software_Devel/830   2/14/08 4:33p tdo
 * PR39331, PR39332: Regroup PEP block to add support for 3548
 *
 * Hydra_Software_Devel/829   2/13/08 9:57p pntruong
 * PR39421: Now vdc compiled for 3548.
 *
 * Hydra_Software_Devel/828   2/13/08 3:11p pntruong
 * PR39421: Removed unused code to improve readability.
 *
 * Hydra_Software_Devel/827   2/8/08 3:51p yuxiaz
 * PR39367: Need to use 4 buffers for progressive source on interlaced
 * display on sync slipped path, when sync locked display is interlaced.
 *
 * Hydra_Software_Devel/826   2/7/08 2:47p tdo
 * PR38387: Add more restriction logic for when to release resources to
 * avoid race condition between acquiring and releasing shared resources.
 *
 * Hydra_Software_Devel/825   2/7/08 11:52a jessem
 * PR 33613: Reverted back to to version without the modular mutlibuffer.
 * Found a problem with HDDVI sources.
 *
 * Hydra_Software_Devel/823   2/4/08 4:58p pntruong
 * PR39266: Graphic can not show when switch to a MPEG that has
 * disconnected cable.
 *
 * Hydra_Software_Devel/822   1/29/08 7:47p hongtaoz
 * PR38793: Freeze buffer should update picture node information to
 * support MAD trick mode;
 *
 * Hydra_Software_Devel/821   1/29/08 9:25a rpan
 * PR39068: Calculate rate gap when determine VNET mode.
 *
 * Hydra_Software_Devel/820   1/24/08 5:26p tdo
 * PR38710: Add blue stretch support
 *
 * Hydra_Software_Devel/819   1/23/08 11:13a tdo
 * PR38710: Rename blue stretch and green stretch to blue boost and green
 * boost to match their functionality
 *
 * Hydra_Software_Devel/818   1/22/08 6:29p pntruong
 * PR38524: Both OSD and Video flashing (3 times) while the transition
 * while clipping.
 *
 * Hydra_Software_Devel/815   1/13/08 3:33a albertl
 * PR38150: Added 7440 HDMI xvYCC CSC support.
 *
 * Hydra_Software_Devel/814   1/9/08 10:46a yuxiaz
 * PR38613, PR38614: Enable force capture and smooth scaling on 7325 and
 * 7335.
 *
 * Hydra_Software_Devel/813   1/8/08 10:20a rpan
 * PR38234: Update buffer delay count along with needed buffer count so
 * that they are in sync.
 *
 * Hydra_Software_Devel/812   1/2/08 2:45p tdo
 * PR38444: MAD will hang up if vnet is MAD before CAP and input source is
 * HDMI
 *
 * Hydra_Software_Devel/811   12/27/07 11:24a rpan
 * PR37979: Decrement buffer count if either source or display format is
 * progressive.
 *
 * Hydra_Software_Devel/810   12/21/07 10:35a yuxiaz
 * PR38221: Vdc run out of buffer though enough buffers allocated at init.
 * Consolidate buffer selection for capture, mad and anr.
 *
 * Hydra_Software_Devel/809   12/20/07 3:17p tdo
 * PR38331: Fix window apply timeout due to Tab dirty bit not clear when
 * scaler is not connected
 *
 * Hydra_Software_Devel/808   12/19/07 2:04p yuxiaz
 * PR37982: PIP buffers can be used by all windows.
 *
 * Hydra_Software_Devel/807   12/13/07 1:31p tdo
 * PR36898: Bringup 7335
 *
 * Hydra_Software_Devel/806   12/4/07 11:20a yuxiaz
 * PR37887: Only set bHistoRect when PEP is supported. Fixed window apply
 * times out on 7405.
 *
 * Hydra_Software_Devel/805   11/28/07 2:01p tdo
 * PR37413: Provide new Api to set offset and size of histogram window and
 * read corresponding parameters from specific window
 *
 * Hydra_Software_Devel/804   11/20/07 11:11p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/803   11/20/07 11:06p tdo
 * PR36889: Remove strictly alias warnings
 *
 * Hydra_Software_Devel/802   11/19/07 2:00p tdo
 * PR37046: changing the large memory block allocation method from stack
 * to heap
 *
 * Hydra_Software_Devel/801   11/13/07 4:07p rpan
 * PR31084: Added support for getting default de-interlace settings.
 *
 * Hydra_Software_Devel/800   11/6/07 6:50p syang
 * PR 36632: make box-detect react faster for PC input
 *
 * Hydra_Software_Devel/799   11/6/07 2:46p yuxiaz
 * PR29569: pFgtSeiBufferAddress passed from XVD is virtual address now.
 *
 * Hydra_Software_Devel/798   11/5/07 11:47a syang
 * PR 29617:  Set SCL's top/left cut with vsync delay according to MAD
 *
 * Hydra_Software_Devel/797   11/2/07 2:19p syang
 * PR 29617:  prepare to set clip in SCL with delay according to MAD
 *
 * Hydra_Software_Devel/796   10/30/07 7:27p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/795   10/30/07 2:30p rpan
 * PR34798: Removed support for obsoleted versions of ANR.
 *
 * Hydra_Software_Devel/794   10/26/07 8:30p pntruong
 * PR 29617: Removed build warnings.
 *
 * Hydra_Software_Devel/793   10/26/07 6:22p syang
 * PR 29617:  still more fix for smooth vert move
 *
 * Hydra_Software_Devel/792   10/26/07 3:45p syang
 * PR 29617: add support for smooth vertical mov with MAD
 *
 * Hydra_Software_Devel/791   10/23/07 11:18a yuxiaz
 * PR29569, PR36290: Add FGT support on 7405.
 *
 * Hydra_Software_Devel/PR29569/5   10/22/07 5:16p yuxiaz
 * PR29569: More FGT work.
 *
 * Hydra_Software_Devel/PR29569/4   10/22/07 3:22p yuxiaz
 * PR29569: Merge from mainline.
 *
 * Hydra_Software_Devel/790   10/22/07 11:27a pntruong
 * PR35941: Added option to allow smooth transition or bandwidth
 * optimization.
 *
 * Hydra_Software_Devel/789   10/12/07 1:07p jessem
 * PR 36042, PR 36045: Changed captured picture dimensions' basis from
 * pWinOut to pVfdIn in BVDC_P_Window_CapturePicture_isr.
 *
 * Hydra_Software_Devel/788   10/12/07 9:34a jessem
 * PR 36042, PR 36045: Based captured buffer dimensions on pWinOut instead
 * of pCapOut in BVDC_P_Window_CapturePicture_isr.
 *
 * Hydra_Software_Devel/787   10/11/07 6:29p syang
 * PR 35090: coverity (FORWARD_NULL) fix
 *
 * Hydra_Software_Devel/786   10/11/07 2:10p albertl
 * PR35135:  Fixed user csc macros and added 10-bit/8-bit video data
 * conversion.
 *
 * Hydra_Software_Devel/785   10/10/07 5:59p syang
 * PR 34773, PR 35313: avoid to alloc 1080p cap buffer by capturing 1080p
 * for PsP only if needed
 *
 * Hydra_Software_Devel/784   10/9/07 11:39a tdo
 * PR35832: Change initial value for CHROMA_CONTROL.CORE to improve the
 * color smearing issue.
 *
 * Hydra_Software_Devel/783   10/8/07 3:09p yuxiaz
 * PR35827: Always capture after scale for MPEG source on 7405
 *
 * Hydra_Software_Devel/782   10/8/07 2:46p pntruong
 * PR34855: Initial VDC bringup.
 *
 * Hydra_Software_Devel/781   10/2/07 11:27a pntruong
 * PR35136: Some cleanup of unused code.
 *
 * Hydra_Software_Devel/780   9/28/07 11:42a pntruong
 * PR35360: Switching aspect ratio for 1080p/60Hz HDMI/Component input,
 * app kill on 3552.
 *
 * Hydra_Software_Devel/779   9/27/07 2:01p yuxiaz
 * PR35013, PR34523: Add new CSC support on 7325.
 *
 * Hydra_Software_Devel/778   9/24/07 7:29p pntruong
 * PR33354: [VDC] On 3552 (using 32 bit DDR), MAD horizontal down-scaling
 * should always be enabled for 1080i source.  Need to use mad downscale
 * for synclock source as well.
 *
 * Hydra_Software_Devel/777   9/21/07 4:35p yuxiaz
 * PR34523: Add 7325 support for VDC.
 *
 * Hydra_Software_Devel/776   9/21/07 4:00p hongtaoz
 * PR34955: added Progressive Segmented Frame(PsF) 1080p support; fixed
 * 1080p pass-through condition; fixed bvdc_dbg register access error;
 * fixed general progressive capture interlaced playback feeder
 * programming problem;
 *
 * Hydra_Software_Devel/775   9/20/07 1:23a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/774   9/11/07 6:10p syang
 * PR 32094, PR 32621, PR 33942: clean up non-linear scl code and fix it
 * for 7403
 *
 * Hydra_Software_Devel/773   8/30/07 10:29p pntruong
 * PR34472: Use MASK block to improve the Contour/Quantization noise
 * issue.
 *
 * Hydra_Software_Devel/772   8/30/07 3:15p pntruong
 * PR34348: MAD optimizing Low Angle Settings for reduce gray noise.
 *
 * Hydra_Software_Devel/PR34348/1   8/29/07 4:09p xhuang
 * PR34348:add low angle customer settings
 *
 * Hydra_Software_Devel/771   8/17/07 10:02a pntruong
 * PR34059: Need to fix FMT name for 1024x768i_43Hz.
 *
 * Hydra_Software_Devel/770   8/15/07 8:41p pntruong
 * PR28892: Fixed window surface field id.  It should be base on the
 * current display field id.
 *
 * Hydra_Software_Devel/769   8/8/07 6:38p syang
 * PR 29617: make sure horiz move does not change MAD width when
 * SrcCnt.width is odd
 *
 * Hydra_Software_Devel/768   8/7/07 12:43p tdo
 * PR33792: Addtional Parameter for MAD chroma settings
 *
 * Hydra_Software_Devel/767   8/6/07 11:09a pntruong
 * PR33348: Identify mpeg format of hd/sd/ed with curinfo->pfmt.
 *
 * Hydra_Software_Devel/766   8/3/07 12:13a pntruong
 * PR33410: Don't toggle scaler on VNET patch when user switch the view
 * mode.  Removed release build warnings.
 *
 * Hydra_Software_Devel/765   8/2/07 6:41p pntruong
 * PR 33237: Fixed 7403 build errors.
 *
 * Hydra_Software_Devel/764   8/2/07 5:35p syang
 * PR 33237: corrected MAD horizontal scale setup
 *
 * Hydra_Software_Devel/763   8/2/07 1:53p pntruong
 * PR31983: C0: Need to enable 10-bit MAD capture mode when source is
 * 480i.
 *
 * Hydra_Software_Devel/762   8/1/07 4:39p syang
 * PR 29617: make sure horiz move does not change MAD width
 *
 * Hydra_Software_Devel/761   7/31/07 1:22p hongtaoz
 * PR33653: window rectangle adjustment fix;
 *
 * Hydra_Software_Devel/760   7/30/07 2:10p hongtaoz
 * PR26785, PR33560, PR33020: added MAD game mode hard start; removed old
 * transition workaround;
 *
 * Hydra_Software_Devel/759   7/27/07 9:33a yuxiaz
 * PR32421: VDC Should Compute the Heap Size Needed for MAD and ANR for
 * Various Sources.
 *
 * Hydra_Software_Devel/758   7/26/07 6:27p pntruong
 * PR33427:  Need to trigger aspect ratio computation when panscan widnow
 * size changes.
 *
 * Hydra_Software_Devel/752   7/23/07 12:55p jessem
 * PR 32553: Added support for  interlaced with reader rate gap condition
 * (43Hz interlaced PC source). This requires 5 buffers.
 *
 * Hydra_Software_Devel/751   7/20/07 5:02p pntruong
 * PR33256: [VDC] Dynamical RTS settings for Component/HDMI 1080i on 3553
 * is wrong.
 *
 * Hydra_Software_Devel/750   7/19/07 6:40p albertl
 * PR33219:  MAD now disabled when vertical scaling is not being used for
 * 7440.
 *
 * Hydra_Software_Devel/749   7/19/07 5:20p hongtaoz
 * PR31754: fixed the timestamp tolerance check to avoid multi-buffer
 * bouncing jitter;
 *
 * Hydra_Software_Devel/748   7/19/07 3:12p tdo
 * PR33086: Include the missing saturation control in the compositor demo
 * mode
 *
 * Hydra_Software_Devel/747   7/17/07 4:59p albertl
 * PR32593:  Removed now unnecessary 24 to 60Hz fix.
 *
 * Hydra_Software_Devel/746   7/12/07 6:11p albertl
 * PR32593:  Added missing case for BAVC_SourceId_eDs0 and added fixes to
 * 24 to 60 Hz conversion requiring an extra buffer and a wider gap
 * between reader and writer.
 *
 * Hydra_Software_Devel/745   7/10/07 11:41p pntruong
 * PR28550: Don't enabled mad for formats require more memory than 1080i
 * until memory work-sheet is updated.
 *
 * Hydra_Software_Devel/744   7/10/07 6:11p tdo
 * PR32853: Extending the range of CMS saturation and hue gains to match
 * BBS3.0 tool
 *
 * Hydra_Software_Devel/743   7/9/07 12:02p pntruong
 * PR32766: reader picture node moves in repeat mode when source trigger
 * is not stable.
 *
 * Hydra_Software_Devel/742   7/6/07 3:37p hongtaoz
 * PR32777: capture buffer heap alignment issue;
 *
 * Hydra_Software_Devel/PR32766/1   7/4/07 5:35p dyzhang
 * PR32766: reader picture node moves in repeat mode when source trigger
 * is not stable.
 *
 * Hydra_Software_Devel/741   6/29/07 11:48a pntruong
 * PR32503: Updated mad settings for oversampling.
 *
 * Hydra_Software_Devel/740   6/28/07 7:26p hongtaoz
 * PR32130, PR29189: added buffer game mode delay control; fixed picture
 * timestamp update bug when isr reorder occurs; invalid timestamp/drift
 * delay should disappear now; lipsync delay reflects the correct game
 * mode effect;
 *
 * Hydra_Software_Devel/739   6/28/07 5:52p syang
 * PR 28550: 1080p src with frame rate <= 30 is OK for anr
 *
 * Hydra_Software_Devel/738   6/28/07 5:09p pntruong
 * PR32503: Updated to use correct heap id for oversampling mode.
 *
 * Hydra_Software_Devel/737   6/28/07 3:58p syang
 * PR 32576: always re-calculate mad/anr bufHeapId inside
 * NeedReconfiguring_isr if mad/anr is to be sued
 *
 * Hydra_Software_Devel/736   6/27/07 10:05p pntruong
 * PR32503: [VDEC] Add oversampling support.
 *
 * Hydra_Software_Devel/735   6/27/07 1:47p pntruong
 * PR32503: Cleanup unused code.  Prepare for 480i/480p vdec oversample.
 *
 * Hydra_Software_Devel/734   6/26/07 1:45p syang
 * PR 32444: 1). reader don't push writer's pic node if src doesn't feed;
 * 2). next writer pic node returned by buf are marked as muted and are
 * un-muted by writer_isr
 *
 * Hydra_Software_Devel/733   6/21/07 3:28p syang
 * PR 32440: use pSource->bPictureChanged to decide if we re-decide
 * MAD/ANR heapId and re-init ANR
 *
 * Hydra_Software_Devel/732   6/21/07 11:54a pntruong
 * PR31821: Fixed merge error.
 *
 * Hydra_Software_Devel/731   6/21/07 5:40a albertl
 * PR31821:  Changed MAD game modes to allocate only the necessary number
 * of buffers and to reconfigure the vnet when buffer numbers change.
 *
 * Hydra_Software_Devel/730   6/20/07 5:37p syang
 * PR 31944: 1). add a new user defined struct ptr to ANR configure 2).
 * update SNR calculation code to match Jorge's script
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bmth.h"
#include "bsur.h"
#include "bvdc_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_capture_priv.h"
#include "bvdc_scaler_priv.h"
#include "bvdc_hscaler_priv.h"
#include "bvdc_mad_priv.h"
#include "bvdc_dnr_priv.h"
#include "bvdc_feeder_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_vdec_priv.h"
#include "bvdc_hddvi_priv.h"
#include "bvdc_gfxfeeder_priv.h"
#include "bvdc_boxdetect_priv.h"
#include "bvdc_bufferheap_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_656in_priv.h"
#include "bvdc_anr_priv.h"
#include "bvdc_mcvp_priv.h"
#include "bvdc_mcdi_priv.h"
#include "bvdc_rts_priv.h"
#include "bvdc_fgt_priv.h"
#include "bvdc_hist_priv.h"
#include "bvdc_tnt_priv.h"

BDBG_MODULE(BVDC_WIN);
BDBG_FILE_MODULE(BVDC_MADR_PICSIZE);
BDBG_FILE_MODULE(BVDC_ASP_RAT);
BDBG_FILE_MODULE(BVDC_WIN_VNET);
BDBG_FILE_MODULE(BVDC_CMP_SIZE);
BDBG_FILE_MODULE(BVDC_WIN_BUF_SIZE);
BDBG_FILE_MODULE(BVDC_BAR);
BDBG_OBJECT_ID(BVDC_WIN);

/* SW3556-886 */
#define BVDC_FORCE_VFD_TOP_CLIP            BVDC_P_VDEC_SECAM_TTD_BYPASS_FILTER_IN_VBI

/* SW7405-3068 */
#define BVDC_P_WIN_MOSAIC_MODE_BVN_WORKAROUND    (1)

/* HW7420-976 or SW7420-2017 workaround */
#if (BCHP_CHIP==7400) || (BCHP_CHIP==7405) || (BCHP_CHIP==7335)
#define BVDC_P_SCL_V_STEP_SIZE_WORKAROUND        (1)
#else
#define BVDC_P_SCL_V_STEP_SIZE_WORKAROUND        (0)
#endif

/* Short hand for make resoure! */
#define BVDC_P_MAKE_RES(win_id, cap, vfd, scl, mad, pep,                    \
	cap_id, vfd_id, scl_id, mad_id)                                         \
{                                                                           \
	cap, vfd, scl, mad, pep,                                                \
	BVDC_P_CaptureId_e##cap_id,                                             \
	BVDC_P_FeederId_e##vfd_id,                                              \
	BVDC_P_ScalerId_e##scl_id,                                              \
	BVDC_P_MadId_e##mad_id,                                                 \
	BVDC_P_WindowId_e##win_id,                                              \
}

/* INDEX: by BVDC_P_WindowId. */
static const BVDC_P_ResourceRequire s_aResourceRequireTable[] =
{
	/*              win_id    c  f  s  m  p  ids... */
#if (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || \
    (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7340) || \
    (BCHP_CHIP==7342) || (BCHP_CHIP==7550) || (BCHP_CHIP==7468) || \
    (BCHP_CHIP==7408) || (BCHP_CHIP==7125) || (BCHP_CHIP==7360) || \
    (BCHP_CHIP==7563)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0, Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp2_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp3_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp4_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7405) || (BCHP_CHIP==7400)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0, Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2, Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp2_V0, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp3_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp4_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7231) || (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || \
      (BCHP_CHIP==7429) || (BCHP_CHIP==7584)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0, Unknown),
#if (BVDC_SUPPORT_DUAL_HD)
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2, Unknown),
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),
#else
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2, Unknown),
#endif
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 1, 0, 0, Cap3, Vfd3, Scl3, Unknown),
	BVDC_P_MAKE_RES(Comp2_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp3_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp4_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7420)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0,    Vfd0,    Scl0,    Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1,    Vfd1,    Scl1,    Unknown),
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2,    Vfd2,    Scl2,    Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 0, 0, 0, Cap3,    Vfd3,    Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp2_V0, 1, 1, 0, 0, 0, Cap3,    Vfd3,    Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp3_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp4_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0, Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),    /* shared w/ Comp2_V0 */
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2, Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp2_V0, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1, Unknown),    /* shared w/ Comp0_V1 */
	BVDC_P_MAKE_RES(Comp3_V0, 1, 1, 1, 0, 0, Cap4, Vfd4, Scl4, Unknown),
	BVDC_P_MAKE_RES(Comp4_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7435)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0,    Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1,    Unknown), /* shared w/ Comp2_V0 */
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2,    Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown), /* shared w/ Comp3_V0 */
	BVDC_P_MAKE_RES(Comp2_V0, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1,    Unknown), /* shared w/ Comp0_V1 */
	BVDC_P_MAKE_RES(Comp3_V0, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown), /* shared w/ Comp1_V1 */
	BVDC_P_MAKE_RES(Comp4_V0, 1, 1, 1, 0, 0, Cap4, Vfd4, Scl4,    Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 1, 1, 1, 0, 0, Cap5, Vfd5, Scl5,    Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),

#elif (BCHP_CHIP==7445)
	BVDC_P_MAKE_RES(Comp0_V0, 1, 1, 1, 0, 1, Cap0, Vfd0, Scl0,    Unknown),
	BVDC_P_MAKE_RES(Comp0_V1, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1,    Unknown), /* shared w/ Comp2_V0 */
	BVDC_P_MAKE_RES(Comp1_V0, 1, 1, 1, 0, 0, Cap2, Vfd2, Scl2,    Unknown),
	BVDC_P_MAKE_RES(Comp1_V1, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown), /* shared w/ Comp3_V0 */
	BVDC_P_MAKE_RES(Comp2_V0, 1, 1, 1, 0, 0, Cap1, Vfd1, Scl1,    Unknown), /* shared w/ Comp0_V1 */
	BVDC_P_MAKE_RES(Comp3_V0, 1, 1, 0, 0, 0, Cap3, Vfd3, Unknown, Unknown), /* shared w/ Comp1_V1 */
	BVDC_P_MAKE_RES(Comp4_V0, 1, 1, 1, 0, 0, Cap4, Vfd4, Scl4,    Unknown),
	BVDC_P_MAKE_RES(Comp5_V0, 1, 1, 1, 0, 0, Cap5, Vfd5, Scl5,    Unknown),
	BVDC_P_MAKE_RES(Comp6_V0, 1, 1, 1, 0, 0, Cap6, Vfd6, Scl6,    Unknown),

#else
#error "Unknown chip!  Not yet supported in VDC."
#endif

	BVDC_P_MAKE_RES(Comp0_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp0_G1, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp0_G2, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp1_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp2_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp3_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp4_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp5_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
	BVDC_P_MAKE_RES(Comp6_G0, 0, 0, 0, 0, 0, Unknown, Unknown, Unknown, Unknown),
};

#define FTR_SD     (0)
#define FTR_HD     (BVDC_P_Able_eHd)
#define FTR_M0     (BVDC_P_Able_eMem0)
#define FTR_M1     (BVDC_P_Able_eMem1)
#define FTR_M01    (BVDC_P_Able_eMem0 | BVDC_P_Able_eMem1)
#define FTR_HD_M0  (BVDC_P_Able_eMem0 | BVDC_P_Able_eHd)
#define FTR_HD_M1  (BVDC_P_Able_eMem1 | BVDC_P_Able_eHd)
#define FTR_HD_XC  (BVDC_P_Able_eHd | BVDC_P_Able_eXcode)

/* this will cause acquire to fail */
#define FTR___     (BVDC_P_Able_eInvalid)

/* INDEX: by BVDC_P_WindowId. */
static const BVDC_P_ResourceFeature s_aResourceFeatureTable[] =
{
#if (BCHP_CHIP==7125)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp0_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp1_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp2_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp3_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || (BCHP_CHIP==7336) || \
	  (BCHP_CHIP==7358) || (BCHP_CHIP==7552) || (BCHP_CHIP==7360)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp0_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp1_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp2_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp3_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || (BCHP_CHIP==7550) || \
	  (BCHP_CHIP==7468) || (BCHP_CHIP==7408) || (BCHP_CHIP==7563)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp0_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp1_V1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp2_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp3_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7420)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD, FTR___ },
	/*Comp0_V1*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp1_V1*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp2_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp3_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7422) || (BCHP_CHIP==7425)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD,    FTR___ },
	/*Comp0_V1*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD,    FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp1_V1*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp2_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp3_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7400) || (BCHP_CHIP==7405) || (BCHP_CHIP==7231) || \
	  (BCHP_CHIP==7344) || (BCHP_CHIP==7346) || (BCHP_CHIP==7429) || \
	  (BCHP_CHIP==7584)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp0_V1*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD, FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp1_V1*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp2_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD, FTR___ },
	/*Comp3_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7435)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD,    FTR___ },
	/*Comp0_V1*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD,    FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp1_V1*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp2_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp3_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_HD_XC, FTR___ },
	/*Comp4_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp5_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp6_V0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },

#elif (BCHP_CHIP==7445)
	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD,    FTR___ },
	/*Comp0_V1*/{ FTR_M0, FTR_M0, FTR_HD, FTR_SD,    FTR___ },
	/*Comp1_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp1_V1*/{ FTR_M0, FTR_M0, FTR_SD, FTR_SD,    FTR___ },
	/*Comp2_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp3_V0*/{ FTR_M0, FTR_M0, FTR_SD, FTR_HD_XC, FTR___ },
	/*Comp4_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp5_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },
	/*Comp6_V0*/{ FTR_M0, FTR_M0, FTR_HD, FTR_HD_XC, FTR___ },

#else
#error "Unknown chip!  Not yet supported in VDC."
#endif

	/*            ulCap;  ulVfd;  ulScl;  ulMad;  ulAnr; */
	/*Comp0_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp0_G1*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp0_G2*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp1_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp2_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp3_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp4_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp5_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ },
	/*Comp6_G0*/{ FTR___, FTR___, FTR___, FTR___, FTR___ }
};

#define BVDC_P_RESOURCE_TABLE_COUNT \
	(sizeof(s_aResourceRequireTable) / sizeof(BVDC_P_ResourceRequire))

#define BVDC_P_RESOURCE_FEATURE_TABLE_COUNT \
	(sizeof(s_aResourceFeatureTable) / sizeof(BVDC_P_ResourceFeature))

#if (BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT < 2)
#define BCHP_VNET_B_CMP_0_V1_SRC  0
#endif

#if (BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_1_V0_SRC  0
#endif

#if (BVDC_P_CMP_1_MAX_VIDEO_WINDOW_COUNT < 2)
#define BCHP_VNET_B_CMP_1_V1_SRC  0
#endif

#if (BVDC_P_CMP_2_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_2_V0_SRC  0
#else
#ifndef BCHP_VNET_B_CMP_2_V0_SRC
#define BCHP_VNET_B_CMP_2_V0_SRC  BCHP_VNET_B_BP_0_SRC
#endif
#endif

#if (BVDC_P_CMP_3_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_3_V0_SRC  0
#else
#ifndef BCHP_VNET_B_CMP_3_V0_SRC
#define BCHP_VNET_B_CMP_3_V0_SRC  BCHP_VNET_B_BP_0_SRC
#endif
#endif

#if (BVDC_P_CMP_4_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_4_V0_SRC  0
#else
#ifndef BCHP_VNET_B_CMP_4_V0_SRC
#define BCHP_VNET_B_CMP_4_V0_SRC  BCHP_VNET_B_BP_0_SRC
#endif
#endif

#if (BVDC_P_CMP_5_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_5_V0_SRC  0
#else
#ifndef BCHP_VNET_B_CMP_5_V0_SRC
#define BCHP_VNET_B_CMP_5_V0_SRC  BCHP_VNET_B_BP_0_SRC
#endif
#endif

#if (BVDC_P_CMP_6_MAX_VIDEO_WINDOW_COUNT < 1)
#define BCHP_VNET_B_CMP_6_V0_SRC  0
#else
#ifndef BCHP_VNET_B_CMP_6_V0_SRC
#define BCHP_VNET_B_CMP_6_V0_SRC  BCHP_VNET_B_BP_0_SRC
#endif
#endif

/* compositor's win src mux addr: index by eWinId */
static const uint32_t s_aulWinOutMuxAddr[] =
{
	BCHP_VNET_B_CMP_0_V0_SRC,  /* BVDC_P_WindowId_eComp0_V0 = 0 */
	BCHP_VNET_B_CMP_0_V1_SRC,  /* BVDC_P_WindowId_eComp0_V1 */
	BCHP_VNET_B_CMP_1_V0_SRC,  /* BVDC_P_WindowId_eComp1_V0 */
	BCHP_VNET_B_CMP_1_V1_SRC,  /* BVDC_P_WindowId_eComp1_V1 */
	BCHP_VNET_B_CMP_2_V0_SRC,  /* BVDC_P_WindowId_eComp2_V0 */
	BCHP_VNET_B_CMP_3_V0_SRC,  /* BVDC_P_WindowId_eComp3_V0 */
	BCHP_VNET_B_CMP_4_V0_SRC,  /* BVDC_P_WindowId_eComp4_V0 */
	BCHP_VNET_B_CMP_5_V0_SRC,  /* BVDC_P_WindowId_eComp5_V0 */
	BCHP_VNET_B_CMP_6_V0_SRC,  /* BVDC_P_WindowId_eComp6_V0 */
	0,                         /* BVDC_P_WindowId_eComp0_G0 */
	0,                         /* BVDC_P_WindowId_eComp0_G1 */
	0,                         /* BVDC_P_WindowId_eComp0_G2 */
	0,                         /* BVDC_P_WindowId_eComp1_G0 */
	0,                         /* BVDC_P_WindowId_eComp2_G0 */
	0,                         /* BVDC_P_WindowId_eComp3_G0 */
	0,                         /* BVDC_P_WindowId_eComp4_G0 */
	0,                         /* BVDC_P_WindowId_eComp5_G0 */
	0                          /* BVDC_P_WindowId_eComp6_G0 */
};

const bool s_BVDC_abWinNonLinearTbl[] =
{
	BVDC_P_CMP_0_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp0_V0 = 0 */
	BVDC_P_CMP_0_NON_LINEAR_CSC_V1,    /* BVDC_P_WindowId_eComp0_V1 */
	BVDC_P_CMP_1_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp1_V0 */
	BVDC_P_CMP_1_NON_LINEAR_CSC_V1,    /* BVDC_P_WindowId_eComp1_V1 */
	BVDC_P_CMP_2_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp2_V0 */
	BVDC_P_CMP_3_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp3_V0 */
	BVDC_P_CMP_4_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp4_V0 */
	BVDC_P_CMP_5_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp5_V0 */
	BVDC_P_CMP_6_NON_LINEAR_CSC_V0,    /* BVDC_P_WindowId_eComp6_V0 */
	false,                             /* BVDC_P_WindowId_eComp0_G0 */
	false,                             /* BVDC_P_WindowId_eComp0_G1 */
	false,                             /* BVDC_P_WindowId_eComp0_G2 */
	false,                             /* BVDC_P_WindowId_eComp1_G0 */
	false,                             /* BVDC_P_WindowId_eComp2_G0 */
	false,                             /* BVDC_P_WindowId_eComp3_G0 */
	false,                             /* BVDC_P_WindowId_eComp4_G0 */
	false,                             /* BVDC_P_WindowId_eComp5_G0 */
	false                              /* BVDC_P_WindowId_eComp6_G0 */
};

/* blender address tables */
static const uint32_t s_aulBlendAddr[] =
{
	BCHP_CMP_0_BLEND_0_CTRL,
	BCHP_CMP_0_BLEND_1_CTRL,
#if (BVDC_P_CMP_0_MAX_WINDOW_COUNT > 2)
	BCHP_CMP_0_BLEND_2_CTRL,
#if (BVDC_P_CMP_0_MAX_WINDOW_COUNT > 3)
	BCHP_CMP_0_BLEND_3_CTRL,
#if (BVDC_P_CMP_0_MAX_WINDOW_COUNT > 4)
	BCHP_CMP_0_BLEND_4_CTRL
#endif
#endif
#endif
};

/***************************************************************************
 * Window Id lookup table with compostior id, and source id.  This
 * only lookup window id for the first gfx or video window.
 ***************************************************************************/
typedef struct BVDC_P_Window_SelectedId
{
	bool bValid;
	BVDC_P_WindowId eWindowId;
} BVDC_P_Window_SelectedId;

/* INDEX: by BAVC_SourceId & BVDC_P_CompositorId. */
static const BVDC_P_Window_SelectedId s_aaWindowIdSelectTable
	[BVDC_P_MAX_COMPOSITOR_COUNT /* compositor id (0,1,bypass) */]
	[BVDC_P_MAX_SOURCE_COUNT     /* source (mpeg, gfx, etc)    */] =
{
	/* compositor 0: could take any video source or gfx0 */
	{{true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* 656In1 */
	 {true,  BVDC_P_WindowId_eComp0_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx2 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx3 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx4 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp0_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* HdDvi1 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Ds0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Vfd0 */
	 {true,  BVDC_P_WindowId_eComp0_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp0_V0},  /* Vfd2 */
	 {false, BVDC_P_WindowId_eComp0_V0},  /* Vfd3 */
	 {false, BVDC_P_WindowId_eComp0_V0},  /* Vfd4 */
	 {false, BVDC_P_WindowId_eComp0_V0},  /* Vfd5 */
	 {false, BVDC_P_WindowId_eComp0_V0},  /* Vfd6 */
	 {false, BVDC_P_WindowId_eComp0_V0}}, /* Vfd7 */

	/* compositor 1: could take any video source or gfx1 */
	{{true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx0 */
	 {true,  BVDC_P_WindowId_eComp1_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx2 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx3 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx4 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp1_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* HdDvi1 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp1_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp1_V0},  /* Vfd1 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Vfd2 */
	 {true,  BVDC_P_WindowId_eComp1_V0},  /* Vfd3 */
	 {false, BVDC_P_WindowId_eComp1_V0},  /* Vfd4 */
	 {false, BVDC_P_WindowId_eComp1_V0},  /* Vfd5 */
	 {false, BVDC_P_WindowId_eComp1_V0},  /* Vfd6 */
	 {false, BVDC_P_WindowId_eComp1_V0}}, /* Vfd7 */

	/* bypass 0: intended for analog PVR; enforce analog video source */
	{{true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx1 */
	 {true,  BVDC_P_WindowId_eComp2_G0},  /* Gfx2 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx3 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx4 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp2_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* HdDvi1 */
	 {false, BVDC_P_WindowId_eComp2_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp2_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp2_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp2_V0},  /* Vfd2 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vfd3 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vfd4 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vfd5 */
	 {true,  BVDC_P_WindowId_eComp2_V0},  /* Vfd6 */
	 {true,  BVDC_P_WindowId_eComp2_V0}}, /* Vfd7 */

	/* VICE */
	{{true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp3_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp3_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp3_G0},  /* Gfx2 */
	 {true,  BVDC_P_WindowId_eComp3_G0},  /* Gfx3 */
	 {true,  BVDC_P_WindowId_eComp3_G0},  /* Gfx4 */
	 {true,  BVDC_P_WindowId_eComp3_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp3_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* HdDvi1 */
	 {false, BVDC_P_WindowId_eComp3_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp3_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp3_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp3_V0},  /* Vfd2 */
	 {false, BVDC_P_WindowId_eComp3_V0},  /* Vfd3 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Vfd4 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Vfd5 */
	 {true,  BVDC_P_WindowId_eComp3_V0},  /* Vfd6 */
	 {true,  BVDC_P_WindowId_eComp3_V0}}, /* Vfd7 */

	/* VICE */
	{{true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp4_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp4_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp4_G0},  /* Gfx2 */
	 {true,  BVDC_P_WindowId_eComp4_G0},  /* Gfx3 */
	 {true,  BVDC_P_WindowId_eComp4_G0},  /* Gfx4 */
	 {true,  BVDC_P_WindowId_eComp4_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp4_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* HdDvi1 */
	 {false, BVDC_P_WindowId_eComp4_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp4_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp4_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp4_V0},  /* Vfd2 */
	 {false, BVDC_P_WindowId_eComp4_V0},  /* Vfd3 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Vfd4 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Vfd5 */
	 {true,  BVDC_P_WindowId_eComp4_V0},  /* Vfd6 */
	 {true,  BVDC_P_WindowId_eComp4_V0}}, /* Vfd7 */

	/* VICE */
	{{true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp5_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp5_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp5_G0},  /* Gfx2 */
	 {true,  BVDC_P_WindowId_eComp5_G0},  /* Gfx3 */
	 {true,  BVDC_P_WindowId_eComp5_G0},  /* Gfx4 */
	 {true,  BVDC_P_WindowId_eComp5_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp5_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* HdDvi1 */
	 {false, BVDC_P_WindowId_eComp5_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp5_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp5_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp5_V0},  /* Vfd2 */
	 {false, BVDC_P_WindowId_eComp5_V0},  /* Vfd3 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Vfd4 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Vfd5 */
	 {true,  BVDC_P_WindowId_eComp5_V0},  /* Vfd6 */
	 {true,  BVDC_P_WindowId_eComp5_V0}}, /* Vfd7 */

	/* VICE */
	{{true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg0 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg1 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg2 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg3 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg4 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Mpeg5 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Vdec0 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Vdec1 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* 656In0 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* 656In1 */
	 {false, BVDC_P_WindowId_eComp6_G0},  /* Gfx0 */
	 {false, BVDC_P_WindowId_eComp6_G0},  /* Gfx1 */
	 {false, BVDC_P_WindowId_eComp6_G0},  /* Gfx2 */
	 {true,  BVDC_P_WindowId_eComp6_G0},  /* Gfx3 */
	 {true,  BVDC_P_WindowId_eComp6_G0},  /* Gfx4 */
	 {true,  BVDC_P_WindowId_eComp6_G0},  /* Gfx5 */
	 {false, BVDC_P_WindowId_eComp6_G0},  /* Gfx6 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* HdDvi0 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* HdDvi1 */
	 {false, BVDC_P_WindowId_eComp6_V0},  /* Ds0 */
	 {false, BVDC_P_WindowId_eComp6_V0},  /* Vfd0 */
	 {false, BVDC_P_WindowId_eComp6_V0},  /* Vfd1 */
	 {false, BVDC_P_WindowId_eComp6_V0},  /* Vfd2 */
	 {false, BVDC_P_WindowId_eComp6_V0},  /* Vfd3 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Vfd4 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Vfd5 */
	 {true,  BVDC_P_WindowId_eComp6_V0},  /* Vfd6 */
	 {true,  BVDC_P_WindowId_eComp6_V0}}, /* Vfd7 */
};

#define BVDC_P_CMP_GET_REG_ADDR_IDX(reg) \
	((reg- BCHP_CMP_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_CMP_GET_REG_ADDR_DATA(reg) \
	(hCompositor->aulRegs[BVDC_P_CMP_GET_REG_ADDR_IDX(reg)])

#if BVDC_ENABLE_50HZ_60HZ_FRAME_CAPTURE
	/* Enabling frame capture for 50Hz source to 60Hz display
	 * will improve picture quality. However, it will require
	 * more capture memory and system bandwidth. By default we
	 * turn it off.
	 */
#define BVDC_P_DO_PULLDOWN(sourceVertRate, displayVertRate) \
	(displayVertRate > sourceVertRate)
#else
#define BVDC_P_DO_PULLDOWN(sourceVertRate, displayVertRate) \
	(((displayVertRate * 10) / (sourceVertRate)) >= ((50 * 10) / 30) ? 1 : 0)
#endif

#define BVDC_P_FrameRate50Hz(eFrameRate) \
	(BAVC_FrameRateCode_e25 == eFrameRate || BAVC_FrameRateCode_e50 == eFrameRate)

#define BVDC_P_FrameRate60Hz(eFrameRate) \
	(BAVC_FrameRateCode_e29_97 == eFrameRate || BAVC_FrameRateCode_e30 == eFrameRate || \
	 BAVC_FrameRateCode_e59_94 == eFrameRate || BAVC_FrameRateCode_e60 == eFrameRate)

#define BVDC_P_CAL_HRZ_SRC_STEP(InW, OutW) \
	BVDC_P_DIV_ROUND_NEAR(InW << BVDC_P_NRM_SRC_STEP_F_BITS, OutW)

#if BVDC_P_SCL_V_STEP_SIZE_WORKAROUND
#define BVDC_P_CAL_VRT_SRC_STEP(InH, OutH) \
	BVDC_P_DIV_ROUND_NEAR(InH << BVDC_P_NRM_SRC_STEP_F_BITS, BVDC_P_MAX(OutH - 2, 1))
#else
#define BVDC_P_CAL_VRT_SRC_STEP(InH, OutH) \
	BVDC_P_DIV_ROUND_NEAR(InH << BVDC_P_NRM_SRC_STEP_F_BITS, BVDC_P_MAX(OutH, 1))
#endif

#if (BDBG_DEBUG_BUILD)
/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Window_DumpRects_isr
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_PictureNode        *pPicture )
{
	bool bForcePrint;
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor->hVdc, BVDC_VDC);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	bForcePrint = hWindow->hCompositor->hVdc->bForcePrint;
	
	if(bForcePrint)
	{
		BDBG_ERR(("---------------Window[%d]'s VnetMode: 0x%04x-------------",
			hWindow->eId, hWindow->stVnetMode));
		BVDC_P_PRINT_CLIP("SrcClip*", &hWindow->stCurInfo.stSrcClip, bForcePrint);
		BVDC_P_PRINT_RECT("DstRect*", &hWindow->stCurInfo.stDstRect, bForcePrint);
		BVDC_P_PRINT_RECT("SclOut* ", &hWindow->stCurInfo.stScalerOutput, bForcePrint);
	}
	else
	{
		BDBG_MSG(("---------------Window[%d]'s VnetMode: 0x%x--------------",
			hWindow->eId, pPicture->stVnetMode));
	}

	BVDC_P_PRINT_RECT("pScnOut", &hWindow->stCurInfo.hSource->stScanOut, bForcePrint);
	BVDC_P_PRINT_CNT_RECT("pSrcCnt", &(hWindow->stSrcCnt), bForcePrint);

	BVDC_P_PRINT_RECT("pSrcOut", pPicture->pSrcOut, bForcePrint);
	if(BVDC_P_VNET_USED_DNR_AT_WRITER(pPicture->stVnetMode))
	{
		BVDC_P_PRINT_RECT("pDnrIn ", pPicture->pDnrIn, bForcePrint);
		BVDC_P_PRINT_RECT("pDnrOut", pPicture->pDnrOut, bForcePrint);
	}
	if(BVDC_P_VNET_USED_HSCL_AT_WRITER(pPicture->stVnetMode))
	{
		BVDC_P_PRINT_RECT("pHsclIn ", pPicture->pHsclIn, bForcePrint);
		BVDC_P_PRINT_HSCLCUT("pHsclCut", pPicture, bForcePrint);
		BVDC_P_PRINT_RECT("pHsclOut", pPicture->pHsclOut, bForcePrint);
	}
	if(BVDC_P_VNET_USED_ANR_AT_WRITER(pPicture->stVnetMode))
	{
		BVDC_P_PRINT_RECT("pAnrIn ", pPicture->pAnrIn, bForcePrint);
		BVDC_P_PRINT_RECT("pAnrOut", pPicture->pAnrOut, bForcePrint);
	}
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(pPicture->stVnetMode) ||
		BVDC_P_VNET_BYPASS_MCVP_AT_WRITER(pPicture->stVnetMode) )
	{
		BVDC_P_PRINT_RECT("pMadIn ", pPicture->pMadIn, bForcePrint);
		BVDC_P_PRINT_RECT("pMadOut", pPicture->pMadOut, bForcePrint);
	}
	if(BVDC_P_VNET_USED_SCALER(pPicture->stVnetMode))
	{
		if(BVDC_P_VNET_USED_SCALER_AT_READER(pPicture->stVnetMode) &&
		   BVDC_P_VNET_USED_CAPTURE(pPicture->stVnetMode))
		{
			BVDC_P_PRINT_RECT("pCapIn ", pPicture->pCapIn, bForcePrint);
			BVDC_P_PRINT_RECT("pCapOut", pPicture->pCapOut, bForcePrint);
			BVDC_P_PRINT_RECT("pVfdIn ", pPicture->pVfdIn, bForcePrint);
			BVDC_P_PRINT_RECT("pVfdOut", pPicture->pVfdOut, bForcePrint);
		}

		if(BVDC_P_VNET_USED_DNR_AT_READER(pPicture->stVnetMode))
		{
			BVDC_P_PRINT_RECT("pDnrIn ", pPicture->pDnrIn, bForcePrint);
			BVDC_P_PRINT_RECT("pDnrOut", pPicture->pDnrOut, bForcePrint);
		}
		if(BVDC_P_VNET_USED_HSCL_AT_READER(pPicture->stVnetMode))
		{
			BVDC_P_PRINT_RECT("pHsclIn ", pPicture->pHsclIn, bForcePrint);
			BVDC_P_PRINT_HSCLCUT("pHsclCut", pPicture, bForcePrint);
			BVDC_P_PRINT_RECT("pHsclOut", pPicture->pHsclOut, bForcePrint);
		}
		if(BVDC_P_VNET_USED_ANR_AT_READER(pPicture->stVnetMode))
		{
			BVDC_P_PRINT_RECT("pAnrIn ", pPicture->pAnrIn, bForcePrint);
			BVDC_P_PRINT_RECT("pAnrOut", pPicture->pAnrOut, bForcePrint);
		}
		if(BVDC_P_VNET_USED_MAD_AT_READER(pPicture->stVnetMode) ||
			BVDC_P_VNET_BYPASS_MCVP_AT_READER(pPicture->stVnetMode))
		{
			BVDC_P_PRINT_RECT("pMadIn ", pPicture->pMadIn, bForcePrint);
			BVDC_P_PRINT_RECT("pMadOut", pPicture->pMadOut, bForcePrint);
		}

		BVDC_P_PRINT_RECT("pSclIn ", pPicture->pSclIn, bForcePrint);
		BVDC_P_PRINT_SCLCUT_RECT("pSclCut", &(pPicture->stSclCut), bForcePrint);
		BVDC_P_PRINT_RECT("pSclOut", pPicture->pSclOut, bForcePrint);
	}

	if(BVDC_P_VNET_USED_CAPTURE(pPicture->stVnetMode) &&
	   !BVDC_P_VNET_USED_SCALER_AT_READER(pPicture->stVnetMode))
	{
		BVDC_P_PRINT_RECT("pCapIn ", pPicture->pCapIn, bForcePrint);
		BVDC_P_PRINT_RECT("pCapOut", pPicture->pCapOut, bForcePrint);
		BVDC_P_PRINT_RECT("pVfdIn ", pPicture->pVfdIn, bForcePrint);
		BVDC_P_PRINT_RECT("pVfdOut", pPicture->pVfdOut, bForcePrint);
	}

	if(BVDC_P_VNET_USED_FGT(pPicture->stVnetMode))
	{
		BVDC_P_PRINT_RECT("pFgtIn*", pPicture->pFgtIn, bForcePrint);
		BVDC_P_PRINT_RECT("pFgtOut", pPicture->pFgtOut, bForcePrint);
	}

	BVDC_P_PRINT_RECT("pWinIn ", pPicture->pWinIn, bForcePrint);
	BVDC_P_PRINT_RECT("pWinOut", pPicture->pWinOut, bForcePrint);

	bForcePrint
		? BDBG_ERR(("--------------------------------------------------------\n"))
		: BDBG_MSG(("--------------------------------------------------------\n"));
	return;
}
#else
#define BVDC_P_Window_DumpRects_isr(hWindow, pPicture)
#endif

void BVDC_P_Window_SetInvalidVnetMode
	( BVDC_P_VnetMode                  *pVnetMode )
{
	BVDC_P_CLEAN_ALL_DIRTY(pVnetMode);
	pVnetMode->stBits.bInvalid = BVDC_P_ON;
}

#if BVDC_P_SUPPORT_HIST
static bool BVDC_P_Hist_Level_Cmp
	( const uint32_t                   *pulNewThres,
	  const uint32_t                   *pulCurThres )
{
	uint32_t id;
	for(id = 0; id < BVDC_LUMA_HISTOGRAM_LEVELS; id++)
	{
		if(*(pulNewThres + id) != *(pulCurThres + id))
			return false;
	}
	return true;
}
#endif

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Window_GetPrivHandle
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_WindowId                    eWinId,
	  BAVC_SourceId                    eSrcId,
	  BVDC_Window_Handle              *phWindow )
{
	BVDC_Window_Handle  hWindow;
	BVDC_P_WindowId  eWindowId;

	/* Check if this window allow on this compositor?  Some compositor does not
	 * support graphics window that crosses to other compositor. */
	if(!s_aaWindowIdSelectTable[hCompositor->eId][eSrcId].bValid)
	{
		BDBG_ERR(("Window is not available."));
		return BERR_TRACE(BVDC_ERR_WINDOW_NOT_AVAILABLE);
	}

	/* Determine what window id to use. */
	eWindowId = s_aaWindowIdSelectTable[hCompositor->eId][eSrcId].eWindowId;
	if(BVDC_P_SRC_IS_VIDEO(eSrcId) && (eWinId != BVDC_WindowId_eAuto))
	{
		eWindowId += eWinId;
	}

	/* Need to select V0 or V1 for comp0/comp1. */
	if((BVDC_P_SRC_IS_VIDEO(eSrcId)) && (eWinId == BVDC_WindowId_eAuto) &&
	   (hCompositor->pFeatures->ulMaxVideoWindow > 1))
	{
		uint32_t              ulIndex;
		BVDC_Window_Handle hV0, hV1;

		ulIndex = (hCompositor->eId == BVDC_CompositorId_eCompositor0)
			? BVDC_P_WindowId_eComp0_V0 : BVDC_P_WindowId_eComp1_V0;
		hV0 = hCompositor->ahWindow[ulIndex];
		hV1 = hCompositor->ahWindow[ulIndex+1];
		BDBG_OBJECT_ASSERT(hV0, BVDC_WIN);
		BDBG_OBJECT_ASSERT(hV1, BVDC_WIN);

		/* Check if V0 or V1 is available */
		hWindow = (BVDC_P_STATE_IS_INACTIVE(hV0)) ? hV0 : hV1;
	}
	else
	{
		hWindow = hCompositor->ahWindow[eWindowId];
		BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	}

	*phWindow = hWindow;
	return BERR_SUCCESS;
}

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Window_Create
	( BVDC_Compositor_Handle            hCompositor,
	  BVDC_Window_Handle               *phWindow,
	  BVDC_P_WindowId                   eWindowId )
{
	BVDC_P_WindowContext *pWindow;
	BAVC_SourceId  eSrcId;

	BDBG_ENTER(BVDC_P_Window_Create);
	BDBG_ASSERT(phWindow);

	/* Make sure the table have enough elements. */
	BDBG_ASSERT(BVDC_P_RESOURCE_TABLE_COUNT == BVDC_P_MAX_WINDOW_COUNT);
	BDBG_ASSERT(BVDC_P_RESOURCE_FEATURE_TABLE_COUNT == BVDC_P_MAX_WINDOW_COUNT);

	/* Get relate context. */
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hVdc, BVDC_VDC);

	/* (1) Alloc the context. */
	pWindow = (BVDC_P_WindowContext*)
		(BKNI_Malloc(sizeof(BVDC_P_WindowContext)));
	if(!pWindow)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pWindow, 0x0, sizeof(BVDC_P_WindowContext));
	BDBG_OBJECT_SET(pWindow, BVDC_WIN);

	/* Initialize window context */
	pWindow->eId          = eWindowId;
	pWindow->ulRegOffset  = BVDC_P_WIN_GET_REG_OFFSET(eWindowId);
	pWindow->hCompositor  = hCompositor;
	pWindow->pResource    = &s_aResourceRequireTable[eWindowId];
	pWindow->pResourceFeature  = &s_aResourceFeatureTable[eWindowId];

	/* (2) Alloc capture & capture memory for this window. */
	eSrcId = BAVC_SourceId_eVfd0 + (pWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
	if(pWindow->pResource->bRequireCapture)
	{
		/* Get the register handle this will be needed capture to setup the
		 * trigger with host write.  Memory handle is for conveting address. */
		BVDC_P_Capture_Create(&pWindow->stNewResource.hCapture, hCompositor->hVdc->hRdc, hCompositor->hVdc->hRegister,
			pWindow->pResource->eCapture, hCompositor->hVdc->hTimer, hCompositor->hVdc->hResource);

#if (!BVDC_P_USE_RDC_TIMESTAMP)
		if (hCompositor->hVdc->ahSource[eSrcId])
		{
			/* the vfd can not be both in cap/playback pair and an gfx src */
			pWindow->stNewResource.hCapture->ulTimestampRegAddr =
				hCompositor->hVdc->ahSource[eSrcId]->hVfdFeeder->stGfxSurface.ulSurAddrReg[0];
		}
		else
		{
			pWindow->stNewResource.hCapture->ulTimestampRegAddr = BRDC_AllocScratchReg(hCompositor->hVdc->hRdc);
			BDBG_MSG(("AllocScratchReg 0x%x for window[%d] capture %d",
				pWindow->stNewResource.hCapture->ulTimestampRegAddr, pWindow->eId, pWindow->pResource->eCapture));
		}
#endif

		/* Create buffer list for managing multi-buffering.  And create device
		 * memory for capture engine. */
		BVDC_P_Buffer_Create(pWindow, &pWindow->hBuffer);
	}

	/* (3) Alloc playback block */
	if(pWindow->pResource->bRequirePlayback)
	{
		BVDC_P_Feeder_Create(&pWindow->stNewResource.hPlayback, hCompositor->hVdc->hRdc, hCompositor->hVdc->hRegister,
			pWindow->pResource->ePlayback, hCompositor->hVdc->hTimer, NULL, hCompositor->hVdc->hResource, false);
		pWindow->stNewResource.hPlayback->hWindow = pWindow;

#if (!BVDC_P_USE_RDC_TIMESTAMP)
		if (hCompositor->hVdc->ahSource[eSrcId])
		{
			/* the vfd can not be both a playback and an gfx src */
			pWindow->stNewResource.hPlayback->ulTimestampRegAddr =
				hCompositor->hVdc->ahSource[eSrcId]->hVfdFeeder->stGfxSurface.ulVsyncCntrReg;
		}
		else
		{
			pWindow->stNewResource.hPlayback->ulTimestampRegAddr = BRDC_AllocScratchReg(hCompositor->hVdc->hRdc);
			BDBG_MSG(("AllocScratchReg 0x%x for window[%d] playback %d",
				pWindow->stNewResource.hPlayback->ulTimestampRegAddr, pWindow->eId, pWindow->pResource->ePlayback));
		}
#endif

	}

	/* (4) Alloc scaler block */
	if(pWindow->pResource->bRequireScaler)
	{
		BVDC_P_Scaler_Create(&pWindow->stNewResource.hScaler, pWindow->pResource->eScaler,
			hCompositor->hVdc->hResource, hCompositor->hVdc->hRegister);
	}

	/* (5) Alloc PEP block */
	if(pWindow->pResource->bRequirePep)
	{
		BVDC_P_Pep_Create(&pWindow->stNewResource.hPep, pWindow->pResource->eWinId, hCompositor->hVdc->hRegister);
	}

	/* (6) Alloc CM3D block */
#if BVDC_P_SUPPORT_CM3D
	if(pWindow->eId == BVDC_P_WindowId_eComp0_V0)
	{
		BVDC_P_Cm3d_Create(&pWindow->stNewResource.hCm3d, pWindow->pResource->eWinId, hCompositor->hVdc->hRegister);
	}
#endif

	/* (7) create a DestroyDone event. */
	BKNI_CreateEvent(&pWindow->hDestroyDoneEvent);

	/* (8) create a AppliedDone event. */
	BKNI_CreateEvent(&pWindow->hAppliedDoneEvent);

	/* (9) Added this compositor to hVdc */
	hCompositor->ahWindow[pWindow->eId] = (BVDC_Window_Handle)pWindow;

	/* (10) Init SubRul that manage compositer' win src vnet setting */
	BVDC_P_SubRul_Init(&(pWindow->stWinOutVnet),
		s_aulWinOutMuxAddr[pWindow->eId], 0, BVDC_P_DrainMode_eNone, 0,
		hCompositor->hVdc->hResource);

	/* (11) Sync up stNewResource and stCurResource. For historical reasons, these resources are
	* not allocated via the resource management library. Weird, but that's how it has been.
	*/
	pWindow->stCurResource = pWindow->stNewResource;

	/* All done. now return the new fresh context to user. */
	*phWindow = (BVDC_Window_Handle)pWindow;

	BDBG_LEAVE(BVDC_P_Window_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Window_Destroy
	( BVDC_Window_Handle               hWindow )
{
	BAVC_SourceId  eSrcId;

	BDBG_ENTER(BVDC_P_Window_Destroy);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Get the compositor that this window belongs to.   The blender
	 * registers are spread out in the compositor. */
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* [9] Added this compositor to hVdc */
	hWindow->hCompositor->ahWindow[hWindow->eId] = NULL;

	/* [8] Destroy event */
	BKNI_DestroyEvent(hWindow->hAppliedDoneEvent);

	/* [7] Destroy event */
	BKNI_DestroyEvent(hWindow->hDestroyDoneEvent);

	/* [6] Free CM3D block */
#if BVDC_P_SUPPORT_CM3D
	if(hWindow->eId == BVDC_P_WindowId_eComp0_V0)
	{
		BVDC_P_Cm3d_Destroy(hWindow->stCurResource.hCm3d);
	}
#endif

	/* [5] Free PEP block */
	if(hWindow->pResource->bRequirePep)
	{
		BVDC_P_Pep_Destroy(hWindow->stCurResource.hPep);
	}

	/* [4] Free scaler block */
	if(hWindow->pResource->bRequireScaler)
	{
		BVDC_P_Scaler_Destroy(hWindow->stCurResource.hScaler);
	}

	/* [3] Free playback block */
	eSrcId = BAVC_SourceId_eVfd0 + (hWindow->pResource->ePlayback - BVDC_P_FeederId_eVfd0);
	if(hWindow->pResource->bRequirePlayback)
	{
#if (!BVDC_P_USE_RDC_TIMESTAMP)
		if (NULL == hWindow->hCompositor->hVdc->ahSource[eSrcId])
		{
			BDBG_MSG(("FreeScratchReg 0x%x for window[%d] playback %d",
				hWindow->stNewResource.hPlayback->ulTimestampRegAddr,
				hWindow->eId, hWindow->pResource->ePlayback));

			BRDC_FreeScratchReg(hWindow->hCompositor->hVdc->hRdc,
				hWindow->stNewResource.hPlayback->ulTimestampRegAddr);
		}
#endif

		BVDC_P_Feeder_Destroy(hWindow->stCurResource.hPlayback);
	}

	/* [2] Free capture & capture memory for this window. */
	if(hWindow->pResource->bRequireCapture)
	{
#if (!BVDC_P_USE_RDC_TIMESTAMP)
		if (NULL == hWindow->hCompositor->hVdc->ahSource[eSrcId])
		{
			BDBG_MSG(("FreeScratchReg 0x%x for window[%d] capture %d",
				hWindow->stNewResource.hCapture->ulTimestampRegAddr,
				hWindow->eId, hWindow->pResource->eCapture));
			BRDC_FreeScratchReg(hWindow->hCompositor->hVdc->hRdc,
				hWindow->stNewResource.hCapture->ulTimestampRegAddr);
		}
#endif
		BVDC_P_Buffer_Destroy(hWindow->hBuffer);
		BVDC_P_Capture_Destroy(hWindow->stCurResource.hCapture);
	}

	BDBG_OBJECT_DESTROY(hWindow, BVDC_WIN);
	/* [1] Free context in system memory */
	BKNI_Free((void*)hWindow);

	BDBG_LEAVE(BVDC_P_Window_Destroy);
	return;
}



/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Window_Init
	( BVDC_Window_Handle               hWindow,
	  BVDC_Source_Handle               hSource )
{
	BVDC_P_Window_Info *pNewInfo;
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_Info *pIsrInfo;
	BVDC_P_Window_DirtyBits *pNewDirty;
	BVDC_P_CscCoeffs stIdentity = BVDC_P_MAKE_CMP_CSC
		(1.0000, 0.0000, 0.0000, 0.0000,
		 0.0000, 1.0000, 0.0000, 0.0000,
		 0.0000, 0.0000, 1.0000, 0.0000);
	uint32_t ulCxIntBits;
	uint32_t ulCxFractBits;

	BDBG_ENTER(BVDC_P_Window_Init);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* Init to default state. */
	hWindow->eState               = BVDC_P_State_eInactive;
	hWindow->bUserAppliedChanges  = false;
	hWindow->bSyncLockSrc         = false;
	BVDC_P_Window_SetInvalidVnetMode(&(hWindow->stVnetMode));
	BVDC_P_Window_SetInvalidVnetMode(&(hWindow->stMcvpMode));

	/* Default CSC */
	hWindow->stPriCsc             = stIdentity;
	hWindow->stPriCscA            = stIdentity;
	hWindow->stPriCscB            = stIdentity;
	hWindow->stPriCscC            = stIdentity;
	hWindow->stSecCsc             = stIdentity;
	hWindow->stSecCscB            = stIdentity;
	hWindow->stSecCscC            = stIdentity;
	hWindow->stInvUserCscC        = stIdentity;

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
	/* Mosaic CSC */
	{
		uint16_t i;

		for (i = 0; i < BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
		{
			hWindow->astMosaicCscList[i] = stIdentity;
		}

		for (i = 0; i < BVDC_P_MAX_MOSAIC; i++)
		{
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
			hWindow->aeMosaicClrSpaceList[i] = BAVC_MatrixCoefficients_eSmpte_170M;
			hWindow->aePrevMosaicClrSpaceList[i] = BAVC_MatrixCoefficients_eSmpte_170M;
#else
			hWindow->aeMosaicClrSpaceList[i] = BAVC_MatrixCoefficients_eUnknown;
			hWindow->aePrevMosaicClrSpaceList[i] = BAVC_MatrixCoefficients_eUnknown;
#endif
		}

		BKNI_Memcpy(hWindow->aeClrSpaceToMosaicCscMap, s_aCMP_ClrSpace_To_MosaicCscType_MapTbl, sizeof(hWindow->aeClrSpaceToMosaicCscMap));
		BKNI_Memcpy(hWindow->aeMosaicCscToClrSpaceMap, s_aCMP_MosaicCscType_To_ClrSpace_MapTbl, sizeof(hWindow->aeMosaicCscToClrSpaceMap));
	}
#endif

	/* MAD delay sw pipeline reset */
	hWindow->bResetMadDelaySwPipe = false;

	/* Use to determine vnet modes. */
	hWindow->bDstFullScreen       = false;
	hWindow->bCapture             = false;
	hWindow->eBufferHeapIdRequest = BVDC_P_BufferHeapId_eUnknown;
	hWindow->eBufferHeapIdPrefer  = BVDC_P_BufferHeapId_eUnknown;
	hWindow->ulBufCntNeeded       = 0;
	hWindow->ulBufCntAllocated    = 0;
	hWindow->eBufAllocMode        = BVDC_P_BufHeapAllocMode_eLeftOnly;
	hWindow->ePrevBufAllocMode    = BVDC_P_BufHeapAllocMode_eLeftOnly;
	hWindow->bRightBufAllocated   = false;
	hWindow->uiAvailCaptureBuffers = 0;
	hWindow->ulBufDelay            = 0;
	hWindow->ulPrevBufCntNeeded    = 0;
	hWindow->bBufferCntDecremented = false;
	hWindow->bBufferCntDecrementedForPullDown = false;
	hWindow->bBufferCntIncremented = false;

	/* adjusted rectangles */
	hWindow->bAdjRectsDirty        = true;
	hWindow->eMadPixelHeapId       = BVDC_P_BufferHeapId_eUnknown;
	hWindow->eMadQmHeapId          = BVDC_P_BufferHeapId_eUnknown;
	hWindow->stPrevSrcCnt.lLeft    = -1; /* force to calc */
	hWindow->stPrevSrcCnt.lTop     = -1;
	hWindow->stPrevSrcCnt.lLeft_R  = 0;
	hWindow->stPrevSrcCnt.ulWidth  = 0;
	hWindow->stPrevSrcCnt.ulHeight = 0;
	hWindow->ePrevMatrixCoeffs     = BAVC_MatrixCoefficients_eUnknown;
	hWindow->bMuteBypass           = false;

	/* MosaicMode mosaic rect set */
	hWindow->ulMosaicRectSet       = ~0; /* to update rect setting */
	hWindow->bClearRectSupport     = hWindow->stSettings.ulMaxMosaicRect
		? true : false;

	hWindow->ulFgtSeiBufAddr       = 0;

	/* Vdec phase low pass filter values */
	hWindow->lRawVdecPhase         = 0;
	hWindow->lLpfLastResult        = 0;
	hWindow->lLpfLastVdecPhase     = 0;
	BKNI_Memset((void *)hWindow->alLpfSamples, 0, sizeof(hWindow->alLpfSamples));

	/* Reset done events */
	BKNI_ResetEvent(hWindow->hDestroyDoneEvent);
	BKNI_ResetEvent(hWindow->hAppliedDoneEvent);
	hWindow->bSetDestroyEventPending = false;
	hWindow->bSetAppliedEventPending = false;

	/* This flag indicate if the stand alone HIST block is available */
#if (BVDC_P_SUPPORT_HIST_VER >= BVDC_P_SUPPORT_HIST_VER_2)
	hWindow->bHistAvail = 1;
#else
	hWindow->bHistAvail = 0;
#endif

	/* This flag indicates if Chroma Histogram is available */
#if (BVDC_P_SUPPORT_CM3D)
	hWindow->bChromaHistAvail = (hWindow->eId == BVDC_P_WindowId_eComp0_V0) ? true : false;
#endif

	/* Initialize cadence handling related fields */
	hWindow->stCadHndl.bForceAltCap = false;
	hWindow->stCadHndl.eLastCapPolarity = BAVC_Polarity_eInvalid;
	hWindow->stCadHndl.bReaderCadMatching = true;
	hWindow->stCadHndl.bDecoderRateCov = false;
	hWindow->stCadHndl.bTrickMode = false;

	hWindow->ulDropCntNonIgnoredPics = 0;
#if BVDC_P_SUPPORT_STG
	hWindow->stMadDelayed[0].bIgnorePicture = true;
	hWindow->stMadDelayed[1].bIgnorePicture = true;
	hWindow->stMadDelayed[2].bIgnorePicture = true;
	hWindow->stMadDelayed[3].bIgnorePicture = true;
	hWindow->stMadDelayed[0].bChannelChange = true;
	hWindow->stMadDelayed[1].bChannelChange = true;
	hWindow->stMadDelayed[2].bChannelChange = true;
	hWindow->stMadDelayed[3].bChannelChange = true;
#endif

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
#if BVDC_P_SUPPORT_MOSAIC_MODE
	{
		uint32_t i;

		for (i = 0; i < BVDC_P_MAX_MOSAIC; i++)
			hWindow->aulMosaicZOrderIndex[i] = i;
	}
#endif
#endif

	/* Initial new/current public states */
	pNewInfo = &hWindow->stNewInfo;
	pCurInfo = &hWindow->stCurInfo;
	pIsrInfo = &hWindow->stIsrInfo;

	/* Clear out user's states. */
	BKNI_Memset((void*)pNewInfo, 0x0, sizeof(BVDC_P_Window_Info));
	BKNI_Memset((void*)pCurInfo, 0x0, sizeof(BVDC_P_Window_Info));
	BKNI_Memset((void*)pIsrInfo, 0x0, sizeof(BVDC_P_Window_Info));

	/* Init dirty bits so there will executed at least once. */
	pNewDirty = &pNewInfo->stDirty;
	BDBG_ASSERT(sizeof(pNewDirty->stBits) <= sizeof(pNewDirty->aulInts));
	pNewDirty->stBits.bCscAdjust      = BVDC_P_DIRTY;
	pNewDirty->stBits.bLumaKeyAdjust  = BVDC_P_DIRTY;

	/* Disconnecting/Connecting Source */
	hWindow->stNewInfo.hSource     = hSource;

	/* Misc flags. */
	pNewInfo->bUseSrcFrameRate     = false;
	pNewInfo->bDeinterlace         = false;
	pNewInfo->bVisible             = false;
	pNewInfo->ucZOrder             = 0;
	pNewInfo->ucAlpha              = BVDC_ALPHA_MAX;
	pNewInfo->ucConstantAlpha      = BVDC_ALPHA_MAX;
	pNewInfo->eFrontBlendFactor    = BVDC_BlendFactor_eSrcAlpha;
	pNewInfo->eBackBlendFactor     = BVDC_BlendFactor_eOneMinusSrcAlpha;
	pNewInfo->ePhaseAdjustMode     = BVDC_PhaseAdjustMode_eDisable;
	pNewInfo->bCscRgbMatching      = false;
	pNewInfo->uiVsyncDelayOffset   = 0;
	pNewInfo->eReaderState         = BVDC_P_State_eInactive;
	pNewInfo->eWriterState         = BVDC_P_State_eInactive;

	/* Where on the canvas */
	pNewInfo->stDstRect.lLeft    = 0;
	pNewInfo->stDstRect.lLeft_R  = 0;
	pNewInfo->stDstRect.lTop     = 0;
	pNewInfo->stDstRect.ulWidth  = hWindow->hCompositor->stCurInfo.pFmtInfo->ulWidth;
	pNewInfo->stDstRect.ulHeight = hWindow->hCompositor->stCurInfo.pFmtInfo->ulHeight;

	pNewInfo->lRWinXOffsetDelta  = 0;

	/* Where on the scaler's output. */
	pNewInfo->stScalerOutput     = pNewInfo->stDstRect;

	/* AFD */
	pNewInfo->stAfdSettings.eMode = BVDC_AfdMode_eDisabled;
	pNewInfo->stAfdSettings.eClip = BVDC_AfdClip_eNominal;
	pNewInfo->stAfdSettings.ulAfd = 0;

	/* Pan Scan */
	pNewInfo->ePanScanType     = BVDC_PanScanType_eDisable;
	pNewInfo->lUserHorzPanScan = 0;
	pNewInfo->lUserVertPanScan = 0;

	/* Aspect Ratio Correct */
	pNewInfo->eAspectRatioMode       = BVDC_AspectRatioMode_eBypass;
	pNewInfo->ulNonlinearSrcWidth    = 0;
	pNewInfo->ulNonlinearSclOutWidth = 0;

	/* scale factor rounding */
	pNewInfo->ulHrzSclFctRndToler = 0;
	pNewInfo->ulVrtSclFctRndToler = 0;

	/* Scaler configuration */
	pNewInfo->stSclSettings.bSclVertDejagging       = true;
	pNewInfo->stSclSettings.bSclHorzLumaDeringing   = true;
	pNewInfo->stSclSettings.bSclVertLumaDeringing   = true;
	pNewInfo->stSclSettings.bSclHorzChromaDeringing = true;
	pNewInfo->stSclSettings.bSclVertChromaDeringing = true;
	pNewInfo->stSclSettings.bSclVertPhaseIgnore     = false;
	pNewInfo->stSclSettings.ulSclDejaggingHorz      = 4;
	pNewInfo->stSclSettings.ulSclDejaggingGain      = 2;
	pNewInfo->stSclSettings.ulSclDejaggingCore      = 0;

	/* Sharpness */
	pNewInfo->bSharpnessEnable       = false;
	pNewInfo->bUserSharpnessConfig   = false;

#if (BVDC_P_SUPPORT_TNT_VER < 5)         /* TNT HW base */
	pNewInfo->ulLumaGain             = 0;
	pNewInfo->ulSharpnessPeakSetting = 0;
	pNewInfo->ulSharpnessPeakScale   = 0;

	/* user sharpness config */
	BVDC_P_Window_Sharpness_Init(hWindow, &pNewInfo->stSharpnessConfig);
#endif

	/* Contrast stretch parameters */
	pNewInfo->bContrastStretch                         = false;
	pNewInfo->stContrastStretch.ulShift                = BVDC_P_PEP_FIX_FRACTIONAL_SHIFT;
	pNewInfo->stContrastStretch.iGain                  = BVDC_P_PEP_ITOFIX(1);
	pNewInfo->stContrastStretch.ulPwmMaxApl              = 130;
	pNewInfo->stContrastStretch.ulPwmMinApl              = 70;
	pNewInfo->stContrastStretch.ulPwmMinPercent          = 40;
	pNewInfo->stContrastStretch.ulDcLoThresh             = 1;
	pNewInfo->stContrastStretch.ulDcHiThresh             = 3;
	pNewInfo->stContrastStretch.ulHiThreshBlendMin       = 250;
	pNewInfo->stContrastStretch.ulHiThreshBlendRng       = 130;
	pNewInfo->stContrastStretch.ulLoThreshBlendMin       = 90;
	pNewInfo->stContrastStretch.ulLoThreshBlendRng       = 40;
	pNewInfo->stContrastStretch.ulHiThreshRatio          = 500;
	pNewInfo->stContrastStretch.ulLoThreshRatio          = 500;
	pNewInfo->stContrastStretch.bInterpolateTables       = true;
	pNewInfo->stContrastStretch.bBypassSat               = false;

	pNewInfo->stContrastStretch.pvCustomParams           = NULL;
	pNewInfo->stContrastStretch.pfCallback               = NULL;
	pNewInfo->stContrastStretch.pvParm1                  = NULL;
	pNewInfo->stContrastStretch.iParm2                   = 0;
	pNewInfo->bUserLabLuma = false;
	pNewInfo->bUserLabCbCr = false;

	/* blue stretch */
	pNewInfo->bBlueStretch = false;
	pNewInfo->stBlueStretch.ulBlueStretchOffset = 300;
	pNewInfo->stBlueStretch.ulBlueStretchSlope  = 4;

	/* Cab parameter */
	pNewInfo->ulFleshtone           = 0;
	pNewInfo->ulGreenBoost          = 0;
	pNewInfo->ulBlueBoost           = 0;
	pNewInfo->bUserCabEnable        = false;

	/* Mosaic mode */
	pNewInfo->bClearRect            = false;
	pNewInfo->bMosaicMode           = false;
	pNewInfo->bClearRectByMaskColor = true;
	pNewInfo->ulClearRectAlpha      = 255;
	BPXL_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8,
		BPXL_MAKE_PIXEL(BPXL_eA8_R8_G8_B8, 0x00, 255, 255, 255),
		(unsigned int*)&pNewInfo->ulMaskColorYCrCb);

	/* CMS paramters */
	BVDC_P_PEP_CMS_DISABLE(&pNewInfo->stSatGain);
	BVDC_P_PEP_CMS_DISABLE(&pNewInfo->stHueGain);

	/* Demo mode */
	pNewInfo->stSplitScreenSetting.eHue             = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eContrast        = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eAutoFlesh       = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eBrightness      = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eColorTemp       = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eSharpness       = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eBlueBoost       = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eGreenBoost      = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eCms             = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eContrastStretch = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eBlueStretch     = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eFgt             = BVDC_SplitScreenMode_eDisable;
	pNewInfo->stSplitScreenSetting.eCm3d            = BVDC_SplitScreenMode_eDisable;

	/* box detect */
	pNewInfo->bBoxDetect = false;
	pNewInfo->bAutoCutBlack = false;
	pNewInfo->BoxDetectCallBack = NULL;
	pNewInfo->pvBoxDetectParm1 = NULL;
	pNewInfo->iBoxDetectParm2 = 0;

	/* Init mad default settings */
	BVDC_P_Mad_Init_Default(
		&pNewInfo->stMadSettings.eGameMode,
		&pNewInfo->stMadSettings.ePixelFmt,
		&pNewInfo->stMadSettings.ePqEnhancement,
		&pNewInfo->stMadSettings.stTestFeature1,
		&pNewInfo->stMadSettings.bShrinkWidth,
		&pNewInfo->stMadSettings.bReverse32Pulldown,
		&pNewInfo->stMadSettings.stReverse32Settings,
		&pNewInfo->stMadSettings.bReverse22Pulldown,
		&pNewInfo->stMadSettings.stReverse22Settings,
		&pNewInfo->stMadSettings.stChromaSettings,
		&pNewInfo->stMadSettings.stMotionSettings);

	/* Init mad custom settings */
	BVDC_P_Mad_Init_Custom(
		&pNewInfo->stMadSettings.stUpSampler,
		&pNewInfo->stMadSettings.stDnSampler,
		&pNewInfo->stMadSettings.stLowAngles);

	pNewInfo->bChromaCustom = false;
	pNewInfo->bMotionCustom = false;
	pNewInfo->bRev32Custom  = false;
	pNewInfo->bRev22Custom  = false;

	/* init ANR default settings */
	/* only vwin0 has ANR as default; */
	pNewInfo->bAnr                  = false;
	pNewInfo->stAnrSettings.eMode   = BVDC_FilterMode_eDisable;
	pNewInfo->stAnrSettings.ePxlFormat = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
	pNewInfo->stAnrSettings.stVideoTestFeature1.bEnable = 0;
	pNewInfo->stAnrSettings.stVideoTestFeature1.ulBitsPerPixel = 0;

	ulCxIntBits   = BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId)? BVDC_P_CSC_GFD_CX_I_BITS : BVDC_P_CSC_CMP_CX_I_BITS;
	ulCxFractBits = BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId)? BVDC_P_CSC_GFD_CX_F_BITS : BVDC_P_CSC_CMP_CX_F_BITS;

	/* Color adjustment attributes */
	pNewInfo->sContrast             = 0;
	pNewInfo->sSaturation           = 0;
	pNewInfo->sHue                  = 0;
	pNewInfo->sBrightness           = 0;
	pNewInfo->sSharpness            = 0;
	pNewInfo->sColorTemp            = 0;
	pNewInfo->lAttenuationR         = BMTH_FIX_SIGNED_ITOFIX(1, ulCxIntBits, ulCxFractBits);
	pNewInfo->lAttenuationG         = BMTH_FIX_SIGNED_ITOFIX(1, ulCxIntBits, ulCxFractBits);
	pNewInfo->lAttenuationB         = BMTH_FIX_SIGNED_ITOFIX(1, ulCxIntBits, ulCxFractBits);
	pNewInfo->lOffsetR              = 0;
	pNewInfo->lOffsetG              = 0;
	pNewInfo->lOffsetB              = 0;

	/* Luma key attributes */
	pNewInfo->bLumaKey              = false;
	pNewInfo->ucLumaKeyMask         = 0;
	pNewInfo->ucLumaKeyHigh         = 0xff;
	pNewInfo->ucLumaKeyLow          = 0;

	/* User capture buffers */
	pNewInfo->uiCaptureBufCnt       = 0;
	pNewInfo->pBufferFromUser       = NULL;

	/* Set default pixel format of buffer */
#if (BSTD_CPU_ENDIAN == BSTD_ENDIAN_LITTLE)
	pNewInfo->ePixelFormat          = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
#else
	pNewInfo->ePixelFormat          = BVDC_P_CAP_PIXEL_FORMAT_8BIT422_BE;
#endif

	/* Luma avg rect set */
	pNewInfo->stLumaRect.stRegion.ulLeft   = 0;
	pNewInfo->stLumaRect.stRegion.ulTop    = 0;
	pNewInfo->stLumaRect.stRegion.ulRight  = 0;
	pNewInfo->stLumaRect.stRegion.ulBottom = 0;
	pNewInfo->stLumaRect.aulLevelThres[0]  =  100;
	pNewInfo->stLumaRect.aulLevelThres[1]  =  500;
	pNewInfo->stLumaRect.aulLevelThres[2]  = 9000;
	pNewInfo->stLumaRect.aulLevelThres[3]  = 9500;
	pNewInfo->stLumaRect.eNumBins = (hWindow->bHistAvail) ?
		BVDC_HistBinSelect_e64_Bins: BVDC_HistBinSelect_e16_Bins;
	pNewInfo->bLumaRectUserSet         = false;
	pNewInfo->bHistEnable              = false;
	pNewInfo->bHistAtSrc               = true;

#if BVDC_P_SUPPORT_CM3D
	/* Chroma rect set */
	pNewInfo->stChromaRect.stRegion.ulLeft   = 0;
	pNewInfo->stChromaRect.stRegion.ulTop    = 0;
	pNewInfo->stChromaRect.stRegion.ulRight  = 0;
	pNewInfo->stChromaRect.stRegion.ulBottom = 0;
	pNewInfo->stChromaRect.eType = BVDC_ChromaHistType_eInvalid;
	pNewInfo->stChromaRect.ulHueMax = 0x3ff;
	pNewInfo->stChromaRect.ulHueMin = 0;
	pNewInfo->stChromaRect.ulSatMin = 0;
#endif

	/* FGT */
	pNewInfo->bFgt = false;
	pNewInfo->stFgtSettings.bEnable    = false;

	/* Pulldown capture */
	hWindow->bDoPulldown     = false;
	hWindow->bFrameCapture   = false;

	/* Callback */
	hWindow->pCurWriterNode  = NULL;
	hWindow->pCurReaderNode  = NULL;
	hWindow->bRepeatCurrReader = false;
	hWindow->stCallbackData.ulVsyncDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
	hWindow->stCallbackData.ulDriftDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
	hWindow->stCallbackData.ulGameModeDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
	hWindow->stCallbackData.stOutputRect.lLeft    = pNewInfo->stDstRect.lLeft;
	hWindow->stCallbackData.stOutputRect.lTop     = pNewInfo->stDstRect.lTop;
	hWindow->stCallbackData.stOutputRect.ulWidth  = pNewInfo->stDstRect.ulWidth;
	hWindow->stCallbackData.stOutputRect.ulHeight = pNewInfo->stDstRect.ulHeight;
	hWindow->stCallbackData.stMask.bDriftDelay = BVDC_P_DIRTY;
	hWindow->stCallbackData.stMask.bVsyncDelay = BVDC_P_DIRTY;
	hWindow->stCallbackData.stMask.bRectAdjust = BVDC_P_DIRTY;
	hWindow->stCallbackData.stMask.bSyncLock   = BVDC_P_DIRTY;
	hWindow->stCallbackData.stMask.bGameModeDelay = BVDC_P_DIRTY;
	hWindow->stCallbackSettings.stMask.bDriftDelay = BVDC_P_DIRTY;
	hWindow->stCallbackSettings.stMask.bVsyncDelay = BVDC_P_DIRTY;
	hWindow->stCallbackSettings.stMask.bRectAdjust = BVDC_P_DIRTY;
	hWindow->stCallbackSettings.stMask.bSyncLock   = BVDC_P_DIRTY;
	hWindow->stCallbackSettings.stMask.bGameModeDelay = BVDC_P_CLEAN;
	hWindow->stCallbackSettings.ulLipSyncTolerance = BVDC_P_LIP_SYNC_TOLERANCE;
	hWindow->stCallbackSettings.ulGameModeReadWritePhaseDiff = BVDC_P_LIP_SYNC_TOLERANCE;
	hWindow->stCallbackSettings.ulGameModeTolerance = BVDC_P_LIP_SYNC_RESET_DELAY;

	/* Game mode delay */
	pNewInfo->stGameDelaySetting.bEnable           = false;
	pNewInfo->stGameDelaySetting.bForceCoarseTrack = false;
	pNewInfo->stGameDelaySetting.ulBufferDelayTarget = BVDC_P_LIP_SYNC_RESET_DELAY;
	pNewInfo->stGameDelaySetting.ulBufferDelayTolerance = BVDC_P_LIP_SYNC_RESET_DELAY;
	hWindow->ulCurBufDelay   = 0;
	hWindow->lCurGameModeLag = 0;
	hWindow->bAdjGameModeClock = false;
	hWindow->bCoarseAdjClock   = false;
	hWindow->bFastAdjClock     = false;

	/* PLATFORMS: Specifics override that either take adavantage of bvn doc
	 * assumptions or otherwise default to support ref rts.  Otherwise can
	 * be overwritten by public API. */
#if (BCHP_CHIP==7400)
	/* (BCHP_CHIP==7038/7401/7403/etc) are legacy chip */
	/* (1st generation) the beginning of day with symetric bandwidth equation */
	pNewInfo->bForceCapture    = false;
	hWindow->bSclCapSymmetric  = true;
	pNewInfo->eSclCapBias      = BVDC_SclCapBias_eAuto;
	pNewInfo->ulBandwidthDelta = BVDC_P_BW_DEFAULT_DELTA;

#elif (BCHP_CHIP==7405) || (BCHP_CHIP==7325) || (BCHP_CHIP==7335) || \
	  (BCHP_CHIP==7336)
	/* (2nd generation) trying to simplify by making force_capture,
	 * scaler_catpure but with symetric bandwidth equation */
	pNewInfo->bForceCapture    = true;
	hWindow->bSclCapSymmetric  = true;
	pNewInfo->eSclCapBias      = BVDC_SclCapBias_eSclBeforeCap;
	pNewInfo->ulBandwidthDelta = 10000000;

#else /* (BCHP_CHIP==7420) || (BCHP_CHIP==7340) || (BCHP_CHIP==7342) || \
	     (BCHP_CHIP==7550) || (BCHP_CHIP==7408) || (BCHP_CHIP==NEWCHIP) */
	/* (3rd generation) new chipset with improved bandwidth equation,
	 * and default for newer chip */
	pNewInfo->bForceCapture    = true;
	hWindow->bSclCapSymmetric  = false;
	pNewInfo->eSclCapBias      = BVDC_SclCapBias_eAuto;
	pNewInfo->ulBandwidthDelta = BVDC_P_BW_DEFAULT_DELTA;

#endif

	if(hWindow->pResource->bRequireScaler)
	{
		BVDC_P_Scaler_Init(hWindow->stCurResource.hScaler, hWindow);
	}

	if(hWindow->pResource->bRequirePlayback)
	{
		BVDC_P_Feeder_Init(hWindow->stCurResource.hPlayback, hWindow->hHeap->hMem, false);
	}

	if(hWindow->pResource->bRequireCapture)
	{
		BVDC_P_Buffer_Init(hWindow->hBuffer);
		BVDC_P_Capture_Init(hWindow->stCurResource.hCapture);
		hWindow->pCurWriterNode = BVDC_P_Buffer_GetCurWriterNode(hWindow->hBuffer);
		hWindow->pCurReaderNode = BVDC_P_Buffer_GetCurReaderNode(hWindow->hBuffer);
	}

	if(hWindow->pResource->bRequirePep)
	{
		BVDC_P_Pep_Init(hWindow->stCurResource.hPep);
	}

#if BVDC_P_SUPPORT_CM3D
	if(hWindow->eId == BVDC_P_WindowId_eComp0_V0)
	{
		BVDC_P_Cm3d_Init(hWindow->stCurResource.hCm3d);
	}
#endif

	/* Clear out user's states. */
	BKNI_Memcpy(pCurInfo, pNewInfo, sizeof(BVDC_P_Window_Info));

	return;
}

/***************************************************************************
 * {private}
 * user sharpness config
 */
void BVDC_P_Window_Sharpness_Init
	( BVDC_Window_Handle               hWindow,
	  BVDC_SharpnessSettings          *pSharpnessConfig )
{
	BSTD_UNUSED(hWindow);
#if (BVDC_P_SUPPORT_TNT_VER < 5)
	BDBG_ASSERT(pSharpnessConfig);

	pSharpnessConfig->ulLumaCtrlCore         = 0x8;
	pSharpnessConfig->ulLumaCtrlGain         = 0;
	pSharpnessConfig->ulLumaCtrlBlur         = 0;
	pSharpnessConfig->bLumaCtrlSoften        = false;
	pSharpnessConfig->bLumaCtrlHOnly         = false;
	pSharpnessConfig->ulLumaPeakingHAvoid    = 0x10;
	pSharpnessConfig->ulLumaPeakingVAvoid    = 0x10;
	pSharpnessConfig->ulLumaPeakingPeakLimit = 0x7f;
	pSharpnessConfig->ulLumaPeakingPeakValue = 0;
	pSharpnessConfig->ulChromaCtrlCore       = 0x10;
	pSharpnessConfig->bChromaCtrlWideChroma  = true;
	pSharpnessConfig->ulChromaCtrlFalseColor = 0;
	pSharpnessConfig->ulChromaCtrlGain       = 0x3f;
	pSharpnessConfig->bChromaCtrlHOnly       = true;
	pSharpnessConfig->ulWideLumaCtrlCore     = 0x8;
	pSharpnessConfig->ulWideLumaCtrlMode     = 2;
	pSharpnessConfig->ulSimpleLumaCtrlCore   = 2;
	pSharpnessConfig->bSimpleLumaCtrlMode    = true;
#else
	BSTD_UNUSED(pSharpnessConfig);
#endif

	return;
}

/***************************************************************************
 * {private}
 *
 * This should contains all the information to detect user error settings.
 * User settings that required checking happen here.
 */
BERR_Code BVDC_P_Window_ValidateChanges
	( const BVDC_Window_Handle         hWindow,
	  const BFMT_VideoInfo            *pDstFormatInfo )
{
	BVDC_P_Resource_Handle hResource;
	BVDC_P_Window_Info *pNewInfo;
	const BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pNewDirty;
	const BFMT_VideoInfo *pSrcFmtInfo;
	BVDC_ClipRect *pNewRect;
	const BVDC_ClipRect *pCurRect;
	uint32_t ulMinV;
	BVDC_DisplayTg eMasterTg;
	bool bDtg;
	uint32_t ulHsize, ulVsize;

	BDBG_ENTER(BVDC_P_Window_ValidateChanges);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stNewInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor->hDisplay, BVDC_DSP);
	hResource = hWindow->hCompositor->hVdc->hResource;
	eMasterTg = hWindow->hCompositor->hDisplay->eMasterTg;
	bDtg      = BVDC_P_DISPLAY_USED_DIGTRIG (eMasterTg);
	ulHsize = bDtg? pDstFormatInfo->ulDigitalWidth : pDstFormatInfo->ulWidth;
	ulVsize = bDtg? pDstFormatInfo->ulDigitalHeight: pDstFormatInfo->ulHeight;

	/* Handle 3d case */
	if(!BFMT_IS_3D_MODE(hWindow->hCompositor->stNewInfo.pFmtInfo->eVideoFmt))
	{
		if(hWindow->hCompositor->stNewInfo.eOrientation == BFMT_Orientation_e3D_LeftRight)
		{
			ulHsize = ulHsize / 2;
		}
		else if(hWindow->hCompositor->stNewInfo.eOrientation == BFMT_Orientation_e3D_OverUnder)
		{
			ulVsize = ulVsize / 2;
		}
	}

	/* To reduce the amount of typing */
	pNewInfo = &hWindow->stNewInfo;
	pCurInfo = &hWindow->stCurInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* if display is digital master, user setting of rectangles of full-screen should
	 * match digital size */
	if(bDtg)
	{
		if(pNewInfo->stDstRect.ulHeight == pDstFormatInfo->ulHeight)
		{
			pNewInfo->stDstRect.ulHeight = pDstFormatInfo->ulDigitalHeight;
		}
		if(pNewInfo->stScalerOutput.ulHeight == pDstFormatInfo->ulHeight)
		{
			pNewInfo->stScalerOutput.ulHeight = pDstFormatInfo->ulDigitalHeight;
		}
	}

	/* Validate new information. */
	pSrcFmtInfo =
		((hWindow->stNewInfo.hSource->stNewInfo.bAutoFmtDetection) &&
		 (hWindow->stNewInfo.hSource->stCurInfo.bAutoFmtDetection))
		? hWindow->stNewInfo.hSource->stCurInfo.pFmtInfo : hWindow->stNewInfo.hSource->stNewInfo.pFmtInfo;

	/* Hist is either enabled by user or dynamic contrast */
	pNewInfo->bHistEnable = pNewInfo->bLumaRectUserSet | pNewInfo->bContrastStretch;

	/* (0) Destination rect should be bigger than min */
	ulMinV = pDstFormatInfo->bInterlaced ? BVDC_P_WIN_DST_OUTPUT_V_MIN * 2 : BVDC_P_WIN_DST_OUTPUT_V_MIN;
	if(pNewInfo->stDstRect.ulHeight < ulMinV)
	{
		BDBG_ERR(("For displaying %s, min dst rect is %dx%d", (pDstFormatInfo->bInterlaced) ?  "interlace" : "progressive", BVDC_P_WIN_DST_OUTPUT_H_MIN, ulMinV));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* (1) Destination rect is bigger than canvas. */
	if((pNewInfo->stDstRect.ulWidth  > ulHsize) ||
	   (pNewInfo->stDstRect.ulHeight > ulVsize))
	{
		return BERR_TRACE(BVDC_ERR_DST_SIZE_LARGER_THAN_CANVAS);
	}

	/* (2) DstRect can not be larger than scaler output. */
	if((pNewInfo->stDstRect.ulWidth  + pNewInfo->stScalerOutput.lLeft > pNewInfo->stScalerOutput.ulWidth) ||
	   (pNewInfo->stDstRect.ulHeight + pNewInfo->stScalerOutput.lTop  > pNewInfo->stScalerOutput.ulHeight))
	{
		return BERR_TRACE(BVDC_ERR_DST_SIZE_LARGER_THAN_SCL_OUTPUT);
	}

	/* (2.5) DstRect can not be out of bound of canvas. */
	if((pNewInfo->stDstRect.ulWidth  + pNewInfo->stDstRect.lLeft > ulHsize)  ||
	   (pNewInfo->stDstRect.ulHeight + pNewInfo->stDstRect.lTop  > ulVsize) ||
	   (pNewInfo->stDstRect.lLeft < 0) || (pNewInfo->stDstRect.lTop < 0))
	{
		BDBG_ERR(("DstRect has to be inside display boundary for now!"));
		return BERR_TRACE(BVDC_ERR_DST_RECT_OUT_OF_BOUND);
	}

#if BVDC_P_SUPPORT_3D_VIDEO
	/* (2.6) DstRect cannot be out of bound of canvas, right window for 3D Left right*/
	if((pNewInfo->stDstRect.ulWidth  + pNewInfo->lRWinXOffsetDelta + pNewInfo->stDstRect.lLeft > ulHsize) ||
		(pNewInfo->lRWinXOffsetDelta + pNewInfo->stDstRect.lLeft < 0))
	{
		BDBG_ERR(("Rigth window DstRect has to be inside canvas boundary!"));
		return BERR_TRACE(BVDC_ERR_DST_RECT_OUT_OF_BOUND);
	}
#endif

	if(BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId))
	{
		/* If MPG window's display is not aligned to or by another display, forced sync
		   lock might cause tearing! */
		if(hWindow->stSettings.bForceSyncLock)
		{
			if(!BVDC_P_SRC_IS_MPEG(hWindow->stNewInfo.hSource->eId))
			{
				BDBG_ERR(("Forced synclock is not supported for non-MPG window yet!"));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			if(!(hWindow->hCompositor->hDisplay->stNewInfo.hTargetDisplay || hWindow->hCompositor->hDisplay->ulAlignSlaves))
			{
				BDBG_ERR(("MPEG window%d can only be forced sync-lock when VEC aligned!", hWindow->eId));
				if(hWindow->hCompositor->hDisplay->stNewInfo.hTargetDisplay)
				{
					BDBG_ERR(("\tDisplay%d's alignment target is display%d.",
						hWindow->hCompositor->hDisplay->eId, hWindow->hCompositor->hDisplay->stNewInfo.hTargetDisplay->eId));
				}
				else
				{
					BDBG_ERR(("\tDisplay%d is aligned by %d display(s).", hWindow->hCompositor->hDisplay->eId, hWindow->hCompositor->hDisplay->ulAlignSlaves));
				}
			}
		}

		/* (3) Source clipped rect sould be inside source rect.  Source can
		 * change overclipped will consider no clip. */

		/* (3.2) Left eye width and right eye width needs to be same, so  */
		if(BVDC_P_ABS(pNewInfo->stSrcClip.lLeftDelta_R) > pNewInfo->stSrcClip.ulRight)
		{
			BDBG_ERR(("Win[%d] Invalid Clipping values for right eye", hWindow->eId));
			BDBG_ERR(("Left eye: (%d, %d), right eye delta: %d",
				pNewInfo->stSrcClip.ulLeft, pNewInfo->stSrcClip.ulRight,
				pNewInfo->stSrcClip.lLeftDelta_R));
			return BERR_TRACE(BVDC_ERR_ILLEGAL_CLIPPING_VALUES);
		}

		/* VEC/VDEC phase adjustment. */
		if(pNewInfo->ePhaseAdjustMode != BVDC_PhaseAdjustMode_eDisable)
		{
			if((pNewInfo->stScalerOutput.ulWidth  < ulHsize) ||
			   (pNewInfo->stScalerOutput.ulHeight < ulVsize) ||
			   (pNewInfo->stDstRect.ulWidth       < ulHsize) ||
			   (pNewInfo->stDstRect.ulHeight      < ulVsize))
			{
				return BERR_TRACE(BVDC_ERR_PHASE_ADJUST_ON_DOWNSCALED_WINDOWS);
			}

			if(!BVDC_P_SRC_IS_ANALOG(hWindow->stNewInfo.hSource->eId))
			{
				return BERR_TRACE(BVDC_ERR_PHASE_ADJUST_ON_ILLEGAL_SOURCE);
			}
		}

		/* (4) This video window can not accept gfx source. */
		if(BVDC_P_SRC_IS_GFX(hWindow->stNewInfo.hSource->eId))
		{
			return BERR_TRACE(BVDC_ERR_SOURCE_WINDOW_MISMATCH);
		}

		/* (5) Non-linear horizontal scaling doesn't coexist with ARC/SFR
		 *     and PIP window */
		if((((0 != pNewInfo->ulNonlinearSrcWidth) || (0 != pNewInfo->ulNonlinearSclOutWidth)) &&
			((BVDC_AspectRatioMode_eUseAllSource      == pNewInfo->eAspectRatioMode) ||
			 (BVDC_AspectRatioMode_eUseAllDestination == pNewInfo->eAspectRatioMode) ||
			 (pNewInfo->stScalerOutput.ulWidth  < (ulHsize  * 3 / 4)))) ||
		   (pNewInfo->stScalerOutput.ulWidth < 2 * pNewInfo->ulNonlinearSclOutWidth))
		{
			return BERR_TRACE(BVDC_ERR_INVALID_NONLINEAR_SCALE);
		}

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* (6) Mosaic window size checking; */
		if(pNewInfo->bClearRect)
		{
			uint32_t i;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
			bool     bSortZorder = false;
#endif

			if(pNewInfo->ulMosaicCount == 0)
			{
				return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
			}

			/* clear outside mode can not coexist with dest cut */
			if((pNewInfo->stScalerOutput.ulWidth  != pNewInfo->stDstRect.ulWidth)  ||
			   (pNewInfo->stScalerOutput.ulHeight != pNewInfo->stDstRect.ulHeight) ||
			   (pNewInfo->stScalerOutput.lLeft    != 0) ||
			   (pNewInfo->stScalerOutput.lLeft_R  != 0) ||
			   (pNewInfo->stScalerOutput.lTop     != 0))
			{
				BDBG_ERR(("Clear outside mosaic mode can not co-exist with dest cut!"));
				BDBG_ASSERT(false ==
					((pNewInfo->stScalerOutput.ulWidth  != pNewInfo->stDstRect.ulWidth)  ||
					 (pNewInfo->stScalerOutput.ulHeight != pNewInfo->stDstRect.ulHeight) ||
					 (pNewInfo->stScalerOutput.lLeft    != 0) ||
					 (pNewInfo->stScalerOutput.lTop     != 0)));
				return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
			}

			for(i = 0; i < pNewInfo->ulMosaicCount; i++)
			{
				/* the mosaics are bounded within ScalerOutput rect */
				if((0 == pNewInfo->astMosaicRect[i].ulWidth)  ||
				   (0 == pNewInfo->astMosaicRect[i].ulHeight) ||
				   (0 > pNewInfo->astMosaicRect[i].lLeft) ||
				   (0 > pNewInfo->astMosaicRect[i].lLeft_R) ||
				   (0 > pNewInfo->astMosaicRect[i].lTop) ||
				   (pNewInfo->astMosaicRect[i].lLeft + pNewInfo->astMosaicRect[i].ulWidth
					> pNewInfo->stScalerOutput.ulWidth)       ||
				   (pNewInfo->astMosaicRect[i].lTop + pNewInfo->astMosaicRect[i].ulHeight
					> pNewInfo->stScalerOutput.ulHeight))
				{
					BDBG_ERR(("Mosaic[%d]: %dx%dx%dx%d - %d", i,
						pNewInfo->astMosaicRect[i].lLeft, pNewInfo->astMosaicRect[i].lTop,
						pNewInfo->astMosaicRect[i].ulWidth, pNewInfo->astMosaicRect[i].ulHeight,
						pNewInfo->astMosaicRect[i].lLeft_R));
					BDBG_ERR(("stScalerOutput: %d %d", pNewInfo->stScalerOutput.ulWidth,
						pNewInfo->stScalerOutput.ulHeight));
					return BERR_TRACE(BVDC_ERR_INVALID_MOSAIC_MODE);
				}

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
				if(pNewInfo->aucMosaicZOrder[i] != pCurInfo->aucMosaicZOrder[i])
				{
					bSortZorder = true;
				}
#endif

				if(!BVDC_P_RECT_CMP_EQ(&pNewInfo->astMosaicRect[i], &pCurInfo->astMosaicRect[i]) ||
				   (pNewInfo->aucMosaicZOrder[i]    != pCurInfo->aucMosaicZOrder[i]) ||
				   (pNewInfo->abMosaicVisible[i]    != pCurInfo->abMosaicVisible[i]))
				{
					pNewDirty->stBits.bMosaicMode = BVDC_P_DIRTY;
				}
			}
			if((pNewInfo->bClearRectByMaskColor != pCurInfo->bClearRectByMaskColor) ||
			   (pNewInfo->bMosaicMode      != pCurInfo->bMosaicMode) ||
			   (pNewInfo->ulClearRectAlpha != pCurInfo->ulClearRectAlpha) ||
			   (pNewInfo->ulMosaicCount != pCurInfo->ulMosaicCount) ||
			   (pNewInfo->ulMaskColorYCrCb != pCurInfo->ulMaskColorYCrCb))
			{
				pNewDirty->stBits.bMosaicMode = BVDC_P_DIRTY;
			}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)

			if(bSortZorder)
			{
				uint32_t  i, j, k;
				uint8_t   ucMaxZ;
				
				/* Find the index to hSource->stNewPic */
				hWindow->aulMosaicZOrderIndex[0] = 0;
				ucMaxZ = pNewInfo->aucMosaicZOrder[0];
				for(i = 1; i < pNewInfo->ulMosaicCount; i++)
				{
					j = 0;
					if(pNewInfo->aucMosaicZOrder[i] < ucMaxZ)
					{
						for(k = 0; k < i; k++)
						{
							if(pNewInfo->aucMosaicZOrder[k] <= pNewInfo->aucMosaicZOrder[i])
							{
								j++;
							}
							else
							{
								hWindow->aulMosaicZOrderIndex[k]++;
							}
						}
						hWindow->aulMosaicZOrderIndex[i] = j;
					}
					else
					{
						hWindow->aulMosaicZOrderIndex[i] = i;
						ucMaxZ = pNewInfo->aucMosaicZOrder[i];
					}
				}
			}
#endif
		}
		else if(pNewInfo->bClearRect != pCurInfo->bClearRect)
		{
			pNewDirty->stBits.bMosaicMode = BVDC_P_DIRTY;
		}
#endif

#if (BVDC_P_SUPPORT_ANR) || (BVDC_P_SUPPORT_MANR)
		/* (7) check anr and MAD setting compatibility */
		if((pNewInfo->bAnr) &&
		   (pNewInfo->bDeinterlace))
		{
			if((BPXL_IS_YCbCr422_FORMAT(pNewInfo->stMadSettings.ePixelFmt) !=
			    BPXL_IS_YCbCr422_FORMAT(pNewInfo->stAnrSettings.ePxlFormat)) &&
			   (BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(pNewInfo->stMadSettings.ePixelFmt) !=
			    BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(pNewInfo->stAnrSettings.ePxlFormat)))
			{
				BDBG_ERR(("Window[%d] ANR's pixel format mismatches with Deinterlacer!MAD[%s], ANR[%s]", hWindow->eId,
					BPXL_ConvertFmtToStr(pNewInfo->stMadSettings.ePixelFmt),
					BPXL_ConvertFmtToStr(pNewInfo->stAnrSettings.ePxlFormat)));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}

			/* TestFeature1 is enabled for both Mad and Anr, but ulBitsPerPixel in
			 * MAD and ANR are different */
			if(pNewInfo->stMadSettings.stTestFeature1.bEnable &&
			   pNewInfo->stAnrSettings.stVideoTestFeature1.bEnable &&
			   (pNewInfo->stMadSettings.stTestFeature1.ulBitsPerPixel !=
			    pNewInfo->stAnrSettings.stVideoTestFeature1.ulBitsPerPixel))
			{
				BDBG_ERR(("Window[%d] ANR's TestFeature1 setting mismatches with MAD!MAD[0x%x], ANR[0x%x]", hWindow->eId,
					pNewInfo->stMadSettings.stTestFeature1.ulBitsPerPixel,
					pNewInfo->stAnrSettings.stVideoTestFeature1.ulBitsPerPixel));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
		}
#endif

		/* (8) Bypass window can't do dest clip */
		if((BVDC_P_WindowId_eComp2_V0 == hWindow->eId) &&
		   (hWindow->hCompositor->hDisplay->hCompositor->bIsBypass))
		{
				/* due to the fact that bypass display doesn't have compositor, bypass
				 * window must be full screen; */
				if((pNewInfo->stScalerOutput.ulWidth  != ulHsize) ||
				   (pNewInfo->stScalerOutput.ulHeight != ulVsize) ||
				   (pNewInfo->stDstRect.ulWidth       != ulHsize) ||
				   (pNewInfo->stDstRect.ulHeight      != ulVsize))
				{
					return BERR_TRACE(BVDC_ERR_BYPASS_WINDOW_NOT_FULL_SCREEN);
				}
				if((BVDC_AspectRatioMode_eBypass            != pNewInfo->eAspectRatioMode) &&
				   (BVDC_AspectRatioMode_eUseAllDestination != pNewInfo->eAspectRatioMode))
				{
					return BERR_TRACE(BVDC_ERR_BYPASS_WINDOW_INVALID_AR_MODE);
				}
		}

		/* (9) bypass win and some 2nd win might might need to use shared SCL */
		if((BVDC_P_SRC_IS_VIDEO(hWindow->stNewInfo.hSource->eId)) &&
#if (BCHP_CHIP==7400)
		   (true)
#else
		   /* then this win must be an one that uses shared scl */
		   (   /* a PIP */
			   (BVDC_P_WindowId_eComp2_V0 != hWindow->eId) ||
			   /* or, bypass win that needs scl */
			   ((BVDC_P_SRC_IS_MPEG(hWindow->stNewInfo.hSource->eId)  ||
				 BVDC_P_SRC_IS_HDDVI(hWindow->stNewInfo.hSource->eId) ||
				 (BVDC_P_SRC_IS_ANALOG(hWindow->stNewInfo.hSource->eId) && hWindow->stNewInfo.hSource->stNewInfo.bCompInput)))
		   )
#endif
		  )
		{
			if (NULL == hWindow->stCurResource.hScaler)
			{
				BVDC_P_Scaler_Handle *phScaler=&hWindow->stNewResource.hScaler;

				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eScl, hWindow->pResourceFeature->ulScl,
					(uint32_t) hWindow, (void **)phScaler, true);
				BKNI_LeaveCriticalSection();
				if(NULL == hWindow->stNewResource.hScaler)
				{
					BDBG_ERR(("Window %d failed to allocate scaler ", hWindow->eId));
					goto fail_res;
				}

					pNewDirty->stBits.bSharedScl = BVDC_P_DIRTY;
				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hScaler = hWindow->stCurResource.hScaler;
				}
		}

#if (BVDC_P_SUPPORT_HSCL)
		/* (9) Acquire HSCL. */
		if((pNewInfo->bDeinterlace || pNewInfo->bAnr))
		{
			if (NULL == hWindow->stCurResource.hHscaler)
			{
				BVDC_P_Hscaler_Handle *phHscaler=&hWindow->stNewResource.hHscaler;

				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eHscl, 0, hWindow->eId, (void **)phHscaler, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hHscaler)
				{
					BDBG_ERR(("Window %d failed to allocate Hscaler ", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hHscaler = hWindow->stCurResource.hHscaler;
			}
		}
#endif


#if (BVDC_P_SUPPORT_ANR)
		/* (11) Shared anr */
		if(pNewInfo->bAnr)
		{
			if (NULL == hWindow->stCurResource.hAnr)
			{
				BVDC_P_Anr_Handle *phAnr=&hWindow->stNewResource.hAnr;

				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eAnr, 0, hWindow->eId, (void **)phAnr, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hAnr)
				{
					BDBG_ERR(("Window %d failed to allocate ANR ", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hAnr = hWindow->stCurResource.hAnr;
			}
		}
#endif

#if (BVDC_P_SUPPORT_MAD)
		/* (10) Aquire MAD */
		if(pNewInfo->bDeinterlace)
		{
			if (NULL == hWindow->stCurResource.hMad32)
			{
				BVDC_P_Mad_Handle *phMad32=&hWindow->stNewResource.hMad32;

				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eMad, hWindow->pResourceFeature->ulMad, hWindow->eId, (void **)phMad32, true);
				BKNI_LeaveCriticalSection();
			}
			else
			{
				hWindow->stNewResource.hMad32 = hWindow->stCurResource.hMad32;
			}
		}
#endif
#if (BVDC_P_SUPPORT_MCVP)
		/* (9-11') Aquire MCVP */
		if((pNewInfo->bDeinterlace || pNewInfo->bAnr))
		{
			BVDC_P_Mcvp_Handle *phMcvp=&hWindow->stNewResource.hMcvp;

			if (NULL == hWindow->stCurResource.hMcvp)
			{
				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eMcvp, hWindow->pResourceFeature->ulMad, hWindow->eId, (void **)phMcvp, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hMcvp)
				{
					BDBG_ERR(("Window %d failed to allocate MCVP ", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hMcvp = hWindow->stCurResource.hMcvp;
			}
		}
#endif

#if (BVDC_P_SUPPORT_MCVP || BVDC_P_SUPPORT_MAD)
		if(pNewInfo->bDeinterlace)
		{
			if ((NULL == hWindow->stNewResource.hMad32)&&(NULL == hWindow->stNewResource.hMcvp))
			{
				BDBG_ERR(("Window %d failed to allocate MAD or MCVP", hWindow->eId));
				goto fail_res;
			}
			else

				hWindow->bAllocResource = true;
		}
#endif

#if (BVDC_P_SUPPORT_DNR)
		/* (12) Aquire DNR */
		if((hWindow->stNewInfo.hSource->stNewInfo.bDnr))
		{
			BVDC_P_Dnr_Handle *phDnr=&hWindow->stNewResource.hDnr;

			if (NULL == hWindow->stCurResource.hDnr)
			{
				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eDnr, 0, hWindow->stNewInfo.hSource->eId, (void **)phDnr, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hDnr)
				{
					BDBG_ERR(("Window %d failed to allocate DNR ", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hDnr = hWindow->stCurResource.hDnr;
			}
		}
#endif

#if BVDC_P_SUPPORT_BOX_DETECT
		/* (13) Shared box detect */
		if(pNewInfo->bBoxDetect)
		{
			if (NULL == hWindow->stCurResource.hBoxDetect)
			{
				BVDC_P_BoxDetect_Handle *phBoxDetect=&hWindow->stNewResource.hBoxDetect;
				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eBoxDetect, 0, hWindow->stNewInfo.hSource->eId, (void **)phBoxDetect, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hBoxDetect)
				{
					BDBG_ERR(("Window %d failed to allocate BoxDetect", hWindow->eId));
					goto fail_res;
				}
				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hBoxDetect = hWindow->stCurResource.hBoxDetect;
			}

		}
#endif

#if BVDC_P_SUPPORT_FGT
		/* (14) Shared FGT */
		if(pNewInfo->bFgt)
		{
			BVDC_P_Fgt_Handle *phFgt=&hWindow->stNewResource.hFgt;

			if (NULL == hWindow->stCurResource.hFgt)
			{
				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eFgt, 0, hWindow->eId, (void **)phFgt, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hFgt)
				{
					BDBG_ERR(("Window %d failed to allocate FGT ", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hFgt = hWindow->stCurResource.hFgt;
			}
		}
#endif

#if BVDC_P_SUPPORT_HIST
		/* (15) Shared HIST */
		if((pNewInfo->bHistEnable) && (hWindow->bHistAvail))
		{
			BVDC_P_Hist_Handle *phHist=&hWindow->stNewResource.hHist;

			if (NULL == hWindow->stCurResource.hHist)
			{
				/* acquire a HW module */
				BKNI_EnterCriticalSection();
				BVDC_P_Resource_AcquireHandle_isr(hResource,
					BVDC_P_ResourceType_eHist, 0, hWindow->eId, (void **)phHist, true);
				BKNI_LeaveCriticalSection();

				if (NULL == hWindow->stNewResource.hHist)
				{
					BDBG_ERR(("Window %d failed to allocate Hist", hWindow->eId));
					goto fail_res;
				}

				hWindow->bAllocResource = true;
			}
			else
			{
				hWindow->stNewResource.hHist = hWindow->stCurResource.hHist;
			}
		}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
	/* (16) Memory for TestFeature1 */

	/* TestFeature1 is enabled for Mad, but ulBitsPerPixel is
	 * greater than or equal to pixel format */
	if((pNewInfo->stMadSettings.stTestFeature1.bEnable) &&
	   (pNewInfo->bDeinterlace))
	{
		unsigned int   uiPitch;
		uint32_t       ulWidth1, ulWidth = 720;
		const BVDC_Video_TestFeature1_Settings *pTestFeature1;

		BPXL_GetBytesPerNPixels(pNewInfo->stMadSettings.ePixelFmt, ulWidth , &uiPitch);

		pTestFeature1 = &pNewInfo->stMadSettings.stTestFeature1;
		ulWidth1 = (ulWidth * pTestFeature1->ulBitsPerPixel) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;
		ulWidth1 = (ulWidth1 + 7) / 8;

		if(ulWidth1 >= uiPitch)
		{
			BDBG_ERR(("Win[%d] Mad invalid testfeature1 settings: bpp = %d, ePixelFmt = %s", hWindow->eId,
				pTestFeature1->ulBitsPerPixel, BPXL_ConvertFmtToStr(pNewInfo->stMadSettings.ePixelFmt)));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	/* TestFeature1 is enabled for ANR, but ulBitsPerPixel is
	 * greater than or equal to pixel format */
	if((pNewInfo->stAnrSettings.stVideoTestFeature1.bEnable) &&
	   (pNewInfo->bAnr))
	{
		unsigned int   uiPitch;
		uint32_t       ulWidth1, ulWidth = 720;
		const BVDC_Video_TestFeature1_Settings *pTestFeature1;

		BPXL_GetBytesPerNPixels(pNewInfo->stAnrSettings.ePxlFormat, ulWidth , &uiPitch);

		pTestFeature1 = &pNewInfo->stAnrSettings.stVideoTestFeature1;
		ulWidth1 = (ulWidth * pTestFeature1->ulBitsPerPixel) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;
		ulWidth1 = (ulWidth1 + 7) / 8;

		if(ulWidth1 >= uiPitch)
		{
			BDBG_ERR(("Win[%d] ANR invalid testfeature1 settings: bpp = %d, ePixelFmt = %s", hWindow->eId,
				pTestFeature1->ulBitsPerPixel, BPXL_ConvertFmtToStr(pNewInfo->stAnrSettings.ePxlFormat)));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}
#endif
	}
	else
	{
		if(!BVDC_P_SRC_IS_GFX(hWindow->stNewInfo.hSource->eId))
		{
			return BERR_TRACE(BVDC_ERR_SOURCE_WINDOW_MISMATCH);
		}
	}

	/* set picture adjust dirty bit if picture adjustment values
	 * have changed */
	if((pNewInfo->sHue            != pCurInfo->sHue           ) ||
	   (pNewInfo->sContrast       != pCurInfo->sContrast      ) ||
	   (pNewInfo->sBrightness     != pCurInfo->sBrightness    ) ||
	   (pNewInfo->sSaturation     != pCurInfo->sSaturation    ) ||
	   (pNewInfo->sColorTemp      != pCurInfo->sColorTemp     ) ||
	   (pNewInfo->lAttenuationR   != pCurInfo->lAttenuationR  ) ||
	   (pNewInfo->lAttenuationG   != pCurInfo->lAttenuationG  ) ||
	   (pNewInfo->lAttenuationB   != pCurInfo->lAttenuationB  ) ||
	   (pNewInfo->lOffsetR        != pCurInfo->lOffsetR       ) ||
	   (pNewInfo->lOffsetG        != pCurInfo->lOffsetG       ) ||
	   (pNewInfo->lOffsetB        != pCurInfo->lOffsetB       ) ||
	   (pNewInfo->bCscRgbMatching != pCurInfo->bCscRgbMatching) ||
	   (pNewInfo->bContrastStretch!= pCurInfo->bContrastStretch)||
	   (hWindow->hCompositor->hDisplay->stNewInfo.bXvYcc!= hWindow->hCompositor->hDisplay->stCurInfo.bXvYcc)||
	   (hWindow->hCompositor->hDisplay->stNewInfo.stDirty.stBits.bTiming))
	{
		pNewDirty->stBits.bCscAdjust = BVDC_P_DIRTY;
	}
#if BVDC_P_SUPPORT_CMP_DEMO_MODE
	if((pNewInfo->stSplitScreenSetting.eHue != pCurInfo->stSplitScreenSetting.eHue) ||
	   (pNewInfo->stSplitScreenSetting.eContrast != pCurInfo->stSplitScreenSetting.eContrast) ||
	   (pNewInfo->stSplitScreenSetting.eBrightness != pCurInfo->stSplitScreenSetting.eBrightness) ||
	   (pNewInfo->stSplitScreenSetting.eColorTemp != pCurInfo->stSplitScreenSetting.eColorTemp) ||
	   (((pNewInfo->stSplitScreenSetting.eHue != BVDC_SplitScreenMode_eDisable) ||
	     (pNewInfo->stSplitScreenSetting.eContrast != BVDC_SplitScreenMode_eDisable) ||
	     (pNewInfo->stSplitScreenSetting.eBrightness != BVDC_SplitScreenMode_eDisable) ||
	     (pNewInfo->stSplitScreenSetting.eColorTemp != BVDC_SplitScreenMode_eDisable)) &&
	    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
	{
		pNewDirty->stBits.bCscAdjust = BVDC_P_DIRTY;
	}
#endif

	/* set luma key adjust dirty bit if luma key settings have changed */
	if((pNewInfo->bLumaKey      != pCurInfo->bLumaKey)      ||
	   (pNewInfo->ucLumaKeyMask != pCurInfo->ucLumaKeyMask) ||
	   (pNewInfo->ucLumaKeyHigh != pCurInfo->ucLumaKeyHigh) ||
	   (pNewInfo->ucLumaKeyLow  != pCurInfo->ucLumaKeyLow))
	{
		pNewDirty->stBits.bLumaKeyAdjust = BVDC_P_DIRTY;
	}

	if(hWindow->eId == BVDC_P_WindowId_eComp0_V0 &&
		((pNewInfo->sSharpness       != pCurInfo->sSharpness ||
		  pNewInfo->bSharpnessEnable != pCurInfo->bSharpnessEnable ||
		  (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect)) ||
		  hWindow->stNewInfo.hSource->stNewInfo.stDirty.stBits.bResume) &&
	     pNewInfo->bUserSharpnessConfig != true))
	{
#if BVDC_P_SUPPORT_TNT
#if (BVDC_P_SUPPORT_TNT_VER == 5)            /* TNT2 HW base */
		if (BVDC_P_Tnt_InterpolateSharpness(hWindow, pNewInfo->sSharpness) != BERR_SUCCESS)
			return BERR_TRACE(BERR_INVALID_PARAMETER);
#endif
		pNewDirty->stBits.bTntAdjust = BVDC_P_DIRTY;
#endif
#if BVDC_P_SUPPORT_TAB
		BVDC_P_Sharpness_Calculate_Peak_Values(pNewInfo->sSharpness,
		                                       &pNewInfo->ulSharpnessPeakSetting,
		                                       &pNewInfo->ulSharpnessPeakScale);
		pNewDirty->stBits.bTabAdjust = BVDC_P_DIRTY;
#endif
	}

#if BVDC_P_SUPPORT_PEP
	if(BVDC_P_WindowId_eComp0_V0 == hWindow->eId)
	{
		/* User loaded LAB table can't be used concurrently with dynamic */
		/* contrast stretch feature since they are sharing the LAB table */
		if(pNewInfo->bUserLabLuma && pNewInfo->bContrastStretch)
		{
			BDBG_ERR(("User LAB table can't be used concurent with other features in the LAB block"));
			return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
		}
		/* User loaded CAB table can't be used concurrently with other CAB */
		/* related features: auto flesh, green boost, blue boost and CMS */
		if(pNewInfo->bUserCabEnable &&
		   (BVDC_P_PEP_CMS_IS_ENABLE(&pNewInfo->stSatGain, &pNewInfo->stHueGain) ||
		    (pNewInfo->ulFleshtone  != 0) ||
		    (pNewInfo->ulBlueBoost  != 0) ||
		    (pNewInfo->ulGreenBoost != 0)))
		{
			BDBG_ERR(("User CAB table can't be used concurent with other features in the CAB block"));
			return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
		}
		/* Since CMS and auto flesh, green boost and blue boost are */
		/* sharing the CAB table, they can't be turned on concurrently  */
		if(BVDC_P_PEP_CMS_IS_ENABLE(&pNewInfo->stSatGain, &pNewInfo->stHueGain) &&
		   ((pNewInfo->ulFleshtone  != 0) ||
		    (pNewInfo->ulBlueBoost  != 0) ||
		    (pNewInfo->ulGreenBoost != 0)))
		{
			BDBG_ERR(("CMS can't be turned on concurent with other features in the CAB block"));
			return BERR_TRACE(BVDC_ERR_PEP_WINDOW_NOT_SUPPORT);
		}

		if(pNewInfo->bContrastStretch != pCurInfo->bContrastStretch ||
		   pNewInfo->bUserLabLuma != pCurInfo->bUserLabLuma)
		{
			pNewDirty->stBits.bLabAdjust = BVDC_P_DIRTY;
		}
		if(pNewInfo->bBlueStretch != pCurInfo->bBlueStretch ||
		   pNewInfo->stBlueStretch.ulBlueStretchOffset != pCurInfo->stBlueStretch.ulBlueStretchOffset ||
		   pNewInfo->stBlueStretch.ulBlueStretchSlope != pCurInfo->stBlueStretch.ulBlueStretchSlope)
		{
			pNewDirty->stBits.bLabAdjust = BVDC_P_DIRTY;
		}
		if(pNewInfo->bUserLabLuma)
		{
			uint32_t id;

			for(id = 0; id < BVDC_P_LAB_TABLE_SIZE; id++)
			{
				if(pNewInfo->aulLabLumaTbl[id] != pCurInfo->aulLabLumaTbl[id])
				{
					pNewDirty->stBits.bLabAdjust = BVDC_P_DIRTY;
					break;
				}
			}
		}
		if(pNewInfo->bUserLabCbCr)
		{
			uint32_t id;

			for(id = 0; id < BVDC_P_LAB_TABLE_SIZE; id++)
			{
				if((pNewInfo->aulLabCbTbl[id] != pCurInfo->aulLabCbTbl[id]) ||
				   (pNewInfo->aulLabCrTbl[id] != pCurInfo->aulLabCbTbl[id]))
				{
					pNewDirty->stBits.bLabAdjust = BVDC_P_DIRTY;
					break;
				}
			}
		}

#if (BVDC_P_SUPPORT_PEP_VER <= BVDC_P_SUPPORT_PEP_VER_3)
		if((!BVDC_P_PEP_CMS_COMPARE_EQ(&pNewInfo->stSatGain, &pCurInfo->stSatGain) ||
		    !BVDC_P_PEP_CMS_COMPARE_EQ(&pNewInfo->stHueGain, &pCurInfo->stHueGain)) ||
		   (BVDC_P_PEP_CMS_IS_ENABLE(&pNewInfo->stSatGain, &pNewInfo->stHueGain) &&
		    !BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect)))
		{
			if(BVDC_P_PEP_CMS_IS_ENABLE(&pNewInfo->stSatGain, &pNewInfo->stHueGain))
			{
				BDBG_MSG(("Compose CMS"));
				BVDC_P_Pep_Cms(hWindow->stCurResource.hPep, &pNewInfo->stSatGain,
					&pNewInfo->stHueGain,
					VIDEO_FORMAT_IS_HD(hWindow->hCompositor->hDisplay->stNewInfo.pFmtInfo->eVideoFmt),
					&pNewInfo->aulCabTable[0]);
			}
			pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
		}

		if((pNewInfo->ulFleshtone  != pCurInfo->ulFleshtone)   ||
		   (pNewInfo->ulBlueBoost  != pCurInfo->ulBlueBoost)   ||
		   (pNewInfo->ulGreenBoost != pCurInfo->ulGreenBoost))
		{
			if((pNewInfo->ulFleshtone  != 0) ||
			   (pNewInfo->ulBlueBoost  != 0) ||
			   (pNewInfo->ulGreenBoost != 0))
			{

				/* Calculate CAB table */
				BDBG_MSG(("Composed CAB table for new Fleshtone = %d, BlueBoost = %d, GreenBoost = %d",
					pNewInfo->ulFleshtone, pNewInfo->ulBlueBoost, pNewInfo->ulGreenBoost));
				BVDC_P_Pep_ComposeCabTable(pNewInfo->ulFleshtone,
				                           pNewInfo->ulGreenBoost,
				                           pNewInfo->ulBlueBoost,
				                           &pNewInfo->aulCabTable[0]);
			}
			pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
		}

		if(pNewInfo->bUserCabEnable != pCurInfo->bUserCabEnable)
		{
			pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
		}
		if(pNewInfo->bUserCabEnable)
		{
			uint32_t id;

			for(id = 0; id < BVDC_P_CAB_TABLE_SIZE; id++)
			{
				if(pNewInfo->aulCabTable[id] != pCurInfo->aulCabTable[id])
				{
					pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
					break;
				}
			}
		}
#endif /* (BVDC_P_SUPPORT_PEP_VER <= BVDC_P_SUPPORT_PEP_VER_3) */
	}
#endif /* BVDC_P_SUPPORT_PEP */

	/* Checking against dst size changed. Since PEP demo mode is only */
	/* available for Win0 CMP0, don't need to check for other windows */
	if(BVDC_P_WindowId_eComp0_V0 == hWindow->eId)
	{
		if((pNewInfo->stSplitScreenSetting.eAutoFlesh  != pCurInfo->stSplitScreenSetting.eAutoFlesh) ||
		   (pNewInfo->stSplitScreenSetting.eBlueBoost  != pCurInfo->stSplitScreenSetting.eBlueBoost) ||
		   (pNewInfo->stSplitScreenSetting.eGreenBoost != pCurInfo->stSplitScreenSetting.eGreenBoost) ||
		   (((pNewInfo->stSplitScreenSetting.eAutoFlesh  != BVDC_SplitScreenMode_eDisable) ||
		     (pNewInfo->stSplitScreenSetting.eBlueBoost  != BVDC_SplitScreenMode_eDisable) ||
		     (pNewInfo->stSplitScreenSetting.eGreenBoost != BVDC_SplitScreenMode_eDisable)) &&
		    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
		{
			pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
		}
		if((pNewInfo->stSplitScreenSetting.eCms !=
		    pCurInfo->stSplitScreenSetting.eCms) ||
		   ((pNewInfo->stSplitScreenSetting.eCms != BVDC_SplitScreenMode_eDisable) &&
		    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
		{
			pNewDirty->stBits.bCabAdjust = BVDC_P_DIRTY;
		}
		if((pNewInfo->stSplitScreenSetting.eContrastStretch !=
		    pCurInfo->stSplitScreenSetting.eContrastStretch) ||
		   ((pNewInfo->stSplitScreenSetting.eContrastStretch != BVDC_SplitScreenMode_eDisable) &&
		    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
		{
			pNewDirty->stBits.bLabAdjust = BVDC_P_DIRTY;
		}
		if((pNewInfo->stSplitScreenSetting.eSharpness !=
		    pCurInfo->stSplitScreenSetting.eSharpness) ||
		   ((pNewInfo->stSplitScreenSetting.eSharpness != BVDC_SplitScreenMode_eDisable) &&
		    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
		{
#if BVDC_P_SUPPORT_TAB
			pNewDirty->stBits.bTabAdjust = BVDC_P_DIRTY;
#endif
#if BVDC_P_SUPPORT_TNT
			pNewDirty->stBits.bTntAdjust = BVDC_P_DIRTY;
#endif
		}

#if BVDC_P_SUPPORT_CM3D
		if((pNewInfo->stSplitScreenSetting.eCm3d != pCurInfo->stSplitScreenSetting.eCm3d) &&
		    (pCurInfo->bCm3dRegionEnable || pCurInfo->bCm3dOverlapRegionEnable))
		{
			if(pCurInfo->bCm3dRegionEnable)
			{
				pNewDirty->stBits.bCm3dRegionAdj = BVDC_P_DIRTY;
			}
			if(pCurInfo->bCm3dOverlapRegionEnable)
			{
				pNewDirty->stBits.bCm3dOverlapRegionAdj = BVDC_P_DIRTY;
			}
		}
#endif
	}

	pNewRect  = &pNewInfo->stLumaRect.stRegion;
	pCurRect  = &pCurInfo->stLumaRect.stRegion;

#if BVDC_P_SUPPORT_HIST
	if((pNewRect->ulLeft   != pCurRect->ulLeft) ||
	   (pNewRect->ulRight  != pCurRect->ulRight) ||
	   (pNewRect->ulTop    != pCurRect->ulTop) ||
	   (pNewRect->ulBottom != pCurRect->ulBottom) ||
	   (pNewInfo->bLumaRectUserSet != pCurInfo->bLumaRectUserSet) ||
	   (!BVDC_P_Hist_Level_Cmp(&pNewInfo->stLumaRect.aulLevelThres[0], &pCurInfo->stLumaRect.aulLevelThres[0])) ||
	   (pNewInfo->stLumaRect.eNumBins != pCurInfo->stLumaRect.eNumBins) ||
	   (pNewInfo->bHistEnable != pCurInfo->bHistEnable) ||
	   (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect) ||
	   (hWindow->hCompositor->stNewInfo.pFmtInfo->bInterlaced != hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced)))
	{
		if((hWindow->stCurResource.hHist) ||
		   ((BVDC_P_WindowId_eComp0_V0 == hWindow->eId) && (!hWindow->bHistAvail)))
		{
			pNewDirty->stBits.bHistoRect = BVDC_P_DIRTY;
		}
	}
#endif

#if BVDC_P_SUPPORT_CM3D
	pNewRect = &pNewInfo->stChromaRect.stRegion;
	pCurRect = &pCurInfo->stChromaRect.stRegion;

	if((pNewRect->ulLeft   != pCurRect->ulLeft) ||
	   (pNewRect->ulRight  != pCurRect->ulRight) ||
	   (pNewRect->ulTop    != pCurRect->ulTop) ||
	   (pNewRect->ulBottom != pCurRect->ulBottom) ||
	   (pNewInfo->stChromaRect.eType != pCurInfo->stChromaRect.eType) ||
	   (pNewInfo->stChromaRect.ulSatMin != pCurInfo->stChromaRect.ulSatMin) ||
	   (pNewInfo->stChromaRect.ulHueMin != pCurInfo->stChromaRect.ulHueMin) ||
	   (pNewInfo->stChromaRect.ulHueMax != pCurInfo->stChromaRect.ulHueMax) ||
	   (pNewInfo->bChromaHistEnable != pCurInfo->bChromaHistEnable))
	{
		if ((BVDC_P_WindowId_eComp0_V0 == hWindow->eId) && hWindow->bChromaHistAvail)
		{
			pNewDirty->stBits.bChromaHistoRect = BVDC_P_DIRTY;
		}
	}
#endif

	if((pNewInfo->stSplitScreenSetting.eFgt != pCurInfo->stSplitScreenSetting.eFgt ) ||
	   ((pNewInfo->stSplitScreenSetting.eFgt != BVDC_SplitScreenMode_eDisable) &&
	    (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect, &pCurInfo->stDstRect))))
	{
		pNewDirty->stBits.bFgt = BVDC_P_DIRTY;
	}

#if (BVDC_P_SUPPORT_STG)
	/* Inherit STG/ViCE enable toggle? */
	pNewDirty->stBits.bStg |=
		hWindow->hCompositor->hDisplay->stNewInfo.stDirty.stBits.bStgEnable;
#endif

	/* Check delay offset */
	if( pNewInfo->uiVsyncDelayOffset )
	{
		uint32_t ulCount;

		ulCount = hWindow->ulBufCntNeeded +
			pNewInfo->uiVsyncDelayOffset  - pCurInfo->uiVsyncDelayOffset;

		if( ulCount > BVDC_P_MAX_MULTI_BUFFER_COUNT )
		{
			BDBG_ERR(("Lip Sync delay more than MAX for window[%d] = %d",
				hWindow->eId, ulCount));
			return BERR_TRACE(BVDC_ERR_LIP_SYNC_DELAY_MORE_THAN_MAX);
		}
	}

	/* Check requested user capture buffer count. */
	if(pNewInfo->uiCaptureBufCnt)
	{
		uint32_t ulCount;

		ulCount = hWindow->ulBufCntNeeded;

		if( pNewInfo->uiVsyncDelayOffset )
		{
			ulCount = ulCount + pNewInfo->uiVsyncDelayOffset - pCurInfo->uiVsyncDelayOffset;
		}

		ulCount = ulCount + pNewInfo->uiCaptureBufCnt - pCurInfo->uiCaptureBufCnt;

		if( ulCount > BVDC_P_MAX_MULTI_BUFFER_COUNT )
		{
			BDBG_ERR(("Number of capture buffer requested exceeds the max allowed for window[%d] = %d",
				hWindow->eId, ulCount));
			return BERR_TRACE(BVDC_ERR_CAPTURE_BUFFERS_MORE_THAN_MAX);
		}
	}

	/* Game mode clock adjustment can not work together with slave mode RM */
	if(pNewInfo->stGameDelaySetting.bEnable)
	{
		uint32_t uiIndex;

		/* No display alignment if game delay control is on; */
		if(hWindow->hCompositor->hDisplay->stNewInfo.hTargetDisplay)
		{
			BDBG_ERR(("No display alignment if game delay control is on!"));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}

		switch(hWindow->hCompositor->hDisplay->eMasterTg)
		{
		case BVDC_DisplayTg_ePrimIt:
		case BVDC_DisplayTg_eSecIt:
		case BVDC_DisplayTg_eTertIt:
			if(hWindow->hCompositor->hDisplay->stNewInfo.bEnableHdmi || hWindow->hCompositor->hDisplay->stNewInfo.bEnable656)
			{
				BDBG_ERR(("Display%d master TG%d with slaved %s cannot have game mode clock control!",
					hWindow->hCompositor->hDisplay->eId, hWindow->hCompositor->hDisplay->eMasterTg,
					hWindow->hCompositor->hDisplay->stNewInfo.bEnableHdmi? "DVO" : "656"));
				return BERR_TRACE(BERR_INVALID_PARAMETER);
			}
			break;
		case BVDC_DisplayTg_e656Dtg:
		case BVDC_DisplayTg_eDviDtg:
			for(uiIndex=0; uiIndex < BVDC_P_MAX_DACS; uiIndex++)
			{
				if(hWindow->hCompositor->hDisplay->stNewInfo.aDacOutput[uiIndex] != BVDC_DacOutput_eUnused)
				{	/* Search for valid Dac combinations */
					BDBG_ERR(("Display%d digital master TG%d with slaved DAC%d cannot have game mode clock control!",
						hWindow->hCompositor->hDisplay->eId, hWindow->hCompositor->hDisplay->eMasterTg, uiIndex));
					return BERR_TRACE(BERR_INVALID_PARAMETER);
				}
			}
			break;
		/* @@@ How to validate the change on STG*/
		case BVDC_DisplayTg_eStg0:
#if (BVDC_P_NUM_SHARED_STG > 1)
		case BVDC_DisplayTg_eStg1:
#endif
			break;
		default:
			BDBG_ERR(("Slave mode display %d cannot adjust clock to reduce game mode delay",
				hWindow->hCompositor->hDisplay->eId));
			return BERR_TRACE(BERR_INVALID_PARAMETER);
		}
	}

	BDBG_LEAVE(BVDC_P_Window_ValidateChanges);
	return BERR_SUCCESS;

fail_res:
	hWindow->stNewResource = hWindow->stCurResource;
	hWindow->bAllocResource = false;
	BDBG_LEAVE(BVDC_P_Window_ValidateChanges);
	return BERR_TRACE(BVDC_ERR_WINDOW_NOT_AVAILABLE);
}

/***************************************************************************
 * {private}
 *
 * This should contains all the information to detect user error settings from
 * _isr functions for user-defined real-time aspect ratio correction.
 * if any user-defined ARC settings are incorrect, we'll ignore those.
 */
void BVDC_P_Window_ValidateRects_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pFieldData )
{
	BVDC_P_Window_Info             *pCurInfo;
	const BVDC_P_Window_Info       *pIsrInfo;
	const BFMT_VideoInfo           *pFmtInfo;

	BDBG_ENTER(BVDC_P_Window_ValidateRects_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	/* To reduce the amount of typing */
	pFmtInfo = hWindow->hCompositor->stCurInfo.pFmtInfo;
	pIsrInfo = &hWindow->stIsrInfo;
	pCurInfo = &hWindow->stCurInfo;

	/* For user-defined Aspect Ratio Correction only. */
	if((BVDC_AspectRatioMode_eBypass_isr != pCurInfo->eAspectRatioMode) ||
	   (!BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId)))
	{
		return;
	}

	/* (1) Destination rect is out of canvas. */
	if((pIsrInfo->stDstRect.lLeft + pIsrInfo->stDstRect.ulWidth > pFmtInfo->ulWidth)  ||
	   (pIsrInfo->stDstRect.lLeft < 0) ||
	   (pIsrInfo->stDstRect.lLeft_R < 0) ||
	   (pIsrInfo->stDstRect.lTop + pIsrInfo->stDstRect.ulHeight > pFmtInfo->ulHeight) ||
	   (pIsrInfo->stDstRect.lTop  < 0))
	{
		return;
	}

	/* (2) Source clipped rect sould be inside source rect.  Source,
	 * can change from picture to picture.  */
	if(pCurInfo->ePanScanType == BVDC_PanScanType_eDisable)
	{
		/* if panscan is disabled, the source rect is the full source size */
		if(((pIsrInfo->stSrcClip.ulLeft + pIsrInfo->stSrcClip.ulRight)
			> (pFieldData->ulSourceHorizontalSize)) ||
		((pIsrInfo->stSrcClip.ulTop + pIsrInfo->stSrcClip.ulBottom)
			> (pFieldData->ulSourceVerticalSize)))
		{
			return;
		}
	}
	else
	{
		uint32_t ulSrcWidth, ulSrcHeight;
		/* if panscan is enabled, the source rect is the smaller of
		 * display rect and src rect */
		ulSrcWidth  = pFieldData->ulDisplayHorizontalSize;
		ulSrcHeight = pFieldData->ulDisplayVerticalSize;
		if((ulSrcWidth  > pFieldData->ulSourceHorizontalSize) ||
			(ulSrcHeight > pFieldData->ulSourceVerticalSize))
		{
			ulSrcWidth  = pFieldData->ulSourceHorizontalSize;
			ulSrcHeight = pFieldData->ulSourceVerticalSize;
		}

		if(((pIsrInfo->stSrcClip.ulLeft + pIsrInfo->stSrcClip.ulRight)  > ulSrcWidth) ||
		   ((pIsrInfo->stSrcClip.ulTop  + pIsrInfo->stSrcClip.ulBottom) > ulSrcHeight))
		{
			return;
		}
	}

	/* (3) DstRect can not be out of scaler output boundary. */
	if((pIsrInfo->stDstRect.ulWidth + pIsrInfo->stScalerOutput.lLeft
		> pIsrInfo->stScalerOutput.ulWidth) ||
	   (pIsrInfo->stDstRect.ulHeight + pIsrInfo->stScalerOutput.lTop
		> pIsrInfo->stScalerOutput.ulHeight))
	{
		return;
	}

	/* (4) Illegal scaler output size. */
	if((pIsrInfo->stScalerOutput.ulWidth  < BVDC_P_WIN_SCL_OUTPUT_H_MIN) ||
	   (pIsrInfo->stScalerOutput.ulHeight < BVDC_P_WIN_SCL_OUTPUT_V_MIN))
	{
		return;
	}

	if((!BVDC_P_RECT_CMP_EQ(&pIsrInfo->stScalerOutput, &pCurInfo->stScalerOutput)) ||
	   (!BVDC_P_RECT_CMP_EQ(&pIsrInfo->stDstRect,      &pCurInfo->stDstRect     )) ||
	   (!BVDC_P_CLIP_RECT_CMP_EQ(&pIsrInfo->stSrcClip, &pCurInfo->stSrcClip     )))
	{
	/* Only when pasing all the above checking do we update user isr
	 * settings into the current state; otherwise we ignore it. */
		pCurInfo->stDstRect      = pIsrInfo->stDstRect;
		pCurInfo->stScalerOutput = pIsrInfo->stScalerOutput;
		pCurInfo->stSrcClip      = pIsrInfo->stSrcClip;

		/* inform *DataReady_isr to recalculate the adjusted rectangles */
		hWindow->bAdjRectsDirty = true;
	}

	BDBG_LEAVE(BVDC_P_Window_ValidateRects_isr);
	return;
}

#if BVDC_P_SUPPORT_CMP_CLEAR_RECT
/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Window_SetClearRect_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Window_Info        *pUserInfo,
	  bool                             bMaskOnly )
{
	BVDC_Compositor_Handle hCompositor;

	BDBG_ENTER(BVDC_P_Window_SetClearRect_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	hCompositor = hWindow->hCompositor;
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

#if BVDC_P_SUPPORT_CMP_CLEAR_RECT
	/* this is called at window reader_isr */
	if(bMaskOnly)
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_ENABLE_MASK) =
			hWindow->ulMosaicRectSet;
	}
	/* this is called at apply changes */
	else
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_TOP_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_RECT_TOP_CTRL, RECT_ENABLE) |
			BCHP_MASK(CMP_0_V0_RECT_TOP_CTRL, RECT_CONFIG) |
#ifdef BCHP_CMP_0_V0_RECT_TOP_CTRL_RECT_KEY_VALUE_MASK
			BCHP_MASK(CMP_0_V0_RECT_TOP_CTRL, RECT_KEY_VALUE) |
#endif
			BCHP_MASK(CMP_0_V0_RECT_TOP_CTRL, RECT_COLOR_SRC));

		if(pUserInfo->bClearRect)
		{
			uint32_t ulIdx;
			bool bVsizeShift = hCompositor->stNewInfo.pFmtInfo->bInterlaced;

			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_TOP_CTRL) |=
				BCHP_FIELD_DATA(CMP_0_V0_RECT_TOP_CTRL, RECT_ENABLE, 1) |
				BCHP_FIELD_DATA(CMP_0_V0_RECT_TOP_CTRL, RECT_CONFIG, pUserInfo->bMosaicMode) |
#ifdef BCHP_CMP_0_V0_RECT_TOP_CTRL_RECT_KEY_VALUE_MASK
				BCHP_FIELD_DATA(CMP_0_V0_RECT_TOP_CTRL, RECT_KEY_VALUE, pUserInfo->ulClearRectAlpha) |
#endif
				BCHP_FIELD_DATA(CMP_0_V0_RECT_TOP_CTRL, RECT_COLOR_SRC, pUserInfo->bClearRectByMaskColor);

#ifndef BCHP_CMP_0_V0_RECT_TOP_CTRL_RECT_KEY_VALUE_MASK
			/* SW7425-2236: Use colorkey to masked out garbage */
			if(pUserInfo->bMosaicMode)
			{
				BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_COLOR) =
					BCHP_FIELD_DATA(CMP_0_V0_RECT_COLOR, Y,  0x00) |
					BCHP_FIELD_DATA(CMP_0_V0_RECT_COLOR, CR, 0x80) |
					BCHP_FIELD_DATA(CMP_0_V0_RECT_COLOR, CB, 0x80);
			}
			else
			{
				BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_COLOR) = pUserInfo->ulMaskColorYCrCb;
			}
#else
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_COLOR) = pUserInfo->ulMaskColorYCrCb;
#endif
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_ENABLE_MASK) = 0;
			for(ulIdx = 0; ulIdx < pUserInfo->ulMosaicCount; ulIdx++)
			{
				if(pUserInfo->abMosaicVisible[ulIdx])
				{
					BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_ENABLE_MASK) |= 1<<ulIdx;

					BVDC_P_WIN_GET_REG_DATA_I(ulIdx, CMP_0_V0_RECT_SIZEi_ARRAY_BASE) =
						BCHP_FIELD_DATA(CMP_0_V0_RECT_SIZEi, HSIZE,
							BVDC_P_ALIGN_UP(pUserInfo->astMosaicRect[ulIdx].ulWidth, 2)) |
						BCHP_FIELD_DATA(CMP_0_V0_RECT_SIZEi, VSIZE,
							pUserInfo->astMosaicRect[ulIdx].ulHeight >> bVsizeShift);

					BVDC_P_WIN_GET_REG_DATA_I(ulIdx, CMP_0_V0_RECT_OFFSETi_ARRAY_BASE) =
						BCHP_FIELD_DATA(CMP_0_V0_RECT_OFFSETi, X_OFFSET,
							BVDC_P_ALIGN_UP(pUserInfo->astMosaicRect[ulIdx].lLeft, 2)) |
						BCHP_FIELD_DATA(CMP_0_V0_RECT_OFFSETi, Y_OFFSET,
							pUserInfo->astMosaicRect[ulIdx].lTop >> bVsizeShift);
				}
			}
			/* update the rects enable mask */
			hWindow->ulMosaicRectSet = BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_ENABLE_MASK);
		}
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		hCompositor->ulMosaicAdjust[hWindow->eId] = BVDC_P_RUL_UPDATE_THRESHOLD;
#else
		hCompositor->ulMosaicAdjust = BVDC_P_RUL_UPDATE_THRESHOLD;
#endif
	}
#endif
	BDBG_LEAVE(BVDC_P_Window_SetClearRect_isr);
	return;
}
#endif

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Window_SetMosaicCsc_isr
	( BVDC_Window_Handle               hWindow )
{
	uint32_t usCscIdxShift = BCHP_CMP_0_V0_RECT_CSC_INDEX_0_CSC_COEFF_INDEX_RECT1_SHIFT -
		                     BCHP_CMP_0_V0_RECT_CSC_INDEX_0_CSC_COEFF_INDEX_RECT0_SHIFT;
	uint32_t i = 0;

	BDBG_ENTER(BVDC_P_Window_SetMosaicCsc_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(hWindow->stCurInfo.ulMosaicCount <= hWindow->stSettings.ulMaxMosaicRect);

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_CSC_INDEX_0) = 0;

	if (hWindow->stSettings.ulMaxMosaicRect > 8)
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_CSC_INDEX_1) = 0;
	}

	for (i = 0; i < hWindow->stCurInfo.ulMosaicCount; i++)
	{
		BAVC_MatrixCoefficients eCurColorSpace = hWindow->aeMosaicClrSpaceList[i];
		uint32_t ulMosaicCscIdx = hWindow->aeClrSpaceToMosaicCscMap[eCurColorSpace];

		if (i < 8)
		{
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_CSC_INDEX_0) |= ulMosaicCscIdx << (i * usCscIdxShift);
		}
		else
		{
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_RECT_CSC_INDEX_1) |= ulMosaicCscIdx << ((i-8) * usCscIdxShift);
		}
	}
	BDBG_LEAVE(BVDC_P_Window_SetMosaicCsc_isr);
}
#endif

/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Window_SetMiscellaneous_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Window_Info        *pWinInfo )
{
	const BVDC_P_DisplayInfo *pDisInfo;

	BDBG_ENTER(BVDC_P_Window_SetMiscellaneous_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	pDisInfo = &hWindow->hCompositor->hDisplay->stNewInfo;

#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
	if(BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, MA_COLOR_CONV_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, MB_COLOR_CONV_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, MC_COLOR_CONV_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, L2N_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, N2L_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, MA_COLOR_CONV_ENABLE, 1) |
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, MB_COLOR_CONV_ENABLE, 1) |
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, MC_COLOR_CONV_ENABLE, 1) |
			/* hack to turn L2N and N2L off by xvYCC output control */
			/* TODO: Remove when not needed */
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, L2N_ENABLE, pDisInfo->bXvYcc) |
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, N2L_ENABLE, pDisInfo->bXvYcc));
	}
	else
#endif /* BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN */
	{
		/* handle linear windows */
#if (BVDC_P_SUPPORT_CSC_MAT_COEF_VER >= 4)
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, MC_COLOR_CONV_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, MC_COLOR_CONV_ENABLE, 1));

#elif BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
		/* using v1 bit fields because they don't exist for non-linear v0 */
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE, 1));

#else
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE, 1));
#endif
	}

	/* Enable or disable color_conv_enable*/
#if BCHP_CMP_0_V0_SURFACE_CTRL_DERING_EN_ENABLE
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
	BCHP_MASK(CMP_0_V0_SURFACE_CTRL, DERING_EN));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, DERING_EN,
		!hWindow->stSettings.bBypassVideoProcessings));
#endif

	/* Enable or disable color_conv_enable*/
#if BCHP_CMP_0_V0_SURFACE_CTRL_COLOR_CONV_ENABLE_MASK
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_ENABLE,
		!hWindow->stSettings.bBypassVideoProcessings));
#endif


	/* Enable or disable visibility */
	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, pWinInfo->bVisible ? 1 : 0));
	}

#if BVDC_P_SUPPORT_WIN_CONST_COLOR
	{
		/* Get the window's source. */
		const BVDC_P_Source_Info *pSrcInfo = &pWinInfo->hSource->stNewInfo;
		BVDC_Compositor_Handle hCompositor = hWindow->hCompositor;

		/* Set window constant color. */
		switch(hWindow->eId)
		{
		case BVDC_P_WindowId_eComp0_V0:
		case BVDC_P_WindowId_eComp1_V0:
		case BVDC_P_WindowId_eComp2_V0:
		case BVDC_P_WindowId_eComp3_V0:
		case BVDC_P_WindowId_eComp4_V0:
		case BVDC_P_WindowId_eComp5_V0:
		case BVDC_P_WindowId_eComp6_V0:
			BVDC_P_CMP_GET_REG_DATA(CMP_0_V0_CONST_COLOR) = (
				BCHP_FIELD_DATA(CMP_0_V0_CONST_COLOR, Y,  BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 2)) |
				BCHP_FIELD_DATA(CMP_0_V0_CONST_COLOR, CR, BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 0)) |
				BCHP_FIELD_DATA(CMP_0_V0_CONST_COLOR, CB, BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 1)));
			break;

#if BVDC_P_CMP_0_MAX_VIDEO_WINDOW_COUNT > 1
		case BVDC_P_WindowId_eComp0_V1:
		case BVDC_P_WindowId_eComp1_V1:
			BVDC_P_CMP_GET_REG_DATA(CMP_0_V1_CONST_COLOR) = (
				BCHP_FIELD_DATA(CMP_0_V1_CONST_COLOR, Y,  BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 2)) |
				BCHP_FIELD_DATA(CMP_0_V1_CONST_COLOR, CR, BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 0)) |
				BCHP_FIELD_DATA(CMP_0_V1_CONST_COLOR, CB, BPXL_GET_COMPONENT(BPXL_eA8_Y8_Cb8_Cr8, pSrcInfo->ulMuteColorYCrCb, 1)));
			break;
#endif

		default:
			break;
		}
	}
#endif

#if BVDC_P_SUPPORT_CMP_CLEAR_RECT
	/* MosaicMode: clear rect settings for V0 only; */
	/* Note: hWindow->hCompositor->hDisplay->stNewInfo.stDirty is already
	 * cleared in BVDC_P_Display_ApplyChanges_isr. Need to use
	 * hWindow->hCompositor->hDisplay->stCurInfo.stDirty instead.
	 */
	if((hWindow->bClearRectSupport) &&
	   (pWinInfo->stDirty.stBits.bMosaicMode ||
		hWindow->hCompositor->hDisplay->stCurInfo.stDirty.stBits.bTiming))
	{
		BVDC_P_Window_SetClearRect_isr(hWindow, pWinInfo, false);
	}
#endif

	BDBG_LEAVE(BVDC_P_Window_SetMiscellaneous_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 * Configure a blender.
 */
static void BVDC_P_Window_SetBlender_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Window_Info        *pNewInfo )
{
	BVDC_Compositor_Handle hCompositor;
	BVDC_BlendFactor eFrontBlendFactor, eBackBlendFactor;
	uint8_t ucConstantAlpha;
	uint32_t ulBlendAddr     = 0;
	uint32_t ulBlendSrcSel   = 0;
	uint32_t ulPrevBlendAddr = 0;

	BDBG_ENTER(BVDC_P_Window_SetBlender_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Get the compositor that this window belongs to.   The blender
	 * registers are spread out in the compositor. */
	hCompositor = hWindow->hCompositor;
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Update bit for blender to be applied now. */
	BDBG_ASSERT(BVDC_Z_ORDER_MAX >= pNewInfo->ucZOrder);

	/* Note: These are compositor's MACRO.  The Z-Order of each window
	 * acctually selected the blender. */
	eFrontBlendFactor = pNewInfo->eFrontBlendFactor;
	eBackBlendFactor  = pNewInfo->eBackBlendFactor;
	ucConstantAlpha   = pNewInfo->ucConstantAlpha;

	ulBlendAddr       = s_aulBlendAddr[hWindow->ulBlenderId];

	/* set new blender source only if it's not destroyed; */
	if(!BVDC_P_STATE_IS_DESTROY(hWindow))
	{
		switch (hWindow->eId)
		{
		case BVDC_P_WindowId_eComp0_G0:
		case BVDC_P_WindowId_eComp1_G0:
		case BVDC_P_WindowId_eComp2_G0:
		case BVDC_P_WindowId_eComp3_G0:
		case BVDC_P_WindowId_eComp4_G0:
		case BVDC_P_WindowId_eComp5_G0:
		case BVDC_P_WindowId_eComp6_G0:
			ulBlendSrcSel = BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, SURFACE_G0);
			break;

#if (BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT >= 2)
		case BVDC_P_WindowId_eComp0_G1:
			ulBlendSrcSel = BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, SURFACE_G1);
			break;
#endif

#if (BVDC_P_CMP_0_MAX_GFX_WINDOW_COUNT >= 3)
		case BVDC_P_WindowId_eComp0_G2:
			ulBlendSrcSel = BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, SURFACE_G2);
			break;
#endif

		case BVDC_P_WindowId_eComp0_V0:
		case BVDC_P_WindowId_eComp1_V0:
		case BVDC_P_WindowId_eComp2_V0:
		case BVDC_P_WindowId_eComp3_V0:
		case BVDC_P_WindowId_eComp4_V0:
		case BVDC_P_WindowId_eComp5_V0:
		case BVDC_P_WindowId_eComp6_V0:
			ulBlendSrcSel = BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, SURFACE_V0);
			break;

		case BVDC_P_WindowId_eComp0_V1:
		case BVDC_P_WindowId_eComp1_V1:
			ulBlendSrcSel = BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, SURFACE_V1);
			break;

		default:
			/* should not get here */
			BDBG_ASSERT(false);
			break;
		}

		/* apply blender source to the correct blender */
		BVDC_P_CMP_GET_REG_ADDR_DATA(ulBlendAddr) &= ~(
			BCHP_MASK(CMP_0_BLEND_0_CTRL, BLEND_SOURCE));
		BVDC_P_CMP_GET_REG_ADDR_DATA(ulBlendAddr) |= ulBlendSrcSel;

		if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
		{
			BVDC_P_GfxFeeder_AdjustBlend_isr(&eFrontBlendFactor, &eBackBlendFactor,
			&ucConstantAlpha);
		}
	}

	/* Clear out the old blender. */
	if(!hWindow->hCompositor->abBlenderUsed[hWindow->ulPrevBlenderId])
	{
		ulPrevBlendAddr = s_aulBlendAddr[hWindow->ulPrevBlenderId];

		BVDC_P_CMP_GET_REG_ADDR_DATA(ulPrevBlendAddr) &= ~(
			BCHP_MASK(CMP_0_BLEND_0_CTRL, BLEND_SOURCE));
		BVDC_P_CMP_GET_REG_ADDR_DATA(ulPrevBlendAddr) |= (
			BCHP_FIELD_ENUM(CMP_0_BLEND_0_CTRL, BLEND_SOURCE, BACKGROUND_BYPASS));
	}

	/* Blending factors */
	BVDC_P_CMP_GET_REG_ADDR_DATA(ulBlendAddr) &= ~(
		BCHP_MASK(CMP_0_BLEND_0_CTRL, CONSTANT_ALPHA          ) |
		BCHP_MASK(CMP_0_BLEND_0_CTRL, FRONT_COLOR_BLEND_FACTOR) |
		BCHP_MASK(CMP_0_BLEND_0_CTRL, BACK_COLOR_BLEND_FACTOR ));
	BVDC_P_CMP_GET_REG_ADDR_DATA(ulBlendAddr) |=  (
		BCHP_FIELD_DATA(CMP_0_BLEND_0_CTRL, CONSTANT_ALPHA, ucConstantAlpha) |
		BCHP_FIELD_DATA(CMP_0_BLEND_0_CTRL, FRONT_COLOR_BLEND_FACTOR, eFrontBlendFactor) |
		BCHP_FIELD_DATA(CMP_0_BLEND_0_CTRL, BACK_COLOR_BLEND_FACTOR, eBackBlendFactor));

	hWindow->ulPrevBlenderId = hWindow->ulBlenderId;
	BDBG_LEAVE(BVDC_P_Window_SetBlender_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 * Enable box detect for the window: create a boxDetect handle if there is
 * free box detect hw module
 */
BERR_Code BVDC_P_Window_EnableBoxDetect
	( BVDC_Window_Handle                 hWindow,
	  BVDC_Window_BoxDetectCallback_isr  BoxDetectCallBack,
	  void                              *pvParm1,
	  int                                iParm2,
	  bool                               bAutoCutBlack)
{
	BVDC_P_Window_Info *pNewInfo;
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
	{
		eResult = BVDC_ERR_BOX_DETECT_GFX;
	}

	/* */
	pNewInfo = &hWindow->stNewInfo;
	pNewInfo->bBoxDetect = true;
	pNewInfo->bAutoCutBlack = bAutoCutBlack;
	pNewInfo->BoxDetectCallBack = BoxDetectCallBack;
	pNewInfo->pvBoxDetectParm1 = pvParm1;
	pNewInfo->iBoxDetectParm2 = iParm2;

	pNewInfo->stDirty.stBits.bBoxDetect = 1;
	hWindow->ulBoxDetectCallBckCntr = 0;

	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * Disable box detect for the window: destroy boxDetect handle
 */
BERR_Code BVDC_P_Window_DisableBoxDetect
	( BVDC_Window_Handle                 hWindow )
{
	BVDC_P_Window_Info *pNewInfo;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pNewInfo = &hWindow->stNewInfo;

	pNewInfo->bBoxDetect = false;
	pNewInfo->bAutoCutBlack = false;

	pNewInfo->stDirty.stBits.bBoxDetect = (hWindow->stCurResource.hBoxDetect)? 1 : 0;
	hWindow->ulBoxDetectCallBckCntr = 0;

	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 * Set up a window to use a user color matrix.
 */
BERR_Code BVDC_P_Window_SetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  const int32_t                    pl32_UserMatrix[BVDC_CSC_COEFF_COUNT],
	  int32_t                          pl32_WinMatrix[BVDC_CSC_COEFF_COUNT])
{
	uint32_t ulIndex;

	BDBG_ENTER(BVDC_P_Window_SetColorMatrix);
	BSTD_UNUSED(hWindow);

	/* set new values */
	for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
	{
		pl32_WinMatrix[ulIndex] = pl32_UserMatrix[ulIndex];
	}

	BDBG_LEAVE(BVDC_P_Window_SetColorMatrix);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 * Get the color matrix currently in use.
 */
BERR_Code BVDC_P_Window_GetColorMatrix
	( BVDC_Window_Handle               hWindow,
	  bool                             bOverride,
	  BVDC_P_CscCoeffs                *pCscCoeffsWin,
	  int32_t                          pl32_MatrixWin[BVDC_CSC_COEFF_COUNT],
	  int32_t                          pl32_Matrix[BVDC_CSC_COEFF_COUNT])
{
	BDBG_ENTER(BVDC_P_Window_GetColorMatrix);
	BSTD_UNUSED(hWindow);

	if(bOverride)
	{
		uint32_t ulIndex;
		for(ulIndex = 0; ulIndex < BVDC_CSC_COEFF_COUNT; ulIndex++)
		{
			pl32_Matrix[ulIndex] = pl32_MatrixWin[ulIndex];
		}
	}
	else
	{
		BKNI_EnterCriticalSection();
		BVDC_P_Csc_ToMatrix_isr(pl32_Matrix, pCscCoeffsWin, BVDC_P_FIX_POINT_SHIFT);
		BKNI_LeaveCriticalSection();
	}

	BDBG_LEAVE(BVDC_P_Window_GetColorMatrix);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 * Configure a Sec color space conversion table inside a compositor, used
 * for demo mode.
 */
void BVDC_P_Window_SetSecCscDemo_isr
	( BVDC_Window_Handle               hWindow )
{
#if BVDC_P_SUPPORT_CMP_DEMO_MODE
	BVDC_Window_SplitScreenSettings *pSplitSetting;
	uint32_t ulCscDemoEnable = 0, ulCscDemoSide, ulCscDemoBoundary;

	BDBG_ENTER(BVDC_P_Window_SetSecCscDemo_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pSplitSetting = &hWindow->stCurInfo.stSplitScreenSetting;
	/* Only enable demo mode if the feature itself is enable and the demo */
	/* mode is enable */
	if((pSplitSetting->eHue != BVDC_SplitScreenMode_eDisable &&
	    (hWindow->stCurInfo.sHue != 0 || hWindow->stCurInfo.sSaturation != 0)) ||
	   (pSplitSetting->eContrast != BVDC_SplitScreenMode_eDisable &&
	    hWindow->stCurInfo.sContrast != 0) ||
	   (pSplitSetting->eBrightness != BVDC_SplitScreenMode_eDisable &&
	    hWindow->stCurInfo.sBrightness != 0) ||
	   (pSplitSetting->eColorTemp != BVDC_SplitScreenMode_eDisable &&
	    hWindow->stCurInfo.sColorTemp != 0))
	{
		ulCscDemoEnable = 1;
	}
#if (BVDC_P_SUPPORT_CMP_DEMO_MODE >= 2)
	ulCscDemoSide = (pSplitSetting->eHue == BVDC_SplitScreenMode_eLeft) ?
		BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_CSC_L_R_LEFT :
		((pSplitSetting->eBrightness == BVDC_SplitScreenMode_eLeft) ?
		  BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_CSC_L_R_LEFT :
		  ((pSplitSetting->eContrast == BVDC_SplitScreenMode_eLeft) ?
		    BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_CSC_L_R_LEFT :
		    ((pSplitSetting->eColorTemp == BVDC_SplitScreenMode_eLeft) ?
		      BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_CSC_L_R_LEFT :
		      BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_CSC_L_R_RIGHT)));
#else
	ulCscDemoSide = (pSplitSetting->eHue == BVDC_SplitScreenMode_eLeft) ?
		BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_L_R_LEFT :
		((pSplitSetting->eBrightness == BVDC_SplitScreenMode_eLeft) ?
		  BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_L_R_LEFT :
		  ((pSplitSetting->eContrast == BVDC_SplitScreenMode_eLeft) ?
		    BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_L_R_LEFT :
		    ((pSplitSetting->eColorTemp == BVDC_SplitScreenMode_eLeft) ?
		      BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_L_R_LEFT :
		      BCHP_CMP_0_CSC_DEMO_SETTING_DEMO_L_R_RIGHT)));
#endif
	ulCscDemoBoundary = hWindow->stAdjDstRect.ulWidth / 2;

	/* Demo mode only for main window.*/
	/* TODO: will add support for CMP_1 for 7401B0,C0 later */
#if (BCHP_CHIP==7400) && (BCHP_VER >= BCHP_VER_B0)
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0 &&
	   hWindow->eId != BVDC_P_WindowId_eComp1_V0 &&
	   hWindow->eId != BVDC_P_WindowId_eComp2_V0)
#else
	if(hWindow->eId != BVDC_P_WindowId_eComp0_V0)
#endif
	{
		return;
	}

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_DEMO_ENABLE));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, COLOR_CONV_DEMO_ENABLE, ulCscDemoEnable));

	/* If it's not enabled there is no need to set the coefficients. */
	if(ulCscDemoEnable == 0)
	{
		return;
	}

	/* [ CMP_0_CSC_DEMO_SETTING ] */
#if (BVDC_P_SUPPORT_CMP_DEMO_MODE >= 2)
	BVDC_P_WIN_SET_REG_DATA(CMP_0_CSC_DEMO_SETTING, \
		BCHP_FIELD_DATA(CMP_0_CSC_DEMO_SETTING, DEMO_CSC_L_R, ulCscDemoSide) | \
		BCHP_FIELD_DATA(CMP_0_CSC_DEMO_SETTING, DEMO_CSC_BOUNDARY, ulCscDemoBoundary));
#else
	BVDC_P_WIN_SET_REG_DATA(CMP_0_CSC_DEMO_SETTING, \
		BCHP_FIELD_DATA(CMP_0_CSC_DEMO_SETTING, DEMO_L_R, ulCscDemoSide) | \
		BCHP_FIELD_DATA(CMP_0_CSC_DEMO_SETTING, DEMO_BOUNDARY, ulCscDemoBoundary));
#endif

	BDBG_LEAVE(BVDC_P_Window_SetSecCscDemo_isr);
#else
	BSTD_UNUSED(hWindow);
#endif
	return;
}

/***************************************************************************
 * {private}
 *
 * Configure a input surface size.
 */
static void BVDC_P_Window_SetSurfaceSize_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Rect               *pSurRect,
	  BAVC_Polarity                    eScanType )
{
	uint32_t ulHeight;
	uint32_t ulTop;
	uint32_t ulWidth;
	uint32_t ulShift;

	BDBG_ENTER(BVDC_P_Window_SetSurfaceSize_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* if we did xfer dst cut to src cut, pSurRect->lTop are
	 * even number or 0. Odd pSurRect->lTop */
	ulShift  = (BAVC_Polarity_eFrame!=eScanType)? 1 : 0;
	ulTop    = pSurRect->lTop >> ulShift;
	ulHeight = pSurRect->ulHeight >> ulShift;

	ulWidth = pSurRect->ulWidth;

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_OFFSET) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_OFFSET, X_OFFSET) |
		BCHP_MASK(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_OFFSET) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_OFFSET, X_OFFSET, pSurRect->lLeft) |
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_OFFSET, Y_OFFSET, ulTop));

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_SIZE) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_SIZE, HSIZE) |
		BCHP_MASK(CMP_0_V0_SURFACE_SIZE, VSIZE));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_SIZE) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_SIZE, HSIZE, ulWidth) |
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_SIZE, VSIZE, ulHeight));

	BDBG_LEAVE(BVDC_P_Window_SetSurfaceSize_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * Configure a display size surface size (clip window of input surface
 * size).
 */
void BVDC_P_Window_SetDisplaySize_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_P_Rect               *pDstRect,
	  BAVC_Polarity                    eScanType,
	  uint32_t                         ulRWinXOffset)
{
	uint32_t ulHeight;
	uint32_t ulTop;
	uint32_t ulShift;

	BDBG_ENTER(BVDC_P_Window_SetDisplaySize_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* TODO: do we allow odd lTop and/or odd height to be honored? */
	ulShift  = (BAVC_Polarity_eFrame!=eScanType)? 1 : 0;
	ulTop    = pDstRect->lTop >> ulShift;
	ulHeight = pDstRect->ulHeight >> ulShift;

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_CANVAS_OFFSET) &= ~(
		BCHP_MASK(CMP_0_V0_CANVAS_OFFSET, X_OFFSET) |
		BCHP_MASK(CMP_0_V0_CANVAS_OFFSET, Y_OFFSET));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_CANVAS_OFFSET) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_CANVAS_OFFSET, X_OFFSET, pDstRect->lLeft) |
		BCHP_FIELD_DATA(CMP_0_V0_CANVAS_OFFSET, Y_OFFSET, ulTop));

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_DISPLAY_SIZE) &= ~(
		BCHP_MASK(CMP_0_V0_DISPLAY_SIZE, HSIZE) |
		BCHP_MASK(CMP_0_V0_DISPLAY_SIZE, VSIZE));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_DISPLAY_SIZE) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_DISPLAY_SIZE, HSIZE, pDstRect->ulWidth) |
		BCHP_FIELD_DATA(CMP_0_V0_DISPLAY_SIZE, VSIZE, ulHeight));

#if BVDC_P_SUPPORT_3D_VIDEO
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_CANVAS_X_OFFSET_R) =
		BCHP_FIELD_DATA(CMP_0_V0_CANVAS_X_OFFSET_R, X_OFFSET, ulRWinXOffset);
#else
	BSTD_UNUSED(ulRWinXOffset);
#endif

	BDBG_LEAVE(BVDC_P_Window_SetDisplaySize_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * Set video surface src and dsp rect for muted case and vnet shut-down
 * case (constant color will be displayed).
 */
static void BVDC_P_Window_SetMutedSurSize_isr
	( BVDC_Window_Handle               hWindow,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Rect  stSrcRect;
	uint32_t     ulRWindowXOffset = 0;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	stSrcRect = hWindow->stCurInfo.stDstRect;
	stSrcRect.lTop = 0;
	stSrcRect.lLeft = 0;
#if (0==BVDC_P_SUPPORT_BVN_444)
	/* user setting stDstRect might not be aligned */
	stSrcRect.ulWidth = BVDC_P_ALIGN_UP(stSrcRect.ulWidth, 2);
#endif

	ulRWindowXOffset = (uint32_t) (hWindow->stCurInfo.lRWinXOffsetDelta + hWindow->stCurInfo.stDstRect.lLeft);

	BVDC_P_Window_SetSurfaceSize_isr(hWindow, &stSrcRect, eFieldId);
	BVDC_P_Window_SetDisplaySize_isr(hWindow, &hWindow->stCurInfo.stDstRect, eFieldId, ulRWindowXOffset);

}

/***************************************************************************
 * {private}
 *
 * This function sets the window's output switch to compositor. When the
 * output is diabled, i.e. the reader vnet is shut down, the compositor might
 * use constant color to show the location of the window if the window is
 * active; or really disable the video surface in compositor is the the window
 * is not active or to be destroyed.
 * ucAlpha is used to control alpha setting for the video window.A sepcial way
 * to hide video window is by setting its alpha to source zero.
 */
static void BVDC_P_Window_WinOutSetEnable_isr
	( BVDC_Window_Handle               hWindow,
	  uint8_t                          ucAlpha,
	  bool                             bEnable )
{
	BDBG_ENTER(BVDC_P_Window_WinOutSetEnable_isr);
	/* Get the compositor that this window belongs to. */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Update for miscellaneous applied now. */
#if BVDC_P_SUPPORT_WIN_CONST_COLOR
	if (bEnable)  /* maybe enable foe drain */
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, CONST_COLOR_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE));
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 1));
	}
	else if ((BVDC_P_State_eActive != hWindow->eState) && /* after create applied */
			 (BVDC_P_State_eDestroy != hWindow->eState)) /* before destroy applied */
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, CONST_COLOR_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE));
	}
	else  /* disable, but not destroy. i.e. only vnet shut down */
	{
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, CONST_COLOR_ENABLE) |
			BCHP_MASK(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE));
		/* temporarily disable const color due to stress test vfd intr issue */
/*
		BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, CONST_COLOR_ENABLE, 1) |
			BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 1));
*/
	}
#else
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, bEnable));
#endif

	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) &= ~(
		BCHP_MASK(CMP_0_V0_SURFACE_CTRL, VWIN_ALPHA));
	BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
		BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, VWIN_ALPHA, ucAlpha));

	BDBG_LEAVE(BVDC_P_Window_WinOutSetEnable_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * This function xfer partial or complete horizontal scaling from SCL to
 * HSCL.  This reduce the bandwidth requirement on Deinterlacer/ANR.
 */
static void BVDC_P_Window_TryXferHrzSclToHscl_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture,
	  uint32_t                         ulHsclSrcHrzSclThr )
{
	uint32_t ulNrmHi, ulNrmLo;
	uint32_t ulHsclInWidth, ulHsclOutWidth;
	uint32_t ulHsclCentralSrcRegion;
	bool bBypassSCL = false;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	ulHsclInWidth = pPicture->pHsclIn->ulWidth;
	if(((pPicture->stSclOut.ulWidth < ulHsclInWidth) ||
	    (hWindow->stCurInfo.stMadSettings.bShrinkWidth)) &&
	   (ulHsclInWidth > ulHsclSrcHrzSclThr))
	{
		/* if mad/mcdi is not used, stMadOut doesn't mean deinterlacer output
		 * rectangle any more, it's borrowed to support ANR-only mode; */
		pPicture->stMadOut = *(pPicture->pHsclIn);

		/* try to xfer partial horizontal scale to HSCL to avoid RTS problem
		 * note: mad is right before scl */

		if(pPicture->stSclOut.ulWidth <= ulHsclSrcHrzSclThr ||
		   hWindow->stCurInfo.stMadSettings.bShrinkWidth)
		{
			/* in this case, for both linear and non-linear scaling, hscl
			 * linearly maps its full input to ulHsclSrcHrzSclThr, no crop,
			 * and setup in SCL will be ajusted according to this map ratio */
			ulHsclOutWidth = ulHsclSrcHrzSclThr;

			/* "bBypassSCL = false;" will cause setup in HSCL and ratio adjust
			 * for setup in SCL */
		}
		else /* if(pPicture->stSclOut.ulWidth > ulHsclSrcHrzSclThr) */
		{
			/* for linear scaling, HSCL will scale to the final
			 * size and do crop, SCL should just pass through;
			 * for non-linear scaling, HSCL will linearly scale to
			 * to the final size too, but no crop, and setup in SCL will be
			 * ajusted according to this map ratio -> perform non-linear
			 * scale between the same final size */
			ulHsclOutWidth = pPicture->stSclOut.ulWidth;

			if(0 == pPicture->ulNonlinearSclOutWidth)
			{
				/* hscl perform hrz SclCut and scale, SCL should just pass through */
				bBypassSCL = true;
				pPicture->lHsclCutLeft = pPicture->stSclCut.lLeft;
				pPicture->lHsclCutLeft_R = pPicture->stSclCut.lLeft_R;
				pPicture->ulHsclCutWidth = pPicture->stSclCut.ulWidth;
				pPicture->ulHsclNrmHrzSrcStep = pPicture->ulNrmHrzSrcStep;

				pPicture->stSclCut.lLeft = 0;
				pPicture->stSclCut.lLeft_R = 0;
				pPicture->stSclCut.ulWidth = ulHsclOutWidth;
				pPicture->ulNrmHrzSrcStep = (1<<BVDC_P_NRM_SRC_STEP_F_BITS);
			}
		}

		/* stMadOut stores hscl's output rect now */
		pPicture->stMadOut.ulWidth = ulHsclOutWidth;

		if (false == bBypassSCL)
		{
			/* pPicture->ulHsclNrmHrzSrcStep = (ulHsclInWidth / ulHsclOutWidth); */
			pPicture->ulHsclNrmHrzSrcStep =
				(ulHsclInWidth << BVDC_P_NRM_SRC_STEP_F_BITS) / ulHsclOutWidth;

			/* pPicture->ulNrmHrzSrcStep *= (ulHsclOutWidth / ulHsclInWidth); */
			BMTH_HILO_32TO64_Mul(pPicture->ulNrmHrzSrcStep, ulHsclOutWidth, &ulNrmHi, &ulNrmLo);
			BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulHsclInWidth, &ulNrmHi, &ulNrmLo);
			BDBG_ASSERT(ulNrmHi == 0);
			pPicture->ulNrmHrzSrcStep = ulNrmLo;

			pPicture->stSclCut.lLeft =
				(pPicture->stSclCut.lLeft * ulHsclOutWidth) / ulHsclInWidth;
			pPicture->stSclCut.ulWidth =
				(pPicture->stSclCut.ulWidth * ulHsclOutWidth) / ulHsclInWidth;
			pPicture->stSclCut.lLeft_R =
				(pPicture->stSclCut.lLeft_R * ulHsclOutWidth) / ulHsclInWidth;

			if(0 != pPicture->ulNonlinearSclOutWidth)
			{
				ulHsclCentralSrcRegion = ulHsclInWidth - 2 * pPicture->ulNonlinearSrcWidth;
				ulHsclCentralSrcRegion =
					(ulHsclCentralSrcRegion * ulHsclOutWidth) / ulHsclInWidth;
				pPicture->ulNonlinearSrcWidth =
					(ulHsclOutWidth - ulHsclCentralSrcRegion) / 2;

				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Xfr'd NL SCL step size: 0x%x", pPicture->ulNrmHrzSrcStep));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Xfr'd NL SCL src width: 0x%x", pPicture->ulNonlinearSrcWidth));
			}
		}

		/* 3549/3556 MAD uses 4:2:2 internally, so hsclOut, anrIn/Out, and madIn/Out
		 * width must be aligned to even number. Note: no need to use AlignPreMadRect_isr
		 * since we are not dealing with hscl cnt rect here, but output which does not
		 * change*/
#if (BVDC_P_DCX_HSIZE_WORKAROUND)
		if(hWindow->stCurInfo.stMadSettings.stTestFeature1.bEnable ||
		   hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1.bEnable)
		{
			pPicture->stMadOut.ulWidth = BVDC_P_ALIGN_UP(pPicture->stMadOut.ulWidth, 4);
		}
		else
#endif
		{
			pPicture->stMadOut.ulWidth = BVDC_P_ALIGN_UP(pPicture->stMadOut.ulWidth, 2);
		}

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
		BVDC_P_Window_PreMadAdjustWidth_isr(pPicture->stMadOut.ulWidth,
			(pPicture->PicComRulInfo.eSrcOrigPolarity == BAVC_Polarity_eFrame) ?
			pPicture->stMadOut.ulHeight : pPicture->stMadOut.ulHeight /2,
			BVDC_P_MADR_DCXS_COMPRESSION(hWindow->stCurInfo.stMadSettings.stTestFeature1.ulBitsPerPixel),
			&pPicture->stMadOut.ulWidth);
#endif

		/* if HSCL is separate, or inside MCVP  */
#if (BVDC_P_SUPPORT_MCVP)
		if(pPicture->stVnetMode.stBits.bUseMcvp)
#else
		if(pPicture->stVnetMode.stBits.bUseHscl)
#endif
		{
			pPicture->pHsclOut = &pPicture->stMadOut;
			pPicture->pMadIn   = pPicture->pHsclOut;
			pPicture->pMadOut  = pPicture->pMadIn;

			if(pPicture->stVnetMode.stBits.bUseAnr)
			{
				pPicture->pAnrIn = pPicture->pHsclOut;
				pPicture->pAnrOut = pPicture->pHsclOut;
			}
		}
		else /* HSCL is inside MAD */
		{
			pPicture->pMadOut = &pPicture->stMadOut;
			if(pPicture->stVnetMode.stBits.bUseAnr)
			{
				pPicture->pAnrOut = &pPicture->stMadOut;
			}
		}
		pPicture->pSclIn = (pPicture->stVnetMode.stBits.bUseMad) ?
			pPicture->pMadOut : pPicture->pAnrOut;

		BDBG_ASSERT(pPicture->stVnetMode.stBits.bUseAnr || pPicture->stVnetMode.stBits.bUseMad);
	}
}

/***************************************************************************
 * {private}
 *
 * This function overrides the size that exceeds the limit with the proper
 * limit
 */
static void BVDC_P_Window_EnforceMinSizeLimit_isr
	( BVDC_P_Rect                     *pRec,
	  const char                      *pchName,
	  uint32_t                         ulMinH,
	  uint32_t                         ulMinV,
	  bool                             bInterlace )
{
	uint32_t ulMinHeight = (bInterlace) ? ulMinV * 2 : ulMinV;

#if !(BDBG_DEBUG_BUILD)
	BSTD_UNUSED(pchName);
#endif

	if(pRec != NULL)
	{
		if(pRec->ulWidth < ulMinH)
		{
			BDBG_MSG(("%s violation: ulWidth = %d vs %d", pchName, pRec->ulWidth, ulMinH));
			pRec->ulWidth = ulMinH;
		}
		if(pRec->ulHeight < ulMinHeight)
		{
			BDBG_MSG(("%s violation: ulHeight = %d vs %d", pchName, pRec->ulHeight, ulMinV));
			pRec->ulHeight = ulMinHeight;
		}
	}
}

/***************************************************************************
 * {private}
 *
 * This function overide pPicture->stFlags.bPictureRepeatFlag to false if clipping
 * to left or top before mad changed, because to mad, its input is no-longer
 * the same.
 */
#if (BVDC_P_SUPPORT_MAD || BVDC_P_SUPPORT_MCVP)
static void BVDC_P_Window_AdjustPicRepeatBit_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture )
{
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* if content or cutiing before mad changes, we overide bPictureRepeatFlag
	 * to false 4 times, to flush the last change out */
	if (((hWindow->lPrevSrcOutLeft != pPicture->stSrcOut.lLeft) ||
		 (hWindow->lPrevSrcOutTop  != pPicture->stSrcOut.lTop)) ||
		(BVDC_P_VNET_USED_CAPTURE(pPicture->stVnetMode) &&
		 BVDC_P_VNET_USED_MAD_AT_READER(pPicture->stVnetMode) &&
		 ((hWindow->lPrevCapOutLeft != pPicture->stCapOut.lLeft) ||
		  (hWindow->lPrevCapOutTop  != pPicture->stCapOut.lTop))))
	{
#if BVDC_P_SUPPORT_MAD
		if(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode))
		{
			hWindow->ulMadFlushCntr = 1 + BVDC_P_Mad_GetVsyncDelayNum_isr(
				hWindow->stCurInfo.stMadSettings.eGameMode);
		}
#elif BVDC_P_SUPPORT_MCVP
		if(BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode))
		{
			hWindow->ulMadFlushCntr = 1 + BVDC_P_Mcdi_GetVsyncDelayNum_isr(
				hWindow->stCurResource.hMcvp->hMcdi,
				hWindow->stCurInfo.stMadSettings.eGameMode);
		}
#endif
	}
	if (hWindow->ulMadFlushCntr > 0)
	{
		hWindow->ulMadFlushCntr --;
		pPicture->stFlags.bPictureRepeatFlag = false;
	}

	hWindow->lPrevSrcOutLeft = pPicture->stSrcOut.lLeft;
	hWindow->lPrevSrcOutTop  = pPicture->stSrcOut.lTop;
	hWindow->lPrevCapOutLeft = pPicture->stCapOut.lLeft;
	hWindow->lPrevCapOutTop  = pPicture->stCapOut.lTop;
}


/***************************************************************************
 * {private}
 *
 * This function counting non-ignore pictures between Rd and Wr index in
 * stMadDelayed structure
 *
 */
static void BVDC_P_Window_CountNonIgnoredPic_isr
	( uint32_t                        ulDeferIdxRd,
	  uint32_t                        ulDeferIdxWr,
	  BVDC_Window_Handle               hWindow)
{
	uint32_t ulDropNonIgnorePics = 0;
	uint32_t i=(ulDeferIdxRd+1)&3;
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

#if (BVDC_P_SUPPORT_STG)

	while (i!= ulDeferIdxWr)
	{
		i = (i+1) & 3;
		ulDropNonIgnorePics += !hWindow->stMadDelayed[i].bIgnorePicture;
	}

#else
	BSTD_UNUSED(ulDeferIdxRd);
	BSTD_UNUSED(ulDeferIdxWr);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(i);
#endif

	hWindow->ulDropCntNonIgnoredPics += ulDropNonIgnorePics;

}
/***************************************************************************
 * {private}
 *
 * This function adjust stuff in picture node to reflect the vsync delay
 * caused by MAD.
 *
 * Note: we assume SCL is always following MAD.
 * This function is very tricky in that it has to mimic the pipelinging
 * of the picture info (size/cropping/hist etc) delayed by deinterlacer;
 * the purpose is to program the BVN blocks (SCL/PEP) after the deinterlacer
 * in the frame accurate fashion to avoid visual artifact;
 */
static void BVDC_P_Window_AdjustForMadDelay_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture )
{
	BVDC_P_Window_MadDelayed *pWriter, *pReader;
	uint32_t ulPrevDeferIdxWr, ulDeferIdxWr,ulPrevDeferIdxRd, ulDeferIdxRd, ulDeferHistIdxRd;
	uint16_t usMadVsyncDelay = 0;
	bool bMadHardStart = false;
	bool bRdIdxStall = false;
	bool bRdIdxIncby01 = true;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	ulPrevDeferIdxRd = hWindow->ulDeferIdxRd;
	ulPrevDeferIdxWr = hWindow->ulDeferIdxWr;

#if BVDC_P_SUPPORT_MAD
	if(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode))
	{
		bMadHardStart = BVDC_P_Mad_BeHardStart_isr(hWindow->stCurResource.hMad32);
		usMadVsyncDelay = BVDC_P_Mad_GetVsyncDelayNum_isr(
			hWindow->stCurInfo.stMadSettings.eGameMode);
	}
#elif BVDC_P_SUPPORT_MCVP
	if(BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode))
	{
		bMadHardStart = BVDC_P_Mcdi_BeHardStart_isr(hWindow->stCurResource.hMcvp->hMcdi);
		usMadVsyncDelay = BVDC_P_Mcdi_GetVsyncDelayNum_isr(
			hWindow->stCurResource.hMcvp->hMcdi,
			hWindow->stCurInfo.stMadSettings.eGameMode);

	}
#endif

	/* Note: mad hard start must reset the pipeline!
	 * MAD would hard start whenever it's enabled for the 1st time or pic size
	 * changes; MAD game mode change does not necessarily cause hard start unless
	 * buffer re-allocation! */
	if (bMadHardStart || hWindow->bResetMadDelaySwPipe)
	{
		/* initialize the rd to coincide with wr to have valid start point; */
		hWindow->ulDeferIdxRd = hWindow->ulDeferIdxWr;

		/* clear the reset flag */
		hWindow->bResetMadDelaySwPipe = false;
	}
	else
	{
		hWindow->ulDeferIdxWr = (ulPrevDeferIdxWr + 1) & 0x3; /* % 4 */

		/* we always increase writer ptr, but increase reader ptr only after writer
		 * has gone usMadVsyncDelay steps ahead of reader ptr.
		 * note: this decision has to be done with the previous ulDeferIdxWr before move,
		 * otherwise in the case that usMadVsyncDelay==3, writer will have gone back
		 * to the original postion before reader starts to move, and then reader will
		 * never move. */
		if(((ulPrevDeferIdxWr + 4 - hWindow->ulDeferIdxRd) & 3) >= (uint32_t)usMadVsyncDelay)
		{
			/* if mad game mode delay decreases at this vsync, this will make reader ptr
			 * jump to catch up writer ptr. If mad game mode delay increases at this vsync,
			 * the above test will fail for a few vsync so that reader ptr will pause and
			 * writer ptr will move alone until the new delay is reached */
			hWindow->ulDeferIdxRd = (hWindow->ulDeferIdxWr + 4 - usMadVsyncDelay) & 0x3; /* % 4 */
		}
	}

	ulDeferIdxWr = hWindow->ulDeferIdxWr;
	ulDeferIdxRd = hWindow->ulDeferIdxRd;

	bRdIdxStall = (ulDeferIdxRd == ulPrevDeferIdxRd) && (!bMadHardStart);
	bRdIdxIncby01 = ((ulDeferIdxRd == ((ulPrevDeferIdxRd+1)&3)) ||
					((ulDeferIdxRd == ulPrevDeferIdxRd) && (!bMadHardStart)));


	/* counting non-ignore picture between Prev Rd and Wr idx */
	if(bMadHardStart) {
		BVDC_P_Window_CountNonIgnoredPic_isr(ulPrevDeferIdxRd, ulPrevDeferIdxWr, hWindow);
	} else if(!bRdIdxIncby01) {
		BVDC_P_Window_CountNonIgnoredPic_isr(ulPrevDeferIdxRd, ulDeferIdxRd, hWindow);
	}
	/* update mad delay affected setting in picture node to pipeline the picture
	 * info that has to flow through the MAD delays;
	 * Note: this ensures the pieplined picture info always reflect the latest
	 * to support smooth transition and pipelining; */
	pWriter = &hWindow->stMadDelayed[ulDeferIdxWr];

	pWriter->stSclCut = pPicture->stSclCut;
	pWriter->ulNrmHrzSrcStep = pPicture->ulNrmHrzSrcStep;
	pWriter->ulNrmVrtSrcStep = pPicture->ulNrmVrtSrcStep;
	pWriter->ulNonlinearSrcWidth = pPicture->ulNonlinearSrcWidth;
	pWriter->ulNonlinearSclOutWidth = pPicture->ulNonlinearSclOutWidth;
	pWriter->ulCentralRegionSclOutWidth = pPicture->ulCentralRegionSclOutWidth;
	pWriter->stSclOut = pPicture->stSclOut;
	pWriter->ulOrigPTS = pPicture->ulOrigPTS;

#if BVDC_P_SUPPORT_STG
	pWriter->bIgnorePicture       = pPicture->bIgnorePicture;
	pWriter->bLast                = pPicture->bLast;
	pWriter->bChannelChange       = pPicture->bChannelChange;
	pWriter->ulDecodePictureId    = pPicture->ulDecodePictureId;
	pWriter->ulStgPxlAspRatio_x_y = pPicture->ulStgPxlAspRatio_x_y;
	pWriter->ePictureType         = pPicture->ePictureType;
	pWriter->eBarDataType         = pPicture->eBarDataType;
	pWriter->ulTopLeftBarValue    = pPicture->ulTopLeftBarValue;
	pWriter->ulBotRightBarValue   = pPicture->ulBotRightBarValue;
	pWriter->bPreChargePicture    = pPicture->bPreChargePicture;
	pWriter->bEndofChunk          = pPicture->bEndofChunk;
	pWriter->ulChunkId            = pPicture->ulChunkId;
#endif

	if (BVDC_P_VNET_USED_MAD_AT_WRITER(pPicture->stVnetMode))
	{
		pWriter->stCapOut = pPicture->stCapOut;
		pWriter->stVfdOut = pPicture->stVfdOut;
		pWriter->stWinIn = pPicture->stWinIn;
	}
	pWriter->stWinOut = pPicture->stWinOut;

	pWriter->stHistData = pPicture->stCurHistData;
	pWriter->ulHistSize = pPicture->ulCurHistSize;

	/* It is very important for modules to read the correctly delayed picture info! */
	if (usMadVsyncDelay > 0)
	{
		pReader = &hWindow->stMadDelayed[ulDeferIdxRd];

		pPicture->ulNrmHrzSrcStep = pReader->ulNrmHrzSrcStep;
		pPicture->ulNrmVrtSrcStep = pReader->ulNrmVrtSrcStep;
		pPicture->ulNonlinearSrcWidth = pReader->ulNonlinearSrcWidth;
		pPicture->ulNonlinearSclOutWidth = pReader->ulNonlinearSclOutWidth;
		pPicture->ulCentralRegionSclOutWidth = pReader->ulCentralRegionSclOutWidth;
		pPicture->stSclOut = pReader->stSclOut;
		pPicture->stSclCut = pReader->stSclCut;
		pPicture->stWinOut = pReader->stWinOut;
		pPicture->ulOrigPTS = pReader->ulOrigPTS;

#if (BVDC_P_SUPPORT_STG)
		pPicture->bIgnorePicture = pReader->bIgnorePicture;

		if(hWindow->ulDropCntNonIgnoredPics)
		{
			pPicture->bIgnorePicture = false;
			hWindow->ulDropCntNonIgnoredPics --;
		}

		pPicture->bLast                = pReader->bLast;
		pPicture->bChannelChange       = pReader->bChannelChange;
		pPicture->ulDecodePictureId    = pReader->ulDecodePictureId;
		pPicture->ulStgPxlAspRatio_x_y = pReader->ulStgPxlAspRatio_x_y;
		pPicture->ePictureType         = pReader->ePictureType;
		pPicture->eBarDataType         = pReader->eBarDataType;
		pPicture->ulTopLeftBarValue    = pReader->ulTopLeftBarValue;
		pPicture->ulBotRightBarValue   = pReader->ulBotRightBarValue;
		pPicture->bPreChargePicture    = pReader->bPreChargePicture;
		pPicture->bEndofChunk          = pReader->bEndofChunk;
		pPicture->ulChunkId            = pReader->ulChunkId;
#endif

		if (BVDC_P_VNET_USED_MAD_AT_WRITER(pPicture->stVnetMode))
		{
			pPicture->stCapOut = pReader->stCapOut;
			pPicture->stVfdOut = pReader->stVfdOut;
			pPicture->stWinIn  = pReader->stWinIn;
		}

		/* If Mpeg Src and no capture, reduce the MAD delay by 1 to achieve */
		/* -1 delay histogram and dynamic contrast */
		if(!BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode))
		{
			ulDeferHistIdxRd = ulDeferIdxRd;
			if(((ulDeferIdxWr + 4 - ulDeferHistIdxRd) & 3) > (uint32_t)(usMadVsyncDelay - 1))
			{
				ulDeferHistIdxRd = (ulDeferIdxWr + 4 - (usMadVsyncDelay - 1)) & 0x3; /* % 4 */
			}
			pReader = &hWindow->stMadDelayed[ulDeferHistIdxRd];
		}
		pPicture->stCurHistData = pReader->stHistData;
		pPicture->ulCurHistSize = pReader->ulHistSize;
	}
}
#endif

/* -----------------------------------------------------------------------------------
 * {private}
 *
 * This function used to align a pre-mad rectangles (left, width) or (top, height) to
 * a 1, 2, or 4 pixel/line boundary. Here is the explain to the parameters:
 *
 * min:       left or top, a fixed point number with fraction bits number minFracBits
 * size:      width or height, an integer number (no fraction bits)
 * fullSize:  the original full size. Interval [min, min+size] is inside [0, fullSize]
 * alignUnit: constant 1, 2 or 4, min and max will be aligned to alignUnit boundary
 * newMin:    aligned min, integer number, no fraction bits
 * newSize:   aligned size, an integer number (no fraction bits)
 *
 * In TV app, we might zoon into part of source rect, and then pan the zoom rect left
 * and right, top and bottom. If we simply align min to floor, max to ceiling, the
 * aligned size (i.e. max - min) will change back and forward even if the zoom rect size
 * does not change and is still fully inside the src rect. Since input size change leads
 * to MAD hard-start (flash), we want to avoid it.
 *
 * When the zoom rect is panned through the src rect, the biggest new size caused by
 * min and max alignment is
 *     ceil(size) + alignUnit.
 * This can be seen in the following picture, for ulSize = 1, 2, 3, 4, 5, 6, 7, 8.
 *
 *   |               |               |               |               |
 *   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
 *   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
 *
 * We make alinged size always be biggest, so it will not change as panning in the src.
 */
void BVDC_P_Window_AlignPreMadRect_isr
	( int32_t                          lMin,
	  int32_t                          lMin_R,
	  uint32_t                         ulSize,
	  uint32_t                         ulFullSize,
	  uint32_t                         ulMinFracBits,
	  uint32_t                         ulAlignUnit,
	  int32_t                         *plNewMin,
	  int32_t                         *plNewMin_R,
	  uint32_t                        *pulNewSize )
{
	uint32_t  ulFracAlignUnit, ulMax;
	uint32_t  ulNewMin, ulNewSize, ulNewMax;
	uint32_t  ulNewMin_R;

	ulFracAlignUnit = ulAlignUnit << ulMinFracBits;
	ulNewMin = BVDC_P_ALIGN_DN(lMin, ulFracAlignUnit) >> ulMinFracBits;
	ulNewMin_R = BVDC_P_ALIGN_DN(lMin_R, ulFracAlignUnit) >> ulMinFracBits;

	ulNewSize = ulAlignUnit + BVDC_P_ALIGN_UP(ulSize, ulAlignUnit);

	/* no longer need to maintain MAD input size if src edge is reached */
	ulNewMax = ulNewMin + ulNewSize;
	if ((ulNewMax >= ulFullSize) || (0 == ulNewMin))
	{
		ulMax = ulSize + ((lMin + (1 << ulMinFracBits) - 1) >> ulMinFracBits);

		/* take off the extra ulAlignUnit added above if OK */
		ulNewSize = (((ulMax + ulAlignUnit) <= ulNewMax) ||
					 (ulNewMax > BVDC_P_ALIGN_UP(ulFullSize, ulAlignUnit)))?
			(ulNewSize - ulAlignUnit) : ulNewSize;
	}

#if (!BVDC_P_MADR_HSIZE_WORKAROUND)
	/* expanding can not go beyond FullSize */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
	{
		uint32_t ulNewSizeMin;

		ulNewSizeMin = BVDC_P_MIN(ulNewSize, ulFullSize - ulNewMin);
		if(ulNewSizeMin != 0)
			ulNewSize = ulNewSizeMin;
	}
#else
	ulNewSize = BVDC_P_MIN(ulNewSize, ulFullSize - ulNewMin);
#endif
#endif

	if(plNewMin)
		*plNewMin = (int32_t)ulNewMin;

	if(plNewMin_R)
		*plNewMin_R = (int32_t)ulNewMin_R;

	if(pulNewSize)
		*pulNewSize = ulNewSize;
}

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
/***************************************************************************
 * {private}
 *
 * This function is the software workaround for HW7425-869 to align a
 * pre-mad rectangles width. The adjustment of the width will be always
 * multiple of 4.
 *
 */
void BVDC_P_Window_PreMadAdjustWidth_isr
	( uint32_t                  ulPicWidth,
	  uint32_t                  ulPicHeight,
	  uint32_t                  ulBitsPerPixel,
	  uint32_t                 *pulNewPicWidth )
{
	bool      bBadAlignment, bBadAlignment2;
	uint32_t  ulRemainder, ulRemainder2, ulNewPicWidth = ulPicWidth;
	/* Minimum is 1/40 = 0.025, 8 bits fraction should be enough */
	uint32_t    ulAlign = 8;
	uint32_t    i, ulDelta, ulIncrease;

	ulRemainder = BVDC_P_MADR_GET_REMAINDER(ulPicWidth, ulPicHeight);
	bBadAlignment = BVDC_P_MADR_BAD_ALIGNMENT(ulRemainder);

#if (BVDC_P_MADR_VARIABLE_RATE)
	ulRemainder2 = BVDC_P_MADR_GET_VARIABLE_RATE_REMAINDER(ulPicWidth,
		ulPicHeight, ulBitsPerPixel);
	bBadAlignment2 = BVDC_P_MADR_VARIABLE_RATE_BAD_ALIGNMENT(ulRemainder2, ulBitsPerPixel);
#else
	ulRemainder2 = BVDC_P_MADR_GET_FIX_RATE_REMAINDER(ulPicWidth,
		ulPicHeight, ulBitsPerPixel);
	bBadAlignment2 = BVDC_P_MADR_FIX_RATE_BAD_ALIGNMENT(ulRemainder2, ulBitsPerPixel);
#endif

	BDBG_MODULE_MSG(BVDC_MADR_PICSIZE,("BVDC_P_Window_PreMadAdjustWidth_isr: %dx%dx%d: %d %d",
		ulNewPicWidth, ulPicHeight, ulBitsPerPixel, ulRemainder, ulRemainder2));

	while(bBadAlignment || bBadAlignment2)
	{
		if(bBadAlignment)
		{
			/* Start algo to adjust width. See HW7425-869 for details. */
			/* Determine increment of remainder value when we increase
			 * picture_width by 4 pixels. */
			ulDelta = 40 - ((ulPicHeight + 1) % 40);

			/* Determine how many steps needed to get out of the hang zone [32,39].
			 * if ulDelta <= ulReminder, ulDelta is negative so we use 31 as base.
			 * if ulDelta > ulReminder, the updated remainder value will wrap around
			 * and ulDelta becomes positive */

			if(ulDelta < ulRemainder)
			{
				i = BVDC_P_MAKE_FIXED_POINT(ulRemainder - 31, ulDelta, ulAlign);
			}
			else
			{
				ulDelta = 40 - ulDelta;
				i = BVDC_P_MAKE_FIXED_POINT(40 - ulRemainder, ulDelta, ulAlign);
			}

			ulIncrease = BVDC_P_ALIGN_UP(i, 1<<ulAlign);
			BDBG_MODULE_MSG(BVDC_MADR_PICSIZE,("Delta: %d, i: 0x%lx, Round up i: %d", ulDelta, i, ulIncrease));
			ulIncrease = ulIncrease >> ulAlign;
			ulNewPicWidth += 4 * ulIncrease;

		}

		if(bBadAlignment2)
		{
			ulNewPicWidth += 4;
		}

		ulRemainder = BVDC_P_MADR_GET_REMAINDER(ulNewPicWidth, ulPicHeight);
		bBadAlignment = BVDC_P_MADR_BAD_ALIGNMENT(ulRemainder);
#if (BVDC_P_MADR_VARIABLE_RATE)
		ulRemainder2 = BVDC_P_MADR_GET_VARIABLE_RATE_REMAINDER(ulNewPicWidth,
			ulPicHeight, ulBitsPerPixel);
		bBadAlignment2 = BVDC_P_MADR_VARIABLE_RATE_BAD_ALIGNMENT(ulRemainder2, ulBitsPerPixel);
#else
		ulRemainder2 = BVDC_P_MADR_GET_FIX_RATE_REMAINDER(ulNewPicWidth,
			ulPicHeight, ulBitsPerPixel);
		bBadAlignment2 = BVDC_P_MADR_FIX_RATE_BAD_ALIGNMENT(ulRemainder2, ulBitsPerPixel);
#endif

		BDBG_MODULE_MSG(BVDC_MADR_PICSIZE,("After adjustment 2: %dx%d: %d, %d",
			ulNewPicWidth, ulPicHeight, ulRemainder, ulRemainder2));
	}

	if(pulNewPicWidth)
		*pulNewPicWidth = ulNewPicWidth;
}

#endif

#if (BVDC_P_SUPPORT_STG)
/***************************************************************************
 * {private}
 *
 * This function combine the Bar data source_info & user_info, and put it into a
 * picture node .
 *
 * Note: this function is intended for video window only!
 */
static void BVDC_P_Window_UpdateBarData_isr
		( BVDC_Window_Handle			   hWindow,
		  BVDC_P_PictureNode			  *pPicture,
		  const BAVC_MVD_Field			  *pMvdFieldData)
{
	BAVC_BarDataType				 eBarDataType;
	uint32_t						 ulTopLeftBarValue,ulBotRightBarValue;
	bool                             bHorizontalBar = false, bVerticalBar = false, bValidBarData= true;
	BVDC_P_Rect                     *pWinOut, *pScanOut, *pSclCut, stCapOut;
	uint32_t                        ulDstLeft, ulDstRight, ulDstTop, ulDstBottom;
	uint32_t                        ulSrcLeft, ulSrcTop,ulSrcCutTopLeft;
	uint32_t                        ulSrcRight, ulSrcBottom,ulSrcCutBotRight;
	uint32_t                        ulCmpCanvasWidth, ulCmpCanvasHeight;
	uint32_t                        ulBarValue;

	BDBG_ENTER(BVDC_P_Window_UpdateBarData_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	eBarDataType           = pMvdFieldData->eBarDataType;
	ulTopLeftBarValue      = pMvdFieldData->ulTopLeftBarValue;
	ulBotRightBarValue     = pMvdFieldData->ulBotRightBarValue;
	ulBarValue             = pMvdFieldData->ulTopLeftBarValue + pMvdFieldData->ulBotRightBarValue;

	bValidBarData          =(BAVC_BarDataType_eLeftRight==eBarDataType)?
		(ulBarValue < pMvdFieldData->ulDisplayHorizontalSize ):
		((BAVC_BarDataType_eTopBottom==eBarDataType)?
		(ulBarValue < pMvdFieldData->ulDisplayVerticalSize) :true);

	pScanOut = &pPicture->stSrcOut;
	pSclCut  = &pPicture->stSclCut;
	pWinOut  = &hWindow->stAdjDstRect;
	stCapOut.lTop = stCapOut.lLeft=stCapOut.ulWidth=stCapOut.ulHeight=0;
	if((hWindow->bCapture) && (pPicture->pCapOut))
	{
		stCapOut.lLeft    = pPicture->pCapOut->lLeft;
		stCapOut.lTop     = pPicture->pCapOut->lTop;
		stCapOut.ulWidth  = pPicture->pCapOut->ulWidth;
		stCapOut.ulHeight = pPicture->pCapOut->ulHeight;
	}

	/* construct the compositor canvas rectangular */
	ulCmpCanvasHeight = BVDC_P_DISPLAY_USED_STG(hWindow->hCompositor->hDisplay->eMasterTg)?
		hWindow->hCompositor->hDisplay->pPrevFmtInfo->ulDigitalHeight:
		(BVDC_P_DISPLAY_USED_DIGTRIG(hWindow->hCompositor->hDisplay->eMasterTg)?
			hWindow->hCompositor->hDisplay->stCurInfo.pFmtInfo->ulDigitalHeight:
			hWindow->hCompositor->hDisplay->stCurInfo.pFmtInfo->ulHeight);

	ulCmpCanvasWidth  = BVDC_P_DISPLAY_USED_STG(hWindow->hCompositor->hDisplay->eMasterTg)?
		hWindow->hCompositor->hDisplay->pPrevFmtInfo->ulDigitalWidth:
		(BVDC_P_DISPLAY_USED_DIGTRIG(hWindow->hCompositor->hDisplay->eMasterTg)?
			hWindow->hCompositor->hDisplay->stCurInfo.pFmtInfo->ulDigitalWidth:
			hWindow->hCompositor->hDisplay->stCurInfo.pFmtInfo->ulWidth);

	ulSrcCutTopLeft = (eBarDataType == BAVC_BarDataType_eLeftRight)?(pScanOut->lLeft + pSclCut->lLeft + stCapOut.lLeft):
					(eBarDataType == BAVC_BarDataType_eTopBottom?(pScanOut->lTop + pSclCut->lTop + stCapOut.lTop):0);
	ulSrcLeft = (eBarDataType == BAVC_BarDataType_eLeftRight)?ulTopLeftBarValue:0;
	ulSrcTop  = (eBarDataType == BAVC_BarDataType_eTopBottom)?ulTopLeftBarValue:0;

	ulSrcCutBotRight = 0;
	if(eBarDataType == BAVC_BarDataType_eLeftRight)
	{
		ulSrcCutBotRight = pMvdFieldData->ulDisplayHorizontalSize - ulSrcCutTopLeft - pSclCut->ulWidth;
	}
	else if(eBarDataType == BAVC_BarDataType_eTopBottom)
	{
		ulSrcCutBotRight = pMvdFieldData->ulDisplayVerticalSize - ulSrcCutTopLeft - pSclCut->ulHeight;
	}

	ulSrcRight  = (eBarDataType == BAVC_BarDataType_eLeftRight)?ulBotRightBarValue:0;
	ulSrcBottom = (eBarDataType == BAVC_BarDataType_eTopBottom)?ulBotRightBarValue:0;


	ulSrcLeft = (ulSrcCutTopLeft < ulSrcLeft)?(ulSrcLeft - ulSrcCutTopLeft):0;
	ulSrcTop  = (ulSrcCutTopLeft < ulSrcTop)?(ulSrcTop - ulSrcCutTopLeft):0;
	ulSrcRight = (ulSrcCutBotRight <ulSrcRight)?(ulSrcRight - ulSrcCutBotRight) : 0;
	ulSrcBottom= (ulSrcCutBotRight <ulSrcBottom)?(ulSrcBottom- ulSrcCutBotRight) : 0;



	ulDstLeft = pWinOut->lLeft + ulSrcLeft*pWinOut->ulWidth/pSclCut->ulWidth;
	ulDstTop  = pWinOut->lTop + ulSrcTop*pWinOut->ulHeight/pSclCut->ulHeight;
	ulDstRight = ulCmpCanvasWidth - pWinOut->ulWidth - pWinOut->lLeft + ulSrcRight*pWinOut->ulWidth/pSclCut->ulWidth;
	ulDstBottom = ulCmpCanvasHeight - pWinOut->ulHeight - pWinOut->lTop + ulSrcBottom*pWinOut->ulHeight/pSclCut->ulHeight;

	bHorizontalBar = ulDstLeft || ulDstRight;
	bVerticalBar   = ulDstTop  || ulDstBottom;

	if((bHorizontalBar ^ bVerticalBar) && bValidBarData)
	{
		pPicture->eBarDataType = bHorizontalBar ?
			BAVC_BarDataType_eLeftRight: BAVC_BarDataType_eTopBottom;
		pPicture->ulTopLeftBarValue = bHorizontalBar ? ulDstLeft:ulDstTop;
		pPicture->ulBotRightBarValue = bHorizontalBar ? ulDstRight:ulDstBottom;

		if(bHorizontalBar && bVerticalBar)
			BDBG_MODULE_WRN(BVDC_BAR, ("Both Horizontal and verital have bars Hor %d %d Ver %d %d ",
			ulDstLeft, ulDstRight, ulDstTop, ulDstBottom));
	}
	else
	{
		pPicture->eBarDataType = BAVC_BarDataType_eInvalid;
		pPicture->ulTopLeftBarValue = 0;
		pPicture->ulBotRightBarValue = 0;
	}

	BDBG_MODULE_MSG(BVDC_BAR, ("input Src %d x %d eBarDataType %d: %d - %d",
		pMvdFieldData->ulDisplayHorizontalSize, pMvdFieldData->ulDisplayVerticalSize,
		pMvdFieldData->eBarDataType, pMvdFieldData->ulTopLeftBarValue, pMvdFieldData->ulBotRightBarValue));
	BDBG_MODULE_MSG(BVDC_BAR, ("win[%d] Rectangular: SrcScan %d %d %d x %d Cap %d %d %d x %d  SclCut %d %d %d x %d Dsp %d %d %d x %d Canvas %d x %d ",
		hWindow->eId,
		pScanOut->lTop, pScanOut->lLeft, pScanOut->ulWidth, pScanOut->ulHeight,
		stCapOut.lTop, stCapOut.lLeft, stCapOut.ulWidth, stCapOut.ulHeight,
		pSclCut->lTop, pSclCut->lLeft, pSclCut->ulWidth, pSclCut->ulHeight,
		pWinOut->lTop, pWinOut->lLeft, pWinOut->ulWidth, pWinOut->ulHeight,
		ulCmpCanvasWidth, ulCmpCanvasHeight));
	BDBG_MODULE_MSG(BVDC_BAR, ("win[%d] Bar info: Src Hor %d %d Ver %d %d Dst Hor %d %d Ver %d %d",
		hWindow->eId,
		ulSrcLeft, ulSrcRight, ulSrcTop, ulSrcBottom,
		ulDstLeft, ulDstRight, ulDstTop, ulDstBottom));
	BDBG_MODULE_MSG(BVDC_BAR, ("win[%d] output pic[%d] Bar type %d value %d %d \n ",
		hWindow->eId,
		pPicture->ulDecodePictureId, pPicture->eBarDataType, pPicture->ulTopLeftBarValue, pPicture->ulBotRightBarValue));

	BDBG_LEAVE(BVDC_P_Window_UpdateBarData_isr);
}
#endif
/***************************************************************************
 * {private}
 *
 * This function combine the source_info & user_info, and put it into a
 * picture node.
 * This determine if the pPicture will required a capture/scaler and
 * whether if scaler is needed before capture.  It merges the applied
 * user parameters from BVDC_ApplyChanges_isr(), and source parameters.
 *
 * Note: this function is intended for video window only!
 */
static void BVDC_P_Window_UpdateSrcAndUserInfo_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  const BAVC_Polarity              eFieldId )
{
	const BVDC_P_Window_Info       *pUserInfo;
	const BFMT_VideoInfo           *pSrcFmtInfo;
	BVDC_Source_Handle              hSource;
	int32_t                         lCntTop, lCntLeft;
	int32_t                         lCntLeft_R;
	uint32_t                        ulCntWidth, ulCntHeight;
	bool                            bNonlinearSclMode;
	bool                            bInterlaced;

	BDBG_ENTER(BVDC_P_Window_UpdateSrcAndUserInfo_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	hSource = hWindow->stCurInfo.hSource;
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);

	/* Get user applied informations. */
	pUserInfo   = &hWindow->stCurInfo;
	pSrcFmtInfo = hSource->stCurInfo.pFmtInfo;
	hSource->bSrcInterlaced =
		((BVDC_P_SRC_IS_MPEG(hSource->eId) && (NULL != pMvdFieldData))?
		(BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity) :
		(pSrcFmtInfo->bInterlaced));

	/* inform SCL to avoid point sampling if field inversion is expected */
	pPicture->stFlags.bHandleFldInv = hWindow->stCadHndl.bHandleFldInv;

	/* setup the PicComRulInfo that is shared by all sub-modules when they build RUL */
	pPicture->PicComRulInfo.PicDirty.stBits.bInputFormat =
		hSource->stCurInfo.stDirty.stBits.bInputFormat;
	pPicture->PicComRulInfo.PicDirty.stBits.bSrcPicChange =
		hSource->bPictureChanged; /* either src detected change or ApplyChange called for the win */

#if BVDC_P_SUPPORT_MOSAIC_MODE
	/* MosaicMode: update ClearRect mask set; */
	if(pUserInfo->bMosaicMode && (NULL != pMvdFieldData))
	{
		if((pMvdFieldData->ulChannelId < pUserInfo->ulMosaicCount) &&
		   (pUserInfo->abMosaicVisible[pMvdFieldData->ulChannelId]))
		{
			pPicture->ulMosaicRectSet |= 1 << pMvdFieldData->ulChannelId;
		}
		if(pUserInfo->ulMosaicTrackChannelId == pMvdFieldData->ulChannelId)
		{
			hSource->eFrameRateCode = pMvdFieldData->eFrameRateCode;
			hSource->eMatrixCoefficients = pMvdFieldData->eMatrixCoefficients;
			/* Correct way is to convert indivisual input color space from
			 * pFieldData->eMatrixCoefficients to tracked channel color space
			 * defined by hSource->eMatrixCoefficients. Since we don't have all
			 * the matrices yet, just convert all to HD.
			 */
			hSource->eMatrixCoefficients = BAVC_MatrixCoefficients_eItu_R_BT_709;
		}
	}
#endif

	/* Update current picture node's pixel format to current pixel format. */
	pPicture->ePixelFormat = pUserInfo->ePixelFormat;
	pPicture->ucAlpha = pUserInfo->bVisible ? pUserInfo->ucAlpha : 0;

	/* (1) This function is only called by BVDC_P_Window_Writer_isr, which
	 * is intended for video window only! */
	if((BVDC_P_SRC_IS_MPEG(hSource->eId)) && (pMvdFieldData))
	{
		/* Build the picture */
		pPicture->stFlags.bMuteFixedColor        = false;
		pPicture->stFlags.bMuteMad               = pMvdFieldData->bMute;
		pPicture->ulAdjQp                        = pMvdFieldData->ulAdjQp;

		pPicture->eMatrixCoefficients            = hSource->eMatrixCoefficients;
		pPicture->eTransferCharacteristics       = pMvdFieldData->eTransferCharacteristics;
		pPicture->eSrcPolarity                   = pMvdFieldData->eSourcePolarity;
		pPicture->PicComRulInfo.eSrcOrigPolarity = pMvdFieldData->eSourcePolarity;
		pPicture->ulOrigPTS                      = pMvdFieldData->ulOrigPTS;

		/*Mailbox data for ViCE2*/
		/*@@@ handle from hdmi input*/
#if (BVDC_P_SUPPORT_STG)
		pPicture->iHorzPanScan                   = pMvdFieldData->i32_HorizontalPanScan;
		pPicture->iVertPanScan                   = pMvdFieldData->i32_VerticalPanScan;
		pPicture->ulDispHorzSize                 = pMvdFieldData->ulDisplayHorizontalSize;
		pPicture->ulDispVertSize                 = pMvdFieldData->ulDisplayVerticalSize;
		pPicture->ePictureType                   = pMvdFieldData->ePictureType;
		pPicture->bIgnorePicture                 = pMvdFieldData->bIgnorePicture ;
		pPicture->bStallStc                      = pMvdFieldData->bStallStc;
		pPicture->bLast                          = pMvdFieldData->bLast;
		pPicture->bChannelChange                 = pMvdFieldData->bChannelChange;
		pPicture->ulPicOrderCnt                  = pMvdFieldData->int32_PicOrderCnt;
		pPicture->ulDecodePictureId              = pMvdFieldData->ulDecodePictureId;
		pPicture->ulStgPxlAspRatio_x_y           = hWindow->ulStgPxlAspRatio_x_y;
		pPicture->bValidAfd                      = pMvdFieldData->bValidAfd;
		pPicture->ulAfd                          = pMvdFieldData->ulAfd;
		pPicture->bPreChargePicture              = pMvdFieldData->bPreChargePicture;
		pPicture->bEndofChunk                    = pMvdFieldData->bEndOfChunk;
		pPicture->ulChunkId                      = pMvdFieldData->ulChunkId;
#endif

		/* hSource->hMpegFeeder->eOutputOrientation is set in BVDC_P_Feeder_SetMpegInfo_isr */
		pPicture->eSrcOrientation = hSource->hMpegFeeder->eOutputOrientation;
		pPicture->eOrigSrcOrientation = pPicture->eSrcOrientation;

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode) &&
			(BFMT_Orientation_e3D_LeftRight == pPicture->eSrcOrientation) &&
			(false == BVDC_P_Scaler_Validate_VertDepth_isr(hWindow, hWindow->stCurResource.hScaler)))
		{
			pPicture->eOrigSrcOrientation = pPicture->eSrcOrientation;
			pPicture->eSrcOrientation = pPicture->eDispOrientation;
		}
#endif
		/* Note: the following logic is intended to detect field repeat to
		   properly config MAD in C0 to support 50to60Hz or trick mode deinterlacing;
		   vdc detects the field repeat by checking both polarity and frame
		   address; might be improved by checking the detection flags sent from mvd
		   field data structure when it's available; */
		/* Note, the muted picture during channel change should go through multi-buffer
		   without being dropped for sync-slipped window; */
		pPicture->stFlags.bPictureRepeatFlag = pMvdFieldData->bPictureRepeatFlag &&
			!pMvdFieldData->bMute;
		pPicture->stFlags.bRepeatField = pMvdFieldData->bRepeatField;
		/* Phase 3 MAD chroma detection */
		pPicture->eChromaType          = (BAVC_YCbCrType_e4_2_2 == pMvdFieldData->eYCbCrType)?
			BVDC_P_ChromaType_eChroma422 :
			((BAVC_InterpolationMode_eField == pMvdFieldData->eChrominanceInterpolationMode)?
				BVDC_P_ChromaType_eField420 : BVDC_P_ChromaType_eFrame420);

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* Copy z-order information. Note: user must set same z-order value on
		 * each mosaic window for the same channel in HD_SD simul mode */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		BKNI_Memcpy((void*)hSource->aulMosaicZOrderIndex, (void*)hWindow->aulMosaicZOrderIndex,
			sizeof(uint32_t) * pUserInfo->ulMosaicCount);
#else
		BKNI_Memcpy((void*)hSource->aucMosaicZOrder, (void*)pUserInfo->aucMosaicZOrder,
			sizeof(uint8_t) * pUserInfo->ulMosaicCount);
#endif
		hSource->ulMosaicCount = pUserInfo->ulMosaicCount;
#endif

#if (BVDC_P_SUPPORT_FGT)
		if(BVDC_P_VNET_USED_FGT(hWindow->stVnetMode))
		{
			pPicture->ulIdrPicID    = pMvdFieldData->ulIdrPicID;
			pPicture->ulPicOrderCnt = pMvdFieldData->int32_PicOrderCnt;

			BMEM_Heap_ConvertAddressToOffset_isr(pMvdFieldData->hFgtHeap,
				pMvdFieldData->pFgtSeiBufferAddress, &(pPicture->ulFgtSeiBufAddr));

			/* SEI data is only valid for 1 frame. Save it in window for now.
			 * Remove later when XVD can hold SEI data longer */
			hWindow->ulFgtSeiBufAddr = pPicture->ulFgtSeiBufAddr;
		}
#endif
		if (hSource->hMpegFeeder->bGfxSrc)
		{
			BVDC_P_SurfaceInfo  *pCurSur;

			pCurSur = &hSource->hMpegFeeder->stGfxSurface.stCurSurInfo;
			pPicture->hSurface = pCurSur->stAvcPic.hSurface;
			pPicture->hSurface_R = pCurSur->stAvcPic.hRSurface;
			hSource->hMpegFeeder->stPicture.eSrcPolarity = pPicture->eSrcPolarity;
		}
	}
	else if((BVDC_P_SRC_IS_HDDVI(hSource->eId)) && (pXvdFieldData))
	{
		/* Build the picture */
		pPicture->stFlags.bMuteFixedColor        = pXvdFieldData->bMute;
		pPicture->eSrcPolarity                   = pXvdFieldData->eSourcePolarity;
		pPicture->eMatrixCoefficients            = hSource->eMatrixCoefficients;
		pPicture->eTransferCharacteristics       = pXvdFieldData->eTransferCharacteristics;
		pPicture->PicComRulInfo.eSrcOrigPolarity = pXvdFieldData->eSourcePolarity;

		/* bOrientationOverride only valid when original
		 * orientation from FMT is 2D */
		if((pSrcFmtInfo->eOrientation == BFMT_Orientation_e2D) &&
		    (hSource->stCurInfo.bOrientationOverride))
		{
			pPicture->eSrcOrientation = hSource->stCurInfo.eOrientation;
		}
		else
		{
			pPicture->eSrcOrientation = pSrcFmtInfo->eOrientation;
		}

		/* hd-dvi source callback might identify the real field repeat from polarity
		   repeat, but vdc alone cannot detect it;
		   so we simply disable the field repeat detection for the moment. */
		pPicture->stFlags.bPictureRepeatFlag = false;

		/* TODO: we should get the correct source chroma type from the HDMI receiver
		         or picture data ready callback;
		         for the timing being, just set it to 'auto' to be safe; */
		pPicture->eChromaType          = BVDC_P_ChromaType_eAuto;
	}
	else if(BVDC_P_SRC_IS_ANALOG(hSource->eId) ||
	        BVDC_P_SRC_IS_ITU656(hSource->eId))
	{
		/* set the values for analog video picture */
		pPicture->stFlags.bMuteFixedColor = false;
		pPicture->eSrcPolarity            = eFieldId;
		pPicture->PicComRulInfo.eSrcOrigPolarity = eFieldId;
		pPicture->eSrcOrientation                = BFMT_Orientation_e2D;
		/* vdec source has no way to identify the real field repeat from polarity
		   repeat, so we simply disable the field repeat detection. */
		pPicture->stFlags.bPictureRepeatFlag    = false;

		/* Determined eMatrixCoefficients for analog source. */
		pPicture->eMatrixCoefficients  =
			VIDEO_FORMAT_27Mhz(hSource->stCurInfo.pFmtInfo->ulPxlFreqMask)
			? BAVC_MatrixCoefficients_eSmpte_170M
			: BAVC_MatrixCoefficients_eItu_R_BT_709;

		/* we have no way to know the original source content's chroma type; */
		pPicture->eChromaType          = BVDC_P_ChromaType_eAuto;
	}
	else if(BVDC_P_SRC_IS_DS(hSource->eId))
	{
		pPicture->eSrcPolarity         = eFieldId;
		pPicture->PicComRulInfo.eSrcOrigPolarity = eFieldId;
		pPicture->eSrcOrientation                = BFMT_Orientation_e2D;
	}
	else if (BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		BVDC_P_SurfaceInfo  *pCurSur;

		pCurSur = &hSource->hVfdFeeder->stGfxSurface.stCurSurInfo;
		pPicture->ePixelFormat = pCurSur->eInputPxlFmt;
		pPicture->hSurface = pCurSur->stAvcPic.hSurface;
		pPicture->hSurface_R = pCurSur->stAvcPic.hRSurface;
		pPicture->eSrcOrientation = pCurSur->stAvcPic.eInOrientation;
		pPicture->ulOrigPTS = pCurSur->stAvcPic.ulOrigPTS;

		pPicture->stFlags.bMute = (0 == pCurSur->ulAddress);
		pPicture->stFlags.bMuteFixedColor = pPicture->stFlags.bMute;
#if 0
		if (pCurSur->ulHeight == hWindow->stAdjSclOut.ulHeight)
		{
			/* no need to vertically scale */
			pPicture->eSrcPolarity = eFieldId; /* equal to display polarity */
		}
		else
		{
			/* need vertically scale */
			pPicture->eSrcPolarity = BAVC_Polarity_eFrame;
		}
#else
		/* it looks better if we always use vertical FIR? */
		pPicture->eSrcPolarity = BAVC_Polarity_eFrame;
#endif
		pPicture->PicComRulInfo.eSrcOrigPolarity = pPicture->eSrcPolarity;

		pPicture->eMatrixCoefficients = hSource->eMatrixCoefficients;
		pPicture->hBuffer = hWindow->hBuffer;
		/*hSource->bPictureChanged = false;*/
	}

	/* Set eDispOrientation and e3dSrcBufSel for 3d */
	pPicture->e3dSrcBufSel = hWindow->hCompositor->hDisplay->stCurInfo.e3dSrcBufSel;
	if(BFMT_IS_3D_MODE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
		pPicture->eDispOrientation = hWindow->hCompositor->stCurInfo.pFmtInfo->eOrientation;
	else
		pPicture->eDispOrientation = hWindow->hCompositor->stCurInfo.eOrientation;

	/* Source Frame rate */
	pPicture->eFrameRateCode = hSource->eFrameRateCode;

	/* (2) Determine the VNET mode and properly assign the rectangles according
	 * to the modes.  If there are source_size changes or panscan vectors changes. */
	pPicture->stVnetMode = hWindow->stVnetMode;
#if BVDC_P_SUPPORT_MCVP
	pPicture->stVnetMode.stBits.bUseMad = hWindow->stMcvpMode.stBits.bUseMad;
	pPicture->stVnetMode.stBits.bUseAnr = hWindow->stMcvpMode.stBits.bUseAnr;
	pPicture->stVnetMode.stBits.bUseMvpBypass = hWindow->stMcvpMode.stBits.bUseMvpBypass;
#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	pPicture->stVnetMode.stBits.bUseHscl = hWindow->stMcvpMode.stBits.bUseHscl;
#endif
#if BVDC_P_SUPPORT_MANR
	pPicture->stVnetMode.stBits.bUseAnr = hWindow->stMcvpMode.stBits.bUseAnr;
#endif
#endif

	/* (3) copy scale factor to pPicture */
	pPicture->ulNrmHrzSrcStep = hWindow->ulNrmHrzSrcStep;
	pPicture->ulNrmVrtSrcStep = hWindow->ulNrmVrtSrcStep;
	pPicture->ulNonlinearSrcWidth = hWindow->ulNonlinearSrcWidth;
	pPicture->ulNonlinearSclOutWidth = hWindow->ulNonlinearSclOutWidth;
	pPicture->ulCentralRegionSclOutWidth = hWindow->ulCentralRegionSclOutWidth;
	bNonlinearSclMode = (0 != pPicture->ulNonlinearSclOutWidth);

	/* (4) Setup scaler, capture and the following video feeder. Further src clipping
	 * after scan-out-clip is performed here.  Notice that user's rectangles are
	 * updated by BVDC_P_Window_AdjustRectangles_isr().  For a majority of time if
	 * there are no user's changes nor source's stream changes, the rectangles will
	 * stay the same.
	 *
	 * Clip algorithm:
	 *    Both scan (i.e. mpeg feeder), capture, video feeder, and compositor
	 * could perform clip. However, only scaler can clip to sub-pixel level.
	 * Mad will not perform any cut.
	 *    Basing on the order defined by vnet mode, we do clip as early as
	 * possible.
	 *    In the case that the previous modules could not perform the needed clip
	 * completely, the following modules clip the rest. For example, scan might
	 * not be able to do clip completely becase another display share the mpeg
	 * source, then the following module should complete the clipping. Another
	 * example is that capture and feeder can only clip to pixel boundary, sub-
	 * pixel clipping should be done by the following scaler.
	 *    Clip in mpeg feeder is specified by pSrcOut, clip in scaler is specified
	 * by stSclCut, clip in capture is specified by pCapOut, clip in feeder is
	 * specified by pVfdOut, and clip in compositor is specified by pWinIn. pSrcOut,
	 * pSclIn, pCapIn, pVfdIn only specify the input width and height. */
	pPicture->stSrcOut = hSource->stScanOut;
	pPicture->stSclOut = hWindow->stAdjSclOut;
	pPicture->stWinOut = hWindow->stAdjDstRect;

	/* sclOut width needs to be aligned to multiple of 2 if either SCL or
	 * BVN is 4:2:2.
	 * note a: srcCnt ulWidth and ulHeight are only used to decide feed/cap/
	 * sclIn size, and they are always aligned due to HW limitation and aligned
	 * up to avoid video info loose. The pixel + subpixel accuracy is passed to
	 * SCL by lLeft/lTop for phase and ulNrmHrzSrcStep/ulNrmVrtSrcStep for src
	 * step. SCL FIR will patch if it runs out of src pixels; And extra pixels
	 * output from SCL will be clipped by CAP and/or CMP.
	 * Note b: dstRect does not really needs align. However, currently we does
	 * not honor odd dstRect height or top in interlaced display mode, nor
	 * odd sclOut height or top. Indeed we are doing BVDC_P_DIV_ROUND_DN for
	 * dstRect/sclOut top and height in interlaced display mode. If we want to
	 * honor odd top and/or height, we need both SCL, SetSurfaceSize_isr and
	 * SetDisplaySize_isr to coorperate and to align-up the heights here.
	 * note c: srcCnt left/top can not be aligned here, its sub-pixel postion is
	 * used in SCL. MFD ScanOut and sclCut vertical and horizontal intervals
	 * will be aligned in Soure and Scaler respectively. srcCnt width/height
	 * alignment is not needed here, and it might even force the final align of MFD
	 * ScanOut, sclCut, CapOut to unnecessary bigger.
	 * ---------------------------------------------------------------------- */
	pPicture->stSclOut.ulWidth = BVDC_P_ALIGN_UP(pPicture->stSclOut.ulWidth, 2);
#if (BVDC_P_DCX_HSIZE_WORKAROUND)
	if((pUserInfo->stCapTestFeature1.bEnable ||
		pUserInfo->stMadSettings.stTestFeature1.bEnable ||
		pUserInfo->stAnrSettings.stVideoTestFeature1.bEnable) &&
	   (BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode)))
	{
		pPicture->stSclOut.ulWidth = BVDC_P_ALIGN_UP(pPicture->stSclOut.ulWidth, 4);
	}
#endif

	lCntLeft = hWindow->stSrcCnt.lLeft - (hSource->stScanOut.lLeft<<BVDC_P_16TH_PIXEL_SHIFT);
	lCntLeft_R = hWindow->stSrcCnt.lLeft_R - (hSource->stScanOut.lLeft_R<<BVDC_P_16TH_PIXEL_SHIFT);
	lCntTop  = hWindow->stSrcCnt.lTop  - (hSource->stScanOut.lTop <<BVDC_P_16TH_PIXEL_SHIFT);
	ulCntWidth  = hWindow->stSrcCnt.ulWidth;
	ulCntHeight = hWindow->stSrcCnt.ulHeight;

	pPicture->pDnrIn  = NULL;
	pPicture->pAnrIn  = NULL;
	pPicture->pMadIn  = NULL;
	pPicture->pSclIn  = NULL;
	pPicture->pCapIn  = NULL;
	pPicture->pVfdIn  = NULL;
	pPicture->pFgtIn  = NULL;


	/* Connecting the rect based on vnet */

#if BVDC_P_SUPPORT_MCVP
	if(BVDC_P_VNET_USED_MCVP(pPicture->stVnetMode) ||
#else
	if(BVDC_P_VNET_USED_HSCL(pPicture->stVnetMode) ||
#endif
	   BVDC_P_VNET_USED_MAD(pPicture->stVnetMode)  ||
	   BVDC_P_VNET_USED_ANR(pPicture->stVnetMode))
	{
		/* init as pass through */
		pPicture->pHsclIn = pPicture->pSrcOut;
		pPicture->pHsclOut= pPicture->pSrcOut;
		pPicture->ulHsclNrmHrzSrcStep = (1 << BVDC_P_NRM_SRC_STEP_F_BITS); /* unit scale by default */
		pPicture->lHsclCutLeft = 0; /* no src clip in hscl by default */
		pPicture->lHsclCutLeft_R = 0; /* no src clip in hscl by default */
		pPicture->ulHsclCutWidth = pPicture->pHsclIn->ulWidth;
	}

	if(BVDC_P_VNET_USED_ANR(pPicture->stVnetMode))
	{
		pPicture->pAnrIn = pPicture->pSrcOut;
		pPicture->pAnrOut= pPicture->pSrcOut;
	}

	if(BVDC_P_VNET_USED_MAD(pPicture->stVnetMode))
	{
		pPicture->pMadIn = pPicture->pSrcOut;
		pPicture->pMadOut= pPicture->pSrcOut;
	}

	pPicture->pWinIn = pPicture->pSclOut;
	if(BVDC_P_VNET_USED_CAPTURE(pPicture->stVnetMode))
	{
		if(BVDC_P_VNET_USED_SCALER_AT_WRITER(pPicture->stVnetMode))
		{
			/* scaler is used before capture, scaler will do clip.
			 * note: scaler horizontal/vertical subpixel pan-scan vectors
			 * are in S11.6, S11.14 fixed point format respectively. */
			pPicture->stSclCut.lLeft = lCntLeft << 2;  /* 6: SCL_0_HORIZ_*_SRC_PIC_OFFSET */
			pPicture->stSclCut.lLeft_R = lCntLeft_R << 2;  /* 6: SCL_0_HORIZ_*_SRC_PIC_OFFSET */
			pPicture->stSclCut.lTop  = lCntTop  << 10; /* 14: SCL_0_VERT_*_SRC_PIC_OFFSET */
			pPicture->stSclCut.ulWidth  = ulCntWidth;
			pPicture->stSclCut.ulHeight = ulCntHeight; /* clip in scaler */

			pPicture->pSclIn = pPicture->pSrcOut;

			if(pUserInfo->bMosaicMode)
			{
				/* cut done in scl, no more in cap */
				pPicture->stCapOut.lLeft = 0;
				pPicture->stCapOut.lLeft_R = 0;
				pPicture->stCapOut.lTop  = 0;
				pPicture->stCapOut.ulWidth  = pPicture->stSclOut.ulWidth;
				pPicture->stCapOut.ulHeight = pPicture->stSclOut.ulHeight;
				pPicture->pCapIn = &pPicture->stCapOut;
				pPicture->pCapOut= pPicture->pCapIn;

				/* stVfdOut is not used in this case; so borrow it here; */
				pPicture->stVfdOut.lLeft = 0;
				pPicture->stVfdOut.lLeft_R = 0;
				pPicture->stVfdOut.lTop  = 0;
				pPicture->stVfdOut.ulWidth  = pUserInfo->stScalerOutput.ulWidth;
				pPicture->stVfdOut.ulHeight = pUserInfo->stScalerOutput.ulHeight;
				pPicture->pVfdIn = &pPicture->stVfdOut;
				pPicture->pVfdOut= pPicture->pVfdIn;
				pPicture->pWinIn = &hWindow->stCurInfo.stScalerOutput;
			}
			else
			{
				uint32_t  ulCapWidth, ulCapHeight;
				uint32_t  ulNewSclOutHeight, ulMoreHeightCut;
				const BFMT_VideoInfo *pDstFmtInfo;

				BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
				pDstFmtInfo = hWindow->hCompositor->stCurInfo.pFmtInfo;

				/* typical cut is already done in scl, but dstCut might have NOT
				 * been completely xfered to src cut, and scale factor rounding up
				 * might cause more sclOut edge, ok to set sclOut size a little big */
				pPicture->stCapOut.lTop = BVDC_P_ALIGN_DN(pPicture->stSclOut.lTop, 2);
				ulCapHeight = hWindow->stAdjDstRect.ulHeight +
					pPicture->stSclOut.lTop - pPicture->stCapOut.lTop;
				pPicture->stCapOut.ulHeight =
					BVDC_P_MIN(pDstFmtInfo->ulHeight, BVDC_P_ALIGN_UP(ulCapHeight, 2));

				pPicture->stCapOut.lLeft = BVDC_P_ALIGN_DN(pPicture->stSclOut.lLeft, 2);
				pPicture->stCapOut.lLeft_R = BVDC_P_ALIGN_DN(pPicture->stSclOut.lLeft_R, 2);
				ulCapWidth = hWindow->stAdjDstRect.ulWidth +
					pPicture->stSclOut.lLeft - pPicture->stCapOut.lLeft;

#if (BVDC_P_DCX_HSIZE_WORKAROUND || BVDC_P_MADR_HSIZE_WORKAROUND)
				if(pUserInfo->stCapTestFeature1.bEnable
#if (BVDC_P_MADR_HSIZE_WORKAROUND)
				|| pUserInfo->bDeinterlace
#endif
				)
				{
					pPicture->stCapOut.ulWidth = BVDC_P_ALIGN_UP(
						BVDC_P_MIN(pDstFmtInfo->ulWidth, ulCapWidth), 4);
				}
				else
#endif
				{
					pPicture->stCapOut.ulWidth =
						BVDC_P_MIN(pDstFmtInfo->ulWidth, BVDC_P_ALIGN_UP(ulCapWidth, 2));
				}

				pPicture->stSclOut.ulWidth =
					pPicture->stCapOut.ulWidth + pPicture->stCapOut.lLeft;
				ulNewSclOutHeight =
					pPicture->stCapOut.ulHeight + pPicture->stCapOut.lTop;
#if (BVDC_P_WIN_MOSAIC_MODE_BVN_WORKAROUND)
				/* avoid cap intr fires before scl input completes*/
				if (pPicture->stSclOut.ulHeight > ulNewSclOutHeight)
				{
					ulMoreHeightCut =
						((pPicture->stSclOut.ulHeight - ulNewSclOutHeight) *
						 hWindow->ulNrmVrtSrcStep) >> BVDC_P_NRM_SRC_STEP_F_BITS;
					pPicture->stSclCut.ulHeight -= ulMoreHeightCut;
				}
#endif
				pPicture->stSclOut.ulHeight = ulNewSclOutHeight;

				pPicture->stVfdOut = pPicture->stCapOut;
				pPicture->stVfdOut.lLeft = 0; /* vfd code still honor crop */
				pPicture->stVfdOut.lLeft_R = 0; /* vfd code still honor crop */
				pPicture->stVfdOut.lTop  = 0;
				pPicture->stWinIn  = pPicture->stVfdOut;

				pPicture->stWinIn.lLeft = pPicture->stSclOut.lLeft - pPicture->stCapOut.lLeft;
				pPicture->stWinIn.lLeft_R = pPicture->stSclOut.lLeft_R - pPicture->stCapOut.lLeft_R;
				pPicture->stWinOut.ulWidth = BVDC_P_MIN(pPicture->stWinOut.ulWidth,
					pPicture->stWinIn.ulWidth -	pPicture->stWinIn.lLeft);

				pPicture->pCapIn = pPicture->pSclOut;
				pPicture->pCapOut= &pPicture->stCapOut;
				pPicture->pVfdIn = pPicture->pCapOut;
				pPicture->pVfdOut= &pPicture->stVfdOut;
				pPicture->pWinIn = &pPicture->stWinIn;
			}
		}
		else /* (scaler not used) || (scaler used in reader) */
		{
			int32_t  lCapVfdCutTop;

			BDBG_ASSERT(!pUserInfo->bMosaicMode);

			/* if capture before (or without) scaling, capturer clips to integer location,
			 * and scaler further cut sub-pixel */
			if(BVDC_P_VNET_USED_SCALER_AT_READER(pPicture->stVnetMode))
			{
				uint32_t  ulAlignUnit;

				/* align left/top to floor and right/bottom to ceil to avoid losing video
				 * info. SCL after CAP/VFD will clip to up to sub-pixel position.
				 * note a: some scl, vfd/cap, anr and mad might still use 4:2:2 format even if
				 * the chip BVN does support 444 format, so we just align for 4:2:2 for simple.
				 * Note b: BVDC_P_16TH_PIXEL_SHIFT = 4 */
				BVDC_P_Window_AlignPreMadRect_isr(lCntLeft, lCntLeft_R,
					ulCntWidth, pPicture->stSrcOut.ulWidth,
					4, 2, &(pPicture->stCapOut.lLeft), &(pPicture->stCapOut.lLeft_R),
					&(pPicture->stCapOut.ulWidth));
				ulAlignUnit = (BAVC_Polarity_eFrame == pPicture->eSrcPolarity)? 1 : 2;
				BVDC_P_Window_AlignPreMadRect_isr(lCntTop, lCntTop,
					ulCntHeight, pPicture->stSrcOut.ulHeight,
					4, ulAlignUnit, &(pPicture->stCapOut.lTop),
					NULL, &(pPicture->stCapOut.ulHeight));

#if (BVDC_P_DCX_HSIZE_WORKAROUND || BVDC_P_MADR_HSIZE_WORKAROUND)
				if(pUserInfo->stCapTestFeature1.bEnable ||
#if (BVDC_P_MADR_HSIZE_WORKAROUND)
				   pUserInfo->bDeinterlace ||
#endif
				   pUserInfo->stMadSettings.stTestFeature1.bEnable ||
				   pUserInfo->stAnrSettings.stVideoTestFeature1.bEnable)
				{
					uint32_t  lCapLeft, lCapLeft_R, ulCapWidth;
					pPicture->pSrcOut->ulWidth = BVDC_P_ALIGN_UP(pPicture->pSrcOut->ulWidth, 4);
					lCapLeft = BVDC_P_ALIGN_DN(pPicture->stCapOut.lLeft, 4);
					lCapLeft_R = BVDC_P_ALIGN_DN(pPicture->stCapOut.lLeft_R, 4);
					ulCapWidth = pPicture->stCapOut.ulWidth +
						(pPicture->stCapOut.lLeft - lCapLeft);
					pPicture->stCapOut.lLeft = lCapLeft;
					pPicture->stCapOut.lLeft_R = lCapLeft_R;
					pPicture->stCapOut.ulWidth = BVDC_P_ALIGN_UP(ulCapWidth, 4);
				}
#endif

#if (BVDC_P_MADR_PICSIZE_WORKAROUND)
				if(pUserInfo->bDeinterlace && BVDC_P_VNET_USED_MCVP(hWindow->stVnetMode))
				{
					uint32_t  ulNewWidth;
					BVDC_P_Window_PreMadAdjustWidth_isr(pPicture->stCapOut.ulWidth,
						(BAVC_Polarity_eFrame == pPicture->PicComRulInfo.eSrcOrigPolarity)?
						pPicture->stCapOut.ulHeight: pPicture->stCapOut.ulHeight / 2,
						BVDC_P_MADR_DCXS_COMPRESSION(pUserInfo->stMadSettings.stTestFeature1.ulBitsPerPixel),
						&ulNewWidth);

					pPicture->pSrcOut->ulWidth += ulNewWidth - pPicture->stCapOut.ulWidth;
					pPicture->stCapOut.ulWidth = ulNewWidth;
				}
#endif

			}
			else /* scl is not used */
			{
				pPicture->stCapOut.lLeft = lCntLeft >> BVDC_P_16TH_PIXEL_SHIFT;
				pPicture->stCapOut.lLeft_R = lCntLeft_R >> BVDC_P_16TH_PIXEL_SHIFT;
				pPicture->stCapOut.lTop  = lCntTop  >> BVDC_P_16TH_PIXEL_SHIFT;
				pPicture->stCapOut.ulWidth  = ulCntWidth;
				pPicture->stCapOut.ulHeight = ulCntHeight;
			}

			lCapVfdCutTop = pPicture->stCapOut.lTop;
			pPicture->stVfdOut = pPicture->stCapOut;
			pPicture->stVfdOut.lLeft = 0;
			pPicture->stVfdOut.lLeft_R = 0;
#if (BVDC_FORCE_VFD_TOP_CLIP)
			if(pUserInfo->stCapTestFeature1.bEnable ||
			   pUserInfo->stMadSettings.stTestFeature1.bEnable ||
			   pUserInfo->stAnrSettings.stVideoTestFeature1.bEnable)
			{
				/* don't clip in vfd for compression */
				pPicture->stVfdOut.lTop = 0;
			}
			else
			{
				/* switch vrt cut from cap to vfd */
				pPicture->stCapOut.lTop = 0;
				pPicture->stCapOut.ulHeight = pPicture->pSrcOut->ulHeight;
			}
#else
			pPicture->stVfdOut.lTop  = 0;
#endif
			pPicture->pCapIn = pPicture->pSrcOut;
			pPicture->pCapOut= &pPicture->stCapOut; /* clip */
			pPicture->pVfdIn = &pPicture->stVfdOut;
			pPicture->pVfdOut= pPicture->pVfdIn;
			/* Feed out const color for HD source on bypass window */
			if( hWindow->bMuteBypass )
			{
				pPicture->pVfdOut= &pPicture->stWinOut;
			}

			if(BVDC_P_VNET_USED_SCALER_AT_READER(pPicture->stVnetMode))
			{
				/* scaler will do scaling and do sub-pixel clip */
				/* scaler horizontal/vertical subpixel pan-scan vectors
				 * are in S11.6, S11.14 fixed point format respectively. */
				pPicture->stSclCut.lLeft = (lCntLeft -
					(pPicture->stCapOut.lLeft << BVDC_P_16TH_PIXEL_SHIFT)) << 2;
				pPicture->stSclCut.lLeft_R = (lCntLeft_R -
					(pPicture->stCapOut.lLeft_R << BVDC_P_16TH_PIXEL_SHIFT)) << 2;
				pPicture->stSclCut.lTop  = (lCntTop  -
					(lCapVfdCutTop  << BVDC_P_16TH_PIXEL_SHIFT)) << 10;
				pPicture->stSclCut.ulWidth  = ulCntWidth;
				pPicture->stSclCut.ulHeight = ulCntHeight;

				pPicture->pSclIn = pPicture->pVfdOut;

				/* before this point pPicture->pVfdOut is not decided yet */
				if(pPicture->stVnetMode.stBits.bUseCap &&
#if BVDC_P_SUPPORT_MCVP
				   pPicture->stVnetMode.stBits.bUseMcvp &&
#else
				    pPicture->stVnetMode.stBits.bUseHscl &&
#endif
				   !pPicture->stVnetMode.stBits.bSclBeforeCap)
				{
					pPicture->pHsclIn = pPicture->pVfdOut;
					pPicture->pHsclOut= pPicture->pVfdOut;
					pPicture->ulHsclCutWidth = pPicture->pHsclIn->ulWidth;
				}

				if(pPicture->stVnetMode.stBits.bUseCap &&
				   pPicture->stVnetMode.stBits.bUseAnr &&
				  !pPicture->stVnetMode.stBits.bSclBeforeCap)
				{
					pPicture->pAnrIn = pPicture->pVfdOut;
					pPicture->pAnrOut= pPicture->pVfdOut;
				}

				if(pPicture->stVnetMode.stBits.bUseCap &&
				   pPicture->stVnetMode.stBits.bUseMad &&
				   !pPicture->stVnetMode.stBits.bSclBeforeCap)
				{
					pPicture->pMadIn = pPicture->pVfdOut;
					pPicture->pMadOut= pPicture->pVfdOut;
				}
			}
		}
	}
	else if(BVDC_P_VNET_USED_SCALER(pPicture->stVnetMode))
	{
		/* scaler is used before capture, scaler will do clip.
		 * the following capture and the following feeder don't clip */
		/* Scaler horizontal/vertical subpixel pan-scan vectors
		 * are in S11.6, S11.14 fixed point format respectively. */
		pPicture->stSclCut.lLeft = lCntLeft << 2;  /* 6: SCL_0_HORIZ_*_SRC_PIC_OFFSET */
		pPicture->stSclCut.lLeft_R = lCntLeft_R << 2;  /* 6: SCL_0_HORIZ_*_SRC_PIC_OFFSET */
		pPicture->stSclCut.lTop  = lCntTop  << 10; /* 14: SCL_0_VERT_*_SRC_PIC_OFFSET */
		pPicture->stSclCut.ulWidth  = ulCntWidth;
		pPicture->stSclCut.ulHeight = ulCntHeight;
		pPicture->pSclIn = pPicture->pSrcOut;
	}

	if (BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		pPicture->pVfdIn = pPicture->pSrcOut;
		pPicture->pVfdOut = pPicture->pVfdIn;
	}

#if (BDBG_DEBUG_BUILD)
	if((BVDC_P_VNET_USED_SCALER(pPicture->stVnetMode)) &&
	   ((pPicture->stSclCut.lLeft < 0) || (pPicture->stSclCut.lTop < 0)))
	{
		BDBG_MSG(("neg sclCut left %d or top %d",
			pPicture->stSclCut.lLeft, pPicture->stSclCut.lTop));
	}
#endif

	if(BVDC_P_VNET_USED_MAD(pPicture->stVnetMode))
	{
#if (BVDC_P_SUPPORT_3D_VIDEO)
		bool      bMadIs3dLR = false;
#endif
		uint32_t  ulHsclSrcHrzSclThr, ulMaxMadWidth;

		BDBG_ASSERT(hWindow->stCurResource.hMad32 || hWindow->stCurResource.hMcvp);
		if (hWindow->stCurResource.hMad32)
		{
			ulMaxMadWidth = hWindow->stCurResource.hMad32->ulMaxWidth;
		}
		else
		{
			ulMaxMadWidth = hWindow->stCurResource.hMcvp->ulMaxWidth;
		}

#if (BVDC_P_SUPPORT_3D_VIDEO)
		/* ulMaxMadWidth is 960 when MCVP is in 3D */
		/* BVDC_P_VNET_USED_MCVP_AT_WRITER = BVDC_P_VNET_USED_MAD_AT_WRITER,
		 * BVDC_P_VNET_USED_MCVP_AT_READER = BVDC_P_VNET_USED_MAD_AT_READER
		 * if BVDC_P_SUPPORT_3D_VIDEO
		 */
		if((BVDC_P_VNET_USED_MCVP_AT_WRITER(pPicture->stVnetMode) &&
		    (pPicture->eSrcOrientation != BFMT_Orientation_e2D)) ||
		    (BVDC_P_VNET_USED_MCVP_AT_READER(pPicture->stVnetMode) &&
		    (pPicture->eDispOrientation != BFMT_Orientation_e2D)))
		{
			bMadIs3dLR = true;
		}

		if(bMadIs3dLR)
			ulMaxMadWidth = ulMaxMadWidth / 2;
#endif

#ifndef HSCL_TUNE_THRESHOLD
		ulHsclSrcHrzSclThr = BVDC_P_MAD_SRC_HORZ_THRESHOLD;
#else
		ulHsclSrcHrzSclThr = BREG_Read32(hSource->hVdc->hRegister, BCHP_HSCL_0_SCRATCH_0);
#endif
		ulHsclSrcHrzSclThr = BVDC_P_MIN(ulHsclSrcHrzSclThr, ulMaxMadWidth);
		if (ulHsclSrcHrzSclThr < pPicture->pMadIn->ulWidth)
		{
			BVDC_P_Window_TryXferHrzSclToHscl_isr(hWindow, pPicture, ulHsclSrcHrzSclThr);
		}
	}

	if(BVDC_P_VNET_USED_DNR(pPicture->stVnetMode))
	{
		pPicture->pDnrIn = pPicture->pSrcOut;
		pPicture->pDnrOut= pPicture->pSrcOut;
	}

	if(BVDC_P_VNET_USED_FGT(pPicture->stVnetMode))
	{
		pPicture->pFgtIn = pPicture->pWinIn;
		pPicture->pFgtOut= pPicture->pWinIn;
	}

#if (BVDC_P_WIN_MOSAIC_MODE_BVN_WORKAROUND)
	if(hSource->stCurInfo.bMosaicMode)
	{
		if(pPicture->pCapIn)
		{
			pPicture->pCapIn->ulWidth = BVDC_P_MAX(pPicture->pCapIn->ulWidth,
				BVDC_P_WIN_MOSAIC_OUTPUT_H_MIN);
			pPicture->pCapIn->ulHeight = BVDC_P_MAX(pPicture->pCapIn->ulHeight,
				BVDC_P_WIN_MOSAIC_OUTPUT_V_MIN);
		}

		if(pPicture->pSclOut)
		{
			pPicture->pSclOut->ulWidth = BVDC_P_MAX(pPicture->pSclOut->ulWidth,
				BVDC_P_WIN_MOSAIC_OUTPUT_H_MIN);
			pPicture->pSclOut->ulHeight = BVDC_P_MAX(pPicture->pSclOut->ulHeight,
				BVDC_P_WIN_MOSAIC_OUTPUT_V_MIN);
		}
	}
#endif

	/* If rectangle computation results in limit violation, override the */
	/* limit-violating rectangle sizes with HW limits */
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pCapIn,   "CAP", BVDC_P_WIN_CAP_INPUT_H_MIN,  BVDC_P_WIN_CAP_INPUT_V_MIN, hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced);
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pVfdOut,  "VFD", BVDC_P_WIN_VFD_OUTPUT_H_MIN, BVDC_P_WIN_VFD_OUTPUT_V_MIN, hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_SCALER_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(&pPicture->stSclCut,  "SCLCUT", BVDC_P_WIN_VFD_OUTPUT_H_MIN, BVDC_P_WIN_VFD_OUTPUT_V_MIN, bInterlaced);
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pSclOut,  "SCL", BVDC_P_WIN_SCL_OUTPUT_H_MIN, BVDC_P_WIN_SCL_OUTPUT_V_MIN, bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_HSCL_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pHsclOut, "HCL", BVDC_P_WIN_SCL_OUTPUT_H_MIN, BVDC_P_WIN_SCL_OUTPUT_V_MIN, bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_MAD_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pMadIn,   "MAD", BVDC_P_WIN_MAD_INPUT_H_MIN,  BVDC_P_WIN_MAD_INPUT_V_MIN, bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_DNR_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pDnrIn,   "DNR", BVDC_P_WIN_DNR_INPUT_H_MIN,  BVDC_P_WIN_DNR_INPUT_V_MIN, bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_ANR_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pAnrIn,   "ANR", BVDC_P_WIN_ANR_INPUT_H_MIN,  BVDC_P_WIN_ANR_INPUT_V_MIN, bInterlaced);

	bInterlaced = BVDC_P_VNET_USED_FGT_AT_READER(pPicture->stVnetMode) ?
		hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced :
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
	BVDC_P_Window_EnforceMinSizeLimit_isr(pPicture->pFgtIn,   "FGT", BVDC_P_WIN_FGT_INPUT_H_MIN,  BVDC_P_WIN_FGT_INPUT_V_MIN, bInterlaced);

	hWindow->ulNrmHrzSrcStep = BVDC_P_CAL_HRZ_SRC_STEP(
			hWindow->stSrcCnt.ulWidth, pPicture->stSclOut.ulWidth);

	hWindow->ulNrmVrtSrcStep = BVDC_P_CAL_VRT_SRC_STEP(
		hWindow->stSrcCnt.ulHeight, pPicture->stSclOut.ulHeight);

	if(hWindow->stCurInfo.stDirty.stBits.bRecAdjust)
	{
		BVDC_P_Window_DumpRects_isr(hWindow, pPicture);
		/*Update the delay VSYNC for seamless transcode*/

		hWindow->stCurInfo.stDirty.stBits.bRecAdjust = BVDC_P_CLEAN;

		/* Set up dirty bit for callback */
		if((hWindow->stCallbackData.stOutputRect.lLeft    != pPicture->pWinOut->lLeft) ||
		   (hWindow->stCallbackData.stOutputRect.lTop     != pPicture->pWinOut->lTop) ||
		   (hWindow->stCallbackData.stOutputRect.ulWidth  != pPicture->pWinOut->ulWidth) ||
		   (hWindow->stCallbackData.stOutputRect.ulHeight != pPicture->pWinOut->ulHeight))
		{
			hWindow->stCallbackData.stOutputRect.lLeft    = pPicture->pWinOut->lLeft;
			hWindow->stCallbackData.stOutputRect.lTop     = pPicture->pWinOut->lTop;
			hWindow->stCallbackData.stOutputRect.ulWidth  = pPicture->pWinOut->ulWidth;
			hWindow->stCallbackData.stOutputRect.ulHeight = pPicture->pWinOut->ulHeight;
			hWindow->stCallbackData.stMask.bRectAdjust = BVDC_P_DIRTY;
		}
	}

#if (BVDC_P_SUPPORT_STG)
	if((BVDC_P_SRC_IS_MPEG(hSource->eId)) && (pMvdFieldData))
		BVDC_P_Window_UpdateBarData_isr(hWindow, pPicture, pMvdFieldData);
#endif
	/* Update video TestFeature1 */
	pPicture->stCapTestFeature1.bEnable = pUserInfo->stCapTestFeature1.bEnable;
	pPicture->stCapTestFeature1.ulBitsPerPixel = pUserInfo->stCapTestFeature1.ulBitsPerPixel;

	BDBG_LEAVE(BVDC_P_Window_UpdateSrcAndUserInfo_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 * Partial decision on the vnet mode and cap buf count, according to user's
 * setting only.  Further and complete decision are done later with
 * combination of both user settings and dynamic src info.
 */
static void BVDC_P_Window_DecideCapture
	( BVDC_Window_Handle               hWindow,
	  BVDC_Source_Handle               hSource,
	  const BVDC_Compositor_Handle     hCompositor )
{
	uint32_t ulBufCntNeeded, ulBufDelay;
	const BFMT_VideoInfo *pSrcFmtInfo;
	const BVDC_P_Window_Info *pWinInfo;
	const BVDC_P_Compositor_Info *pCmpInfo;
	const BVDC_P_Source_Info *pSrcInfo;
	bool bEnableCaptureByMosaicOrPsf = false, bProgressivePullDown=false;
	bool bCapture, bEnableCaptureByDelay;
	BVDC_P_WrRateCode eWriterVsReaderRateCode, eReaderVsWriterRateCode;

	BDBG_ENTER(BVDC_P_Window_DecideCapture);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Not intended for gfx windows. */
	BDBG_ASSERT(BVDC_P_SRC_IS_VIDEO(hSource->eId));

	ulBufCntNeeded = BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT;

	/* Save some typing */
	pWinInfo = &hWindow->stNewInfo;
	pCmpInfo = &hCompositor->stNewInfo;
	pSrcInfo = &hSource->stNewInfo;
	pSrcFmtInfo = ((hSource->stNewInfo.bAutoFmtDetection) &&
				   (hSource->stCurInfo.bAutoFmtDetection))?
		hSource->stCurInfo.pFmtInfo : hSource->stNewInfo.pFmtInfo;

	/* Full screen is consider as display destination scale to greater
	 * or equal to 75% of full screen.  In addition it must not be ARC
	 * that uses all source, because we'll downscale possibly below 75%, and
	 * that would required capture.
	 *
	 * Why >= width-3?  This is to allow app to pass in 480 and interpret it
	 * as full screen.  Basically we want to have the lagacy 480 and 482 lines
	 * for 480i, 483 for 480p to be similar. */
	hWindow->bDstFullScreen =
		(((pWinInfo->stScalerOutput.ulWidth  == pCmpInfo->pFmtInfo->ulWidth) &&
		  (pWinInfo->stScalerOutput.ulHeight >= pCmpInfo->pFmtInfo->ulDigitalHeight)) ||
		 ((pWinInfo->stScalerOutput.ulWidth  >= pCmpInfo->pFmtInfo->ulWidth * 3 / 4) &&
		  (pWinInfo->stScalerOutput.ulHeight >= pCmpInfo->pFmtInfo->ulDigitalHeight * 3 / 4)&&
		  (BVDC_AspectRatioMode_eUseAllSource != pWinInfo->eAspectRatioMode)));

	if(BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		/* Defer actual determniation until mpeg_data_isr (defer).  Or last state. */

		/* bCapture do we need to capture?
		 * TODO: this logic will be moved to _isr to support _SetXyzRect_isr; */
		bCapture = (
#if (BVDC_P_AUTO_ENABLE_CAPTURE)
			!hWindow->bDstFullScreen        ||
#endif
			!hWindow->bSyncLockSrc          ||
			 pWinInfo->bForceCapture        ||
			 pWinInfo->bBoxDetect           ||
			 hWindow->uiAvailCaptureBuffers ||
			/* In the case of HD-SD simul or single source multiple windows!
			 * We'll turn on capture for these addtional cases to relieve bandwidth:
			 *   (1) Vertical Cropping: AllDestinataion, PanScan, SrcClip
			 *   (2) Locked on non cmp0. */
			((hSource->stNewInfo.ulWindows > 1) &&
			 ((BVDC_AspectRatioMode_eUseAllDestination == pWinInfo->eAspectRatioMode) ||
			  (BVDC_PanScanType_eDisable != pWinInfo->ePanScanType) ||
			  (pWinInfo->stSrcClip.ulTop + pWinInfo->stSrcClip.ulBottom))));


		/* NRT mode shouldn't capture otherwise EOP trigger might come faster than capture side can sustain;
		   upper layer should not add vsync delay buffer either for NRT window; plus NRT window must be sync-locked; */
#if (BVDC_P_SUPPORT_STG)
		bCapture = bCapture && !hCompositor->hDisplay->stNewInfo.bStgNonRealTime;
#endif
		/* Calculate ulBufDelay */
		if( bCapture )
		{
			ulBufDelay = (hWindow->bSyncLockSrc || hWindow->stSettings.bForceSyncLock)?
				BVDC_P_FIELD_VSYNC_DELAY : BVDC_P_FRAME_VSYNC_DELAY;
		}
		else
		{
			ulBufDelay = BVDC_P_NO_CAPTURE_VSYNC_DELAY;
		}
	}
	else if (BVDC_P_SRC_IS_VFD(hSource->eId)) /* capture is not possible with VFD as source */
	{
		bCapture = false;
		ulBufDelay = BVDC_P_NO_CAPTURE_VSYNC_DELAY;
	}
	else
	{
		/* Non mpeg source always require capture. */
		bCapture = true;
		/* Need to or with bForceCapture if not always capture */
		/* bCapture |= pWinInfo->bForceCapture; */
		ulBufDelay = BVDC_P_FRAME_VSYNC_DELAY;
	}

	/* Always capture if uiVsyncDelayOffset is not 0 */
	bEnableCaptureByDelay = false;
	if( pWinInfo->uiVsyncDelayOffset && !bCapture && !BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		bEnableCaptureByDelay = true;
		bCapture = true;
	}

	/* Always capture if source mosaic mode is on; */
	if( (hSource->stNewInfo.bMosaicMode || hSource->stNewInfo.bPsfEnable) && !bCapture && !BVDC_P_SRC_IS_VFD(hSource->eId))
	{
		bEnableCaptureByMosaicOrPsf = true;
		bCapture = true;
	}

#if (BVDC_P_SUPPORT_3D_VIDEO)
	/* Always capture if source orientation and display orientation not match */
	if((BFMT_IS_3D_MODE(pCmpInfo->pFmtInfo->eVideoFmt) &&
	    pSrcFmtInfo->eOrientation != pCmpInfo->pFmtInfo->eOrientation) ||
	   (pSrcFmtInfo->eOrientation != pCmpInfo->eOrientation))
	{
		bCapture = true;
	}
#endif

	/* hWindow->bCapture is completely decided by user info, it will be used to
	 * decide vnetMode */
	hWindow->bCapture = bCapture;

	/* How many buffers do we need for given mode. */
	if(bCapture)
	{
		ulBufCntNeeded = (hWindow->bSyncLockSrc || hWindow->stSettings.bForceSyncLock)?
			BVDC_P_SYNC_LOCK_MULTI_BUFFER_COUNT : BVDC_P_SYNC_SLIP_MULTI_BUFFER_COUNT;
	}
	else
	{
		/* if (!BVDC_P_SRC_IS_VFD(hWindow->stNewInfo.hSource->eId))
		   BDBG_ASSERT(hWindow->bSyncLockSrc); */
		ulBufCntNeeded = BVDC_P_BYPASS_MULTI_BUFFER_COUNT;
	}

	hWindow->bBufCntNeededChanged = (ulBufCntNeeded != hWindow->ulPrevBufCntNeeded);
	hWindow->ulPrevBufCntNeeded = ulBufCntNeeded;

	/* Update ulBufDelay & ulBufCntNeeded based on delay offset */
	if( bEnableCaptureByDelay )
	{
		/* Don't need to capture without delay */
		ulBufDelay     = pWinInfo->uiVsyncDelayOffset ;
		ulBufCntNeeded = ulBufDelay + 1;
	}
	else if( bEnableCaptureByMosaicOrPsf )
	{
		/* Don't need to capture without mosaic */
		ulBufDelay     = BVDC_P_FIELD_VSYNC_DELAY ;
		ulBufCntNeeded = ulBufDelay + 1;
	}
	else
	{
		/* Need to capture without delay */
		ulBufDelay     += pWinInfo->uiVsyncDelayOffset ;
		ulBufCntNeeded += pWinInfo->uiVsyncDelayOffset ;
	}
	hWindow->ulBufCntNeeded = ulBufCntNeeded;
	hWindow->ulBufDelay     = ulBufDelay;

	/* Determine rate gap */
	BVDC_P_Buffer_CalculateRateGap(hWindow->stNewInfo.hSource->ulVertFreq,
		hCompositor->stNewInfo.pFmtInfo->ulVertFreq,
		&eWriterVsReaderRateCode, &eReaderVsWriterRateCode);

	/* Reset the buffer count decremented/incremented flags */
	hWindow->bBufferCntDecremented = false;
	hWindow->bBufferCntDecrementedForPullDown = false;

	/* If the buffer count needs to be incremented,
	 * it will be set when the interlace with rate gap condition is detected. */
	hWindow->bBufferCntIncremented = false;

	/* Decrement buffer count if new video format is progressive and reader and writer rate gaps
	 * are the same or if we are doing progressive 24/25/30 Hz to 48/50/60 Hz conversion.
	 * Set the bBufferCntDecremented flag.
	 */
	bProgressivePullDown = VIDEO_FORMAT_IS_PROGRESSIVE(hSource->stNewInfo.pFmtInfo->eVideoFmt) &&
						   (eWriterVsReaderRateCode == BVDC_P_WrRate_NotFaster) &&
						   (eReaderVsWriterRateCode == BVDC_P_WrRate_2TimesFaster);

	if((!hWindow->bSyncLockSrc) && (!hWindow->stSettings.bForceSyncLock) && !BVDC_P_SRC_IS_VFD(hWindow->stNewInfo.hSource->eId) &&
		((VIDEO_FORMAT_IS_PROGRESSIVE(hCompositor->stNewInfo.pFmtInfo->eVideoFmt)
		&& (eWriterVsReaderRateCode == eReaderVsWriterRateCode)) || bProgressivePullDown))
	{
		hWindow->ulBufCntNeeded--;
		if(eWriterVsReaderRateCode == eReaderVsWriterRateCode)
			hWindow->ulBufDelay--;
		else
			/* Progressive pull down case */
			hWindow->bBufferCntDecrementedForPullDown = true;

		hWindow->bBufferCntDecremented = true;
	}

	BDBG_LEAVE(BVDC_P_Window_DecideCapture);
	return;
}


/***************************************************************************
 * {private}
 *
 * This function calcualtes DCX heap size in bytes.
 */
static uint32_t BVDC_P_Window_GetDCXBufSize_isr
	( const BVDC_Window_Handle                hWindow,
	  const BVDC_P_Rect                      *pSrcRect,
	  const bool                              bInterlaced,
	  const BVDC_Video_TestFeature1_Settings *pTestFeature1 )
{
	uint32_t  ulBufSize = 0; /* in bytes */

	BSTD_UNUSED(hWindow);
	BDBG_ASSERT(pTestFeature1);

	/* DCX buffer:
	 * ulBufSize = bits_per_pixel * pixels_per_picture + 16*1024 + 64 */
	ulBufSize = (((pSrcRect->ulWidth * pSrcRect->ulHeight) >> bInterlaced)
		* pTestFeature1->ulBitsPerPixel) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;
	ulBufSize += 16*1024 + 64 + 7 ;

	/* In bytes */
	ulBufSize /= 8;

	/* Make sure 32 byte aligned */
	ulBufSize = BVDC_P_ALIGN_UP(ulBufSize, BVDC_P_HEAP_ALIGN_BYTES);

	return ulBufSize;
}

/***************************************************************************
 * {private}
 *
 * This function calcualtes heap size in bytes.
 */
static void BVDC_P_Window_GetBufSize_isr
	( const BVDC_Window_Handle                hWindow,
	  const BVDC_P_Rect                      *pSrcRect,
	  const bool                              bInterlaced,
	  const BPXL_Format                       eBufPxlFmt,
	  const BVDC_Video_TestFeature1_Settings *pTestFeature1,
	  BVDC_P_BufHeapType                      eBufHeapType,
	  uint32_t                               *pulBufSize )
{
	uint32_t             ulBufSize, ulQMCount; /* in bytes */
	unsigned int         uiPitch;

	ulQMCount = BVDC_P_MAD_QM_FIELD_STORE_COUNT;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* Get buffer size */
	if(eBufHeapType == BVDC_P_BufHeapType_eMad_QM)
	{
#if (BVDC_P_SUPPORT_MCVP)
		ulQMCount = (!hWindow->stCurResource.hMcvp->hMcdi->bMadr)?
			BVDC_P_MCDI_QM_FIELD_STORE_COUNT:ulQMCount;
#endif
		/* QM buffer is always 2 bits per pixel */
		ulBufSize = (((pSrcRect->ulWidth * pSrcRect->ulHeight) >> bInterlaced)
			* BVDC_P_MAD_QM_BITS_PER_PIXEL * ulQMCount) / 8;
	}
#if (BVDC_P_OPTIMIZE_MEM_USAGE)
	else if(pTestFeature1 && pTestFeature1->bEnable)
#else
	else if(pTestFeature1 && pTestFeature1->bEnable &&
		(eBufHeapType  == BVDC_P_BufHeapType_eCapture))
#endif
	{
		ulBufSize = BVDC_P_Window_GetDCXBufSize_isr(hWindow, pSrcRect,
			bInterlaced, pTestFeature1);
	}
	else
	{
		BPXL_GetBytesPerNPixels(eBufPxlFmt, pSrcRect->ulWidth, &uiPitch);

		/* Need to be aligned for capture buffers */
		if(eBufHeapType == BVDC_P_BufHeapType_eCapture)
			uiPitch = BVDC_P_ALIGN_UP(uiPitch, BVDC_P_PITCH_ALIGN);
		ulBufSize = (uiPitch * pSrcRect->ulHeight) >> bInterlaced;
	}

	/* Make sure 32 byte aligned */
	ulBufSize = BVDC_P_ALIGN_UP(ulBufSize, BVDC_P_HEAP_ALIGN_BYTES);

	BDBG_MODULE_MSG(BVDC_WIN_BUF_SIZE, ("Win[%d] (%6s) pSrcRect : %d, %d (%d)", hWindow->eId,
		(eBufHeapType == BVDC_P_BufHeapType_eCapture)   ? "Cap"    :
		(eBufHeapType == BVDC_P_BufHeapType_eMad_Pixel) ? "Mad_PX" :
		(eBufHeapType == BVDC_P_BufHeapType_eMad_QM)    ? "MAD_QM" : "Anr",
		pSrcRect->ulWidth,
		(pSrcRect->ulHeight >> bInterlaced), bInterlaced));

	if( pulBufSize )
	{
		*pulBufSize = ulBufSize;
	}

	return;
}

/***************************************************************************
 * {private}
 *
 * This function attempt to figure out which heap to use (HD, SD, Pip, etc)
 * for capture, MAD or ANR.
 *
 */
static void BVDC_P_Window_GetBufHeapId_isr
	( const BVDC_Window_Handle         hWindow,
	  uint32_t                         ulBufSize,
	  BVDC_P_BufferHeapId             *peBufferHeapIdRequest,
	  BVDC_P_BufferHeapId             *peBufferHeapIdPrefer )
{
	BVDC_P_BufferHeapId  eBufferHeapId = BVDC_P_BufferHeapId_eUnknown;
	BVDC_P_BufferHeapId  eBufferHeapIdPrefer = BVDC_P_BufferHeapId_eUnknown;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hHeap, BVDC_BFH);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	/* Get buffer heap ID by size */
	BVDC_P_BufferHeap_GetHeapIdBySize(hWindow->hHeap, ulBufSize, &eBufferHeapId);

	/* Update eBufferHeapIdPrefer for capture buffers */
	if((eBufferHeapId == BVDC_P_BufferHeapId_eSD) &&
	   (hWindow->hCompositor->eId == BVDC_CompositorId_eCompositor0))
	{
		if(hWindow->hCompositor->hVdc->stSettings.stHeapSettings.ulBufferCnt_2HD)
			eBufferHeapIdPrefer = BVDC_P_BufferHeapId_e2HD;
		else
			eBufferHeapIdPrefer = BVDC_P_BufferHeapId_eHD;
	}


	BDBG_MSG(("Window[%d] selects buffer %s, prefered buffer %s", hWindow->eId,
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapId),
		BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapIdPrefer)));

	/* Update */
	if(peBufferHeapIdRequest)
	{
		*peBufferHeapIdRequest = eBufferHeapId;
	}

	if( peBufferHeapIdPrefer )
	{
		*peBufferHeapIdPrefer = eBufferHeapIdPrefer;
	}

	return;
}


static void BVDC_P_Window_AcquireBvnResources_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Resource_Handle hResource;
	BVDC_P_Window_Info *pCurInfo;

	hResource = hWindow->hCompositor->hVdc->hResource;
	pCurInfo  = &hWindow->stCurInfo;


	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hScaler)
	{
		BVDC_P_Scaler_Handle *phScaler=&hWindow->stNewResource.hScaler;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eScl, hWindow->pResourceFeature->ulScl,
			(uint32_t) hWindow, (void **)phScaler, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hScaler);
		hWindow->stCurResource.hScaler = hWindow->stNewResource.hScaler;

		BVDC_P_Scaler_Init(hWindow->stCurResource.hScaler, hWindow);
	}


#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hHscaler)
	{
		BVDC_P_Hscaler_Handle *phHscaler=&hWindow->stNewResource.hHscaler;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eHscl, 0, hWindow->eId, (void **)phHscaler, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hHscaler);
		hWindow->stCurResource.hHscaler = hWindow->stNewResource.hHscaler;

		BVDC_P_Hscaler_AcquireConnect(hWindow->stCurResource.hHscaler, hWindow->eId);
	}
#endif


#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hMad32)
	{
		BVDC_P_Mad_Handle *phMad32=&hWindow->stNewResource.hMad32;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eMad, hWindow->pResourceFeature->ulMad, hWindow->eId,
			(void **)phMad32, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hMad32);
		hWindow->stCurResource.hMad32 = hWindow->stNewResource.hMad32;

		BVDC_P_Mad_AcquireConnect(
			hWindow->stCurResource.hMad32, hWindow->hHeap, hWindow->eId);
	}
#endif

#if (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hAnr)
	{
		BVDC_P_Anr_Handle *phAnr=&hWindow->stNewResource.hAnr;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eAnr, 0, hWindow->eId,
			(void **)phAnr, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hAnr);
		hWindow->stCurResource.hAnr = hWindow->stNewResource.hAnr;

		BVDC_P_Anr_AcquireConnect(hWindow->stCurResource.hAnr, hWindow->hHeap, hWindow->eId,
			&pCurInfo->stAnrSettings, &hWindow->stNewInfo.hSource->stCurInfo);
	}
#endif


#if (BVDC_P_SUPPORT_MCVP)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hMcvp)
	{
		BVDC_P_Mcvp_Handle *phMcvp=&hWindow->stNewResource.hMcvp;
		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eMcvp, hWindow->pResourceFeature->ulMad,
			hWindow->eId, (void **)phMcvp, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hMcvp);
		hWindow->stCurResource.hMcvp = hWindow->stNewResource.hMcvp;

		BVDC_P_Mcvp_AcquireConnect(hWindow->stCurResource.hMcvp, hWindow->hHeap,
			hWindow->eId, &hWindow->stCurInfo, &pCurInfo->stAnrSettings,
			&hWindow->stNewInfo.hSource->stCurInfo);
	}
#endif

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hDnr)
	{
		BVDC_P_Dnr_Handle *phDnr=&hWindow->stNewResource.hDnr;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eDnr, 0, hWindow->stNewInfo.hSource->eId,
			(void **)phDnr, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hDnr);
		hWindow->stCurResource.hDnr = hWindow->stNewResource.hDnr;

		BVDC_P_Dnr_AcquireConnect(hWindow->stCurResource.hDnr, hWindow->stNewInfo.hSource);
	}
#endif

#if (BVDC_P_SUPPORT_BOX_DETECT)
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hBoxDetect)
	{
		BVDC_P_BoxDetect_Handle *phBoxDetect=&hWindow->stNewResource.hBoxDetect;
		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eBoxDetect, 0, hWindow->stNewInfo.hSource->eId,
			(void **)phBoxDetect, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hBoxDetect);
		hWindow->stCurResource.hBoxDetect = hWindow->stNewResource.hBoxDetect;

		BVDC_P_BoxDetect_AcquireConnect(hWindow->stCurResource.hBoxDetect, hWindow->stNewInfo.hSource->eId,
			&(hWindow->stNewInfo.hSource->stCurInfo));
	}
#endif


#if BVDC_P_SUPPORT_FGT
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hFgt)
	{
		BVDC_P_Fgt_Handle *phFgt=&hWindow->stNewResource.hFgt;
		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eFgt, 0, hWindow->eId,
			(void **)phFgt, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hFgt);
		hWindow->stCurResource.hFgt = hWindow->stNewResource.hFgt;

		BVDC_P_Fgt_AcquireConnect(hWindow->stCurResource.hFgt, hWindow, pCurInfo);
	}
#endif

#if BVDC_P_SUPPORT_HIST
	if(BVDC_P_RESOURCE_ID_AVAIL == (unsigned int) hWindow->stNewResource.hHist)
	{
		BVDC_P_Hist_Handle *phHist=&hWindow->stNewResource.hHist;

		BVDC_P_Resource_AcquireHandle_isr(hResource,
			BVDC_P_ResourceType_eHist, 0, hWindow->eId, (void **)phHist, false);

		BDBG_ASSERT(NULL != hWindow->stNewResource.hHist);
		hWindow->stCurResource.hHist = hWindow->stNewResource.hHist;

		BVDC_P_Hist_AcquireConnect(hWindow->stCurResource.hHist, hWindow);
	}
#endif

}

/***************************************************************************
 * {private}
 */
BERR_Code BVDC_P_Window_ApplyChanges_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_Info *pNewInfo;
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pNewDirty;
	bool  bWindowStateChanged = false;

	BDBG_ENTER(BVDC_P_Window_ApplyChanges_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* Get the compositor that this window belongs to. */
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->stNewInfo.hSource, BVDC_SRC);

	/* To reduce the amount of typing */
	pNewInfo  = &hWindow->stNewInfo;
	pCurInfo  = &hWindow->stCurInfo;
	pNewDirty = &pNewInfo->stDirty;

	/* Update to take in new changes. */
	BVDC_P_Window_SetBlender_isr(hWindow, pNewInfo);
	BVDC_P_Window_SetMiscellaneous_isr(hWindow, pNewInfo);

	/* Allocate new resource if needed */
	if(hWindow->bAllocResource)
	{
		BVDC_P_Window_AcquireBvnResources_isr(hWindow);
		hWindow->bAllocResource = false;
	}

	/* Release resources that are allocated but unused!  Since it's unused
	 * it does not trigger shutdown to release them.  Release them here
	 * to avoid forced-shutdown side-effects. */
#if BVDC_P_SUPPORT_MAD
	if((!hWindow->stVnetMode.stBits.bUseMad) &&
	   (!hWindow->stNewInfo.bDeinterlace) &&
	   ( hWindow->stCurResource.hMad32))
	{
		BDBG_MSG(("window[%d] releases shared MAD", hWindow->eId));
		BVDC_P_Mad_ReleaseConnect_isr(&hWindow->stCurResource.hMad32);
		hWindow->stNewResource.hMad32 = NULL;
	}
#endif

#if BVDC_P_SUPPORT_MCVP
	if((!hWindow->stVnetMode.stBits.bUseMcvp) &&
	   (!hWindow->stNewInfo.bDeinterlace) &&
	   (!hWindow->stNewInfo.bAnr) &&
	   ( hWindow->stCurResource.hMcvp))
	{
		BDBG_MSG(("window[%d] releases shared MCVP", hWindow->eId));
		BVDC_P_Mcvp_ReleaseConnect_isr(&hWindow->stCurResource.hMcvp);
		hWindow->stNewResource.hMcvp = NULL;
	}
#endif

	/* State transitions. */
	if(BVDC_P_STATE_IS_CREATE(hWindow))
	{
		/* (1) Connect this window with new source. */
		hWindow->eState = BVDC_P_State_eActive;

		/* this flags a window is being created; */
		bWindowStateChanged = true;

		BVDC_P_Source_ConnectWindow_isr(hWindow->stNewInfo.hSource, hWindow);
		if(BVDC_P_SRC_IS_VIDEO(hWindow->stNewInfo.hSource->eId))
		{
			/* mute the window in src-pending mode */
			if(hWindow->stNewInfo.hSource->stNewInfo.eResumeMode)
			{
				BDBG_MSG(("Source[%d] is pending mode!", hWindow->stNewInfo.hSource->eId));
				BVDC_P_Window_SetReconfiguring_isr(hWindow, true, false);
			}
			else  /* start the new win now! */
			{
				/* pCurInfo->eWriterState  = BVDC_P_State_eShutDownPending;
				 * will cause a shudown before vnet start, but it is not needed.
				 * without that, the following cause vnet start right away */
				BVDC_P_Window_SetReconfiguring_isr(hWindow, false, true);
			}
		}
		else /* gfx window */
		{
			pNewInfo->eReaderState = BVDC_P_State_eActive;
			pNewInfo->eWriterState = BVDC_P_State_eActive;
		}
#if BVDC_P_SUPPORT_STG
		if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
		{
			hWindow->hCompositor->bGfxChannelChange = false;
		}
#endif
		BDBG_MSG(("Window[%d] activated.", hWindow->eId));
		BDBG_MSG(("Window[%d] is BVDC_P_State_eActive", hWindow->eId));
	}
	else if(BVDC_P_STATE_IS_DESTROY(hWindow))
	{
		/* (1) Disconnecting this window from source. */
		pNewDirty->stBits.bDestroy              = BVDC_P_DIRTY;
		hWindow->bUserAppliedChanges     = true;
		hWindow->bSetDestroyEventPending = true;
		BDBG_MSG(("Window[%d] de-activated.", hWindow->eId));

		/* inform BVDC_P_CheckApplyChangesStatus to wait for destroy-done event */
		hWindow->eState                  = BVDC_P_State_eShutDownPending;
		BDBG_MSG(("(1) Window[%d] is BVDC_P_State_eShutDownPending", hWindow->eId));

#if BVDC_P_SUPPORT_STG
		/* xcode clear bGfxChannelchange indicating meaningless gfx content*/
		if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
		{
			hWindow->hCompositor->bGfxChannelChange = true;
		}
#endif
		/* in this case BLEND_SOURCE is already set to BACKGROUND_BYPASS by
		 * BVDC_P_Window_SetBlender_isr, the next reader RUL must start to
		 * shut down. Therefore we can not wait for writer_isr to setReconfig
		 * in the case that reader builds RUL next */
		BVDC_P_Window_SetReconfiguring_isr(hWindow, false, true);
	}

	/* Update does not affect the number of buffer nodes nor vnet. */
	if((pNewInfo->ucAlpha           != pCurInfo->ucAlpha           ) ||
	   (pNewInfo->bVisible          != pCurInfo->bVisible          ) ||
	   (pNewInfo->ucZOrder          != pCurInfo->ucZOrder          ) ||
	   (pNewInfo->ucConstantAlpha   != pCurInfo->ucConstantAlpha   ) ||
	   (pNewInfo->eBackBlendFactor  != pCurInfo->eBackBlendFactor  ) ||
	   (pNewInfo->eFrontBlendFactor != pCurInfo->eFrontBlendFactor ) ||
	   (pNewInfo->ePhaseAdjustMode  != pCurInfo->ePhaseAdjustMode  ) ||
	   (pNewInfo->ulMosaicTrackChannelId != pCurInfo->ulMosaicTrackChannelId) ||
	   (pNewInfo->bUseSrcFrameRate  != pCurInfo->bUseSrcFrameRate  ))
	{
		hWindow->bUserAppliedChanges = true;
	}

	/* This ensure that the callback will get call at least once. */
	if((pNewInfo->pfGenCallback      != pCurInfo->pfGenCallback      ) ||
	   (pNewInfo->pvGenCallbackParm1 != pCurInfo->pvGenCallbackParm1 ) ||
	   (pNewInfo->iGenCallbackParm2  != pCurInfo->iGenCallbackParm2  ))
	{
		hWindow->stCallbackData.ulVsyncDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
		hWindow->stCallbackData.ulDriftDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
		hWindow->stCallbackData.ulGameModeDelay = BVDC_P_LIP_SYNC_RESET_DELAY;
		hWindow->stCallbackData.stOutputRect.lLeft    = pNewInfo->stDstRect.lLeft;
		hWindow->stCallbackData.stOutputRect.lTop     = pNewInfo->stDstRect.lTop;
		hWindow->stCallbackData.stOutputRect.ulWidth  = pNewInfo->stDstRect.ulWidth;
		hWindow->stCallbackData.stOutputRect.ulHeight = pNewInfo->stDstRect.ulHeight;
		hWindow->bUserAppliedChanges = true;
	}

	/* Any of the dirty bit set then mark as applied to be use for next rul
	 * building. */
	if(BVDC_P_IS_DIRTY(pNewDirty))
	{
		hWindow->bUserAppliedChanges = true;
		BDBG_MSG(("Window[%d]'s new dirty bits = 0x%08x",
			hWindow->eId, pNewDirty->aulInts[0]));
	}

	/* Window to be removed or added? */
	if((hWindow->stNewInfo.hSource->stNewInfo.ulWindows != hWindow->stNewInfo.hSource->stCurInfo.ulWindows) ||
	   (hWindow->stNewInfo.hSource->stNewInfo.bPsfEnable!= hWindow->stNewInfo.hSource->stCurInfo.bPsfEnable) ||
	   (hWindow->stNewInfo.hSource->stNewInfo.bDnr      != hWindow->stNewInfo.hSource->stCurInfo.bDnr) ||
	   (hWindow->stNewInfo.hSource->stNewInfo.stSplitScreenSetting.eDnr != hWindow->stNewInfo.hSource->stCurInfo.stSplitScreenSetting.eDnr))
	{
		hWindow->bUserAppliedChanges = true;
	}

	/* game mode tracking window */
	if((pNewInfo->stGameDelaySetting.bEnable != pCurInfo->stGameDelaySetting.bEnable) ||
	   (pNewInfo->stGameDelaySetting.ulBufferDelayTarget != pCurInfo->stGameDelaySetting.ulBufferDelayTarget) ||
	   (pNewInfo->stGameDelaySetting.ulBufferDelayTolerance != pCurInfo->stGameDelaySetting.ulBufferDelayTolerance))
	{
		hWindow->bUserAppliedChanges = true;

		/* Note: only one window could be game mode tracked */
		if(pNewInfo->stGameDelaySetting.bEnable)
		{
			hWindow->hCompositor->hDisplay->hWinGameMode = hWindow;
		}
	}

	/* Update to be pick up by display queue, could change the number
	 * of buffers node and/or vnet.  Sensitive to input/output size
	 * changes. */
	if((bWindowStateChanged) ||
	   (hWindow->bUserAppliedChanges) || /* preset by dirty bits model. */
	   (pNewInfo->stAfdSettings.eMode       != pCurInfo->stAfdSettings.eMode ) ||
	   (pNewInfo->stAfdSettings.eClip       != pCurInfo->stAfdSettings.eClip ) ||
	   (pNewInfo->stAfdSettings.ulAfd       != pCurInfo->stAfdSettings.ulAfd ) ||
	   (pNewInfo->ePanScanType              != pCurInfo->ePanScanType        ) ||
	   (pNewInfo->lUserHorzPanScan          != pCurInfo->lUserHorzPanScan    ) ||
	   (pNewInfo->lUserVertPanScan          != pCurInfo->lUserVertPanScan    ) ||
	   (pNewInfo->eAspectRatioMode          != pCurInfo->eAspectRatioMode    ) ||
	   (pNewInfo->ulNonlinearSrcWidth       != pCurInfo->ulNonlinearSrcWidth ) ||
	   (pNewInfo->ulNonlinearSclOutWidth    != pCurInfo->ulNonlinearSclOutWidth ) ||
	   (pNewInfo->uiVsyncDelayOffset        != pCurInfo->uiVsyncDelayOffset  ) ||
	   (pNewInfo->bForceCapture             != pCurInfo->bForceCapture       ) ||
	   (pNewInfo->ulHrzSclFctRndToler       != pCurInfo->ulHrzSclFctRndToler ) ||
	   (pNewInfo->ulVrtSclFctRndToler       != pCurInfo->ulVrtSclFctRndToler ) ||
	   (pNewInfo->ulBandwidthDelta          != pCurInfo->ulBandwidthDelta    ) ||
	   (pNewInfo->eSclCapBias               != pCurInfo->eSclCapBias         ) ||
	   (hWindow->hCompositor->stNewInfo.eOrientation != hWindow->hCompositor->stCurInfo.eOrientation) ||
	   (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stScalerOutput, &pCurInfo->stScalerOutput)) ||
	   (!BVDC_P_RECT_CMP_EQ(&pNewInfo->stDstRect,      &pCurInfo->stDstRect     )) ||
	   (!BVDC_P_CLIP_RECT_CMP_EQ(&pNewInfo->stSrcClip, &pCurInfo->stSrcClip     )) ||
	   (hWindow->hCompositor->bDspAspRatDirty) ||
	   (BVDC_P_ItState_eActive != hWindow->hCompositor->hDisplay->eItState))
	{
		hWindow->bUserAppliedChanges = true;
		if(BVDC_P_SRC_IS_VIDEO(hWindow->stNewInfo.hSource->eId))
		{
			/* Set number of user capture buffers that will be used */
			if(pNewInfo->uiCaptureBufCnt != pCurInfo->uiCaptureBufCnt)
			{
				hWindow->uiAvailCaptureBuffers = pNewInfo->uiCaptureBufCnt;
				if(pNewInfo->uiCaptureBufCnt > pCurInfo->uiCaptureBufCnt)
				{
					BDBG_MSG(("Added %d buffers for user capture", hWindow->uiAvailCaptureBuffers));
				}
				else
				{
					BDBG_MSG(("Decremented %d buffers for user capture", hWindow->uiAvailCaptureBuffers));
				}
			}

			/* Partial decision on the vnet mode and cap buf count, according to user's
			 * setting only.  Further and complete decision are done later with
			 * combination of both user settings and dynamic src info in writer_isr. */
			BVDC_P_Window_DecideCapture(hWindow, hWindow->stNewInfo.hSource, hWindow->hCompositor);

			/* inform writer_isr to redecide vnet mode */
			hWindow->stNewInfo.stDirty.stBits.bReDetVnet = BVDC_P_DIRTY;

			/* Add number of user capture buffers to number of needed buffers. */
			/* This code is a bit tricky here. We always use the uiCaptureBufCnt
			 * from pNewInfo. This is under the fact that if we changed capture
			 * buffer count before, that number is always stored in hWindow->stNewInfo
			 * till user changes it, which will also be done thru hWindow->stNewinfo.
			 */
			hWindow->ulBufCntNeeded += pNewInfo->uiCaptureBufCnt;

			if(hWindow->ulBufCntNeeded != hWindow->ulBufCntAllocated)
			{
				BDBG_MSG(("ulBufCntAllocated (%d) != ulBufCntNeeded (%d), stVnetMode = 0x%x",
					hWindow->ulBufCntAllocated,
					hWindow->ulBufCntNeeded, hWindow->stVnetMode));
				pNewDirty->stBits.bReallocBuffers = BVDC_P_DIRTY;
			}

			/* The life of dirty bit bUserCaptureBuffer ends here since new dirty bit
			 * bReallocBuffers has been set up if buffer re-allocation needs to happen.
			 */
			pNewDirty->stBits.bUserCaptureBuffer = BVDC_P_CLEAN;

			/* this has to stay here (rather than in writer_isr) to avoid reader using
			 * those old bufs before writer_isr flush them */
			if(BVDC_P_ItState_eActive != hWindow->hCompositor->hDisplay->eItState)
			{
				BDBG_MSG(("window[%d] flushes buffers due to display format change",
					hWindow->eId));
				BVDC_P_Buffer_Invalidate_isr(hWindow->hBuffer);
				hWindow->pCurWriterNode = BVDC_P_Buffer_GetCurWriterNode(hWindow->hBuffer);
				hWindow->pCurReaderNode = BVDC_P_Buffer_GetCurReaderNode(hWindow->hBuffer);

				/* NOTE: display format switch must invalidate window MAD delay sw pipeline
				 * if MAD is at vnet reader side!
				 * since it might not cause vnet reconfig and MAD delay pipeline might not
				 * have hard_start initialization, which may result in BVN error! */
				if(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) ||
				   BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode))
					hWindow->bResetMadDelaySwPipe = true;
			}
		}
	}/* don't make above changes if no changes */

	/* reset the count for gfx window to be programmed later in reader isr to
	 * accomandate vbi pass through info. */
	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
	{
		/* hWindow->ulGwinSetCount = 0; */
		/* BACK OUT the above changes for now to make analog working */
		BAVC_Polarity eScanType = (hWindow->hCompositor->stNewInfo.pFmtInfo->bInterlaced)
			? BAVC_Polarity_eTopField : BAVC_Polarity_eFrame;
		BVDC_P_GfxFeeder_GetAdjSclOutRect_isr(&pNewInfo->stSrcClip, &pNewInfo->stScalerOutput,
			&pNewInfo->stDstRect, &hWindow->stAdjSclOut);
		BVDC_P_Window_SetSurfaceSize_isr(hWindow, &hWindow->stAdjSclOut, eScanType);
		BVDC_P_Window_SetDisplaySize_isr(hWindow, &pNewInfo->stDstRect, eScanType,
			(uint32_t)(pNewInfo->stDstRect.lLeft + pNewInfo->lRWinXOffsetDelta));
	}

	/* update the phase adjust mode in the compositor context */
	if(pNewInfo->ePhaseAdjustMode != BVDC_PhaseAdjustMode_eDisable)
	{
		BDBG_ASSERT(hWindow->hCompositor->ePhaseAdjustMode == BVDC_PhaseAdjustMode_eDisable);
		hWindow->hCompositor->ePhaseAdjustMode = pNewInfo->ePhaseAdjustMode;
	}

	/* Isr will set event to notify apply done. */
	if(hWindow->bUserAppliedChanges)
	{
		/* inform *DataReady_isr to recalculate the adjusted rectangles,
		 * for this window and related window that share the same source. */
		hWindow->bAdjRectsDirty  = true;
		hWindow->stNewInfo.hSource->bPictureChanged = true;

		/* If BypassDisplay's 656 is not enable it will not produce interrupt
		 * hence no one to set the event, so we just set it here. */
		if((!hWindow->hCompositor->hDisplay->bIsBypass) ||
		   (hWindow->hCompositor->hDisplay->stCurInfo.bEnable656) ||
		   (hWindow->hCompositor->hDisplay->stCurInfo.bEnableHdmi))
		{
			hWindow->bSetAppliedEventPending = true;
			BKNI_ResetEvent(hWindow->hAppliedDoneEvent);
			BDBG_MSG(("Window[%d] New changes reset applied event.", hWindow->eId));
		}

		/* copy stNewInfo to stCurInfo here !!! */
		BVDC_P_Window_UpdateUserState_isr(hWindow);
	}

	/* ANR demo mode */
#if (BVDC_P_SUPPORT_ANR)
	if(hWindow->stCurResource.hAnr)
	{
		BVDC_P_Anr_SetDemoMode_isr(hWindow->stCurResource.hAnr, hWindow->stNewInfo.stSplitScreenSetting.eAnr);
	}
#endif
#if (BVDC_P_SUPPORT_MANR)
	if(hWindow->stCurResource.hMcvp && hWindow->stCurResource.hMcvp->hAnr)
	{
		BVDC_P_Anr_SetDemoMode_isr(hWindow->stCurResource.hMcvp->hAnr, hWindow->stNewInfo.stSplitScreenSetting.eAnr);
	}
#endif

	BDBG_LEAVE(BVDC_P_Window_ApplyChanges_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 * This function is called to release resources, to cleanup, to send done
 * event to ApplyChanges, after vnet shut down is completed.
 *
 * Video Window
 *   Shutdown -> Inactive -> Set destroy done event
 *
 * Gfx Window
 *   ShutdownPending -> Shutdown -> Inactive -> Set destroy done event
 */
static void BVDC_P_Window_ProcPostShutDown_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;
	BVDC_P_Resource_Handle hResource;

	BDBG_ENTER(BVDC_P_Window_ProcPostShutDown_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pCurInfo  = &hWindow->stCurInfo;
	pCurDirty = &hWindow->stCurInfo.stDirty;

	/* This print alter isr timing dramatically, so leave it to avoid
	 * flooding of messages. */
#if (BVDC_P_PRINT_DIRTY_BITS)
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		BDBG_ERR(("pCurDirty->stBits.bRecAdjust             = %d", pCurDirty->stBits.bRecAdjust));
		BDBG_ERR(("pCurDirty->stBits.bReDetVnet             = %d", pCurDirty->stBits.bReDetVnet));
		BDBG_ERR(("pCurDirty->stBits.bCscAdjust             = %d", pCurDirty->stBits.bCscAdjust));
		BDBG_ERR(("pCurDirty->stBits.bTabAdjust             = %d", pCurDirty->stBits.bTabAdjust));
		BDBG_ERR(("pCurDirty->stBits.bTntAdjust             = %d", pCurDirty->stBits.bTntAdjust));
		BDBG_ERR(("pCurDirty->stBits.bLabAdjust             = %d", pCurDirty->stBits.bLabAdjust));
		BDBG_ERR(("pCurDirty->stBits.bCabAdjust             = %d", pCurDirty->stBits.bCabAdjust));
		BDBG_ERR(("pCurDirty->stBits.bDitAdjust             = %d", pCurDirty->stBits.bDitAdjust));
#if BVDC_P_SUPPORT_CM3D
		BDBG_ERR(("pCurDirty->stBits.bCm3dRegionAdj         = %d", pCurDirty->stBits.bCm3dRegionAdj));
		BDBG_ERR(("pCurDirty->stBits.bCm3dOverlapRegionAdj  = %d", pCurDirty->stBits.bCm3dOverlapRegionAdj));
		BDBG_ERR(("pCurDirty->stBits.bChromaHistoRect       = %d", pCurDirty->stBits.bChromaHistoRect));
#endif
		BDBG_ERR(("pCurDirty->stBits.bLumaKeyAdjust         = %d", pCurDirty->stBits.bLumaKeyAdjust));
		BDBG_ERR(("pCurDirty->stBits.bShutdown              = %d", pCurDirty->stBits.bShutdown));
		BDBG_ERR(("pCurDirty->stBits.bDestroy               = %d", pCurDirty->stBits.bDestroy));
		BDBG_ERR(("pCurDirty->stBits.bSrcPending            = %d", pCurDirty->stBits.bSrcPending));
		BDBG_ERR(("pCurDirty->stBits.bReConfigVnet          = %d", pCurDirty->stBits.bReConfigVnet));
		BDBG_ERR(("pCurDirty->stBits.bDeinterlace           = %d", pCurDirty->stBits.bDeinterlace));
		BDBG_ERR(("pCurDirty->stBits.bAnrAdjust             = %d", pCurDirty->stBits.bAnrAdjust));
		BDBG_ERR(("pCurDirty->stBits.bSharedScl             = %d", pCurDirty->stBits.bSharedScl));
		BDBG_ERR(("pCurDirty->stBits.bUserCaptureBuffer     = %d", pCurDirty->stBits.bUserCaptureBuffer));
		BDBG_ERR(("pCurDirty->stBits.bUserReleaseBuffer     = %d", pCurDirty->stBits.bUserReleaseBuffer));
		BDBG_ERR(("pCurDirty->stBits.bMosaicMode            = %d", pCurDirty->stBits.bMosaicMode));
		BDBG_ERR(("pCurDirty->stBits.bReallocBuffers        = %d", pCurDirty->stBits.bReallocBuffers));
		BDBG_ERR(("pCurDirty->stBits.bBoxDetect             = %d", pCurDirty->stBits.bBoxDetect));
		BDBG_ERR(("pCurDirty->stBits.bHistoRect             = %d", pCurDirty->stBits.bHistoRect));
		BDBG_ERR(("pCurDirty->stBits.bFgt                   = %d", pCurDirty->stBits.bFgt));
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
		BDBG_ERR(("pCurDirty->stBits.bTestFeature1          = %d", pCurDirty->stBits.bTestFeature1));
#endif
		BDBG_ERR(("bSetAppliedEventPending           = %d", hWindow->bSetAppliedEventPending));
		BDBG_ERR(("----------------------------------------"));
	}
#endif

	/* clean up right after shut-down, or win-destroy applied when
	 * the vnet is idle due to src pending */
	if(((BVDC_P_State_eShutDown == pCurInfo->eReaderState) &&
	    (BVDC_P_State_eShutDown == pCurInfo->eWriterState)) ||
	   ((pCurDirty->stBits.bDestroy) &&
	    (BVDC_P_State_eInactive == pCurInfo->eReaderState) &&
	    (BVDC_P_State_eInactive == pCurInfo->eWriterState)))
	{
		hResource = hWindow->hCompositor->hVdc->hResource;

		BVDC_P_Window_SetInvalidVnetMode(&(hWindow->stVnetMode));
		BVDC_P_Window_SetInvalidVnetMode(&(hWindow->stMcvpMode));
		pCurInfo->eReaderState   = BVDC_P_State_eInactive;
		pCurInfo->eWriterState   = BVDC_P_State_eInactive;
		pCurDirty->stBits.bShutdown     = BVDC_P_CLEAN;

		BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] shutdown completed", hWindow->eId));

		BDBG_OBJECT_ASSERT(pCurInfo->hSource, BVDC_SRC);

		/* release shared scaler for SD PIP or Bypass window */
		if((hWindow->stCurResource.hScaler) && (!hWindow->pResource->bRequireScaler) &&
			/* win destroyed by user, or */
			(pCurDirty->stBits.bDestroy ||
#if (BCHP_CHIP==7400)
			 false
#else
			 /* bypass win that does not need scl */
			 ((BVDC_P_WindowId_eComp2_V0 == hWindow->eId) &&
			  !(BVDC_P_SRC_IS_MPEG(pCurInfo->hSource->eId)  ||
				BVDC_P_SRC_IS_HDDVI(pCurInfo->hSource->eId) ||
				(BVDC_P_SRC_IS_ANALOG(pCurInfo->hSource->eId) && pCurInfo->hSource->stNewInfo.bCompInput)))
#endif
			 ))
		{
			BDBG_MSG(("window %d releases shared SCL", hWindow->eId));
			BVDC_P_Resource_ReleaseHandle_isr(hResource,
				BVDC_P_ResourceType_eScl, (void *)hWindow->stCurResource.hScaler);
			hWindow->stCurResource.hScaler = hWindow->stNewResource.hScaler = NULL;
		}

#if (BVDC_P_SUPPORT_DNR)
		/* relase unneeded dnr */
		if((hWindow->stCurResource.hDnr) &&
		   (pCurDirty->stBits.bDestroy ||
		   (!pCurInfo->hSource->stCurInfo.bDnr && !pCurInfo->hSource->stNewInfo.bDnr)))
		{
			BDBG_MSG(("Window %d releases shared DNR", hWindow->eId));
			BVDC_P_Dnr_ReleaseConnect_isr(&hWindow->stCurResource.hDnr);
			hWindow->stNewResource.hDnr = NULL;
		}
#endif

#if BVDC_P_SUPPORT_BOX_DETECT
		/* relase unneeded box-detect */
		if((hWindow->stCurResource.hBoxDetect) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bBoxDetect &&
			 !hWindow->stNewInfo.bBoxDetect)))
		{
			BDBG_MSG(("Window %d releases shared BoxDetect", hWindow->eId));
			BVDC_P_BoxDetect_ReleaseConnect_isr(&hWindow->stCurResource.hBoxDetect);
			hWindow->stNewResource.hBoxDetect = NULL;
			pCurDirty->stBits.bBoxDetect = BVDC_P_CLEAN;
		}
#endif

#if BVDC_P_SUPPORT_FGT
		/* relase unneeded fgt */
		if((hWindow->stCurResource.hFgt) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bFgt && !hWindow->stNewInfo.bFgt)))
		{
			BDBG_MSG(("Window %d releases shared Fgt", hWindow->eId));
			BVDC_P_Fgt_ReleaseConnect_isr(&hWindow->stCurResource.hFgt);
			hWindow->stNewResource.hFgt = NULL;
		}
#endif

#if BVDC_P_SUPPORT_HIST
		/* relase unneeded Hist */
		if((hWindow->stCurResource.hHist) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bHistEnable &&
			 !hWindow->stNewInfo.bHistEnable)))
		{
			BDBG_MSG(("Window %d releases shared Hist", hWindow->eId));
			BVDC_P_Hist_ReleaseConnect_isr(&hWindow->stCurResource.hHist);
			hWindow->stNewResource.hHist = NULL;
		}
#endif

#if BVDC_P_SUPPORT_HSCL
		if((hWindow->stCurResource.hHscaler) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bDeinterlace &&
			 !hWindow->stNewInfo.bDeinterlace &&
			 !hWindow->stCurInfo.bAnr &&
			 !hWindow->stNewInfo.bAnr)))
		{
			/* Assume: HSCL is tied to MAD/ANR usage! */
			BDBG_MSG(("window %d releases shared HSCL", hWindow->eId));
			BVDC_P_Hscaler_ReleaseConnect_isr(&hWindow->stCurResource.hHscaler);
			hWindow->stNewResource.hHscaler = NULL;
		}
#endif

#if BVDC_P_SUPPORT_ANR
		/* relase unneeded anr */
		if((hWindow->stCurResource.hAnr) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bAnr &&
			 !hWindow->stNewInfo.bAnr)))
		{
			BDBG_MSG(("Window %d releases shared ANR", hWindow->eId));
			BVDC_P_Anr_ReleaseConnect_isr(&hWindow->stCurResource.hAnr);
			hWindow->stNewResource.hAnr = NULL;
		}
#endif

#if BVDC_P_SUPPORT_MAD
		/* release unneeded mad */
		if((hWindow->stCurResource.hMad32) &&
		   (pCurDirty->stBits.bDestroy ||
		   (!hWindow->stCurInfo.bDeinterlace &&
		    !hWindow->stNewInfo.bDeinterlace)))
		{
			BDBG_MSG(("window %d releases shared MAD", hWindow->eId));
			BVDC_P_Mad_ReleaseConnect_isr(&hWindow->stCurResource.hMad32);
			hWindow->stNewResource.hMad32 = NULL;
		}
#endif

#if BVDC_P_SUPPORT_MCVP
		if((hWindow->stCurResource.hMcvp) &&
		   (pCurDirty->stBits.bDestroy ||
			(!hWindow->stCurInfo.bDeinterlace &&
			 !hWindow->stNewInfo.bDeinterlace &&
			 !hWindow->stCurInfo.bAnr &&
			 !hWindow->stNewInfo.bAnr)))
		{
			BDBG_MSG(("window %d releases shared MCVP", hWindow->eId));
			BVDC_P_Mcvp_ReleaseConnect_isr(&hWindow->stCurResource.hMcvp);
			hWindow->stNewResource.hMcvp = NULL;
		}
#endif

		/* The window shutdown is completed for SHUTDOWN::DESTROY. */
		if(pCurDirty->stBits.bDestroy)
		{
			BVDC_P_Source_DisconnectWindow_isr(pCurInfo->hSource, hWindow);
			hWindow->eState       = BVDC_P_State_eInactive;
			BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
			BDBG_MSG(("(4) Window[%d] is BVDC_P_State_eInactive", hWindow->eId));

			/* */
			BDBG_ASSERT(hWindow->bSetDestroyEventPending);
			BKNI_SetEvent_isr(hWindow->hDestroyDoneEvent);
			hWindow->bSetDestroyEventPending = false;
			BDBG_MSG(("(5) Window[%d] is set destroy done event", hWindow->eId));
		}
	}

	BDBG_LEAVE(BVDC_P_Window_ProcPostShutDown_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * Video Window:
 *   Shutdown -> Inactive -> Set destroy done event
 *
 * Gfx Window
 *   ShutdownPending -> Shutdown -> Inactive -> Set destroy done event
 */
void BVDC_P_Window_UpdateState_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_DirtyBits *pCurDirty;
	BDBG_ENTER(BVDC_P_Window_UpdateState_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurDirty = &hWindow->stCurInfo.stDirty;

	/* ProcPostShutDown will check whether we just completed vnet shutdown */
	BVDC_P_Window_ProcPostShutDown_isr(hWindow);

	/* handle done event if window is not mpeg on compositor that drives its
	   source (synclocked or identical timing).  Such windows are handled earlier
	   in BVDC_Source_MpegDataReady_isr. */
	if (!(hWindow->stCurInfo.hSource) ||
		!(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId)) ||
		((hWindow->hCompositor->hSyncLockSrc != hWindow->stCurInfo.hSource) &&
		 (hWindow->hCompositor->hForceTrigPipSrc != hWindow->stCurInfo.hSource)))
	{
		/* (1) apply done event. */
		if((hWindow->bSetAppliedEventPending) &&
		   ((BVDC_P_IS_CLEAN(pCurDirty)) ||
			(pCurDirty->stBits.bSrcPending && !pCurDirty->stBits.bShutdown)))
		{
			BKNI_SetEvent_isr(hWindow->hAppliedDoneEvent);
			hWindow->bSetAppliedEventPending = false;
			BDBG_MSG(("Window[%d] set apply done event", hWindow->eId));
		}
	}

	BDBG_LEAVE(BVDC_P_Window_UpdateState_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Window_UpdateUserState_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_PictureNode *pBufferFromUser = NULL;

	BDBG_ENTER(BVDC_P_Window_UpdateUserState_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Use the new applied states, and make it as current. */
	if(hWindow->bUserAppliedChanges)
	{
		uint32_t bCurShutdown;
		BVDC_P_Window_DirtyBits *pNewDirty, *pCurDirty;
		BVDC_P_State eCurReaderState = hWindow->stCurInfo.eReaderState;
		BVDC_P_State eCurWriterState = hWindow->stCurInfo.eWriterState;

		pCurDirty = &hWindow->stCurInfo.stDirty;
		pNewDirty = &hWindow->stNewInfo.stDirty;
		bCurShutdown = pCurDirty->stBits.bShutdown;

		/* Copying the new info to the current info.  Must be careful here
		 * of not globble current dirty bits set by source, but rather ORed
		 * them together. */
		BVDC_P_OR_ALL_DIRTY(pNewDirty, pCurDirty);

		if(pCurDirty->stBits.bUserReleaseBuffer)
		{
			pBufferFromUser = hWindow->stCurInfo.pBufferFromUser;
		}

		/* copy stNewInfo to stCurInfo here !!! */
		hWindow->stCurInfo = hWindow->stNewInfo;

		if(pCurDirty->stBits.bUserReleaseBuffer)
		{
			hWindow->stCurInfo.pBufferFromUser = pBufferFromUser;
		}

		/* be careful: restore the current states if current is still dirty,
		 * when new Writer/ReaderState is inactive, that is not set on purpose,
		 * it should be copied to current Writer/ReaderSate and cause
		 * SetWriterVnet_isr/SetReaderVnet_isr to called */
		if((bCurShutdown == BVDC_P_DIRTY) ||
		   (BVDC_P_State_eInactive == hWindow->stNewInfo.eReaderState))
		{
			hWindow->stCurInfo.eReaderState = eCurReaderState;
		}
		if((bCurShutdown == BVDC_P_DIRTY) ||
		   (BVDC_P_State_eInactive == hWindow->stNewInfo.eWriterState))
		{
			hWindow->stCurInfo.eWriterState = eCurWriterState;
		}

		/* Clear dirty bits since it's already OR'ed into current.  Notes
		 * the it might not apply until next vysnc, so we're defering
		 * setting the event until next vsync. */
		BVDC_P_CLEAN_ALL_DIRTY(pNewDirty);
		hWindow->bUserAppliedChanges = false;

		/* can not set them into Active here in case ApplyChanges are called
		 * twice before Writer_isr and Reader_isr are called ? */
		hWindow->stNewInfo.eWriterState = BVDC_P_State_eInactive;
		hWindow->stNewInfo.eReaderState = BVDC_P_State_eInactive;
	}

	BDBG_LEAVE(BVDC_P_Window_UpdateUserState_isr);
	return;
}

#ifndef BVDC_UINT32_ONLY

#define BVDC_P_SUB_ASPR_INT_BITS_NUM       (12)
#define BVDC_P_SUB_ASPR_FRAC_BITS_NUM      (40)
#define BVDC_P_SUB_ASPR_ALL_BITS_NUM       (BVDC_P_SUB_ASPR_INT_BITS_NUM+BVDC_P_SUB_ASPR_FRAC_BITS_NUM)

#else

/* for NOT well bounded value such as sub-rect aspect ratio value */
#define BVDC_P_SUB_ASPR_INT_BITS_NUM       10
#define BVDC_P_SUB_ASPR_FRAC_BITS_NUM      11
#define BVDC_P_SUB_ASPR_ALL_BITS_NUM       (BVDC_P_SUB_ASPR_INT_BITS_NUM+BVDC_P_SUB_ASPR_FRAC_BITS_NUM)

#endif

/***************************************************************************
 * {private}
 *
 * Utility func called by BVDC_P_Window_AdjustRectangles_isr to calculate
 * user pan scan disp size, in the case that it is not provided by stream
 *
 * The calculated user disp size is stored in hWindow context
 */
static void BVDC_P_Window_CalcuAutoDisplaySize_isr
	( BVDC_Window_Handle               hWindow,
  uintAR_t                         ulSrcPxlAspRatio,  /* U4.16 fix pt */
  uintAR_t                         ulDspPxlAspRatio ) /* U4.16 fix pt */
{
	uint32_t ulFullSrcWidth, ulFullSrcHeight;
	uint32_t ulFullDspWidth, ulFullDspHeight;
	uint32_t ulTmpWidth, ulTmpHeight;
	uintAR_t ulFullSrcAspR = 0;
	uintAR_t ulFullDspAspR = 0;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* calculate the src aspect ratio -- 4.16 fixed pointer numer */
	ulFullSrcWidth  = hWindow->ulFieldSrcWidth;
	ulFullSrcHeight = hWindow->ulFieldSrcHeight;
	ulFullSrcAspR = BVDC_P_DIV_ROUND_NEAR(
		ulSrcPxlAspRatio * ulFullSrcWidth, ulFullSrcHeight);

	/* calculate the full disp aspect ratio -- 4.16 fixed pointer numer */
	ulFullDspWidth  = hWindow->hCompositor->stCurInfo.pFmtInfo->ulWidth;
	ulFullDspHeight = hWindow->hCompositor->stCurInfo.pFmtInfo->ulHeight;
	ulFullDspAspR = BVDC_P_DIV_ROUND_NEAR(
		ulDspPxlAspRatio * ulFullDspWidth, ulFullDspHeight);

	/* calcu user pan scan disp size (cut src width or height):
	 * see the equation in BVDC_P_Window_AspectRatioCorrection_isr */
	if( ulFullSrcAspR > ulFullDspAspR )
	{
		/* needs cut cw: new cw = cw * dar / sar */
		ulTmpWidth = BVDC_P_DIV_ROUND_NEAR(
			ulFullSrcWidth *  ulFullDspAspR, ulFullSrcAspR);
		hWindow->ulAutoDispHorizontalSize = ulTmpWidth;
		hWindow->ulAutoDispVerticalSize   = ulFullSrcHeight;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("User pan scan cut W: new width %d", ulTmpWidth));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
			(ulTmpWidth * ulFullSrcAspR) / ulFullSrcWidth, ulFullDspAspR));
	}
	else if( ulFullSrcAspR < ulFullDspAspR )
	{
		/* needs cut ch: new ch = ch * sar / dar */
		ulTmpHeight = BVDC_P_DIV_ROUND_NEAR(
			ulFullSrcHeight * ulFullSrcAspR, ulFullDspAspR);
		hWindow->ulAutoDispHorizontalSize = ulFullSrcWidth;
		hWindow->ulAutoDispVerticalSize   = ulTmpHeight;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("User pan scan cut H: new height %d", ulTmpHeight));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
			(ulFullSrcHeight * ulFullSrcAspR) / ulTmpHeight, ulFullDspAspR));
	}
	else
	{
		/* no cut is needed */
		hWindow->ulAutoDispHorizontalSize = ulFullSrcWidth;
		hWindow->ulAutoDispVerticalSize   = ulFullSrcHeight;
	}
}

static void BVDC_P_Window_Bar_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Rect                     *pSrcCnt,            /* interm src cnt, in and out */
	  BAVC_BarDataType                 eBarDataType,       /* specify top/bottom or left/right data */
	  uint32_t                         ulTopLeftBarValue,  /* either the top or left bar data value */
	  uint32_t                         ulBotRightBarValue) /* either the bottom or right bar data value */
{
	uint32_t ulTotalSrcCut, ulSrcLength;

	BDBG_ENTER(BVDC_P_Window_Bar_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pSrcCnt);

	ulTotalSrcCut = ulTopLeftBarValue + ulBotRightBarValue;
	ulSrcLength = (BAVC_BarDataType_eTopBottom==eBarDataType)?
		pSrcCnt->ulHeight: pSrcCnt->ulWidth;



	/* Validate for valid Bar value */
	if(ulSrcLength <= ulTotalSrcCut)
	{
		BDBG_ERR(("Invalid Bar Data value: BarDataType %s %d %d",
			(BAVC_BarDataType_eTopBottom==eBarDataType)?"TopBottom":"LeftRight",
			ulTopLeftBarValue, ulBotRightBarValue));
		return;
	}

	if(BAVC_BarDataType_eTopBottom==eBarDataType)
	{
		pSrcCnt->lTop = ulTopLeftBarValue<< BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->lLeft= 0;
		pSrcCnt->ulHeight -= ulTotalSrcCut;
	}
	else
	{
		pSrcCnt->lTop  = 0 ;
		pSrcCnt->lLeft = ulTopLeftBarValue<< BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->ulWidth -= ulTotalSrcCut;
	}


	pSrcCnt->lLeft_R = pSrcCnt->lLeft;

	BDBG_LEAVE(BVDC_P_Window_Bar_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * This function clip the source based on AFD. When AFD is used, box
 * auto cut  and pan scan are ignored, and AFD is the first clipping
 * applied to src.  It only affect hWindow->stSrcCnt.
 *
 * It is only used for mpeg src. It is called at every vsync when
 * RUL is built, by BVDC_P_Window_AdjustRectangles_isr.
 */
static const BVDC_P_Afd s_a4x3AfdInfo[] =
{
	             /*  a  h0    v0    h1    v1    h2    v2    comment */
	BVDC_P_MAKE_AFD( 0, 0,    0,    0,    0,    0,    0,    "No AFD Specified No AFD Specified"),
	BVDC_P_MAKE_AFD( 1, 0,    0,    0,    0,    0,    0,    "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 2, 0,    2500, 0,    2500, 0,    2500, "16x9 letterbox top"),
	BVDC_P_MAKE_AFD( 3, 0,    1250, 0,    1250, 0,    1250, "14x9 letterbox top"),

	BVDC_P_MAKE_AFD( 4, 0,    1250, 0,    1250, 0,    1250, "Greater than 16x9 letterbox image"),
	BVDC_P_MAKE_AFD( 5, 0,    0,    0,    0,    0,    0,    "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 6, 0,    0,    0,    0,    0,    0,    "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 7, 0,    0,    0,    0,    0,    0,    "Reserved Reserved"),

	BVDC_P_MAKE_AFD( 8, 0,    0,    0,    0,    0,    0,    "4x3 full frame image"),
	BVDC_P_MAKE_AFD( 9, 0,    0,    0,    0,    0,    0,    "4x3 full frame image"),
	BVDC_P_MAKE_AFD(10, 0,    1250, 0,    1250, 0,    1250, "16x9 letterbox image"),
	BVDC_P_MAKE_AFD(11, 0,    625,  0,    625,  0,    625,  "14x9 letterbox image"),

	BVDC_P_MAKE_AFD(12, 0,    0,    0,    0,    0,    0,    "Reserved Reserved"),
	BVDC_P_MAKE_AFD(13, 0,    0,    0,    625,  0,    625,  "4x3 full frame image, alt 14x9 center"),
	BVDC_P_MAKE_AFD(14, 0,    1250, 625,  1250, 625,  1250, "16x9 letterbox image, alt 14x9 center"),
	BVDC_P_MAKE_AFD(15, 0,    1250, 625,  1250, 1250, 1250, "16x9 letterbox image, alt 4x3 center ")
};

static const BVDC_P_Afd s_a16x9AfdInfo[] =
{
	             /*  a  h0    v0   h1    v1   h2    v2   comment */
	BVDC_P_MAKE_AFD( 0, 0,    0,   0,    0,   0,    0,   "No AFD Specified No AFD Specified"),
	BVDC_P_MAKE_AFD( 1, 0,    0,   0,    0,   0,    0,   "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 2, 0,    0,   0,    0,   0,    0,   "16x9 letterbox top"),
	BVDC_P_MAKE_AFD( 3, 625,  0,   625,  0,   625,  0,   "14x9 letterbox top"),

	BVDC_P_MAKE_AFD( 4, 0,    625, 0,    625, 0,    625, "Greater than 16x9 letterbox image"),
	BVDC_P_MAKE_AFD( 5, 0,    0,   0,    0,   0,    0,   "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 6, 0,    0,   0,    0,   0,    0,   "Reserved Reserved"),
	BVDC_P_MAKE_AFD( 7, 0,    0,   0,    0,   0,    0,   "Reserved Reserved"),

	BVDC_P_MAKE_AFD( 8, 0,    0,   0,    0,   0,    0,   "4x3 full frame image"),
	BVDC_P_MAKE_AFD( 9, 1250, 0,   1250, 0,   1250, 0,   "4x3 full frame image"),
	BVDC_P_MAKE_AFD(10, 0,    0,   0,    0,   0,    0,   "16x9 letterbox image"),
	BVDC_P_MAKE_AFD(11, 625,  0,   625,  0,   625,  0,   "14x9 letterbox image"),

	BVDC_P_MAKE_AFD(12, 0,    0,   0,    0,   0,    0,   "Reserved Reserved"),
	BVDC_P_MAKE_AFD(13, 1250, 0,   1250, 625, 1250, 625, "4x3 full frame image, alt 14x9 center"),
	BVDC_P_MAKE_AFD(14, 0,    0,   625,  0,   625,  0,   "16x9 letterbox image, alt 14x9 center"),
	BVDC_P_MAKE_AFD(15, 0,    0,   625,  0,   1250, 0,   "16x9 letterbox image, alt 4x3 center ")
};

static void BVDC_P_Window_Afd_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Rect                     *pSrcCnt,          /* interm src cnt, in and out */
	  uint32_t                         ulAfdVal,         /* AFD value */
	  BFMT_AspectRatio                 eSrcAspectRatio,  /* aspect ratio of coded frame */
	  uint32_t                         ulSampleAspectRatioX, /* width of one sampled src pixel */
	  uint32_t                         ulSampleAspectRatioY, /* height of one sampled src pixel */
	  uint32_t                         ulSrcWidth,          /* source width */
	  uint32_t                         ulSrcHeight )        /* source height */

{
	uint32_t ulHClip, ulVClip;
	const BVDC_P_Afd *pAfd;
	BFMT_AspectRatio  eAR;

	BDBG_ENTER(BVDC_P_Window_Afd_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pSrcCnt);

	if(eSrcAspectRatio == BFMT_AspectRatio_eSAR)
	{
		uint32_t ulDAR = (ulSrcWidth * ulSampleAspectRatioX * 100) / (ulSrcHeight * ulSampleAspectRatioY);
		eAR = BVDC_P_EQ_DELTA(ulDAR, 133, 25)
			? BFMT_AspectRatio_e4_3 :
			  (BVDC_P_EQ_DELTA(ulDAR, 177, 25) ? BFMT_AspectRatio_e16_9 : BFMT_AspectRatio_eUnknown);
		BDBG_MSG(("SrcW = %d, SrcH = %d, SarX = %d, SarY = %d, ulDAR = %d, AR = %d",
			ulSrcWidth, ulSrcHeight, ulSampleAspectRatioX, ulSampleAspectRatioY, ulDAR, eAR));
	}
	else
	{
		eAR = eSrcAspectRatio;
	}

	/* only support 4x3 and 16x9 source aspect ration for now */
	if(eAR != BFMT_AspectRatio_e4_3 && eAR != BFMT_AspectRatio_e16_9)
	{
		BDBG_WRN(("AFD is not supported for source aspect ratio %d", eAR));
		return;
	}

	/* Check for valid AFD value */
	if(ulAfdVal >= (sizeof(s_a4x3AfdInfo) / sizeof(BVDC_P_Afd)) ||
	   ulAfdVal >= (sizeof(s_a16x9AfdInfo) / sizeof(BVDC_P_Afd)))
	{
		BDBG_ERR(("Invalid AFD value"));
		return;
	}

	pAfd = (eAR == BFMT_AspectRatio_e4_3) ? &(s_a4x3AfdInfo[ulAfdVal]) : &(s_a16x9AfdInfo[ulAfdVal]);
	ulHClip = pAfd->aulHClip[hWindow->stCurInfo.stAfdSettings.eClip];
	ulVClip = pAfd->aulVClip[hWindow->stCurInfo.stAfdSettings.eClip];

	if(eAR == BFMT_AspectRatio_e4_3 && (ulAfdVal == 2 || ulAfdVal == 3))
	{
		pSrcCnt->lTop = 0;
		pSrcCnt->ulHeight = pSrcCnt->ulHeight - (pSrcCnt->ulHeight * ulVClip / 10000);
		pSrcCnt->lLeft = (pSrcCnt->ulWidth * ulHClip / 10000) << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->ulWidth = pSrcCnt->ulWidth - (pSrcCnt->ulWidth * ulHClip * 2 / 10000);
	}
	else
	{
		pSrcCnt->lTop = (pSrcCnt->ulHeight * ulVClip / 10000) << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->ulHeight = pSrcCnt->ulHeight - (pSrcCnt->ulHeight * ulVClip * 2 / 10000);
		pSrcCnt->lLeft = (pSrcCnt->ulWidth * ulHClip / 10000) << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->ulWidth = pSrcCnt->ulWidth - (pSrcCnt->ulWidth * ulHClip * 2 / 10000);
	}

	pSrcCnt->lLeft_R = pSrcCnt->lLeft;

	BDBG_LEAVE(BVDC_P_Window_Afd_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * This function clip the src based on pan-scan. When pan scan is used, box
 * auto cut is ignored, and pan-scan is the first clipping applied to src.
 * It only affect hWindow->stSrcCnt.
 *
 * It is only used for mpeg and hddvi src. It is called at every vsync when
 * RUL is built, by BVDC_P_Window_AdjustRectangles_isr.
 */
static void BVDC_P_Window_PanScan_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Rect                     *pSrcCnt,  /* interm src cnt, in and out */
	  uint32_t                         ulSrcDispWidth,    /* pan disp width */
	  uint32_t                         ulSrcDispHeight,   /* pan disp width */
	  int32_t                          lHorizontalPanScan,/* stream pan x, 11.4 */
	  int32_t                          lVerticalPanScan ) /* stream pan y, 11.4 */
{
	const BVDC_P_Window_Info *pUserInfo;
	int32_t lZeroHorzPanScan, lZeroVertPanScan;
	uint32_t ul16DispWidth, ul16DispHeight; /* src pan disp size, 11.4.fixed ptr */
	uint32_t ul16SrcWidth, ul16SrcHeight;   /* src full size, 11.4.fixed ptr */
	int32_t lCntXMin; /* src x_min, 11.4 fixed ptr */
	int32_t lCntXMax; /* src x_max, 11.4.fixed ptr */
	int32_t lCntYMin; /* src y_min, 11.4.fixed ptr */
	int32_t lCntYMax; /* src y_max, 11.4.fixed ptr */

	BDBG_ENTER(BVDC_P_Window_PanScan_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pSrcCnt);
	pUserInfo = &hWindow->stCurInfo;

	/* When pan scan is used, box auto cut is ignored, and pan-scan is the
	 * first clipping apllied to src. */
	ul16SrcWidth  = pSrcCnt->ulWidth  << BVDC_P_16TH_PIXEL_SHIFT; /* 11.4 fixed ptr */
	ul16SrcHeight = pSrcCnt->ulHeight << BVDC_P_16TH_PIXEL_SHIFT; /* 11.4 fixed ptr */
	ul16DispWidth  = ulSrcDispWidth  << BVDC_P_16TH_PIXEL_SHIFT; /* 11.4 fixed ptr */
	ul16DispHeight = ulSrcDispHeight << BVDC_P_16TH_PIXEL_SHIFT; /* 11.4 fixed ptr */

	/* when srcDisp size > src, it does not mean to further cut src, but adjust
	 * the position of display window, hence we don't need to do anything, and let
	 * user to set scl-out and dest window in _isr
	 * TODO: auto adjust scl-out and dest rect in VDC */
	if((ul16DispWidth > ul16SrcWidth) || (ul16DispHeight > ul16SrcHeight))
	{
		return;
	}

	/* Zero panscan vector specifies the position of the top left corner of the
	 * display rectangle from the top left corner of the decompressed frame.
	 * Horizontal pan scan = float(dWidth/2)
	 * Integer part i = dWidth >> 1
	 * Fraction part f = dWidth % 2
	 * To convert it = (i << 1 ) + (f)
	 * = ((dWidth >> 1) << 1) + (dWidth % 2) = dWidth */
	/* Zero pan scan in units of 1/16 pixel and signed two's complement format; */
	lZeroHorzPanScan = ((int32_t) ul16SrcWidth  - (int32_t) ul16DispWidth)  >> 1;
	lZeroVertPanScan = ((int32_t) ul16SrcHeight - (int32_t) ul16DispHeight) >> 1;
	/* BDBG_MODULE_MSG(BVDC_ASP_RAT,("PANSCAN is enabled: type = %d", pUserInfo->ePanScanType)); */

	if(pUserInfo->ePanScanType == BVDC_PanScanType_eStream)
	{
		/* Use zero pan scan plus stream pan scan;
		 * Stream pan scan in units of 1/16th of a pixel in two's complement.
		 * Bit 15 is the sign bit. Bits 14:8 are the macro block grid.
		 * Bits 7:4 are the pixel grid. Bits 3:0 are 1/16 pixel grid.
		 * calculate in two's complement; remember stream panscan vector
		 * specifies the position of the centre of the reconstructed frame
		 * from the centre of the display rectangle.
		 * ResultVector := ZeroVector - StreamVector */
		lCntXMin = (lZeroHorzPanScan - lHorizontalPanScan);
		lCntYMin = (lZeroVertPanScan - lVerticalPanScan);
	}
	else if(pUserInfo->ePanScanType == BVDC_PanScanType_eUser)
	{
		/* Get user pan scan in units of 1/16 pixel and two's complement format;
		 * assume user panscan vector follows the same convention of mpeg stream.
		 * ResultVector := ZeroVector - UserVector */
		lCntXMin = (lZeroHorzPanScan - (pUserInfo->lUserHorzPanScan << BVDC_P_16TH_PIXEL_SHIFT));
		lCntYMin = (lZeroVertPanScan - (pUserInfo->lUserVertPanScan << BVDC_P_16TH_PIXEL_SHIFT));
	}
	else
	{
		/* This must be reserved.  For BVDC_PanScanType_eStreamAndUser mode, the
		 * vector is actually:
		 * ResultVector := ZeroVector - StreamVector - UserVector */
		BDBG_ASSERT(BVDC_PanScanType_eStreamAndUser == pUserInfo->ePanScanType);

		lCntXMin = (lZeroHorzPanScan - lHorizontalPanScan -
					(pUserInfo->lUserHorzPanScan << BVDC_P_16TH_PIXEL_SHIFT));
		lCntYMin = (lZeroVertPanScan - lVerticalPanScan -
					(pUserInfo->lUserVertPanScan << BVDC_P_16TH_PIXEL_SHIFT));
	}

	/* convert to U27.4 fixed point;
	 * Note: it's always non-negative(bound by source boundary left/top). */
	lCntXMin = (lCntXMin > 0)? lCntXMin : 0;
	lCntYMin = (lCntYMin > 0)? lCntYMin : 0;

	/* the width/height comes from source display size */
	lCntXMax = lCntXMin + (int32_t) ul16DispWidth;
	lCntYMax = lCntYMin + (int32_t) ul16DispHeight;

	/* Make sure the window source content is bound by source
	 * boundary(right/bottom). */
	if(lCntXMax > (int32_t)(ul16SrcWidth))
	{
		lCntXMax = ul16SrcWidth;
		lCntXMin = lCntXMax - (int32_t) ul16DispWidth;
	}

	if(lCntYMax > (int32_t)(ul16SrcHeight))
	{
		lCntYMax = ul16SrcHeight;
		lCntYMin = lCntYMax - (int32_t) ul16DispHeight;
	}

	/* store result to hWindow->stSrcCnt */
	pSrcCnt->lLeft    = lCntXMin;
	pSrcCnt->lTop     = lCntYMin;
	pSrcCnt->lLeft_R  = lCntXMin;
	pSrcCnt->ulWidth  = (lCntXMax - lCntXMin) >> BVDC_P_16TH_PIXEL_SHIFT;
	pSrcCnt->ulHeight = (lCntYMax - lCntYMin) >> BVDC_P_16TH_PIXEL_SHIFT;

	BDBG_LEAVE(BVDC_P_Window_PanScan_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 * This function clip the src based on box black patch auto cut. If pan
 * scan is used, box auto cut is ignored. However, this function might
 * still be called to report box detect info to user through box detetct call
 * back func. When box auto cut is used, it is the first clipping applied to
 * src. It only affect hWindow->stSrcCnt.
 *
 * It is called only for video window. It is called at every vsync when RUL
 * is built, by BVDC_P_Window_AdjustRectangles_isr.
 */
static void BVDC_P_Window_BoxCut_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Rect                     *pSrcCnt,  /* interm src cnt, in and out */
	  bool                             bDoPanScan )
{
	BVDC_P_Rect stBoxCut;
	uint32_t  ulSrcWidth, ulSrcHeight; /* full src size */
	const BVDC_BoxDetectInfo  *pBoxInfo;
	uint32_t  ulCallBackCntr;

	BDBG_ENTER(BVDC_P_Window_BoxCut_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pSrcCnt);

	/* no clipping has applied to src yet */
	ulSrcWidth  = pSrcCnt->ulWidth;
	ulSrcHeight = pSrcCnt->ulHeight;

	/* stBoxCut used for in and out by BVDC_P_BoxDetect_Do_isr. */
	stBoxCut.lTop = 0;
	stBoxCut.lLeft_R = 0;
	stBoxCut.lLeft = 0;
	stBoxCut.ulWidth = ulSrcWidth;
	stBoxCut.ulHeight = ulSrcHeight;

	/* Check hw reg, and decide cutting.
	 * Note: When pan-scan is enabled, auto-box-cut is ignored. However,
	 * even if in this case. this func should still be called because
	 * of BoxDetectCallBack calling might still be needed */
	BVDC_P_BoxDetect_GetStatis_isr(hWindow->stCurResource.hBoxDetect,
		&stBoxCut, &pBoxInfo, &ulCallBackCntr);

	if((NULL != hWindow->stCurInfo.BoxDetectCallBack) &&
		(NULL != pBoxInfo) &&
		(ulCallBackCntr != hWindow->ulBoxDetectCallBckCntr))
	{
		(*hWindow->stCurInfo.BoxDetectCallBack) (
			hWindow->stCurInfo.pvBoxDetectParm1,
			hWindow->stCurInfo.iBoxDetectParm2, pBoxInfo);
		hWindow->ulBoxDetectCallBckCntr = ulCallBackCntr;
	}

	/* when pan scan is enabled, box auto cut is ignored */
	if((! bDoPanScan) && (hWindow->stCurInfo.bAutoCutBlack))
	{
		/* apply the box auto cut to hWindow->stSrcCnt. note that with the 1st
		 * BVDC_P_BOX_DETECT_NUM_ACCUM vsync stBoxCut.ulWidth/ulHeiight are
		 * BVDC_P_BOX_DETECT_MAX_EDGE */
		pSrcCnt->lLeft = stBoxCut.lLeft << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->lLeft_R = stBoxCut.lLeft_R << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->lTop  = stBoxCut.lTop  << BVDC_P_16TH_PIXEL_SHIFT;
		pSrcCnt->ulWidth  = stBoxCut.ulWidth;
		pSrcCnt->ulHeight = stBoxCut.ulHeight;

		BDBG_ASSERT(stBoxCut.lLeft >= 0);
		BDBG_ASSERT(stBoxCut.lLeft_R >= 0);
		BDBG_ASSERT(stBoxCut.lTop  >= 0);
		BDBG_ASSERT(stBoxCut.lLeft + pSrcCnt->ulWidth  <= ulSrcWidth);
		BDBG_ASSERT(stBoxCut.lTop  + pSrcCnt->ulHeight <= ulSrcHeight);
	}

	BDBG_LEAVE(BVDC_P_Window_BoxCut_isr);
}


/***************************************************************************
 * {private}
 *
 * This function applys the user set src clip (including the setting by _isr)
 * to hWindow->stSrcCnt, which is used to set picture nodes. This func is
 * called after box auto cut and pan-scan. This func only affect
 * hWindow->stSrcCnt.
 *
 * If the src is mpeg, hddvi, or if letter box auto back cut is enabled, it is
 * called at every vsync when RUL is built. Otherwise, it is called only once
 * at the first vsync after ApplyChanges.
 *
 * It is only used by BVDC_P_Window_AdjustRectangles_isr
 */
static void BVDC_P_Window_UserSrcCut_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Rect                     *pSrcCnt,
	  const BVDC_P_ClipRect           *pSrcClp )
{
	BDBG_ENTER(BVDC_P_Window_UserSrcCut_isr);
	BDBG_ASSERT(pSrcCnt);
	BSTD_UNUSED(hWindow);

	/* Source clip, and ignore over clipping */
	if(((pSrcClp->ulLeft + pSrcClp->ulRight)  < pSrcCnt->ulWidth) &&
	   ((pSrcClp->ulTop  + pSrcClp->ulBottom) < pSrcCnt->ulHeight))
	{
		pSrcCnt->lLeft    += (pSrcClp->ulLeft << BVDC_P_16TH_PIXEL_SHIFT);
		pSrcCnt->ulWidth  -= (pSrcClp->ulLeft + pSrcClp->ulRight);
		pSrcCnt->lTop     += (pSrcClp->ulTop  << BVDC_P_16TH_PIXEL_SHIFT);
		pSrcCnt->ulHeight -= (pSrcClp->ulTop  + pSrcClp->ulBottom);

#if (BVDC_P_SUPPORT_3D_INDEP_SRC_CLIP)
		pSrcCnt->lLeft_R = pSrcCnt->lLeft +
			(pSrcClp->lLeftDelta_R << BVDC_P_16TH_PIXEL_SHIFT);
#else
		pSrcCnt->lLeft_R = pSrcCnt->lLeft;
#endif
	}

	BDBG_LEAVE(BVDC_P_Window_UserSrcCut_isr);
	return;
}

/* Those THD values are set to 0 to work around PEP's TNT(sharp) problem with
 * CMP_0_V0_SURFACE_SIZE > 1920. 8 should be a good number if the TNT problem is gone.
 */
/* Smooth scaling is enabled on 7405, 7325 and 7335. Need to use smaller
 * THD values, otherwise VFD don't have enough bandwidth.
 */
#define BVDC_P_DST_CUT_TOP_THRESH      (0)
#define BVDC_P_DST_CUT_BOTTOM_THRESH   (0)
#define BVDC_P_DST_CUT_LEFT_THRESH     (0)
#define BVDC_P_DST_CUT_RIGHT_THRESH    (0)

/***************************************************************************
 * {private}
 *
 * It is only used by BVDC_P_Window_AdjustRectangles_isr
 */
static void BVDC_P_Window_AdjDstCut_isr
	( const BFMT_VideoInfo            *pDstFmtInfo,
	  BVDC_P_Rect                     *pSrcCnt,
	  BVDC_P_Rect                     *pAdjSclOut,
	  const BVDC_P_Rect               *pAdjDstRect )
{
	uint32_t  ulOldSclOutRightCut, ulOldSclOutBottomCut;
	uint32_t  ulNewSclOutLeft, ulNewSclOutWidth, ulNewSclOutLeft_R;
	uint32_t  ulNewSclOutTop, ulNewSclOutHeight;
	uint32_t  ulOffCntLeft, ulOffCntTop, ulOffCntLeft_R;
	uint32_t  ulBottomThresh, ulRightThresh;
	int32_t   lTopThresh, lLeftThresh;

	BDBG_ENTER(BVDC_P_Window_AdjDstCut_isr);
	BDBG_ASSERT(pSrcCnt);

	/* If SclOut is larger than canvas, transfer to src cut, otherwise */
	/* allow dst cut */
	if(pAdjSclOut->ulWidth  > pDstFmtInfo->ulWidth ||
	   pAdjSclOut->ulHeight > pDstFmtInfo->ulHeight)
	{
		lTopThresh     = 0;
		ulBottomThresh = 0;
		lLeftThresh    = 0;
		ulRightThresh  = 0;
	}
	else
	{
		lTopThresh     = BVDC_P_DST_CUT_TOP_THRESH;
		ulBottomThresh = BVDC_P_DST_CUT_BOTTOM_THRESH;
		lLeftThresh    = BVDC_P_DST_CUT_LEFT_THRESH;
		ulRightThresh  = BVDC_P_DST_CUT_RIGHT_THRESH;
	}

	/* Transferfing large dest clip to src clip.  Currently when destination cut
	 * (specially from top) is relative large the system could run into bandwidth
	 * issues.  Here we're transfering the cut to source, so we can cut early in
	 * the process to save bandwidth. */
	ulOldSclOutRightCut =
		pAdjSclOut->ulWidth - pAdjSclOut->lLeft - pAdjDstRect->ulWidth;
	if((lLeftThresh < pAdjSclOut->lLeft) || (lLeftThresh < pAdjSclOut->lLeft_R) ||
	   (ulRightThresh < ulOldSclOutRightCut))
	{
		/* prepare to xfer hrz dst cut into src cut*/
		ulNewSclOutLeft = (lLeftThresh < pAdjSclOut->lLeft)?
			(uint32_t)lLeftThresh : (uint32_t)pAdjSclOut->lLeft;
		ulNewSclOutLeft_R = (lLeftThresh < pAdjSclOut->lLeft_R)?
			(uint32_t)lLeftThresh : (uint32_t)pAdjSclOut->lLeft_R;


		ulNewSclOutWidth = (ulRightThresh < ulOldSclOutRightCut)?
			(ulNewSclOutLeft + pAdjDstRect->ulWidth + ulRightThresh):
			(ulNewSclOutLeft + pAdjDstRect->ulWidth + ulOldSclOutRightCut);

		/* add src left cut */
		ulOffCntLeft = BVDC_P_DIV_ROUND_NEAR(
			(((pAdjSclOut->lLeft - ulNewSclOutLeft) * pSrcCnt->ulWidth) <<
			 BVDC_P_16TH_PIXEL_SHIFT), pAdjSclOut->ulWidth);
		pSrcCnt->lLeft += ulOffCntLeft;

		ulOffCntLeft_R = BVDC_P_DIV_ROUND_NEAR(
			(((pAdjSclOut->lLeft_R - ulNewSclOutLeft_R) * pSrcCnt->ulWidth) <<
			 BVDC_P_16TH_PIXEL_SHIFT), pAdjSclOut->ulWidth);
		pSrcCnt->lLeft_R += ulOffCntLeft;

		/* add src width cut: set cnt width a little big to ensure no edge content
		 * loss, it is ok to do so because phase and SrcStep also control how much
		 * edge src contribute */
		pSrcCnt->ulWidth = BVDC_P_DIV_ROUND_UP(
			ulNewSclOutWidth * pSrcCnt->ulWidth, pAdjSclOut->ulWidth);

		/* Adjust pSclOut since we already move the cut to source. */
		pAdjSclOut->lLeft   = ulNewSclOutLeft;
		pAdjSclOut->lLeft_R = ulNewSclOutLeft_R;
		pAdjSclOut->ulWidth = ulNewSclOutWidth;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("dstCutXfer: AdjCnt left %d/16(%d), width %d, sclOut left %d, width %d",
			pSrcCnt->lLeft, pSrcCnt->lLeft/16, pSrcCnt->ulWidth, pAdjSclOut->lLeft, pAdjSclOut->ulWidth));
	}

	ulOldSclOutBottomCut =
		pAdjSclOut->ulHeight - pAdjSclOut->lTop - pAdjDstRect->ulHeight;
	if((lTopThresh < pAdjSclOut->lTop) ||
	   (ulBottomThresh < ulOldSclOutBottomCut))
	{
		/* prepare to xfer vrt dst cut into src cut*/
		ulNewSclOutTop = (lTopThresh < pAdjSclOut->lTop)?
			ulBottomThresh : (uint32_t)pAdjSclOut->lTop;
		ulNewSclOutHeight = (ulBottomThresh < ulOldSclOutBottomCut)?
			(ulNewSclOutTop + pAdjDstRect->ulHeight + ulBottomThresh):
			(ulNewSclOutTop + pAdjDstRect->ulHeight + ulOldSclOutBottomCut);

		/* add src top cut */
		ulOffCntTop = BVDC_P_DIV_ROUND_NEAR(
			(((pAdjSclOut->lTop - ulNewSclOutTop) * pSrcCnt->ulHeight) <<
			 BVDC_P_16TH_PIXEL_SHIFT), pAdjSclOut->ulHeight);
		pSrcCnt->lTop += ulOffCntTop;

		/* add src height cut: set cnt height a little big to ensure no edge content
		 * loss, it is ok to do so because phase and SrcStep also control how much
		 * edge src contribute */
		pSrcCnt->ulHeight = BVDC_P_DIV_ROUND_UP(
			ulNewSclOutHeight * pSrcCnt->ulHeight, pAdjSclOut->ulHeight);

		/* Adjust pSclOut since we already move the cut to source. */
		pAdjSclOut->lTop   = ulNewSclOutTop;
		pAdjSclOut->ulHeight = ulNewSclOutHeight;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("dstCutXfer: AdjCnt top %d/16(%d), height %d, sclOut top %d, height %d",
			pSrcCnt->lTop, pSrcCnt->lTop/16, pSrcCnt->ulHeight, pAdjSclOut->lTop, pAdjSclOut->ulHeight));
	}

	BDBG_LEAVE(BVDC_P_Window_AdjDstCut_isr);
	return;
}

#define BVDC_P_WIN_ROUND_CUT_OVER_ORIG        0
#if (BVDC_P_WIN_ROUND_CUT_OVER_ORIG == 1)
/***************************************************************************
 * {private}
 *
 * Utility function called by BVDC_P_Window_AspectRatioCorrection_isr to
 * round the (ulCutLen / ulFullLen) to i/16, iff the rounded ulCutLen really
 * makes the ratio exactly == i/16, and Src / Dst offset is inside
 * ulSclFctRndToler %
 */
static uint32_t BVDC_P_Window_CutRounding_isr
	( uint32_t                         ulSclFctRndToler,
	  uint32_t                         ulCutLen,
	  uint32_t                         ulFullLen )
{
	uint32_t ul16TmpLen, ul16CutRatio;
	uint32_t ulOff, ulTmpLen, ulNewLen;

	ulNewLen = ulCutLen;

	if((0 != ulSclFctRndToler) && (ulCutLen < ulFullLen) && (0 != ulCutLen))
	{
		ul16CutRatio = BVDC_P_DIV_ROUND_NEAR(ulCutLen * 16, ulFullLen);
		ul16TmpLen = ul16CutRatio * ulFullLen;
		if(0 == (ul16TmpLen & 15)) /* fully devided by 16 */
		{
			ulTmpLen = ul16TmpLen / 16;
			ulOff = (ulTmpLen < ulCutLen)? (ulCutLen - ulTmpLen): (ulTmpLen - ulCutLen);
			if(200 * ulOff < ulCutLen * ulSclFctRndToler)
			{
				ulNewLen = ulTmpLen;
			}
		}
	}

	return BVDC_P_MIN(ulNewLen, ulFullLen);
}
#else
#define BVDC_P_Window_CutRounding_isr(t, c, f)  BVDC_P_MIN(c, f)
#endif

/* temporary struct for auto adjust status */
typedef struct BVDC_P_AutoAdj
{
	uint32_t     ulAdjFlags;  /* what is adjusted */

	uint32_t     ulCntWidth;  /* new cntWidth */
	uint32_t     ulOutWidth;  /* new SclOutWidth */
	uint32_t     ulCntHeight; /* new cntHeight */
	uint32_t     ulOutHeight; /* new SclOutHeight */
} BVDC_P_AutoAdj;

#define BVDC_P_ADJ_CNT_CUT            (0x1)
#define BVDC_P_ADJ_OUT_CUT            (0x2)
#define BVDC_P_ADJ_SRC_STEP           (0x4)
#define BVDC_P_ADJ_FLAG_HRZ_SHIFT     (0)
#define BVDC_P_ADJ_FLAG_VRT_SHIFT     (4)

#define BVDC_P_ADJ_CNT_WIDTH          (BVDC_P_ADJ_CNT_CUT  << BVDC_P_ADJ_FLAG_HRZ_SHIFT)
#define BVDC_P_ADJ_OUT_WIDTH          (BVDC_P_ADJ_OUT_CUT  << BVDC_P_ADJ_FLAG_HRZ_SHIFT)
#define BVDC_P_ADJ_HRZ_SRC_STEP       (BVDC_P_ADJ_SRC_STEP << BVDC_P_ADJ_FLAG_HRZ_SHIFT)
#define BVDC_P_ADJ_CNT_HEIGHT         (BVDC_P_ADJ_CNT_CUT  << BVDC_P_ADJ_FLAG_VRT_SHIFT)
#define BVDC_P_ADJ_OUT_HEIGHT         (BVDC_P_ADJ_OUT_CUT  << BVDC_P_ADJ_FLAG_VRT_SHIFT)
#define BVDC_P_ADJ_VRT_SRC_STEP       (BVDC_P_ADJ_SRC_STEP << BVDC_P_ADJ_FLAG_VRT_SHIFT)

/***************************************************************************
 * {private}
 *
 * This function atomatically cut the window's content size (width or height)
 * or scl-out-rect size (width or height), to enforce correct aspect ratio.
 * It outputs the new content and scl-out-rect size, and appropriate AdjFlags
 * bits to indicate what has changed, by modifying the struct BVDC_P_AutoAdj
 * pointed by pAutoAdj. ulAdjFlags is also set
 *
 * If AspectRatioMode is Zoom, it will cut src rect to make it have the same
 * aspect ratio as the scaler-out-rect. When AspectRatioMode is Box, it will
 * cut the scaler-out-rect to make the src aspect ration unchanged after
 * scaling.
 *
 * When this func is called, it is called after box auto cut, pan scan, and
 * user set src/dst clipping. It assumes that aspect-ratio mode is set to
 * either box or zoom, and that it is not in non-linear scale mode.
 *
 * Theoretically, if the src is mpeg, hddvi, or if letter box auto back cut
 * is enabled, it should be called at every vsync when RUL is built.
 * Otherwise, it should be called only once at the first vsync after
 * ApplyChanges.
 *
 * Optimize: aspect ratio correctio and scale factor rounding needs re-do
 * only if values of SrcCut changed after box cut, pan scan, and user clip,
 * or right after ApplyChanges has been called.
 */
static void BVDC_P_Window_AspectRatioCorrection_isr
	( BVDC_Window_Handle               hWindow,
  uintAR_t                         ulSrcPxlAspRatio,  /* U4.16 value */
  uintAR_t                         ulDspPxlAspRatio,  /* U4.16 value */
	  BVDC_P_AutoAdj                  *pAutoAdj )         /* auto adj */
{
	const BVDC_P_Window_Info *pUserInfo;
	uint32_t  ulSclOutWidth, ulSclOutHeight;
	uint32_t  ulCntLeft, ulCntTop, ulCntWidth, ulCntHeight;
	uintAR_t  ulCntAspR, ulDspAspR;
	uint32_t  ulTmpWidth, ulTmpHeight;

	BDBG_ENTER(BVDC_P_Window_AspectRatioCorrection_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pAutoAdj);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	pUserInfo = &hWindow->stCurInfo;
	BDBG_ASSERT((BVDC_AspectRatioMode_eUseAllDestination == pUserInfo->eAspectRatioMode) ||
				(BVDC_AspectRatioMode_eUseAllSource == pUserInfo->eAspectRatioMode));

	ulCntLeft   = hWindow->stSrcCnt.lLeft;
	ulCntWidth  = hWindow->stSrcCnt.ulWidth;
	ulCntTop    = hWindow->stSrcCnt.lTop;
	ulCntHeight = hWindow->stSrcCnt.ulHeight;
	ulSclOutWidth  = hWindow->stAdjSclOut.ulWidth;
	ulSclOutHeight = hWindow->stAdjSclOut.ulHeight;
	if((0 != ulSclOutWidth)  &&
	   (0 != ulSclOutHeight) &&
	   (0 != ulCntWidth)  &&
	   (0 != ulCntHeight))
	{
		/* Lets use notation as the following
		 * cw:  content width
		 * ch:  content height
		 * spar: src pixel aspect ratio value in U4.16
		 * dw:  scaler output width
		 * dh:  scaler output height
		 * dpar: display pixel aspect ratio value in U4.16
		 * sar: aspect ratio of the content rect in U10.11
		 * dar: aspect ratio of the scaler-output rect in U10.11
		 *
		 * Then
		 *        cw
		*  sar = -- * spar >> (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM)
		 *        ch
		 *
		 *        dw
		*  dar = -- * dpar >> (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM)
		 *        dh
		 * or
		 *        dw
		 *  dar = --    if hWindow->hCompositor->stCurInfo.eAspectRatio IS eSquarePxl
		 *        dh
		 *
		 * If AspectRatioMode is Zoom, our goal is to cut cw or ch to make sar to be the same as dar,
		 * When AspectRatioMode is Box, our goal is to cut dw or dh to make dar to be the same as sar.
		 */

		/* calculate the aspect ratio of the scalerOutput, in U10.11 fix pt */
		ulDspAspR = (BVDC_P_DIV_ROUND_NEAR(ulSclOutWidth * ulDspPxlAspRatio, ulSclOutHeight) >>
					 (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM));
		ulDspAspR = BVDC_P_MIN(ulDspAspR, (((uintAR_t)1<<BVDC_P_SUB_ASPR_ALL_BITS_NUM) - 1));

		/* calculate the aspect ratio of the content rect, in U10.11 fix pt */
		ulCntAspR = (BVDC_P_DIV_ROUND_NEAR(ulCntWidth * ulSrcPxlAspRatio, ulCntHeight) >>
					 (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM));
		ulCntAspR = BVDC_P_MIN(ulCntAspR, (((uintAR_t)1<<BVDC_P_SUB_ASPR_ALL_BITS_NUM) - 1));

		if(BVDC_AspectRatioMode_eUseAllDestination == pUserInfo->eAspectRatioMode)
		{
			if( ulCntAspR > ulDspAspR )
			{
				/* needs cut cw: new cw = cw * dar / sar */
				ulTmpWidth = BVDC_P_DIV_ROUND_NEAR(ulCntWidth * ulDspAspR, ulCntAspR);
				ulTmpWidth = BVDC_P_MIN(ulTmpWidth, ulCntWidth);
				/* round cut/orig ratio to i/16 if it make sense and
				 * does not break the scale factor rounding tolerance */
				pAutoAdj->ulCntWidth = BVDC_P_Window_CutRounding_isr(
					hWindow->stCurInfo.ulHrzSclFctRndToler, ulTmpWidth, ulCntWidth);
				pAutoAdj->ulAdjFlags |= (BVDC_P_ADJ_CNT_WIDTH | BVDC_P_ADJ_HRZ_SRC_STEP);
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("AspR correct: cut content W: new width %d", pAutoAdj->ulCntWidth));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
					(pAutoAdj->ulCntWidth * ulCntAspR) / ulCntWidth, ulDspAspR));
			}
			else if( ulCntAspR < ulDspAspR )
			{
				/* needs cut ch: new ch = ch * sar / dar */
				ulTmpHeight = BVDC_P_DIV_ROUND_NEAR(ulCntHeight * ulCntAspR, ulDspAspR);
				/* round cut/orig ratio to i/16 if it make sense and
				 * does not break the scale factor rounding tolerance */
				pAutoAdj->ulCntHeight = BVDC_P_Window_CutRounding_isr(
					hWindow->stCurInfo.ulVrtSclFctRndToler, ulTmpHeight, ulCntHeight);
				pAutoAdj->ulAdjFlags |= (BVDC_P_ADJ_CNT_HEIGHT | BVDC_P_ADJ_VRT_SRC_STEP);
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("AspR correct: cut content H: new height %d", pAutoAdj->ulCntHeight));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
					(ulCntHeight * ulCntAspR) / pAutoAdj->ulCntHeight, ulDspAspR));
			}
			/* else: no cut is needed */
		}

		else if(BVDC_AspectRatioMode_eUseAllSource == pUserInfo->eAspectRatioMode)
		{
			if( ulCntAspR > ulDspAspR )
			{
				/* needs cut dh: new dh = dh * dar / sar */
				ulTmpHeight = BVDC_P_DIV_ROUND_NEAR(ulSclOutHeight * ulDspAspR, ulCntAspR);
				/* round cut/orig ratio to i/16 if it make sense and
				 * does not break the scale factor rounding tolerance */
				ulTmpHeight = BVDC_P_Window_CutRounding_isr(
					hWindow->stCurInfo.ulVrtSclFctRndToler, ulTmpHeight, ulSclOutHeight);
				pAutoAdj->ulOutHeight = ulTmpHeight;
#if BVDC_P_SHOW_ASP_RAT_MSG
				if((!hWindow->bCapture) &&
				   (hWindow->hCompositor->stCurInfo.pFmtInfo->ulDigitalHeight * 3 / 4 > ulTmpHeight))
				{
					BDBG_MODULE_MSG(BVDC_ASP_RAT,("AllSrc asp-ratio correction cuts more than 1/4 SclOut height,"));
					BDBG_MODULE_MSG(BVDC_ASP_RAT,("it might cause bandwidth problem if cap is not used!"));
				}
#endif
				pAutoAdj->ulAdjFlags |= (BVDC_P_ADJ_OUT_HEIGHT | BVDC_P_ADJ_VRT_SRC_STEP);
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("AspR correct: cut out H: new height %d", pAutoAdj->ulOutHeight));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
					ulCntAspR, (ulSclOutHeight * ulDspAspR) / pAutoAdj->ulOutHeight));
			}
			else if( ulCntAspR < ulDspAspR )
			{
				/* needs cut dw: new dw = dw * sar / dar */
				ulTmpWidth = BVDC_P_DIV_ROUND_NEAR(ulSclOutWidth * ulCntAspR, ulDspAspR);
				/* round cut/orig ratio to i/16 if it make sense and
				 * does not break the scale factor rounding tolerance */
				pAutoAdj->ulOutWidth = BVDC_P_Window_CutRounding_isr(
					hWindow->stCurInfo.ulHrzSclFctRndToler, ulTmpWidth, ulSclOutWidth);
				pAutoAdj->ulAdjFlags |= (BVDC_P_ADJ_OUT_WIDTH | BVDC_P_ADJ_HRZ_SRC_STEP);
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("AspR correct: cut out W: new width %d", pAutoAdj->ulOutWidth));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("New equation cnt side 0x%Lx, disp side 0x%Lx",
					ulCntAspR, (pAutoAdj->ulOutWidth * ulDspAspR) / ulSclOutWidth));
			}
			/* else: no cut is needed */
		}
		/* else: should not call this func */
	}
	else
	{
		BDBG_WRN(("Zero Rect Edge, Non-Linear Horizontal Scaling, or AspR unknown."));
	}

	BDBG_LEAVE(BVDC_P_Window_AspectRatioCorrection_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * This function calculate the destination width of the central region of the
 * 9 non-linear horizontal scaling regions to enforce correct aspect ratio.
 */
static void BVDC_P_Window_CentralRegionWidth_isr
	( BVDC_Window_Handle               hWindow,
  uintAR_t                         ulSrcPxlAspRatio,  /* 4.16 fx pt */
  uintAR_t                         ulDspPxlAspRatio ) /* 4.16 fx pt */
{
	const BVDC_P_Window_Info *pUserInfo;
	uint32_t  ulSclOutWidth, ulSclOutHeight;
	uint32_t  ulCntWidth, ulCntHeight;
	uintAR_t  ulCntAspR, ulDspAspR;
	uint32_t  ulNonlinearSrcWidth, ulNonlinearSclOutWidth;
	uint32_t  ulCentralRegionSrcWidth, ulCentralRegionSclOutWidth;

	BDBG_ENTER(BVDC_P_Window_CentralRegionDispWidth_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pUserInfo = &hWindow->stCurInfo;
	ulNonlinearSrcWidth = pUserInfo->ulNonlinearSrcWidth;
	ulNonlinearSclOutWidth = BVDC_P_ALIGN_DN(pUserInfo->ulNonlinearSclOutWidth, 2);
	/* note: src step increases on even pixel only in HW for non-linear scl case */

	BDBG_ASSERT((0 != ulNonlinearSrcWidth) || (0 != ulNonlinearSclOutWidth));
	BDBG_ASSERT((BVDC_AspectRatioMode_eUseAllDestination != pUserInfo->eAspectRatioMode) &&
				(BVDC_AspectRatioMode_eUseAllSource != pUserInfo->eAspectRatioMode));

	ulCntWidth  = hWindow->stSrcCnt.ulWidth;
	ulCntHeight = hWindow->stSrcCnt.ulHeight;
	ulSclOutWidth  = hWindow->stAdjSclOut.ulWidth;
	ulSclOutHeight = hWindow->stAdjSclOut.ulHeight;

	BDBG_ASSERT(ulCntWidth);
	BDBG_ASSERT(ulCntHeight);
	BDBG_ASSERT(ulSclOutWidth);
	BDBG_ASSERT(ulSclOutHeight);

	ulNonlinearSrcWidth = BVDC_P_MIN(ulNonlinearSrcWidth, ulCntWidth >> 1);
	ulNonlinearSclOutWidth = BVDC_P_ALIGN_DN(
		BVDC_P_MIN(ulNonlinearSclOutWidth, ulSclOutWidth >> 1), 2);
	ulCentralRegionSclOutWidth = ulSclOutWidth - 2 * ulNonlinearSclOutWidth;
	ulCentralRegionSrcWidth = ulCntWidth - 2 * ulNonlinearSrcWidth;
	if((2 > ulCentralRegionSclOutWidth) || (0 == ulCentralRegionSrcWidth))
	{
		/* HW increase SrcStep by delta at even pixel, this does not work well
		 * if ulCentralRegionSclOutWidth is 0 */
		ulNonlinearSclOutWidth = BVDC_P_MAX((ulSclOutWidth >> 1), 2) - 2;
		ulCentralRegionSclOutWidth = ulSclOutWidth - 2 * ulNonlinearSclOutWidth;
		ulNonlinearSrcWidth = 0; /* force to re-calculate */
	}

	/* make compiler happy */
	ulCntAspR = 0;
	ulDspAspR = 0;
	if((0 == ulNonlinearSrcWidth) || (0 == ulNonlinearSclOutWidth))
	{
		/* calculate the aspect ratio of the scalerOutput, in U10.11 fix pt
		 */
		ulDspAspR = (BVDC_P_DIV_ROUND_NEAR(ulSclOutWidth * ulDspPxlAspRatio, ulSclOutHeight) >>
					 (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM));
		ulDspAspR = BVDC_P_MIN(ulDspAspR, (((uintAR_t)1<<BVDC_P_SUB_ASPR_ALL_BITS_NUM) - 1));

		/* calculate the aspect ratio of the content rect, in U10.11 fix pt  */
		ulCntAspR = (BVDC_P_DIV_ROUND_NEAR(ulCntWidth * ulSrcPxlAspRatio, ulCntHeight) >>
					 (BVDC_P_ASPR_FRAC_BITS_NUM - BVDC_P_SUB_ASPR_FRAC_BITS_NUM));
		ulCntAspR = BVDC_P_MIN(ulCntAspR, (((uintAR_t)1<<BVDC_P_SUB_ASPR_ALL_BITS_NUM) - 1));
	}

	if(0 == ulNonlinearSclOutWidth)
	{
		/* need to calculate ulNonlinearSclOutWidth based on ulNonlinearSrcWidth
		 * and maintaining aps ratio in central area */
		ulNonlinearSrcWidth = BVDC_P_MIN(ulNonlinearSrcWidth, (ulCntWidth >> 1) - 1);
		ulCentralRegionSrcWidth = ulCntWidth - 2 * ulNonlinearSrcWidth;

		ulCentralRegionSclOutWidth = BVDC_P_DIV_ROUND_NEAR(
			(BVDC_P_DIV_ROUND_NEAR(ulSclOutWidth * ulCntAspR, ulDspAspR) *
			 ulCentralRegionSrcWidth), ulCntWidth);
		ulCentralRegionSclOutWidth =
			BVDC_P_MIN(ulCentralRegionSclOutWidth, ulSclOutWidth);

		/* side rgn must be even pxl unit src step increases on even pixel only */
		ulNonlinearSclOutWidth = BVDC_P_ALIGN_DN(
			(ulSclOutWidth - ulCentralRegionSclOutWidth) / 2, 2);
		ulCentralRegionSclOutWidth = ulSclOutWidth - 2 * ulNonlinearSclOutWidth;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Try SclOut central region width %d <= %d",
					   ulCentralRegionSclOutWidth, ulSclOutWidth));
	}
	else if(0 == ulNonlinearSrcWidth)
	{
		/* need to calculate ulNonlinearSrcWidth based on ulNonlinearSclOutWidth
		 * and maintaining aps ratio in central area */
		ulCentralRegionSrcWidth = BVDC_P_DIV_ROUND_NEAR(
			(BVDC_P_DIV_ROUND_NEAR(ulCntWidth * ulDspAspR, ulCntAspR) *
			 ulCentralRegionSclOutWidth), ulSclOutWidth);
		ulCentralRegionSrcWidth =
			BVDC_P_MIN(ulCentralRegionSrcWidth, ulCntWidth);
		if((ulCntWidth - ulCentralRegionSrcWidth) & 0x1)
			ulCentralRegionSrcWidth += 1; /* side rgn must be pxl unit */
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Try Src central region width %d <= %d",
					   ulCentralRegionSrcWidth, ulCntWidth));
	}

	/* round to floor to avoid overflow */
	hWindow->ulNrmHrzSrcStep = BVDC_P_DIV_ROUND_DN(
		ulCentralRegionSrcWidth << BVDC_P_NRM_SRC_STEP_F_BITS, ulCentralRegionSclOutWidth);
	hWindow->ulNonlinearSrcWidth = (ulCntWidth - ulCentralRegionSrcWidth) / 2;
	hWindow->ulNonlinearSclOutWidth = (ulSclOutWidth - ulCentralRegionSclOutWidth) / 2;
	hWindow->ulCentralRegionSclOutWidth = ulCentralRegionSclOutWidth;

	BDBG_MODULE_MSG(BVDC_ASP_RAT,("NonLi SrcW %d, SclOutW %d, Cntr SrcW %d, SclOutW %d",
				   hWindow->ulNonlinearSrcWidth, hWindow->ulNonlinearSclOutWidth,
				   ulCentralRegionSrcWidth, ulCentralRegionSclOutWidth));
	BDBG_MODULE_MSG(BVDC_ASP_RAT,("Centr ulNrmHrzSrcStep %x", hWindow->ulNrmHrzSrcStep));

	BDBG_LEAVE(BVDC_P_Window_CentralRegionDispWidth_isr);
	return;
}

#define BVDC_P_SCL_FCTR_SHIFT       12
#define BVDC_P_SCL_FCTR_INT1        (1<<BVDC_P_SCL_FCTR_SHIFT)
#define BVDC_P_SCL_FCTR_FRAC_MASK   (BVDC_P_SCL_FCTR_INT1 - 1)
/***************************************************************************
 * {private}
 *
 * This function atomatically adjust the window's content size (width or
 * height) or scl-out-rect size (width or height), to round scale factor
 * to an integers when it is close to enough.
 *
 * It is called for both horizontal and vertical scale factor rounding. It
 * uses *pulAdjFlags, *pulSrcCutLen, *pulOutCutLen, *pulNrmSrcStep as both
 * input and output. It outputs its further adjustment by modifying them.
 *
 * Scale factor rounding involves either "adjust to cut less from src and/or
 * further cut out", or "adjust to cut less from out and/or further cut src".
 * It will affect the aspect ratio accuracy. However, since rounding is done
 * only when an integer approximates close enough to the scale factor,
 * therefore it does not totally break the aspect ratio correctness.
 *
 * When adjusting to cut less from src, VDC ensures that user set source
 * clipping is honered.
 *
 * When this func is called, it is called after box auto cut, pan scan, user
 * set src/dst cut, and aspect ratio correction.
 *
 * Theoretically, if the src is mpeg, hddvi, or if letter box auto back cut
 * is enabled, it should be called at every vsync when RUL is built. Otherwise,
 * it should be called only once at the first vsync after ApplyChanges.

 * Optimize: aspect ratio correctio and scale factor rounding needs re-do
 * only if values of SrcCut changed after box cut, pan scan, and user clip,
 * or right after ApplyChanges has been called.
 */
static void BVDC_P_Window_ScaleFactorRounding_isr
	( uint32_t                         ulSclFctRndToler,
	  uint32_t                         ulSrcFullLen,
	  uint32_t                         ulAdjFlagShift,
	  uint32_t                        *pulAdjFlags,    /* in and out */
	  uint32_t                        *pulSrcCutLen,   /* in and out */
	  uint32_t                        *pulOutCutLen,   /* in and out */
	  uint32_t                        *pulNrmSrcStep ) /* in and out */
{
	uint32_t  ulSrcCutLen, ulOutCutLen, ulAdjFlags;
	uint32_t  ulSclFactor, ulRoundFactor, ulFraction;
	uint32_t  ulTolerance;
	bool  bAdjSrcCut, bAdjOutCut, bAdjSrcStep;

	bAdjSrcCut = false;
	bAdjOutCut = false;
	bAdjSrcStep = false;
	ulAdjFlags = *pulAdjFlags;
	ulSrcCutLen = *pulSrcCutLen;
	ulOutCutLen = *pulOutCutLen;

	if((0 != ulSclFctRndToler) && (0!= ulSrcCutLen) && (0 != (*pulNrmSrcStep >> 3)))
	{
		ulTolerance = ulSclFctRndToler << BVDC_P_SCL_FCTR_SHIFT;
		ulSclFactor = (ulAdjFlags & (BVDC_P_ADJ_SRC_STEP << ulAdjFlagShift))?
			(ulOutCutLen << BVDC_P_SCL_FCTR_SHIFT) / ulSrcCutLen :
			(1 << (BVDC_P_NRM_SRC_STEP_F_BITS - 3 + BVDC_P_SCL_FCTR_SHIFT)) / (*pulNrmSrcStep >> 3);
		ulFraction = ulSclFactor & BVDC_P_SCL_FCTR_FRAC_MASK;
		ulRoundFactor = ulSclFactor >> BVDC_P_SCL_FCTR_SHIFT;
		if((ulFraction <= (BVDC_P_SCL_FCTR_INT1 / 2)) && (0 < ulRoundFactor))
		{
			/* try rounding scale factor down, ==> round srcStep up */
			if((0 != ulFraction) &&
			   ((100 * ulFraction) < (ulTolerance * ulRoundFactor)))
			{
				/* rounding down, need to adjust to cut less cnt length, and/or cut more out
				 * note:  if we don't adjust srcCut, we might see a 704 dst window not display
				 * the middle part of 720 src, but the left part */
				if(ulSrcCutLen < ulSrcFullLen)
				{
					*pulSrcCutLen = BVDC_P_MIN(ulSrcFullLen, ulOutCutLen / ulRoundFactor);
				}
				*pulOutCutLen = ulRoundFactor * (*pulSrcCutLen);

				bAdjSrcCut = (*pulSrcCutLen != ulSrcCutLen);
				bAdjOutCut = (*pulOutCutLen != ulOutCutLen);
				bAdjSrcStep = true;
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Scale up and SclFctr rouding down, new srcCut %d, outCut %d",
							   *pulSrcCutLen, *pulOutCutLen));
			}
		}
		else
		{
			/* try rounding sclae factor up  ==> round srcStep down */
			ulRoundFactor ++;
			ulFraction = BVDC_P_SCL_FCTR_INT1 - ulFraction;
			if((100 * ulFraction) < (ulTolerance * ulRoundFactor))
			{
				/* rounding up, need to adjust to cut less out length, or expand outside.
				 * note 1: ok to make sclOut size big, it will not cause garbage into video
				 * like the srcCnt case, and extra output will be clipped by CMP (dstRect
				 * is not expanded), note 2:  if we don't adjust outCut, we might see a 704
				 * src not displayed in the middile of 720 screen, but to the left. */
				*pulOutCutLen = ulSrcCutLen * ulRoundFactor;
				bAdjOutCut = (*pulOutCutLen != ulOutCutLen);
				bAdjSrcStep = true;
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Scale up and SclFctr rouding up, new OutCut %d", *pulOutCutLen));
			}
		}

		/* more apply of our adj decision */
		if (bAdjSrcStep)
		{
			*pulNrmSrcStep = BVDC_P_DIV_ROUND_NEAR(
				1 << BVDC_P_NRM_SRC_STEP_F_BITS, ulRoundFactor);
			ulAdjFlags &= ~(BVDC_P_ADJ_SRC_STEP << ulAdjFlagShift); /* just did above */
			ulAdjFlags |= (
				((bAdjSrcCut)? (BVDC_P_ADJ_CNT_CUT << ulAdjFlagShift) : 0) |
				((bAdjOutCut)? (BVDC_P_ADJ_OUT_CUT << ulAdjFlagShift) : 0));
			*pulAdjFlags =  ulAdjFlags;
		}
	}
}

/***************************************************************************
 * {private}
 *
 * Utility func called by BVDC_P_Window_AdjustRectangles_isr to perform
 * automatic rectangle cut, decided by BVDC_P_Window_AspectRatioCorrection_isr
 * and/or BVDC_P_Window_ScaleFactorRounding_isr, and stored the result into
 * struct hWindow->stSrcCnt or hWindow->stAdjSclOut / stAdjDstRect.
 */
static void BVDC_P_Window_ApplyAutoAdj_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_AutoAdj                  *pAutoAdj,
	  uint32_t                         ulFullSrcWidth,
	  uint32_t                         ulFullSrcHeight )
{
	uint32_t  ulNewSclOutWidth, ulOldSclOutWidth, ulNewDstWidth;
	uint32_t  ulNewSclOutHeight, ulOldSclOutHeight, ulNewDstHeight;
	uint32_t  ulTmpOff;
	uint32_t  ulUsrClipLeft, ulUsrClipRight, ulUsrClipTop, ulUsrClipBottom;
	BVDC_P_ClipRect  *pUsrClip = &hWindow->stCurInfo.stSrcClip;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* cnt horiz cut */
	if((pAutoAdj->ulCntWidth < hWindow->stSrcCnt.ulWidth) &&
	   (pAutoAdj->ulAdjFlags & BVDC_P_ADJ_CNT_WIDTH))
	{
		/* more cut to src cnt width */
		ulTmpOff = ((hWindow->stSrcCnt.ulWidth - pAutoAdj->ulCntWidth) <<
					(BVDC_P_16TH_PIXEL_SHIFT - 1));
		hWindow->stSrcCnt.lLeft += ulTmpOff;
		hWindow->stSrcCnt.lLeft_R += ulTmpOff;
		hWindow->stSrcCnt.ulWidth = pAutoAdj->ulCntWidth;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut cnt width, new SrcWidth %d", pAutoAdj->ulCntWidth));
	}
	else if((pAutoAdj->ulCntWidth > hWindow->stSrcCnt.ulWidth) &&
			(pAutoAdj->ulAdjFlags & BVDC_P_ADJ_CNT_WIDTH))
	{
		/* less cut to src cnt width */
		ulTmpOff = ((pAutoAdj->ulCntWidth - hWindow->stSrcCnt.ulWidth) <<
					(BVDC_P_16TH_PIXEL_SHIFT - 1));

		/* BVDC_P_Window_ScaleFactorRounding_isr make sure that
		 * pAutoAdj->ulCntWidth < ulFullSrcWidth - pUsrClip->ulLeft - pUsrClip->ulRight */
		ulUsrClipLeft  = pUsrClip->ulLeft  << BVDC_P_16TH_PIXEL_SHIFT;
		ulUsrClipRight = pUsrClip->ulRight << BVDC_P_16TH_PIXEL_SHIFT;
		if (hWindow->stSrcCnt.lLeft <= (int32_t)(ulTmpOff + ulUsrClipLeft))
		{
			/* left edge over-shoot handle */
			hWindow->stSrcCnt.lLeft = ulUsrClipLeft;
		}
		else if ((hWindow->stSrcCnt.lLeft - ulTmpOff +
				  (pAutoAdj->ulCntWidth << BVDC_P_16TH_PIXEL_SHIFT)) >
				 ((ulFullSrcWidth << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipRight))
		{
			/* right edge over-shoot handle */
			hWindow->stSrcCnt.lLeft =
				((ulFullSrcWidth - pAutoAdj->ulCntWidth) << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipRight;
		}
		else
		{
			/* expand both sides */
			hWindow->stSrcCnt.lLeft -= ulTmpOff;
		}

		ulUsrClipLeft  = (pUsrClip->ulLeft  + pUsrClip->lLeftDelta_R) << BVDC_P_16TH_PIXEL_SHIFT;
		ulUsrClipRight = (pUsrClip->ulRight - pUsrClip->lLeftDelta_R) << BVDC_P_16TH_PIXEL_SHIFT;
		if (hWindow->stSrcCnt.lLeft_R <= (int32_t)(ulTmpOff + ulUsrClipLeft))
		{
			/* left edge over-shoot handle */
			hWindow->stSrcCnt.lLeft_R = ulUsrClipLeft;
		}
		else if ((hWindow->stSrcCnt.lLeft_R - ulTmpOff +
				  (pAutoAdj->ulCntWidth << BVDC_P_16TH_PIXEL_SHIFT)) >
				 ((ulFullSrcWidth << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipRight))
		{
			/* right edge over-shoot handle */
			hWindow->stSrcCnt.lLeft_R =
				((ulFullSrcWidth - pAutoAdj->ulCntWidth) << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipRight;
		}
		else
		{
			/* expand both sides */
			hWindow->stSrcCnt.lLeft_R -= ulTmpOff;
		}
		hWindow->stSrcCnt.ulWidth = pAutoAdj->ulCntWidth;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto expand cnt width, new SrcWidth %d", pAutoAdj->ulCntWidth));
	}

	/* cnt vertical cut */
	if((pAutoAdj->ulCntHeight < hWindow->stSrcCnt.ulHeight) &&
	   (pAutoAdj->ulAdjFlags & BVDC_P_ADJ_CNT_HEIGHT))
	{
		/* more cut to src height */
		ulTmpOff = ((hWindow->stSrcCnt.ulHeight - pAutoAdj->ulCntHeight) <<
					(BVDC_P_16TH_PIXEL_SHIFT - 1));
		hWindow->stSrcCnt.lTop += ulTmpOff;
		hWindow->stSrcCnt.ulHeight = pAutoAdj->ulCntHeight;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut cnt height, new SrcHeight %d", pAutoAdj->ulCntHeight));
	}
	else if ((pAutoAdj->ulCntHeight > hWindow->stSrcCnt.ulHeight) &&
			 (pAutoAdj->ulAdjFlags & BVDC_P_ADJ_CNT_HEIGHT))
	{
		/* less cut to src cnt height */
		ulTmpOff = ((pAutoAdj->ulCntHeight - hWindow->stSrcCnt.ulHeight) <<
					(BVDC_P_16TH_PIXEL_SHIFT - 1));

		/* BVDC_P_Window_ScaleFactorRounding_isr make sure that
		 * pAutoAdj->ulCntHeight < ulFullSrcHeight - pUsrClip->ulTop - pUsrClip->ulBottom */
		ulUsrClipTop    = pUsrClip->ulTop    << BVDC_P_16TH_PIXEL_SHIFT;
		ulUsrClipBottom = pUsrClip->ulBottom << BVDC_P_16TH_PIXEL_SHIFT;
		if (hWindow->stSrcCnt.lTop <= (int32_t)(ulTmpOff + ulUsrClipTop))
			/* top edge over-shoot handle */
			hWindow->stSrcCnt.lTop = ulUsrClipTop;
		else if ((hWindow->stSrcCnt.lTop - ulTmpOff +
				  (pAutoAdj->ulCntHeight << BVDC_P_16TH_PIXEL_SHIFT)) >
				 ((ulFullSrcHeight << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipBottom))
			/* bottom edge over-shoot handle */
			hWindow->stSrcCnt.lTop =
				((ulFullSrcHeight - pAutoAdj->ulCntHeight) << BVDC_P_16TH_PIXEL_SHIFT) - ulUsrClipBottom;
		else
			/* expand both sides */
			hWindow->stSrcCnt.lTop -= ulTmpOff;
		hWindow->stSrcCnt.ulHeight = pAutoAdj->ulCntHeight;
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto expand cnt height, new SrcHeight %d", pAutoAdj->ulCntHeight));
	}

	/* scl/dst cut */
	if(!hWindow->stCurInfo.bMosaicMode)
	{
		/* cut scl-out/dst width */
		if((pAutoAdj->ulOutWidth < hWindow->stAdjSclOut.ulWidth) &&
		   (pAutoAdj->ulAdjFlags & BVDC_P_ADJ_OUT_WIDTH))
		{
			/* Due to HW limit, scale-out width should be >= 16.
			 * we should not introduce the violation */
			ulNewSclOutWidth = BVDC_P_MAX(pAutoAdj->ulOutWidth, BVDC_P_WIN_SCL_OUTPUT_H_MIN);

			/* adjust scaler-out width */
			ulOldSclOutWidth = hWindow->stAdjSclOut.ulWidth;
			hWindow->stAdjSclOut.ulWidth = ulNewSclOutWidth;
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut scl-out width, new SclOutWidth %d", ulNewSclOutWidth));

			/* adjust dest rect and its offset inside scaler-output rect */
			if(0!=ulOldSclOutWidth)
			{
				hWindow->stAdjSclOut.lLeft = BVDC_P_DIV_ROUND_NEAR(
					ulNewSclOutWidth * hWindow->stAdjSclOut.lLeft, ulOldSclOutWidth);
				hWindow->stAdjSclOut.lLeft_R = BVDC_P_DIV_ROUND_NEAR(
					ulNewSclOutWidth * hWindow->stAdjSclOut.lLeft_R, ulOldSclOutWidth);
				ulNewDstWidth = BVDC_P_DIV_ROUND_NEAR(
					ulNewSclOutWidth * hWindow->stAdjDstRect.ulWidth, ulOldSclOutWidth);
				hWindow->stAdjDstRect.lLeft += (hWindow->stAdjDstRect.ulWidth - ulNewDstWidth) / 2;
				hWindow->stAdjDstRect.lLeft_R += (hWindow->stAdjDstRect.ulWidth - ulNewDstWidth) / 2;
				hWindow->stAdjDstRect.ulWidth = ulNewDstWidth;
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut dst w: new left %d, width %d",
							   hWindow->stAdjDstRect.lLeft, ulNewDstWidth));
			}
		}
		else if((pAutoAdj->ulOutWidth > hWindow->stAdjSclOut.ulWidth) &&
				(pAutoAdj->ulAdjFlags & BVDC_P_ADJ_OUT_WIDTH))
		{
			/* sclOut width expanded a little due to sclFactorRounding,
			 * but we can not expand dstRect */
			ulNewSclOutWidth = pAutoAdj->ulOutWidth;
			ulOldSclOutWidth = hWindow->stAdjSclOut.ulWidth;
			hWindow->stAdjSclOut.ulWidth = ulNewSclOutWidth;
			hWindow->stAdjSclOut.lLeft += (ulNewSclOutWidth - ulOldSclOutWidth) / 2;
			hWindow->stAdjSclOut.lLeft_R += (ulNewSclOutWidth - ulOldSclOutWidth) / 2;
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto expand scl-out width, new SclOutWidth %d", ulNewSclOutWidth));
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("dstRect no change, new sclOut.left %d", hWindow->stAdjSclOut.lLeft));
		}

		/* cut scl-out/dst height */
		if((pAutoAdj->ulOutHeight < hWindow->stAdjSclOut.ulHeight) &&
		   (pAutoAdj->ulAdjFlags & BVDC_P_ADJ_OUT_HEIGHT))
		{
			ulNewSclOutHeight = pAutoAdj->ulOutHeight;

			/* adjust scaler-out height */
			ulOldSclOutHeight = hWindow->stAdjSclOut.ulHeight;
			hWindow->stAdjSclOut.ulHeight = ulNewSclOutHeight;
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut scl-out height: new SclOutHeight %d", ulNewSclOutHeight));

			/* adjust dest rect and its offset inside scaler-output rect */
			if(0!=ulOldSclOutHeight)
			{
				hWindow->stAdjSclOut.lTop = BVDC_P_DIV_ROUND_NEAR(
					ulNewSclOutHeight * hWindow->stAdjSclOut.lTop, ulOldSclOutHeight);
				ulNewDstHeight = BVDC_P_DIV_ROUND_NEAR(
					ulNewSclOutHeight * hWindow->stAdjDstRect.ulHeight, ulOldSclOutHeight);
				hWindow->stAdjDstRect.lTop += ((hWindow->stAdjDstRect.ulHeight - ulNewDstHeight) / 2);
				hWindow->stAdjDstRect.ulHeight = ulNewDstHeight;
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto cut dst H: new top %d, height %d",
							   hWindow->stAdjDstRect.lTop, ulNewDstHeight));
			}
		}
		else if((pAutoAdj->ulOutHeight > hWindow->stAdjSclOut.ulHeight) &&
				(pAutoAdj->ulAdjFlags & BVDC_P_ADJ_OUT_HEIGHT))
		{
			/* sclOut height expanded a little due to sclFactorRounding,
			 * but we can not expand dstRect */
			ulNewSclOutHeight = pAutoAdj->ulOutHeight;
			ulOldSclOutHeight = hWindow->stAdjSclOut.ulHeight;
			hWindow->stAdjSclOut.ulHeight = ulNewSclOutHeight;
			hWindow->stAdjSclOut.lTop += (ulNewSclOutHeight - ulOldSclOutHeight) / 2;
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("Auto expand scl-out height, new SclOutHeight %d", ulNewSclOutHeight));
			BDBG_MODULE_MSG(BVDC_ASP_RAT,("dstRect no change, new sclOut.top %d", hWindow->stAdjSclOut.lTop));
		}
	}
}


/***************************************************************************
 * {private}
 *
 * Utility func called by BVDC_P_Window_AdjustRectangles_isr to perform
 * automatic rectangle adjustment, decided by BVDC_P_Window_AspectRatio-
 * Correction_isr and/or BVDC_P_Window_ScaleFactorRounding_isr,  and stored
 * the result into struct hWindow->stSrcCnt or hWindow->stAdjSclOut /
 * stAdjDstRect.
 */
static void BVDC_P_Window_AutoAdjRelatedToSclRatio_isr
	( BVDC_Window_Handle               hWindow,
	  bool                             bMosaicMode,
	  uintAR_t                         ulSrcPxlAspRatio, /* 4.16 fx pt */
	  uintAR_t                         ulDspPxlAspRatio, /* 4.16 fx pt */
	  uint32_t                         ulSrcWidth,
	  uint32_t                         ulSrcHeight )
{
	BVDC_P_Window_Info *pUserInfo;
	BVDC_P_AutoAdj  stAutoAdj;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	stAutoAdj.ulAdjFlags = 0;
	stAutoAdj.ulCntWidth = hWindow->stSrcCnt.ulWidth;
	stAutoAdj.ulCntHeight = hWindow->stSrcCnt.ulHeight;
	stAutoAdj.ulOutWidth = hWindow->stAdjSclOut.ulWidth;
	stAutoAdj.ulOutHeight = hWindow->stAdjSclOut.ulHeight;

	/* --- (6.1) update stAutoAdj for aspect ratio correction --- */
	pUserInfo = &hWindow->stCurInfo;
	if((BVDC_AspectRatioMode_eUseAllDestination == pUserInfo->eAspectRatioMode) ||
	   (BVDC_AspectRatioMode_eUseAllSource      == pUserInfo->eAspectRatioMode))
	{
#if BVDC_P_SHOW_ASP_RAT_MSG
		BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
#endif

		BDBG_MODULE_MSG(BVDC_ASP_RAT,("UserSrcClip: (L %d, R %d, T %d, B %d)",
			pUserInfo->stSrcClip.ulLeft, pUserInfo->stSrcClip.ulRight,
			pUserInfo->stSrcClip.ulTop, pUserInfo->stSrcClip.ulBottom));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("UserSclOut: (L %d, T %d, W %d, H %d); UserDstRect: (L %d, T %d, W %d, H %d)",
			pUserInfo->stScalerOutput.lLeft, pUserInfo->stScalerOutput.lTop,
			pUserInfo->stScalerOutput.ulWidth, pUserInfo->stScalerOutput.ulHeight,
			pUserInfo->stDstRect.lLeft, pUserInfo->stDstRect.lTop,
			pUserInfo->stDstRect.ulWidth, pUserInfo->stDstRect.ulHeight));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("CntRect before AspR correct: (L %d.%d/16, T %d.%d/16, W %d, H %d)",
			hWindow->stSrcCnt.lLeft>>4, hWindow->stSrcCnt.lLeft & 0xf,
			hWindow->stSrcCnt.lTop>>4,  hWindow->stSrcCnt.lTop  & 0xf,
			hWindow->stSrcCnt.ulWidth, hWindow->stSrcCnt.ulHeight));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("SclOut before AspR correct: (L %d, T %d, W %d, H %d)",
			hWindow->stAdjSclOut.lLeft, hWindow->stAdjSclOut.lTop,
			hWindow->stAdjSclOut.ulWidth, hWindow->stAdjSclOut.ulHeight,
			hWindow->hCompositor->hDisplay->stCurInfo.eAspectRatio));
		BDBG_MODULE_MSG(BVDC_ASP_RAT,("DstRect before AspR correct: (L %d, T %d, W %d, H %d)",
			hWindow->stAdjDstRect.lLeft, hWindow->stAdjDstRect.lTop,
			hWindow->stAdjDstRect.ulWidth, hWindow->stAdjDstRect.ulHeight));

		BVDC_P_Window_AspectRatioCorrection_isr(
			hWindow, ulSrcPxlAspRatio, ulDspPxlAspRatio, &stAutoAdj);
	}

	/* --- (6.2) adjust stAutoAdj for scale factor rounding --- */
	if((0 != pUserInfo->ulHrzSclFctRndToler) && !bMosaicMode)
	{
		BVDC_P_Window_ScaleFactorRounding_isr(pUserInfo->ulHrzSclFctRndToler,
			ulSrcWidth - pUserInfo->stSrcClip.ulLeft - pUserInfo->stSrcClip.ulRight,
			BVDC_P_ADJ_FLAG_HRZ_SHIFT, &(stAutoAdj.ulAdjFlags),
			&(stAutoAdj.ulCntWidth), &(stAutoAdj.ulOutWidth), &(hWindow->ulNrmHrzSrcStep));
	}
	if((0 != pUserInfo->ulVrtSclFctRndToler) && !bMosaicMode)
	{
		BVDC_P_Window_ScaleFactorRounding_isr(pUserInfo->ulVrtSclFctRndToler,
			ulSrcHeight - pUserInfo->stSrcClip.ulTop - pUserInfo->stSrcClip.ulBottom,
			BVDC_P_ADJ_FLAG_VRT_SHIFT, &(stAutoAdj.ulAdjFlags),
		    &(stAutoAdj.ulCntHeight), &(stAutoAdj.ulOutHeight), &(hWindow->ulNrmVrtSrcStep));
	}

	/* --- (6.3) apply stAutoAdj to hWindow->stSrcCnt/stAdjSclOut/stAdjDstRect --- */
	BVDC_P_Window_ApplyAutoAdj_isr(hWindow, &stAutoAdj, ulSrcWidth, ulSrcHeight);

	/* --- (6.4) re-calculate nrm srcStep */
	if (stAutoAdj.ulAdjFlags & BVDC_P_ADJ_HRZ_SRC_STEP)
	{
		hWindow->ulNrmHrzSrcStep = BVDC_P_CAL_HRZ_SRC_STEP(
			hWindow->stSrcCnt.ulWidth, hWindow->stAdjSclOut.ulWidth);
	}
	if (stAutoAdj.ulAdjFlags & BVDC_P_ADJ_VRT_SRC_STEP)
	{
		hWindow->ulNrmVrtSrcStep = BVDC_P_CAL_VRT_SRC_STEP(
			hWindow->stSrcCnt.ulHeight, hWindow->stAdjSclOut.ulHeight);
	}
}

#if (BVDC_P_SUPPORT_STG)
/***************************************************************************
 * {private}
 *
 * Utility func called by BVDC_P_ScaleSrcPxlAspRatio_isr to shift off the
 * lower bits if the pixel aspect ratio x and y have more than 16 bits
 */
static uint32_t BVDC_P_PxlAspRatio32BitsTo16Bits_isr
	( uint32_t                         ulPxlAspRatio_x,
	  uint32_t                         ulPxlAspRatio_y,
	  uint32_t                         ulBitsToCheck )
{
	uint32_t  ulNewCheckBits;
	uint32_t b=0, a=0, m=0, i=0;

	if(ulPxlAspRatio_x == ulPxlAspRatio_y)
		ulPxlAspRatio_x = ulPxlAspRatio_y = 1;
	else
	{

		a = ulPxlAspRatio_y > ulPxlAspRatio_x ? ulPxlAspRatio_y:ulPxlAspRatio_x;
		b = ulPxlAspRatio_y > ulPxlAspRatio_x ? ulPxlAspRatio_x:ulPxlAspRatio_y;

		while (b  && (i<10)) { m = a % b; a = b; b = m; i++;}

		if (i<10) {
			ulPxlAspRatio_y/=a;
			ulPxlAspRatio_x/=a;
		}
		if(((ulPxlAspRatio_y & 0xffff) == ulPxlAspRatio_y) &&
			((ulPxlAspRatio_x& 0xffff) == ulPxlAspRatio_x))
			ulBitsToCheck = 1;

	}

	if (ulBitsToCheck == 1)
	{
		if ((ulPxlAspRatio_x >> 16) || (ulPxlAspRatio_y >> 16))
			return ((ulPxlAspRatio_x >> 1) << 16) | (ulPxlAspRatio_y >> 1);
		else
			return ((ulPxlAspRatio_x) << 16) | (ulPxlAspRatio_y);
	}
	else
	{
		ulNewCheckBits = ulBitsToCheck / 2;
		if ((ulPxlAspRatio_x >> (ulNewCheckBits + 16)) ||
			(ulPxlAspRatio_y >> (ulNewCheckBits + 16)))
		{
			return BVDC_P_PxlAspRatio32BitsTo16Bits_isr(
				ulPxlAspRatio_x >> ulNewCheckBits,
				ulPxlAspRatio_y >> ulNewCheckBits,
				ulNewCheckBits);
		}
		else
		{
			return BVDC_P_PxlAspRatio32BitsTo16Bits_isr(
				ulPxlAspRatio_x,
				ulPxlAspRatio_y,
				ulNewCheckBits);
		}
	}
}

/***************************************************************************
 * {private}
 *
 * Utility func called by BVDC_P_Window_AdjustRectangles_isr to calculate
 * the STG pixel aspect ratio when aspect ratio correction is not performed.
 * It is the scaled source pixel aspect ratio
 */
static void BVDC_P_ScaleSrcPxlAspRatio_isr
	( uint32_t                         ulSrcPxlAspRatio_x_y,
	  uint32_t                         ulSrcWidth,
	  uint32_t                         ulSrcHeight,
	  uint32_t                         ulDstWidth,
	  uint32_t                         ulDstHeight,
	  BFMT_Orientation                 eDspOrientation,
	  uint32_t                        *pulOutPxlAspRatio_x_y )
{
	uint32_t ulPxlAspR_x, ulPxlAspR_y;
	uint32_t ulPxlAspR_x_hi, ulPxlAspR_x_low, ulPxlAspR_y_hi, ulPxlAspR_y_low;

	ulPxlAspR_x = ulSrcPxlAspRatio_x_y >> 16;
	ulPxlAspR_y = ulSrcPxlAspRatio_x_y & 0xffff;

#if (BVDC_P_SUPPORT_3D_VIDEO)
	if(eDspOrientation == BFMT_Orientation_e3D_OverUnder)
	{
		ulPxlAspR_y = ulPxlAspR_y * 2;
	}
	else if(eDspOrientation == BFMT_Orientation_e3D_LeftRight)
	{
		ulPxlAspR_x = ulPxlAspR_x * 2;
	}
#endif

	ulPxlAspR_x *= ulSrcWidth;
	BMTH_HILO_32TO64_Mul(ulPxlAspR_x, ulDstHeight,
						 &ulPxlAspR_x_hi, &ulPxlAspR_x_low);
	ulPxlAspR_y *= ulSrcHeight;
	BMTH_HILO_32TO64_Mul(ulPxlAspR_y, ulDstWidth,
						 &ulPxlAspR_y_hi, &ulPxlAspR_y_low);

	/* we only need 16 bits for output PxlAspR_x and PxlAspR_y */
	if (ulPxlAspR_x_hi || ulPxlAspR_y_hi)
	{
		/* shift off the low 16 bits. */
		ulPxlAspR_x_low >>= 16;
		ulPxlAspR_y_low >>= 16;
		ulPxlAspR_x_low |= (ulPxlAspR_x_hi << 16);
		ulPxlAspR_y_low |= (ulPxlAspR_y_hi << 16);
	}

	/* the original numbers are less than 11 + 11 + 16 = 38 bits,
	 * so we now have less than 32 bits */
	*pulOutPxlAspRatio_x_y = BVDC_P_PxlAspRatio32BitsTo16Bits_isr(
		ulPxlAspR_x_low, ulPxlAspR_y_low, 16);
}
#endif

/***************************************************************************
 * {private}
 *
 * This function will be called when source is muted instead of
 * AdjustRectangles_isr to initialize all variables needed by
 * UpdateSrcAndUserInfo_isr.
 */
void BVDC_P_Window_InitMuteRec_isr
	( BVDC_Window_Handle               hWindow,
	  const BFMT_VideoInfo            *pDstFmtInfo,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  uint32_t                         ulRectIdx )
{
	bool bNonlinearScl;
	bool bDoAspRatCorrect;

	uintAR_t  ulSrcPxlAspRatio = 0, ulDspPxlAspRatio = 0;  /* pixel asp ratio in U4.16 fix pt */
	uint32_t  ulSrcPxlAspRatio_x_y=0;
	bool  bMosaicMode;

	BDBG_ENTER(BVDC_P_Window_InitMuteRec_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	bMosaicMode = hWindow->stCurInfo.bMosaicMode;

#if BVDC_P_SUPPORT_MOSAIC_MODE
	/* MosaicMode: reset SclOut rect as the mosaic sub-window size */
	if(bMosaicMode)
	{
		hWindow->stAdjSclOut.lLeft    = 0;
		hWindow->stAdjSclOut.lLeft_R  = 0;
		hWindow->stAdjSclOut.lTop     = 0;
		hWindow->stAdjSclOut.ulWidth  = hWindow->stCurInfo.astMosaicRect[
			BVDC_P_MIN(ulRectIdx, hWindow->stCurInfo.ulMosaicCount - 1)].ulWidth;
		hWindow->stAdjSclOut.ulHeight = hWindow->stCurInfo.astMosaicRect[
			BVDC_P_MIN(ulRectIdx, hWindow->stCurInfo.ulMosaicCount - 1)].ulHeight;
	}
	else
#else
	BSTD_UNUSED(ulRectIdx);
#endif
	{
		if(hWindow->stCurInfo.stScalerOutput.ulWidth  > pDstFmtInfo->ulWidth ||
		   hWindow->stCurInfo.stScalerOutput.ulHeight > pDstFmtInfo->ulHeight)
		{
			hWindow->stAdjSclOut = hWindow->stCurInfo.stDstRect;
		}
		else
		{
			hWindow->stAdjSclOut = hWindow->stCurInfo.stScalerOutput;
		}
	}
	/* Round up to even number (PR48513) */
	hWindow->stAdjSclOut.ulWidth = BVDC_P_ALIGN_UP(hWindow->stAdjSclOut.ulWidth, 2);

	hWindow->stAdjDstRect = hWindow->stCurInfo.stDstRect;
	if(pMvdFieldData->ulSourceHorizontalSize >(hWindow->stCurInfo.stSrcClip.ulLeft + hWindow->stCurInfo.stSrcClip.ulRight + BVDC_P_SRC_INPUT_H_MIN))
	{
		hWindow->stSrcCnt.lLeft = hWindow->stCurInfo.stSrcClip.ulLeft;
		hWindow->stSrcCnt.lLeft_R = hWindow->stCurInfo.stSrcClip.ulLeft + hWindow->stCurInfo.stSrcClip.lLeftDelta_R;
		hWindow->stSrcCnt.ulWidth = pMvdFieldData->ulSourceHorizontalSize - (hWindow->stCurInfo.stSrcClip.ulLeft + hWindow->stCurInfo.stSrcClip.ulRight);
	}
	else
	{
		hWindow->stSrcCnt.lLeft = 0;
		hWindow->stSrcCnt.lLeft_R = 0;
		hWindow->stSrcCnt.ulWidth = pMvdFieldData->ulSourceHorizontalSize;
	}
	if(pMvdFieldData->ulSourceVerticalSize >(hWindow->stCurInfo.stSrcClip.ulTop + hWindow->stCurInfo.stSrcClip.ulBottom + BVDC_P_SRC_INPUT_V_MIN))
	{
		hWindow->stSrcCnt.lTop = hWindow->stCurInfo.stSrcClip.ulTop;
		hWindow->stSrcCnt.ulHeight = pMvdFieldData->ulSourceVerticalSize - (hWindow->stCurInfo.stSrcClip.ulTop + hWindow->stCurInfo.stSrcClip.ulBottom);
	}
	else
	{
		hWindow->stSrcCnt.lTop = 0;
		hWindow->stSrcCnt.ulHeight = pMvdFieldData->ulSourceVerticalSize;
	}

	/* when a mpeg src is connected to more than one window, eUseAllDestination with
	 * mosaic mode could cause diff vertical content clipping --> bandwidth failure;
	 * if only one window is using the src, it is fine to use eUseAllDestination with
	 * mosaic mode, but we want to have the same effect no matter how many window
	 * are connected to the src */
	bDoAspRatCorrect = (
		(!bMosaicMode) &&
		((BVDC_AspectRatioMode_eUseAllDestination == hWindow->stCurInfo.eAspectRatioMode) ||
		 (BVDC_AspectRatioMode_eUseAllSource == hWindow->stCurInfo.eAspectRatioMode)));

	bNonlinearScl = (
		!bDoAspRatCorrect &&
		((0 != hWindow->stCurInfo.ulNonlinearSrcWidth) ||
		 (0 != hWindow->stCurInfo.ulNonlinearSclOutWidth)));

	if(!bMosaicMode)
	{
		BVDC_P_Window_AdjDstCut_isr(pDstFmtInfo,
			&hWindow->stSrcCnt, &hWindow->stAdjSclOut, &hWindow->stAdjDstRect);
	}

	if(bDoAspRatCorrect)
	{
		BVDC_P_CalcuPixelAspectRatio_isr(
			pMvdFieldData->eAspectRatio,
			pMvdFieldData->uiSampleAspectRatioX,
			pMvdFieldData->uiSampleAspectRatioY,
			pMvdFieldData->ulDisplayHorizontalSize,
			pMvdFieldData->ulDisplayVerticalSize,
			&hWindow->stCurInfo.hSource->stCurInfo.stAspRatRectClip,
			&ulSrcPxlAspRatio, &ulSrcPxlAspRatio_x_y);
		ulDspPxlAspRatio = hWindow->hCompositor->hDisplay->ulPxlAspRatio;
	}

#if (BVDC_P_SUPPORT_STG)
	if(hWindow->hCompositor->hDisplay->stStgChan.bEnable)
	{
		if (bMosaicMode || bDoAspRatCorrect || bNonlinearScl ||
			BVDC_P_EQ_DELTA(hWindow->stAdjSclOut.ulWidth,  pDstFmtInfo->ulWidth,  8) ||
			BVDC_P_EQ_DELTA(hWindow->stAdjSclOut.ulHeight, pDstFmtInfo->ulHeight, 8))
		{
			hWindow->ulStgPxlAspRatio_x_y = hWindow->hCompositor->hDisplay->ulPxlAspRatio_x_y;
		}
		else
		{
			if(0 == ulSrcPxlAspRatio)    /* non-0 iff already calculated */
			{
				BVDC_P_CalcuPixelAspectRatio_isr(
					pMvdFieldData->eAspectRatio,
					pMvdFieldData->uiSampleAspectRatioX,
					pMvdFieldData->uiSampleAspectRatioY,
					pMvdFieldData->ulDisplayHorizontalSize,
					pMvdFieldData->ulDisplayVerticalSize,
					&hWindow->stCurInfo.hSource->stCurInfo.stAspRatRectClip,
					&ulSrcPxlAspRatio, &ulSrcPxlAspRatio_x_y);
			}
			BVDC_P_ScaleSrcPxlAspRatio_isr(
					ulSrcPxlAspRatio_x_y,
					hWindow->stSrcCnt.ulWidth, hWindow->stSrcCnt.ulHeight,
					hWindow->stAdjSclOut.ulWidth, hWindow->stAdjSclOut.ulHeight,
					hWindow->hCompositor->hDisplay->stCurInfo.eOrientation,
					&hWindow->ulStgPxlAspRatio_x_y );
			}
		}
#endif

	/* ulNrmHrz/VrtSrcStep needed in AutoAdjRelatedToSclRatio_isr */
	hWindow->ulNrmHrzSrcStep = BVDC_P_CAL_HRZ_SRC_STEP(
		hWindow->stSrcCnt.ulWidth, hWindow->stAdjSclOut.ulWidth);
	hWindow->ulNrmVrtSrcStep = BVDC_P_CAL_VRT_SRC_STEP(
		hWindow->stSrcCnt.ulHeight, hWindow->stAdjSclOut.ulHeight);

	if((bDoAspRatCorrect) ||
	   (((0 != hWindow->stCurInfo.ulHrzSclFctRndToler) ||
		 (0 != hWindow->stCurInfo.ulVrtSclFctRndToler)) &&
	     !bMosaicMode && !bNonlinearScl))
	{
		BVDC_P_Window_AutoAdjRelatedToSclRatio_isr(hWindow, bMosaicMode,
			ulSrcPxlAspRatio, ulDspPxlAspRatio,
			pMvdFieldData->ulSourceHorizontalSize, pMvdFieldData->ulSourceVerticalSize);
	}

	hWindow->ulNonlinearSrcWidth = 0;
	hWindow->ulNonlinearSclOutWidth = 0;
	hWindow->ulCentralRegionSclOutWidth = BVDC_P_OVER_CENTER_WIDTH;

	/* bAdjRectsDirty is set to true by BVDC_P_Window_ApplyChanges_isr and
	 * BVDC_P_Window_ValidateRects_isr, or something changes here.
	 * It will get clear once the rectangle are used,
	 * BVDC_P_Window_UpdateSrcAndUserInfo_isr(). */
	hWindow->bAdjRectsDirty = false;
	hWindow->stCurInfo.stDirty.stBits.bRecAdjust = BVDC_P_DIRTY;
	hWindow->stCurInfo.stDirty.stBits.bReDetVnet = BVDC_P_DIRTY;
	if(hWindow->stCurInfo.bHistEnable && hWindow->bHistAvail)
	{
		hWindow->stCurInfo.stDirty.stBits.bHistoRect = BVDC_P_DIRTY;
	}

#if BVDC_P_SUPPORT_CM3D
	if(hWindow->stCurInfo.bChromaHistEnable && hWindow->bChromaHistAvail)
	{
		hWindow->stCurInfo.stDirty.stBits.bChromaHistoRect = BVDC_P_DIRTY;
	}
#endif

	/* The source also need to adjust it output rect. */
	hWindow->stCurInfo.hSource->stCurInfo.stDirty.stBits.bRecAdjust = BVDC_P_DIRTY;

	BDBG_LEAVE(BVDC_P_Window_InitMuteRec_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * This function is the root frunction to transfer the 3 user set window's
 * rectangles into the window's "adjusted rectangles", and then adjust them.
 *
 * The "adjusted rectangles" are the ones really used to build picture node
 * and RULs.
 *
 * The adjustment is applied in the order of box black patch cut, pan scan,
 * user's src clip, aspect ratio correctio, and scale factor.
 *
 * This func is called at every vsync.
 *
 * Note:
 *   - The pan/scan can only moves source content rectangle within the source
 *     boundary.
 *   - when "over-clipped" case, we will ignore the user source clip.
 *   - _isr rectangles setting could be done at every vsync.
 *   - (pMvdFieldData and pXvdFieldData are NULL) implies it's an analog source.
 *   - For MosaicMode, we don't wanna cut destination sub-rectangles in any kind,
 *     e.g. AllSource aspec ratio, scale factor rounding;
 *     therefore, don't adjust DstRect; also AdjSclOut is actual sub-rectangle;
 */
void BVDC_P_Window_AdjustRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  uint32_t                         ulRectIdx
)
{
	BVDC_P_Window_Info *pUserInfo;
	const BVDC_P_Source_Info *pSrcInfo;
	const BFMT_VideoInfo *pDstFmtInfo;
	BVDC_P_Rect stSrcCnt; /* src clip rect after box cut, pan-scan and user clip */
	uint32_t  ulSrcWidth, ulSrcHeight; /* full src size */
	uint32_t  ulSrcDispWidth, ulSrcDispHeight; /* pan scan disp size */
	int32_t   lHorizontalPanScan, lVerticalPanScan; /* stream pan scan x, y */
	BFMT_AspectRatio eSrcAspectRatio;
	uintAR_t  ulSrcPxlAspRatio, ulDspPxlAspRatio;  /* pixel asp ratio in U4.16 fix pt */
	const BVDC_P_ClipRect *pSrcAspRatRectClip;
	bool  bDoAspRatCorrect;
	bool  bDoPanScan;
	bool  bDoAfd = false, bDoBar = false;
	uint32_t ulAfdVal = 0;
	bool  bNonlinearScl;
	bool  bMosaicMode;
	bool  bCaptureCrc = false;
	uint16_t uiSampleAspectRatioX = 0, uiSampleAspectRatioY = 0;
	uint32_t ulBotRightBarValue=0, ulTopLeftBarValue=0;
	BAVC_BarDataType eBarDataType = BAVC_BarDataType_eInvalid;
	uint32_t  ulSrcPxlAspRatio_x_y=0;
	BVDC_Compositor_Handle hCompositor;
	BVDC_Display_Handle hDisplay;

	BDBG_ENTER(BVDC_P_Window_AdjustRectangles_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* Get user info */
	pSrcInfo  = &hWindow->stCurInfo.hSource->stCurInfo;
	pUserInfo = &hWindow->stCurInfo;
	hCompositor = hWindow->hCompositor;
	pDstFmtInfo = hCompositor->stCurInfo.pFmtInfo;
	hDisplay = hCompositor->hDisplay;
	ulDspPxlAspRatio = hCompositor->hDisplay->ulPxlAspRatio;
	BDBG_ASSERT(ulDspPxlAspRatio != 0);

	bMosaicMode = pUserInfo->bMosaicMode;

	/* --------- Collect calcu-related info from each src type ------ */
	ulSrcPxlAspRatio = 0;  /* become non-zero after calculated */
	pSrcAspRatRectClip = &hWindow->stCurInfo.hSource->stCurInfo.stAspRatRectClip;
	if(BVDC_P_SRC_IS_DS(hWindow->stCurInfo.hSource->eId) ||
	   BVDC_P_SRC_IS_ITU656(hWindow->stCurInfo.hSource->eId) ||
	   BVDC_P_SRC_IS_ANALOG(hWindow->stCurInfo.hSource->eId))
	{
		const BFMT_VideoInfo  *pSrcFmtInfo;

		pSrcFmtInfo = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;
		ulSrcWidth  = pSrcFmtInfo->ulWidth;
		ulSrcHeight = pSrcFmtInfo->ulHeight;

		/* Adjust for oversample */
		ulSrcWidth  = BVDC_P_OVER_SAMPLE(hWindow->stCurInfo.hSource->ulSampleFactor, ulSrcWidth);

		/* only used for aspect ratio correction */
		eSrcAspectRatio = hWindow->stCurInfo.hSource->stCurInfo.eAspectRatio;

		/* user afd */
		bDoAfd = (pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eUser);
		ulAfdVal = pUserInfo->stAfdSettings.ulAfd;

		/* user pan scan set-up */
		bDoPanScan = (BVDC_PanScanType_eUser == pUserInfo->ePanScanType) && !bDoAfd;
		ulSrcDispWidth  = ulSrcWidth;
		ulSrcDispHeight = ulSrcHeight;
		lHorizontalPanScan = 0; /* not used, only to make compiler quiet */
		lVerticalPanScan   = 0; /* not used, only to make compiler quiet */
	}
	else if(BVDC_P_SRC_IS_HDDVI(hWindow->stCurInfo.hSource->eId))
	{
		/* user afd */
		bDoAfd = (pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eUser);
		ulAfdVal = pUserInfo->stAfdSettings.ulAfd;

		bDoPanScan = (BVDC_PanScanType_eDisable != pUserInfo->ePanScanType) && !bDoAfd;

		BDBG_ASSERT(pXvdFieldData);
		ulSrcWidth  = pXvdFieldData->ulSourceHorizontalSize;
		ulSrcHeight = pXvdFieldData->ulSourceVerticalSize;

		/* only used for aspect ratio correction */
		eSrcAspectRatio      = pXvdFieldData->eAspectRatio;
		uiSampleAspectRatioX = pXvdFieldData->uiSampleAspectRatioX;
		uiSampleAspectRatioY = pXvdFieldData->uiSampleAspectRatioY;

		/* only used for pan scan */
		ulSrcDispWidth     = pXvdFieldData->ulDisplayHorizontalSize;
		ulSrcDispHeight    = pXvdFieldData->ulDisplayVerticalSize;
		lHorizontalPanScan = pXvdFieldData->i32_HorizontalPanScan;
		lVerticalPanScan   = pXvdFieldData->i32_VerticalPanScan;

#if BFMT_LEGACY_3DTV_SUPPORT
		if(BFMT_IS_3D_MODE(pSrcInfo->pFmtInfo->eVideoFmt)) /* 3D format */
		{
			/* HDDVI sends Right frame (bot half) plus some scrap
			   area of the 3D format through DE shift; */
			if(pSrcInfo->stHdDviSetting.bEnableDe &&
				pSrcInfo->bHVStartOverride)
			{
				/* 1080p3D has 2205-line active region;
				   DE shift by 1023-line (10-bit register) where the first
				   41-line is VBI (VSYNC_WIDTH + V_BackPorch); */
				ulSrcHeight -= (pSrcInfo->ulVstart - pSrcInfo->pFmtInfo->ulTopActive + 1);
			}
			/* HDDVI sends Left frame (top half) of the 3D format */
			else
			{
				ulSrcHeight = pSrcInfo->pFmtInfo->ulDigitalHeight;
			}
			ulSrcDispHeight = ulSrcHeight;
		}
#endif

#if (BVDC_P_SUPPORT_3D_VIDEO)
		/* bOrientationOverride only valid when original
		 * orientation from FMT is 2D */
		if(pSrcInfo->pFmtInfo->eOrientation == BFMT_Orientation_e2D)
		{
			if(pSrcInfo->bOrientationOverride)
			{
				if(pSrcInfo->eOrientation == BFMT_Orientation_e3D_LeftRight)
				{
					ulSrcWidth = ulSrcWidth / 2;
				}
				else if(pSrcInfo->eOrientation == BFMT_Orientation_e3D_OverUnder)
				{
					ulSrcHeight = ulSrcHeight / 2;
				}
			}
		}
#endif

	}
	else if (BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
	{
		BVDC_P_SurfaceInfo  *pCurSurface;

		pCurSurface = &hWindow->stCurInfo.hSource->hVfdFeeder->stGfxSurface.stCurSurInfo;
		eSrcAspectRatio = BFMT_AspectRatio_eSquarePxl;
		lHorizontalPanScan = 0;
		lVerticalPanScan = 0;
		ulSrcDispWidth = pCurSurface->ulWidth;
		ulSrcDispHeight = pCurSurface->ulHeight;
		bDoPanScan = false;
		bCaptureCrc = false;
		ulSrcWidth  = ulSrcDispWidth;
		ulSrcHeight = ulSrcDispHeight;
	}
	else /* mpeg video source */
	{
		BDBG_ASSERT(pMvdFieldData);

		ulAfdVal = (pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eStream)
			? pMvdFieldData->ulAfd : pUserInfo->stAfdSettings.ulAfd;

		bDoAfd = ((pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eUser) ||
		          ((pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eStream) &&
		          (!BVDC_P_WINDOW_ISBARDATA(ulAfdVal, pMvdFieldData->eBarDataType)) &&
		           (pMvdFieldData->bValidAfd)));

		bDoBar = ((!bDoAfd) && (pMvdFieldData->bValidAfd) &&
			(pUserInfo->stAfdSettings.eMode == BVDC_AfdMode_eStream)   &&
			(BVDC_P_WINDOW_ISBARDATA(ulAfdVal, pMvdFieldData->eBarDataType )));


		bDoPanScan = (BVDC_PanScanType_eDisable != pUserInfo->ePanScanType) && !bDoAfd && !bDoBar;

		ulSrcWidth  = pMvdFieldData->ulSourceHorizontalSize;
		ulSrcHeight = pMvdFieldData->ulSourceVerticalSize;

		/* only used for aspect ratio correction */
		eSrcAspectRatio      = pMvdFieldData->eAspectRatio;
		uiSampleAspectRatioX = pMvdFieldData->uiSampleAspectRatioX;
		uiSampleAspectRatioY = pMvdFieldData->uiSampleAspectRatioY;

		/* only used for Bar Data */
		eBarDataType       = pMvdFieldData->eBarDataType;
		ulBotRightBarValue = pMvdFieldData->ulBotRightBarValue;
		ulTopLeftBarValue  = pMvdFieldData->ulTopLeftBarValue;

		/* only used for pan scan */
		ulSrcDispWidth     = pMvdFieldData->ulDisplayHorizontalSize;
		ulSrcDispHeight    = pMvdFieldData->ulDisplayVerticalSize;
		lHorizontalPanScan = pMvdFieldData->i32_HorizontalPanScan;
		lVerticalPanScan   = pMvdFieldData->i32_VerticalPanScan;

		/* Capture CRC */
		bCaptureCrc        = pMvdFieldData->bCaptureCrc;
	}

	/* in case bad data */
	ulSrcDispWidth  = (BVDC_P_SRC_INPUT_H_MIN > ulSrcDispWidth)?  ulSrcWidth:  ulSrcDispWidth;
	ulSrcDispHeight = (BVDC_P_SRC_INPUT_V_MIN > ulSrcDispHeight)? ulSrcHeight: ulSrcDispHeight;
	ulSrcDispWidth  = BVDC_P_MIN(ulSrcWidth, ulSrcDispWidth);
	ulSrcDispHeight = BVDC_P_MIN(ulSrcHeight, ulSrcDispHeight);

	/* Handle unknown source aspect ratio */
	if(BVDC_P_IS_UNKNOWN_ASPR(eSrcAspectRatio, uiSampleAspectRatioX, uiSampleAspectRatioY))
	{
		uint32_t ulHVRatio = (ulSrcDispWidth * 100) / ulSrcDispHeight;
		eSrcAspectRatio = BVDC_P_EQ_DELTA(ulHVRatio, 130, 25)
			? BFMT_AspectRatio_e4_3 : BFMT_AspectRatio_eSquarePxl;
	}

	/* (0) Init stSrcCnt: prepare for box cut, pan scan and user's clip */
	stSrcCnt.lLeft    = 0;
	stSrcCnt.lTop     = 0;
	stSrcCnt.lLeft_R  = 0;
	stSrcCnt.ulWidth  = ulSrcWidth;
	stSrcCnt.ulHeight = ulSrcHeight;

	/* The following operations will make adjustment to dst and scl, and
	 * produce:
	 * hWindow->stSrcCnt
	 * hWindow->stAdjSclOut
	 * hWindow->stAdjDstRect
	 *
	 * For a source, the union of the stSrcCnt of all windows shared the
	 * source will be store in:
	 * hWindow->stCurInfo.hSource->stScanOut
	 *
	 * Note the adjustment does not always involve cutting, in the case off
	 * enable VBI pass-thru we'd added more lines. */
	/* --------------------------------------------------------------- */
	/* (1) do box detect and auto black patch cut                      */
	/* --------------------------------------------------------------- */
	if(hWindow->stCurResource.hBoxDetect && !bDoAfd && !bDoBar)
	{
		BVDC_P_Window_BoxCut_isr(hWindow, &stSrcCnt, bDoPanScan);
	}

	/* --------------------------------------------------------------- */
	/* (2) do pan scan (src cut)                                       */
	/* --------------------------------------------------------------- */
	if(bDoPanScan)
	{
		/* for user pan scan type, use auto-calculated SrcDisp size if it is not available */
		if((BVDC_PanScanType_eUser == pUserInfo->ePanScanType) &&
		   (ulSrcWidth  == ulSrcDispWidth) &&
		   (ulSrcHeight == ulSrcDispHeight))
		{
			/* reclculate the "auto-calculated SrcDisp size" only if we have to */
			if((hWindow->ulFieldSrcWidth  != ulSrcWidth)      ||
			   (hWindow->ulFieldSrcHeight != ulSrcHeight)     ||
			   (hWindow->eFieldSrcAspectR != eSrcAspectRatio) ||
			   (hWindow->uiSampleAspectRatioX != uiSampleAspectRatioX) ||
			   (hWindow->uiSampleAspectRatioY != uiSampleAspectRatioY) ||
			   (hWindow->bAdjRectsDirty)                      ||
			   (hWindow->stCurInfo.hSource->bPictureChanged))
			{
				hWindow->ulFieldSrcWidth  = ulSrcWidth;
				hWindow->ulFieldSrcHeight = ulSrcHeight;
				hWindow->eFieldSrcAspectR = eSrcAspectRatio;
				hWindow->uiSampleAspectRatioX = uiSampleAspectRatioX;
				hWindow->uiSampleAspectRatioY = uiSampleAspectRatioY;

				/* note: CalcuAutoDisplaySize will change the dispSize, it will affect
				 * Src Aspect Ratio. However ulSrcPxlAspRatio is not to be re-calculated
				 * and eSrcAspectRatio is no longer used */
				BVDC_P_CalcuPixelAspectRatio_isr(
					eSrcAspectRatio, uiSampleAspectRatioX, uiSampleAspectRatioY,
					ulSrcDispWidth, ulSrcDispHeight,
					pSrcAspRatRectClip, &ulSrcPxlAspRatio, &ulSrcPxlAspRatio_x_y);
				BVDC_P_Window_CalcuAutoDisplaySize_isr(hWindow, ulSrcPxlAspRatio, ulDspPxlAspRatio);
			}
			ulSrcDispWidth  = hWindow->ulAutoDispHorizontalSize;
			ulSrcDispHeight = hWindow->ulAutoDispVerticalSize;
		}

		/* perform pan scan */
		BVDC_P_Window_PanScan_isr(hWindow, &stSrcCnt, ulSrcDispWidth,
			ulSrcDispHeight, lHorizontalPanScan, lVerticalPanScan);
	}

	/* Note:
	 * 1). It is quite likely the above clipings result in the same as in last
	 *   vsync, and aspect ratio correction and scale factor rounding are
	 *   expensive, therefore it is worthwhile to optimize.
	 * 2). The following could adjust pSrcCnt or pAdjDstRect/pAdjSclOut.
	 * 3). We should not introduce field invesion when we adjust the rectangles,
	 *   that means all height offset we introduced should be multiple of 2
	 * 4). bAdjRectsDirty is set to true by BVDC_P_Window_ApplyChanges_isr and
	 * BVDC_P_Window_ValidateRects_isr */
	if((hWindow->bAdjRectsDirty) ||
	   (hWindow->stCurInfo.hSource->bPictureChanged) ||
	   (!BVDC_P_RECT_CMP_EQ(&hWindow->stPrevSrcCnt, &stSrcCnt))
#if BVDC_P_SUPPORT_MOSAIC_MODE
	   ||
	   (bMosaicMode &&
	    (pUserInfo->astMosaicRect[ulRectIdx].ulWidth != hWindow->stAdjSclOut.ulWidth ||
	     pUserInfo->astMosaicRect[ulRectIdx].ulHeight!= hWindow->stAdjSclOut.ulHeight))
#endif
	     )
	{
		BDBG_MSG(("--------------------------"));
		BVDC_P_PRINT_RECT("New stSrcCnt", &stSrcCnt, false);
		BDBG_MSG(("Window[%d] usr change = %d, src change = %d",
			hWindow->eId, hWindow->bAdjRectsDirty,
			!BVDC_P_RECT_CMP_EQ(&hWindow->stPrevSrcCnt, &stSrcCnt)));

		/* for optimization in future vsync */
		hWindow->stPrevSrcCnt = stSrcCnt;
		hWindow->stSrcCnt     = stSrcCnt;
		hWindow->stAdjDstRect = pUserInfo->stDstRect;

		/* init scale factor as 0 to indicate it needs re-compute */
		hWindow->ulNrmHrzSrcStep = 0;
		hWindow->ulNrmVrtSrcStep = 0;
		hWindow->ulNonlinearSrcWidth = 0;
		hWindow->ulNonlinearSclOutWidth = 0;
		hWindow->ulCentralRegionSclOutWidth = BVDC_P_OVER_CENTER_WIDTH;

		/* --------------------------------------------------------------- */
		/* (3) do AFd (src cut)                                            */
		/* --------------------------------------------------------------- */
		if(bDoBar)
		{
			BVDC_P_Window_Bar_isr(hWindow, &hWindow->stSrcCnt, eBarDataType, ulTopLeftBarValue, ulBotRightBarValue);
		}
		if(bDoAfd)
		{
			BVDC_P_Window_Afd_isr(hWindow, &hWindow->stSrcCnt, ulAfdVal, eSrcAspectRatio,
				uiSampleAspectRatioX, uiSampleAspectRatioY, ulSrcWidth, ulSrcHeight);
		}

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* MosaicMode: reset SclOut rect as the mosaic sub-window size */
		if(bMosaicMode)
		{
			hWindow->stAdjSclOut.lLeft    = 0;
			hWindow->stAdjSclOut.lLeft_R  = 0;
			hWindow->stAdjSclOut.lTop     = 0;
			hWindow->stAdjSclOut.ulWidth  = pUserInfo->astMosaicRect[
				BVDC_P_MIN(ulRectIdx, pUserInfo->ulMosaicCount - 1)].ulWidth;
			hWindow->stAdjSclOut.ulHeight = pUserInfo->astMosaicRect[
				BVDC_P_MIN(ulRectIdx, pUserInfo->ulMosaicCount - 1)].ulHeight;

#if (BVDC_P_WIN_MOSAIC_MODE_BVN_WORKAROUND)
			hWindow->stAdjSclOut.ulWidth = BVDC_P_MAX(hWindow->stAdjSclOut.ulWidth,
				BVDC_P_WIN_MOSAIC_OUTPUT_H_MIN);
			hWindow->stAdjSclOut.ulHeight = BVDC_P_MAX(hWindow->stAdjSclOut.ulHeight,
				BVDC_P_WIN_MOSAIC_OUTPUT_V_MIN);
#endif
		}
		else
#else
		BSTD_UNUSED(ulRectIdx);
#endif
		{
			hWindow->stAdjSclOut  = pUserInfo->stScalerOutput;
		}

		/* when a mpeg src is connected to more than one window, eUseAllDestination with
		 * mosaic mode could cause diff vertical content clipping --> bandwidth failure;
		 * if only one window is using the src, it is fine to use eUseAllDestination with
		 * mosaic mode, but we want to have the same effect no matter how many window
		 * are connected to the src */
		bDoAspRatCorrect = (
			(!bMosaicMode) &&
			((BVDC_AspectRatioMode_eUseAllDestination == hWindow->stCurInfo.eAspectRatioMode) ||
			 (BVDC_AspectRatioMode_eUseAllSource == hWindow->stCurInfo.eAspectRatioMode)));

		bNonlinearScl = (!bDoAspRatCorrect &&
						 ((0 != pUserInfo->ulNonlinearSrcWidth) ||
						  (0 != pUserInfo->ulNonlinearSclOutWidth)));

		/* --------------------------------------------------------------- */
		/* (4) do user's src clip setting, including setting by isr        */
		/* --------------------------------------------------------------- */
		/* note: _isr rectangles setting could be done at every vsync */
		BVDC_P_Window_UserSrcCut_isr(hWindow, &hWindow->stSrcCnt, &pUserInfo->stSrcClip);

		/* ------------------------------------------------------------------------
		 * (5) calculate scale factor before xfering dst cut into src cut for accuracy
		 * note: ulWidth and ulHeight are only used to decide feed/cap/scl_in/out
		 * size, and they are always aligned due to HW limitation and aligned up to
		 * avoid video info loose. The pixel + subpixel accuracy is passed to SCL by
		 * lLeft/lTop for phase and ulNrmHrzSrcStep/ulNrmVrtSrcStep for src step.
		 * ------------------------------------------------------------------------ */
		hWindow->ulNrmHrzSrcStep = BVDC_P_CAL_HRZ_SRC_STEP(
			hWindow->stSrcCnt.ulWidth, hWindow->stAdjSclOut.ulWidth);

		hWindow->ulNrmVrtSrcStep = BVDC_P_CAL_VRT_SRC_STEP(
			hWindow->stSrcCnt.ulHeight, hWindow->stAdjSclOut.ulHeight);

		/* --------------------------------------------------------------- */
		/* (6) transfer large dest cut into src cut                        */
		/* --------------------------------------------------------------- */
		/* note: AdjDstCut_isr must be done before aspect ratio correction,
		 * because the later will change scale ratio, therefore how dst cut
		 * is xfered into src clip, and please notice that aspect ratio
		 * correction does not cause new dst cut */
		if(!bMosaicMode)
		{
			BVDC_P_Window_AdjDstCut_isr(pDstFmtInfo,
				&hWindow->stSrcCnt, &hWindow->stAdjSclOut, &hWindow->stAdjDstRect);
		}

		/* According to mpeg spec the full aspect ratio is fully contributed by the
		 * ulSrcDispWidth x ulSrcDispHeight area */
		if((0 == ulSrcPxlAspRatio) &&    /* non-0 iff already calculated */
		   ((bDoAspRatCorrect) || (bNonlinearScl)))
		{
			BVDC_P_CalcuPixelAspectRatio_isr(
				eSrcAspectRatio, uiSampleAspectRatioX, uiSampleAspectRatioY,
				ulSrcDispWidth, ulSrcDispHeight,
				pSrcAspRatRectClip, &ulSrcPxlAspRatio, &ulSrcPxlAspRatio_x_y);
		}

		/* ---------------------------------------------------------------
		 * (7) do aspect ratio correction and scale factor rounding
		 * --------------------------------------------------------------- */
		if((bDoAspRatCorrect) ||
		   (((0 != pUserInfo->ulHrzSclFctRndToler) ||
		     (0 != pUserInfo->ulVrtSclFctRndToler)) && !bMosaicMode && !bNonlinearScl))
		{
#if (BVDC_P_SHOW_ASP_RAT_MSG==1)
			if (bDoAspRatCorrect)
			{
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("--- AspR correct Win Id %d, Src Id %d:",  hWindow->eId, hWindow->stCurInfo.hSource->eId));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Src: eAspR %d, (W %d, H %d), CnvsClp (L %d, R %d, T %d, B %d), ulPxlAspR 0x%Lx",
					eSrcAspectRatio, ulSrcDispWidth, ulSrcDispHeight,
					pSrcAspRatRectClip->ulLeft, pSrcAspRatRectClip->ulRight,
					pSrcAspRatRectClip->ulTop, pSrcAspRatRectClip->ulBottom,
					ulSrcPxlAspRatio));
				BDBG_MODULE_MSG(BVDC_ASP_RAT,("Dsp: eAspR %d, (W %d, H %d), CnvsClp (L %d, R %d, T %d, B %d), ulPxlAspR 0x%Lx",
					hWindow->hCompositor->stCurInfo.eAspectRatio,
					pDstFmtInfo->ulWidth, pDstFmtInfo->ulHeight,
					hWindow->hCompositor->stCurInfo.stAspRatRectClip.ulLeft, hWindow->hCompositor->stCurInfo.stAspRatRectClip.ulRight,
					hWindow->hCompositor->stCurInfo.stAspRatRectClip.ulTop, hWindow->hCompositor->stCurInfo.stAspRatRectClip.ulBottom,
					ulDspPxlAspRatio));
			}
#endif
			BVDC_P_Window_AutoAdjRelatedToSclRatio_isr(hWindow, bMosaicMode,
				ulSrcPxlAspRatio, ulDspPxlAspRatio, ulSrcWidth, ulSrcHeight);
		}

		/* ---------------------------------------------------------------
		 * (8) non-linear scaling central region should be computed after
		 * BVDC_P_Window_AdjDstCut_isr.
		 * Note: non-linear scaling doesn't coexist with overall ARC/SFR;
		 * --------------------------------------------------------------- */
		if(bNonlinearScl)
		{
			BVDC_P_Window_CentralRegionWidth_isr(hWindow,
				ulSrcPxlAspRatio, ulDspPxlAspRatio);
		}

		/* --------------------------------------------------------------- */
		/* (9) Bypass without scaler adjustment                            */
		/* --------------------------------------------------------------- */
		if((hWindow->eId == BVDC_P_WindowId_eComp2_V0) &&
			(!hWindow->stCurResource.hScaler))
		{
			/* No scaler for bypass. Need to mute video if source size
			 * does not match display format */
			if ((hWindow->stSrcCnt.ulWidth == pDstFmtInfo->ulDigitalWidth) &&
				BVDC_P_EQ_DELTA(hWindow->stSrcCnt.ulHeight, pDstFmtInfo->ulDigitalHeight, 3))
			{
				hWindow->bMuteBypass = false;
			}
			else
			{
				BDBG_MSG(("Bypass Window[%d] Src & Dst mismatch w/o scaler", hWindow->eId));
				BDBG_MSG(("Window[%d] SrcCnt: %d, %d", hWindow->eId,
					hWindow->stSrcCnt.ulWidth, hWindow->stSrcCnt.ulHeight));
				BDBG_MSG(("Window[%d] DstFmt: %d, %d", hWindow->eId,
					pDstFmtInfo->ulWidth, pDstFmtInfo->ulHeight));

				/* Reset stSrcCnt to MIN to minimize capture. */
				hWindow->stSrcCnt.ulWidth  = BVDC_P_SRC_INPUT_H_MIN;
				hWindow->stSrcCnt.ulHeight = BVDC_P_SRC_INPUT_V_MIN;

				/* Mute video */
				hWindow->bMuteBypass = true;
				BDBG_MSG(("Mute bypass Window[%d]", hWindow->eId));
			}
		}

		/* ------------------------------------------------------------------*/
		/* (10) Must not violate HW limiations.                              */
		/* scan out the minimal size can be supported.                       */
		/* ------------------------------------------------------------------*/
		if((hWindow->stSrcCnt.ulWidth  < BVDC_P_SRC_INPUT_H_MIN) && !bCaptureCrc)
		{
			int32_t  lLeftAdj;

			/* Adjust clipping */
			lLeftAdj = BVDC_P_SRC_INPUT_H_MIN - hWindow->stSrcCnt.ulWidth;
			hWindow->stSrcCnt.lLeft = BVDC_P_MAX(0,
				hWindow->stSrcCnt.lLeft - (lLeftAdj << BVDC_P_16TH_PIXEL_SHIFT));
			hWindow->stSrcCnt.lLeft_R = BVDC_P_MAX(0,
				hWindow->stSrcCnt.lLeft_R - (lLeftAdj << BVDC_P_16TH_PIXEL_SHIFT));
			hWindow->stSrcCnt.ulWidth  = BVDC_P_SRC_INPUT_H_MIN;

			hWindow->stAdjDstRect      = pUserInfo->stDstRect;
			hWindow->stAdjSclOut.ulWidth = BVDC_P_MIN(
				pDstFmtInfo->ulWidth,
				(hWindow->stSrcCnt.ulWidth << BVDC_P_NRM_SRC_STEP_F_BITS) /
				hWindow->ulNrmHrzSrcStep);
		}
		if((hWindow->stSrcCnt.ulHeight < BVDC_P_SRC_INPUT_V_MIN) && !bCaptureCrc)
		{
			int32_t  lTopAdj;

			/* Adjust clipping */
			lTopAdj = BVDC_P_SRC_INPUT_V_MIN - hWindow->stSrcCnt.ulHeight;
			hWindow->stSrcCnt.lTop = BVDC_P_MAX(0,
				hWindow->stSrcCnt.lTop - (lTopAdj << BVDC_P_16TH_PIXEL_SHIFT));
			hWindow->stSrcCnt.ulHeight = BVDC_P_SRC_INPUT_V_MIN;

			hWindow->stAdjDstRect      = pUserInfo->stDstRect;
			hWindow->stAdjSclOut.ulHeight = BVDC_P_MIN(
				pDstFmtInfo->ulHeight,
				(hWindow->stSrcCnt.ulHeight << BVDC_P_NRM_SRC_STEP_F_BITS) /
				hWindow->ulNrmVrtSrcStep);
		}

		/* ------------------------------------------------------------------*/
		/* (11) calculate STG needed pixel aspect ratio                      */
		/* ------------------------------------------------------------------*/
#if (BVDC_P_SUPPORT_STG)
		if(hCompositor->hDisplay->stStgChan.bEnable)
		{
#if defined(HUMAX_PLATFORM_BASE)
			if ((!hWindow->stSrcCnt.lLeft) && (!hWindow->stSrcCnt.lTop) &&
                            (bMosaicMode || bDoAspRatCorrect || bNonlinearScl ||
                             BVDC_P_EQ_DELTA(hWindow->stAdjSclOut.ulWidth,  pDstFmtInfo->ulWidth,  8) ||
                             BVDC_P_EQ_DELTA(hWindow->stAdjSclOut.ulHeight, pDstFmtInfo->ulHeight, 8)))
#else
			if (bMosaicMode || bDoAspRatCorrect || bNonlinearScl)
#endif			
			{
				hWindow->ulStgPxlAspRatio_x_y = hCompositor->hDisplay->ulPxlAspRatio_x_y;

			}
			else
			{
				if(0 == ulSrcPxlAspRatio)    /* non-0 iff already calculated */
				{
					BVDC_P_CalcuPixelAspectRatio_isr(
						eSrcAspectRatio, uiSampleAspectRatioX, uiSampleAspectRatioY,
						ulSrcDispWidth, ulSrcDispHeight,
						pSrcAspRatRectClip, &ulSrcPxlAspRatio, &ulSrcPxlAspRatio_x_y);
				}
				BVDC_P_ScaleSrcPxlAspRatio_isr(
					ulSrcPxlAspRatio_x_y,
					hWindow->stSrcCnt.ulWidth, hWindow->stSrcCnt.ulHeight,
					hWindow->stAdjSclOut.ulWidth, hWindow->stAdjSclOut.ulHeight,
					hCompositor->hDisplay->stCurInfo.eOrientation,
					&hWindow->ulStgPxlAspRatio_x_y );
			}
		}
#endif

		/* bAdjRectsDirty is set to true by BVDC_P_Window_ApplyChanges_isr and
		 * BVDC_P_Window_ValidateRects_isr, or something changes here.
		 * It will get clear once the rectangle are used,
		 * BVDC_P_Window_UpdateSrcAndUserInfo_isr(). */
		hWindow->bAdjRectsDirty = false;
		hWindow->stCurInfo.stDirty.stBits.bRecAdjust = BVDC_P_DIRTY;
		hWindow->stCurInfo.stDirty.stBits.bReDetVnet = BVDC_P_DIRTY;
		if(hWindow->stCurInfo.bHistEnable && hWindow->bHistAvail)
		{
			hWindow->stCurInfo.stDirty.stBits.bHistoRect = BVDC_P_DIRTY;
		}

#if BVDC_P_SUPPORT_CM3D
		if(hWindow->stCurInfo.bChromaHistEnable && hWindow->bChromaHistAvail)
		{
			hWindow->stCurInfo.stDirty.stBits.bChromaHistoRect = BVDC_P_DIRTY;
		}
#endif

		/* The source also need to adjust it output rect. */
		hWindow->stCurInfo.hSource->stCurInfo.stDirty.stBits.bRecAdjust = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_P_Window_AdjustRectangles_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 * lWinXMin, lWinYMin, lWinXMax and lWinYMax in S27.4 format
 *
 * When box detect is used with mpeg, capture and replay should be used,
 * and mpeg feeder should fetch the full src
 */
void BVDC_P_Window_GetSourceContentRect_isr
	( const BVDC_Window_Handle         hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  int32_t                         *plWinXMin,
	  int32_t                         *plWinYMin,
	  int32_t                         *plWinXMin_R,
	  int32_t                         *plWinXMax,
	  int32_t                         *plWinYMax )
{
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_ASSERT(plWinXMin);
	BDBG_ASSERT(plWinXMax);
	BDBG_ASSERT(plWinYMin);
	BDBG_ASSERT(plWinYMax);
	BSTD_UNUSED(pXvdFieldData);

	if((BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId)) &&
		(hWindow->stCurResource.hBoxDetect))
	{
		BDBG_ASSERT(pMvdFieldData);

		/* force mpeg feeder to feed the whole src */
		*plWinXMin = 0;
		*plWinYMin = 0;
		*plWinXMin_R = 0;
		*plWinXMax = pMvdFieldData->ulSourceHorizontalSize << BVDC_P_16TH_PIXEL_SHIFT;
		*plWinYMax = pMvdFieldData->ulSourceVerticalSize   << BVDC_P_16TH_PIXEL_SHIFT;
	}
	else
	{
		*plWinXMin = hWindow->stSrcCnt.lLeft;
		*plWinYMin = hWindow->stSrcCnt.lTop;
		*plWinXMin_R = hWindow->stSrcCnt.lLeft_R;
		*plWinXMax = (hWindow->stSrcCnt.lLeft +
			(hWindow->stSrcCnt.ulWidth  << BVDC_P_16TH_PIXEL_SHIFT));
		*plWinYMax = (hWindow->stSrcCnt.lTop +
			(hWindow->stSrcCnt.ulHeight << BVDC_P_16TH_PIXEL_SHIFT));
	}

	return;
}


/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect )
{
	BDBG_ENTER(BVDC_P_Window_GetNewRectangles);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(ppClipRect);
	BDBG_ASSERT(ppSclOutRect);
	BDBG_ASSERT(ppDstRect);

	/* Returns the new infos for gfx to validate. */
	*ppClipRect = &hWindow->stNewInfo.stSrcClip;
	*ppSclOutRect = &hWindow->stNewInfo.stScalerOutput;
	*ppDstRect = &hWindow->stNewInfo.stDstRect;

	BDBG_LEAVE(BVDC_P_Window_GetNewRectangles);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewRectangles
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect )
{
	BDBG_ENTER(BVDC_P_Window_GetNewRectangles);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(ppClipRect);
	BDBG_ASSERT(ppSclOutRect);
	BDBG_ASSERT(ppDstRect);

	BKNI_EnterCriticalSection();
	BVDC_P_Window_GetNewRectangles_isr(hWindow, ppClipRect, ppSclOutRect, ppDstRect);
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_P_Window_GetNewRectangles);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetCurrentRectangles_isr
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect )
{
	BDBG_ENTER(BVDC_P_Window_GetCurrentRectangles_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(ppClipRect);
	BDBG_ASSERT(ppSclOutRect);
	BDBG_ASSERT(ppDstRect);

	/* Returns the new infos for gfx to validate. */
	*ppClipRect = &hWindow->stCurInfo.stSrcClip;
	*ppSclOutRect = &hWindow->stCurInfo.stScalerOutput;
	*ppDstRect = &hWindow->stCurInfo.stDstRect;

	BDBG_LEAVE(BVDC_P_Window_GetCurrentRectangles_isr);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetCurrentRectangles
	( const BVDC_Window_Handle         hWindow,
	  const BVDC_P_ClipRect **         ppClipRect,
	  const BVDC_P_Rect **             ppSclOutRect,
	  const BVDC_P_Rect **             ppDstRect )
{
	BDBG_ENTER(BVDC_P_Window_GetCurrentRectangles);

	BKNI_EnterCriticalSection();
	BVDC_P_Window_GetCurrentRectangles_isr( hWindow, ppClipRect, ppSclOutRect, ppDstRect );
	BKNI_LeaveCriticalSection();

	BDBG_LEAVE(BVDC_P_Window_GetCurrentRectangles);
	return;
}


/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetCurrentDestColorSpace
	( const BVDC_Window_Handle         hWindow,
	  BVDC_P_CmpColorSpace            *peDestColorSpace )
{
	BDBG_ENTER(BVDC_P_Window_GetCurrentDestColorSpace);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(peDestColorSpace);

	/* Returns the curr display color space */
	*peDestColorSpace = hWindow->hCompositor->eCmpColorSpace;

	BDBG_LEAVE(BVDC_P_Window_GetCurrentDestColorSpace);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewWindowAlpha
	( const BVDC_Window_Handle         hWindow,
	  uint8_t                         *pucWindowAlpha )
{
	const BVDC_P_Window_Info *pUserInfo;

	BDBG_ENTER(BVDC_P_Window_GetNewWindowAlpha);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Returns the new infos for gfx to validate. */
	pUserInfo = &hWindow->stNewInfo;

	if(pucWindowAlpha)
	{
		*pucWindowAlpha = pUserInfo->ucAlpha;
	}

	BDBG_LEAVE(BVDC_P_Window_GetNewWindowAlpha);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewDispOrientation
	( const BVDC_Window_Handle         hWindow,
	  BFMT_Orientation                 *pOrientation)
{
#if (BVDC_P_SUPPORT_3D_VIDEO)
	BVDC_Display_Handle   hDisplay;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pOrientation);

	/* update display orientation */
	hDisplay = hWindow->hCompositor->hDisplay;
	if(BFMT_IS_3D_MODE(hDisplay->stNewInfo.pFmtInfo->eVideoFmt))
		*pOrientation = hDisplay->stNewInfo.pFmtInfo->eOrientation;
	else
		*pOrientation = hDisplay->stNewInfo.eOrientation;
#else
	BSTD_UNUSED(hWindow);
	*pOrientation = BFMT_Orientation_e2D;
#endif
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewBlendFactor
	( const BVDC_Window_Handle         hWindow,
	  BVDC_BlendFactor                *peFrontBlendFactor,
	  BVDC_BlendFactor                *peBackBlendFactor,
	  uint8_t                         *pucConstantAlpha )
{
	const BVDC_P_Window_Info *pUserInfo;

	BDBG_ENTER(BVDC_P_Window_GetNewBlendFactor);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	/* Returns the new infos for gfx to validate. */
	pUserInfo = &hWindow->stNewInfo;

	if(peFrontBlendFactor)
	{
		*peFrontBlendFactor = pUserInfo->eFrontBlendFactor;
	}
	if(peBackBlendFactor)
	{
		*peBackBlendFactor = pUserInfo->eBackBlendFactor;
	}
	if(pucConstantAlpha)
	{
		*pucConstantAlpha = pUserInfo->ucConstantAlpha;
	}

	BDBG_LEAVE(BVDC_P_Window_GetNewBlendFactor);
	return;
}

/***************************************************************************
 * {private}
 */
void BVDC_P_Window_GetNewScanType
	( const BVDC_Window_Handle         hWindow,
	  bool                            *pbInterlaced )
{
	BDBG_ENTER(BVDC_P_Window_GetNewScanType_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	if(pbInterlaced)
	{
		/* Get the new validated format from BVDC_P_Display! */
		*pbInterlaced = hWindow->hCompositor->stNewInfo.pFmtInfo->bInterlaced;
	}

	BDBG_LEAVE(BVDC_P_Window_GetNewScanType_isr);
	return;
}

/*************************************************************************
 * Generate Low pass filter
 *
 * Calculates low pass filter based on formula below, and returns result.
 * F1(n)= inp(n)*A0 + F1(n-1)*A1 - F2(n-1)*A2 + F3(n-1)*A3
 * F2(n) = F1(n-1)
 * F3(n) = F2(n-1)
 * F4(n) = F3(n-1)
 * lpf(n) = B0*F1(n) + B1*F2(n) + B2*F3(n) + B3*F4(n)
 *
 * NOTE: For internal porting interface use only!!!
 *************************************************************************/
static int32_t BVDC_P_IIRLowPassFilter
	( const BVDC_Window_Handle         hWindow,
	  int32_t                          lSample )
{
	int32_t lB;
	const int32_t alB[4] = { 1, 3, 3, 1 };
	int32_t *alLpfSamples;
	int32_t lA;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	alLpfSamples = hWindow->alLpfSamples;

	/* These are the coefficients:
	 * const double A[4] = { 1.0000, -2.609375, 2.2890625, -0.673828125 };
	 *
	 * Converting them to powers of two, you'd get:
	 * A = (1, -(2+1/2+1/8-1/64), 2+1/4+1/32+1/128, -(1/2+1/8+1/32+1/64+1/512))
	 *
	 * That is what is implemented below.
	 */

	/* convert to fixed point */
	lA = lSample << BVDC_P_LPF_FIXED_SHIFT;
	lA +=
		(alLpfSamples[0] << 1) +
		(alLpfSamples[0] >> 1) +
		(alLpfSamples[0] >> 3) -
		(alLpfSamples[0] >> 6);

	lA -=
		(alLpfSamples[1] << 1) +
		(alLpfSamples[1] >> 2) +
		(alLpfSamples[1] >> 5) +
		(alLpfSamples[1] >> 7);

	lA +=
		(alLpfSamples[2] >> 1) +
		(alLpfSamples[2] >> 3) +
		(alLpfSamples[2] >> 5) +
		(alLpfSamples[2] >> 6) +
		(alLpfSamples[2] >> 9);

	lB = (lA*alB[0] + alLpfSamples[0]*alB[1] + alLpfSamples[1]*alB[2] + alLpfSamples[2]*alB[3]) >> 3;

	alLpfSamples[2] = alLpfSamples[1];
	alLpfSamples[1] = alLpfSamples[0];
	alLpfSamples[0] = lA;

	if(alLpfSamples[2] > BVDC_P_LPF_ROLLOVER_OFFSET &&
	    alLpfSamples[1] > BVDC_P_LPF_ROLLOVER_OFFSET &&
		alLpfSamples[0] > BVDC_P_LPF_ROLLOVER_OFFSET)
	{
		alLpfSamples[2] -= BVDC_P_LPF_ROLLOVER_OFFSET;
		alLpfSamples[1] -= BVDC_P_LPF_ROLLOVER_OFFSET;
		alLpfSamples[0] -= BVDC_P_LPF_ROLLOVER_OFFSET;
		lB           -= BVDC_P_LPF_ROLLOVER_OFFSET;
	}
	else if(alLpfSamples[2] < 0 &&
		     alLpfSamples[1] < 0 &&
		     alLpfSamples[0] < 0)
	{
		alLpfSamples[2] += BVDC_P_LPF_ROLLOVER_OFFSET;
		alLpfSamples[1] += BVDC_P_LPF_ROLLOVER_OFFSET;
		alLpfSamples[0] += BVDC_P_LPF_ROLLOVER_OFFSET;
		lB           += BVDC_P_LPF_ROLLOVER_OFFSET;
	}

	/*
	 * This reduces the output by the gain of the amplifier
	 * The gain in 170 * 3/4 = 128
	 */
	lB >>= 8;
	lB += lB >> 1;

	/* And now shift away the extra bits for fixed point */
	lB >>= BVDC_P_LPF_FIXED_SHIFT;

	return lB;
}

/*************************************************************************
 *  {secret}
 *  BVDC_P_Vec_LowPassFilter
 *  Low Pass Filter function.  Software implementation of low pass
 *  filter, workaround for hardware bug in
 *  VDEC_NP_Picture_Adjust.SUBCARRIER_PHASE_LPF.
 **************************************************************************/
static int32_t BVDC_P_Vdec_LowPassFilter
	( BVDC_Window_Handle               hWindow,
	  int32_t                          lVdecPhase )
{
	int32_t lLpfVdecPhase = lVdecPhase;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* input value same as previous indicates same
	 * frame.  Use previous filtered value */
	if(lVdecPhase == hWindow->lRawVdecPhase)
	{
		goto BVDC_P_Vdec_LowPassFilter_Done;
	}

	hWindow->lRawVdecPhase = lVdecPhase;

	/* filter raw phase on every top field */
	if(hWindow->bVdecFlipPhase)
	{
		/* Flip msb (bit 9) before filtering */
		lLpfVdecPhase = (lVdecPhase & ~0x200) |
			(~lVdecPhase & 0x200);
	}

	/* handle rollover */
	if((lLpfVdecPhase - hWindow->lLpfLastResult) > (BVDC_P_LPF_ROLLOVER / 2))
	{
		lLpfVdecPhase -= BVDC_P_LPF_ROLLOVER;
	}
	else if((hWindow->lLpfLastResult - lLpfVdecPhase) > (BVDC_P_LPF_ROLLOVER / 2))
	{
		lLpfVdecPhase += BVDC_P_LPF_ROLLOVER;
	}

	hWindow->lLpfLastResult = BVDC_P_IIRLowPassFilter(hWindow, lLpfVdecPhase);
	lLpfVdecPhase = (hWindow->lLpfLastResult & (BVDC_P_LPF_ROLLOVER - 1));

	if(hWindow->bVdecFlipPhase)
	{
		/* Restore msb (bit 9) */
		lLpfVdecPhase = (lLpfVdecPhase & ~0x200) |
			(~lLpfVdecPhase & 0x200);
	}

	/* flip vdec phase bits only on every other frame */
	hWindow->bVdecFlipPhase = !hWindow->bVdecFlipPhase;
	hWindow->lLpfLastVdecPhase = lLpfVdecPhase;

BVDC_P_Vdec_LowPassFilter_Done:

	return hWindow->lLpfLastVdecPhase;
}


/***************************************************************************
 * {private}
 *
 * Check the status if this window is shut down, and ready for reconfiguring
 * source or vnet.
 */
void BVDC_P_Window_SetReconfiguring_isr
	( BVDC_Window_Handle               hWindow,
	  bool                             bSrcPending,
	  bool                             bReConfigVnet )
{
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;

	/* Get dirty bits to check if we're ready. */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurInfo  = &hWindow->stCurInfo;
	pCurDirty = &hWindow->stCurInfo.stDirty;

	/* Need to do the following when we want the window to shutdown,
	 * Notes there are mutually exclusive. */
	if((bReConfigVnet) &&
	   (!bSrcPending) &&
	   (!pCurDirty->stBits.bSrcPending))
	{
		pCurDirty->stBits.bReConfigVnet = BVDC_P_DIRTY;
	}
	else if(bSrcPending)
	{
		pCurDirty->stBits.bSrcPending   = BVDC_P_DIRTY;
		pCurDirty->stBits.bReConfigVnet = BVDC_P_CLEAN;
	}

	/* Don't shutdown again if it's in middle of shutdown process; or if
	 * we have shutdown but idle for src-pending.  Destroy must go thru
	 * shutdown in order to prevent premature return to user */
	if((hWindow->bSetDestroyEventPending) ||
	   ((pCurDirty->stBits.bShutdown == BVDC_P_CLEAN) &&
	    ((BVDC_P_State_eInactive != pCurInfo->eWriterState) ||
	     (BVDC_P_State_eInactive != pCurInfo->eReaderState))))
	{
		pCurInfo->eReaderState  = BVDC_P_State_eShutDownPending;
		pCurInfo->eWriterState  = BVDC_P_State_eShutDownPending;
		pCurDirty->stBits.bShutdown    = BVDC_P_DIRTY;
	}
	return;
}


/***************************************************************************
 * {private}
 *
 * Check the status if this window is shut down, and ready for reconfiguring
 * source or vnet.  Return true if completed.
 */
bool BVDC_P_Window_NotReconfiguring_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_DirtyBits *pCurDirty;

	/* Get dirty bits to check if we're ready. */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	pCurDirty = &hWindow->stCurInfo.stDirty;

	if(BVDC_P_IS_CLEAN(pCurDirty))
	{
		return true;
	}
	else
	{
		return ((BVDC_P_CLEAN == pCurDirty->stBits.bShutdown) &&
				(BVDC_P_CLEAN == pCurDirty->stBits.bSrcPending) &&
				(BVDC_P_CLEAN == pCurDirty->stBits.bReConfigVnet));
	}
}


/* Decision test for minimal memory bandwidth and usage:
 * ----------------------------------------------------
 * Because VDEC/656in/HD_DVI sources are fixed rate real-time pixels pump,
 * and VEC is a fixed rate real-time pixels drain, they constrain how fast
 * pixels need to be captured and fed out; the memory bandwidth is allocated
 * based on those constraints. To save memory bandwidth imposed by the
 * possible capture/playback path, we want to choose optimal placement of
 * SCL (before or after CAP/VFD) based on the potential memory rd/wr rate:
 *
 * in case of SRC -> SCL -> CAP,
 *   capture rate =
 *      (OutX * OutY) / ((SrcFmtRasterX * InY) / SrcFmtPixelClk);
 *
 * in case of VFD -> SCL -> VEC,
 *   feedout rate =
 *      (InX * InY) / (VecFmtRasterX * OutY) / VecFmtPixelClk;
 *
 * if capture rate < feedout rate, then scale first;
 */
static uint32_t BVDC_P_Window_DecideSclCapSymmetric_isr
	( BVDC_Window_Handle               hWindow,
	  const BVDC_Source_Handle         hSource,
	  const BFMT_VideoInfo            *pSrcFmt,
	  const BFMT_VideoInfo            *pDstFmt,
	  bool                             bSrcInterlace )
{
	uint32_t ulInX, ulInY, ulInLine, ulInClk;
	uint32_t ulOutX, ulOutY, ulOutLine, ulOutClk;
	uint32_t ulCapRate, ulFeedRate, ulCapFdrRate;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	ulInX     = hWindow->stSrcCnt.ulWidth;
	ulOutX    = hWindow->stAdjSclOut.ulWidth;
	ulInClk   = pSrcFmt->ulPxlFreq;
	ulOutClk  = pDstFmt->ulPxlFreq;
	ulInLine  = pSrcFmt->ulScanWidth; /* SrcFmtRasterX */
	ulOutLine = pDstFmt->ulScanWidth; /* VecFmtRasterX */

	ulInY  = (bSrcInterlace)
		? (hWindow->stSrcCnt.ulHeight / BVDC_P_FIELD_PER_FRAME)
		: (hWindow->stSrcCnt.ulHeight);

	ulOutY = (pDstFmt->bInterlaced)
		? (hWindow->stAdjSclOut.ulHeight / BVDC_P_FIELD_PER_FRAME)
		: (hWindow->stAdjSclOut.ulHeight);

	/* Adjust for oversample */
	ulInClk  = BVDC_P_OVER_SAMPLE(hSource->ulSampleFactor, ulInClk);
	ulInLine = BVDC_P_OVER_SAMPLE(hSource->ulSampleFactor, ulInLine);

	/* Make it friendly for integer divsion.  For equal bandwidth let the
	 * capture before scaler as the capture is more tolerance to external
	 * input than MAD/SCL.
	 * TODO: maybe mad's row doubling effect should be considered here  */
	if(BVDC_P_SRC_IS_MPEG(hSource->eId))
	{
		ulCapRate = (ulOutX * ulOutY) / (BVDC_P_MAX(1, 1000000 / pDstFmt->ulVertFreq));
		ulCapRate += (ulOutX * ulOutY) / (BVDC_P_MAX(1, 1000000 / ((5*pDstFmt->ulVertFreq)/100)));
	}
	else
	{
		ulCapRate = (ulOutX * ulOutY) / (BVDC_P_MAX(1, ulInLine  * ulInY / ulInClk));
	}
	ulFeedRate   = (ulInX  * ulInY ) / (BVDC_P_MAX(1, ulOutLine * ulOutY / ulOutClk));

	/* At lease 1 so not do divide by zero!  Use decision osciallation by
	 * some delta.  Here we allow to stay with the last decision if it's with
	 * some BVDC_P_BW_DELTA.  Currently it's set to 1% for rate oscillation.
	 * But cut also overschedule RTS for aspect ratio correct without
	 * toggle scl/cap, though need to find the delta and update accordingly. */
	ulFeedRate   = BVDC_P_MAX(1, ulFeedRate);
	ulCapFdrRate = (ulCapRate * BVDC_P_BW_RATE_FACTOR / ulFeedRate);

	hWindow->ulCapRate = ulCapRate;
	hWindow->ulFeedRate = ulFeedRate;

	return ulCapFdrRate;
}


/* Bandwidth equation without deinterlacer
 *    ratio = ((ceil(ox / SCB_burst_size)) / (ceil(ix / SCB_burst_size))) * sy
 * Bandwidth equation with deinterlacer
 *    ratio = ((ceil(ox / SCB_burst_size)) / (ceil(ix / SCB_burst_size))) * sy * hx * 2
 * If ratio < 1 => SCL before CAP/VFD
 * where:
 *    ceil(x) is ceiling operation that rounds up x value to the immediate next
 *            integer
 *    ix is the width of SCL input picture in pixels unit
 *    ox is the width of SCL output picture in pixels unit
 *    sy is the SCL vertical scaling ratio, i.e. sy = oy / iy, where oy is the
 *          height of SCL output picture and iy is the height of SCL input picture
 *    SCB_burst_size is the SCB burst size of CAP and VFD in pixels unit;
 *                      7420 has SCB_burst_size = 256 pixels (or 16 JWords)
 *    hx is the horizontal scaling ratio of HSCL/MCVP, i.e.
 *          hx = hscl_out_x / hscl_in_x; if HSCL is bypassed, hx = 1
 */
static uint32_t BVDC_P_Window_DecideSclCapAsymmetric_isr
	( BVDC_Window_Handle               hWindow,
	  const BFMT_VideoInfo            *pDstFmt,
	  bool                             bSrcInterlace,
	  bool                             bDeinterlace)
{
	uint32_t ulCapFdrRate;
	uint32_t ulInX, ulInY, ulOutX, ulOutY;
	uint32_t ulOutRate, ulInRate;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	ulInX      = hWindow->stSrcCnt.ulWidth;
	ulOutX     = hWindow->stAdjSclOut.ulWidth;
	ulInY  = (bSrcInterlace)
		? (hWindow->stSrcCnt.ulHeight / BVDC_P_FIELD_PER_FRAME)
		: (hWindow->stSrcCnt.ulHeight);

	ulOutY = (pDstFmt->bInterlaced)
		? (hWindow->stAdjSclOut.ulHeight / BVDC_P_FIELD_PER_FRAME)
		: (hWindow->stAdjSclOut.ulHeight);

	ulOutRate = BVDC_P_DIV_ROUND_UP(ulOutX, BVDC_P_SCB_BURST_SIZE) * ulOutY;
	ulInRate  = BVDC_P_DIV_ROUND_UP(ulInX,  BVDC_P_SCB_BURST_SIZE) * ulInY;

	/* At lease 1 so not do divide by zero!  Use decision osciallation by
	 * some delta.  Here we allow to stay with the last decision if it's with
	 * some BVDC_P_BW_DELTA.  Currently it's set to 1% for rate oscillation.
	 * But cut also overschedule RTS for aspect ratio correct without
	 * toggle scl/cap, though need to find the delta and update accordingly. */
	ulInRate   = BVDC_P_MAX(1, ulInRate);
	ulCapFdrRate = (ulOutRate * BVDC_P_BW_RATE_FACTOR / ulInRate);

	if(ulCapFdrRate > BVDC_P_BW_BASE && bDeinterlace)
	{
		/* Need to take into account the MAD -> SCL case */
		/* when MAD is placed after CAP (ulCapFdrRate > BVDC_P_BW_BASE), the */
		/* output rate of MAD must  NOT exceed oclk / (sx' * sy') */
		/* where oclk is the display specific output pixel clk rate */
		/* MAD output rate is chip specific, SD-MAD: 27MHz, HD-MAD: 148.5MHz */
		/* (sx' * sy') is the SCL scaling ratio */
		/* where sy = 2sy' when MAD is used and sx = hx*sx' when HSCL is used */
#if BVDC_P_SUPPORT_MAD_SRC_1080I
#define BVDC_P_MAD_OUTPUT_RATE   (1485 * BFMT_FREQ_FACTOR / 10)
#else
#define BVDC_P_MAD_OUTPUT_RATE   (27 * BFMT_FREQ_FACTOR)
#endif
		uint32_t ulSclRatio = (ulOutX * ulOutY * BVDC_P_BW_RATE_FACTOR) / (ulInX * hWindow->stSrcCnt.ulHeight);
#if 0
		/* TODO: Currently line buffering is not working, will include this */
		/* again for #if BVDC_P_SUPPORT_1080p_60HZ once it's fixed */
		/* HD-MAD/MCDI always have line-averaging FIFO after CMP */
		/* so oclk = Pixel Clk Rate * active pixel / pixels including blanking */
		uint32_t ulOclk = pDstFmt->ulPxlFreq * pDstFmt->ulWidth / pDstFmt->ulScanWidth;
#else
		uint32_t ulOclk = pDstFmt->ulPxlFreq ;
#endif
		uint32_t ulMadOutClk = ulOclk * BVDC_P_BW_RATE_FACTOR / ulSclRatio;

		BDBG_MSG(("BW: OutX=%d InX=%d OutY=%d InY=%d SclRatio=%d oclk=%d MadOutClk=%d",
			ulOutX, ulInX, ulOutY, hWindow->stSrcCnt.ulHeight, ulSclRatio,
			ulOclk, ulMadOutClk));

		if(ulMadOutClk > BVDC_P_MAD_OUTPUT_RATE)
		{
			BDBG_MSG(("Need to force SCL/CAP"));
			ulCapFdrRate = BVDC_P_MAD_OUTPUT_RATE * BVDC_P_BW_RATE_FACTOR / ulMadOutClk;
		}
	}

	return ulCapFdrRate;
}


/***************************************************************************
 * {private}
 *
 * Combining user info and src dynamic info, we decide new vnetMode. When
 * bApplyNewVnet is false, we only check whether a vnet reconfiguring is
 * needed, no window context is changed here, and return true if a vnet
 * reconfiguring is needed.  If bApplyNewVnet is true, we store new
 * vnetMode into win context.
 */
static bool BVDC_P_Window_DecideVnetMode_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  bool                             bApplyNewVnet,
	  bool                            *pbRfcgMcvp)
{
	const BFMT_VideoInfo *pSrcFmt;
	const BFMT_VideoInfo *pDstFmt;
	const BVDC_P_FormatInfo *pSrcFmtDetails;
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_VnetMode stVnetMode;
	BVDC_P_VnetMode *pVnetMode, *pCurVnetMode;
#if (BVDC_P_SUPPORT_MCVP)
	BVDC_P_VnetMode stMcvpMode;
	BVDC_P_VnetMode *pMcvpMode, *pCurMcvpMode;
#endif
	uint32_t ulCapFdrRate;
	bool bSclCapBaseOnRate;
	bool bSrcInterlace = false;
	bool bScalerFirst = false;
	bool bDeinterlace = false;
	bool bAnr = false;
	bool bFgt = false;
	bool bRecfgVnet = false;
	bool bStreamProgressive = false;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	pCurInfo  = &hWindow->stCurInfo;
	pDstFmt = hWindow->hCompositor->stCurInfo.pFmtInfo;
	pSrcFmt = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;
	BDBG_ASSERT(NULL!=pbRfcgMcvp);
	*pbRfcgMcvp = false;

	/* no need to calculate new vnetMode if we are destroying the window */
	if(pCurInfo->stDirty.stBits.bDestroy)
	{
		return true;
	}

	/* May need to fine tune the dst timing for mpeg source.  In the case  */
	/* "&& (NULL != pMvdFieldData)" check is added only for Coverity check */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) && (NULL != pMvdFieldData))
	{
		pSrcFmt = pDstFmt;
		bSrcInterlace = (BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity);
		bStreamProgressive = pMvdFieldData->bStreamProgressive;

#if (BVDC_P_SUPPORT_FGT)
		/* currently only mpeg src can use FGT feature */
		bFgt = ((hWindow->stCurInfo.bFgt) &&     /* User enabled */
				(hWindow->stCurInfo.hSource->hVdc->stSettings.bEnableFgt)); /* VDC default */
#endif
	}
	else
	{
		bSrcInterlace = pSrcFmt->bInterlaced;
	}

	/* Get the source info with details informations */
	pSrcFmtDetails = BVDC_P_GetFormatInfoPtr_isr(pSrcFmt);

	/* Check if we need to turn/on or off deinterlacer.
	   Note, mosaic mode should force off MAD since the input pictures list
	   could contain dynamic format change while MAD on-the-fly toggling
	   requires vnet reconfig process which take multiple vsyncs now; */
	/*bDeinterlace = false;*/
	if (hWindow->stCurInfo.bDeinterlace)
	{
		bool bMadSrcSizeOk;
		uint32_t ulMaxMadWidth, ulMaxMadHeight;
		BVDC_P_WrRateCode eWriterVsReaderRateCode;
		BVDC_P_WrRateCode eReaderVsWriterRateCode;

		BDBG_ASSERT(hWindow->stCurResource.hMad32 || hWindow->stCurResource.hMcvp);
		if (hWindow->stCurResource.hMad32)
		{
			ulMaxMadWidth  = hWindow->stCurResource.hMad32->ulMaxWidth;
			ulMaxMadHeight = hWindow->stCurResource.hMad32->ulMaxHeight;
		}
		else
		{
			ulMaxMadWidth  = hWindow->stCurResource.hMcvp->ulMaxWidth;
			ulMaxMadHeight = hWindow->stCurResource.hMcvp->ulMaxHeight;

#if (BVDC_P_SUPPORT_STG)
			/* SW7425-973: (1) USAGE POLICY: Further restrict to SD input base
			 * deinterlacing if output to vice.  Need to re-consider expose to
			 * API if usage of MAD-R becomes too non-orthogonal and
			 * customer/platform dependence. */
			if((hWindow->stCurResource.hMcvp->hMcdi->bMadr) &&
			   (hWindow->hCompositor->hDisplay->stCurInfo.bEnableStg) &&
			   ((hWindow->stAdjSclOut.ulHeight >> pDstFmt->bInterlaced) <= BFMT_720P_HEIGHT))
			{
				ulMaxMadWidth  = BFMT_PAL_WIDTH;
				ulMaxMadHeight = BFMT_PAL_HEIGHT;
			}
#endif
		}

		bMadSrcSizeOk =
			pCurInfo->stMadSettings.bShrinkWidth ||
			((hWindow->stCurInfo.hSource->stScanOut.ulHeight <= ulMaxMadHeight) &&
			 (hWindow->stCurInfo.hSource->stScanOut.ulWidth  <= ulMaxMadWidth));

		bDeinterlace = (
			(bSrcInterlace) &&
			(bMadSrcSizeOk) &&
			/* MAD can be enabled in PIG mode with new BW equation. */
			(!hWindow->bSclCapSymmetric || hWindow->bDstFullScreen) &&
			(!hWindow->stCurInfo.bMosaicMode));

		/* SW7425-772: (2) USAGE POLICY:  Better to have deinterlacer to be off
		 * in this case. */
		if(BVDC_Mode_eOff != pCurInfo->stMadSettings.ePqEnhancement)
		{
			bool bRateGap;
			if(hWindow->bSyncLockSrc && bStreamProgressive)
			{
				bRateGap = false;
			}
			else
			{
				uint32_t ulVertFreq = (hWindow->bSyncLockSrc)
					? hWindow->stCurInfo.hSource->ulStreamVertFreq
					: hWindow->stCurInfo.hSource->ulVertFreq;

				BVDC_P_Buffer_CalculateRateGap(ulVertFreq, pDstFmt->ulVertFreq,
					&eWriterVsReaderRateCode, &eReaderVsWriterRateCode);

				bRateGap = (BVDC_P_WrRate_NotFaster != eReaderVsWriterRateCode);
			}

			if((!bRateGap) &&
			   ( pDstFmt->bInterlaced) &&
			   ( hWindow->ulNrmVrtSrcStep == (1<<BVDC_P_NRM_SRC_STEP_F_BITS)))
			{
				bDeinterlace = false;
			}
		}
	}

	/* check if we need to turn off anr due to anr hw limit */
	bAnr = hWindow->stCurInfo.bAnr;     /* User enabled */

	/* Further restrictions for fixed rate source */
#if (BVDC_P_SUPPORT_ANR_VER < BVDC_P_ANR_VER_2) /* pre 3548Ax */
	if(!BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		bAnr &= ((pSrcFmt->ulPxlFreq <= 7425) &&  /* All ATSCs upto 1080i@60hz, 1080p@30Hz */
				 (pSrcFmt->ulVertFreq > (30*BFMT_FREQ_FACTOR)));   /* Excludes 1080p@24hz/25Hz/30Hz */
	}
#endif

	if(hWindow->bSclCapSymmetric)
	{
		ulCapFdrRate = BVDC_P_Window_DecideSclCapSymmetric_isr(
		               hWindow, hWindow->stCurInfo.hSource, pSrcFmt, pDstFmt, bSrcInterlace);
	}
	else
	{
		ulCapFdrRate = BVDC_P_Window_DecideSclCapAsymmetric_isr(
		               hWindow, pDstFmt, bSrcInterlace, bDeinterlace);
	}

	if(BVDC_P_EQ_DELTA(ulCapFdrRate, BVDC_P_BW_BASE, hWindow->stCurInfo.ulBandwidthDelta))
	{
		if(hWindow->stCurInfo.eSclCapBias == BVDC_SclCapBias_eAuto)
		{
			if(hWindow->bSclCapSymmetric)
			{
				bSclCapBaseOnRate = BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode);
				BDBG_MSG(("Rate [r=%d/%d] is optimized either way: %s!", ulCapFdrRate,
					BVDC_P_BW_BASE, bSclCapBaseOnRate ? "SCL -> CAP" : "CAP -> SCL"));
			}
			else
			{
				bSclCapBaseOnRate = true;
				BDBG_MSG(("Rate [r=%d/%d] is more optimized if: SCL -> CAP", ulCapFdrRate,
					BVDC_P_BW_BASE));
			}
		}
		else if(hWindow->stCurInfo.eSclCapBias == BVDC_SclCapBias_eSclBeforeCap)
		{
			bSclCapBaseOnRate = true;
			BDBG_MSG(("Rate [r=%d/%d] is bias: SCL -> CAP", ulCapFdrRate,
				BVDC_P_BW_BASE));
		}
		else /* hWindow->stCurInfo.eSclCapBias == BVDC_SclCapBias_eSclAfterCap */
		{
			bSclCapBaseOnRate = false;
			BDBG_MSG(("Rate [r=%d/%d] is bias: CAP -> SCL", ulCapFdrRate,
				BVDC_P_BW_BASE));
		}
	}
	else if(ulCapFdrRate > BVDC_P_BW_BASE)
	{
		bSclCapBaseOnRate = false;
		BDBG_MSG(("Rate [r=%d/%d] is more optimized if: CAP -> SCL", ulCapFdrRate,
			BVDC_P_BW_BASE));
	}
	else
	{
		bSclCapBaseOnRate = true;
		BDBG_MSG(("Rate [r=%d/%d] is more optimized if: SCL -> CAP", ulCapFdrRate,
			BVDC_P_BW_BASE));
	}

	bScalerFirst =
		/* Feed rate to high for post scaling! */
		( (bSclCapBaseOnRate) ||

		/* Scale first on sd format with HD or ED sources */
		((pSrcFmtDetails->bHd || pSrcFmtDetails->bEd) && (VIDEO_FORMAT_IS_SD(pDstFmt->eVideoFmt))) ||

		  /* MosaicMode: always scale first! */
		  (hWindow->stCurInfo.bMosaicMode) );

	/* calculate new vnetMode */
	pVnetMode = &stVnetMode;
	pCurVnetMode = &hWindow->stVnetMode;
	BVDC_P_CLEAN_ALL_DIRTY(pVnetMode);
#if (BVDC_P_SUPPORT_MCVP)
	pMcvpMode = &stMcvpMode;
	pCurMcvpMode = &hWindow->stMcvpMode;
	BVDC_P_CLEAN_ALL_DIRTY(pMcvpMode);
	pMcvpMode->stBits.bUseMcvp = pVnetMode->stBits.bUseMcvp =
		((pCurInfo->bDeinterlace && (NULL !=hWindow->stSettings.pMinSrcFmt)) ||
		 (bAnr) ||
		 (bDeinterlace)) &&
		(!pCurInfo->bMosaicMode) ?
		BVDC_P_ON : BVDC_P_OFF;
	pMcvpMode->stBits.bUseMvpBypass = ((BVDC_P_ON == pMcvpMode->stBits.bUseMcvp) && (!bDeinterlace) && (!bAnr)) ? BVDC_P_ON : BVDC_P_OFF;
	pMcvpMode->stBits.bUseMad = (bDeinterlace)? BVDC_P_ON : BVDC_P_OFF;
	pMcvpMode->stBits.bUseCap = (hWindow->bCapture)? BVDC_P_ON : BVDC_P_OFF;
	pMcvpMode->stBits.bSclBeforeCap = (pMcvpMode->stBits.bUseCap && bScalerFirst) ? BVDC_P_ON : BVDC_P_OFF;
#if BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED
	pMcvpMode->stBits.bUseHscl = (bDeinterlace || bAnr)? BVDC_P_ON : BVDC_P_OFF;
#endif
#if BVDC_P_SUPPORT_MANR
	pMcvpMode->stBits.bUseAnr = (bAnr)? BVDC_P_ON : BVDC_P_OFF;
#endif
#endif
#if (BVDC_P_SUPPORT_MAD)
	pVnetMode->stBits.bUseMad = (bDeinterlace)? BVDC_P_ON : BVDC_P_OFF;
#endif
	pVnetMode->stBits.bUseScl = (NULL != hWindow->stCurResource.hScaler)? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bUseCap = (hWindow->bCapture)? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bUseDnr = (hWindow->stCurInfo.hSource->stCurInfo.bDnr)? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bUseFgt = (bFgt)? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bUseHist = (hWindow->stCurInfo.bHistEnable && hWindow->bHistAvail) ? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bUseHistAtSrc = (pVnetMode->stBits.bUseHist && hWindow->stCurInfo.bHistAtSrc) ? BVDC_P_ON : BVDC_P_OFF;
	pVnetMode->stBits.bSclBeforeCap = (pVnetMode->stBits.bUseCap && bScalerFirst) ? BVDC_P_ON : BVDC_P_OFF;
#if (!BVDC_P_SUPPORT_MANR)
	pVnetMode->stBits.bUseAnr = (bAnr)? BVDC_P_ON : BVDC_P_OFF;
#endif

#if BVDC_P_SUPPORT_HSCL
	pVnetMode->stBits.bUseHscl = (bDeinterlace || bAnr)? BVDC_P_ON : BVDC_P_OFF;
#endif

	/* Enable bInvalid bit if vnetmode is not set */
	if(*(uint32_t *) pVnetMode == 0)
		pVnetMode->stBits.bInvalid = BVDC_P_ON;

	if(*(uint32_t *) pVnetMode != *(uint32_t *) pCurVnetMode)
	{
		BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] %s vnetMode 0x%x -> 0x%x UseMAD %d -> %d",
			hWindow->eId, bApplyNewVnet ? "changes" : "checks",
			*(uint32_t *) pCurVnetMode, *(uint32_t *) pVnetMode,
			pCurVnetMode->stBits.bUseMad, pVnetMode->stBits.bUseMad));

		bRecfgVnet = true;
		if (bApplyNewVnet)
		{
			/* store new vnetMode into win contrext */
			*(uint32_t *) pCurVnetMode = *(uint32_t *) pVnetMode;
		}
	}

#if (BVDC_P_SUPPORT_MCVP)
	if(*(uint32_t *) pMcvpMode != *(uint32_t *) pCurMcvpMode)
	{
		if(pMcvpMode->stBits.bUseMvpBypass != pCurMcvpMode->stBits.bUseMvpBypass)
		{
			*pbRfcgMcvp = true;
			if(NULL!= hWindow->stCurResource.hMcvp)
				hWindow->stCurResource.hMcvp->hMcdi->bInitial = true;
		}
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] %s ReconfgMcvp %d McvpMode 0x%x -> 0x%x \n UseMcvp %d -> %d, \n UseMad %d -> %d \n BypassMcvp %d -> %d ",
		hWindow->eId, bApplyNewVnet ? "changes" : "checks", *pbRfcgMcvp,
		*(uint32_t *) pCurMcvpMode, *(uint32_t *) pMcvpMode,
		pCurMcvpMode->stBits.bUseMcvp, pMcvpMode->stBits.bUseMcvp,
		pCurMcvpMode->stBits.bUseMad,  pMcvpMode->stBits.bUseMad,
		pCurMcvpMode->stBits.bUseMvpBypass, pMcvpMode->stBits.bUseMvpBypass));
		*(uint32_t *) pCurMcvpMode = *(uint32_t *) pMcvpMode;
	}
#endif

	return bRecfgVnet;
}

/***************************************************************************
 * {private}
 *
 * Combining user info and src dynamic info, we decide new cap/mad/anr bufs
 * heapId. When bApplyNewCfg is false, we only check whether a vnet
 * reconfiguring is needed, no window context is changed here, and return
 * true if a vnet reconfiguring is needed.  If bApplyNewCfg is true,
 * we store new buf configures into win context.
 */
static bool BVDC_P_Window_DecideBufsCfgs_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  bool                             bApplyNewCfg )
{
	const BFMT_VideoInfo *pDstFmtInfo;
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_BufferHeapId  eBufferHeapIdRequest, eBufferHeapIdPrefer;
	BVDC_P_BufHeapAllocMode eBufAllocMode;
	uint32_t ulSrcVertRate, ulDspVertRate, ulBufSize;
	bool bCapInterlaced, bInterlacedSrc;
	bool bSrcInterlace = false;
	bool bCapture = false;
	bool bDoPulldown = false;
	bool bRecfgVnet = false;
	BVDC_P_Rect stCapBufRect;
	BVDC_P_Rect stMadBufRect;
	BVDC_P_Rect stMinBufRect;
	const BFMT_VideoInfo * pMinSrcFmt = NULL;
	const BFMT_VideoInfo * pMinDspFmt = NULL;
#if BVDC_P_SUPPORT_MCVP
	bool bDeinterlace = false;
	bool bAnr = false;
#endif
#if (BVDC_P_SUPPORT_3D_VIDEO)
	bool bSrcIs3d = false, bDispIs3d = false;
#endif
	uint32_t ulMinSrcSize, ulMinDspSize, ulSrcSize;
	BFMT_Orientation eSrcOrientation = BFMT_Orientation_e2D, eDspOrientation = BFMT_Orientation_e2D;
	const BVDC_P_Source_Info *pSrcInfo;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	pCurInfo  = &hWindow->stCurInfo;
	pDstFmtInfo = hWindow->hCompositor->stCurInfo.pFmtInfo;
	pSrcInfo  = &hWindow->stCurInfo.hSource->stCurInfo;

	bCapture = BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode);
	bInterlacedSrc = bSrcInterlace = ((BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) && (NULL != pMvdFieldData))?
					 (BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity) :
					 (hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced));
	stCapBufRect = stMadBufRect = stMinBufRect = hWindow->stCurInfo.hSource->stScanOut;
	BDBG_MODULE_MSG(BVDC_WIN_BUF_SIZE, ("0 Src Mad %4dx%4d",
		stMadBufRect.ulWidth, stMadBufRect.ulHeight));

#if (BVDC_P_SUPPORT_3D_VIDEO)
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) &&
	   (NULL != pMvdFieldData))
	{
		if(pSrcInfo->bOrientationOverride)
		{
			bSrcIs3d =
				(pSrcInfo->eOrientation != BFMT_Orientation_e2D);
			eSrcOrientation = pSrcInfo->eOrientation;
		}
		else
		{
			bSrcIs3d = (pMvdFieldData->eOrientation != BFMT_Orientation_e2D);
			eSrcOrientation = pMvdFieldData->eOrientation;
		}
	}
	else if(BVDC_P_SRC_IS_HDDVI(hWindow->stCurInfo.hSource->eId))
	{
		/* bOrientationOverride only valid when original
		 * orientation from FMT is 2D */
		if((pSrcInfo->bOrientationOverride) &&
		  (pSrcInfo->pFmtInfo->eOrientation == BFMT_Orientation_e2D))
		{
			bSrcIs3d =
				(pSrcInfo->eOrientation != BFMT_Orientation_e2D);
			eSrcOrientation = pSrcInfo->eOrientation;
		}
		else
		{
			bSrcIs3d =
				(pSrcInfo->pFmtInfo->eOrientation != BFMT_Orientation_e2D);
			eSrcOrientation = pSrcInfo->pFmtInfo->eOrientation;
		}
	}
	else
	{
		bSrcIs3d = false;
	}

	if(BFMT_IS_3D_MODE(hWindow->hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
		eDspOrientation = hWindow->hCompositor->stCurInfo.pFmtInfo->eOrientation;
	else
		eDspOrientation = hWindow->hCompositor->stCurInfo.eOrientation;

	bDispIs3d = (eDspOrientation != BFMT_Orientation_e2D);
#endif

	/* hWindow->stCurInfo.hSource->stScanOut is already set to the full scan out
	 * for other sources in BVDC_P_Source_GetScanOutRect_isr */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) && (NULL != pMvdFieldData))
	{
#if (!BVDC_P_OPTIMIZE_MEM_USAGE)
		stCapBufRect.ulWidth  = stMadBufRect.ulWidth = pMvdFieldData->ulSourceHorizontalSize;
		stCapBufRect.ulHeight = stMadBufRect.ulHeight= pMvdFieldData->ulSourceVerticalSize;
#endif
		/* guaranteed some minimal memory allocation to avoid reallocations. */
		pMinSrcFmt = hWindow->stSettings.pMinSrcFmt;
		if(pMinSrcFmt)
		{
			stMinBufRect.ulWidth  = pMinSrcFmt->ulWidth >> (eSrcOrientation == BFMT_Orientation_e3D_LeftRight);
			stMinBufRect.ulHeight = pMinSrcFmt->ulHeight>> (eSrcOrientation == BFMT_Orientation_e3D_OverUnder);
			stMinBufRect.lLeft = stMinBufRect.lLeft_R = stMinBufRect.lTop = 0;

			/* rough estimation minSrc and Src buffer size requirement */
			BVDC_P_Window_GetBufSize_isr(hWindow, &stMinBufRect,
				pMinSrcFmt->bInterlaced, pCurInfo->ePixelFormat, &pCurInfo->stCapTestFeature1,
				BVDC_P_BufHeapType_eCapture, &ulMinSrcSize);

			BVDC_P_Window_GetBufSize_isr(hWindow, &stCapBufRect, bInterlacedSrc,
				pCurInfo->ePixelFormat, &pCurInfo->stCapTestFeature1,
				BVDC_P_BufHeapType_eCapture, &ulSrcSize);

			pMinSrcFmt = hWindow->stSettings.pMinSrcFmt;

			if(ulSrcSize < ulMinSrcSize)
			{
				stCapBufRect.ulWidth = stMadBufRect.ulWidth =
					pMinSrcFmt->ulWidth >> (eSrcOrientation == BFMT_Orientation_e3D_LeftRight);
				stCapBufRect.ulHeight= stMadBufRect.ulHeight=
					pMinSrcFmt->ulHeight>> (eSrcOrientation == BFMT_Orientation_e3D_OverUnder);
				bInterlacedSrc = pMinSrcFmt->bInterlaced;
				stMadBufRect.ulHeight >>= (!bInterlacedSrc);
			}
		}
	}

	if(hWindow->stSettings.pMinSrcFmt)
		BDBG_MODULE_MSG(BVDC_WIN_BUF_SIZE, ("MinSrc %4dx%4d",
		hWindow->stSettings.pMinSrcFmt->ulWidth >> (eSrcOrientation == BFMT_Orientation_e3D_LeftRight),
		hWindow->stSettings.pMinSrcFmt->ulHeight>> (eSrcOrientation == BFMT_Orientation_e3D_OverUnder)));

	if(hWindow->stSettings.pMinDspFmt)
		BDBG_MODULE_MSG(BVDC_WIN_BUF_SIZE, ("MinDsp %4dx%4d",
			hWindow->stSettings.pMinDspFmt->ulWidth, hWindow->stSettings.pMinDspFmt->ulHeight));

	/* (1) decide anr buf heapId.
	 * changing Anr buf pixel format, heapId, or cnt causes VNET reconfiguration */
#if BVDC_P_SUPPORT_ANR
	if((NULL != hWindow->stCurResource.hAnr) &&
	   BVDC_P_VNET_USED_ANR(hWindow->stVnetMode))
	{
		BVDC_P_BufferHeapId eNewAnrHeapId;
		uint16_t  usNewAnrBufferCnt;
		BPXL_Format eNewAnrPixel;
		bool bNewAnrSplitBuf = false;
		const BVDC_Video_TestFeature1_Settings *pNewTestFeature1 = NULL;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		uint32_t  ulBufSizeTestFeature1;
#endif

		/* user setting for ANR pixel format */
#if (BVDC_P_SUPPORT_ANR_VER > 1 || BVDC_P_SUPPORT_MANR)
		eNewAnrPixel = hWindow->stCurInfo.stAnrSettings.ePxlFormat;
#else
		eNewAnrPixel = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		pNewTestFeature1 = &hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1;
#endif

		/* anr input after hrz scale */
		BVDC_P_Window_GetBufSize_isr(hWindow, &stCapBufRect,
			bInterlacedSrc, eNewAnrPixel, pNewTestFeature1,
			BVDC_P_BufHeapType_eAnr, &ulBufSize);
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if((bSrcIs3d && BVDC_P_VNET_USED_ANR_AT_WRITER(hWindow->stVnetMode)) ||
		   (bDispIs3d && BVDC_P_VNET_USED_ANR_AT_READER(hWindow->stVnetMode)))
		{
			ulBufSize = 2 * ulBufSize;
		}
#endif
		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewAnrHeapId, NULL);
		usNewAnrBufferCnt =
			(bSrcInterlace)? 2 * BVDC_P_ANR_BUFFER_COUNT : BVDC_P_ANR_BUFFER_COUNT;

#if (BVDC_P_DCX_ANR_CROSS_OVER_WORKAROUND)
		/* Get real bufsize needed when TestFeature1 is enabled */
		if(pNewTestFeature1 && pNewTestFeature1->bEnable)
		{
			uint32_t  ulBufHeapSize;

			ulBufSizeTestFeature1 = BVDC_P_Window_GetDCXBufSize_isr(hWindow,
				&stCapBufRect, bInterlacedSrc, pNewTestFeature1);

			/* With TestFeature1 enabled, need separate CAP and VFD
			 * buffers in ANR */
			BVDC_P_BufferHeap_GetHeapSizeById(hWindow->hCompositor->hVdc->hBufferHeap,
				eNewAnrHeapId, &ulBufHeapSize);
			if(2*ulBufSizeTestFeature1 > ulBufHeapSize)
			{
				usNewAnrBufferCnt *= 2;
				bNewAnrSplitBuf    = false;
			}
			else
			{
				bNewAnrSplitBuf    = true;
			}
			BDBG_MSG(("ulBufHeapSize: %d, ulBufSizeTestFeature1: %d, bSplitBuf: %d",
				ulBufHeapSize, ulBufSizeTestFeature1, hWindow->stCurResource.hAnr->bSplitBuf));
		}
#endif

		/* changing Anr buf pixel format, heapId, or cnt causes VNET reconfiguration */
		if((hWindow->eAnrHeapId != eNewAnrHeapId) ||
		   (hWindow->stCurResource.hAnr->ePxlFmt != eNewAnrPixel)  ||
		   (hWindow->stCurResource.hAnr->bSplitBuf != bNewAnrSplitBuf) ||
		   (hWindow->usAnrPixelBufferCnt != usNewAnrBufferCnt))
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] changes ANR buf %d %s -> %d %s",
				hWindow->eId, hWindow->usAnrPixelBufferCnt,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eAnrHeapId),
				usNewAnrBufferCnt, BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eNewAnrHeapId)));

			if(!bApplyNewCfg)
			{
				return true;
			}
			else
			{
				/* store changes into win context */
				hWindow->eAnrHeapId = eNewAnrHeapId;
				hWindow->usAnrPixelBufferCnt = usNewAnrBufferCnt;
				hWindow->stCurResource.hAnr->bSplitBuf = bNewAnrSplitBuf;
				BVDC_P_Anr_SetBufPxlFmt_isr(hWindow->stCurResource.hAnr, eNewAnrPixel);
				bRecfgVnet = true;
			}
		}
	}
#endif

#if BVDC_P_SUPPORT_MAD
	/* (2) decide mad buf heapId.
	 * changing MAD buffer heapId or cnt causes VNET reconfiguration
	 */
	if((NULL != hWindow->stCurResource.hMad32) &&
	   BVDC_P_VNET_USED_MAD(hWindow->stVnetMode))
	{
		BVDC_P_BufferHeapId eNewMadPixelHeapId, eNewMadQmHeapId;
		uint16_t  usMadPixBufCnt;
		const BVDC_Video_TestFeature1_Settings *pNewTestFeature1 = NULL;

		/* Changing MAD buffer cntr causes VNET reconfiguration */
		usMadPixBufCnt = BVDC_P_Mad_GetPixBufCnt_isr(
			hWindow->stSettings.bDeinterlacerAllocFull ?
			BVDC_MadGameMode_eOff : hWindow->stCurInfo.stMadSettings.eGameMode);

		if(hWindow->usMadPixelBufferCnt != usMadPixBufCnt)
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] changes mad buf cntr %d->%d", hWindow->eId,
				hWindow->usMadPixelBufferCnt, usMadPixBufCnt));

			if(!bApplyNewCfg)
			{
				return true;
			}
			else
			{
				/* store new values into win context */
				hWindow->usMadPixelBufferCnt = usMadPixBufCnt;
				bRecfgVnet = true;
			}
		}

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		pNewTestFeature1 = &hWindow->stCurInfo.stMadSettings.stTestFeature1;
#endif
		if (pCurInfo->stMadSettings.bShrinkWidth)
		{
			/* horizontal scl before MAD will shrink width to the MAD supported size */
			stMadBufRect.ulWidth = BVDC_P_MIN(stMadBufRect.ulWidth,
				hWindow->stCurResource.hMad32->ulMaxWidth);
		}

		/* pixel filed buffer  */
		BVDC_P_Window_GetBufSize_isr(hWindow, &stMadBufRect,
			bSrcInterlace, hWindow->stCurInfo.stMadSettings.ePixelFmt,
			pNewTestFeature1, BVDC_P_BufHeapType_eMad_Pixel, &ulBufSize);
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if((bSrcIs3d && BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode)) ||
		   (bDispIs3d && BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode)))
		{
			ulBufSize = 2 * ulBufSize;
		}
#endif
		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewMadPixelHeapId, NULL);

		/* Qm filed buffer */
		BVDC_P_Window_GetBufSize_isr(hWindow, &stMadBufRect,
			bSrcInterlace, hWindow->stCurInfo.stMadSettings.ePixelFmt,
			pNewTestFeature1, BVDC_P_BufHeapType_eMad_QM, &ulBufSize);
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if((bSrcIs3d && BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode)) ||
		   (bDispIs3d && BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode)))
		{
			ulBufSize = 2 * ulBufSize;
		}
#endif
		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewMadQmHeapId, NULL);

		/* Changing MAD buffer heapId causes VNET reconfiguration */
		if( (hWindow->eMadPixelHeapId != eNewMadPixelHeapId) ||
		    (hWindow->eMadQmHeapId != eNewMadQmHeapId) )
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] changes mad buf MStartHpId[%s->%s], QmHpId[%s->%s]", hWindow->eId,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eMadPixelHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eNewMadPixelHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eMadQmHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eNewMadQmHeapId)));

			if(!bApplyNewCfg)
			{
				return true;
			}
			else
			{
				/* store new values into win context */
				hWindow->eMadPixelHeapId = eNewMadPixelHeapId;
				hWindow->eMadQmHeapId = eNewMadQmHeapId;
				bRecfgVnet = true;
			}
		}
	}
#endif

#if BVDC_P_SUPPORT_MCVP
	bDeinterlace = BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode);
	bAnr = BVDC_P_VNET_USED_ANR(hWindow->stMcvpMode);
	if((NULL != hWindow->stCurResource.hMcvp) &&
	   (bDeinterlace || bAnr ))
	{
		uint16_t usMcvpPixBufCnt;
		BVDC_P_BufferHeapId eNewMcvpHeapId, eNewMcvpQmHeapId;
		BVDC_Video_TestFeature1_Settings *pNewTestFeature1 = NULL;

		if(hWindow->stSettings.bDeinterlacerAllocFull || bSrcInterlace){
			usMcvpPixBufCnt =BVDC_P_Mcdi_GetPixBufCnt_isr(
					hWindow->stCurResource.hMcvp->hMcdi,
					BVDC_MadGameMode_eOff);
		}
		else
		{
			usMcvpPixBufCnt = BVDC_P_ANR_BUFFER_COUNT;
		}

		/* ANR Only buffer requirement: 
		 interlace stream:   buffer count 4: size: src fmt
		 progressive stream: buffer count 2, size  src fmt. 
		 Thus for progressive alternative, it is
		 progressive stream: buffer count 4, 1/2 size  src fmt. 
		*/
		if(!bDeinterlace && bAnr && (pMinSrcFmt==NULL))
		{
			stMadBufRect.ulHeight >>= (!bSrcInterlace);
		}		

		if(hWindow->usMadPixelBufferCnt != usMcvpPixBufCnt)
		{
			/* bSrcInterlace must have changed */
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] changes mcvp buf cntr %d->%d", hWindow->eId,
				hWindow->usMadPixelBufferCnt, usMcvpPixBufCnt));

			/* no reconfig if deinterlacer buffer count changes when MinSrcFmt is defined 
			user prefer seamless transaction without flash when src switch between interlaced and progressive */
			if((!bApplyNewCfg) && (pMinSrcFmt==NULL))
			{
				return true;
			}
			else
			{
				/* store new values into win context */
				hWindow->usMadPixelBufferCnt = usMcvpPixBufCnt;
				bRecfgVnet = (pMinSrcFmt==NULL);
			}
		}

		/* (2) decide mcvp buf heap id
		 * changing MCVP buffer heapId causes VNET reconfiguration */
		if (pCurInfo->stMadSettings.bShrinkWidth && bDeinterlace)
		{
			/* horizontal scl before MAD will shrink width to the MAD supported size */
			stMadBufRect.ulWidth = BVDC_P_MIN(stMadBufRect.ulWidth,
				hWindow->stCurResource.hMcvp->ulMaxWidth);
		}
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		pNewTestFeature1 = (bDeinterlace)?
			&hWindow->stCurInfo.stMadSettings.stTestFeature1 :
			&hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1;
#endif
		BVDC_P_Window_GetBufSize_isr(hWindow, &stMadBufRect,
			hWindow->stSettings.bDeinterlacerAllocFull? bInterlacedSrc:bSrcInterlace,
			hWindow->stCurInfo.stMadSettings.ePixelFmt,
			pNewTestFeature1, BVDC_P_BufHeapType_eMad_Pixel, &ulBufSize);
		hWindow->stCurResource.hMcvp->bBufIsContinuous = false;
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if((bSrcIs3d && BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode)) ||
		   (bDispIs3d && BVDC_P_VNET_USED_MCVP_AT_READER(hWindow->stVnetMode)))
		{
			ulBufSize = 2 * ulBufSize;
		}

		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewMcvpHeapId, NULL);

		/* Temp fix to use 2 continous 2HD buffer as 1 4HD buffer for ANR */
		if(eNewMcvpHeapId == BVDC_P_BufferHeapId_eUnknown)
		{
			BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize/2,
				&eNewMcvpHeapId, NULL);
			hWindow->usMadPixelBufferCnt = 2*hWindow->usMadPixelBufferCnt;
			hWindow->stCurResource.hMcvp->bBufIsContinuous = true;
		}

#else
		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewMcvpHeapId, NULL);
#endif

		/* Qm filed buffer */
		BVDC_P_Window_GetBufSize_isr(hWindow, &stMadBufRect,
			hWindow->stSettings.bDeinterlacerAllocFull? bInterlacedSrc:bSrcInterlace,
			hWindow->stCurInfo.stMadSettings.ePixelFmt,
			pNewTestFeature1, BVDC_P_BufHeapType_eMad_QM, &ulBufSize);
#if (BVDC_P_SUPPORT_3D_VIDEO)
		if((bSrcIs3d && BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode)) ||
		   (bDispIs3d && BVDC_P_VNET_USED_MCVP_AT_READER(hWindow->stVnetMode)))
		{
			ulBufSize = 2 * ulBufSize;
		}
#endif
		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eNewMcvpQmHeapId, NULL);

		/* Changing MCVP buffer heapId causes VNET reconfiguration */
		if(
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
		   (hWindow->stCurResource.hMcvp->hMcdi->stTestFeature1.bEnable != pNewTestFeature1->bEnable) ||
		   (hWindow->stCurResource.hMcvp->hMcdi->stTestFeature1.ulBitsPerPixel != pNewTestFeature1->ulBitsPerPixel) ||
#endif
		   (hWindow->eMadPixelHeapId != eNewMcvpHeapId  ) ||
		   (hWindow->eMadQmHeapId    != eNewMcvpQmHeapId))
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] changes mcvp buf MStartHpId[%s->%s], QmHpId[%s->%s]", hWindow->eId,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eMadPixelHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eNewMcvpHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eMadQmHeapId),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eNewMcvpQmHeapId)));

			if(!bApplyNewCfg)
			{
				return true;
			}
			else
			{
				/* store new values into win context */
				hWindow->eMadPixelHeapId = eNewMcvpHeapId;
				hWindow->eMadQmHeapId = eNewMcvpQmHeapId;
				hWindow->eAnrHeapId = eNewMcvpHeapId;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
				hWindow->stCurResource.hMcvp->hMcdi->stTestFeature1.bEnable = pNewTestFeature1->bEnable;
				hWindow->stCurResource.hMcvp->hMcdi->stTestFeature1.ulBitsPerPixel = pNewTestFeature1->ulBitsPerPixel;
#endif
				bRecfgVnet = true;
			}
		}

#if BVDC_P_SUPPORT_MANR
		if(hWindow->stCurResource.hMcvp->hAnr)
			BVDC_P_Anr_SetBufPxlFmt_isr(hWindow->stCurResource.hMcvp->hAnr, hWindow->stCurInfo.stAnrSettings.ePxlFormat);
#endif

		/* TODO: assert TestFeature1 mismatch between MAD and ANR */
#if (BDBG_DEBUG_BUILD)
		BDBG_ASSERT(!bDeinterlace || !bAnr ||
					(hWindow->stCurInfo.stMadSettings.ePixelFmt ==
					 hWindow->stCurInfo.stAnrSettings.ePxlFormat));
#endif
	}
#endif

	/* (3) decide cap buf heap Id
	 * changing cap buf heapId causes VNET reconfiguration
	 */
	ulSrcVertRate = BVDC_P_ROUND_OFF(
		hWindow->stCurInfo.hSource->ulVertFreq, (BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);
	ulDspVertRate = BVDC_P_ROUND_OFF(
		pDstFmtInfo->ulVertFreq, (BFMT_FREQ_FACTOR/2), BFMT_FREQ_FACTOR);

	if((!bInterlacedSrc) && BVDC_P_DO_PULLDOWN(ulSrcVertRate, ulDspVertRate))
	{
		/* P2I frame rate conversion case: frame capture -> fields playback,
		 * ideally for 24/25/30 to 50/60 pull-down */
		bCapInterlaced = false;
		bDoPulldown = true;
	}
	else
	{
		bCapInterlaced = pDstFmtInfo->bInterlaced;
		bDoPulldown = false;
	}
	/* always store bDoPulldown into win context because its change might not need
	 * need vnet reconfigure */
	hWindow->bDoPulldown = bDoPulldown;

	if(bCapture)
	{
		BVDC_P_WrRateCode eWriterVsReaderRateCode;
		BVDC_P_WrRateCode eReaderVsWriterRateCode;

		if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
		{
			bCapInterlaced = bInterlacedSrc;

			BVDC_P_Window_GetBufSize_isr(hWindow, &stCapBufRect, bCapInterlaced,
				pCurInfo->ePixelFormat, &pCurInfo->stCapTestFeature1,
				BVDC_P_BufHeapType_eCapture, &ulBufSize);
		}
		else /* cap and scl used, scl before cap */
		{
			const BFMT_VideoInfo *pAllocFmt = pDstFmtInfo;
			uint32_t    ulMinDspWidth, ulMinDspHeight;
			pMinDspFmt = hWindow->stSettings.pMinDspFmt;
			stCapBufRect = (pCurInfo->bMosaicMode)
				? pCurInfo->stScalerOutput : hWindow->stAdjDstRect;

			/* pAllocFmt */
			stMinBufRect.ulWidth  = pAllocFmt->ulWidth;
			stMinBufRect.ulHeight = pAllocFmt->ulHeight;
			stMinBufRect.lLeft    = stMinBufRect.lLeft_R = stMinBufRect.lTop = 0;
			/* guaranteed some minimal memory allocation to avoid reallocations
			 * when display output format changes. */
			if(pMinDspFmt)
			{
				ulMinDspWidth = pMinDspFmt->ulWidth >> (eDspOrientation==BFMT_Orientation_e3D_LeftRight);
				ulMinDspHeight= pMinDspFmt->ulHeight>> (eDspOrientation==BFMT_Orientation_e3D_OverUnder);
				if((ulMinDspWidth * ulMinDspHeight) >
				   (pAllocFmt->ulWidth * pAllocFmt->ulHeight))
				{
					pAllocFmt = pMinDspFmt;
					stMinBufRect.ulWidth  = ulMinDspWidth;
					stMinBufRect.ulHeight = ulMinDspHeight;
				}
			}


#if (BVDC_P_DCX_HSIZE_WORKAROUND || BVDC_P_MADR_HSIZE_WORKAROUND)
			/* the "+ 1" is for potential cap left/top align down, and
			 * the 4 is for DCX_HSIZE_WORKAROUND */
			stCapBufRect.ulWidth  = BVDC_P_ALIGN_UP(stCapBufRect.ulWidth + 1,  4);
			stCapBufRect.ulHeight = BVDC_P_ALIGN_UP(stCapBufRect.ulHeight + 1, 2);
			stMinBufRect.ulWidth  = BVDC_P_ALIGN_UP(stMinBufRect.ulWidth  + 1, 4);
			stMinBufRect.ulHeight = BVDC_P_ALIGN_UP(stMinBufRect.ulHeight + 1, 2);
#endif
			/* Don't optimize memory for PIG/PBP or format changes, use fullscreen size */
			ulMinDspSize = 0;
			if((pMinDspFmt) ||
			   (hWindow->stSettings.bAllocFullScreen))
			{
				/*stCapBufRect.ulWidth  = BVDC_P_MAX(stCapBufRect.ulWidth,  pAllocFmt->ulWidth);
				stCapBufRect.ulHeight = BVDC_P_MAX(stCapBufRect.ulHeight, pAllocFmt->ulHeight);*/
				BVDC_P_Window_GetBufSize_isr(hWindow, &stMinBufRect, pAllocFmt->bInterlaced,
				pCurInfo->ePixelFormat, &pCurInfo->stCapTestFeature1,
				BVDC_P_BufHeapType_eCapture, &ulMinDspSize);
			}

			BVDC_P_Window_GetBufSize_isr(hWindow, &stCapBufRect, bCapInterlaced,
			pCurInfo->ePixelFormat, &pCurInfo->stCapTestFeature1,
			BVDC_P_BufHeapType_eCapture, &ulBufSize);

			if(ulBufSize < ulMinDspSize)
			{
				ulBufSize = ulMinDspSize;
				stCapBufRect.ulWidth  = stMinBufRect.ulWidth ;
				stCapBufRect.ulHeight = stMinBufRect.ulHeight;
			}
		}

		BVDC_P_Window_GetBufHeapId_isr(hWindow, ulBufSize,
			&eBufferHeapIdRequest, &eBufferHeapIdPrefer);

#if (BVDC_P_SUPPORT_3D_VIDEO)
		/* Check if left and right buffers can be combined */
		/* Capture in 3D mode if src orientation is 3D */
		if(bSrcIs3d)
		{
			/* Always create separate buffers for Left and right for now */
			eBufAllocMode = BVDC_P_BufHeapAllocMode_eLRSeparate;
		}
		else
#endif
		{
			eBufAllocMode = BVDC_P_BufHeapAllocMode_eLeftOnly;
		}

		/* changing eBufAllocMode causes  re-allocate right buffers only */
		if(hWindow->eBufAllocMode != eBufAllocMode)
		{
			pCurInfo->stDirty.stBits.bBufAllocMode = BVDC_P_DIRTY;

			BVDC_P_BUF_MSG(("Win[%d] changes cap buf allocmode %d -> %d",
				hWindow->eId, hWindow->eBufAllocMode, eBufAllocMode));
			hWindow->ePrevBufAllocMode = hWindow->eBufAllocMode;
			hWindow->eBufAllocMode = eBufAllocMode;
		}

		/* changing cap buf heapId causes VNET reconfiguration */
		if(hWindow->eBufferHeapIdRequest != eBufferHeapIdRequest)
		{
			BVDC_P_BUF_MSG(("Win[%d] changes cap buf heapIdRequest %s -> %s", hWindow->eId,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest),
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(eBufferHeapIdRequest)));

			if(!bApplyNewCfg)
			{
				return true;
			}
			else
			{
				/* store changes into win context */
				hWindow->eBufferHeapIdRequest = eBufferHeapIdRequest;
				hWindow->ePrevBufAllocMode = hWindow->eBufAllocMode;
				hWindow->eBufAllocMode = eBufAllocMode;
				bRecfgVnet = true;
			}
		}

		if(bApplyNewCfg)
		{
			hWindow->eBufferHeapIdPrefer = eBufferHeapIdPrefer;
		}

		/* Here we determine if the capture is going to store the picture
		 * as frame. We always store bFrameCapture into win context because
		 * its change might not need vnet reconfigure.
		 * TODO: what is the relation of bCapInterlaced and bFrameCapture?? */
		hWindow->bFrameCapture = (
			/* (1) Window require do pulldown */
			(bDoPulldown) ||

			/* (2) Destination is progressive, and we don't use the scaler,
			 * or the scaler is at the writer. */
			((!pDstFmtInfo->bInterlaced) &&
			 ( BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode) ||
			   (!BVDC_P_VNET_USED_SCALER(hWindow->stVnetMode)))) ||

			/* (3) Source is progressive, and scaler is not on the writer. */
			((!hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced) &&
			 ( BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))));

		/* if source dynamic format change results in possible buffer count change,
		   do it as soon as possible to avoid unnecessary big allocation in the 1st
		   place;
		   Note, ulBufCntNeeded computed at ApplyChanges time might not reflect
		   the current situation since the source format might have changed! */
		BVDC_P_Buffer_CalculateRateGap(hWindow->stCurInfo.hSource->ulVertFreq,
			hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
			&eWriterVsReaderRateCode, &eReaderVsWriterRateCode);

		bDoPulldown =
			((!hWindow->bSyncLockSrc) && (!hWindow->stSettings.bForceSyncLock) && !BVDC_P_SRC_IS_VFD(hWindow->stNewInfo.hSource->eId) &&
			((VIDEO_FORMAT_IS_PROGRESSIVE(hWindow->hCompositor->stNewInfo.pFmtInfo->eVideoFmt)
			&& (eWriterVsReaderRateCode == eReaderVsWriterRateCode)) || bDoPulldown));

		if(bDoPulldown && !hWindow->bBufferCntDecremented)
		{
			if (hWindow->bBufferCntIncremented)
			{
				/* From N+1 buffers to the N buffers first */
				hWindow->ulBufCntNeeded --;
				hWindow->ulBufDelay--;

				hWindow->bBufferCntIncremented = false;
			}

			/* From N buffers to N-1 buffers */
			hWindow->ulBufCntNeeded --;

			hWindow->bBufferCntDecrementedForPullDown = true;
			hWindow->bBufferCntDecremented = true;
			BDBG_MSG(("Win[%d] Decrementing buffer count to %d in BVDC_P_Window_DecideBufsCfgs_isr",
				hWindow->eId, hWindow->ulBufCntNeeded));
		}
		else if(!bDoPulldown && hWindow->bBufferCntDecremented)
		{
			/* From N-1 buffers to N buffers first */
			hWindow->ulBufCntNeeded ++;
			if (!hWindow->bBufferCntDecrementedForPullDown)
			{
				hWindow->ulBufDelay++;
				hWindow->hBuffer->ulVsyncDelay++;
			}
			else
			{
				hWindow->bBufferCntDecrementedForPullDown = false;
			}
			hWindow->bBufferCntDecremented = false;

			BDBG_MSG(("Win[%d] Change buffer count back to %d in BVDC_P_Window_DecideBufsCfgs_isr",
				hWindow->eId, hWindow->ulBufCntNeeded));
		}
	}

	return bRecfgVnet;
}

/***************************************************************************
 * {private}
 *
 * This function determines how the window is going to handle picture
 * cadences. Based on the result, the window will either capture
 * the picture using the original source polarity or alternet the captured
 * picture polarity. On the playback side, the window will either
 * match VEC polarity against the captured picture polarity or simply
 * ignore cadence matching.
 *
 */
void BVDC_P_Window_SetCadenceHandling_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  bool                             bTrickMode,
	  bool                             bInit )
{
	uint32_t  ulSrcVertRate, ulDstVertRate;
	bool bSrcInterlaced;
	bool bFieldDrop = false;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* Reset cadence handling related fields */
	hWindow->stCadHndl.bForceAltCap = false;
	hWindow->stCadHndl.bReaderCadMatching = true;
	hWindow->stCadHndl.bTrickMode = false;

	/* */
	ulDstVertRate = BVDC_P_ROUND_OFF(hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq,
		BFMT_FREQ_FACTOR/2, BFMT_FREQ_FACTOR);
	ulSrcVertRate = BVDC_P_ROUND_OFF(hWindow->stCurInfo.hSource->ulVertFreq,
		BFMT_FREQ_FACTOR/2, BFMT_FREQ_FACTOR);
	if (BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) && (NULL != pMvdFieldData))
	{
		bSrcInterlaced = (BAVC_Polarity_eFrame != pMvdFieldData->eSourcePolarity);
		hWindow->stCadHndl.bHandleFldInv =
			(bSrcInterlaced &&
			 hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced &&
			 !(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) ||
			   BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode)) &&
			 pMvdFieldData->bIgnoreCadenceMatch);
	}
	else
	{
		bSrcInterlaced = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->bInterlaced;
		hWindow->stCadHndl.bHandleFldInv =
			(bSrcInterlaced &&
			 hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced &&
			 !(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) ||
			   BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode)) &&
			 ((ulSrcVertRate == 50 && ulDstVertRate == 60) ||
			  (ulSrcVertRate == 60 && ulDstVertRate == 50)));
	}

	/* Check if the window is handling the case that decoder does
	 * 50->60Hz rate conversion by repeating a field of every five or does
	 * 60->50Hz rate conversion by dropping a field of every six.
	 * In this case, the pictures scanned out from decoder doesn't follow
	 * the T/B/T/B cadence and we have to rely on scaler to correct.
	 * Trick mode and non-trick mode are handled the same.
	 */

	/* Note: When doing 60->50Hz rate converstion, in addition to dropping
	 * one field out of six, another option is dropping two fields out of
	 * every twelve fields. In this way, the picture scanned out still follow
	 * the cadence. However this come at the price of more motion judder.
	 * DM will set "bIgnoreCadenceMatch" to false in this frame drop mode.
	 */
	hWindow->stCadHndl.bDecoderRateCov = BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) &&
						(((ulSrcVertRate == 60) && (ulDstVertRate == 60) &&
						(BVDC_P_FrameRate50Hz(pMvdFieldData->eFrameRateCode))) ||
						((ulSrcVertRate == 50) && (ulDstVertRate == 50) &&
						(BVDC_P_FrameRate60Hz(pMvdFieldData->eFrameRateCode))));

	if (hWindow->stCadHndl.bDecoderRateCov && (NULL != pMvdFieldData))
	{
		bFieldDrop = pMvdFieldData->bIgnoreCadenceMatch;
	}

	if (hWindow->bSyncLockSrc)
	{
		/* Sync-locked window reader doesn't match captured picture polarity
		 * against VEC. For interlaced source, decoder display manager should
		 * guarantee pictures being scanned out have the correct cadence sequence.
		 * For progressive source and interlaced display, or decoder performs 50<->60Hz
		 * convertion, or in trick modes, VDC will alternate capture polarity for
		 * interlaced display if scaler is at writer side.
		 */
		hWindow->stCadHndl.bReaderCadMatching = false;

		if ( BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode) &&
			 (((!bSrcInterlaced) && (hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced)
			 &&(!hWindow->bFrameCapture)) || (hWindow->stCadHndl.bDecoderRateCov && bFieldDrop)))
		{
			hWindow->stCadHndl.bForceAltCap = true;
		}
		else
		{
			/* In this category, force alternating capture is enabled
			 * only in trick mode, scaler at writer side and interlaced source.
			 */
			if (bTrickMode && BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode)
				&& bSrcInterlaced)
			{
				hWindow->stCadHndl.bForceAltCap = true;
			}
			else
			{
				hWindow->stCadHndl.bForceAltCap = false;
			}
		}
	}
	else
	{
		/* Always capture as source polarity and no reader cadence
		 * match in one of the following senarios:
		 *
		 *	1) Progressive display.
		 *	   This is because either the picture has been captured
		 *	   as frame, or we have MAD/Scaler on the reader side
		 *	   to convert it. Our VNET configuration must guarantee
		 *	   this.
		 *
		 *	2) Picture is captured as frame.
		 *	   Scaler or VFD (if no scaler at reader side) will convert
		 *	   the frame to field with proper polarity.
		 *
		 *	3) MAD is at reader side.
		 *	   MAD will convert the picture to frame, then scaler
		 *	   will convert the frame to field with proper polarity.
		 */
		if ((!hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced)	||
			(hWindow->bFrameCapture) ||
			(BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode)) ||
			(BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stMcvpMode)))
		{
			hWindow->stCadHndl.bForceAltCap = false;
			hWindow->stCadHndl.bReaderCadMatching = false;
		}
		/* Progressive source and capture as field for interlaced display.
		 */
		else if ((!bSrcInterlaced) && (!hWindow->bFrameCapture))
		{
			hWindow->stCadHndl.bForceAltCap = true;
			hWindow->stCadHndl.bReaderCadMatching = true;
		}
		/* Interlaced source and interlaced display.
		 */
		else
		{
			/* For MPEG source, decoder might convert frame rate from 50Hz to 60Hz or from
			 * 60Hz to 50Hz by repeating or dropping field. Cases such as progressive pictures from DM,
			 * progressive display and capture as frame are handled as usual.
			 *
			 * When pictures from DM is interlaced and display is also interlaced, we
			 * rely on scaler to do field conversion.
			 */
			if (hWindow->stCadHndl.bDecoderRateCov)
			{
				if (BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode) ||
					(!hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced))
				{
					/* If scaler is at reader side, we will use it to do polarity correction.
					 * No forced alternative capturing and cadence matching for progressive display.
					 */
					hWindow->stCadHndl.bForceAltCap = false;
					hWindow->stCadHndl.bReaderCadMatching = bFieldDrop ? false : true;
				}
				else
				{

					/* If scaler is at writer side, writer side will generate the
					 * correct T/B/T/B cadence and reader side will match captured
					 * polarity against VEC polarity.
					 */
					hWindow->stCadHndl.bForceAltCap = bFieldDrop;
					hWindow->stCadHndl.bReaderCadMatching = true;
				}
			}
			else if (!bTrickMode)
			{
				/* Always capture as source polarity and rely on
				 * reader side to match VEC polarity by repeating.
				 */
				hWindow->stCadHndl.bForceAltCap = false;
				hWindow->stCadHndl.bReaderCadMatching = true;
			}
			else
			{
				if (BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode))
				{
					hWindow->stCadHndl.bForceAltCap = true;
					hWindow->stCadHndl.bReaderCadMatching = true;
				}
				else
				{
					/* Rely on scaler to compensate */
					hWindow->stCadHndl.bForceAltCap = false;
					hWindow->stCadHndl.bReaderCadMatching = false;
				}
			}
		}
	}

/*  SW7125-762: eLastCapPolarity should be reset when bInit is true, indicating
	vnet reconfiguration or reset. Other conditions should be able to be
	removed without adverse effects.
*/

/*	if (bInit && hWindow->stCadHndl.bForceAltCap &&
		(hWindow->stCadHndl.eLastCapPolarity == BAVC_Polarity_eFrame))
*/
	if (bInit)
	{
		hWindow->stCadHndl.eLastCapPolarity = BAVC_Polarity_eInvalid;
	}

	BDBG_MSG(( "Win[%d]: ForceAltCap=%d, ReaderCadMatching=%d, TrickMode=%d ",
				hWindow->eId, (int) hWindow->stCadHndl.bForceAltCap,
				(int) hWindow->stCadHndl.bReaderCadMatching, (int) bTrickMode));
}



/***************************************************************************
 * {private}
 *
 * This function detects trick modes and adjusts how the window is going to
 * handle picture cadences.
 */
void BVDC_P_Window_AdjustCadenceHandling_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData )
{
	bool bTrickMode;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/* ignore muted pictures
	 */
	if (pMvdFieldData != NULL)
	{
		bTrickMode = pMvdFieldData->bIgnoreCadenceMatch;
	}
	else
	{
		bTrickMode = hWindow->stCadHndl.bTrickMode;
	}

	if (hWindow->stCadHndl.bTrickMode != bTrickMode)
	{
		BVDC_P_Window_SetCadenceHandling_isr(hWindow, pMvdFieldData, bTrickMode, false);
		hWindow->stCadHndl.bTrickMode = bTrickMode;

		BDBG_MSG(("Win[%d], %s trick mode", hWindow->eId, bTrickMode ? "enter" : "exit"));
	}
}


/***************************************************************************
 * {private}
 *
 * This function sets a picture's polarity to be captured
 *
 */
void BVDC_P_Window_SetCapturePolarity_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  BVDC_P_PictureNode              *pPicture )
{
	uint16_t usMadVsyncDelay = 0;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
#if BVDC_P_SUPPORT_MAD
	if (BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode))
	{
		usMadVsyncDelay = BVDC_P_Mad_GetVsyncDelayNum_isr(
			hWindow->stCurInfo.stMadSettings.eGameMode);
	}
#endif
#if BVDC_P_SUPPORT_MCVP
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stMcvpMode))
	{
		usMadVsyncDelay = BVDC_P_Mcdi_GetVsyncDelayNum_isr(
			hWindow->stCurResource.hMcvp->hMcdi,
			hWindow->stCurInfo.stMadSettings.eGameMode);
	}
#endif

	/* Predicting capture polarity is needed only if scaler is at writer side
	 * Sync-locked path always uses interrupt polarity carried by picture to
	 * do prediction. Sync-slipped path always uses source polarity.
	 * If MAD is at writer side, delays introduced by it should be offset.
	 */
	if (BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode))
	{
		if (hWindow->stCadHndl.bForceAltCap)
		{
			if (hWindow->bSyncLockSrc && (NULL != pMvdFieldData))
			{
				/* Sync-Locked source guarantees pMvdFieldData field is not NULL */
				pPicture->eDstPolarity = (usMadVsyncDelay % BVDC_P_FIELD_PER_FRAME) ?
							BVDC_P_NEXT_POLARITY(pMvdFieldData->eInterruptPolarity) :
							pMvdFieldData->eInterruptPolarity;
			}
			else
			{
				pPicture->eDstPolarity = (hWindow->stCadHndl.eLastCapPolarity == BAVC_Polarity_eInvalid) ?
					BAVC_Polarity_eTopField :
					BVDC_P_NEXT_POLARITY(hWindow->stCadHndl.eLastCapPolarity);
			}
		}
		else
		{
			if ((!hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced) || hWindow->bFrameCapture)
			{
				pPicture->eDstPolarity = BAVC_Polarity_eFrame;
			}
			else
			{
				if (hWindow->bSyncLockSrc && (NULL != pMvdFieldData))
				{
					/* Sync-Locked source guarantees pMvdFieldData field is not NULL */
					pPicture->eDstPolarity = (usMadVsyncDelay % BVDC_P_FIELD_PER_FRAME) ?
								BVDC_P_NEXT_POLARITY(pMvdFieldData->eInterruptPolarity) :
								pMvdFieldData->eInterruptPolarity;
				}
				else
				{
					pPicture->eDstPolarity = (usMadVsyncDelay % BVDC_P_FIELD_PER_FRAME) ?
							BVDC_P_NEXT_POLARITY(pPicture->eSrcPolarity) : pPicture->eSrcPolarity;
				}
			}
		}
	}
	else
	{
		pPicture->eDstPolarity = pPicture->eSrcPolarity;
	}

	pPicture->stFlags.bCadMatching = hWindow->stCadHndl.bReaderCadMatching;
	hWindow->stCadHndl.eLastCapPolarity = pPicture->eDstPolarity;
}


/***************************************************************************
 *
 */
static void BVDC_P_Window_UpdateCallback_isr
( BVDC_Window_Handle               hWindow,
 const BAVC_Polarity              eFieldId )
{
	bool  bValidTimestamp = true;
	const BVDC_P_Source_Info   *pSrcInfo;
	BVDC_P_Window_Info         *pCurInfo;
	unsigned int uiNewVsyncDelay, ulVsyncRate;
	uint32_t     ulAdjustCnt;
	uint32_t     ulMpegVecDelay;
	uint32_t  ulCaptureTimestamp = 0, ulPlaybackTimestamp = 0;
	uint32_t  ulCurrDriftDelay = 0, ulPrevDriftDelay, ulDeltaDriftDelay;
	uint32_t ulGameModeDelay = 0;
	uint32_t ulUsecPerVsync;
	BVDC_P_PictureNode       *pNode;
	BVDC_Window_CallbackData *pCbData;
	BVDC_Window_CallbackMask *pCbMask;
	BVDC_Window_CallbackSettings *pCbSettings;
	uint16_t usMadVsyncDelay = 0;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	pSrcInfo = &hWindow->stCurInfo.hSource->stCurInfo;
	pCurInfo = &hWindow->stCurInfo;

	pCbData = &hWindow->stCallbackData;
	pCbMask = &pCbData->stMask;
	pCbSettings = &hWindow->stCallbackSettings;

	/* (0) initialize uiNewVsyncDelay and ulAdjustCnt */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		/* actually this is RUL delay: from a picture info is passed to VDC to be built into RUL,
		   to when it's actually triggered to be scaned out;  */
		ulMpegVecDelay = BVDC_P_LIP_SYNC_VEC_DELAY;
	}
	else /* PR40895: non-mpeg window shall exclude "vec" delay due to live-in/live-out; */
	{
		ulMpegVecDelay = 0;
	}
	uiNewVsyncDelay = ulAdjustCnt = ulMpegVecDelay;

	/* (1) Update vsync delay (excluding user offset);
	          This delay is ideally the center of the drift range excluding the user delay offset. */
	/* (1.1) total multibuffer delay - user offset */
	if(hWindow->bCapture)
	{
		uiNewVsyncDelay += (hWindow->ulBufDelay - pCurInfo->uiVsyncDelayOffset);
	}

	/* (1.2) deinterlace delay */
#if BVDC_P_SUPPORT_MAD
	if(BVDC_P_VNET_USED_MAD(hWindow->stVnetMode))
	{
		usMadVsyncDelay = BVDC_P_Mad_GetVsyncDelayNum_isr(
			hWindow->stCurInfo.stMadSettings.eGameMode);
		uiNewVsyncDelay += usMadVsyncDelay;
		ulAdjustCnt     += usMadVsyncDelay;
	}
#endif
#if BVDC_P_SUPPORT_MCVP
	if(BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode))
	{
		usMadVsyncDelay = BVDC_P_Mcdi_GetVsyncDelayNum_isr(
			hWindow->stCurResource.hMcvp->hMcdi,
			hWindow->stCurInfo.stMadSettings.eGameMode);
		uiNewVsyncDelay += usMadVsyncDelay;
		ulAdjustCnt     += usMadVsyncDelay;
	}
#endif

	BSTD_UNUSED(usMadVsyncDelay);

	/* (1.3) 3D comb delay */
#if BVDC_P_SUPPORT_VDEC
	if((pSrcInfo->bEnable3DComb) &&
	   (hWindow->stCurInfo.hSource->hVdec->b3DComb))
	{
		uiNewVsyncDelay += hWindow->stCurInfo.hSource->hVdec->ul3DCombVsyncDelay;
		ulAdjustCnt     += hWindow->stCurInfo.hSource->hVdec->ul3DCombVsyncDelay;
	}
#endif

	/* total delay includes user offset minus VEC delay; this is for MPEG source trigger field swap logic; */
	hWindow->ulTotalBvnDelay = uiNewVsyncDelay + pCurInfo->uiVsyncDelayOffset - ulMpegVecDelay;

	if( pCurInfo->pfGenCallback )
	{
		/* (2) Update drift delay */
		if( hWindow->bCapture )
		{
			if( hWindow->bSyncLockSrc )
			{
				ulAdjustCnt += hWindow->ulBufCntNeeded - 1;
			}
			else
			{
				/* Wait until both reader and writer RUL executed.
				 * Check for non-zero is a simple way for that, can also check
				 * actual RUL execution status
				 */
				pNode = hWindow->pCurReaderNode;
				if( pNode )
				{
					ulCaptureTimestamp  = pNode->ulCaptureTimestamp;
					ulPlaybackTimestamp = pNode->ulPlaybackTimestamp;
					bValidTimestamp = true;
				}

				if( !ulCaptureTimestamp || !ulPlaybackTimestamp )
				{
					BDBG_MSG(("Wind%d: Wait for valid time stamp", hWindow->eId));
					bValidTimestamp = false;
					pCbMask->bDriftDelay = BVDC_P_CLEAN;
				}

				ulGameModeDelay = ulCurrDriftDelay = hWindow->ulCurBufDelay;
			}
		}

		/* Minus user delay offset */
		if( pCurInfo->uiVsyncDelayOffset )
			ulAdjustCnt -= pCurInfo->uiVsyncDelayOffset;

		ulUsecPerVsync = BVDC_P_USEC_ONE_VSYNC_INTERVAL(pSrcInfo->pFmtInfo->ulVertFreq);
		ulCurrDriftDelay += ulAdjustCnt * ulUsecPerVsync;

#if (!BVDC_P_USE_RDC_TIMESTAMP)
		/* Check drift delay
		 * On a heavy loaded system, VDC can miss interrupts for more than
		 * 3 fields. Ignore those cases. The drift delay should
		 * be < uiNewVsyncDelay + 1 field + RUL execution time
		 */
		if(((uiNewVsyncDelay+1)* ulUsecPerVsync +
			BVDC_P_MULTIBUFFER_RW_TOLERANCE) < ulCurrDriftDelay)
		{
			BDBG_MSG(("Win[%d]: Invalid time stamp %d -- %d fields, gamedelay = %d, adj=%d, new=%d",
				hWindow->eId, ulCurrDriftDelay,
				ulCurrDriftDelay / ulUsecPerVsync, ulGameModeDelay, ulAdjustCnt, uiNewVsyncDelay));
			bValidTimestamp = false;
		}
#endif

		/* Update drift delay change */
		ulPrevDriftDelay = pCbData->ulDriftDelay;
		if( ulPrevDriftDelay < ulCurrDriftDelay )
			ulDeltaDriftDelay = ulCurrDriftDelay - ulPrevDriftDelay;
		else
			ulDeltaDriftDelay = ulPrevDriftDelay - ulCurrDriftDelay;

		/* Slower of the source/display pair dictates the rate Vsync delay value is based on; */
		ulVsyncRate = (hWindow->stCurInfo.hSource->ulVertFreq < hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq)
			? hWindow->stCurInfo.hSource->ulVertFreq
			: hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq;

		/* NOTE: BVN path delay is ignored for NRT transcode mode since BVN
		 * doesn't insert bubble pictures to STG/ViCE2!  Only DM non-ignore
		 * picture will be encoded in NRT mode!  That means NRT mode AV sync
		 * delay matching doesn't need to count the BVN vsync delay! */
#if (BVDC_P_SUPPORT_STG)
		if(hWindow->hCompositor->hDisplay->stCurInfo.bStgNonRealTime) {
			uiNewVsyncDelay = 0;
		}
#endif

		/* (3) Which one triggers callback? */
		if( ((uiNewVsyncDelay != pCbData->ulVsyncDelay) ||
		      (ulVsyncRate         != pCbData->ulVsyncRate)  ||
		     (pCbData->ulVsyncDelay == BVDC_P_LIP_SYNC_RESET_DELAY)) &&
		    pCbSettings->stMask.bVsyncDelay )
		{
			pCbMask->bVsyncDelay = BVDC_P_DIRTY;
		}

		if( ((ulDeltaDriftDelay >= pCbSettings->ulLipSyncTolerance) ||
		    (pCbData->ulDriftDelay == BVDC_P_LIP_SYNC_RESET_DELAY)) &&
		    bValidTimestamp && pCbSettings->stMask.bDriftDelay )
		{
			pCbMask->bDriftDelay = BVDC_P_DIRTY;
		}
		else
		{
			/* Wait for valid time stamp */
			pCbMask->bDriftDelay = BVDC_P_CLEAN;
		}

		/* Check if game mode delay is out of range.
		 */
		if(((ulGameModeDelay >
				(pCbSettings->ulGameModeReadWritePhaseDiff + pCbSettings->ulGameModeTolerance)) ||
		      (ulGameModeDelay <
				 (pCbSettings->ulGameModeReadWritePhaseDiff - pCbSettings->ulGameModeTolerance))) &&
			 bValidTimestamp && pCbSettings->stMask.bGameModeDelay)
		{
				pCbMask->bGameModeDelay = BVDC_P_DIRTY;
		}
		else
		{
			pCbMask->bGameModeDelay = BVDC_P_CLEAN;
		}

		if( BVDC_P_CB_IS_DIRTY(pCbMask) )
		{
			/* Update Callback data */
			if(pCbMask->bVsyncDelay)
			{
				pCbData->ulVsyncDelay  = uiNewVsyncDelay;
				pCbData->ulVsyncRate    = ulVsyncRate;
			}
			if(pCbMask->bDriftDelay)
			{
				pCbData->ulDriftDelay  = ulCurrDriftDelay;
			}
			if(pCbMask->bGameModeDelay)
			{
				/* Note:
				     game mode delay here is purely multi-buffer delay;
				 */
				pCbData->ulGameModeDelay = ulGameModeDelay;
			}

			if( pCbMask->bVsyncDelay )
			{
				BDBG_MSG(("Window[%d] callback bVsyncDelay: ", hWindow->eId));
				BDBG_MSG(("\tulVsyncDelay = %d", pCbData->ulVsyncDelay));
			}
			if( pCbMask->bDriftDelay )
			{
				BDBG_MSG(("Window[%d] callback bDriftDelay: ", hWindow->eId));
				BDBG_MSG(("\tW = %d, R = %d", ulCaptureTimestamp, ulPlaybackTimestamp));
				BDBG_MSG(("\tulDriftDelay = %d (%d fields)", pCbData->ulDriftDelay,
					pCbData->ulDriftDelay / ulUsecPerVsync));
			}
			if( pCbMask->bGameModeDelay )
			{
				BDBG_MSG(("Window[%d] callback bGameModeDelay: ", hWindow->eId));
				BDBG_MSG(("\tW = %d, R = %d", ulCaptureTimestamp, ulPlaybackTimestamp));
				BDBG_MSG(("\tulGameModeDelay = %d (%d fields)", pCbData->ulGameModeDelay,
					pCbData->ulGameModeDelay / ulUsecPerVsync));

				BDBG_MSG(("Reader B%d : Writer B%d", hWindow->hBuffer->pCurReaderBuf->ulBufferId,
					hWindow->hBuffer->pCurWriterBuf->ulBufferId));
			}
			if( pCbMask->bRectAdjust )
			{
				BDBG_MSG(("Window[%d] callback bRectAdjust: ", hWindow->eId));

				/* Match CMP_0_V0_SURFACE_OFFSET.Y_OFFSET and
				* CMP_0_V0_SURFACE_SIZE.VSIZE settings in
				* BVDC_P_Window_SetSurfaceSize_isr  */
				if(eFieldId != BAVC_Polarity_eFrame)
				{
					pCbData->stOutputRect.lTop = BVDC_P_ALIGN_DN(
						pCbData->stOutputRect.lTop, 2) ;
					pCbData->stOutputRect.ulHeight = BVDC_P_ALIGN_DN(
						pCbData->stOutputRect.ulHeight, 2) ;
				}

				BDBG_MSG(("\t %d %d %d %d", pCbData->stOutputRect.lLeft,
					pCbData->stOutputRect.lTop, pCbData->stOutputRect.ulWidth,
					pCbData->stOutputRect.ulHeight));
			}

			if( pCbMask->bSyncLock )
			{
				BDBG_MSG(("Window[%d] callback bSyncLock: ", hWindow->eId));

				pCbData->bSyncLock = hWindow->bSyncLockSrc;
				BDBG_MSG(("\t %d", pCbData->bSyncLock));
			}

			/* Callback application with the above data */
			pCurInfo->pfGenCallback(pCurInfo->pvGenCallbackParm1,
				pCurInfo->iGenCallbackParm2, (void*)pCbData);

			/* clear dirty bits */
			BVDC_P_CB_CLEAN_ALL_DIRTY(pCbMask);
		}
	}

	return;
}

/* set SrcMuxValue for each module, from front to back
 * note: a sub-module might be shared by more than one window, but the
 * sharing is handled by each sub-module. And each sub-module will
 * acquire and release the pre-patch of free-channel or loop-back
 * internally basing on eVnetPatchMode.
 */
static BERR_Code BVDC_P_Window_SetWriterVnet_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_VnetMode                 *pVnetMode )
{
	BVDC_P_VnetPatch  eVnetPatchMode;
	uint32_t  ulSrcMuxValue;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor->hVdc, BVDC_VDC);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	hWindow->hCompositor->hVdc->bForcePrint
		? BDBG_ERR(("Set writer vnet, win %d, vnetMode 0x%x ", hWindow->eId, *pVnetMode))
		: BDBG_MSG(("Set writer vnet, win %d, vnetMode 0x%x ", hWindow->eId, *pVnetMode));

	eVnetPatchMode = BVDC_P_VnetPatch_eNone;
	ulSrcMuxValue = BVDC_P_Source_PostMuxValue(hWindow->stCurInfo.hSource);

#if (BVDC_P_SUPPORT_HIST)
	if(BVDC_P_VNET_USED_HIST_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Hist_SetVnet_isr(hWindow->stCurResource.hHist, ulSrcMuxValue, BVDC_P_VnetPatch_eFreeCh);
	}
#endif

#if (BVDC_P_SUPPORT_DNR)
	/* DNR is always upstream of ANR */
	if(BVDC_P_VNET_USED_DNR_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Dnr_SetVnet_isr(hWindow->stCurResource.hDnr, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Dnr_PostMuxValue(hWindow->stCurResource.hDnr); /* put hDnr in win ? */
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Hscaler_SetVnet_isr(hWindow->stCurResource.hHscaler, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Hscaler_PostMuxValue(hWindow->stCurResource.hHscaler);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_VNET_USED_ANR_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Anr_SetVnetAllocBuf_isr(hWindow->stCurResource.hAnr, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eAnrHeapId, hWindow->usAnrPixelBufferCnt,
			BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE && pVnetMode->stBits.bUseMad);
		ulSrcMuxValue = BVDC_P_Anr_PostMuxValue(hWindow->stCurResource.hAnr);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Mad_SetVnetAllocBuf_isr(hWindow->stCurResource.hMad32, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eMadPixelHeapId, hWindow->eMadQmHeapId, hWindow->usMadPixelBufferCnt);
		ulSrcMuxValue = BVDC_P_Mad_PostMuxValue(hWindow->stCurResource.hMad32);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_MCVP)
	/* note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
	 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
	if(BVDC_P_VNET_USED_MCVP_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Mcvp_SetVnetAllocBuf_isr(hWindow->stCurResource.hMcvp, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eMadPixelHeapId, hWindow->eMadQmHeapId, hWindow->usMadPixelBufferCnt, &hWindow->stMcvpMode, true);
		ulSrcMuxValue = BVDC_P_Mcvp_PostMuxValue(hWindow->stCurResource.hMcvp);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Scaler_SetVnet_isr(hWindow->stCurResource.hScaler, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Scaler_PostMuxValue(hWindow->stCurResource.hScaler);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_WRITER(*pVnetMode))
	{
		BVDC_P_Fgt_SetVnet_isr(hWindow->stCurResource.hFgt, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Fgt_PostMuxValue(hWindow->stCurResource.hFgt);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

	if(BVDC_P_VNET_USED_CAPTURE(*pVnetMode))
	{
		eVnetPatchMode = (BVDC_P_VnetPatch_eLpBack == eVnetPatchMode) ?
			BVDC_P_VnetPatch_eNone : BVDC_P_VnetPatch_eFreeCh;
		/* cap bufs will be allocated in writer_isr together with handling of
		 * buf adding/dropping requested by user */
		BVDC_P_Capture_SetVnet_isr(hWindow->stCurResource.hCapture, ulSrcMuxValue, eVnetPatchMode);
	}

	return BERR_SUCCESS;
}


/* set SrcMuxValue for each module, from front to back
 * note: a sub-module might be shared by more than one window, but the
 * sharing is handled by each sub-module. And each sub-module will
 * acquire and release the pre-patch of free-channel or loop-back
 * internally basing on eVnetPatchMode.
 */
static BERR_Code BVDC_P_Window_SetReaderVnet_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_VnetMode                 *pVnetMode )
{
	BVDC_P_VnetPatch  eVnetPatchMode;
	uint32_t  ulSrcMuxValue;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_MSG(("Set reader vnet, win %d, vnetMode 0x%x ", hWindow->eId, *pVnetMode));

	eVnetPatchMode = BVDC_P_VnetPatch_eNone;
	if(BVDC_P_VNET_USED_CAPTURE(*pVnetMode))
	{
		ulSrcMuxValue = BVDC_P_Feeder_PostMuxValue(hWindow->stCurResource.hPlayback);
	}
	else
	{
		ulSrcMuxValue = BVDC_P_Source_PostMuxValue(hWindow->stCurInfo.hSource);
	}

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_VNET_USED_DNR_AT_READER(*pVnetMode))
	{
		BVDC_P_Dnr_SetVnet_isr(hWindow->stCurResource.hDnr, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Dnr_PostMuxValue(hWindow->stCurResource.hDnr); /* put hDnr in win ? */
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_READER(*pVnetMode))
	{
		BVDC_P_Hscaler_SetVnet_isr(hWindow->stCurResource.hHscaler, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Hscaler_PostMuxValue(hWindow->stCurResource.hHscaler);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_VNET_USED_ANR_AT_READER(*pVnetMode))
	{
		BVDC_P_Anr_SetVnetAllocBuf_isr(hWindow->stCurResource.hAnr, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eAnrHeapId, hWindow->usAnrPixelBufferCnt,
			BVDC_P_SUPPORT_ANR_MAD_MEM_SAVING_MODE && pVnetMode->stBits.bUseMad);
		ulSrcMuxValue = BVDC_P_Anr_PostMuxValue(hWindow->stCurResource.hAnr);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_VNET_USED_MAD_AT_READER(*pVnetMode))
	{
		BVDC_P_Mad_SetVnetAllocBuf_isr(hWindow->stCurResource.hMad32, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eMadPixelHeapId, hWindow->eMadQmHeapId, hWindow->usMadPixelBufferCnt);
		ulSrcMuxValue = BVDC_P_Mad_PostMuxValue(hWindow->stCurResource.hMad32);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

#if (BVDC_P_SUPPORT_MCVP)
	/* note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
	 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
	if(BVDC_P_VNET_USED_MCVP_AT_READER(*pVnetMode))
	{
		BVDC_P_Mcvp_SetVnetAllocBuf_isr(hWindow->stCurResource.hMcvp, ulSrcMuxValue, eVnetPatchMode,
			hWindow->eMadPixelHeapId, hWindow->eMadQmHeapId, hWindow->usMadPixelBufferCnt, &hWindow->stMcvpMode, true);
		ulSrcMuxValue = BVDC_P_Mcvp_PostMuxValue(hWindow->stCurResource.hMcvp);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_READER(*pVnetMode))
	{
		BVDC_P_Scaler_SetVnet_isr(hWindow->stCurResource.hScaler, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Scaler_PostMuxValue(hWindow->stCurResource.hScaler);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_READER(*pVnetMode))
	{
		BVDC_P_Fgt_SetVnet_isr(hWindow->stCurResource.hFgt, ulSrcMuxValue, eVnetPatchMode);
		ulSrcMuxValue = BVDC_P_Fgt_PostMuxValue(hWindow->stCurResource.hFgt);
		eVnetPatchMode = BVDC_P_VnetPatch_eLpBack;
	}
#endif

	/* set vnet for compositor's win src */
	eVnetPatchMode = (BVDC_P_VnetPatch_eLpBack == eVnetPatchMode) ?
		BVDC_P_VnetPatch_eNone : BVDC_P_VnetPatch_eFreeCh;
	BVDC_P_SubRul_SetVnet_isr(&(hWindow->stWinOutVnet),
		ulSrcMuxValue, eVnetPatchMode);

#if (BVDC_P_SUPPORT_HIST)
	if(BVDC_P_VNET_USED_HIST_AT_READER(*pVnetMode))
	{
		/*eVnetPatchMode is decided above with WinOut */
		BVDC_P_Hist_SetVnet_isr(hWindow->stCurResource.hHist, ulSrcMuxValue, eVnetPatchMode);
	}
#endif

	return BERR_SUCCESS;
}

/* NOTE: This function will release software resources, not hw vnet disconnection!
 * The hw vnet is disconnected by SHUTDOWNRUL state!
 * release the free-channel or loop-back that might have been used to patch
 * the vnet. all bufs used by mad, anr and cap are released togerther inside
 * this function to avoid fragmentation.  It must be called after reader's
 * vnet shut-down rul and writer's drain rul are executed
 */
static void BVDC_P_Window_UnsetWriterVnet_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_VnetMode                 *pVnetMode )
{
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_MSG(("Unset writer vnet, win %d, vnetMode 0x%x ", hWindow->eId, *pVnetMode));

	if(BVDC_P_VNET_IS_INVALID(*pVnetMode))
		return;

#if (BVDC_P_SUPPORT_HIST)
	if(BVDC_P_VNET_USED_HIST_AT_WRITER(*pVnetMode))
		BVDC_P_Hist_UnsetVnet_isr(hWindow->stCurResource.hHist);
#endif

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_VNET_USED_DNR_AT_WRITER(*pVnetMode))
		BVDC_P_Dnr_UnsetVnet_isr(hWindow->stCurResource.hDnr);
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_WRITER(*pVnetMode))
		BVDC_P_Hscaler_UnsetVnet_isr(hWindow->stCurResource.hHscaler);
#endif

#if (BVDC_P_SUPPORT_ANR)
	/* this will also release anr bufs */
	if(BVDC_P_VNET_USED_ANR_AT_WRITER(*pVnetMode))
		BVDC_P_Anr_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hAnr);
#endif

#if (BVDC_P_SUPPORT_MAD)
	/* this will also release mad bufs */
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(*pVnetMode))
		BVDC_P_Mad_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hMad32);
#endif

#if (BVDC_P_SUPPORT_MCVP)
	/* this will also release anr/mad bufs.
	 * note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
	 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
	if(BVDC_P_VNET_USED_MCVP_AT_WRITER(*pVnetMode))
		BVDC_P_Mcvp_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hMcvp);
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_WRITER(*pVnetMode))
		BVDC_P_Scaler_UnsetVnet_isr(hWindow->stCurResource.hScaler);

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_WRITER(*pVnetMode))
		BVDC_P_Fgt_UnsetVnet_isr(hWindow->stCurResource.hFgt);
#endif

	if(BVDC_P_VNET_USED_CAPTURE(*pVnetMode))
	{
		BVDC_P_HeapNodePtr apHeapNode[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#if (BVDC_P_SUPPORT_3D_VIDEO)
		BVDC_P_HeapNodePtr apHeapNode_R[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#endif
		uint32_t  ulCount;

		/* Release cap buffer after synch with reader shutdown */
		ulCount = hWindow->hBuffer->ulActiveBufCnt;
		BVDC_P_BUF_MSG(("Win[%d] shutdown -> free %d cap buffers (%s)", hWindow->eId, ulCount,
			BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(((hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate) &&
		     (BVDC_P_CLEAN == hWindow->stCurInfo.stDirty.stBits.bBufAllocMode)) ||
		   ((hWindow->ePrevBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)  &&
		     (BVDC_P_DIRTY == hWindow->stCurInfo.stDirty.stBits.bBufAllocMode)))
		{
			BVDC_P_BUF_MSG(("Win[%d] shutdown -> free %d cap buffers (%s) for right", hWindow->eId, ulCount,
				BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));

			BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
				apHeapNode, apHeapNode_R, ulCount, hWindow->ulBufDelay);
			BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap,
				apHeapNode_R, ulCount, false);

			hWindow->stCurInfo.stDirty.stBits.bBufAllocMode = BVDC_P_CLEAN;
		}
		else
#endif
		{
			BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
				apHeapNode, NULL, ulCount, hWindow->ulBufDelay);
		}
		BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap,
			apHeapNode, ulCount, false);

		/* User capture buffer is explicitly released by user so that we
		 * won't interrupt its usage.
		 */
		hWindow->ulBufCntAllocated -= ulCount;

		/* release capture vnet resource */
		BVDC_P_Capture_UnsetVnet_isr(hWindow->stCurResource.hCapture);
	}

	return;
}

/* NOTE: This function will release software resources, not hw vnet disconnection!
 * The hw vnet is disconnected by SHUTDOWNRUL state!
 * release the free-channel or loop-back that might have been used to patch
 * the vnet. It is called after vnet shut-down and drain rul is executed
 */
static void BVDC_P_Window_UnsetReaderVnet_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_VnetMode                 *pVnetMode )
{
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_MSG(("Unset reader vnet, win %d, vnetMode 0x%x ", hWindow->eId, *pVnetMode));

	if(BVDC_P_VNET_IS_INVALID(*pVnetMode))
		return;

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_VNET_USED_DNR_AT_READER(*pVnetMode))
		BVDC_P_Dnr_UnsetVnet_isr(hWindow->stCurResource.hDnr);
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_READER(*pVnetMode))
		BVDC_P_Hscaler_UnsetVnet_isr(hWindow->stCurResource.hHscaler);
#endif

#if (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_VNET_USED_ANR_AT_READER(*pVnetMode))
		BVDC_P_Anr_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hAnr);
#endif

#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_VNET_USED_MAD_AT_READER(*pVnetMode))
		BVDC_P_Mad_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hMad32);
#endif

#if (BVDC_P_SUPPORT_MCVP)
	/* note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
	 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
	if(BVDC_P_VNET_USED_MCVP_AT_READER(*pVnetMode))
		BVDC_P_Mcvp_UnsetVnetFreeBuf_isr(hWindow->stCurResource.hMcvp);
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_READER(*pVnetMode))
		BVDC_P_Scaler_UnsetVnet_isr(hWindow->stCurResource.hScaler);

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_READER(*pVnetMode))
		BVDC_P_Fgt_UnsetVnet_isr(hWindow->stCurResource.hFgt);
#endif

#if (BVDC_P_SUPPORT_HIST)
	if(BVDC_P_VNET_USED_HIST_AT_READER(*pVnetMode))
		BVDC_P_Hist_UnsetVnet_isr(hWindow->stCurResource.hHist);
#endif

	/* unset vnet for compositor's win src */
	BVDC_P_SubRul_UnsetVnet_isr(&(hWindow->stWinOutVnet));

	return;
}

/***************************************************************************
 * {private}
 *
 * writer vnet shut down process.
 */
void BVDC_P_Window_WriterShutDown_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eFieldId,
	  BVDC_P_ListInfo                 *pList )
{
	bool bLastExecuted;
	BVDC_P_Window_Info *pCurInfo;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	/* Get dirty bits to check if needed to rebuild csc rul. */
	pCurInfo  = &hWindow->stCurInfo;

	/* to prevent back-to-back callbacks prematurely exiting writer
	 * shutdown without execution;
	 * Note: progressive format uses the same slot for the next RUL
	 * as the current one; */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId) &&
	   (BAVC_Polarity_eFrame != eFieldId))
	{
		BRDC_Slot_Handle hCurSlot;
		BRDC_List_Handle hCurList;
		BAVC_Polarity eNextFieldId = BVDC_P_NEXT_POLARITY(eFieldId);

		hCurSlot = BVDC_P_SRC_GET_SLOT(hWindow->stCurInfo.hSource, eNextFieldId);
		hCurList = BVDC_P_SRC_GET_LIST(hWindow->stCurInfo.hSource, eNextFieldId);
		BRDC_Slot_UpdateLastRulStatus_isr(hCurSlot, hCurList, true);
		bLastExecuted = BRDC_List_GetLastExecStatus_isr(hCurList);
		BDBG_MSG(("Mpeg window (W) current slot bExe = %d for field %d",
				  bLastExecuted, eNextFieldId));
	}
	else
	{
		bLastExecuted = pList->bLastExecuted;
	}

	/* Shutdown writer as soon as possible, then synchronize on reader
 	 * shutdown process before releasing cap bufs */
	if(((false == bLastExecuted) &&
		(BVDC_P_State_eShutDownRul == pCurInfo->eWriterState)) ||
	   (BVDC_P_State_eShutDownPending == pCurInfo->eWriterState))
	{
		if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
		{
			/* in case another win is sharing this MFD, we don't want to
			 * start MFD before this win drops off from it */
			if((!BVDC_P_VNET_IS_INVALID(hWindow->stVnetMode))&&
				(pCurInfo->eReaderState==BVDC_P_State_eActive))
				BVDC_P_feeder_SetRulBuildWinId_isr(hWindow->stCurInfo.hSource->hMpegFeeder, hWindow->eId);
		}

		pCurInfo->eWriterState = BVDC_P_State_eShutDownRul;
		BDBG_MSG(("(3.a) Window[%d] Slip(W%d) is BVDC_P_State_eShutDownRul, bExe = %d",
				  hWindow->eId, eFieldId, bLastExecuted));
	}
 	else if(((false == bLastExecuted) &&
			 (BVDC_P_State_eDrainVnet == pCurInfo->eWriterState)) ||
			(BVDC_P_State_eShutDownRul == pCurInfo->eWriterState))
	{
		pCurInfo->eWriterState = BVDC_P_State_eDrainVnet;
		BDBG_MSG(("(3.b) Window[%d] Slip(W%d) is BVDC_P_State_eDrainVnet, bExe = %d",
				  hWindow->eId, eFieldId, bLastExecuted));
	}
	else /* synchronize with reader shutdown here */
	if((BVDC_P_State_eDrainVnet == pCurInfo->eWriterState) &&
	   ((BVDC_P_State_eShutDown == pCurInfo->eReaderState) ||
		(hWindow->bSyncLockSrc)))
	{
		/* clear off writer side vnet sw infor and release resources including loop-back,
		 * free-channel, mad/anr/cap bufs; It must be called after reader's vnet shut-down
		 * rul and writer's drain rul are executed.
		 * note: it is state BVDC_P_State_eShutDownRul that causes each module to build
		 * into RUL to disable HW and to drop off from vnet */
		BVDC_P_Window_UnsetWriterVnet_isr(hWindow, &hWindow->stVnetMode);

		pCurInfo->eWriterState = BVDC_P_State_eShutDown;
		BDBG_MSG(("(3.c) Window[%d] Slip(W%d) is BVDC_P_State_eShutDown, bExe = %d",
				  hWindow->eId, eFieldId, bLastExecuted));
	}
	else if(BVDC_P_State_eShutDown == pCurInfo->eWriterState)
	{
		/* similar protection at writer side; */
		BDBG_MSG(("(*) Window[%d] (W%d) stays off", hWindow->eId, eFieldId));
	}
}


/***************************************************************************
 * {private}
 *
 * This function save histogram data to picture node and compensate for mad
 * delay as well.
 */
static void BVDC_P_Window_UpdateHistInfo_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture )
{
	BDBG_ENTER(BVDC_P_Window_UpdateHistInfo_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pPicture->stCurHistData = hWindow->stCurResource.hHist->stHistData;
	pPicture->ulCurHistSize = hWindow->stCurResource.hHist->ulHistSize;

	BDBG_LEAVE(BVDC_P_Window_UpdateHistInfo_isr);
	return;
}

static void BVDC_P_Window_DumpBufHeapConfigure_isr
	( BVDC_Window_Handle               hWindow,
	  bool                             bLeftBuffer )
{
#if (BDBG_DEBUG_BUILD)
	BDBG_ERR(("Win[%d] Not enough memory for %s! Configuration:",
		hWindow->eId, bLeftBuffer ? "CAP" : "CAP_R"));
	BDBG_ERR(("Win[%d] stVnetMode: 0x%lx, ulBufCntNeeded: %d, ulBufCntAllocated: %d",
		hWindow->eId, hWindow->stVnetMode, hWindow->ulBufCntNeeded,
		hWindow->ulBufCntAllocated));
	BDBG_ERR(("Win[%d] Src: %s, Disp: %s", hWindow->eId,
		hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo->pchFormatStr,
		hWindow->hCompositor->stCurInfo.pFmtInfo->pchFormatStr));
	BDBG_ERR(("Win[%d] uiVsyncDelayOffset: %d, uiCaptureBufCnt: %d", hWindow->eId,
		hWindow->stCurInfo.uiVsyncDelayOffset,
		hWindow->stCurInfo.uiCaptureBufCnt));
	BDBG_ERR(("Win[%d] bDoPulldown: %d, bFrameCapture: %d, bSyncLockSrc: %d",
		hWindow->eId, hWindow->bDoPulldown,
		hWindow->bFrameCapture, hWindow->bSyncLockSrc));
	BDBG_ERR(("Win[%d] SrcVertRate: %d, DstVertRate: %d", hWindow->eId,
		hWindow->stCurInfo.hSource->ulVertFreq,
		hWindow->hCompositor->stCurInfo.pFmtInfo->ulVertFreq));
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(bLeftBuffer);
#endif

	return;
}

/***************************************************************************
 * {private}
 * This function does the following as needed
 *    (1) change the reader state according to writer state.
 *    (2) set flush data flag for mute color buffer
 */
static void BVDC_P_Window_AdjustRdState_isr
	( BVDC_Window_Handle               hWindow,
	  bool*                            pbFlushPicQueue)
{
	BVDC_P_PictureNode *pPicture;

	/* both writer vnet and reader vnet are configured in writer_isr right after new
	 * vnetMode is decided; writerState changes to Active there too. readerState neeed
	 * to wait for the muted capture buffers are flushed. should not move this to
	 * reader_isr in case that reader_isr is called before writer_isr as syncLock */
	if(BVDC_P_State_eInactive == hWindow->stCurInfo.eReaderState)
	{
		if (BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
		{
			pPicture = &hWindow->stCurInfo.hSource->hVfdFeeder->stPicture;
		}
		else
		{
			pPicture = BVDC_P_Buffer_GetCurrReaderNode_isr(hWindow->hBuffer);
		}

		if(BVDC_P_State_eActive == hWindow->stCurInfo.eWriterState)
		{
			if(false == pPicture->stFlags.bMute)
			{
				/* this will make VDC modules in reader to build vnet and enabling
				 * into RUL. */
				hWindow->stCurInfo.eReaderState = BVDC_P_State_eActive;
				BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] ReaderState --> eActive", hWindow->eId));
			}
			else
			{
				/* flush mute color buffer */
				if((NULL !=pbFlushPicQueue) && (BVDC_P_VNET_USED_MCVP(hWindow->stMcvpMode)))
					*pbFlushPicQueue = true;
			}
		}
#if (BVDC_P_SHOW_VNET_MSG==1)
		else if((false == pPicture->stFlags.bMute) &&
				 (BVDC_P_State_eActive != hWindow->stCurInfo.eWriterState) &&
				 (false == hWindow->stCurInfo.stDirty.stBits.bSrcPending))
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("Win[%d] reader see pic un-muted before writeState to eActive", hWindow->eId));
		}
#endif
	}

}

/***************************************************************************
 * {private}
 *
 * This WRITER function does the following in order:
 *   (1) handle source pending (new vnet-start pause and resume)
 *   (2) put user returned captured buffer back to hBuffer
 *   (3) start new vnet, if we just created the window, or we just completed
 *       the shuting-downn of the old vnet; otherwise, check if we need to
 *       reconfigure vnet, and start shutdown process if we need to.
 *   (4) add/release, and flush cap-usr buf
 *   (5) setup picture node
 *   (6) Set Info (register softeare shadow) for each sub-modules in
 *       write vnet
 *
 * To be called with source _isr (every vsync).
 */
void BVDC_P_Window_Writer_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_MVD_Field            *pMvdFieldData,
	  const BAVC_VDC_HdDvi_Picture    *pXvdFieldData,
	  const BAVC_Polarity              eFieldId,
	  BVDC_P_ListInfo                 *pList,
	  uint32_t                         ulPictureIdx )
{
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;
	BVDC_P_PictureNode   *pPicture, *pCurPicture;
	const BVDC_P_Source_DirtyBits *pSrcDirty;
	const BVDC_P_Source_Info *pSrcInfo;
	bool bVideoDetect = false;
	bool bFlushPicQueue = false;
	BERR_Code  err = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Window_Writer_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);


	/* Get dirty bits to check if needed to rebuild csc rul. */
	pSrcInfo  = &hWindow->stCurInfo.hSource->stCurInfo;
	pCurInfo  = &hWindow->stCurInfo;
	pCurDirty = &hWindow->stCurInfo.stDirty;
	pSrcDirty = &hWindow->stCurInfo.hSource->stCurInfo.stDirty;

	/* Only video window has writer (gfx and others does not). */
	BDBG_ASSERT(BVDC_P_WIN_IS_VIDEO_WINDOW(hWindow->eId));

	/* --------------------------------------------------------------- */
	/* (1) handle source pending (new vnet-start pause and resume      */
	/* --------------------------------------------------------------- */
	if(BVDC_P_IS_DIRTY(pSrcDirty))
	{
		if((pSrcInfo->eResumeMode) &&
		   (pSrcDirty->stBits.bInputFormat))
		{
			BDBG_MSG(("Source[%d] is pending mode!", hWindow->stCurInfo.hSource->eId));
			BVDC_P_Window_SetReconfiguring_isr(hWindow, true, false);
		}

		if((pSrcDirty->stBits.bResume) &&
		   (pCurDirty->stBits.bSrcPending))
		{
			pCurDirty->stBits.bSrcPending = BVDC_P_CLEAN;
			BDBG_MSG(("Source[%d] is now resumed!", hWindow->stCurInfo.hSource->eId));
			BVDC_P_Window_SetReconfiguring_isr(hWindow, false, true);
		}

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(pSrcDirty->stBits.bOrientation &&
		   hWindow->stCurInfo.hSource->stCurInfo.bOrientationOverride)
		{
			BVDC_P_BufHeapAllocMode  eBufAllocMode;

			if(hWindow->stCurInfo.hSource->stCurInfo.eOrientation != BFMT_Orientation_e2D)
			{
				eBufAllocMode = BVDC_P_BufHeapAllocMode_eLRSeparate;
			}
			else
			{
				eBufAllocMode = BVDC_P_BufHeapAllocMode_eLeftOnly;
			}
			hWindow->ePrevBufAllocMode = hWindow->eBufAllocMode;
			hWindow->eBufAllocMode = eBufAllocMode;
			pCurDirty->stBits.bBufAllocMode = BVDC_P_DIRTY;
			pCurDirty->stBits.bReDetVnet = BVDC_P_DIRTY;
			pCurDirty->stBits.bRecAdjust = BVDC_P_DIRTY;
		}
#endif
	}

	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		BVDC_P_HeapNodePtr apHeapNode[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#if (BVDC_P_SUPPORT_3D_VIDEO)
		BVDC_P_HeapNodePtr apHeapNode_R[BVDC_P_MAX_MULTI_BUFFER_COUNT];
#endif

		/* NOTE: Most of these does not require additional actions, but they do
		 * needed to be cleaned.  Otherwise hWindow->hAppliedDoneEvent won't
		 * be set and resulted in timeout.  Be aware that these dirty are still
		 * require as it is needed to trigger the copy and apply.  Note it can
		 * be clear all at once with BVDC_P_CLEAN_ALL_DIRTY() since some dirty
		 * bit require more than 1 vsync to handle. */
		pCurDirty->stBits.bMiscCtrl    = BVDC_P_CLEAN;
		pCurDirty->stBits.bCtIndex     = BVDC_P_CLEAN;
		pCurDirty->stBits.bAnrAdjust   = BVDC_P_CLEAN;
		pCurDirty->stBits.bFgt         = BVDC_P_CLEAN;
		pCurDirty->stBits.bStg         = BVDC_P_CLEAN;
		pCurDirty->stBits.b3D          = BVDC_P_CLEAN;
#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_CAP)
		pCurDirty->stBits.bTestFeature1 = BVDC_P_CLEAN;
#endif

		/* defer MAD changes until MAD is configured */
		if(pCurDirty->stBits.bDeinterlace)
		{
			if(hWindow->stCurResource.hMad32) /* if have MAD */
			{
				hWindow->stCurResource.hMad32->bUsrChanges = true;
			}
			pCurDirty->stBits.bDeinterlace = BVDC_P_CLEAN;
		}

		/* --------------------------------------------------------------- */
		/* (2) Check if user returned a previously captured buffer       */
		/* --------------------------------------------------------------- */
		if(pCurDirty->stBits.bUserReleaseBuffer)
		{
			/* TODO: need to check heapId, might need to free the buf if
			 * heapId does not match */
			if((pCurInfo->pBufferFromUser) &&
			   (pCurInfo->pBufferFromUser->hSurface) &&
			   (pCurInfo->pBufferFromUser->stFlags.bUsedByUser))
			{
				BDBG_MSG(("Returning user captured picture."));
				BVDC_P_Buffer_ReturnBuffer_isr(hWindow->hBuffer, pCurInfo->pBufferFromUser);
				hWindow->uiAvailCaptureBuffers++;
			}
			/* clean dirty bit */
			pCurDirty->stBits.bUserReleaseBuffer = BVDC_P_CLEAN;
		}

		BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] writer_isr: Dirty 0x%08lx, wrtState %d, rdState %d",
			hWindow->eId, *pCurDirty, pCurInfo->eWriterState, pCurInfo->eReaderState));
		BDBG_MODULE_MSG(BVDC_WIN_VNET,("   rCfgVnet %d, SrcPending %d, ShtDn %d, Dstry %d, rDetVnet %d",
			pCurDirty->stBits.bReConfigVnet, pCurDirty->stBits.bSrcPending, pCurDirty->stBits.bShutdown,
			pCurDirty->stBits.bDestroy, pCurDirty->stBits.bReDetVnet));

		/* --------------------------------------------------------------- */
		/* (3) start new vnet, if we just created the window, or we just   */
		/* completed the shuting-downn of the old vnet;                    */
		/*     otherwise, check if we need to reconfigure vnet, and start  */
		/* shutdown process if we need to.                                 */
		/* --------------------------------------------------------------- */
		if((pCurDirty->stBits.bReConfigVnet) &&
		   (BVDC_P_CLEAN == pCurDirty->stBits.bShutdown))    /* start new vnet at this vsync */
		{
			/* we should start the new vnet even if original mpeg src is muted, so that we
			 * could show a fixed color win for old chips that does not support this in cmp
			 * HW. however, if bSrcPending is on, we will clean bReConfigVnet here to pause
			 * the new vnet building, and wait for the user's instruction to resume. */
			if(pCurDirty->stBits.bSrcPending)
			{
				BDBG_MSG(("Window[%d] defer reconfigure stVnetMode"));
				pCurDirty->stBits.bReConfigVnet = BVDC_P_CLEAN;
				hWindow->ulDropCntNonIgnoredPics += pMvdFieldData && !pMvdFieldData->bIgnorePicture;
				return;
			}
			else
			{
				bool bRfcgMcvp = false;
				/* start new vnet right now */
				BVDC_P_Window_DecideVnetMode_isr(hWindow, pMvdFieldData, true, &bRfcgMcvp);
				BVDC_P_Window_DecideBufsCfgs_isr(hWindow, pMvdFieldData, true);
				if(hWindow->stCurInfo.hSource->ulMosaicFirstUnmuteRectIndex == ulPictureIdx)
					BVDC_P_Window_SetCadenceHandling_isr(hWindow, pMvdFieldData, false, true);

				/* NOTE: configure vnet into software records!  The following
				 * functions will alloc mad/anr buf memory. */
				BVDC_P_Window_SetWriterVnet_isr(hWindow, &hWindow->stVnetMode);
				BVDC_P_Window_SetReaderVnet_isr(hWindow, &hWindow->stVnetMode);

				/* setting eWriterState as BVDC_P_State_eActive will cause BuildWriterRul
				 * to build vnet into RUL, and start to enable VDC modules in writer */
				hWindow->stCurInfo.eWriterState = BVDC_P_State_eActive;

				/* vnet configure in software completed, so clear dirty bits;
				 * and since we just calucated new vnetMode and buf configures,
				 * so no need to check again until new dirty; also, we need to
				 * alloc fresh bufs for cap. */
				pCurDirty->stBits.bReConfigVnet   = BVDC_P_CLEAN;
				pCurDirty->stBits.bReDetVnet      = BVDC_P_CLEAN;
				pCurDirty->stBits.bReallocBuffers = BVDC_P_DIRTY;
				bFlushPicQueue = true;

				/* TODO: why it is here? */
				pCurDirty->stBits.bSharedScl    = BVDC_P_CLEAN;

				BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] cfg vnet 0x%x, writerState -> eActive, field %d",
					hWindow->eId, hWindow->stVnetMode, eFieldId));
			}
		}
		else /* not start new vnet at this vsync */
		{
			/* Check for the need of vnet reconfigure */
			if(pCurDirty->stBits.bReDetVnet &&
			   BVDC_P_Window_NotReconfiguring_isr(hWindow))
			{
				bool bRfcgVnet = false;
				bool bRfcgMcvp = false;

				/* check for vnetMode change */
				bRfcgVnet = BVDC_P_Window_DecideVnetMode_isr(hWindow, pMvdFieldData, false, &bRfcgMcvp);

				/* if vnetMode does not change, futher check for mad/anr/cap buf heapId changes */
				if(!bRfcgVnet)
				{
					/* buf heapId could change if (1) input/output size changes; (2) vnet
					 * changes; (3) capture pixel format changes or datamode. */
					bRfcgVnet = BVDC_P_Window_DecideBufsCfgs_isr(hWindow, pMvdFieldData, false);

				}
				if(hWindow->stCurInfo.hSource->ulMosaicFirstUnmuteRectIndex == ulPictureIdx)
				{
					BVDC_P_Window_SetCadenceHandling_isr(hWindow, pMvdFieldData, false, true);
				}

				if(bRfcgVnet)
				{
					/* this sets bShutDown to true unless vnet is already inactive */
					BVDC_P_Window_SetReconfiguring_isr(hWindow, false, true);
				}
			}

			/* we just checked if need to recfg vnet, or already in shut down process */
			pCurDirty->stBits.bReDetVnet = BVDC_P_CLEAN;

			/* note: besides above code, shutdown might be requested in several other
			 * places */
			if(pCurDirty->stBits.bShutdown)
			{
				/* Increase writer state if we are shuting down. */
				BVDC_P_Window_WriterShutDown_isr(hWindow, eFieldId, pList);
				hWindow->ulDropCntNonIgnoredPics += pMvdFieldData && !pMvdFieldData->bIgnorePicture;
				return;
			}
			else if(pCurDirty->stBits.bSrcPending)
			{
				/* due to src pending. similar protection at reader side; */
				BDBG_MSG(("($) Window[%d] (W%d) stays off", hWindow->eId, eFieldId));
				hWindow->ulDropCntNonIgnoredPics += pMvdFieldData && !pMvdFieldData->bIgnorePicture;
				return;
			}
		}

		/* --------------------------------------------------------------- */
		/* (4.1) allocate more bufs for capture if we don't have enough;    */
		/* free some bufs if we have more than needed.                      */
		/* note: if we start new vnet at this vsync, all capture bufs are   */
		/* allocated here (to avoid duplicated code), and mad/anr bufs are  */
		/* allocated in SetWriterVnet_isr/SetReaderVnet_isr called above in */
		/* this vsync. Therefore no fragmentation are generated             */
		/* ---------------------------------------------------------------- */
		if(pCurDirty->stBits.bBufAllocMode && !pCurDirty->stBits.bReConfigVnet &&
		   !pCurDirty->stBits.bSrcPending)
		{
#if (BVDC_P_SUPPORT_3D_VIDEO)
			if((hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate) &&
			   (hWindow->ePrevBufAllocMode == BVDC_P_BufHeapAllocMode_eLeftOnly))
			{
				BVDC_P_HeapNodePtr apHeapNode_R[BVDC_P_MAX_MULTI_BUFFER_COUNT];

				/* 3D: Allocate right buffer */
				BVDC_P_BUF_MSG(("Win[%d] alloc %d cap right buffers (req %s, prefer %s)",
					hWindow->eId, hWindow->ulBufCntAllocated,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest),
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdPrefer)));
				/* Allocate right buffers for 3D */
				err = BVDC_P_BufferHeap_AllocateBuffers_isr(hWindow->hHeap,
					apHeapNode_R, hWindow->ulBufCntAllocated, false, hWindow->eBufferHeapIdRequest,
					hWindow->eBufferHeapIdPrefer);
				/* Not enough memory, dump out configuration */
				if(err == BERR_OUT_OF_DEVICE_MEMORY)
				{
					BVDC_P_Window_DumpBufHeapConfigure_isr(hWindow, false);
				}

				BVDC_P_Buffer_SetRightBufferPictureNodes_isr(hWindow->hBuffer,
					apHeapNode_R, hWindow->ulBufCntAllocated, true);
			}
			else if((hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLeftOnly) &&
			   (hWindow->ePrevBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate))
			{
				/* 2D: free right buffer */
				BVDC_P_Buffer_SetRightBufferPictureNodes_isr(hWindow->hBuffer,
					apHeapNode_R, hWindow->ulBufCntAllocated, false);

				BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap,
					apHeapNode_R, hWindow->ulBufCntAllocated, false);
			}
#endif

			pCurDirty->stBits.bBufAllocMode = BVDC_P_CLEAN;
		}

		if(pCurDirty->stBits.bReallocBuffers &&
		   !pCurDirty->stBits.bReConfigVnet && !pCurDirty->stBits.bSrcPending)
		{
			uint32_t  ulCount;

			if(hWindow->ulBufCntNeeded > hWindow->ulBufCntAllocated)
			{
				ulCount = hWindow->ulBufCntNeeded - hWindow->ulBufCntAllocated;
				BVDC_P_BUF_MSG(("Win[%d] alloc %d cap buffers (req %s, prefer %s)", hWindow->eId, ulCount,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest),
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdPrefer)));

				err = BVDC_P_BufferHeap_AllocateBuffers_isr(hWindow->hHeap,
					apHeapNode, ulCount, false, hWindow->eBufferHeapIdRequest,
					hWindow->eBufferHeapIdPrefer);

				/* Not enough memory, dump out configuration */
				if(err == BERR_OUT_OF_DEVICE_MEMORY)
				{
					BVDC_P_Window_DumpBufHeapConfigure_isr(hWindow, true);
				}
				BDBG_ASSERT(BERR_SUCCESS == err);
#if (BVDC_P_SUPPORT_3D_VIDEO)
				if(hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)
				{
					BVDC_P_BUF_MSG(("Win[%d] alloc %d cap right buffers (req %s, prefer %s)",
						hWindow->eId, ulCount,
						BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest),
						BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdPrefer)));
					/* Allocate right buffers for 3D */
					err = BVDC_P_BufferHeap_AllocateBuffers_isr(hWindow->hHeap,
						apHeapNode_R, ulCount, false, hWindow->eBufferHeapIdRequest,
						hWindow->eBufferHeapIdPrefer);
					/* Not enough memory, dump out configuration */
					if(err == BERR_OUT_OF_DEVICE_MEMORY)
					{
						BVDC_P_Window_DumpBufHeapConfigure_isr(hWindow, false);
					}
					BDBG_ASSERT(BERR_SUCCESS == err);

					hWindow->stCurInfo.stDirty.stBits.bBufAllocMode = BVDC_P_CLEAN;

					BVDC_P_Buffer_AddPictureNodes_isr(hWindow->hBuffer, apHeapNode,
						apHeapNode_R, ulCount,
						hWindow->ulBufDelay, hWindow->bSyncLockSrc, bFlushPicQueue);
				}
				else
#endif
				{
					BVDC_P_Buffer_AddPictureNodes_isr(hWindow->hBuffer, apHeapNode,
						NULL, ulCount,
						hWindow->ulBufDelay, hWindow->bSyncLockSrc, bFlushPicQueue);
				}

			}
			/* the '=' in the following is for the case that forceCap is turnt off during src
			 * pending, so that ReleasePictureNodes_isr will sets ulSyncDelay. Otherwise
			 * readerNode will be fixed to buf0, writerNode fixed to buf1 => mute forever */
			else if(hWindow->ulBufCntNeeded <= hWindow->ulBufCntAllocated)
			{
				ulCount = hWindow->ulBufCntAllocated - hWindow->ulBufCntNeeded;
				BVDC_P_BUF_MSG(("Win[%d] free %d cap buffers (%s)", hWindow->eId, ulCount,
					BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));

#if (BVDC_P_SUPPORT_3D_VIDEO)
				if(((hWindow->eBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate) &&
				     (BVDC_P_CLEAN == hWindow->stCurInfo.stDirty.stBits.bBufAllocMode)) ||
				   ((hWindow->ePrevBufAllocMode == BVDC_P_BufHeapAllocMode_eLRSeparate)  &&
				     (BVDC_P_DIRTY == hWindow->stCurInfo.stDirty.stBits.bBufAllocMode)))
				{
					BVDC_P_BUF_MSG(("Win[%d] free %d cap buffers (%s) for right",
						hWindow->eId, ulCount,
						BVDC_P_BUFFERHEAP_GET_HEAP_ID_NAME(hWindow->eBufferHeapIdRequest)));
					BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
						apHeapNode, apHeapNode_R,
						BVDC_P_MIN(hWindow->hBuffer->ulActiveBufCnt, ulCount),
						hWindow->ulBufDelay);
					BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap,
						apHeapNode_R, ulCount, false);

					hWindow->stCurInfo.stDirty.stBits.bBufAllocMode = BVDC_P_CLEAN;
				}
				else
#endif
				{
				BVDC_P_Buffer_ReleasePictureNodes_isr(hWindow->hBuffer,
					apHeapNode, NULL,
					BVDC_P_MIN(hWindow->hBuffer->ulActiveBufCnt, ulCount),
					hWindow->ulBufDelay);
				}
				BVDC_P_BufferHeap_FreeBuffers_isr(hWindow->hHeap,
					apHeapNode, ulCount, false);
			}

			hWindow->ulBufCntAllocated = hWindow->ulBufCntNeeded;
			pCurDirty->stBits.bReallocBuffers = BVDC_P_CLEAN;
		}

		/* --------------------------------------------------------------- */
		/* (4.2) flush new allocated bufs                                  */
		/* --------------------------------------------------------------- */
		if(bFlushPicQueue)
		{
			BVDC_P_BUF_MSG(("win[%d] flushes cap buffers", hWindow->eId));
			BVDC_P_Buffer_Invalidate_isr(hWindow->hBuffer);
			hWindow->pCurWriterNode = BVDC_P_Buffer_GetCurWriterNode(hWindow->hBuffer);
			hWindow->pCurReaderNode = BVDC_P_Buffer_GetCurReaderNode(hWindow->hBuffer);
		}
	}

	/* --------------------------------------------------------------- */
	/* (5) setup picture node                                          */
	/* --------------------------------------------------------------- */
	pCurPicture = hWindow->hBuffer->pCurWriterBuf;

	/* Update VEC/VDEC phase for the current picture node
	 * NEED to set before calling BVDC_P_Buffer_GetNextWriterNode_isr */
	if(BVDC_P_SRC_IS_ANALOG(hWindow->stCurInfo.hSource->eId))
	{
		bool b240pMode = false;
		bool bVcrHeadSwitch = false;
		pPicture = BVDC_P_Buffer_GetPrevWriterNode_isr(hWindow->hBuffer);
		BVDC_P_Vdec_GetStatus_isr(hWindow->stCurInfo.hSource->hVdec, &pPicture->ulVdecPhase,
			&bVideoDetect, &bVcrHeadSwitch, &b240pMode, &pPicture->PicComRulInfo.bDrTf);
		pPicture->stFlags.b240pMode = b240pMode;
		pPicture->stFlags.bVcrHeadSwitch = bVcrHeadSwitch;

		if(pCurInfo->ePhaseAdjustMode)
		{
			pPicture->ulVdecPhase = BVDC_P_Vdec_LowPassFilter(hWindow,
			(int32_t)pPicture->ulVdecPhase);
		}
	}
	else if(BVDC_P_SRC_IS_ITU656(hWindow->stCurInfo.hSource->eId))
	{
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
		BVDC_P_656In_GetStatus_isr(hWindow->stCurInfo.hSource->h656In, &bVideoDetect);
#else
		uint32_t   ulVdecPhase = 0;

		pPicture = BVDC_P_Buffer_GetPrevWriterNode_isr(hWindow->hBuffer);
		BVDC_P_Vdec_GetStatus_isr(hWindow->stCurInfo.hSource->hVdec,
			&ulVdecPhase, &bVideoDetect, NULL, NULL, NULL);
		pPicture->ulVdecPhase = ulVdecPhase;
#endif
	}
	else if(BVDC_P_SRC_IS_HDDVI(hWindow->stCurInfo.hSource->eId))
	{
		BVDC_P_HdDvi_GetStatus_isr(hWindow->stCurInfo.hSource->hHdDvi, &bVideoDetect);
	}
	else
	{
		bVideoDetect = true;
	}

	if(BVDC_P_VNET_USED_HIST_AT_WRITER(hWindow->stVnetMode))
	{
		/* Sample histogram data and store to previous picture node */
		BVDC_P_Hist_UpdateHistData_isr(hWindow->stCurResource.hHist);
		BVDC_P_Window_UpdateHistInfo_isr(hWindow, hWindow->pCurWriterNode);
	}

	/* Get next buffer node */
	/* MosaicMode: don't advance multi-buffer node for the chained mosaic rects; */
	/* Done advance writer node when in repeat mute mode */
	if (BVDC_P_SRC_IS_VFD(pCurInfo->hSource->eId))
	{
		pPicture = &pCurInfo->hSource->hVfdFeeder->stPicture;
		bVideoDetect = true;
	}
	else if((0 == ulPictureIdx) && (pSrcInfo->eMuteMode != BVDC_MuteMode_eRepeat))
	{
		pPicture = BVDC_P_Buffer_GetNextWriterNode_isr(hWindow, eFieldId);
		hWindow->pCurWriterNode = pPicture;

#if BVDC_P_SUPPORT_MOSAIC_MODE
		/* MosaicMode: reset ClearRect mask set, which will be set by the following
		   mosaic pictures list UpdateSrcAndUserInfo_isr; */
		pPicture->ulMosaicRectSet = 0;
#endif
	}
	else
	{
		pPicture = hWindow->pCurWriterNode;
	}

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
	/* Set up MosaicMode colorspaces */
	if(hWindow->stCurInfo.bClearRect && (pMvdFieldData != NULL))
	{
		hWindow->aeMosaicClrSpaceList[ulPictureIdx] = pMvdFieldData->eMatrixCoefficients;
	}
#endif

	/* This is where we combine the source_info and user_info into
	 * node_info.  This node_info is then use to program the scaler and
	 * capture to create node, and the created node should have all the
	 * info to setup the playback blocks.
	 * <source_info> + <user_info> ==> <node_info> */

	BVDC_P_Window_UpdateSrcAndUserInfo_isr(hWindow, pPicture,
		pMvdFieldData, pXvdFieldData, eFieldId);

	/* PR57098: test routine for HDMI garbage issue.
	 * Note, it is the application/middle-ware's responsibility
	 * to make sure enough buffers are allocated for reader/writer
	 * gap. This can be done via BVDC_Window_SetDelayOffset(). */
	if((!bVideoDetect) && hWindow->stCurInfo.hSource->stCurInfo.stDirty.stBits.bVideoDetected)
	{
		BVDC_P_Buffer_Invalidate_isr(hWindow->hBuffer);
		hWindow->pCurWriterNode = BVDC_P_Buffer_GetCurWriterNode(hWindow->hBuffer);
		hWindow->pCurReaderNode = BVDC_P_Buffer_GetCurReaderNode(hWindow->hBuffer);
	}

	/* Mute the picture if video is not detected.  There are two type of mute:
	 * (1) bMute           - Which mean shutdown reader modules to mute!
	 * (2) bMuteFixedColor - Display fixed color, like in the case of ne video */
	pPicture->stFlags.bMute            = !hWindow->stCurInfo.hSource->bStartFeed;
	pPicture->stFlags.bMuteFixedColor |= !bVideoDetect;

	/* Tag display format as frame if it's progressive.  Removed the
	 * previous prediction of display polarity. */
	if(!hWindow->hCompositor->stCurInfo.pFmtInfo->bInterlaced)
	{
		pPicture->eDisplayPolarity = BAVC_Polarity_eFrame;
	}

	if(0 == ulPictureIdx)
	{
		BVDC_P_Window_AdjustCadenceHandling_isr(hWindow, pMvdFieldData);
		BVDC_P_Window_SetCapturePolarity_isr(hWindow, pMvdFieldData, pPicture);
	}
	/* Prevent this from flooding the screen.*/
	/* BDBG_MSG(("Win%d W(%d): B(%d) (%d -> %d)", hWindow->eId, eFieldId,
	   pPicture->ulBufferId, pPicture->eSrcPolarity, pPicture->eDstPolarity));
	*/

	/* --------------------------------------------------------------- */
	/* (6) Set Info (register softeare shadow) for each sub-modules in */
	/* write vnet                                                      */
	/* --------------------------------------------------------------- */

	/* NOTE: we should config MAD32 before SCL since the picture node's source
	 * polarity could be modified by mad setinfo_isr; */
	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		/* restore dropped non-ignore pictures while shutdown/reconfig in NRT mode */
		if((hWindow->ulDropCntNonIgnoredPics) &&
			(!BVDC_P_VNET_USED_MAD(hWindow->stMcvpMode)))
		{
			/* assume non-ignore pic always needs to advance STC */
			pPicture->bIgnorePicture = false;
			pPicture->bStallStc      = false;/* assume non-ignore pic always needs to advance STC */
			hWindow->ulDropCntNonIgnoredPics--;
		}

		/* MFD must be build by the last window connected to the src, otherwise
		 * Mfd might starts to feed after the 1st win gets enabled, then the
		 * following win might join in the vnet after feeding started */
		BVDC_P_feeder_SetRulBuildWinId_isr(hWindow->stCurInfo.hSource->hMpegFeeder, hWindow->eId);
	}

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_VNET_USED_DNR_AT_WRITER(hWindow->stVnetMode))
	{
		/* TODO: avoid duplicate setinfo when dnr is shared */
		BVDC_P_Dnr_SetInfo_isr(hWindow->stCurResource.hDnr, pPicture);
		BVDC_P_Dnr_SetRulBuildWinId_isr(hWindow->stCurResource.hDnr, hWindow->eId);
	}
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_WRITER(hWindow->stVnetMode))
	{
		BVDC_P_Hscaler_SetInfo_isr(hWindow->stCurResource.hHscaler, hWindow, pPicture);
	}
#endif

#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode))
	{
		if(bVideoDetect)
		{
			BVDC_P_Window_AdjustPicRepeatBit_isr(hWindow, pPicture);
			BVDC_P_Mad_SetInfo_isr(hWindow->stCurResource.hMad32, hWindow, pPicture);
			BVDC_P_Window_AdjustForMadDelay_isr(hWindow, pPicture);
		}
		else
		{
			/* TODO: why we could see NULL hWindow->hMad32 here? */
			if(hWindow->stCurResource.hMad32)
				BVDC_P_Mad_SetEnable_isr(hWindow->stCurResource.hMad32, false);
		}
	}
#endif

#if (BVDC_P_SUPPORT_MCVP)
	if(BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode))
	{
		BVDC_P_Hscaler_SetInfo_isr(hWindow->stCurResource.hMcvp->hHscaler, hWindow, pPicture);
		BVDC_P_MCVP_SetInfo_isr(hWindow->stCurResource.hMcvp,hWindow, pPicture);
	}
	if(BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stMcvpMode))
	{
		if(bVideoDetect)
		{
			BVDC_P_Window_AdjustPicRepeatBit_isr(hWindow, pPicture);
			/* this is for downstream submodules like scl and fgt */
			pPicture->eSrcPolarity = BAVC_Polarity_eFrame;
			BVDC_P_Window_AdjustForMadDelay_isr(hWindow, pPicture);
		}
	}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) && (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_VNET_USED_ANR_AT_WRITER(hWindow->stVnetMode))
	{
		if(!hWindow->stCurResource.hAnr->bMemSaving)
		{
			BVDC_P_Window_SetMadAnrTestFeature1_isr(hWindow,
				hWindow->stCurInfo.stAnrSettings.ePxlFormat, pPicture->pAnrIn,
				pPicture, &hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1,
				true, &hWindow->stCurResource.hAnr->stTestFeature1);
		}
	}
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode))
	{
		BVDC_P_Scaler_SetInfo_isr(hWindow->stCurResource.hScaler, pPicture);
	}

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_WRITER(hWindow->stVnetMode))
	{
		BVDC_P_Fgt_SetInfo_isr(hWindow->stCurResource.hFgt, pPicture);
	}
#endif

	if(BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode))
	{
		uint32_t ulRectIdx;

		/* MosaicMode: set the rect idx to program capture */
		if(pMvdFieldData != NULL)
		{
			ulRectIdx = (pCurInfo->bMosaicMode)?
				pMvdFieldData->ulChannelId : ulPictureIdx;
			BVDC_P_Capture_SetInfo_isr(hWindow->stCurResource.hCapture, hWindow, pPicture,
				ulRectIdx, (NULL == pMvdFieldData->pNext));
		}
		else
		{
			ulRectIdx = ulPictureIdx;
			BVDC_P_Capture_SetInfo_isr(hWindow->stCurResource.hCapture, hWindow, pPicture,
				ulRectIdx, true);
		}
	}

	BDBG_LEAVE(BVDC_P_Window_Writer_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 * This READER function displays a buffer node that is captured by the
 * WRITER function.
 */
void BVDC_P_Window_Reader_isr
	( BVDC_Window_Handle               hWindow,
	  const BAVC_Polarity              eFieldId,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_Compositor_Handle hCompositor;
	BVDC_P_PictureNode *pPicture;
	BVDC_P_Window_Info *pCurInfo;
	BVDC_P_Window_DirtyBits *pCurDirty;
	const BVDC_P_Source_Info *pSrcInfo;
	bool bVideoDetect = false;
	bool bFixedColor = false;
	bool bCscAdjust = false;
#if BFMT_LEGACY_3DTV_SUPPORT
	bool bOrigMute = false;
#endif
	uint32_t                 ulRWindowXOffset;

	BDBG_ENTER(BVDC_P_Window_Reader_isr);

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	hCompositor = hWindow->hCompositor;
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);


	/* Get dirty bits to check if needed to rebuild csc rul. */
	pSrcInfo  = &hWindow->stCurInfo.hSource->stCurInfo;
	pCurInfo  = &hWindow->stCurInfo;
	pCurDirty = &hWindow->stCurInfo.stDirty;

	/* Nothing to do for gfx window, except for when destroying. */
	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId) &&
	  (BVDC_P_CLEAN == pCurDirty->stBits.bDestroy))
	{
		/* Gfx does not need to use these dirty bits. */
		BVDC_P_CLEAN_ALL_DIRTY(pCurDirty);
		return;
	}

	/* Shutting down the READER's blocks that's in the display _isr
	 * (1) Shutdown for destroy
	 * (2) Shutdown for removing/adding block from/to READER (change vnet)
	 * (3) Shutdown for source bringup or transitioning
	 *
	 * After READER is shutdown (eReaderState == BVDC_P_State_eShutDown)
	 * WRITER will shut WRITER's blocks (eWriterState == BVDC_P_State_eShutDown).
	 * Target state move:
	 *
	 * eReaderState == BVDC_P_State_eShutDown
	 * eWriterState == BVDC_P_State_eShutDown
	 *
	 * To initiate the shutdown put both into BVDC_P_State_eShutDownPending. */
	if(BVDC_P_IS_DIRTY(pCurDirty))
	{
		/* clear dirty bit and invalidate window color to redetermine new matrix */
		if(pCurDirty->stBits.bCscAdjust)
		{
			pCurDirty->stBits.bCscAdjust = BVDC_P_CLEAN;
			hWindow->ePrevMatrixCoeffs = BAVC_MatrixCoefficients_eUnknown;
			bCscAdjust = true;
		}

		/* Set new luma key */
		if((pCurDirty->stBits.bMosaicMode) ||
		   (pCurDirty->stBits.bLumaKeyAdjust))
		{
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_LUMA_KEYING) =
				BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, ENABLE, pCurInfo->bLumaKey     ) |
				BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_MASK, pCurInfo->ucLumaKeyMask) |
				BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_HIGH, pCurInfo->ucLumaKeyHigh) |
				BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_LOW , pCurInfo->ucLumaKeyLow );

			/* SW7425-2236: Hijacked colorkey to masked out garbage for mosaic mode. */
#if  defined(BCHP_CMP_0_V0_RECT_COLOR) && \
    !defined(BCHP_CMP_0_V0_RECT_TOP_CTRL_RECT_KEY_VALUE_MASK)
			if(pCurInfo->bMosaicMode)
			{
				BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_LUMA_KEYING) =
					BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, ENABLE, 1   ) |
					BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_MASK, 0xff) |
					BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_HIGH, 0x01) |
					BCHP_FIELD_DATA(CMP_0_V0_LUMA_KEYING, Y_LOW , 0x00);
			}
#endif
			/* Clear window dirty bit */
			pCurDirty->stBits.bLumaKeyAdjust = BVDC_P_CLEAN;
			/* Set dirty bit for compositor */
			hCompositor->ulLumaKeyAdjust[hWindow->eId] = BVDC_P_RUL_UPDATE_THRESHOLD;
		}

		if(pCurDirty->stBits.bShutdown)
		{
			if(((BVDC_P_State_eShutDownPending == pCurInfo->eReaderState) ||
			    (BVDC_P_State_eShutDownRul == pCurInfo->eReaderState) ||
			    (BVDC_P_State_eDrainVnet == pCurInfo->eReaderState)))
			{
				/* eWriterState is waiting for eReaderState to move to shutdown. */
				/* Make sure we executed the RUL that shutdown reader's modules. */
				bool bLastExecuted;

				/* to prevent back-to-back callbacks prematurely exiting reader
				 * shutdown without execution;
				 * Note: progressive format uses the same slot for the next RUL
				 * as the current one; */
				if(BAVC_Polarity_eFrame != eFieldId)
				{
					BRDC_Slot_Handle hCurSlot;
					BRDC_List_Handle hCurList;
					BAVC_Polarity eNextFieldId = BVDC_P_NEXT_POLARITY(eFieldId);

					if(!hCompositor->hSyncLockSrc)
					{
						hCurSlot = BVDC_P_CMP_GET_SLOT(hCompositor, eNextFieldId);
						hCurList = BVDC_P_CMP_GET_LIST(hCompositor, eNextFieldId);
					}
					else
					{
						/* if field swap, the field Id already rotated */
						if(hCompositor->hSyncLockSrc->bFieldSwap)
						{
							eNextFieldId = eFieldId;
						}
						BDBG_ASSERT(hCompositor->hSyncLockSrc);
						hCurSlot = BVDC_P_SRC_GET_SLOT(hCompositor->hSyncLockSrc, eNextFieldId);
						hCurList = BVDC_P_SRC_GET_LIST(hCompositor->hSyncLockSrc, eNextFieldId);
					}
					BRDC_Slot_UpdateLastRulStatus_isr(hCurSlot, hCurList, true);
					bLastExecuted = BRDC_List_GetLastExecStatus_isr(hCurList);
					BDBG_MSG(("Window (R) current slot bExe = %d for field %d",
						bLastExecuted, eNextFieldId));
				}
				else
				{
					bLastExecuted = pList->bLastExecuted;
				}

				if((BVDC_P_State_eShutDownPending == pCurInfo->eReaderState) ||
				   ((!bLastExecuted) && (BVDC_P_State_eShutDownRul == pCurInfo->eReaderState)))
				{
					hCompositor->bInitial = true; /* inform comp to reset */
#if BVDC_P_SUPPORT_CMP_CLEAR_RECT
					/* MosaicMode: clear rect settings for V0 only; */
					if(pCurInfo->bMosaicMode)
					{
						BVDC_P_Window_SetClearRect_isr(hWindow, pCurInfo, false);
					}
#endif

					pCurInfo->eReaderState = BVDC_P_State_eShutDownRul;
					BDBG_MSG(("(2.a) Window[%d] Slip(R%d) is BVDC_P_State_eShutDownRul, bExe = %d",
						hWindow->eId, eFieldId, bLastExecuted));

					/* size must be set so constant color doesn't hang compositor */
					BVDC_P_Window_SetMutedSurSize_isr(hWindow, eFieldId);

					/* so that cmp would build less to RUL */
					BVDC_P_Window_WinOutSetEnable_isr(hWindow, 0, false);
				}
				else if((BVDC_P_State_eShutDownRul == pCurInfo->eReaderState) ||
						((!bLastExecuted) && (BVDC_P_State_eDrainVnet == pCurInfo->eReaderState)))
				{
					pCurInfo->eReaderState = BVDC_P_State_eDrainVnet;
					BDBG_MSG(("(2.b) Window[%d] Slip(R%d) is BVDC_P_State_eDrainVnet, bExe = %d",
						hWindow->eId, eFieldId, bLastExecuted));
				}
				else if(BVDC_P_State_eDrainVnet == pCurInfo->eReaderState) /* && bLastEcecuted */
				{
					/* clear off reader side vnet sw infor and release resources including
					 * loop-back, free-channel, mad/anr/cap bufs; It must be called after
					 * reader's drain rul are executed.
					 * note: it is state BVDC_P_State_eShutDownRul that causes each module
					 * to build into RUL to disable HW and to drop off from vnet */
					BVDC_P_Window_UnsetReaderVnet_isr(hWindow, &hWindow->stVnetMode);
					pCurInfo->eReaderState = BVDC_P_State_eShutDown;
					BDBG_MSG(("(2.c) Window[%d] Slip(R%d) is BVDC_P_State_eShutDown, bExe = %d",
						hWindow->eId, eFieldId, bLastExecuted));

					/* this causes empty RDC slot with mpeg src? */
					/*BVDC_P_Window_ProcPostShutDown_isr(hWindow);*/
				}
				/* Gfx does not have writer so, just complete the shutdown
				 * when reader is shutdown. */
				if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId) &&
				   (BVDC_P_State_eShutDown == pCurInfo->eReaderState))
				{
					pCurInfo->eWriterState = BVDC_P_State_eShutDown;
				}
			}
			else if(BVDC_P_State_eShutDown == pCurInfo->eReaderState)
			{
				/* Waiting for eWriterState move to shutdown.
				 * stay off after reader turns off the video path; */
				BDBG_MSG(("(*) Window[%d] Slip(R%d) stay off", hWindow->eId, eFieldId));
			}
			return;
		}
		/* TODO: reader will indeed stay off automatically according to readerState
		 * and pic->bMute, therefore this short circuit might not be needed. */
		else if((pCurDirty->stBits.bDestroy) ||
		        (pCurDirty->stBits.bSrcPending) ||
		        (pCurDirty->stBits.bReConfigVnet))
		{
			/* Reader stays off until dirty bits are handle. */
			BDBG_MSG(("($) Window[%d] Slip(R%d) stay off", hWindow->eId, eFieldId));
			return;
		}
	}

	if (!BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
	{
		if((pSrcInfo->eMuteMode == BVDC_MuteMode_eRepeat) && (hWindow->pCurReaderNode != hWindow->pCurWriterNode))
		{
			pPicture = hWindow->pCurReaderNode;

			/* repeated picture may need MAD trick mode to handle; */
			pPicture->stFlags.bPictureRepeatFlag = true;
		}
		else
		{
			pPicture = BVDC_P_Buffer_GetNextReaderNode_isr(hWindow, eFieldId);
		}
	}
	else
	{
		pPicture = &hWindow->stCurInfo.hSource->hVfdFeeder->stPicture;
	}
	BDBG_ASSERT (pPicture);

	/* Set eDstPolarity before set playback */
	if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
	{
		/* If scaler was used to correct field inverstion here are thing that
		 * we need to keep in mind.  If scaler is after capture the destination
		 * polarity is determine now (in reader), but if scaler before capture
		 * the is destination polarity already determined (in writer). */
#if BFMT_LEGACY_3DTV_SUPPORT
		if(VIDEO_FORMAT_IS_CUSTOM_1080P3D(hCompositor->stCurInfo.pFmtInfo->eVideoFmt))
		{
			pPicture->eDstPolarity = BAVC_Polarity_eFrame;
		}
		else
#endif
		{
			pPicture->eDstPolarity = eFieldId;
		}
	}

	/* Save the display polarity as a feedback to help writer to predict
	 * destination polarity in the next round. Or in the case that
	 * reader node gets moved by writer ISR, this will help predict the
	 * VEC polarity.
	 */
	pPicture->eDisplayPolarity = eFieldId;

	/* Prevent this from flooding the screen.
	BDBG_MSG(("Win%d R(%d): B(%d) (%d -> %d)", hWindow->eId, eFieldId,
		pPicture->ulBufferId, pPicture->eSrcPolarity, pPicture->eDstPolarity));
	*/

	if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
	{
		BVDC_P_Source_MpegGetStatus_isr(pCurInfo->hSource, &bVideoDetect);
	}
	else if(BVDC_P_SRC_IS_HDDVI(hWindow->stCurInfo.hSource->eId))
	{
		BVDC_P_HdDvi_GetStatus_isr(pCurInfo->hSource->hHdDvi, &bVideoDetect);
	}
	else if(BVDC_P_SRC_IS_ANALOG(hWindow->stCurInfo.hSource->eId))
	{
		BVDC_P_Vdec_GetStatus_isr(hWindow->stCurInfo.hSource->hVdec, NULL, &bVideoDetect, NULL, NULL, NULL);
	}
	else if(BVDC_P_SRC_IS_ITU656(hWindow->stCurInfo.hSource->eId))
	{
#if (BVDC_P_SUPPORT_NEW_656_IN_VER)
		BVDC_P_656In_GetStatus_isr(hWindow->stCurInfo.hSource->h656In, &bVideoDetect);
#else
		BVDC_P_Vdec_GetStatus_isr(hWindow->stCurInfo.hSource->hVdec, NULL, &bVideoDetect, NULL, NULL, NULL);
#endif
	}

	/* Update reader node's playback time stamp in either slip case above */
	if(!hWindow->bSyncLockSrc)
	{
		/* also calculate the absolute actual buf delay of the previous picture node;
		   Note: if source lost signal, don't need to update the current buf delay
		         or make any clock adjustment;
		 */
		/* counting down TS sampling vsyncs */
		if(hWindow->hBuffer->ulGameDelaySampleCnt == 0)
		{
			/* reset sampling count only for new picture */
			if(!pPicture->stFlags.bPictureRepeatFlag)
			{
				hWindow->hBuffer->ulGameDelaySampleCnt = hWindow->hBuffer->ulGameDelaySamplePeriod - 1;
			}

			/* alignment measurement */
			if( hWindow->pCurReaderNode && hWindow->stCurInfo.hSource->bStartFeed)
			{
				hWindow->ulCurBufDelay = BVDC_P_Buffer_CalculateBufDelay(pPicture);

				/* check game mode clock adjustment */
				if(pCurInfo->stGameDelaySetting.bEnable)
				{
					#define BVDC_P_GAME_MODE_PXL_FREQ_MAX   (14850) /* 1080p60 pxl freq */
					int32_t lTolerance =
						(int32_t)pCurInfo->stGameDelaySetting.ulBufferDelayTolerance;

					hWindow->lCurGameModeLag = hWindow->ulCurBufDelay -
						pCurInfo->stGameDelaySetting.ulBufferDelayTarget;

					if((hWindow->lCurGameModeLag > lTolerance) ||
					   (hWindow->lCurGameModeLag < (-lTolerance)))
					{
						/* Note: no adjustment for rate gap and force repeat; */
						hWindow->bAdjGameModeClock =
							(BVDC_P_WrRate_NotFaster == hWindow->hBuffer->eWriterVsReaderRateCode) &&
							/*(BVDC_P_WrRate_NotFaster == hWindow->hBuffer->eReaderVsWriterRateCode) &&*/
							(pSrcInfo->eMuteMode != BVDC_MuteMode_eRepeat);

						/* VESA formats allow 0.3% variance in rate; 59/60 slip without
						   frame rate tracking; both will automatically turn on coarse
						   clock adjustment to track the source rate; */
						hWindow->bCoarseAdjClock   =
							pCurInfo->stGameDelaySetting.bForceCoarseTrack ||
							VIDEO_FORMAT_IS_VESA(pSrcInfo->pFmtInfo->eVideoFmt) ||
							!pCurInfo->bUseSrcFrameRate;

						/* No fast adjustment if both source and display formats are high
						   pixel frequency to avoid RTS violation due to display clock change; */
						hWindow->bFastAdjClock =
							(pSrcInfo->pFmtInfo->ulPxlFreq < BVDC_P_GAME_MODE_PXL_FREQ_MAX) ||
							(hCompositor->stCurInfo.pFmtInfo->ulPxlFreq < BVDC_P_GAME_MODE_PXL_FREQ_MAX);
					}
					else
					{
						hWindow->bAdjGameModeClock = false;
					}
				}
			}
			else
			{
				hWindow->bAdjGameModeClock = false;
			}
		}
		else
		{
			/* count down and disable alignment control */
			hWindow->hBuffer->ulGameDelaySampleCnt--;
			hWindow->bAdjGameModeClock = false;
		}
		/*
		BDBG_MSG(("Rd buf delay = %d, lag=%d, sample cnt=%d", hWindow->ulCurBufDelay,
			hWindow->lCurGameModeLag, hWindow->hBuffer->ulGameDelaySampleCnt));*/
	}

#if BFMT_LEGACY_3DTV_SUPPORT /* alternate L/R windows visibility for 1080p3D->2160i48 only */
	bOrigMute = pPicture->stFlags.bMute;
	if(VIDEO_FORMAT_IS_CUSTOM_1080P3D(hCompositor->stCurInfo.pFmtInfo->eVideoFmt) &&
	   VIDEO_FORMAT_IS_1080P_3DOU_AS(pSrcInfo->pFmtInfo->eVideoFmt))
	{
		if(BAVC_Polarity_eTopField == eFieldId)
		{
			if(BVDC_P_WindowId_eComp0_V1 == hWindow->eId)
			{
				pPicture->stFlags.bMute = true;
			}
		}
		else
		{
			if(BVDC_P_WindowId_eComp0_V0 == hWindow->eId)
			{
				pPicture->stFlags.bMute = true;
			}
		}
	}
#endif

	/* General mute when picture is not ready, shutdown is requested! */
	if(pPicture->stFlags.bMute)
	{
		/*BDBG_MSG(("Window[%d] R: %s(%d) mutes", hWindow->eId,
			eFieldId ? "B" : "T", pPicture->ulBufferId)); */

		/* size must be set so constant color doesn't hang compositor */
		BVDC_P_Window_SetMutedSurSize_isr(hWindow, eFieldId);

		/* so that cmp would build less to RUL */
		BVDC_P_Window_WinOutSetEnable_isr(hWindow, 0, false);

		goto done;
	}

	/* Note: only one video window could be the master. */
	if((pCurInfo->bUseSrcFrameRate) &&
	   (BAVC_FrameRateCode_eUnknown != pPicture->eFrameRateCode))
	{
		if(hCompositor->eSrcFRateCode != pPicture->eFrameRateCode)
		{
			hCompositor->eSrcFRateCode = pPicture->eFrameRateCode;
			hCompositor->bFullRate     =
				BVDC_P_IS_FULL_FRAMRATE(pPicture->eFrameRateCode);
		}
	}

	if(BVDC_PhaseAdjustMode_eDisable != pCurInfo->ePhaseAdjustMode)
	{
		hCompositor->ulVdecPhase = pPicture->ulVdecPhase;
	}

	hCompositor->b240pMode = pPicture->stFlags.b240pMode;

	/* If we're doing fixed color make sure to bypass colorspace conversion.
	 * otherwise fixed color going thru one of the color coversion will be
	 * different due precisions lost.  */
	if((BVDC_MuteMode_eConst == pSrcInfo->eMuteMode) ||
	   (pPicture->stFlags.bMuteFixedColor) ||
	   (hWindow->bMuteBypass))
	{
		bFixedColor = true;
		pPicture->eMatrixCoefficients = BAVC_MatrixCoefficients_eUnknown;
	}

	/* mark csc to be adjusted */
	if ((hWindow->ePrevMatrixCoeffs != pPicture->eMatrixCoefficients)
	    || pCurDirty->stBits.bMosaicMode
#if(BVDC_P_SUPPORT_PEP_VER < BVDC_P_SUPPORT_PEP_VER_4)
	    || pCurInfo->bContrastStretch
#endif
	   )
	{
		bCscAdjust = true;
	}

	if (pCurDirty->stBits.bMosaicMode)
	{
		pCurDirty->stBits.bMosaicMode = BVDC_P_CLEAN;
	}

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
	if(hWindow->stCurInfo.bMosaicMode)
	{
		uint16_t i = 0;
		for (i = 0; i < hWindow->stCurInfo.ulMosaicCount; i++)
		{
			if (hWindow->aePrevMosaicClrSpaceList[i] != hWindow->aeMosaicClrSpaceList[i])
			{
				bCscAdjust = true;
				break;
			}
		}
	}
#endif

	/* Update the compositor's surface color space conversion matrix. */
	if(!hCompositor->bIsBypass && bCscAdjust)
	{
		bool bInputXvYcc = (pPicture->eTransferCharacteristics == BAVC_TransferCharacteristics_eIec_61966_2_4);
		BVDC_P_CmpColorSpace eCmpColorSpace = hCompositor->eCmpColorSpace;

		/* hack to disable xvYCC input and always load non xvYCC Matrix A */
		/* bInputXvYcc = false : don't load special xvYCC Matrix A */
		/* bInputXvYcc = true  : always load special xvYCC Matrix A */
		/* bInputXvYcc = hCompositor->hDisplay->stCurInfo.bXvYcc : have output control both matrices and N2L/L2N bits */
/*
		bInputXvYcc  = false;
*/
		BDBG_MSG(("Window[%d] CSC matrix change: old = %d, new = %d.",
			hWindow->eId, hWindow->ePrevMatrixCoeffs, pPicture->eMatrixCoefficients));

		/* Select the color space conversion. */
#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN

		if(BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
		{
			BVDC_P_CscCoeffs *pCscA, *pCscB, *pCscC;

			BVDC_P_Compositor_GetCscTablesNonLinear_isr(&pCscA, &pCscB, &pCscC,
					pPicture->eMatrixCoefficients, eCmpColorSpace, bInputXvYcc);

			if(pCurInfo->bUserCscA)
			{
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stPriCscA, pCurInfo->pl32_MatrixA, pCurInfo->ulUserShiftA);
			}
			else
			{
				hWindow->stPriCscA = *pCscA;
			}

			if(pCurInfo->bUserCscB)
			{
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stPriCscB, pCurInfo->pl32_MatrixB, pCurInfo->ulUserShiftB);
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stSecCscB, pCurInfo->pl32_MatrixB, pCurInfo->ulUserShiftB);
			}
			else
			{
				hWindow->stPriCscB = *pCscB;
				hWindow->stSecCscB = *pCscB;
			}

			if(pCurInfo->bUserCscC)
			{
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stPriCscC, pCurInfo->pl32_MatrixC, pCurInfo->ulUserShiftC);
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stSecCscC, pCurInfo->pl32_MatrixC, pCurInfo->ulUserShiftC);
			}
			else
			{
				hWindow->stPriCscC = *pCscC;
				hWindow->stSecCscC = *pCscC;
			}
		}
		else
#endif
#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
		if(hWindow->stCurInfo.bClearRect)
		{
			BVDC_P_CscCoeffs *pCsc;
			uint16_t i = 0;

			bool abFreeMosaicCscList[BVDC_P_SUPPORT_CMP_MOSAIC_CSC];
			bool bDoXvYcc = false;

			BKNI_Memcpy(hWindow->aeClrSpaceToMosaicCscMap, s_aCMP_ClrSpace_To_MosaicCscType_MapTbl, sizeof(hWindow->aeClrSpaceToMosaicCscMap));
			BKNI_Memcpy(hWindow->aeMosaicCscToClrSpaceMap, s_aCMP_MosaicCscType_To_ClrSpace_MapTbl, sizeof(hWindow->aeMosaicCscToClrSpaceMap));

#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC < BVDC_P_NUM_MOSAIC_CSC_TYPE
			/* reset mosaic csc used flags */
			BKNI_Memset(abFreeMosaicCscList, true, sizeof(abFreeMosaicCscList));

			/* mark mosaic cscs used by current list of mosaic pictures */
			for (i = 0; i < hWindow->stCurInfo.ulMosaicCount; i++)
			{
				BAVC_MatrixCoefficients eCurColorSpace;
				uint16_t usMosaicCscIdx;

				eCurColorSpace = hWindow->aeMosaicClrSpaceList[i];
				usMosaicCscIdx = hWindow->aeClrSpaceToMosaicCscMap[eCurColorSpace];

				if (pCurInfo->bCscRgbMatching &&
					((eCurColorSpace == BAVC_MatrixCoefficients_eXvYCC_601) ||
					 ((eCurColorSpace == BAVC_MatrixCoefficients_eSmpte_170M) && bInputXvYcc )))
				{
					bDoXvYcc = true;
				}
				else if (usMosaicCscIdx != BVDC_P_CmpColorSpace_eInvalid)
				{
					abFreeMosaicCscList[usMosaicCscIdx] = false;
				}
				else
				{
					BDBG_WRN(("Mosaic picture [%d] has invalid colorspace [%d]", i, usMosaicCscIdx));
				}
			}

			if (bDoXvYcc)
			{
				bool bPrevInputXvYcc = bInputXvYcc;
				bInputXvYcc = false;
				hWindow->aeClrSpaceToMosaicCscMap[BVDC_P_MatrixCoefficients_eXvYcc_601] = BVDC_P_CmpColorSpace_eNtscSdYCrCb;

				/* since there are only 5 cscs and XvYcc makes up a unique 6th csc, replace
				unused csc with XvYcc if possible */
				for	(i = 0;	i <	BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
				{
					if (abFreeMosaicCscList[i] == true)
					{
						bInputXvYcc = bPrevInputXvYcc;
						hWindow->aeMosaicCscToClrSpaceMap[i] = BVDC_P_MatrixCoefficients_eXvYcc_601;
						hWindow->aeClrSpaceToMosaicCscMap[BVDC_P_MatrixCoefficients_eXvYcc_601] = i;
						break;
					}
				}
			}

#endif

			/* get the actual mosaic csc tables	 */
			for	(i = 0;	i <	BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
			{
				BVDC_P_Compositor_GetCscTable_isr(&pCsc, pCurInfo->bCscRgbMatching,
					hWindow->aeMosaicCscToClrSpaceMap[i], eCmpColorSpace, bInputXvYcc);
				/* TODO: make sure user csc is not enabled with mosaic */
				hWindow->astMosaicCscList[i] = *pCsc;
			}

			/* set the mosaic csc indices */
			BVDC_P_Window_SetMosaicCsc_isr(hWindow);
			BKNI_Memcpy(hWindow->aePrevMosaicClrSpaceList, hWindow->aeMosaicClrSpaceList, sizeof(hWindow->aePrevMosaicClrSpaceList));
		}
		else
#endif
		{
			BVDC_P_CscCoeffs *pCsc;

			BVDC_P_Compositor_GetCscTable_isr(&pCsc, pCurInfo->bCscRgbMatching,
				pPicture->eMatrixCoefficients, eCmpColorSpace, bInputXvYcc);

			if(pCurInfo->bUserCsc)
			{
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stPriCsc, pCurInfo->pl32_Matrix, pCurInfo->ulUserShift);
				BVDC_P_Csc_FromMatrix_isr(&hWindow->stSecCsc, pCurInfo->pl32_Matrix, pCurInfo->ulUserShift);
			}
			else
			{
				hWindow->stPriCsc = *pCsc;
				hWindow->stSecCsc = *pCsc;
			}
		}

		/* If dynamic contrast stretch is enable, brightness, contrast and */
		/* saturation will be modified every other field, so this assignment */
		/* will be take care of in the PEP block */
		if(!pCurInfo->bContrastStretch)
		{
			hWindow->sFinalBrightness = pCurInfo->sBrightness;
			hWindow->sFinalContrast   = pCurInfo->sContrast;
			hWindow->sFinalSaturation = pCurInfo->sSaturation;
		}

		/* set count to let cmp RUL update matrix */
		hCompositor->ulCscAdjust[hWindow->eId] = BVDC_P_RUL_UPDATE_THRESHOLD;
		hCompositor->bCscCompute[hWindow->eId] = true;
		hCompositor->bCscDemoCompute[hWindow->eId] = true;
		hWindow->ePrevMatrixCoeffs = pPicture->eMatrixCoefficients;
	}

	/* Note: the reader side setting should start from upstream then to the downstream. */
	/* Setup video feeder that playback from captured buffer */
	if(BVDC_P_VNET_USED_PLAYBACK(hWindow->stVnetMode))
	{
		BVDC_P_Feeder_SetPlaybackInfo_isr(hWindow->stCurResource.hPlayback,
			pPicture, bFixedColor, pSrcInfo->ulMuteColorYCrCb);
	}

	else if (BVDC_P_SRC_IS_VFD(pCurInfo->hSource->eId))
	{
		BVDC_P_Feeder_SetPlaybackInfo_isr(pCurInfo->hSource->hVfdFeeder,
			 pPicture, bFixedColor, pSrcInfo->ulMuteColorYCrCb);
	}

#if (BVDC_P_SUPPORT_DNR)
	if(BVDC_P_VNET_USED_DNR_AT_READER(hWindow->stVnetMode))
	{
		/* TODO: avoid duplicate setinfo when dnr is shared */
		BVDC_P_Dnr_SetInfo_isr(hWindow->stCurResource.hDnr, pPicture);
		BVDC_P_Dnr_SetRulBuildWinId_isr(hWindow->stCurResource.hDnr, hWindow->eId);
	}
#endif

#if (BVDC_P_SUPPORT_HSCL)
	if(BVDC_P_VNET_USED_HSCL_AT_READER(hWindow->stVnetMode))
	{
		BVDC_P_Hscaler_SetInfo_isr(hWindow->stCurResource.hHscaler, hWindow, pPicture);
	}
#endif

	/* NOTE: we should config MAD32 before SCL since the picture node's source
	 * polarity could be modified by mad setinfo_isr; */
#if (BVDC_P_SUPPORT_MAD)
	if(BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode))
	{
		BVDC_P_Window_AdjustPicRepeatBit_isr(hWindow, pPicture);
		BVDC_P_Mad_SetInfo_isr(hWindow->stCurResource.hMad32, hWindow, pPicture);
		BVDC_P_Window_AdjustForMadDelay_isr(hWindow, pPicture);
	}
#endif

#if (BVDC_P_SUPPORT_MCVP)
	if(BVDC_P_VNET_USED_MCVP_AT_READER(pPicture->stVnetMode))
	{
		BVDC_P_Hscaler_SetInfo_isr(hWindow->stCurResource.hMcvp->hHscaler, hWindow, pPicture);
		BVDC_P_MCVP_SetInfo_isr(hWindow->stCurResource.hMcvp, hWindow, pPicture);
	}

	if(BVDC_P_VNET_USED_MAD_AT_READER(pPicture->stVnetMode))
	{
		BVDC_P_Window_AdjustPicRepeatBit_isr(hWindow, pPicture);
		pPicture->eSrcPolarity = BAVC_Polarity_eFrame;
		BVDC_P_Window_AdjustForMadDelay_isr(hWindow, pPicture);
	}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR) && (BVDC_P_SUPPORT_ANR)
	if(BVDC_P_VNET_USED_ANR_AT_READER(hWindow->stVnetMode))
	{
		if(!hWindow->stCurResource.hAnr->bMemSaving)
		{
			BVDC_P_Window_SetMadAnrTestFeature1_isr(hWindow,
				hWindow->stCurInfo.stAnrSettings.ePxlFormat, pPicture->pAnrIn,
				pPicture, &hWindow->stCurInfo.stAnrSettings.stVideoTestFeature1,
				false, &hWindow->stCurResource.hAnr->stTestFeature1);
		}
	}
#endif

	if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
	{
		BVDC_P_Scaler_SetInfo_isr(hWindow->stCurResource.hScaler, pPicture);
	}

#if (BVDC_P_SUPPORT_FGT)
	if(BVDC_P_VNET_USED_FGT_AT_READER(hWindow->stVnetMode))
	{
		BVDC_P_Fgt_SetInfo_isr(hWindow->stCurResource.hFgt, pPicture);
	}
#endif

	if(BVDC_P_VNET_USED_MAD_AT_READER(pPicture->stVnetMode))
	{
		/* restore original source polarity in case the next time sync slip */
		pPicture->eSrcPolarity = pPicture->PicComRulInfo.eSrcOrigPolarity;
	}

#if(BVDC_P_SUPPORT_HIST && BVDC_P_SUPPORT_HIST_VER == BVDC_P_SUPPORT_HIST_VER_1)
	if(hWindow->eId == BVDC_P_WindowId_eComp0_V0)
		BVDC_P_Histo_UpdateHistoData_isr(hWindow->stCurResource.hPep);
#endif

	/* Update the informations that are tied to pPicture;
	 * don't set the window's canvas position yet since it might need adjustment
	 * if its compositor has a VBI pass-throughed window; */
	ulRWindowXOffset = (uint32_t) (hWindow->stCurInfo.lRWinXOffsetDelta + pPicture->pWinOut->lLeft);
	BVDC_P_Window_SetSurfaceSize_isr(hWindow, pPicture->pWinIn, eFieldId);
	BVDC_P_Window_SetDisplaySize_isr(hWindow, pPicture->pWinOut, eFieldId,ulRWindowXOffset);

	BDBG_MODULE_MSG(BVDC_CMP_SIZE,("Surface %d Delay %d %4d x %4d", hWindow->hCompositor->eId, hWindow->hCompositor->ulDelayCnt,
		pPicture->pWinIn->ulWidth,  pPicture->pWinIn->ulHeight>>(eFieldId!=BAVC_Polarity_eFrame)));
	BDBG_MODULE_MSG(BVDC_CMP_SIZE,("Display %d Delay %d %4d x %4d", hWindow->hCompositor->eId, hWindow->hCompositor->ulDelayCnt,
		pPicture->pWinOut->ulWidth, pPicture->pWinOut->ulHeight>>(eFieldId!=BAVC_Polarity_eFrame)));
	/* compositor check the sur enable bit to start program  */
	BVDC_P_Window_WinOutSetEnable_isr(hWindow, pPicture->ucAlpha,  true);

	/* MosaicMode: update ClearRect mask */
#if BVDC_P_SUPPORT_CMP_CLEAR_RECT
	/* MosaicMode: clear rect settings for V0 only; */
	if(pCurInfo->bMosaicMode &&
	   (hWindow->ulMosaicRectSet != pPicture->ulMosaicRectSet))
	{
		hWindow->ulMosaicRectSet = pPicture->ulMosaicRectSet;
		BVDC_P_Window_SetClearRect_isr(hWindow, pCurInfo, true);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
		hCompositor->ulMosaicAdjust[hWindow->eId] = BVDC_P_RUL_UPDATE_THRESHOLD;
#else
		hCompositor->ulMosaicAdjust = BVDC_P_RUL_UPDATE_THRESHOLD;
#endif
	}
#endif

	/* This needs to be called after BVDC_P_Window_CheckReaderIsrOrder_isr,
	 * where BVDC_P_Window_UpdateTimestamps_isr is called */
	BVDC_P_Window_UpdateCallback_isr(hWindow, eFieldId);

	/* Update the corresponding STG meta data*/
	BVDC_P_Compositor_SetMBoxMetaData_isr(pPicture, hWindow->hCompositor);

done:
	hWindow->pCurReaderNode = pPicture;
#if BFMT_LEGACY_3DTV_SUPPORT
	pPicture->stFlags.bMute = bOrigMute;
#endif

	BDBG_LEAVE(BVDC_P_Window_Reader_isr);
	return;
}

static void BVDC_P_Window_WinOutBuildRul_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_PictureNode              *pPicture,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState )
{
	uint32_t  ulRulOpsFlags;
	BVDC_Compositor_Handle hCompositor;

	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	hCompositor = hWindow->hCompositor;
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);

	/* Winow_BuildReaderRul_isr could be called before the surface in comp is
	 * really enabled. BVDC_P_SubRul_GetOps_isr's internal state could be mess-up
	 * if we call it in this case */
	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(&(hWindow->stWinOutVnet),
			 hWindow->eId, eVnetState, pList->bLastExecuted);

	/* join in vnet after enable. note: its src mux is initialed as disabled */
	if(ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		/* HW does not require to write SURFACE_CTRL_ENABLE every vsync, but we found
		 * main win-sur disabled after PIP is opened, so we write it every vsync */
		BVDC_P_Window_WinOutSetEnable_isr(hWindow, pPicture->ucAlpha, true);
		BVDC_P_WIN_WRITE_TO_RUL(CMP_0_V0_SURFACE_CTRL, pList->pulCurrent);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if(ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hWindow->stWinOutVnet), pList);
		}
	}
	else
	{
		if(ulRulOpsFlags & BVDC_P_RulOp_eDisable)
		{
			BVDC_P_SubRul_DropOffVnet_isr(&(hWindow->stWinOutVnet), pList);

			/* win sur will not be disabled automatically per vsync */
#if BVDC_P_SUPPORT_WIN_CONST_COLOR
			BVDC_P_Window_WinOutSetEnable_isr(hWindow, pPicture->ucAlpha, false);
#else
			BVDC_P_Window_WinOutSetEnable_isr(hWindow, 0, false);
#endif
			BVDC_P_WIN_WRITE_TO_RUL(CMP_0_V0_SURFACE_CTRL, pList->pulCurrent);
		}
		else if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
		{
			/* save current drain mux value */
			BVDC_P_SubRul_StartDrain_isr(&(hWindow->stWinOutVnet), pList, 0, 0 /* 0 means no need to reset */);

			/* enabling-surface and then disabling-surface cause constant color flash */

			/* drain the free-channel used to to patch the vnet. restore
			 * the current drain mux value */
			BVDC_P_SubRul_FinishDrain_isr(&(hWindow->stWinOutVnet), pList, 0, 0, 0, 0);
		}
	}
}

/***************************************************************************
 * {private}
 * note: a sub-module might be shared by more than one window, but the
 * sharing is handled by each sub-module. And each sub-module will
 * acquire and release the pre-patch of free-channel or loop-back
 * internally basing on eVnetPatchMode.
 *
 * return: if reader is on
 */
static bool BVDC_P_Window_BuildReaderRul_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eNextFieldId,
	  bool                             bBuildCanvasCtrl )
{
	BVDC_Compositor_Handle hCompositor;
	BVDC_P_PicComRulInfo *pPicComRulInfo;
	BVDC_P_State  eReaderState;
	BVDC_P_PictureNode  *pPicture;

	BDBG_ENTER(BVDC_P_Window_BuildReaderRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	hCompositor = hWindow->hCompositor;
	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	BDBG_OBJECT_ASSERT(hCompositor->hDisplay, BVDC_DSP);

	/* Note: When building RUL, hardware highly recommend that we build from
	 * backend to the frontend, that is starting with:
	 *   VEC, CMP, SCL, VFD, CAP, MFD.  This to prevent the false start of
	 * downstream modules.
	 * Note: In the case readerState is not eActive, we still needs to build
	 * RUL in order to shut down. VnetMode is the only one that indicates if
	 * we need to build RUL */
	if(BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId))
	{
		if((BVDC_P_State_eInactive == hWindow->stCurInfo.eReaderState) &&
		   (hWindow->stCurInfo.bVisible))
		{
			hWindow->stCurInfo.eReaderState = BVDC_P_State_eActive;
			hWindow->stCurInfo.eWriterState = BVDC_P_State_eActive;
		}

		eReaderState = hWindow->stCurInfo.eReaderState;

		/* VEC alignment may be too fast to sustain gfx RTS; mute it! */
		if((BVDC_P_State_eActive == eReaderState) &&
		   ((hCompositor->hDisplay->bAlignAdjusting && !hCompositor->hDisplay->stCurInfo.stAlignCfg.bKeepBvnConnected)||

			(0==hWindow->stCurInfo.hSource->hGfxFeeder->stGfxSurface.stCurSurInfo.ulAddress) || /* no valid sur */

			(!hWindow->stCurInfo.bVisible)))  /* muted by user */
		{
			eReaderState = BVDC_P_State_eShutDownRul;
		}

		/* according to readerState, enable or disable gfx surface in cmp,
		 * and build rul for GFD */
		if (BVDC_P_State_eActive != eReaderState)
		{
			BVDC_P_WIN_WRITE_IMM_TO_RUL(CMP_0_V0_SURFACE_CTRL, 0, pList->pulCurrent);
		}
		else
		{
			BVDC_P_WIN_GET_REG_DATA(CMP_0_V0_SURFACE_CTRL) |=  (
				BCHP_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 1));
			BVDC_P_WIN_WRITE_TO_RUL(CMP_0_V0_SURFACE_CTRL, pList->pulCurrent);
		}

		if (hWindow->stCurInfo.hSource->hGfxFeeder->stGfxSurface.stCurSurInfo.ulAddress)
		{
			BVDC_P_GfxFeeder_BuildRul_isr(hWindow->stCurInfo.hSource->hGfxFeeder,
				&(hWindow->stCurInfo.hSource->stCurInfo), pList, eNextFieldId, eReaderState);
		}
	}
	else /* video window */
	{
		eReaderState = hWindow->stCurInfo.eReaderState;

		if (BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
		{
			pPicture = &hWindow->stCurInfo.hSource->hVfdFeeder->stPicture;
			pPicComRulInfo = &pPicture->PicComRulInfo;
		}
		else
		{
			pPicture = BVDC_P_Buffer_GetCurrReaderNode_isr(hWindow->hBuffer);
			pPicComRulInfo = &((BVDC_P_Buffer_GetCurrReaderNode_isr(hWindow->hBuffer))->PicComRulInfo);
		}
		pPicComRulInfo->eWin = hWindow->eId;

#if (BVDC_P_SHOW_VNET_MSG==1)
#if BVDC_P_SUPPORT_WIN_CONST_COLOR
		if(pPicture->stFlags.bMute !=
		   (BVDC_P_WIN_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, CONST_COLOR_ENABLE, 1) ||
			BVDC_P_WIN_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 0)))
#else
		if(pPicture->stFlags.bMute !=
		   BVDC_P_WIN_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 0))
#endif
		{
			if(!((!pPicture->stFlags.bMute) &&
				  ((BVDC_P_State_eShutDownRul == hWindow->stCurInfo.eReaderState) ||
				   (BVDC_P_State_eShutDown == hWindow->stCurInfo.eReaderState))))
			{
				BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] build reader: bPicMute != bSurDis, bMute %d, readerState %d, writerState %d",
					hWindow->eId, pPicture->stFlags.bMute? 1 : 0,
					hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
			}
		}
#endif

		/* Fast VEC alignment should mute the video window to avoid RTS failure */
		/* sur should ahve been enabled in reader_isr.   */
		if((eReaderState == BVDC_P_State_eActive) &&
		   ((pPicture->stFlags.bMute) ||
		    (!hWindow->stCurInfo.bVisible) ||
		    (hCompositor->hDisplay->bAlignAdjusting && !hCompositor->hDisplay->stCurInfo.stAlignCfg.bKeepBvnConnected) ||
		    (BVDC_P_VNET_IS_INVALID(hWindow->stVnetMode)) ||
		    (BVDC_P_State_eInactive == hWindow->stCurInfo.eWriterState) ||
		    (BVDC_P_WIN_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 0))))
		{
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] build reader: force down, bMute %d, bDsipAlign %d, bSurEn %d, vnetMode 0x%x,  writerState %d",
				hWindow->eId, pPicture->stFlags.bMute? 1 : 0, hCompositor->hDisplay->bAlignAdjusting? 1 : 0,
				BVDC_P_WIN_COMPARE_FIELD_DATA(CMP_0_V0_SURFACE_CTRL, SURFACE_ENABLE, 1)? 1 : 0,
				*(uint32_t *) &hWindow->stVnetMode, hWindow->stCurInfo.eWriterState));

			/* this will cause reader modules to drop off vnet and disable. If
			 * pic ummute again later without vnet-reconfigure, the reader
			 * modules will automatically re-join into vnet and enable.
			 * This is handled in subrul. */
			eReaderState = BVDC_P_State_eShutDownRul;

			if (BVDC_P_VNET_IS_INVALID(hWindow->stVnetMode))
			{
#if BVDC_P_SHOW_VNET_MSG
				BVDC_P_Window_DirtyBits  *pDirty = &(hWindow->stCurInfo.stDirty);

				BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] build reader RUL with NULL VnetMode, readerState %d, writerState %d",
					hWindow->eId, hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
				BDBG_MODULE_MSG(BVDC_WIN_VNET,("   bShutDown %d, bRecfgVnet %d, bSrcPending %d, dirty 0x%08lx",
					pDirty->stBits.bShutdown, pDirty->stBits.bReConfigVnet, pDirty->stBits.bSrcPending, BVDC_P_CAST_DIRTY(pDirty)));
#endif
				/* clean up to avoid to build rul for a module accidently */
				BVDC_P_Window_SetInvalidVnetMode(&(hWindow->stVnetMode));
			}
		}

#if (BVDC_P_SUPPORT_HIST)
		if(BVDC_P_VNET_USED_HIST_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Hist_BuildRul_isr(hWindow->stCurResource.hHist, pList, eReaderState, pPicComRulInfo);
		}
#endif

		BVDC_P_Window_WinOutBuildRul_isr(hWindow, pPicture, pList, eReaderState);
		if (bBuildCanvasCtrl)
		{
			BVDC_P_Compositor_BuildConvasCtrlRul_isr(hWindow->hCompositor, pList);
		}

#if (BVDC_P_SUPPORT_FGT)
		if(BVDC_P_VNET_USED_FGT_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Fgt_BuildRul_isr(hWindow->stCurResource.hFgt, pList, eReaderState, pPicComRulInfo);
		}
#endif

		if(BVDC_P_VNET_USED_SCALER_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Scaler_BuildRul_isr(hWindow->stCurResource.hScaler, pList, eReaderState, pPicComRulInfo);
		}

#if (BVDC_P_SUPPORT_MCVP)
		/* note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
		 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
		if(BVDC_P_VNET_USED_MCVP_AT_READER(hWindow->stVnetMode))
		{
#if BVDC_P_STG_RUL_DELAY_WORKAROUND
			BVDC_P_STG_DelayRUL_isr (hWindow->hCompositor->hDisplay, pList);
#endif
			BVDC_P_Mcvp_BuildRul_isr(hWindow->stCurResource.hMcvp, pList, eReaderState, hWindow, pPicture);
		}
#endif

#if (BVDC_P_SUPPORT_MAD)
		if(BVDC_P_VNET_USED_MAD_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Mad_BuildRul_isr(hWindow->stCurResource.hMad32, pList, eReaderState, pPicComRulInfo);
		}
#endif

#if (BVDC_P_SUPPORT_ANR)
		if(BVDC_P_VNET_USED_ANR_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Anr_BuildRul_isr(hWindow->stCurResource.hAnr, pList, eReaderState, pPicture);
		}
#endif

#if (BVDC_P_SUPPORT_HSCL)
		if(BVDC_P_VNET_USED_HSCL_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Hscaler_BuildRul_isr(hWindow->stCurResource.hHscaler, pList, eReaderState, pPicComRulInfo);
		}
#endif

#if (BVDC_P_SUPPORT_DNR)
		/* DNR is always upstream of ANR */
		if(BVDC_P_VNET_USED_DNR_AT_READER(hWindow->stVnetMode))
		{
			BVDC_P_Dnr_BuildRul_isr(hWindow->stCurResource.hDnr, pList, eReaderState, pPicComRulInfo);

		}
#endif

		if(BVDC_P_VNET_USED_PLAYBACK(hWindow->stVnetMode))
		{
			/* 656/dvo master mode fixed.  Don't start the feeder if dtg
			 * is disabled on the bypass.  Or another word only start vfd
			 * if non-bypass, or bypass w/ 656 or dvo out enable. */
			if((!hCompositor->hDisplay->bIsBypass) ||
			   (hCompositor->hDisplay->stCurInfo.bEnable656) ||
			   (hCompositor->hDisplay->stCurInfo.bEnableHdmi))
			{
				BVDC_P_Feeder_BuildRul_isr(hWindow->stCurResource.hPlayback, pList, eReaderState, pPicComRulInfo);
			}
		}

		else if(BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
		{
			BVDC_P_Feeder_BuildRul_isr(hWindow->stCurInfo.hSource->hVfdFeeder, pList, eReaderState, pPicComRulInfo);
		}
	}

	BDBG_LEAVE(BVDC_P_Window_BuildReaderRul_isr);
	return (BVDC_P_State_eActive == eReaderState);
}


static void BVDC_P_Window_BuildWriterRul_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  bool                             bReaderOn )
{
	BVDC_P_PictureNode  *pPicture;
	BVDC_P_PicComRulInfo *pPicComRulInfo;
	BVDC_P_State  eWriterState;

	BDBG_ENTER(BVDC_P_Window_BuildWriterRul_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);

	eWriterState = hWindow->stCurInfo.eWriterState;
	if (BVDC_P_SRC_IS_VFD(hWindow->stCurInfo.hSource->eId))
	{
		pPicture = &hWindow->stCurInfo.hSource->hVfdFeeder->stPicture;
		pPicComRulInfo = &hWindow->stCurInfo.hSource->hVfdFeeder->stPicture.PicComRulInfo;
	}
	else
	{
		pPicture = BVDC_P_Buffer_GetCurrWriterNode_isr(hWindow->hBuffer);
		pPicComRulInfo = &(pPicture->PicComRulInfo);
	}
	pPicComRulInfo->eWin = hWindow->eId;

	/* Note: When building RUL, hardware highly recommend that we build from
	 * backend to the frontend, that is starting with:
	 *   VEC, CMP, SCL, VFD, CAP, MFD.  This to prevent the false start of
	 * downstream modules.
	 * Note: In the case writerState is not eActive, we still needs to build
	 * RUL in order to shut down. VnetMode is the only one that indicates if
	 * we need to build RUL */
	if(!BVDC_P_VNET_IS_INVALID(hWindow->stVnetMode))
	{
		/* src is not feeding, must have lost signal, we need to tear off writer vnet
		 * right away so that src can have a free run and get locked again. The loss
		 * of signal will cause vnet reconfigure later. In case that src start to feed
		 * again without vnet-reconfigure, the writer modules would also automatically
		 * re-join into vnet and enable. This is handled in subrul.
		 * we also let writer vnet tear-off as soon as shut-down process starts, in
		 * case another win is sharing the src */
		if((BVDC_P_State_eActive == eWriterState) &&
		   ((!hWindow->stCurInfo.hSource->bStartFeed) || /* likely vdec / 656 / hdmi src lost */
			((!BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode)) && (!bReaderOn)))) /* syncLocked, reader down */
		{
			eWriterState = BVDC_P_State_eShutDownRul;
			BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] build writer: force down, srcFeed %d, byReader %d, rdState %d, wrtState %d",
				hWindow->eId, hWindow->stCurInfo.hSource->bStartFeed,
				(!BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode)) && (!bReaderOn),
				hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
		}

		/* back -> front for both enabling and disabling (due to new drain alg) */
		if(BVDC_P_VNET_USED_CAPTURE(hWindow->stVnetMode))
		{
			BVDC_P_Capture_BuildRul_isr(hWindow->stCurResource.hCapture, pList, eWriterState, pPicComRulInfo);
		}

#if (BVDC_P_SUPPORT_FGT)
		if(BVDC_P_VNET_USED_FGT_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Fgt_BuildRul_isr(hWindow->stCurResource.hFgt, pList, eWriterState, pPicComRulInfo);
		}
#endif

		if(BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Scaler_BuildRul_isr(hWindow->stCurResource.hScaler, pList, eWriterState, pPicComRulInfo);
		}

#if (BVDC_P_SUPPORT_MCVP)
		/* note: with 7420, BVDC_P_SUPPORT_HSCL/ANR/MAD is 0, and
		 * BVDC_P_VNET_USED_HSCL/ANR/MAD(hWindow->stVnetMode) is false too */
		if(BVDC_P_VNET_USED_MCVP_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Mcvp_BuildRul_isr(hWindow->stCurResource.hMcvp, pList, eWriterState, hWindow, pPicture);
		}
#endif

#if (BVDC_P_SUPPORT_MAD)
		if(BVDC_P_VNET_USED_MAD_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Mad_BuildRul_isr(hWindow->stCurResource.hMad32, pList, eWriterState, pPicComRulInfo);
		}
#endif

#if (BVDC_P_SUPPORT_ANR)
		if(BVDC_P_VNET_USED_ANR_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Anr_BuildRul_isr(hWindow->stCurResource.hAnr, pList, eWriterState, pPicture);
		}
#endif

#if (BVDC_P_SUPPORT_HSCL)
		if(BVDC_P_VNET_USED_HSCL_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Hscaler_BuildRul_isr(hWindow->stCurResource.hHscaler, pList, eWriterState, pPicComRulInfo);
		}
#endif

#if (BVDC_P_SUPPORT_DNR)
		/* DNR is always upstream of ANR */
		if(BVDC_P_VNET_USED_DNR_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Dnr_BuildRul_isr(hWindow->stCurResource.hDnr, pList, eWriterState, pPicComRulInfo);
		}
#endif

#if (BVDC_P_SUPPORT_BOX_DETECT)
		if(hWindow->stCurResource.hBoxDetect)
		{
			BVDC_P_BoxDetect_BuildRul_isr(&(hWindow->stCurResource.hBoxDetect), pList, eWriterState,
				pPicComRulInfo, pPicture->pSrcOut, hWindow->stCurInfo.bBoxDetect);
			hWindow->stCurInfo.stDirty.stBits.bBoxDetect = 0;
		}
#endif

#if (BVDC_P_SUPPORT_HIST)
		if(BVDC_P_VNET_USED_HIST_AT_WRITER(hWindow->stVnetMode))
		{
			BVDC_P_Hist_BuildRul_isr(hWindow->stCurResource.hHist, pList, eWriterState, pPicComRulInfo);
		}
#endif

		if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
		{
			BVDC_P_Feeder_BuildRul_isr(hWindow->stCurInfo.hSource->hMpegFeeder, pList, eWriterState, pPicComRulInfo);
		}
	}
#if BVDC_P_SHOW_VNET_MSG
	else
	{
		BVDC_P_Window_DirtyBits  *pDirty = &(hWindow->stCurInfo.stDirty);
		BDBG_MODULE_MSG(BVDC_WIN_VNET,("win[%d] build writer RUL with NULL VnetMode, readerState %d, writerState %d",
			hWindow->eId, hWindow->stCurInfo.eReaderState, hWindow->stCurInfo.eWriterState));
		BDBG_MODULE_MSG(BVDC_WIN_VNET,("   bShutDown %d, bRecfgVnet %d, bSrcPending %d, dirty 0x%08lx",
			pDirty->stBits.bShutdown, pDirty->stBits.bReConfigVnet, pDirty->stBits.bSrcPending, BVDC_P_CAST_DIRTY(pDirty)));
	}
#endif

	BDBG_LEAVE(BVDC_P_Window_BuildWriterRul_isr);
	return;
}


/***************************************************************************
 * {private}
 * note: a sub-module might be shared by more than one window, but the
 * sharing is handled by each sub-module. And each sub-module will
 * acquire and release the pre-patch of free-channel or loop-back
 * internally basing on eVnetPatchMode.
 */
void BVDC_P_Window_BuildRul_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eNextFieldId,
	  bool                             bBuildWriter,
	  bool                             bBuildReader,
	  bool                             bBuildCanvasCtrl )
{
	BDBG_ENTER(BVDC_P_Window_BuildRul_isr);

	/* Note: When building RUL, hardware highly recommend that we build from
	 * backend to the frontend, that is starting with:
	 *   VEC, CMP, SCL, VFD, CAP, MFD.  This to prevent the false start of
	 * downstream modules. */
	/* TODO: in the case of sync mpeg display, we might want to call
	 * BVDC_P_Window_BuildReaderRul_isr and BVDC_P_Window_BuildWriterRul_isr
	 * twice in the same vsync/RUL, shut-down old vnet the 1st time and build
	 * new vnet the the 2nd time. If we do so, remember to modify subrul to
	 * store the last released patch mux addr/mode for RUL loss handling */
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	/*BDBG_MSG(("Build Rul for Window[%d], Reader %d, Writer %d, nextFld %d, List 0x%x",
	   hWindow->eId, bBuildReader, bBuildWriter, eNextFieldId, pList));*/
	if((hWindow->stCurInfo.stDirty.stBits.bShutdown) ||
	   (BVDC_P_WIN_IS_GFX_WINDOW(hWindow->eId)))
	{
		if(bBuildReader)
		{
			BVDC_P_Window_BuildReaderRul_isr(hWindow, pList, eNextFieldId, bBuildCanvasCtrl);
		}
		if(bBuildWriter)
		{
			BVDC_P_Window_BuildWriterRul_isr(hWindow, pList, false);
		}
	}
	else
	{
		bool  bReaderOn = true; /* default to true for sync slip case */

		if(bBuildReader)
		{
			BVDC_P_Window_AdjustRdState_isr(hWindow, NULL);
			bReaderOn = BVDC_P_Window_BuildReaderRul_isr(hWindow, pList, eNextFieldId, bBuildCanvasCtrl);

			/* PsF: mark the chopped RUL size! */
			if(hWindow->bSyncLockSrc && hWindow->stCurInfo.hSource->bPsfScanout)
			{
				pList->ulPsfMark = (uint32_t)(pList->pulCurrent - pList->pulStart);
			}
		}

		if(bBuildWriter)
		{
			BVDC_P_Window_BuildWriterRul_isr(hWindow, pList, bReaderOn);
		}
	}

	BDBG_LEAVE(BVDC_P_Window_BuildRul_isr);
	return;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Window_CapturePicture_isr
	( BVDC_Window_Handle               hWindow,
	  BVDC_P_Window_CapturedPicture   *pCapturedPic )
{
	BERR_Code eRet = BERR_SUCCESS;
	unsigned int          uiPitch;

	BDBG_ENTER(BVDC_P_Window_CapturePicture_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_ASSERT(pCapturedPic);

	/* Clear content */
	BKNI_Memset((void*)pCapturedPic, 0x0, sizeof(BVDC_P_Window_CapturedPicture));

	if(hWindow->uiAvailCaptureBuffers)
	{

		/* Mark the buffer as used by user */
		if((eRet = BVDC_P_Buffer_ExtractBuffer_isr(hWindow->hBuffer, &pCapturedPic->pPicture)))
		{
			pCapturedPic->pvBufAddr   = NULL;
			pCapturedPic->pvBufAddr_R = NULL;
			return eRet;
		}

		/* Decrement number of capture buffers used */
		hWindow->uiAvailCaptureBuffers--;

		/* Give the virtual address to convert a picture to a surface. */
		pCapturedPic->pvBufAddr = pCapturedPic->pPicture->pHeapNode->pvBufAddr;

#if (BVDC_P_SUPPORT_3D_VIDEO)
		if(pCapturedPic->pPicture->pHeapNode_R != NULL)
			pCapturedPic->pvBufAddr_R  = pCapturedPic->pPicture->pHeapNode_R->pvBufAddr;
		pCapturedPic->eDispOrientation = pCapturedPic->pPicture->eDispOrientation;
#endif

		/* Get polarity */
		pCapturedPic->ePolarity = (BVDC_P_VNET_USED_SCALER_AT_WRITER(pCapturedPic->pPicture->stVnetMode)
				? pCapturedPic->pPicture->eDstPolarity : pCapturedPic->pPicture->eSrcPolarity);

		/* Get Pixel Format */
		pCapturedPic->ePxlFmt = pCapturedPic->pPicture->ePixelFormat;

		/* Get Height */
		if(pCapturedPic->ePolarity != BAVC_Polarity_eFrame)
		{
			pCapturedPic->ulHeight = pCapturedPic->pPicture->pVfdIn->ulHeight/2;
		}
		else
		{
			pCapturedPic->ulHeight = pCapturedPic->pPicture->pVfdIn->ulHeight;
		}

		/* Get width */
		pCapturedPic->ulWidth = pCapturedPic->pPicture->pVfdIn->ulWidth;

		/* Get pitch. See ulPitch in BVDC_P_Capture_SetEnable_isr and ulStride in
		   BVDC_P_Feeder_SetPlaybackInfo_isr */
		BPXL_GetBytesPerNPixels(pCapturedPic->ePxlFmt,
			pCapturedPic->pPicture->pVfdIn->ulWidth, &uiPitch);
		pCapturedPic->ulPitch = BVDC_P_ALIGN_UP(uiPitch, BVDC_P_PITCH_ALIGN);
	}
	else
	{
		pCapturedPic->pvBufAddr   = NULL;
		pCapturedPic->pvBufAddr_R = NULL;
		eRet = BVDC_ERR_NO_AVAIL_CAPTURE_BUFFER;
	}

	BDBG_LEAVE(BVDC_P_Window_CapturePicture_isr);
	return eRet;
}


/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Window_ReleasePicture_isr
	( BVDC_Window_Handle               hWindow,
	  BSUR_Surface_Handle              hCaptureBuffer )
{
	BVDC_P_PictureNode *pPicture;
	uint32_t cnt = 0;

	BDBG_ENTER(BVDC_P_Window_ReleasePicture_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	pPicture = hWindow->hBuffer->pCurReaderBuf;

	BDBG_MSG(("Returning surface buffer 0x%x", hCaptureBuffer));

	while ((hCaptureBuffer != pPicture->hSurface) &&
			(cnt < BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT))
	{
		BVDC_P_Buffer_GetNextUsedByUserNode(pPicture, pPicture);
		cnt++;

		BDBG_MSG(("Surface buffer 0x%x; Is Used? %d", pPicture->hSurface, pPicture->stFlags.bUsedByUser));
	}

	if(cnt > BVDC_P_MAX_USER_CAPTURE_BUFFER_COUNT)
		return BERR_TRACE(BVDC_ERR_CAPTURED_BUFFER_NOT_FOUND);
	else
	{
		hWindow->stCurInfo.pBufferFromUser = pPicture;
		hWindow->stCurInfo.stDirty.stBits.bUserReleaseBuffer = BVDC_P_DIRTY;
	}

	BDBG_LEAVE(BVDC_P_Window_ReleasePicture_isr);
	return BERR_SUCCESS;
}



/***************************************************************************
 * {private}
 *
 */
bool BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_PictureNode *pPicture;
	bool bBufferHeldByUser = false;
	uint32_t cnt = 0;

	BDBG_ENTER(BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);

	if(hWindow->hBuffer)
	{
		pPicture = hWindow->hBuffer->pCurReaderBuf;
		if(pPicture)
		{
			while (!pPicture->stFlags.bUsedByUser && (cnt < hWindow->hBuffer->ulActiveBufCnt))
			{
				BVDC_P_Buffer_GetNextActiveNode(pPicture, pPicture);
				cnt++;
			}

			if(cnt >= hWindow->hBuffer->ulActiveBufCnt)
				bBufferHeldByUser = false;
			else
			{
				bBufferHeldByUser = true;
			}
		}
	}

	BDBG_LEAVE(BVDC_P_Window_CheckForUnReturnedUserCapturedBuffer_isr);
	return bBufferHeldByUser;
}


/***************************************************************************
 *
 */
void BVDC_P_Window_CalculateCsc_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_Info *pCurInfo;
	const BVDC_P_DisplayInfo *pDisInfo;

	BVDC_P_CscCoeffs *pPriCsc;
	BVDC_P_CscCoeffs *pSecCsc;
	BVDC_P_CscCoeffs *pRGBToYCbCr;
	BVDC_P_CscCoeffs *pYCbCrToRGB;

	BDBG_ENTER(BVDC_P_Window_CalculateCsc_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	pCurInfo = &hWindow->stCurInfo;

	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	pDisInfo = &hWindow->hCompositor->hDisplay->stCurInfo;

	BDBG_MSG(("Window %d: Calculate CSC", hWindow->eId));

#if BVDC_P_SUPPORT_CMP_NONLINEAR_CSC_WIN
	if(BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId))
	{
		pPriCsc = &hWindow->stPriCscC;
		pSecCsc = &hWindow->stSecCscC;

	}
	else
#endif
	{
		pPriCsc = &hWindow->stPriCsc;
		pSecCsc = &hWindow->stSecCsc;
	}

	/* use user specified Matrix C as RGB->YCbCr matrix and
	inverse of Matrix C as YCbCr->RGB matrix. */

	/* user Matrix C now required when using user matrices even in platforms
	without non-linear support */
	if (pCurInfo->bUserCscC && (BVDC_P_WIN_IS_NONLINEAR_CSC_WIN(hWindow->eId) || pCurInfo->bUserCsc))
	{
		BDBG_MSG(("Using User WIN CSC C for RGBToYCbCr Matrix"));
		BDBG_MSG(("Using Inverse User WIN CSC C for YCbCrToRGB Matrix"));

		pRGBToYCbCr = pPriCsc; /* same as pSecCsc here */
		pYCbCrToRGB = &hWindow->stInvUserCscC; /* inverse of user Matrix C is YCbCr->RGB */
	}
	else
	{
		BFMT_VideoFmt eVideoFmt = pDisInfo->pFmtInfo->eVideoFmt;

		/* Matrix C is RGB->YCbCr and Matrix A is YCbCr->RGB if we set input
		   and output colorspaces to be the display's and call the get CSC function. */
		BAVC_MatrixCoefficients eInputColorSpace =
			(VIDEO_FORMAT_IS_SD(eVideoFmt) || VIDEO_FORMAT_IS_ED(eVideoFmt))
				? BAVC_MatrixCoefficients_eSmpte_170M : BAVC_MatrixCoefficients_eItu_R_BT_709;
		BVDC_P_CmpColorSpace eOutputcolorSpace = pDisInfo->eCmpColorSpace;

		if (pCurInfo->bUserCscC)
		{
			BDBG_WRN(("User WIN CSC Matrix C set without WIN CSC Main Matrix being set."));
			BDBG_WRN(("User WIN CSC Matrix C will be ignored in Color Temp/RGB attenuation."));
		}

		BDBG_MSG(("Using Display Format's CSCs for YCbCrToRGB and RGBToYCbCr Matrix"));

		BVDC_P_Compositor_GetCscTablesNonLinear_isr(&pYCbCrToRGB, NULL, &pRGBToYCbCr, eInputColorSpace,
			                                        eOutputcolorSpace, false);
	}

	/* apply adjustments to primary color matrix */
	if(pCurInfo->stSplitScreenSetting.eContrast == BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyContrast_isr(hWindow->sFinalContrast, pPriCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eBrightness == BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyBrightness_isr(hWindow->sFinalBrightness, pPriCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eHue == BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplySaturationAndHue_isr(hWindow->sFinalSaturation,
		                                 pCurInfo->sHue,
		                                 pPriCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eColorTemp == BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyAttenuationRGB_isr(pCurInfo->lAttenuationR,
										   pCurInfo->lAttenuationG,
										   pCurInfo->lAttenuationB,
										   pCurInfo->lOffsetR,
										   pCurInfo->lOffsetG,
										   pCurInfo->lOffsetB,
										   pPriCsc,
										   pYCbCrToRGB,
			pRGBToYCbCr,
			pCurInfo->bUserCsc,
			pCurInfo->bUserCscC);
	}

	/* apply adjustment to secondary color matrix */
	if(pCurInfo->stSplitScreenSetting.eContrast != BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyContrast_isr(hWindow->sFinalContrast, pSecCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eBrightness != BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyBrightness_isr(hWindow->sFinalBrightness, pSecCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eHue != BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplySaturationAndHue_isr(hWindow->sFinalSaturation,
		                                 pCurInfo->sHue,
		                                 pSecCsc);
	}
	if(pCurInfo->stSplitScreenSetting.eColorTemp != BVDC_SplitScreenMode_eDisable)
	{
		BVDC_P_Csc_ApplyAttenuationRGB_isr(pCurInfo->lAttenuationR,
										   pCurInfo->lAttenuationG,
										   pCurInfo->lAttenuationB,
										   pCurInfo->lOffsetR,
										   pCurInfo->lOffsetG,
										   pCurInfo->lOffsetB,
										   pSecCsc,
										   pYCbCrToRGB,
			pRGBToYCbCr,
			pCurInfo->bUserCsc,
			pCurInfo->bUserCscC);
	}

	BDBG_LEAVE(BVDC_P_Window_CalculateCsc_isr);
	return;
}

/***************************************************************************
*
*/
BERR_Code BVDC_P_Window_SetMcvp_DeinterlaceConfiguration
	(BVDC_Window_Handle               hWindow,
	 bool                             bDeinterlace,
	 const BVDC_Deinterlace_Settings *pMadSettings)
{
#if (0 != BVDC_P_SUPPORT_MCVP)
	BVDC_P_Deinterlace_Settings *pNewMad;

	pNewMad = &hWindow->stNewInfo.stMadSettings;
	hWindow->stNewInfo.bDeinterlace = bDeinterlace;

	switch (pMadSettings->eGameMode)
	{
		case (BVDC_MadGameMode_eOff):
		case (BVDC_MadGameMode_e4Fields_0Delay):
		case (BVDC_MadGameMode_e4Fields_1Delay):
		case (BVDC_MadGameMode_e4Fields_2Delay):
		case (BVDC_MadGameMode_e4Fields_ForceSpatial):
			pNewMad->eGameMode = pMadSettings->eGameMode;
			break;
		default:
			BDBG_ERR(("This chipset doesn't support MAD Game Mode other than 4 fields 0, 1, 2 Delay!"));
			return BERR_TRACE(BVDC_ERR_MCVP_NOT_SUPPORTED);
	}
#else
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(bDeinterlace);
	BSTD_UNUSED(pMadSettings);
#endif

	return (BERR_SUCCESS);
}


/***************************************************************************
*
*/
BERR_Code BVDC_P_Window_SetMad_DeinterlaceConfiguration
	(BVDC_Window_Handle               hWindow,
	 bool                             bDeinterlace,
	 const BVDC_Deinterlace_Settings *pMadSettings)
{
#if (0!=BVDC_P_SUPPORT_MAD)

	BVDC_P_Deinterlace_Settings *pNewMad;
	pNewMad = &hWindow->stNewInfo.stMadSettings;

	/* set new value */
	hWindow->stNewInfo.bDeinterlace = bDeinterlace;

	/* only set new 3:2 and 2:2 pulldown setting if Mad is enable, */
	/* otherwise ignore the setting, use default setting instead   */
	if(bDeinterlace)
	{
#if (5>BVDC_P_SUPPORT_MAD_VER)
		if((pMadSettings->eGameMode == BVDC_MadGameMode_e3Fields_1Delay) ||
			(pMadSettings->eGameMode == BVDC_MadGameMode_e3Fields_0Delay) ||
			(pMadSettings->eGameMode == BVDC_MadGameMode_e3Fields_ForceSpatial))
		{
			BDBG_ERR(("This chipset doesn't support 3-Field MAD Game Mode!"));
			return BERR_TRACE(BVDC_ERR_MAD_NOT_SUPPORTED);
		}
#endif
		if((pMadSettings->eGameMode >= BVDC_MadGameMode_eMaxCount)       ||
			(pMadSettings->eGameMode == BVDC_MadGameMode_e5Fields_2Delay)||
			(pMadSettings->eGameMode == BVDC_MadGameMode_e4Fields_2Delay)||
			(pMadSettings->eGameMode == BVDC_MadGameMode_e3Fields_2Delay))
		{
			BDBG_ERR(("This MAD Game Mode is not supported!"));
			return BERR_TRACE(BVDC_ERR_MAD_NOT_SUPPORTED);
		}

#if (BVDC_P_SUPPORT_MAD_VER < 6)
		if(!BPXL_IS_YCbCr422_FORMAT(pMadSettings->ePxlFormat))
		{
			BDBG_MSG(("This chip will ignore MAD pixel format user setting!"));
			/* ignore user setting for now */
			pNewMad->ePixelFmt = BVDC_P_CAP_PIXEL_FORMAT_8BIT422;
		}
#else
		if(!BPXL_IS_YCbCr422_FORMAT(pMadSettings->ePxlFormat) &&
			!BPXL_IS_YCbCr422_10BIT_FORMAT(pMadSettings->ePxlFormat) &&
			!BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(pMadSettings->ePxlFormat))
		{
			BDBG_ERR(("This chip only supports 8-bit or 10-bit 4:2:2 MAD pixel format!"));
			return BERR_TRACE(BVDC_ERR_MAD_NOT_SUPPORTED);
		}
		pNewMad->ePixelFmt = pMadSettings->ePxlFormat;
#endif
	}
#else

	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(bDeinterlace);
	BSTD_UNUSED(pMadSettings);
#endif

	return (BERR_SUCCESS);
}


#if BVDC_P_SUPPORT_CMP_MOSAIC_CSC
/***************************************************************************
 *
 */
void BVDC_P_Window_CalculateMosaicCsc_isr
	( BVDC_Window_Handle               hWindow )
{
	BVDC_P_Window_Info *pCurInfo;
	const BVDC_P_DisplayInfo *pDisInfo;
	BVDC_P_CscCoeffs *pCsc, *pYCbCrToRGB, *pRGBToYCbCr;
	int i = 0;

	BDBG_ENTER(BVDC_P_Window_CalculateMosaicCsc_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);
	pCurInfo = &hWindow->stCurInfo;
	pDisInfo = &hWindow->hCompositor->hDisplay->stNewInfo;

	BDBG_MSG(("Window %d: Calculate Mosaic CSC", hWindow->eId));

	for (i = 0; i < BVDC_P_SUPPORT_CMP_MOSAIC_CSC; i++)
	{
		pCsc = &hWindow->astMosaicCscList[i];

		BVDC_P_Compositor_GetCscTablesNonLinear_isr(&pYCbCrToRGB, NULL,
			&pRGBToYCbCr, hWindow->aeMosaicCscToClrSpaceMap[i],
			hWindow->hCompositor->eCmpColorSpace, false);

		/* apply adjustments to mosaic color matrix */
		BVDC_P_Csc_ApplyContrast_isr(hWindow->sFinalContrast, pCsc);
		BVDC_P_Csc_ApplyBrightness_isr(hWindow->sFinalBrightness, pCsc);
		BVDC_P_Csc_ApplySaturationAndHue_isr(hWindow->sFinalSaturation,
			pCurInfo->sHue, pCsc);
		BVDC_P_Csc_ApplyAttenuationRGB_isr(pCurInfo->lAttenuationR,
										   pCurInfo->lAttenuationG,
										   pCurInfo->lAttenuationB,
										   pCurInfo->lOffsetR,
										   pCurInfo->lOffsetG,
										   pCurInfo->lOffsetB,
										   pCsc,
										   pYCbCrToRGB,
			pRGBToYCbCr,
			pCurInfo->bUserCsc,
			pCurInfo->bUserCscC);
	}

	BDBG_LEAVE(BVDC_P_Window_CalculateMosaicCsc_isr);
	return;
}
#endif

#if (BVDC_P_SUPPORT_VIDEO_TESTFEATURE1_MAD_ANR)
/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Window_SetMadAnrTestFeature1_isr
	( BVDC_Window_Handle                       hWindow,
	  BPXL_Format                              ePxlFormat,
	  const BVDC_P_Rect                       *pRect,
	  const BVDC_P_PictureNodePtr              pPicture,
	  const BVDC_Video_TestFeature1_Settings  *pUserVideoTestFeature1,
	  bool                                     bWriter,
	  BVDC_P_TestFeature1_Settings            *pNewTestFeature1 )
{
	bool        bSrcInterlaced;
	uint32_t    ulThroughput;
	uint32_t    ulSrcFmtRasterX, ulSrcFmtPixelClk;
	uint32_t    ulVecFmtRasterX, ulVecFmtPixelClk;
	const BFMT_VideoInfo           *pSrcFmtInfo, *pDstFmtInfo;

	BDBG_ENTER(BVDC_P_Window_SetMadAnrTestFeature1_isr);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	BDBG_OBJECT_ASSERT(hWindow->hCompositor, BVDC_CMP);

	/* Allow the source pending (rts) to happen first before setting the new
	 * settings. */
	if((hWindow->stCurInfo.hSource->stCurInfo.eResumeMode) &&
	   ((hWindow->stCurInfo.hSource->stNewInfo.stDirty.stBits.bInputFormat) &&
	    (hWindow->stCurInfo.hSource->bUserAppliedChanges)))
	{
		goto Done;
	}

	pNewTestFeature1->bEnable = pUserVideoTestFeature1->bEnable;
	pNewTestFeature1->ulBitsPerPixel = pUserVideoTestFeature1->ulBitsPerPixel;

	if(!pNewTestFeature1->bEnable)
		goto Done;

	if(BPXL_IS_YCbCr444_10BIT_FORMAT(ePxlFormat))
	{
		/* Data is 444 */
		pNewTestFeature1->ulPixelPerGroup = 2;
	}
	else
	{
		/* Data is 422 */
		pNewTestFeature1->ulPixelPerGroup = 4;
	}
	pNewTestFeature1->ulBitsPerGroup = (pNewTestFeature1->ulBitsPerPixel *
		pNewTestFeature1->ulPixelPerGroup) >> BVDC_P_TestFeature1_FRACTIONAL_SHIFT;

	pSrcFmtInfo = hWindow->stCurInfo.hSource->stCurInfo.pFmtInfo;
	pDstFmtInfo = hWindow->hCompositor->stCurInfo.pFmtInfo;
	bSrcInterlaced = hWindow->stCurInfo.hSource->bSrcInterlaced;

	/* TODO: Use data in window directly when it's available */
	ulSrcFmtPixelClk = pSrcFmtInfo->ulPxlFreq;
	ulVecFmtPixelClk = pDstFmtInfo->ulPxlFreq;
	ulSrcFmtRasterX  = pSrcFmtInfo->ulScanWidth;
	ulVecFmtRasterX  = pDstFmtInfo->ulScanWidth;

	if(bWriter)
	{
		/* MAD/ANR is in writer
		 * SRC -> HSCL -> ANR/MAD -> SCL -> CAP -> VFD -> VEC
		 * SRC is memory to memory, DST is real time
		 */
		if(BVDC_P_SRC_IS_MPEG(hWindow->stCurInfo.hSource->eId))
		{
			ulThroughput = (pRect->ulWidth * (pRect->ulHeight >> bSrcInterlaced)
				* (hWindow->stCurInfo.hSource->ulVertFreq / BFMT_FREQ_FACTOR))/ 1000000;
		}
		else
		{
			ulThroughput =
				(pRect->ulWidth * (pRect->ulHeight >> bSrcInterlaced) *
				(ulSrcFmtPixelClk/BFMT_FREQ_FACTOR)) / (ulSrcFmtRasterX *
				(pPicture->pSrcOut->ulHeight >> pDstFmtInfo->bInterlaced));
		}
	}
	else
	{
		/* MAD/ANR is in reader
		 * SRC -> CAP -> VFD -> HSCL -> ANR/MAD -> SCL -> VEC
		 */
		ulThroughput =
			(pRect->ulWidth * (pPicture->pVfdOut->ulHeight >> bSrcInterlaced) *
			(ulVecFmtPixelClk/BFMT_FREQ_FACTOR)) / (ulVecFmtRasterX *
			(pPicture->pSclOut->ulHeight >> pDstFmtInfo->bInterlaced));
	}

	if(ulThroughput > (BVDC_P_TestFeature1_THRESHOLD/BFMT_FREQ_FACTOR))
	{
		pNewTestFeature1->ulPredictionMode = 1;
	}
	else
	{
		pNewTestFeature1->ulPredictionMode = 0;
	}

Done:
	BDBG_LEAVE(BVDC_P_Window_SetMadAnrTestFeature1_isr);
	return BERR_SUCCESS;
}

#endif
/* End of file. */
