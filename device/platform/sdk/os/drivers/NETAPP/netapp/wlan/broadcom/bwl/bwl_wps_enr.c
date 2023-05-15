/*
 * Broadcom WPS Enrollee
 *
 * Copyright (C) 2010, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: bwl_wps_enr.c,v 1.3 2010/06/17 00:39:02 Exp $
 */
#include <stdio.h>
#include <signal.h>

#include <unistd.h>
#include <wpserror.h>
#include <portability.h>
#include <reg_prototlv.h>
#include <wps_enrapi.h>
#include <wps_sta.h>
#include <wps_enr_osl.h>
#include <wps_version.h>
#include <wps_staeapsm.h>
#include <wlioctl.h>

#include "bwl.h"

#if !defined(MOD_VERSION_STR)
#error "wps_version.h doesn't exist !"
#endif

#ifdef _TUDEBUGTRACE
void print_buf(unsigned char *buff, int buflen);
#endif

extern char *ether_ntoa(const struct ether_addr *addr);

#define WPS_VERSION_STRING
#define WPS_EAP_DATA_MAX_LENGTH         2048
#define WPS_EAP_READ_DATA_TIMEOUT         3

static char def_pin[9] = "12345670\0";

int
display_aplist(wps_ap_list_info_t *ap)
{
	int i = 0;
	if (!ap)
		return 0;

	printf("-------------------------------------\n");
	while (ap->used == TRUE) {
		printf(" %d :  ", i);
		printf("SSID:%s  ", ap->ssid);
		printf("BSSID:%s  ", ether_ntoa((struct ether_addr *)ap->BSSID));
		printf("Channel:%d  ", ap->channel);
		if (ap->wep)
			printf("WEP");
		printf("\n");
		ap++;
		i++;
	}

	printf("-------------------------------------\n");
	return 0;
}

/*
 * find an AP with PBC active or timeout.
 * Returns SSID and BSSID.
 * Note : when we join the SSID, the bssid of the AP might be different
 * than this bssid, in case of multiple AP in the ESS ...
 * Don't know what to do in that case if roaming is enabled ...
 */
int
find_pbc_ap(char * bssid, char *ssid, uint8 *wsec)
{
	int pbc_ret = PBC_NOT_FOUND;
	char start = true;
	wps_ap_list_info_t *wpsaplist;

	/* add wps ie to probe  */
	add_wps_ie(NULL, 0);

	while (PBC_NOT_FOUND == pbc_ret) {
		wpsaplist = create_aplist();
		if (wpsaplist) {
			wps_get_aplist(wpsaplist, wpsaplist);
			display_aplist(wpsaplist);
			pbc_ret = wps_get_pbc_ap(wpsaplist, bssid, ssid,
				wsec, get_current_time(), start);
			start = false;
		}
		sleep(1);
	}

	rem_wps_ie(NULL, 0);
	if (pbc_ret != PBC_FOUND_OK) {
		printf("Could not find a PBC enabled AP, %s\n",
			(pbc_ret == PBC_OVERLAP) ?
			"OVERLAP" : "TIMEOUT");
		return 0;
	}

	return 1;
}

/*
 * Fill up the device info and pass it to WPS.
 * This will need to be tailored to specific platforms (read from a file,
 * nvram ...)
 */
void
config_init()
{
	DevInfo info;
	unsigned char mac[6];
	char uuid[16] = {0x22, 0x21, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0xa, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

	/* fill in device specific info. The way this information is stored is app specific */
	/* Would be good to document all of these ...  */

	memset((char *)(&info), 0, sizeof(info));
	info.version = 0x10;

	/* MAC addr */
	wps_osl_get_mac(mac);
	memcpy(info.macAddr, mac, 6);

	/* generate UUID base on the MAC addr */
	memcpy(info.uuid, uuid, 16);
	memcpy(info.uuid + 10, mac, 6);

	strcpy(info.deviceName, "Broadcom Client");
	info.primDeviceCategory = 1;
	info.primDeviceOui = 0x0050F204;
	info.primDeviceSubCategory = 1;
	strcpy(info.manufacturer, "Broadcom");
	strcpy(info.modelName, "WPS Wireless Client");
	strcpy(info.modelNumber, "1234");
	strcpy(info.serialNumber, "5678");
	info.configMethods = 0x008C;
	info.authTypeFlags = 0x003f;
	info.encrTypeFlags = 0x000f;
	info.connTypeFlags = 0x01;
	/* rfBand will update again later */
	info.rfBand = WPS_RFBAND_24GHZ;
	info.osVersion = 0x80000000;
	info.featureId = 0x80000000;
	wps_enr_config_init(&info);
}

/* Main loop. */
int
registration_loop(unsigned long start_time)
{
	uint32 retVal;
	char buf[WPS_EAP_DATA_MAX_LENGTH];
	uint32 len;
	char *sendBuf;
	unsigned long now;
	int last_recv_msg, last_sent_msg;
	int state;
	char msg_type;

	now = get_current_time();

	/*
	 * start the process by sending the eapol start . Created from the
	 * Enrollee SM Initialize.
	 */
	len = wps_get_msg_to_send(&sendBuf, (uint32)now);

#ifdef _TUDEBUGTRACE
	print_buf((unsigned char*)sendBuf, len);
#endif

	if (sendBuf) {
		send_eapol_packet(sendBuf, len);
		printf("Send EAPOL-Start\n");
	}
	else {
		/* this means the system is not initialized */
		return WPS_ERR_NOT_INITIALIZED;
	}

	/* loop till we are done or failed */
	while (1) {
		len = WPS_EAP_DATA_MAX_LENGTH;

		now = get_current_time();

		if (now > start_time + 120) {
			printf("Overall protocol timeout \n");
			return REG_FAILURE;
		}

		if ((retVal = wait_for_eapol_packet(buf, &len, WPS_EAP_READ_DATA_TIMEOUT))
			== WPS_SUCCESS) {

			/* Show receive message */
			msg_type = wps_get_ap_msg_type(buf, len);
				printf("Receive EAP-Request%s\n",
					wps_get_msg_string((int)msg_type));

			/* process ap message */
			retVal = wps_process_ap_msg(buf, len);

			/* check return code to do more things */
			if (retVal == WPS_SEND_MSG_CONT ||
				retVal == WPS_SEND_MSG_SUCCESS ||
				retVal == WPS_SEND_MSG_ERROR) {
				len = wps_get_msg_to_send(&sendBuf, now);
				if (sendBuf) {
					msg_type = sendBuf[WPS_MSGTYPE_OFFSET];

					wps_eap_send_msg(sendBuf, len);
					printf("Send EAP-Response%s\n",
						wps_get_msg_string((int)msg_type));
				}

				/*
				 * sleep a short time for driver to send last WPS DONE message,
				 * otherwise doing leave_network before do_wpa_psk in
				 * enroll_device() may cause driver to drop the last WPS DONE
				 * message if it not transmit.
				 */
				if (retVal == WPS_SEND_MSG_SUCCESS ||
				    retVal == WPS_SEND_MSG_ERROR)
					usleep(2000);

				/* over-write retVal */
				if (retVal == WPS_SEND_MSG_SUCCESS)
					retVal = WPS_SUCCESS;
				else if (retVal == WPS_SEND_MSG_ERROR)
					retVal = REG_FAILURE;
				else
					retVal = WPS_CONT;
			}
			else if (retVal == EAP_FAILURE) {
				/* we received an eap failure from registrar */
				/*
				 * check if this is coming AFTER the protocol passed the M2
				 * mark or is the end of the discovery after M2D.
				 */
				last_recv_msg = wps_get_recv_msg_id();
				printf("Received eap failure, last recv msg EAP-Request%s\n",
					wps_get_msg_string(last_recv_msg));
				if (last_recv_msg > WPS_ID_MESSAGE_M2D)
					return REG_FAILURE;
				else
					return WPS_CONT;
			}
			/* special case, without doing wps_eap_create_pkt */
			else if (retVal == WPS_SEND_MSG_IDRESP) {
				len = wps_get_msg_to_send(&sendBuf, now);
				if (sendBuf) {
					send_eapol_packet(sendBuf, len);
					printf("Send EAP-Response / Identity\n");
				}
			}

			/* SUCCESS or FAILURE */
			if (retVal == WPS_SUCCESS || retVal == REG_FAILURE) {
				return retVal;
			}
		}
		/* timeout with no data, should we re-transmit ? */
		else if (retVal == EAP_TIMEOUT) {
			/* check eap receive timer. It might be time to re-transmit */
			/*
			 * Do we need this API ? We could just count how many
			 * times we re-transmit right here.
			 */
			if ((retVal = wps_eap_check_timer(now)) == WPS_SEND_MSG_CONT) {
				len = wps_get_retrans_msg_to_send(&sendBuf, now, &msg_type);
				if (sendBuf) {
					state = wps_get_eap_state();

					if (state == EAPOL_START_SENT)
						printf("Re-Send EAPOL-Start\n");
					else if (state == EAP_IDENTITY_SENT)
						printf("Re-Send EAP-Response / Identity\n");
					else
						printf("Re-Send EAP-Response%s\n",
							wps_get_msg_string((int)msg_type));

					send_eapol_packet(sendBuf, len);
				}
			}
			/* re-transmission count exceeded, give up */
			else if (retVal == EAP_TIMEOUT) {
				last_recv_msg = wps_get_recv_msg_id();

				if (last_recv_msg == WPS_ID_MESSAGE_M2D) {
					printf("M2D Wait timeout, again.\n");
					return WPS_CONT;
				}
				else if (last_recv_msg > WPS_ID_MESSAGE_M2D) {
					last_sent_msg = wps_get_sent_msg_id();
					printf("Timeout, give up. Last recv/sent msg "
						"[EAP-Response%s/EAP-Request%s]\n",
						wps_get_msg_string(last_recv_msg),
						wps_get_msg_string(last_sent_msg));
					return REG_FAILURE;
				}
				else {
					printf("Re-transmission count exceeded, again\n");
					return WPS_CONT;
				}
			}
		}
	}

	return WPS_SUCCESS;
}

int
interactive_start(char *bssid, char *ssid, uint8 *wsec, char **pin)
{
	char inp[3], inp2[3];
	bool b_tryAgain = true;
	wps_ap_list_info_t *wpsaplist;
	int start_ok = false;
	int i, valc;

	while (b_tryAgain) {
		printf("\nOptions:\n");
		printf("0. Quit\n");
		printf("1. Get configured\n");
		printf("2. Get configured via push-button\n");
		printf("Enter selection: ");
		fgets(inp, 3, stdin);
		fflush(stdin);

		if (0 == strlen(inp)-1) {
			/* We got no input */
			printf("Error: Invalid input.\n");
			continue;
		}

		switch (inp[0]) {
		case '0':
			printf("\nShutting down...\n");
			b_tryAgain = false;
			break;

		case '1': /* Get configured */

			/* Not doing PBC */
			wpsaplist = create_aplist();
			if (wpsaplist) {
				wps_get_aplist(wpsaplist, wpsaplist);
				printf("--------- WPS Enabled AP list -----------\n");
				display_aplist(wpsaplist);
			}
		scan_retry:
			printf("Choose one AP to start!!\n");
			printf("Enter selection: ('a' for scan again, 'q' for quit)");
			fgets(inp2, 3, stdin);
			fflush(stdin);
			if ('a' == inp2[0]) {
				wpsaplist = create_aplist();
				if (wpsaplist) {
					wps_get_aplist(wpsaplist, wpsaplist);
					printf("--------- WPS Enabled AP list -----------\n");
					display_aplist(wpsaplist);
				}
				goto scan_retry;
			}
			else if ('q' == inp2[0]) {
				printf("\nShutting down...\n");
				b_tryAgain = false;
				break;
			}
			else if ('0' <= inp2[0] && '9' >= inp2[0]) {
				valc = inp2[0]-48;
				if (wpsaplist[valc].used == TRUE) {
					for (i = 0; i < 6; i++)
						bssid[i] = wpsaplist[valc].BSSID[i];
					memcpy(ssid, wpsaplist[valc].ssid,
						wpsaplist[valc].ssidLen);
					ssid[wpsaplist[valc].ssidLen] = '\0';
					*wsec = wpsaplist[valc].wep;
					start_ok = true;
					b_tryAgain = false;
				}
				else {
					printf("Type error, incorrect number !\n");
					goto scan_retry;
				}
			}
			else {
				printf("Type error!\n");
				goto scan_retry;
			}

			/*  if pin unset, use default */
			if (!*pin) {
				*pin = def_pin;
				printf("\n\nStation Pin not specified, use default Pin %s\n\n",
					def_pin);
			}

			break;
		case '2': /*  Get configured via push-button */
			start_ok = find_pbc_ap((char *)bssid, (char *)ssid, wsec);
			if (start_ok) {
				b_tryAgain = false;
				*pin = NULL;
			}
			break;

		default:
			printf("ERROR: Invalid input.\n");
			break;
		}
	}

	return start_ok;
}
int
enroll_device(char *pin, char *ssid, uint8 wsec, char *bssid, char* key, uint32 key_len)
{
	int res;
	unsigned long start_time;

	start_time = get_current_time();

	while (1) {

		wps_start_enrollment(pin, get_current_time());

		/* registration loop */
		/*
		 * exits with either success, failure or indication that
		 * the registrar has not started its end of the protocol yet.
		*/
		if ((res = registration_loop(start_time)) == WPS_SUCCESS) {
			char keystr[65];
			int len = 0;
			char ssid[SIZE_SSID_LENGTH];
			WpsEnrCred *credential = (WpsEnrCred *)alloc_init(sizeof(WpsEnrCred));

			printf("WPS Protocol SUCCEEDED !!\n");

			/* get credentials */
			wps_get_ssid(ssid, &len);
			wps_get_credentials(credential, ssid, len);
			printf("SSID = %s\n", credential->ssid);
			printf("Key Mgmt type is %s\n", credential->keyMgmt);
			strncpy(keystr, credential->nwKey, credential->nwKeyLen);
			keystr[credential->nwKeyLen] = 0;
			if (key != NULL)
			{
			    strncpy(key, keystr, key_len);
			}
			printf("Key : %s\n", keystr);
			if (credential->encrType == ENCRYPT_NONE) {
				printf("Encryption : NONE\n");
			}
			else {
				if (credential->encrType & ENCRYPT_WEP)
					printf("Encryption :  WEP\n");
				if (credential->encrType & ENCRYPT_TKIP)
					printf("Encryption :  TKIP\n");
				if (credential->encrType & ENCRYPT_AES)
					printf("Encryption :  AES\n");
			}

			leave_network();
			sleep(1);

			/* Apply to driver */
			printf("\nApply security to driver ... ");
			fflush(stdout);
			if (do_wpa_psk(credential)) {
				printf("Fail !!\n\n");
			}
			else {
				printf("Success !!\n\n");
			}
			break;
		}
		else if (res == WPS_CONT) {
			/* Do enrollement again */
			/* leave network before join again */
			leave_network();
			sleep(1);

			/* Do join again */
			join_network_with_bssid(ssid, wsec, bssid);
		}
		else {
			printf("WPS Protocol FAILED \n");
			break;
		}
	}

	wps_cleanup();

	return 0;
}

int
set_mac_address(char *mac_string, char *mac_bin)
{
	int i = 0;
	char *endptr, *nptr;
	long val;

	nptr = mac_string;

	do {
		val = strtol(nptr, &endptr, 16);
		if (val > 255) {
			printf("invalid MAC address\n");
			return -1;
		}

		if (endptr == nptr) {
			/* no more digits. */
			if (i != 6) {
				printf("invalid MAC address\n");
				return -1;
			}
			return 0;
		}

		if (i >= 6) {
			printf("invalid MAC address\n");
			return -1;
		}

		mac_bin[i++] = val;
		nptr = endptr+1;
	} while (nptr[0]);

	if (i != 6) {
		printf("invalid MAC address\n");
		return -1;
	}

	return 0;
}


#ifdef _TUDEBUGTRACE
void
print_buf(unsigned char *buff, int buflen)
{
	int i;
	printf("\n print buf : \n");
	for (i = 0; i < buflen; i++) {
		printf("%02X ", buff[i]);
		if (!((i+1)%16))
			printf("\n");
	}
	printf("\n");
}
#endif

void
hup_hdlr(int sig)
{
	/*
	 * In case we are in find_pbc_ap loop,
	 * force to remove probe request pbc ie
	 */
	rem_wps_ie(NULL, 0);
	wps_osl_deinit();
	exit(0);
}

int32 WPS_Init(void)
{
	char if_name[10] = "eth1";

	/* we need to specify the if name before anything else */
	wps_osl_set_ifname( if_name );

	/* establish a handler to handle SIGTERM. */
	signal(SIGINT, hup_hdlr);


    return 0;
}


void WPS_Uninit(void)
{
    wps_osl_deinit();
}
