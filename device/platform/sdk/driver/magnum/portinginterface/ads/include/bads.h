/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads.h $
 * $brcm_Revision: Hydra_Software_Devel/75 $
 * $brcm_Date: 10/18/12 3:42p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/include/bads.h $
 * 
 * Hydra_Software_Devel/75   10/18/12 3:42p atanugul
 * SW3128-228: Add API Support for Reading New Front-End 312X, 314X, 346X
 * F/W Version IDs
 * 
 * Hydra_Software_Devel/73   9/28/12 2:16p atanugul
 * SW3128-228: Add support for  BADS_GetVersionInfo()
 * 
 * Hydra_Software_Devel/72   3/5/12 5:27p atanugul
 * SW3128-130: Add feGain and digitalAgcGain fields to ADS status and
 * increase the resolution for equalizerGain units to 1/100th of dB
 * 
 * Hydra_Software_Devel/71   2/16/12 6:14p atanugul
 * SW3128-124: Update description for BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/70   2/15/12 3:11p atanugul
 * SW3128-123: Fix description for dsChannelPower
 * 
 * Hydra_Software_Devel/69   2/15/12 2:57p atanugul
 * SW3128-123: Add accCleanCount to BADS_STATUS structure
 * 
 * Hydra_Software_Devel/68   2/14/12 10:42a atanugul
 * SW3128-119: Update SetAcquireParams API to make the second parameter as
 * const
 * 
 * Hydra_Software_Devel/67   2/8/12 5:14p atanugul
 * SW3128-119: Implement new apis for SetAcquireParams, GetAcquireParams
 * and GetDefaultAcquireParams
 * 
 * Hydra_Software_Devel/66   1/25/12 3:32p atanugul
 * SW7425-2260: Update majVer and minVer in BADS_Version structure to be
 * uint32_t
 * 
 * Hydra_Software_Devel/65   1/10/12 4:56p atanugul
 * SW3128-107:Enable HAB CRC Check in PI
 * 
 * Hydra_Software_Devel/64   12/22/11 3:37p atanugul
 * SW3128-100: Check for power status before sending HAB commands
 * 
 * Hydra_Software_Devel/63   12/8/11 3:12p atanugul
 * SW3128-89: Add PI support for Spectrum Analyzer Data retrieval
 * 
 * Hydra_Software_Devel/62   11/16/11 4:23p atanugul
 * SW3461-79: Add No Signal to GetLockStatus()
 * 
 * Hydra_Software_Devel/61   11/3/11 3:07p atanugul
 * SW3128-2: Add familyId to BADS_Version and change chipId to type
 * uint32_t
 * 
 * Hydra_Software_Devel/60   10/5/11 3:53p atanugul
 * SW3128-50: Update BADS_312x_GetScanStatus()
 * 
 * Hydra_Software_Devel/59   9/15/11 3:19p atanugul
 * SW3128-50: Add BADS_GetScanStatus()
 * 
 * Hydra_Software_Devel/58   8/31/11 5:42p nickh
 * SW7425-1129: Expose 3255 RPC call that returns the number of bonded
 * channels reserved for Docsis
 * 
 * Hydra_Software_Devel/57   7/6/11 3:18p vishk
 * SW7125-983: SW7125-1017: [Kbro 7019] Tuner 3109 AGC Return Value Not
 * Accurate Accumulated counters for 7125 & 3255 frontends
 * 
 * Hydra_Software_Devel/56   6/27/11 10:28a atanugul
 * SW3128-21: Add minor AP version to BADS_Version
 * 
 * Hydra_Software_Devel/55   5/27/11 1:57p vishk
 * SW7231-180: Remove duplicate member variables in bads.h and convert
 * BADS_AutomaticFrequencyOffset and NEXUS_FrontendQamFrequencyOffset
 * enum types to unsigned integer.
 * 
 * Hydra_Software_Devel/54   5/26/11 5:16p vishk
 * SW3128-2: update variables according to naming convention. Introduced
 * BADS_AutomaticFrequencyOffset_eVariable for variable frequency
 * offset/carrier range.
 * 
 * Hydra_Software_Devel/53   5/16/11 2:52p atanugul
 * SW3128-2: update variables according to naming convention
 * 
 * Hydra_Software_Devel/52   5/16/11 12:52p farshidf
 * SWDTV-7035: update naming convention
 * 
 * Hydra_Software_Devel/51   5/11/11 11:35a vishk
 * SWDTV-6857: waring fix and add BADS_Acquire_Type_eLast
 * 
 * Hydra_Software_Devel/50   5/6/11 4:23p farshidf
 * SWDTV-6857: waring fix
 * 
 * Hydra_Software_Devel/49   5/6/11 2:22p farshidf
 * SWDTV-6857: merge main
 * 
 * Hydra_Software_Devel/SWDTV-6857/1   5/6/11 2:01p farshidf
 * SWDTV-6857: add the scan structure and Calls
 * 
 * Hydra_Software_Devel/48   4/25/11 6:22p farshidf
 * SWDTV-6191: update the ADS callback
 * 
 * Hydra_Software_Devel/47   2/1/11 2:14p atanugul
 * SW3128-2: Add Support for BCM3128
 * 
 * Hydra_Software_Devel/46   12/2/10 12:20p mward
 * SW7125-728: Add Annex C support.
 * 
 * Hydra_Software_Devel/45   11/24/10 8:07p agin
 * SW7405-5021:  Allow 7405 host to control LNA via 31xx tuner/demod
 * chips.
 * 
 * Hydra_Software_Devel/44   10/28/10 4:26p VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/43   10/22/10 11:12a VISHK
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/42   10/21/10 6:56p vishk
 * SW7420-1020: Request for non-blocking call support in Nexus frontend
 * 
 * Hydra_Software_Devel/41   10/5/10 7:30p VISHK
 * SW7405-4915: Add nexus api to control power saver mode in downstream
 * demod and tuner, out of banddemod and tuner and upstream. Also add
 * nexus api to control daisy chaining of the frontends.
 * 
 * Hydra_Software_Devel/40   9/21/10 11:17a farshidf
 * SW7550-573: merge to main
 * 
 * Hydra_Software_Devel/SW7550-573/1   9/17/10 1:46p pinglong
 * SW7550-573: add corrected bits for QAM status
 * 
 * Hydra_Software_Devel/39   9/1/10 2:06p VISHK
 * SW7405-4839: To change the default frequncy offset.
 * 
 * Hydra_Software_Devel/38   8/30/10 4:01p farshidf
 * SW7550-543: merge to main branch
 * 
 * Hydra_Software_Devel/37   8/24/10 11:31a VISHK
 * SW7405-4808 : Connect reset status api for ADS and AOB to nexus api.
 * 
 * Hydra_Software_Devel/36   8/5/10 4:41p VISHK
 * SW7405-4722: Add support for "Frequency offset" and "Null packets" for
 * ADS and expose "auto acquire" in nexus for AOB
 * Hydra_Software_Devel/SW7550-543/2   8/30/10 3:27p farshidf
 * SW7550-543: change the stopTimer to untune
 * 
 * Hydra_Software_Devel/SW7550-543/1   8/27/10 4:55p pinglong
 * SW7550-543: support channel switch between QAM and OFDM on bcm97590
 * 
 * Hydra_Software_Devel/35   7/2/10 4:28p vishk
 * SW7420-873: Add support to inform application to update frontend tuner
 * gain and to inform that there is no signal to tune upon multiple tries
 * to acquire
 * 
 * Hydra_Software_Devel/34   6/25/10 2:28p farshidf
 * SW7550-475: add the symbolRateError for QAM status
 * 
 * Hydra_Software_Devel/33   4/14/10 4:43p farshidf
 * SW7550-38: merge the ADS changes requires for 7550
 * 
 * Hydra_Software_Devel/ADS_7550_40_9/1   4/14/10 3:29p cbrooks
 * Fixed AGC
 * 
 * Hydra_Software_Devel/32   2/11/10 11:02a farshidf
 * SW7550-262: add the re-acquire feature for 7550 QAM code
 * 
 * Hydra_Software_Devel/31   1/21/10 1:48p VISHK
 * SW7405-3596: To make QAM auto acquire a user configurable parameter
 * from Nexus
 * 
 * Hydra_Software_Devel/30   1/13/10 6:58p VISHK
 * SW7405-3722: BCM3109: Signal power reported is not correct
 * 
 * Hydra_Software_Devel/29   12/31/09 3:08p VISHK
 * SW7550-135: Support 7550 QAM/DVB-C support in Nexus
 * 
 * Hydra_Software_Devel/29   12/31/09 3:07p VISHK
 * SW7550-135: Support 7550 QAM/DVB-C support in Nexus
 * 
 * Hydra_Software_Devel/28   12/3/09 6:31p farshidf
 * SW7550-38: initial ADS code
 * 
 * Hydra_Software_Devel/27   12/1/09 4:27p farshidf
 * SW7550-38: compile fix
 * 
 * Hydra_Software_Devel/26   12/1/09 4:25p farshidf
 * SW7550-38: add the settings for 7550
 * 
 * Hydra_Software_Devel/25   11/18/09 6:08p farshidf
 * SW7550-38: add the 7550 ADS code
 * 
 * Hydra_Software_Devel/24   11/5/09 1:46p VISHK
 * SW7405-3210: BADS_31xx_GetStatus not returning values for
 * correctedCount, uncorrectedCount and berRawCount
 * 
 * Hydra_Software_Devel/23   9/15/09 4:34p vishk
 * SW7405-2852: 31xx: ADS add support configure of Parallel or Serial
 * Downstream interface.
 * 
 * SW7405-3009:Need to be able to shut down bcm3117 GPIO_00 to reduce
 * potential FCC concern.
 * 
 * Hydra_Software_Devel/22   9/14/09 2:29p vishk
 * SW7405-2852: 31xx: ADS add support configure of Parallel or Serial
 * Downstream interface.
 * 
 * SW7405-3009:Need to be able to shut down bcm3117 GPIO_00 to reduce
 * potential FCC concern.
 * 
 * Hydra_Software_Devel/21   8/25/09 11:19a vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/20   8/24/09 3:44p vishk
 * SW7405-2839: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/20   8/21/09 6:24p vishk
 * PR 57482: Bcm31xx: Add Power Management support for ADS
 * 
 * Hydra_Software_Devel/19   6/29/09 5:25p vishk
 * PR 56070: BCM31xx: Need to read dsChannelPower
 * 
 * Hydra_Software_Devel/18   6/17/09 11:26a vishk
 * PR 55960: Need to query spectrumInverted setting for BCM31xx
 * 
 * Hydra_Software_Devel/17   6/16/09 4:38p vishk
 * PR 55960: Need to query spectrumInverted setting for BCM31xx
 * 
 * Hydra_Software_Devel/16   2/24/09 3:23p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/15   2/20/09 4:43p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/14   2/20/09 4:34p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/13   1/29/09 7:23p vishk
 * PR 46431: ADS PI: Add support for BCM3117, BCM3114, and BCM3112.
 * PR 46436: AOB PI: Add support for BCM3117 and BCM3114
 * 
 * Hydra_Software_Devel/12   10/13/08 11:41a anilmm
 * PR47842: BER calculation software implementation
 * 
 * Hydra_Software_Devel/11   5/27/08 4:27p haisongw
 * PR43047: Add two parameters in BADS_GetStatus() to sync up with 325X
 * docsis 2.0.0 release
 *
 * Hydra_Software_Devel/10   12/20/07 6:02p haisongw
 * PR36061: speed up 325x QAM acqusition time
 *
 * Hydra_Software_Devel/9   10/12/07 1:28p haisongw
 * PR36060: add OCAP DPM support for three 3420 tuners
 *
 * Hydra_Software_Devel/8   9/7/07 3:03p haisongw
 * PR28691: adding comments for BADS_EnablePowerSaver()
 *
 * Hydra_Software_Devel/7   5/10/07 6:22p haisongw
 * PR30640: Add an option to enable/disable 325X ADS auto-reacquiring
 *
 * Hydra_Software_Devel/6   4/16/07 6:12p haisongw
 * PR25049: SetTop API function btuner_get_qam_status() returns incomplete
 * information
 *
 * Hydra_Software_Devel/5   6/27/06 3:08p haisongw
 * PR22368: Add AOB, AUS and TNR3255ob support.
 *
 * Hydra_Software_Devel/4   6/26/06 6:24p haisongw
 * PR 20984: Update DocJet comments
 *
 * Hydra_Software_Devel/3   5/12/06 5:32p haisongw
 * PR 20984: fix DocJet errors
 *
 * Hydra_Software_Devel/2   4/11/06 4:53p dlwin
 * PR 19978: Merge 3255 support.
 *
 * Hydra_Software_Devel/PROD_DSG_REFSW_Devel/1   4/3/06 4:18p haisongw
 * PR 19978:add  tuner Settop API support on BCM97455 platform
 *
 * Hydra_Software_Devel/1   12/13/05 2:56p dlwin
 * PR 18598: Add new ADS for 3255
 *
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
The Advance Qam inband DownStream (ADS) receiver represents one of the components
that makes up the Front End subsystem of a SetTop Box.  The ADS receiver is
responsible for demodulating a Qam signal.

Broadcom provides two classes of Qam Demodulation devices, simple and advance.
The term "simple" and "advance" refers to the use of embedded Acquisition Processor.

The simple Qam Demodulation devices do not contain an embedded Acquistion Processor,
therefore requires the host processor to start acquisition, monitor the state of the
downstream signal, and if signal it lost, restart the process of reacquiring the
downstream signal. Bcm3255, Bcm3125 are classified as simple Qam Demodulation devices.

Advance Qam Demodulation devices contain an embedded Acquisition Processor (AP).
Taking advantage of the AP, advance device requires much less resources of the
host processor. Once host processor requests AP to acquire a signal, the AP will
start the acquisition process. When it locks to signal, it will notify the host.
When signal is loss, the AP will notify the host and start the process of
reacquiring the downstream signal without host involvement. Bcm3255 is
classified as advance Qam Demodulation devices.

This module represents the public API for all advance ADS modules. It was created
to support multiple advance Qam Downstream modules, 3255, 3256, etc on the same
system (board). To support multiple Qam Downstream chips for a given system, all the
public API have been vitualized. These vitual function pointer are initialized
to a specific device when the device is created. Please see the sample code
for how this is done. After the function pointers are initialized, ADS functions
can be used.


Design
The design for BADS PI API is broken into three parts.

o Part 1 (open/close/configuration):

    These APIs are used for opening and closing BADS device/device channel.
    When a device/device channel is opened, the device/device channel can be
    configured.

o Part 2 (acquire/get/set):

    These APIs are used for getting and setting runtime attributes of BADS.
    Along with get and set APIs, an acquire API is also provided.  The acquire
    API is used to acquire (lock) Inband Downstream signal.

o Part 3 (process):

    This API is used to process notification.


Usage
The usage of BADS involves the following:

   * Configure/Open of BADS

      * Configure BADS device for the target system
      * Open BADS device
      * Configure BADS device channel for the target system
      * Open BADS device channel

   * Program/Acquire device channel

      * Try to acquire Inband Downstream signal
      * Check status of acquisition


Interrupt Requirements
No interrrupt processing is done by this module.

************** Sample code for 3255 **********************
//
// NOTE: The following sample code does not do any error checking.
//
// In this example, the target H/W board is configured for an
// Inband Downstream Annex B application, with a Crystal Frequency of 24.00 MHz
// and IF Frequency of 43.75 MHz.
//
static BADS_Handle adsDev;
static BADS_ChannelHandle ibChn[2];
static BCHP_Handle hChip3255;
static BREG_Handle hReg3255;
static BRPC_Handle hRpc;

static BADS_CallbackFunc cbLockChangeFunc(
    BADS_ChannelHandle hIbChn,          // Handle of channel in question
    void *pParam
    )
{
    bool *lockStatus = (bool *) pParam;

    BADS_GetLockStatus( hIbChn, lockStatus );

    return;
}

main( void )
{
    BADS_Settings adsDevSettings;
    BADS_ChannelSettings adsChnDevSettings;
    BADS_InbandParam ibParam;
    bool isLock;

    // Initialize hChip3255, hReg3255, and hRpc . . .

    // BADS initialization for BCM3255
    BADS_3255_GetDefaultSetting( &settings );
    // Mutex to lock access to 3255 commands, so the other lock 3255 tuner, OOB
    // are not access during DS access.
    settings.xtalFreq = 24000000;
    settings.hRpc = hRpc;
    BADS_Open( hAds, hChip3255, hReg3255, &settings );

    // Now, get ready to open the DS channel
    BADS_GetChannelDefaultSettings( hAds, &adsChnDevSettings );
    adsChnDevSettings.ifFreq = 43750000;
    // Open it channel 0
    BADS_OpenChannel( adsDev, &ibChn[0], 0, &adsChnDevSettings );

    // Now startup the RPC Notificiation Backup Ground thread/task . . .

    // Install callback for state change event, where last parameter
    // is for general use.
    BADS_InstallCallback( ibChn[0], BADS_Callback_eLockChange, cbLockChangeFunc, (void *) &isLock );

    // Tune Tuner . . .

    // Acquire Inband Downstream Signal, Annex B, Qam 64 modulation with a
    // symbol rate of 5.056 MBaud.
    BADS_GetIbParamsDefault( ibChn[0], &inParams );
    ibParams.modType = BADS_ModulationType_eAnnexBQam64;
    ibParams.symbolRate = 5056000;
    BADS_Acquire( ibChn[0], &ibParams );

    while(1)
    {
        // Print the lock status every second
        BKNI_Sleep(1000);
        printf("Lock Status: %s\n", (isLock==true): "locked", "not lock");
    }
}

void checkRpcNotificationThread(. . .)
{
    while (!system_terminated)
    {
        uint32_t device_id, event;

        lock_with_system_sw();
        BRPC_CheckNotification(hRpc, &device_id, &event, 0);
        if ((event>>16))
        {
            switch (device_id)
            {
                case BRPC_DEV_ID_DS0:
                    BADS_ProcessNotification(ibChn[0]);
                    break;
                case BRPC_DEV_ID_DS1:
                    BADS_ProcessNotification(ibChn[1]);
                    break;
                case BRPC_DEV_ID_OB0:
                    BAOB_ProcessNotification(ob);
                    break;
                default:
                    // device not used
                    break;
            }
            unlock_with_system_sw();
        }
        else
        {
            unlock_with_system_sw();
            BKNI_Sleep(500);
        }
    }
}

</verbatim>

************** Sample code for chips 3117/3114/3112/3109 **********************

</verbatim>

******************* Handle L1 Interrup from 3117 *****************************

static void App_L1_isr_handler(void *pParam1, int pParam2)
{
    BHAB_Handle hab = (BHAB_Handle)pParam2;
    BHAB_HandleInterrupt_isr(hab);
}

static void Interrupt_event_handler(void *pParam)
{
    BHAB_Handle hab = (BHAB_Handle)pParam;
    BHAB_ProcessInterruptEvent(hab);
}

***************************** Create HAB ************************************
//The application needs to create the instance of this basemodule for 3117/3114/3112 or 3109
//and pass the handle to the pi as mentioned in the sample code below.

BHAB_Handle hab;

static void Hab_create()
{
    BHAB_Settings stHabSettings;
   
    BHAB_3117_GetDefaultSettings(&stHabSettings);

    Populate the HAB settings structure. 

    BHAB_Open( &hab, i2c/spi handle, &stHabSettings);

    //Download code binary for the Acquisition Processon
    BHAB_InitAp(&hab, AP_binary);
    
    Get interrupt_event handle and register interrupt_event callback 

    BHAB_GetInterruptEventHandle(&hab, interrupt_event handle);

    Interrupt_event_handler should be called when interrupt event is set by HAB
}

******************* Notify the app of lock status change ************************
static BADS_CallbackFunc cbLockChangeFunc(
    BADS_ChannelHandle hIbChn,          
    void *pParam
    )
{
    bool *lockStatus = (bool *) pParam;

    BADS_GetLockStatus( hIbChn, lockStatus );

    return;
}

**************************************************************************
static BTNR_Handle hTnr; 
static BADS_Handle hAds;
static BADS_ChannelHandle ibChn[2];
static BCHP_Handle hChip3117;
static BREG_Handle hReg3117;


main( void )
{
    BADS_Settings settings;
    BADS_ChannelSettings adsChnDevSettings;
    BADS_InbandParam ibParam;
    bool isLock;
    BTNR_3117Ib_Settings tnrIb3117_cfg;
    uint8_t frequency = 765; //in MHZ (for example).
        
    //Open Tuner . . .
    BTNR_3117Ib_GetDefaultSettings(&tnrIb3117_cfg);
    BTNR_3117Ib_Open(&hTnr,&tnrIb3117_cfg, hab);
    
    BADS_3117_GetDefaultSettings( &settings );
    settings.hGeneric = hab;
    BADS_Open( hAds, hChip3117, hReg3117, &settings );

    BADS_GetChannelDefaultSettings( hAds, &adsChnDevSettings );
    BADS_OpenChannel( hAds, &ibChn[0], 0, &adsChnDevSettings );

    // Install callback for state change event, where last parameter
    // is for general use.
    BADS_InstallCallback( ibChn[0], BADS_Callback_eLockChange, cbLockChangeFunc, (void *) &isLock );

    //Tune Tuner . . .
    BTNR_SetTunerRfFreq(hTnr, frequency, BTNR_TunerMode_eDigital);

    // Acquire Inband Downstream Signal, Annex B, Qam 64 modulation with a
    // symbol rate of 5.056 MBaud.
    ibParam.modType = BADS_ModulationType_eAnnexBQam64;
    ibParam.symbolRate = 5056000;
    BADS_Acquire( ibChn[0], &ibParam );

    while(1)
    {
        // Print the lock status every second
        BKNI_Sleep(1000);
        printf("Lock Status: %s\n", (isLock==true): "locked", "not lock");
    }
}

</verbatim>

**************************************************************************/
#ifndef BADS_H__
#define BADS_H__

#include "bchp.h"
#include "breg_mem.h"
#include "bint.h"
#include "bkni.h"
#include "berr_ids.h"
#include "bmem.h"
#include "btmr.h"
#include "bfec.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
Summary:
    Error Codes specific to BADS

Description:

See Also:

****************************************************************************/
#define BADS_ERR_NOTAVAIL_MODULATION_TYPE       BERR_MAKE_CODE(BERR_ADS_ID, 0)
#define BADS_ERR_NOTAVAIL_CHN_NO                BERR_MAKE_CODE(BERR_ADS_ID, 1)
#define BADS_ERR_SYMBOLRATE_EXCEEDED            BERR_MAKE_CODE(BERR_ADS_ID, 2)
#define BADS_ERR_POWER_DOWN                     BERR_MAKE_CODE(BERR_ADS_ID, 3)
#define BADS_ERR_INVALID_CRC                    BERR_MAKE_CODE(BERR_ADS_ID, 4)


/***************************************************************************
Summary:
    The handle for Qam In-Band Downstream module.

Description:
    An opaque handle for BADS device.

See Also:
    BADS_Open()

****************************************************************************/
typedef struct BADS_P_Handle                *BADS_Handle;

/***************************************************************************
Summary:
    The handle for Qam In-Band Downstream module.

Description:
    An opaque handle for BADS device channel.

See Also:
    BADS_OpenChannel()

****************************************************************************/
typedef struct BADS_P_ChannelHandle         *BADS_ChannelHandle;



/***************************************************************************
Summary:
    Enumeration for QPSK OOB spectrum setting

Description:
    This enumeration defines the valid spectrum setting.

See Also:
****************************************************************************/
typedef enum BADS_SpectrumMode
{
    BADS_SpectrumMode_eAuto,
    BADS_SpectrumMode_eManual,
    BADS_SpectrumMode_eLast
} BADS_SpectrumMode;

/***************************************************************************
Summary:
    Enumeration for QPSK OOB DPM setting

Description:
    This enumeration defines the valid DPM setting.

See Also:
****************************************************************************/
typedef enum BADS_DpmMode
{
    BADS_DpmMode_Disabled,
    BADS_DpmMode_Enabled,
    BADS_DpmMode_eLast
} BADS_DpmMode;


/***************************************************************************
Summary:
    Enumeration for QPSK OOB Inverted spectrum setting

Description:
    This enumeration defines the valid Inverted spectrum setting.

See Also:
****************************************************************************/
typedef enum BADS_InvertSpectrum
{
    BADS_InvertSpectrum_eNoInverted,
    BADS_InvertSpectrum_eInverted,
    BADS_InvertSpectrum_eLast
} BADS_InvertSpectrum;

/***************************************************************************
Summary:
    Enumeration for modulation type

Description:
    This enumeration defines the valid modulation types.

See Also:
    BADS_Acquire(), BADS_GetStatus()

****************************************************************************/
typedef enum BADS_ModulationType
{
    /* Most of these are currently not supported, are here for future use */
    BADS_ModulationType_eAnnexAQam16,
    BADS_ModulationType_eAnnexAQam32,
    BADS_ModulationType_eAnnexAQam64,
    BADS_ModulationType_eAnnexAQam128,
    BADS_ModulationType_eAnnexAQam256,
    BADS_ModulationType_eAnnexAQam512,
    BADS_ModulationType_eAnnexAQam1024,
    BADS_ModulationType_eAnnexAQam2048,
    BADS_ModulationType_eAnnexAQam4096,
    BADS_ModulationType_eAnnexBQam16,
    BADS_ModulationType_eAnnexBQam32,
    BADS_ModulationType_eAnnexBQam64,
    BADS_ModulationType_eAnnexBQam128,
    BADS_ModulationType_eAnnexBQam256,
    BADS_ModulationType_eAnnexBQam512,
    BADS_ModulationType_eAnnexBQam1024,
    BADS_ModulationType_eAnnexBQam2048,
    BADS_ModulationType_eAnnexBQam4096,
    BADS_ModulationType_eAnnexCQam16,
    BADS_ModulationType_eAnnexCQam32,
    BADS_ModulationType_eAnnexCQam64,
    BADS_ModulationType_eAnnexCQam128,
    BADS_ModulationType_eAnnexCQam256,
    BADS_ModulationType_eAnnexCQam512,
    BADS_ModulationType_eAnnexCQam1024,
    BADS_ModulationType_eAnnexCQam2048,
    BADS_ModulationType_eAnnexCQam4096,
    BADS_ModulationType_eLast
} BADS_ModulationType;

/***************************************************************************
The BADS_AutomaticFrequencyOffset enum has been deprecated. It is an unsigned integer in units of Hz now.
The following #defines are for backward compatibility only.
***************************************************************************/

#define BADS_AutomaticFrequencyOffset uint32_t

#define BADS_AutomaticFrequencyOffset_eAnnexAQam200 200000
#define BADS_AutomaticFrequencyOffset_eAnnexAQam125 125000
#define BADS_AutomaticFrequencyOffset_eAnnexAQam250 250000
#define BADS_AutomaticFrequencyOffset_eAnnexAMax 3
#define BADS_AutomaticFrequencyOffset_eAnnexBQam180 180000
#define BADS_AutomaticFrequencyOffset_eAnnexBQam125 125000
#define BADS_AutomaticFrequencyOffset_eAnnexBQam250 250000
#define BADS_AutomaticFrequencyOffset_eAnnexBQam200 200000
#define BADS_AutomaticFrequencyOffset_eLast 8

/***************************************************************************
Summary:
    Enumeration for Callback types

Description:
    This enumeration defines Callback types.

See Also:

****************************************************************************/
typedef enum BADS_Callback
{
    BADS_Callback_eLockChange,          /* Callback to notify application of lock change */
    BADS_Callback_eUpdateGain,          /* Callback to notify application to update gain */ 
    BADS_Callback_eNoSignal,            /* Callback to notify application there is no signal */ 
    BADS_Callback_eAsyncStatusReady,    /* Callback to notify application there is no signal */    
    BADS_Callback_eTuner,               /* Callback to tuner regarding a new setting */ 
    BADS_Callback_eSpectrumDataReady,   /* Callback to notify application that spectrum analyzer data is ready */ 
    BADS_Callback_eLast                 /* More may be required */
} BADS_Callback;

/***************************************************************************
Summary:
    Enumeration for Acquire types

Description:
    This enumeration defines the different acquisition type.

See Also:

****************************************************************************/
typedef enum BADS_AcquireType
{
	BADS_AcquireType_eAuto,
	BADS_AcquireType_eFast,
	BADS_AcquireType_eSlow,
	BADS_AcquireType_eScan,
	BADS_AcquireType_eLast
}BADS_AcquireType;

/***************************************************************************
Summary:
    Enumeration for Acquisition Status

Description:
    This enumeration defines the different acquisition statuses.

See Also:

****************************************************************************/
typedef enum BADS_AcquisitionStatus
{           
    BADS_AcquisitionStatus_eNoSignal,      
    BADS_AcquisitionStatus_eUnlocked,      
    BADS_AcquisitionStatus_eLockedFast,      
    BADS_AcquisitionStatus_eLockedSlow      
} BADS_AcquisitionStatus;

/***************************************************************************
Summary:
    Enumeration for BPS

Description:
    This enumeration defines the different BPS modes.

See Also:

****************************************************************************/
typedef enum BADS_BPS
{      
    BADS_BPS_eQam16,      
    BADS_BPS_eQam32,      
    BADS_BPS_eQam64,     
    BADS_BPS_eQam128,      
    BADS_BPS_eQam256,     
    BADS_BPS_eQam512,      
    BADS_BPS_eQam1024
} BADS_BPS;

/***************************************************************************
Summary:
    Enumeration for Interleaver

Description:
    This enumeration defines the different Interleaver types.

See Also:

****************************************************************************/
typedef enum BADS_Interleaver
{      
    BADS_Interleaver_eI128_J1,      
    BADS_Interleaver_eI128_J2,     
    BADS_Interleaver_eI128_J3,    
    BADS_Interleaver_eI128_J4,      
    BADS_Interleaver_eI64_J2,      
    BADS_Interleaver_eI32_J4,    
    BADS_Interleaver_eI16_J8,     
    BADS_Interleaver_eI8_J16,     
    BADS_Interleaver_eI4_J32,      
    BADS_Interleaver_eI2_J64,      
    BADS_Interleaver_eI1_J128,      
    BADS_Interleaver_eI12_J17,      
    BADS_Interleaver_eUnsupported
} BADS_Interleaver;

/***************************************************************************
Summary:
    Enumeration for Lock Status

Description:
    This enumeration represents the lock status of of a ADS channel.

See Also:

****************************************************************************/
typedef enum BADS_LockStatus
{           
    BADS_LockStatus_eUnlocked,
    BADS_LockStatus_eLocked, 
    BADS_LockStatus_eNoSignal,      
    BADS_LockStatus_eLast    
} BADS_LockStatus;

/***************************************************************************
Summary:
    Enumeration for FFT Size for Spetrum Analyzer Data

Description:
    This enumeration represents the FFT Size for Spetrum Analyzer Data.

See Also:

****************************************************************************/
typedef enum BADS_FftSize
{
    BADS_FftSize_e64 = 0x6,
    BADS_FftSize_e128 = 0x7,
    BADS_FftSize_e256 = 0x8,
    BADS_FftSize_e512 = 0x9,
    BADS_FftSize_e1024 = 0x0a,
    BADS_FftSize_e2048 = 0x0b,
    BADS_FftSize_e4096 = 0x0c,
    BBADS_FftSize_eLast
} BADS_FftSize;

/***************************************************************************
Summary:
    This structure represents ADS Qam In-Band Downstream acquisition parameters.

Description:

See Also:
    BADS_Acquire()

****************************************************************************/
typedef struct BADS_InbandParam
{
    BADS_ModulationType modType;        /* Modulation type */
    uint32_t            symbolRate;             /* in Baud, for 3128/3461 this is only valid for Annex A  */
    BADS_InvertSpectrum invertSpectrum;
    BADS_SpectrumMode   spectrum;
    BADS_DpmMode        enableDpm;
    bool                autoAcquire;
    bool                enableNullPackets; /* Enables/disables improved locking mechanism for Annex_A signals containing >98% null packets.*/
    unsigned            frequencyOffset; /*   Automatic frequency Offset (pull-in) range of the qam demodulator 
                                                        For example "frequencyOffset = 180000" can pull in carriers offset +/-180KHz   from the tuned frequency */
    BADS_AcquireType	acquisitionType;
    bool                tuneAcquire; /* This bit directs the Downstream Demodulator to initiate an acquire immediately after its associated WFE or Tuner is tuned.  
                                        If the bit is set, then the Demodulator will initiate an acquire immediately after the tuner has completed a tune  regardless of autoacquire. 	
                                        Note: Unless otherwise indicated, this flag only applies to the Acquisition Processors Internal WFE or Internal Tuner. */
} BADS_InbandParam;

/***************************************************************************
Summary:
    This structure represents ADS Qam In-Band Downstream version.

Description:

See Also:
    BADS_GetVersion

****************************************************************************/
typedef struct BADS_Version
{
    uint32_t    familyId; /* chip Family ID */
    uint32_t    chipId; /* chip ID */
    uint32_t    majVer; /* major chip revision number */
    uint32_t    minVer; /* minor chip revision number */
    uint8_t     apVer; /* major AP microcode version */
    uint8_t     minApVer; /* minor AP microcode version */
} BADS_Version;

/***************************************************************************
Summary:
    This structure represents the ADS Scan Status for a Qam In-Band Downstream
    channel.

Description:
    This structure is returned when BADS_GetScanStatus() is called.  This
    structure contains the scan status of Qam In-Band Downstream
    channel.

See Also:
    BADS_ScanStatus()

****************************************************************************/
typedef struct BADS_ScanStatus
{
    BADS_AcquisitionStatus acquisitionStatus; /* Acquisition Status (NoSignal, Unlocked, LockedFast, LockedSlow)  */
    BADS_ModulationType modType;        /* Modulation type */
    bool isSpectrumInverted;            /* If 1, Spectrum is inverted. */
    bool autoAcquire;                   /* If 1, autoAcquire is enabled */
    BADS_Interleaver interleaver;	    /* FEC interleaver */
    uint32_t symbolRate;                /* symbol rate in Baud */
    int32_t carrierFreqOffset;          /* carrier frequency offset in 1/1000 Hz */	
} BADS_ScanStatus;

/***************************************************************************
Summary:
    This structure represents the ADS Status for a Qam In-Band Downstream
    channel.

Description:
    This structure is returned when BADS_GetStatus() is called.  This
    structure contains the complete status of Qam In-Band Downstream
    channel.

See Also:
    BADS_GetStatus()

****************************************************************************/
typedef struct BADS_Status
{
    bool isPowerSaverEnabled;           /* Eanble=1, Disable=0 */
    BADS_ModulationType modType;        /* Modulation type */
    uint32_t ifFreq;                    /* in Hertz, IF freq. */
    uint32_t symbolRate;                /* in Baud */
    int32_t  symbolRateError;            /* symbol rate error in Baud */
    bool isFecLock;                     /* lock=1, unlock=0 */
    bool isQamLock;                     /* lock=1, unlock=0 */
    uint32_t correctedCount;            /* reset on every read */
    uint32_t uncorrectedCount;          /* reset on every read */
    int32_t snrEstimate;                /* in 1/256 dB */
    int32_t agcIntLevel;                /* in 1/10 percent */
    int32_t agcExtLevel;                /* in 1/10 percent */
    int32_t carrierFreqOffset;          /* in 1/1000 Hz */
    int32_t carrierPhaseOffset;         /* in 1/1000 Hz */
    uint32_t rxSymbolRate;              /* in Baud, received symbol rate */
    uint16_t interleaveDepth;           /* use in Docsis */
    uint32_t goodRsBlockCount;          /* reset on every read */
    uint32_t berRawCount;               /* reset on every read */
    int32_t dsChannelPower;             /* in 1/10th of a dBmV unit; OCAP DPM support for video channels */
    uint32_t mainTap;                   /* Channel main tap coefficient */
    int32_t feGain;                     /* in 1/100th of a dB */
    int32_t digitalAgcGain;             /* in 1/100th of a dB */    
    uint32_t equalizerGain;             /* Channel equalizer gain value in in 1/100th of a dB */
    /* OCAP required postRsBER for all DS channels. postRsBER and elapsedTimeSec will be reset on every channel change*/
    /* fpostRsBER normally between xxxe-6 to xxxe-12 float value, to send this small float number over rMagnum to host, we convert it to uint32 using the formula*/
    uint32_t postRsBER;                 /* Converted floating point fpostRsBER --> uint32_t postRsBER for rMagnum transport: */
                                        /* int ipostRsBER = (int)( log(fpostRsBER) * 1000000.0 ); uint32_t postRsBER = (uint32_t)ipostRsBER; */
                                        /* Host side will need this to convert it back: int ipostRsBER = (int)postRsBER; float fpostRsBER = exp( (float)ipostRsBER/1000000.0 ); */
    uint32_t elapsedTimeSec;            /* postRsBER over this time */
    bool isSpectrumInverted;            /* If 1, Spectrum is inverted relative to the AP bootup. */
    uint32_t cleanCount;                /* reset on every reset status*/
    int32_t agcAGFLevel;                /* in 1/10 percent */
    int32_t agcAGF2Level;               /* in 1/10 percent */
    uint32_t correctedBits;             /* reset on every reset status */
    uint32_t accCorrectedCount;         /* Accumulated corrected block count. Reset on every reset status */
    uint32_t accUncorrectedCount;       /* Accumulated un corrected block count. Reset on every reset status */
    uint32_t accCleanCount;             /* Accumulated clean block count. Reset on every reset status */
} BADS_Status;

/***************************************************************************
Summary:
    Structure for Spectrum Analyzer Data

Description:

See Also:
    BADS_GetSpectrumAnalyzerData()

****************************************************************************/
typedef struct BADS_SpectrumData
{
    uint32_t *data; /* ptr to the data */
    uint16_t datalength; /* data length in number of words */    
    bool moreData; /* this bit indicates whether there is more data available or not */
} BADS_SpectrumData;

/***************************************************************************
Summary:
    Spectrum settings for Spectrum Analyzer Data

Description:

See Also:
    BADS_RequestSpectrumAnalyzerData()

****************************************************************************/
typedef struct BADS_SpectrumSettings
{
    uint32_t startFreq; /* The start frequency of the spectrum analyzer in Hz */
    uint32_t stopFreq; /* The stop frequency of the spectrum analyzer in Hz */
    BADS_FftSize fftSize; /*  FFT Size */
    uint8_t binAverage; /*  Total number of samples to be averaged per bin. It's value ranges from 0 to 255*/
    uint32_t numSamples; /* Total number of 32-bit frequency values to return */
} BADS_SpectrumSettings;

/***************************************************************************
Summary:
    Callback used for event notification.

Description:
    When this PI wants to notify an application, it will call this callback
    function the callback function is registered.

See Also:
    BADS_InstallLockStateChangeCallback(), BADS_InstallInterruptCallback()

****************************************************************************/
typedef BERR_Code (*BADS_CallbackFunc)(void *pParam );


/***************************************************************************
Summary:
    Structure for API function table

Description:
   This structure contains pointers to all public BADS functions.

See Also:
    None.

****************************************************************************/
struct BADS_Settings;
struct BADS_ChannelSettings;
struct BADS_DaisyChainSettings;
struct BADS_ChannelScanSettings;

typedef struct BADS_ApiFunctTable
{
    BERR_Code (*Open)(BADS_Handle *, BCHP_Handle, BREG_Handle, BINT_Handle, const struct BADS_Settings *);
    BERR_Code (*Close)(BADS_Handle);
    BERR_Code (*Init)(BADS_Handle);
    BERR_Code (*GetVersion)(BADS_Handle, BADS_Version *);
    BERR_Code (*GetVersionInfo)(BADS_Handle, BFEC_VersionInfo *);    
    BERR_Code (*GetBondingCapability)(BADS_Handle, unsigned int *);
    BERR_Code (*GetTotalChannels)(BADS_Handle, unsigned int *);
    BERR_Code (*OpenChannel)(BADS_Handle, BADS_ChannelHandle *, unsigned int, const struct BADS_ChannelSettings *);
    BERR_Code (*CloseChannel)(BADS_ChannelHandle);
    BERR_Code (*GetDevice)(BADS_ChannelHandle, BADS_Handle *);
    BERR_Code (*GetChannelDefaultSettings)(BADS_Handle, unsigned int, struct BADS_ChannelSettings *);
    BERR_Code (*GetStatus)(BADS_ChannelHandle, BADS_Status *);
    BERR_Code (*GetLockStatus)(BADS_ChannelHandle, BADS_LockStatus *);
    BERR_Code (*GetSoftDecision)(BADS_ChannelHandle, int16_t nbrToGet, int16_t *iVal, int16_t *qVal, int16_t *nbrGot );
    BERR_Code (*InstallCallback)(BADS_ChannelHandle, BADS_Callback, BADS_CallbackFunc, void *);
    BERR_Code (*GetDefaultAcquireParams)(BADS_InbandParam *);
    BERR_Code (*SetAcquireParams)(BADS_ChannelHandle, const BADS_InbandParam *);
    BERR_Code (*GetAcquireParams)(BADS_ChannelHandle, BADS_InbandParam *);   
    BERR_Code (*Acquire)(BADS_ChannelHandle, BADS_InbandParam *);
    BERR_Code (*EnablePowerSaver)(BADS_ChannelHandle);	
    BERR_Code (*DisablePowerSaver)(BADS_ChannelHandle);
    BERR_Code (*ProcessNotification)(BADS_ChannelHandle, unsigned int event);
    BERR_Code (*SetDaisyChain)(BADS_Handle, bool enableDaisyChain);
    BERR_Code (*GetDaisyChain)(BADS_Handle, bool *isEnableDaisyChain);
    BERR_Code (*ResetStatus)(BADS_ChannelHandle);
    BERR_Code (*GetInterruptEventHandle)(BADS_Handle, BKNI_EventHandle*);
    BERR_Code (*ProcessInterruptEvent)(BADS_Handle);
    BERR_Code (*Untune)(BADS_Handle);
    BERR_Code (*RequestAsyncStatus)(BADS_ChannelHandle);
    BERR_Code (*GetAsyncStatus)(BADS_ChannelHandle, BADS_Status *);
    BERR_Code (*GetScanStatus)(BADS_ChannelHandle, BADS_ScanStatus *);	
    BERR_Code (*ReadSlave)(BADS_ChannelHandle, uint8_t chipAddr, uint32_t subAddr, uint8_t subAddrLen, uint32_t *data, uint8_t dataLen);
    BERR_Code (*WriteSlave)(BADS_ChannelHandle, uint8_t chipAddr, uint32_t subAddr, uint8_t subAddrLen, uint32_t *data, uint8_t dataLen);
    BERR_Code (*SetScanParam)(BADS_ChannelHandle,  struct BADS_ChannelScanSettings *);
    BERR_Code (*GetScanParam)(BADS_ChannelHandle, struct BADS_ChannelScanSettings *);
    BERR_Code (*RequestSpectrumAnalyzerData)(BADS_ChannelHandle, struct BADS_SpectrumSettings *);
    BERR_Code (*GetSpectrumAnalyzerData)(BADS_ChannelHandle, struct BADS_SpectrumData *);
} BADS_ApiFunctTable;
/***************************************************************************
Summary:
    Required daisy chain settings structure for controlling the daisy chain of 31xx devices.

Description:
    Required daisy chain settings structure for controlling the daisy chain of 31xx devices.
    When turned off the RF out is turned off for the particulat 31xx device and hence all the 
    31xx devices connected to the current device wont recieve the RF signal.

See Also:
    BADS_Open()

****************************************************************************/
typedef struct BADS_DaisyChainSettings
{
    bool enable; /* 1 = enable, 0=disable. */
} BADS_DaisyChainSettings;

/***************************************************************************
Summary:
    Enumeration for 

Description:
    This enumeration defines 

See Also:

****************************************************************************/
typedef enum BADS_TransportData
{
    BADS_TransportData_eSerial,         /* GPIO[0:7] on 31xx are configured as input and the transport data is sent out on DS_Data. */
    BADS_TransportData_eGpioSerial,     /* GPIO[1:7] on 31xx are configured as input and the transport data is sent out on GPIO[0]. */
    BADS_TransportData_eGpioParallel,   /* GPIO[0:7] on 31xx are configured as output and the transport data is sent out on GPIO[0:7]. */
    BADS_TransportData_eMax
} BADS_TransportData;

/***************************************************************************
Summary:
    Required default settings structure for Qam In-Band Downstream module.

Description:
    The default setting structure defines the default configuration of
    Qam In-Band Downstream when the device is opened.

See Also:
    BADS_Open()

****************************************************************************/
typedef struct BADS_Settings
{
    unsigned int devId;     /* generic device ID */
    void *hGeneric;         /* generic handle can be used for anything */
    BADS_ApiFunctTable funcPtr;
    bool isOpenDrain; /* true = Open Drain mode, false = Normal mode. */
    BADS_TransportData transportConfig; /* Chooses the trasnport configuration as mentioned in the BADS_TransportData structure. */
    BMEM_Heap_Handle      hHeap;        /* require to be able to share memory with BBS required for 7550 */
    BTMR_Handle           hTmr;        /* for peridic check equired for 7550 */
} BADS_Settings;

/***************************************************************************
Summary:
    Required default settings structure for Qam In-Band Downstream module.

Description:
    The default setting structure defines the default configuration of
    Qam In-Band Downstream channel when the device channel is opened.

See Also:
    BADS_OpenChannel()

****************************************************************************/
#define BADS_SETTINGS_IFFREQ                    (43750000)      /* 43.75 MHz */
typedef struct BADS_ChannelSettings
{
    unsigned long ifFreq; /* IF Frequency in Hertz */
    bool autoAcquire; /* 325X does auto-reacuire or not*/
    bool fastAcquire; /* 325X uses 2X faster acquire time
                        in expense of stability or not*/
} BADS_ChannelSettings;

/***************************************************************************
Summary:
    Scan Parameter settings.

Description:

See Also:
    BADS_SetScanParam()

****************************************************************************/
typedef struct BADS_ChannelScanSettings
{
    bool		AI;			/* Enable/Disable auto invert spectrum */
    bool		QM;			/* Enable/Disable QAM mode search  */
    bool		CO;			/* Enable/Disable Carrier Offset search  */
    bool		TO;			/* Enable/Disable auto baud rate detection  */
    bool		B1024;		/* Enable/Disable Auto Search for 1024Q while receiver is in Annex B mode */
    bool		B256;		/* Enable/Disable Auto Search for 256Q while receiver is in Annex B mode */
    bool		B64;   
    bool		A1024;		 /* Enable/Disable Auto Search for 1024Q while receiver is in Annex A mode  */
    bool		A512;
    bool		A256;
    bool		A128;
    bool		A64;
    bool		A32;
    bool		A16;		/* Enable/Disable Auto Search for 16Q while receiver is in Annex A mode */
    uint32_t    carrierSearch;		/* 256*Hz */
    uint32_t    upperBaudSearch;  /*upper baud search range in Hz */ 
    uint32_t    lowerBaudSearch; /*Lower baud search range in Hz */
} BADS_ChannelScanSettings;

/***************************************************************************
Summary:
    This function opens Qam In-Band Downstream module.

Description:
    This function is responsible for opening BADS module. When BADS is
    opened, it will create a module handle and configure the module based
    on the default settings. Once the device is opened, it must be closed
    before it can be opened again.

Returns:
    TODO:

See Also:
    BADS_Close(), BADS_OpenChannel(), BADS_CloseChannel(),

****************************************************************************/
BERR_Code BADS_Open(
    BADS_Handle *pAds,                  /* [out] Returns handle */
    BCHP_Handle hChip,                  /* [in] Chip handle */
    BREG_Handle hRegister,              /* [in] Register handle */
    BINT_Handle hInterrupt,             /* [in] Interrupt handle, Bcm3250 */
    const BADS_Settings *pDefSettings   /* [in] Default settings */
    );

/***************************************************************************
Summary:
    This function closes Qam In-Band Downstream module.

Description:
    This function is responsible for closing BADS module. Closing BADS
    will free main BADS handle. It is required that all opened
    BDQS channels must be closed before calling this function. If this
    is not done, the results will be unpredicable.

Returns:
    TODO:

See Also:
    BADS_Open(), BADS_CloseChannel()

****************************************************************************/
BERR_Code BADS_Close(
    BADS_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function initialize Qam In-Band Downstream module.

Description:
    This function is responsible for initializing BADS module. The initialize
    function may do none or one or more of the following:
    - Download FW into ADS core
    - Startup ADS core
    - Retrieve information from ADS core regarding ADS static configuration
    - etc

Returns:
    TODO:

See Also:
    BADS_3255_Open(), BADS_3255_Close()

****************************************************************************/
BERR_Code BADS_Init(
    BADS_Handle hDev                    /* [in] Device handle */
    );

/***************************************************************************
Summary:
    This function returns the version information.

Description:
    This function is responsible for returning the version information.

Returns:
    TODO:

See Also:
    BADS_Open()

****************************************************************************/
BERR_Code BADS_GetVersion(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_Version *pVersion              /* [out] Returns version */
    );

/***************************************************************************
Summary:
    This function returns the version information.

Description:
    This function is responsible for returning the core driver version 
    information. It return the majorVersion and minorVersion of the core
    driver.
Returns:
    TODO:

See Also:
    BADS_Open()

****************************************************************************/
BERR_Code BADS_GetVersionInfo(
    BADS_Handle hDev,                        /* [in] Device handle */
    BFEC_VersionInfo *pVersionInfo /* [out] Returns version Info */
    );    
    
/***************************************************************************
Summary:
    This function returns the number of bonded channels stored and reserved
    for Docsis.  The Docsis channels start at DS0 while the video channels
    start at the offset determined by the number of bonded channels returned
    from this function

Description:
    This function is responsible for returning the number of bonded channels.

Returns:
    TODO:

See Also:
    BADS_GetTotalChannels()

****************************************************************************/
BERR_Code BADS_GetBondingCapability(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *bondedChannels         /* [out] Returns the number of bonded channels  */
    );

/***************************************************************************
Summary:
    This function returns the total number of channels supported by
    Qam In-Band Downstream module.

Description:
    This function is responsible for getting total number of channels
    supported by BADS module, since BADS device is implemented as a
    device channel.

Returns:
    TODO:

See Also:
    BADS_OpenChannel(), BADS_ChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_GetTotalChannels(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int *totalChannels         /* [out] Returns total number downstream channels supported */
    );

/***************************************************************************
Summary:
    This function gets default setting for a Qam In-Band Downstream module channel.

Description:
    This function is responsible for returning the default setting for
    channel of BADS. The return default setting is used when opening
    a channel.

Returns:
    TODO:

See Also:
    BADS_OpenChannel()

****************************************************************************/
BERR_Code BADS_GetChannelDefaultSettings(
    BADS_Handle hDev,                   /* [in] Device handle */
    unsigned int channelNo,             /* [in] Channel number to default setting for */
    BADS_ChannelSettings *pChnDefSettings /* [out] Returns channel default setting */
    );

/***************************************************************************
Summary:
    This function opens Qam In-Band Downstream module channel.

Description:
    This function is responsible for opening BADS module channel. When a
    BADS channel is opened, it will create a module channel handle and
    configure the module based on the channel default settings. Once a
    channel is opened, it must be closed before it can be opened again.

Returns:
    TODO:

See Also:
    BADS_CloseChannel(), BADS_GetChannelDefaultSettings()

****************************************************************************/
BERR_Code BADS_OpenChannel(
    BADS_Handle hDev,                   /* [in] Device handle */
    BADS_ChannelHandle *phChn,          /* [out] Returns channel handle */
    unsigned int channelNo,             /* [in] Channel number to open */
    const BADS_ChannelSettings *pChnDefSettings /* [in] Channel default setting */
    );

/***************************************************************************
Summary:
    This function closes Qam In-Band Downstream module channel.

Description:
    This function is responsible for closing BADS module channel. Closing
    BADS channel it will free BADS channel handle. It is required that all
    opened BDQS channels must be closed before closing BADS.

Returns:
    TODO:

See Also:
    BADS_OpenChannel(), BADS_CloseChannel()

****************************************************************************/
BERR_Code BADS_CloseChannel(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets Qam In-Band Downstream module device handle based on
    the device channel handle.

Description:
    This function is responsible returning BADS module handle based on the
    BADS module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_GetDevice(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Handle *pAds                   /* [out] Returns Device handle */
    );


/***************************************************************************
Summary:
    This function gets the status synchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for synchronously getting the complete status for
    a Qam In-Band Downstream module channel. 

Returns:
    TODO:

See Also: BADS_RequestStatus

****************************************************************************/
BERR_Code BADS_GetStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    );


/***************************************************************************
Summary:
    This function requests the status asynchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for requesting the status to be calculated asynchronously for
    a Qam In-Band Downstream module channel. The Qam frontend is responsible to inform 
    the backend when the status is ready either through an interrupt or by any other predetermined
    method.
    
Returns:
    TODO:

See Also: BADS_GetStatus

****************************************************************************/
BERR_Code BADS_RequestAsyncStatus(
    BADS_ChannelHandle hChn            /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function gets the status asynchronously of Qam In-Band Downstream module channel.

Description:
    This function is responsible for asynchronously getting the complete status for
    a Qam In-Band Downstream module channel. BADS_RequestAsyncStatus() is called before to
    notify the frontend to calculate the status and notify the backend asynchronously that 
    the status is ready then BADS_GetAsyncStatus() needs to be called to retrieve the status.

Returns:
    TODO:

See Also: BADS_RequestAsyncStatus

****************************************************************************/
BERR_Code BADS_GetAsyncStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Status *pStatus                /* [out] Returns status */
    );


/***************************************************************************
Summary:
    This function gets the scan status synchronously of a Qam In-Band 
    Downstream module channel.

Description:
    This function is responsible for synchronously getting the scan status 
    for a Qam In-Band Downstream module channel. Please note that this API
    should only be called after calling tune and acquire. Scan Status is an
    acquisition status, it gives the status at the last acquisition and not
    the current status. So, it is only valid if a tune and acquire was 
    issued prior to calling this API. 

Returns:
    TODO:

See Also: 

****************************************************************************/
BERR_Code BADS_GetScanStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_ScanStatus *pScanStatus                /* [out] Returns status */
    );


/***************************************************************************
Summary:
    This function gets the lock status for a Qam In-Band Downstream
    module channel.

Description:
    This function is responsible for getting the lock status
    for a BADS module channel.

    NOTE: Do not call this function from an ISR context.

Returns:
    TODO:

See Also:
    BADS_GetStatus()

****************************************************************************/
BERR_Code BADS_GetLockStatus(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_LockStatus *pLockStatus         /* [out] Returns lock status */
    );

/***************************************************************************
Summary:
    This function gets the I and Q values for soft decision of a
    Qam In-Band Downstream module channel.

Description:
    This function is responsible for getting the I and Q values for soft
    decision of a BADS module channel.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_GetSoftDecision(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    int16_t nbrToGet,                   /* [in] Number values to get */
    int16_t *iVal,                      /* [out] Ptr to array to store output I soft decision */
    int16_t *qVal,                      /* [out] Ptr to array to store output Q soft decision */
    int16_t *nbrGotten                  /* [out] Number of values gotten/read */
    );

/***************************************************************************
Summary:
    This function returns the default acquire parameters for a specific 
    ADS module.

Description:
    This function returns the default acquire parameters for a specific 
    ADS module.

Returns:
    TODO:

See Also:

****************************************************************************/ 
BERR_Code BADS_GetDefaultAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [out] default Inband Parameters */
    );
    
/***************************************************************************
Summary:
    This function sends the acquire parameters for a specific ADS module.

Description:
    This function sends the acquire parameters for a specific ADS module.
    This sets the acquire parameters for a specific acquire.

Returns:
    TODO:

See Also:
    BADS_GetAsyncStatus(), BADS_GetSoftDecision()

****************************************************************************/
BERR_Code BADS_SetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    const BADS_InbandParam *ibParams          /* [in] Inband Parameters to use */
    );

/***************************************************************************
Summary:
    This function retrieves the acquire parameters set for a specific ADS module.

Description:
    This function gets the acquire parameters for a specific ADS module.

Returns:
    TODO:

See Also:

****************************************************************************/ 
BERR_Code BADS_GetAcquireParams(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [out] Inband Parameters to use */
    );
    
/***************************************************************************
Summary:
    This function tries to acquire downstream lock for the specific
    Qam In-Band Downstream module channel.

Description:
    This function starts the process of trying to acquire a specific Qam
    In-Band Downstream channel. Once the acquistion process is started, the
    ADS module will notify the application when its lock status changes
    through a Callback function. ADS will try to maintain lock with the
    downstream signal. If ADS loses lock, it will notify the application and
    automatically try to reacquire the downstream. Once it reacquire downstream
    lock, it will notify the application.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_Acquire(
    BADS_ChannelHandle hChn ,           /* [in] Device channel handle */
    BADS_InbandParam *ibParams          /* [in] Inband Parameters to use */
    );

/***************************************************************************
Summary:
    This function enable the power-saver mode.

Description:
    This function is responsible for enabling the downstream receiver
    power-saver mode.  When the BADS is in the power-saver mode, the
    Qam In-Band Downstream receiver is shutdown. BADS_Acquire() will
    disables power-saver mode automatically. Also BADS_DisablePowerSaver()
    can be used to disable power-saver mode.


Returns:
    TODO:

See Also:
    BADS_Acquire()

****************************************************************************/
BERR_Code BADS_EnablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );


/***************************************************************************
Summary:
    This function disables the power-saver mode.

Description:
    This function is responsible for disabling the downstream receiver
    power-saver mode.  When the BADS is in the power-saver mode, the
    Qam In-Band Downstream receiver is shutdown. 


Returns:
    TODO:

See Also:
    BADS_Acquire()

****************************************************************************/
BERR_Code BADS_DisablePowerSaver(
    BADS_ChannelHandle hChn             /* [in] Device channel handle */
    );

/***************************************************************************
Summary:
    This function is responsible for processing a notificiation for the specific
    Qam In-Band Downstream module channel.

Description:
    This function needs to called when notification is received.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_ProcessNotification(
    BADS_ChannelHandle hChn,                /* [in] Device channel handle */
    unsigned int event                      /* [in] Event code and event data*/
    );

/***************************************************************************
Summary:
    This function is responsible for installing a callback function.

Description:
    This function installs a callback function.

Returns:
    TODO:

See Also:

****************************************************************************/
BERR_Code BADS_InstallCallback(
    BADS_ChannelHandle hChn,            /* [in] Device channel handle */
    BADS_Callback callbackType,         /* [in] Type of callback */
    BADS_CallbackFunc pCallback,        /* [in] Function Ptr to callback */
    void *pParam                        /* [in] Generic parameter send on callback */
    );

/***************************************************************************
Summary:
    This function opens configures (enables/disables) the 31xx device's RF out for daisy chaining.

Description:
    This function opens configures (enables/disables) the 31xx device's RF out for daisy chaining.
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_SetDaisyChain(
    BADS_Handle hDev,       /* [in] Device channel handle */
    bool enableDaisyChain   /* [in] Eanble/disable daisy chain. */
    );

/***************************************************************************
Summary:
    This function opens tells if the 31xx device's RF out daisy chaining is enabled/disabled.

Description:
    This function opens tells if the 31xx device's RF out daisy chaining is enabled/disabled.
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_GetDaisyChain(
    BADS_Handle hDev,           /* [in] Device channel handle */
    bool *isEnableDaisyChain    /* [out] Eanble/disable daisy chain. */
    );

/***************************************************************************
Summary:
    This function resets the 31xx device's FEC bit error and block counters.

Description:
    
Returns:
    TODO:

See Also:   
****************************************************************************/
BERR_Code BADS_ResetStatus(
    BADS_ChannelHandle hChn     /* [in] Device channel handle */
    );

/******************************************************************************
Summary:
   This function returns the BADS interrupt event handle.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_GetInterruptEventHandle(
   BADS_Handle hDev,            /* [in] BADS handle */
   BKNI_EventHandle *phEvent /* [out] event handle */
);

/******************************************************************************
Summary:
   This function returns the BADS interrupt event handle.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_ProcessInterruptEvent(
   BADS_Handle hDev            /* [in] BADS handle */
);

/******************************************************************************
Summary:
   This function stops the BADS timer.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_Untune(
    BADS_Handle hDev			/* [in] BADS handle */
);

/******************************************************************************
Summary:
   This function allows the reading from a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_ReadSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [out] ptr to the data that we will read from the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to read? one to four*/
);

/******************************************************************************
Summary:
   This function allows the writing to a slave device attached to the demod chip.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_WriteSlave(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */
	uint8_t chipAddr,			 /* [in] chip addr of the i2c slave device */
	uint32_t subAddr,			 /* [in] sub addr of the register to read from the slave device */
	uint8_t subAddrLen,			 /* [in] how many bytes is the sub addr? one to four*/
	uint32_t *data,				 /* [in] ptr to the data that we will write to the slave device */
	uint8_t dataLen				 /* [in] how many bytes are we going to write? one to four*/
);

/******************************************************************************
Summary:
   This function will pass the scan parameter to ADS.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_SetScanParam(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_ChannelScanSettings *pChnScanSettings /* [in] Set Channel setting */
);

/******************************************************************************
Summary:
   This function will get the scan parameter from ADS.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_GetScanParam (
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_ChannelScanSettings *pChnScanSettings /* [out] Returns channel default setting */
);

/******************************************************************************
Summary:
   This function sends request for spectrum analyzer data to the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/
BERR_Code BADS_RequestSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumSettings *pSettings /* [in] spectrum settings */
    );

/******************************************************************************
Summary:
   This function gets spectrum analyzer data from the LEAP.
Description:
  
Returns:
   BERR_Code
******************************************************************************/    
BERR_Code BADS_GetSpectrumAnalyzerData(
    BADS_ChannelHandle hChn,     /* [in] Device channel handle */ 
    BADS_SpectrumData  *pSpectrumData /* [out] spectrum Data*/
    );
    
#ifdef __cplusplus
}
#endif

#endif

