/**
 * cmd_hwtest_parser.c
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note
 * @brief
 * @file 			cmd_hwtest_parser.c
*/

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#endif
#include "taskdef.h"

#include "cmdtool.h"
#include "unittest.h"
#include "cmd_hwtest_parser_util.h"
#include "cmd_hwtest_parser_av.h"
#if defined(CONFIG_AUDIO_DECODER)
#include "cmd_hwtest_parser_audio.h"
#endif
#if defined(CONFIG_SCART)
#include "cmd_hwtest_parser_scart.h"
#endif
#include "cmd_hwtest_parser_hdmi.h"
#include "cmd_hwtest_parser_system.h"
#include "cmd_hwtest_parser_ext.h"
#include "cmd_hwtest_parser_channel.h"
#if defined(CONFIG_HWTEST_EEPROM)
#include "cmd_hwtest_parser_eeprom.h"
#endif
#include "cmd_hwtest_parser_nvram.h"
#if defined(CONFIG_HWTEST_FLASH)
#include "cmd_hwtest_parser_flash.h"
#endif
#include "cmd_swtest_parser_vkernel.h"
#include "cmd_swtest_parser_modules.h"
#include "cmd_hwtest_parser_demux.h"
#include "cmd_hwtest_parser_pvr.h"
#include "cmd_hwtest_parser_usb.h"
#if defined (CONFIG_SMARTCARD) || defined(CONFIG_SMARTCARD_SLOT1)
#include "cmd_hwtest_parser_sc.h"
#endif
#if defined (CONFIG_DSC)
#include "cmd_hwtest_parser_dsc.h"
#endif

#if defined(CONFIG_CABLE_MODEM)
#include "cmd_hwtest_parser_cm.h"
#endif

#include "cmd_hwtest_parser_ip.h"

#include "cmd_hwtest_parser_fs.h"
#include "cmd_hwtest_parser_nandflash.h"
#if defined(CONFIG_EMMC_FLASH)
#include "cmd_hwtest_parser_emmcflash.h"
#endif
#include "cmd_hwtest_parser_vkernel.h"

#if defined(CONFIG_MEDIA20)
#include "cmd_hwtest_parser_media.h"
#endif

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#if defined(CONFIG_CALYPSO)
#include "cmd_hwtest_parser_calypso.h"
#endif
#endif

#if defined(CONFIG_RF4CE)
#include "cmd_hwtest_parser_rf4ce.h"
#endif

#if defined(CONFIG_DINETWORK)
#if defined(CONFIG_WIFI) || defined(CONFIG_ETHERNET) || defined(CONFIG_PPPOE)
#include "cmd_hwtest_parser_network.h"
#endif
#endif

#if defined(CONFIG_CAS)
#if defined(CONFIG_CAS_NA)
#include "cmd_hwtest_parser_ota.h"
#endif
#endif

#if defined(CONFIG_TRANSCODER)
#include "cmd_hwtest_parser_transcoder.h"
#endif

#if defined(CONFIG_MMC)
#if !defined(CONFIG_DI_MODULE_MMC_SHARED_LIB)
#include "cmd_hwtest_parser_mmc.h"
#elif defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#include "cmd_hwtest_parser_mmc.h"
#endif
#endif

#if defined(CONFIG_MEDIA20)
#include "cmd_hwtest_parser_demo.h"
#endif

void CMD_HwTest_Parser_Init(void)
{
///////////////////////////////////////////////////////////////////////
/////				AV
///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_SCART)
	// SCART
	CMD_RegisterWord(CMD_HwTest_Parser_Scart,
	/* keyword */	(char *)"scart",
	/* help */		(char *)"ar-scart aspect ratio control (Slow Blanking control), tv_vid-tv scart video format setting (CVBS/RGB/S-VIDEO), vcr_vid-vcr scart video format setting (CVBS/S-VIDEO),\
bypass-scart bypass on/off, w-write scart register directly, reg-print scart register",
	/* usage */ 	(char *)"scart [command] [arg1] [arg2] : command=ar/tv_vid/vcr_vid/bypass \n>ar [0(12V_4:3)/1(6V_16:9)/2(0V_Int TV)] \
>tv_vid [0(CVBS,FB-0V)/1(RGB,FB-3.5V)/2(S-VIDEO,FB-0V)] >vcr_vid [0(CVBS,FB-0V)/2(S-VIDEO,FB-0V)] >bypass [on/off]\n>w [offset(hex)] [data(hex)] >reg");
#endif
#if defined(CONFIG_AUDIO_DECODER)
	// AUDIO
	CMD_RegisterWord(CMD_HwTest_Parser_Audio,
	/* keyword */	(char *)"audio",
	/* help */		(char *)"streamplay test start, stop",
	/* usage */		(char *)"audio [command] : command=start /stop / streamplay\n");
#endif
#if defined(CONFIG_VIDEO_DECODER)
	// VIDEO
	CMD_RegisterWord(CMD_HwTest_Parser_Video,
	/* keyword */	(char *)"video",
	/* help */		(char *)"bar-color bar on/off, sd-scart&composite on/off comp-component on/off vf-set video format",
	/* usage */		(char *)"video [command] [arg1] : command=bar/sd/comp/vf \n>bar [on/off] >sd [on/off] >comp [on/off] >vf [480i/480p/..] [50/60]");
#endif

	// HDMI
	CMD_RegisterWord(CMD_HwTest_Parser_Hdmi,
	/* keyword */	(char *)"hdmi",
	/* help */		(char *)"av-hdmi av on/off, hdcp-hdcp on/off",
	/* usage */		(char *)"hdmi [command] [arg1] : command=av/hdcp \n>av [on/off] >hdcp [on/off]");

#if defined(CONFIG_VIDEO_DECODER) && defined(CONFIG_AUDIO_DECODER)
	// AV
	CMD_RegisterWord(CMD_HwTest_Parser_AV,
	/* keyword */	(char *)"av",
	/* help */		(char *)"test-play specified channel [qam/ofdm/qpsk/asia]",
	/* usage */ 	(char *)"av [command] [arg1] : command=test \n>test [qam/ofdm/qpsk/asia]");
#endif

#if defined(CONFIG_TRANSCODER)
    // TRANSCODER
    CMD_RegisterWord(CMD_HwTest_Parser_Transcoder,
    /* keyword */   (char *)"transcoder",
    /* help */      (char *)"test-transcoder",
    /* usage */     (char *)"transcoder [command] [arg1] : command=test \n>transcoder [start/stop]");
#endif

#if !defined(CONFIG_DIRECTFB) || defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	// OSD for debugging
	CMD_RegisterWord(CMD_HwTest_Parser_OSD,
	/* keyword */	(char *)"osd",
	/* help */		(char *)"info-print osd info, dbg-on/off debug print of layer #, bar-color bar display, clear-clear FrameBuffer & OSD Display, copy-osd surface copy test, blend-osd surface blend test,\n\
show-show osd plane, hide-hide osd plane, st0/st1- Display Still image (st0:AudioOnlyStillImage, st1:ScrambledStillImage) on selected plane(layer),\n\
src- set in window rect of selected layer, win- set out window rect of selected layer, bg- set background rect of bottom layer,\n\
test - test osd func, fb-framebuffer disp or print or set alpha, surf-surface(layer) print or fill or flush",
	/* usage */		(char *)"osd [command] [arg1] : command=info/dbg/ bar/clear/copy/blend/show/hide/test/fb/surf \n\
>info >dgb on/off [layer#] >bar >clear >copy >blend [0~255]>show >hide \n\
>osd src/win [layer#]  [x] [y] [w] [h] >osd bg [x] [y] [w] [h]\n\
>test [0~7] >fb [fb#] disp [x] [y]/print [x_idx]/alpha [0~255] >surf [layer#] print / fill [x y w h color<hex>] / flush [x y w h]");
#endif
///////////////////////////////////////////////////////////////////////
/////				PVR
///////////////////////////////////////////////////////////////////////
#if	defined(CONFIG_RECORD)
	CMD_RegisterWord(CMD_HwTest_Parser_Pvr,
	/* keyword */	(char *)"pvr",
	/* help */		(char *)"PVR function test. \n rstart-record start, rstop-record stop, pstart-playback start, pstop-playback stop",
	/* usage */		(char *)"pvr [command] [arg1] [arg2]... command=\n rstart [device ID] [vformat(0:MPEG2/1:H.264)] [video PID] [audio PID] [PCR PID]\n  rstop [device ID]\n\
	rdelay [device ID][start time(sec)]\n\
	pstart [vformat(0:MPEG2/1:H.264)] [aformat(0:MPEG/1:AC3)] [video PID] [audio PID] [PCR PID]\n pstop\n\
	pjp [time(sec)]\n pfa [frame number(<0 : reverse, >0 : forward)][sleep(ms)]\n ptr [speed(-12 ~ 12)]\n\
	testhdd [1:start][0:file pb->rec][video PID][audio PID][PCR PID][0:int/1:ext]\n\
	testhdd [1:start][1:live rec->pb][video PID][audio PID][PCR PID][0:int/1:ext][tuner ID]\n\
	testhdd [0:stop][0:file pb->rec/1:live rec->pb]\n\
	dev [sata|esata|usb]");
	CMD_RegisterWord(CMD_HwTest_Parser_PvrEx,
	/* keyword */	(char *)"pvrex",
	/* help */		(char *)"thermal test only",
	/* usage */		(char *)"refer code");
#if defined(CONFIG_HDD)
	CMD_RegisterWord(CMD_HwTest_Parser_Hdd,
		/* keyword */(char *)"hdd",
		/* help */	(char *)"command = hdd,  param = long / short / spinup / spindown",
		/* usage */ 	(char *)"hdd long or hdd short or hdd spin up/down");
#endif
#endif

///////////////////////////////////////////////////////////////////////
/////				Demux
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Demux,
	/* keyword */	(char *)"demux",
	/* help */		(char *)"demux function test",
	/* usage */		(char *)"demux [command] [arg] command=test");

///////////////////////////////////////////////////////////////////////
/////				CI
///////////////////////////////////////////////////////////////////////
#if	defined(CONFIG_CI)
	CMD_RegisterWord(CMD_HwTest_Parser_Ci,
	/* keyword */	(char *)"ci",
	/* help */		(char *)"ci function test",
	/* usage */		(char *)"ci [command] [arg] command=readcis");
#endif
///////////////////////////////////////////////////////////////////////
/////				System
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Mem,
						 (char *)"mem",
						 (char *)"info - display mem(heap) alloc/free information",
						  (char *)"mem [command] [arg1] [arg2] ...\n command = info");

	CMD_RegisterWord(CMD_HwTest_Parser_Bmem,
						 (char *)"bmem",
						 (char *)"info - display bmem(heap) alloc/free information",
						  (char *)"bmem [command] [arg1] [arg2] ...\n command = info");


	CMD_RegisterWord(CMD_HwTest_Parser_I2c,
						 (char *)"i2c",
						 (char *)"info - test i2c functionality",
						  (char *)"i2c [command] [arg1] [arg2]\n command = info | setclock | scan \n arg1 = SCART | CHANNEL | EEPROM | HDMI\n\
arg2 = 47 | 50 |93 | 100 | 187 | 200 | 375 | 400");

#if defined(CONFIG_CRYPTO)
	CMD_RegisterWord(CMD_HwTest_Parser_Descrypt,
						 (char *)"des",
						 (char *)"info - encrption/decryption with 3des test",
						  (char *)"des");
#endif

#if defined(CONFIG_SUPPORT_CPU_TEMP)
	CMD_RegisterWord(CMD_HwTest_Parser_Cpu,
						(char *)"cpu",
						(char *)"temp - get CPU temperature, log - log on/off",
						(char *)"cpu [command] [arg1][arg2] : command=temp/log \n>temp >log [on/off]");
#endif

///////////////////////////////////////////////////////////////////////
/////				Ext.Interface
///////////////////////////////////////////////////////////////////////

	// FRONT Module for debugging - MICOM, PANNEL(LCD/VFD/7Segment...), LED, POWER, CLOCK
	CMD_RegisterWord(CMD_HwTest_Parser_Front,
	/* keyword */	(char *)"front",
	/* help */		(char *)"ver-micom version, reboot-system(STB) Rebbot, poweroff-STB PowerOff By micom, lcdpower-set front LCD power on/off,\n\
dim- set front LCD dimming level, unittest- display module unit test on/off, reason-Get Wakeup Source, code- Set RCU CustomCode to micom, test-full micom func test,\n\
disp-display string by front pannel, led-control led, clock-set/get micom local/wakeup clock",
	/* usage */		(char *)"front [command] [arg1][arg2] : command=ver/reboot/poweroff/lcdpower/reason/disp/led/clock \n>ver >reboot >poweroff [save/other(normal)] >lcdpower [on/off]\n\
>dim test/[level(0~255)] >unittest [on/off] >reason >code [CustomCode(hex)] >test >disp [string] \n>led power [red/other]/on [led#]/off [led#]/clear \n\
>clock local set YYYY/MM/DD HH:MM:SS (ex. front clock local set 2009/02/16 14:59:03)\n\
>clock local (for get local clock)> clock disp [off/other(on)]\n\
>clock wakeup set YYYY/MM/DD HH:MM:SS > clock wakeup on/off >clock wakeup(for get wakeup clock)");

#if	defined(CONFIG_FAN)
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	CMD_RegisterWord(CMD_HwTest_Parser_Fan,
	/* keyword */	(char *)"fan",
	/* help */		(char *)"info-set display on/off of HDD temperature & Fan control infomation, mode - set automatic(0)/manual(not 0) fan control mode, \n\
volt-set fan control voltage. Change to manual fan control mode if you use this command.\nreg- register callback func. temp- get hdd temperature\n\
uninit-uninit fan(release resources).\ninit-init fan(get resources).",
	/* usage */		(char *)"fan [command] [arg1][arg2] : command=info/mode/volt \n>info [off/on(or NULL)] >mode [%d(0/else)] >volt [%d(<14)] \n>reg >temp");
#endif
#else
	CMD_RegisterWord(CMD_HwTest_Parser_Fan,
	/* keyword */	(char *)"fan",
	/* help */		(char *)"info-set display on/off of HDD temperature & Fan control infomation, mode - set automatic(0)/manual(not 0) fan control mode, \n\
	volt-set fan control voltage. Change to manual fan control mode if you use this command.\nreg- register callback func. temp- get hdd temperature\n\
	uninit-uninit fan(release resources).\ninit-init fan(get resources).",
	/* usage */		(char *)"fan [command] [arg1][arg2] : command=info/mode/volt \n>info [off/on(or NULL)] >mode [%d(0/else)] >volt [%d(<14)] \n>reg >temp");
#endif
#endif

#if defined(CONFIG_ETHERNET)
	CMD_RegisterWord(CMD_HwTest_Parser_Ethernet,
	/* keyword */	(char *)"ethernet",
	/* help */		(char *)"",
	/* usage */		(char *)"");

#if defined(SUPPORT_ETHERNET)
	/* FTP OTA info init data */
	CMD_RegisterWord(CMD_HwTest_Parser_SetFtpOtaInfo,
	/* keyword */	(char *)"setftpota",
	/* help */		(char *)"",
	/* usage */		(char *)"");
#endif
#endif

	CMD_RegisterWord(CMD_HwTest_Parser_Rcu,
	/* keyword */	(char *)"rcu",
	/* help */		(char *)"",
	/* usage */ 	(char *)"rcu [defaultset/info]");

#if defined(CONFIG_CRYPTO)
	CMD_RegisterWord(CMD_HwTest_Parser_Crypto,
	/* keyword */	(char *)"crypto",
	/* help */		(char *)"",
	/* usage */ 	(char *)"crypto [info/fuse [scs/jtag/cwe]]");
#endif

#if defined(CONFIG_RF4CE)
	CMD_RegisterWord(CMD_HwTest_Parser_Rf4ce,
	/* keyword */	(char *)"rf4ce",
	/* help */		(char *)"",
	/* usage */ 	(char *)"rf4ce [init/pair/pair_abort/allow_pair/allow_pair_abort\n\
								unpair/send_data/standby/rx_enable/enable_sleep\n\
								disable_sleep/swreset/test_mode/test_rx_counter_get]");
#endif
#if defined(CONFIG_CHANNEL)
///////////////////////////////////////////////////////////////////////
/////				Channel
///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_FRONTEND_SAT)
	CMD_RegisterWord(CMD_HwTest_Parser_ChLock_QPSK,
		/* keyword */(char *)"avtest",
		/* help */	(char *)"av lock test (sat, ter, cab) ",
		/* usage */ 	(char *)"avtest sat 0 \n unitId(0/1)\n SAT: 12300, 27500, 3/4, H (Antenna: 10600, off, D)");

	CMD_RegisterWord(CMD_HwTest_Parser_ChLock_VER,
		/* keyword */(char *)"ver",
		/* help */	(char *)"vertical lock test (sat) ",
		/* usage */ 	(char *)"ver sd 0 \n unitId(0/1)\n SAT: 1130000, 27250, 3/4, V (Antenna: 5150, on, B)");

	CMD_RegisterWord(CMD_HwTest_Parser_ChLock_SD,
		/* keyword */(char *)"sd",
		/* help */	(char *)"command = lock, param = unit(tuner0/tuner1)",
		/* usage */ 	(char *)"lock sd 0 >>> tuner 0 SD locking (freq = 1150000 symbolrate = 28125  dvb-s)");

	CMD_RegisterWord(CMD_HwTest_Parser_ChLock_HD,
		/* keyword */(char *)"hd",
		/* help */	(char *)"command = lock, param = unit(tuner0/tuner1)",
		/* usage */ 	(char *)"lock hd 0 >>> tuner 0 HD locking (freq = 1100000 symbolrate = 27403  dvb-s2)");
#endif

#if defined(CONFIG_FRONTEND_CAB)
	CMD_RegisterWord(CMD_HwTest_Parser_Chlock_CAB,
		/* keyword */(char *)"cab",
		/* help */	(char *)"command = lock, param = unit(tuner0/tuner1)",
		/* usage */ 	(char *)"cab lock 0 >>> tuner 0 QAM locking (freq = 674000KHz symbolrate = 4500  64QAM)");

	CMD_RegisterWord(CMD_HwTest_Parser_ClearErrCount,
		/* keyword */(char *)"chclear",
		/* help */	(char *)"channel(sat/ter/cab) error count clear",
		/* usage */ 	(char *)"chclear sat/ter/cab unitId(0/1)");

#endif

#if defined(CONFIG_FRONTEND_TER)
	CMD_RegisterWord(CMD_HwTest_Parser_Chlock_TER,
		/* keyword */(char *)"ter",
		/* help */	(char *)"command = lock, param = unit(tuner0/tuner1)",
		/* usage */ 	(char *)"ter lock 0 >>> tuner 0 OFDM locking (freq = 767000KHz bandwidth = 6MHz  64QAM)");
//JAMES
	CMD_RegisterWord(CMD_HwTest_Parser_Dump_TER,
		/* keyword */(char *)"chdump",
		/* help */	(char *)"channel debug register dump ",
		/* usage */ 	(char *)"chdump 0(unitID)\n");
	CMD_RegisterWord(CMD_HwTest_Parser_READ_TER,
		/* keyword */(char *)"chrread",
		/* help */	(char *)"channel register read ",
		/* usage */ 	(char *)"chread 0(unitID) ADDR \n");
	CMD_RegisterWord(CMD_HwTest_Parser_WRITE_TER,
		/* keyword */(char *)"chrwrite",
		/* help */	(char *)"channel register write ",
		/* usage */ 	(char *)"chwrite 0(unitID) ADDR DATA\n");
#endif

	CMD_RegisterWord(CMD_HwTest_Parser_ChLock,
		/* keyword */(char *)"chlock",
		/* help */	(char *)"channel (sat/ter/cab/ana) locking test ",
		/* usage */ 	(char *)"chlock (type, unit, ...) \
		\n\n [sat] \n chlock sat 0 0 1 1150 28125 0 0 2 1 0 \n trans: 0)dvb-s, 1)dvb-s2\n mod: 0)auto, 1)qpsk, 2)8psk \n freq: Mhz \n symbolrate: Ksps \n fec: 0)auto ~ 3)3/4 ~ 9)9/10 \n polar: 0)Hor, 1)Ver \n diseqc: 1)A, 2)B, 3)C, 4)D \n tone: 0)off, 1)on \n pilot: 0)auto, 1)off, 2)on \
		\n\n [cab] \n chlock cab 0 674 4500 64 \n freq: Khz \n symbolrate: Ksps \n mod: 0)auto, 64)64QAM, 128)128QAM, 256)256QAM \
		\n\n [ter] \n chlock ter 0 767 6 64 \n freq: Mhz \n BW: 0)auto, 8)8M, 7)7M, 6)6M \n mod: 0)auto, 1)QPSK, 16)16QAM, 64)64QAM \n System 1)T2 other)T\n");

	CMD_RegisterWord(CMD_HwTest_Parser_ChStatus,
		/* keyword */(char *)"chstatus",
		/* help */	(char *)"channel (sat/ter/cab) lock status",
		/* usage */ 	(char *)"chstatus sat 0 \n unitId(0/1) ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChTSEnable,
		/* keyword */(char *)"chtsen",
		/* help */	(char *)"channel ts output (sat/ter/cab) enable",
		/* usage */ 	(char *)"chtsen sat 0 \n unitId(0/1) ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChTSDisable,
		/* keyword */(char *)"chtsdis",
		/* help */	(char *)"channel ts output (sat/ter/cab) chtsdis",
		/* usage */ 	(char *)"chtsdis sat 0 \n unitId(0/1) ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChReInitDev,
		/* keyword */(char *)"chreinit",
		/* help */	(char *)"channel reinit device",
		/* usage */ 	(char *)"chreinit");
    	CMD_RegisterWord(CMD_HwTest_Parser_ChUnInitDev,
		/* keyword */(char *)"chuninit",
		/* help */	(char *)"channel uninit device",
		/* usage */ 	(char *)"chuninit");

    CMD_RegisterWord(CMD_HwTest_Parser_ChDeepSleep,
		/* keyword */(char *)"chdeepsleep",
		/* help */	(char *)"channel deepsleep(0:wakeup, 1:deepsleep)",
		/* usage */ 	(char *)"chdeepsleep");

#if defined(CONFIG_FRONTEND_SAT)
	CMD_RegisterWord(CMD_HwTest_Parser_ChLnb,
		/* keyword */(char *)"chlnb",
		/* help */	(char *)"channel (sat) lnb test ",
		/* usage */ 	(char *)"chlnb sat off 1 \n on)LLC On, off)LLC Off \n 0) Lnb Out Off 1)Tuner1 Hor, 2)Tuner1 Ver, 3)Tuner2  Hor, 4)Tuner2 Ver");

	CMD_RegisterWord(CMD_HwTest_Parser_Ch22kTone,
		/* keyword */(char *)"ch22k",
		/* help */	(char *)"channel (sat) 22k Tone test ",
		/* usage */ 	(char *)"ch22k sat 1 \n 1) Tuner 1 Tone on, 2)Tuner 1 Tone off, 3)Tuner 2 Tone on, 4)Tuner 2 Tone off ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChDiseqc,
		/* keyword */(char *)"chdiseqc",
		/* help */	(char *)"channel (sat) diseqc test",
		/* usage */ 	(char *)"chdiseqc sat 0 1 \n unitId(0/1), DiSEqC: 1)A, 2)B, 3)C, 4)D ");
#endif

	CMD_RegisterWord(CMD_HwTest_Parser_ChRead,
		/* keyword */(char *)"chread",
		/* help */	(char *)"channel(sat/ter/cab) register read",
		/* usage */ 	(char *)"chread sat 0 1 1 \n unitId(0/1), index, length");

	CMD_RegisterWord(CMD_HwTest_Parser_ChWrite,
		/* keyword */(char *)"chwrite",
		/* help */	(char *)"channel(sat/ter/cab) register write",
		/* usage */ 	(char *)"chwrite sat 0 1 1 \n unitId(0/1), index, length, data");

	CMD_RegisterWord(CMD_HwTest_Parser_ChTop,
		/* keyword */(char *)"chtop",
		/* help */	(char *)"channel(sat/ter/cab) top control",
		/* usage */ 	(char *)"chtop sat 0 0 0xff 0 0xff 1\n unitId(0/1), rfmin, rfmax, ifmin, ifmax, on/off(1/0) ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChAntPowerEx,
		/* keyword */(char *)"chant",
		/* help */	(char *)"channel(ter) OFDM active antenna on/off",
		/* usage */ 	(char *)"chant ter 0 \n OFDM active antenna: 0)on, 1)off");

	CMD_RegisterWord(CMD_HwTest_Parser_ChSetLogLevel,
		/* keyword */(char *)"chlog",
		/* help */	(char *)"channel debug log level setting ",
		/* usage */ 	(char *)"chlog [enable|disable|show] [all|opt3|opt2|opt1|tsinfo|args|status|siginfo|tune]\n\t *2nd param is bit-ordered except for \"all\" \n");

	CMD_RegisterWord(CMD_HwTest_Parser_ChSetLockingChType,
		/* keyword */(char *)"chtype",
		/* help */	(char *)"channel current locking chtype setting ",
		/* usage */ 	(char *)"chtype sat/cab/ter \n");

#if defined(CONFIG_FRONTEND_SAT)
	CMD_RegisterWord(CMD_HwTest_Parser_ChSetInputMode,
		/* keyword */(char *)"chinput",
		/* help */	(char *)"channel (sat) input mode",
		/* usage */ 	(char *)"chinput mode 0 \n Mode: 0)One same, 1)Two different ");

	CMD_RegisterWord(CMD_HwTest_Parser_ChSetInput,
		/* keyword */(char *)"chsetinput",
		/* help */	(char *)"channel set input source",
		/* usage */ 	(char *)"chsetinput sat 0 0\n");

	CMD_RegisterWord(CMD_HwTest_Parser_ChGetInput,
		/* keyword */(char *)"chgetinput",
		/* help */	(char *)"channel get tuner capacity",
		/* usage */ 	(char *)"chgetinput sat 0 \n");

	CMD_RegisterWord(CMD_HwTest_Parser_ChSendDiseqc,
		/* keyword */(char *)"chsnd",
		/* help */	(char *)"channel send diseqc",
		/* usage */ 	(char *)"chsnd sat 0 0\n");

	CMD_RegisterWord(CMD_HwTest_Parser_ChReceiveDiseqc,
		/* keyword */(char *)"chrcv",
		/* help */	(char *)"channel receive diseqc",
		/* usage */ 	(char *)"chrcv sat 0 0\n");

#endif
	CMD_RegisterWord(CMD_HwTest_Parser_ChTemp,
		/* keyword */(char *)"demod",
		/* help */	(char *)"temp aging",
		/* usage */ 	(char *)"chtemp temp aging [on/off] \n");




#if defined(CONFIG_DEMOD_TSMF)
	CMD_RegisterWord(CMD_HwTest_Parser_ChTSMFTest,
		/* keyword */(char *)"chtsmf",
		/* help */	(char *)"ch tsmf test",
		/* usage */ 	(char *)"chtsmf testcb/stop/get");
	CMD_RegisterWord(CMD_HwTest_Parser_ChTSMFSetLogLevel,
		/* keyword */(char *)"chtsmflog",
		/* help */	(char *)"channel tsmf debug log level setting ",
		/* usage */ 	(char *)"chtsmflog 2 (0 ~ 6) \n");
	CMD_RegisterWord(CMD_HwTest_Parser_ChPathInfo,
		/* keyword */(char *)"chpathinfo",
		/* help */	(char *)"ch path info",
		/* usage */ 	(char *)"chpathinfo");
#endif

#endif /* End of CONFIG_CHANNEL */

#if defined(CONFIG_CABLE_MODEM)
	CMD_RegisterWord(CMD_HwTest_Parser_CmSetLogLevel,
		/* keyword */(char *)"cmlog",
		/* help */	(char *)"cm debug log level setting ",
		/* usage */ 	(char *)"cmlog 2 (0 ~ 6) \n");

	CMD_RegisterWord(CMD_HwTest_Parser_SnmpSetLogLevel,
		/* keyword */(char *)"snmplog",
		/* help */	(char *)"snmp debug log level setting ",
		/* usage */ 	(char *)"snmplog 2 (0 ~ 6) \n");
#endif


#if defined(CONFIG_HWTEST_EEPROM)
///////////////////////////////////////////////////////////////////////
/////				EEPROM
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Eeprom,
		/* keyword */(char *)"eeprom",
		/* help */	(char *)"command = r|w|info, param = offset value length",
		/* usage */ 	(char *)"eeprom [r|w|info] [offset] [value] [length]");
#endif

///////////////////////////////////////////////////////////////////////
/////				NVRAM
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Nvram,
		/* keyword */(char *)"nvram",
		/* help */	(char *)"command = w|r|dir|del, param = ro|rw filename value",
		/* usage */ 	(char *)"nvram [w|r|dir|del], param = [ro|rw] [filename] [value/size]");

#if defined(CONFIG_HWTEST_FLASH)
///////////////////////////////////////////////////////////////////////
/////				FLASH
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Flash,
		/* keyword */(char *)"flash",
		/* help */	(char *)"command = r|w|e|v|info, param = offset value length dump on/off",
		/* usage */ 	(char *)"eeprom [r|w|e|v|info] [offset] [value] [length] [dump on/off]");
#endif

#if defined(CONFIG_NAND_FLASH)
///////////////////////////////////////////////////////////////////////
/////				NAND FLASH
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_NandFlash,
		/* keyword */	(char *)"nandflash",
		/* help */		(char *)"command = r|w|v|info, param = offset value length dump on/off",
		/* usage */ 	(char *)"nandflash [r|w|v|info] [offset] [value] [length] [dump on/off]");
#endif

#if defined(CONFIG_EMMC_FLASH)
///////////////////////////////////////////////////////////////////////
/////				EMMC FLASH
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_EmmcFlash,
		/* keyword */	(char *)"emmcflash",
		/* help */		(char *)"command = image, param = write|erase, param = uocto|kernel|rootfs",
		/* usage */ 	(char *)"emmcflash [image] [write|erase] [uocto|kernel|rootfs]\n        emmcflash [xxx]");
#endif

#if defined(CONFIG_USB)
///////////////////////////////////////////////////////////////////////
/////				USB
///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	CMD_RegisterWord(CMD_HwTest_Parser_USB,
		/* keyword */(char *)"usb",
		/* help */	(char *)"command = r|w|e|v|info, param = offset value length dump on/off",
		/* usage */ 	(char *)"usb [r|w|e|v|info] [offset] [value] [length] [dump on/off]\n\r"
								"usb overcurrent\n\r"
								"usb test port [0,1]\n\r"
								"usb power [0|1]\n\r"
								"usb procinfo");
#endif
#else
	CMD_RegisterWord(CMD_HwTest_Parser_USB,
		/* keyword */(char *)"usb",
		/* help */	(char *)"command = r|w|e|v|info, param = offset value length dump on/off",
		/* usage */ 	(char *)"usb [r|w|e|v|info] [offset] [value] [length] [dump on/off]\n\r"
								"usb overcurrent\n\r"
								"usb test port [0,1]\n\r"
								"usb power [0|1]\n\r"
								"usb procinfo");
#endif
#endif
///////////////////////////////////////////////////////////////////////
/////				SmartCard
///////////////////////////////////////////////////////////////////////
#if defined (CONFIG_SMARTCARD) || defined(CONFIG_SMARTCARD_SLOT1)

	CMD_RegisterWord(CMD_HwTest_Parser_SC,
		/* keyword */(char *)"sc",
		/* help */	(char *)"atr",
		/* usage */ 	(char *)"sc [atr] [0/1]");
#endif
///////////////////////////////////////////////////////////////////////
/////				DSC
///////////////////////////////////////////////////////////////////////
#if defined (CONFIG_DSC)
	CMD_RegisterWord(CMD_HwTest_Parser_DSC,
		/* keyword */(char *)"dsc",
		/* help */	(char *)"decType",
		/* usage */ 	(char *)"dsc [type]");

	CMD_RegisterDSC();
#endif

#if defined(CONFIG_CABLE_MODEM)
///////////////////////////////////////////////////////////////////////
/////				CM
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_CM,
		/* keyword */(char *)"cm",
		/* help */	(char *)"command = cm,  param = ip mac fdc scan",
		/* usage */ 	(char *)"ip mac ver fdc scan reset sys route");
#endif

#if defined(CONFIG_VIDEO_DECODER) && defined(CONFIG_AUDIO_DECODER)
///////////////////////////////////////////////////////////////////////
/////				UTIL
///////////////////////////////////////////////////////////////////////
	CMD_RegisterWord(CMD_HwTest_Parser_Util,
		/* keyword */(char *)"util",
					 (char *)"",
					  (char *)"");
#endif

///////////////////////////////////////////////////////////////////////
/////				FS
///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_FS)
#if defined(CONFIG_DI_SHARED_LIB)
#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	CMD_RegisterWord(CMD_HwTest_Parser_Fs,
		/* keyword */(char *)"fs",
		/* help */	(char *)"command = mount|umount|format, param = fs function test",
		/* usage */ 	(char *)"fs mount [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda2)] [mountnode(/mnt/hd2)]\n\r"
						"fs umount [mountnode(/mnt/hd2)]\n\r"
						"fs autoformat [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda)] [mountnode(/mnt/hd)]\n\r" );
#endif
#else
	CMD_RegisterWord(CMD_HwTest_Parser_Fs,
		/* keyword */(char *)"fs",
		/* help */	(char *)"command = mount|umount|format, param = fs function test",
		/* usage */ 	(char *)"fs mount [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda2)] [mountnode(/mnt/hd2)]\n\r"
						"fs umount [mountnode(/mnt/hd2)]\n\r"
						"fs autoformat [fs_type(ext3/ext4/xfs/ntfs)] [devicenode(/dev/sda)] [mountnode(/mnt/hd)]\n\r" );
#endif
#endif

	CMD_RegisterWord(CMD_HwTest_Parser_VK,
	/* keyword */	(char *)"vkernel",
	/* help */		(char *)"vkernel [mem/sem/task/timer]",
	/* usage */ 	(char *)"vkernel [command] [arg1] : vk mem all");

	CMD_RegisterWord(CMD_HwTest_Parser_Uart,
	/* keyword */	(char *)"uart",
	/* help */		(char *)"uart dump [option]",
	/* usage */ 	(char *)"ex)uart dump clear\n\r");

#if defined(CONFIG_MEDIA20)
	CMD_RegisterWord(CMD_HwTest_Parser_MEDIA,
		/* keyword */	(char*)"media",
		/* help */	(char*)"media<enter>",
		/* usage */	(char*)"media <specific command without option>");
#endif

#if defined(CONFIG_PRODUCT_IMAGE_HWTEST)
#if defined(CONFIG_CALYPSO)
	CMD_RegisterWord(CMD_HwTest_Parser_CALYPSO,
		/* keyword */ (char*)"cso",
		/* help    */ (char*)"cso <enter>",
		/* usage   */ (char*)"cso <specific command without option>");
#endif
#endif

///////////////////////////////////////////////////////////////////////
/////				NETWORK
///////////////////////////////////////////////////////////////////////
#if defined(CONFIG_DINETWORK)
#if defined(CONFIG_ETHERNET)
	CMD_RegisterWord(CMD_HwTest_Parser_Network,
		/* keyword */(char *)"net",
		/* help */	(char *)"command = info|up|down|setmac|setaddr|setaddr6|setroute|setdns|setdnsmode|dhcp|regcb",
		/* usage */ 	(char *)"net info\n\r" );
#endif
#if defined(CONFIG_WIFI)
	CMD_RegisterWord(CMD_HwTest_Parser_WiFi,
		/* keyword */(char *)"wifi",
		/* help */	(char *)"command = scan|open|wep64a|wep64h|wep128a|wep128h|wpa|wpa2|wpawpa2|disconnect|apinfo|status|strength|rssi|reset",
		/* usage */ 	(char *)"wifi [command] [deviceid] [opts...]\n\r" );
#endif
#if defined(CONFIG_BLUETOOTH)
	CMD_RegisterWord(CMD_HwTest_Parser_Bluetooth,
		/* keyword */(char *)"bt",
		/* help */	(char *)"command = on|off|dev|ver|scan|info|cc|dc|rm|setmac|getmac|tr",
		/* usage */ 	(char *)"bt [command] [arg1]([devid] or [BD Address])\n\r" );
#endif
#if defined(CONFIG_PPPOE)
	CMD_RegisterWord(CMD_HwTest_Parser_PPPoE,
		/* keyword */(char *)"ppp",
		/* help */	(char *)"command = on|off|set|get",
		/* usage */ 	(char *)"ppp [command]\n\r" );
#endif
#endif

#if defined(CONFIG_CAS)
#if defined(CONFIG_CAS_NA)
	CMD_RegisterWord(CMD_HwTest_Parser_OTA,
		/* keyword */(char *)"ota",
		/* help */	(char *)"command = setpid|setotaver|settp|setftpinfo|usb2raw|setotatype",
		/* usage */ 	(char *)"ota [command] [arg1]\n\r" );
#endif
#endif

#if defined(CONFIG_MMC)
#if !defined(CONFIG_DI_MODULE_MMC_SHARED_LIB)
	CMD_RegisterWord(CMD_HwTest_Parser_MMC,
			/* keyword */(char *)"mmc",
			/* help */	(char *)"command = power | format | mount | umount | info",
			/* usage */ 	(char *)"mmc [command] [arg1] ...\n" );
#elif defined(CONFIG_PRODUCT_IMAGE_HWTEST)
	CMD_RegisterWord(CMD_HwTest_Parser_MMC,
			/* keyword */(char *)"mmc",
			/* help */	(char *)"command = power | format | mount | umount | info",
			/* usage */ 	(char *)"mmc [command] [arg1] ...\n" );

#endif
#endif
#if defined(CONFIG_MEDIA20)
	CMD_RegisterWord(CMD_HwTest_Parser_Demo,
		/* keyword */	(char*)"demo",
		/* help */	(char*)"demo<enter>",
		/* usage */	(char*)"demo <specific command without option>");
#endif
	(void)UT_Init();
}


