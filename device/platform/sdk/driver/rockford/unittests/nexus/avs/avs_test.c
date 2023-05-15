/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: avs_test.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 10/9/12 4:33p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/avs/avs_test.c $
 * 
 * Hydra_Software_Devel/3   10/9/12 4:33p rjlewis
 * SW7425-2218: checkin for Mike.  Updated test.
 * 
 * Hydra_Software_Devel/2   10/9/12 4:25p rjlewis
 * SW7425-2218: expanded test.
 * 
 * Hydra_Software_Devel/1   10/9/12 3:46p rjlewis
 * SW7425-4011: AVS extensions test.
 * 
 *****************************************************************************/
#include <stdio.h>

#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_avs.h"
#include "nexus_avs_extension.h"

BDBG_MODULE(boot);

typedef enum { Central, Remote } oscillator_t;

#define DIVIDER_DEFINE 1000
#define COUNTER_MAX    0x7FFF
#define EDGE_COUNT     2

/* This converts an oscillator value to a frequency value */
static unsigned long ConvertThresholdValue_fromRegister(oscillator_t oscillator, uint32_t reg)
{
	if (oscillator == Central)
		return reg * (2 * 108 * DIVIDER_DEFINE) / (COUNTER_MAX * EDGE_COUNT);
	else
		return reg * (108 * DIVIDER_DEFINE) / (COUNTER_MAX * EDGE_COUNT);
}

static void print_oscillator(char *what_kind, oscillator_t oscillator, int which_one, uint32_t current, uint32_t lower, uint32_t upper)
{
	printf("%s %2d %08x %08x %08x (%c %c)", what_kind, which_one, current, lower, upper, (current<=lower)?'L':'H', (current<=upper)?'L':'H');
#if 1
	{
		float f_current = ConvertThresholdValue_fromRegister(oscillator, current);
		float f_lower   = ConvertThresholdValue_fromRegister(oscillator, lower);
		float f_upper   = ConvertThresholdValue_fromRegister(oscillator, upper);
		printf(" c=%#6.3f l=%#6.3f u=%#6.3f", f_current/1000., f_lower/1000., f_upper/1000.);
		printf(" (ld=%+#6.3f, ud=%+#6.3f)", (f_current - f_lower)/1000., (f_current - f_upper)/1000.);
	}
#endif
	printf("\n");
}

static void dumpRegisters(void)
{
	unsigned i, centrals, remotes;
	uint32_t status, upper, lower;

	centrals = NEXUS_GetAvsNumberOfCentral();
	for (i=0; i<centrals; i++) {
		status = NEXUS_GetAvsCentralOscillatorStatus(i);
		lower = NEXUS_GetAvsLowerCentralThreshold(i);
		upper = NEXUS_GetAvsHigherCentralThreshold(i);
		print_oscillator("Central:", Central, i, status, lower, upper);
	}
	remotes = NEXUS_GetAvsNumberOfRemotes();
	for (i=0; i<remotes; i++) {
		status = NEXUS_GetAvsRemoteOscillatorStatus(i);
		lower = NEXUS_GetAvsLowerRemoteThreshold(i);
		upper =  NEXUS_GetAvsHigherRemoteThreshold(i);
		print_oscillator("Remote:", Remote, i, status, lower, upper);
	}
}

int main(void)
{
    NEXUS_Error rc;

/*    while (1) { */
		{
    		NEXUS_PlatformSettings platformSettings;
        NEXUS_PlatformStatus platformStatus;
        char versionString[256];

        /* NOTE: can't use KNI or DBG before NEXUS_Platform_Init or after NEXUS_Platform_Uninit. Nexus/magnum stack must be up. */
        fprintf(stderr, "Ready to initialize\n");

    		NEXUS_Platform_GetDefaultSettings(&platformSettings);
    		platformSettings.openFrontend = false;
    		rc = NEXUS_Platform_Init(&platformSettings);
        if ( rc ) {
            fprintf(stderr, "Unable to initialize nexus: %d\n", rc);
            return -1;
        }

				{
					unsigned maxDacValue;
					unsigned minDacValue;
					
					maxDacValue = NEXUS_GetAvsMaxDACValue();
					printf("Max DAC value is: %x (%d)\n", maxDacValue, maxDacValue);
					minDacValue = NEXUS_GetAvsMinDACValue();
					printf("Min DAC value is: %x (%d)\n", minDacValue, minDacValue);
				}

				{
					struct NEXUS_AvsExtensionStatus avsExtStatus;
					NEXUS_GetAvsExtensionStatus(&avsExtStatus);
					printf("VdacPower:  %d\n", avsExtStatus.VDAC_Power);
					printf("VdacHizN:   %d\n", avsExtStatus.VDAC_Hiz_n);
					printf("DacPgrmEnb: %d\n", avsExtStatus.DAC_PGRM_ENB);
					printf("DacCode:    %d\n", avsExtStatus.DAC_CODE);
					printf("PvtPwrDwn:  %d\n", avsExtStatus.Pvt_PowerDown);
				}

				{
					unsigned loopCounter;
					unsigned lowCenEnb, hiCenEnb;
					unsigned lowRemEnb, hiRemEnb;
					printf("Central Oscillator Enables:\n");
					for (loopCounter = 0; loopCounter < NEXUS_GetAvsNumberOfCentral(); loopCounter++) 
					{
						lowCenEnb = NEXUS_GetAvsLowerCentralThresholdEnable(loopCounter);
						hiCenEnb  = NEXUS_GetAvsHigherCentralThresholdEnable(loopCounter);
						printf("[%02d]: loCenEnb %d, hiCenEnb %d\n", loopCounter, lowCenEnb, hiCenEnb);
					}
					printf("Remote Oscillator Enables:\n");
					for (loopCounter = 0; loopCounter < NEXUS_GetAvsNumberOfRemotes(); loopCounter++) 
					{
						lowRemEnb = NEXUS_GetAvsLowerRemoteThresholdEnable(loopCounter);
						hiRemEnb  = NEXUS_GetAvsHigherRemoteThresholdEnable(loopCounter);
						printf("[%02d]: loRemEnb %d, hiRemEnb %d\n", loopCounter, lowRemEnb, hiRemEnb);
					}
				}

				NEXUS_LockAvs(eAvsLockRegisters);
				printf("AVS Locked... no register updates, but tracking will continue (hit return to continue)\n");
				getchar();

				{
						uint32_t current;
						unsigned loopCounter;
						current = NEXUS_GetAvsDAC();
						printf("Old DAC value is: %x (%d)\n", current, current);
						current -= 16; /* increase the voltage (by decreasing the DAC) by 10mv */

						NEXUS_SetAvsDAC(current);
						for (loopCounter = 0; loopCounter < 40; loopCounter++)
						{
							current = NEXUS_GetAvsDAC();
							printf("New DAC value is: %x (%d)\n", current, current);
							BKNI_Sleep(500);
						}
				}

				NEXUS_LockAvs(eAvsPause);
				printf("AVS Locked... no register updates, AND no tracking (hit return to continue)\n");
				getchar();

				{
						uint32_t current;
						unsigned loopCounter;
						current = NEXUS_GetAvsDAC();
						printf("Old DAC value is: %x (%d)\n", current, current);
						current -= 16; /* increase the voltage (by decreasing the DAC) by 10mv */

						NEXUS_SetAvsDAC(current);
						for (loopCounter = 0; loopCounter < 40; loopCounter++)
						{
							current = NEXUS_GetAvsDAC();
							printf("New DAC value is: %x (%d)\n", current, current);
							BKNI_Sleep(500);
						}
				}

				NEXUS_UnlockAvs();
				printf("AVS Unlocked... (hit return to continue)\n");
				getchar();

				{
						uint32_t current;
						unsigned loopCounter;
						current = NEXUS_GetAvsDAC();
						printf("Old DAC value is: %x (%d)\n", current, current);
						for (loopCounter = 0; loopCounter < 40; loopCounter++)
						{
							current = NEXUS_GetAvsDAC();
							printf("New DAC value is: %x (%d)\n", current, current);
							BKNI_Sleep(500);
						}
				}

				{
						NEXUS_AvsStatus pvt_status;
						NEXUS_GetAvsStatus(&pvt_status);
						printf("From the Public Nexus API:\n");
						printf("Voltage 1p1_0 : %f V\n", (double)pvt_status.voltage / 1000.0);
						printf("Temperature : %f C\n", (double)pvt_status.temperature / 1000.0);
				}

				{
					AvsTestData testData;
					NEXUS_AvsGetTestData(&testData);
					printf("From the Private (Test) Nexus API:\n");
					printf("Valid          : %d\n", testData.valid);
					printf("Last DAC       : %d\n", testData.last_dac);
					printf("Last Temp      : %d\n", testData.last_temp);
					printf("Register 0p99  : %4d\n", testData.V_0p99);
					printf("Register 1p1_0 : %4d\n", testData.V_1p1_0);
					printf("Register 1p1_1 : %4d\n", testData.V_1p1_1);
					printf("Register 2p75  : %4d\n", testData.V_2p75);
					printf("Register 3p63  : %4d\n", testData.V_3p63);
					printf("Voltage 0p99   : %f V\n", (double)testData.last_voltage_0p99/ 1000.0);
					printf("Voltage 1p1_0  : %f V\n", (double)testData.last_voltage_1p1_0/ 1000.0);
					printf("Voltage 1p1_1  : %f V\n", (double)testData.last_voltage_1p1_1/ 1000.0);
					printf("Voltage 2p75   : %f V\n", (double)testData.last_voltage_2p75/ 1000.0);
					printf("Voltage 3p63   : %f V\n", (double)testData.last_voltage_3p63/ 1000.0);
				}

				NEXUS_UnlockAvs();
				printf("AVS Unlocked... (hit return to continue)\n");
				getchar();

				dumpRegisters();
				printf("(hit return to continue)\n");
				getchar();

        rc = NEXUS_Platform_GetStatus(&platformStatus);
        BDBG_ASSERT(!rc);
        NEXUS_Platform_GetReleaseVersion(versionString, 256);

        BDBG_WRN(("Initialization of BCM%04x Nexus, release %s, complete.\nPress ENTER to uninit.", platformStatus.chipId, versionString));
        getchar();

        NEXUS_Platform_Uninit();

/*        fprintf(stderr, "Uninit complete. Press ENTER to init again.\n");  */
        fprintf(stderr, "Uninit complete. Press ENTER to end test.\n");
        getchar();
    }

    return 0;
}


