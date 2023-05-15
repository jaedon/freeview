/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: filterts.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 1/30/12 10:04a $
 *
 * Module Description: Converts startcode index to bcmplayer index
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/utils/filterts.c $
 * 
 * 8   1/30/12 10:04a erickson
 * SWDEPRECATED-2425: allow decimal pids
 * 
 * 7   11/4/11 4:50p erickson
 * SW7425-1657: fix -timestamp output, add -strip_timestamp option
 * 
 * 6   10/28/09 1:29p erickson
 * SW7405-3287: add -pts_offset option
 *
 * 5   9/14/09 11:46a jtna
 * SW3548-2462: added packetnum to show which packet is out of sync
 *
 * 4   4/16/08 10:35a erickson
 * PR40307: added -pktsz support (for 204 byte streams)
 *
 * 3   4/30/07 1:09p erickson
 * PR30310: added TIMING_MARKER, PICTURE_TAG. changed debug code to
 * function.
 *
 * 2   2/8/07 10:58a erickson
 * PR22543: added -remap option for changing pids
 *
 * Irvine_BSEAVSW_Devel/6   2/16/06 4:24p erickson
 * PR17108: added -timestamp support
 *
 * Irvine_BSEAVSW_Devel/5   9/14/05 2:24p erickson
 * PR17148: converted to BCHP_7411_REV
 *
 * Irvine_BSEAVSW_Devel/4   9/1/05 4:55p erickson
 * PR16964: fix cmdline bug
 *
 * Irvine_BSEAVSW_Devel/3   9/1/05 3:36p erickson
 * PR16964: added -btp option and all-pass option
 *
 * Irvine_BSEAVSW_Devel/2   8/8/05 3:57p erickson
 * PR16138: added option to strip transport header and generate PES or ES
 * stream (for one pid)
 *
 *************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "ts_utils.h"

int main(int argc, char **argv)
{
#define MAXPIDS 20
    unsigned char *buf;
    unsigned short pidlist[MAXPIDS];
    int totalpids;
    int strip = 0;
    int btp_strip = 0;
    int curarg = 1;
    int timestamp_offset = 0;
    bool strip_timestamp = false;
    int remap = 0;
    int total_packets = 0;
    int packetsize = 188;
    int packetnum = 1;
    uint32_t pts_offset = 0;

    struct {
        bool in_process;
        int data_start_byte;
        int data_end_packet;
        int data_end_byte;
        int count;

        bool data_start_byte_hack_on; /* We can't send data_end_packet 1 or 3 through the 7411c,
                            so we have to add a packet. However, adding a packet makes trimming start and end impossible.
                            However, because of another bug in the 7411c, we stick in a 0x1FFF packet, and it is
                            counted in the data_end_packet count, but the data_start_byte is still applied on the first
                            video packet. So we have to make filterts work the same way. */
    } btp;

    memset(&btp, 0, sizeof(btp));

    /* at least one arg is required. you must filter or strip */
    if (argc < 2) {
        fprintf(stderr,
        "Usage: %s [-s|-btp] [pid1 [pid2 ...]]\n"
        "  stdin is MPEG2 Transport in. stdout is output.\n"
        "  -s          = strip transport headers (only valid if one pid)\n"
        "  -timestamp  = packets have 4 byte timestamp prepended to each packet\n"
        "  -strip_timestamp = strip prepended timestamp\n"
        "  -pktsz #    = specify size of the packets\n"
        "  -btp        = parse stream using embedded PROCESS BTP packets (only valid if one pid)\n"
        "  -remap      = rewrite pid1 as pid2\n"
        "  -pts_offset # = add a PTS offset to each specified pid. pass through other pids unmodified.\n"
        , argv[0]);
        exit(1);
    }

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-s")) {
            strip = 1;
        }
        else if (!strcmp(argv[curarg], "-btp")) {
            btp_strip = 1;
        }
        else if (!strcmp(argv[curarg], "-remap")) {
            remap = 1;
        }
        else if (!strcmp(argv[curarg], "-pts_offset") && curarg+1 < argc) {
            pts_offset = strtoul(argv[++curarg], NULL, 0);
        }
        else if (!strcmp(argv[curarg], "-timestamp")) {
            timestamp_offset = 4;
            packetsize += timestamp_offset;
        }
        else if (!strcmp(argv[curarg], "-strip_timestamp")) {
            strip_timestamp = true;
        }
        else if (!strcmp(argv[curarg], "-pktsz") && curarg+1 < argc) {
            packetsize = atoi(argv[++curarg]);
        }
        else {
            break;
        }
        curarg++;
    }
    if (btp_strip && strip) {
        fprintf(stderr, "ERROR: Cannot specific both -s and -btp.\n");
        exit(1);
    }
    if (remap && strip) {
        fprintf(stderr, "ERROR: Cannot specific both -s and -remap.\n");
        exit(1);
    }
    if (pts_offset && strip) {
        fprintf(stderr, "ERROR: Cannot specific both -s and -pts_offset.\n");
        exit(1);
    }

    btp.in_process = false;

    for (totalpids=0; totalpids<MAXPIDS && curarg<argc; totalpids++,curarg++)
        pidlist[totalpids] = strtoul(argv[curarg], NULL, 0);

    /* you must be doing something */
    assert(totalpids || strip || btp_strip);

    if ((btp_strip || strip) && totalpids > 1) {
        fprintf(stderr, "ERROR: Stripping transport headers is only valid for one pid.\n");
        exit(1);
    }
    if (remap && totalpids != 2) {
        fprintf(stderr, "ERROR: -remap requires exactly two pids.\n");
        exit(1);
    }

#define READ_SIZE (packetsize*1024)

    buf = malloc(READ_SIZE);

    while (!feof(stdin)) {
        int i, n, pos;

        n = fread(buf, 1, READ_SIZE, stdin);
        if (n <= 0)
            break;

        for (pos=0;pos<n;pos+=packetsize, packetnum++) {
            unsigned offset = 0;
            unsigned payload_start = 0;
            unsigned char *pkt = &buf[pos];
            bool selected_pid = false;

            pkt += timestamp_offset;

            if (pkt[0] != 0x47) {
                fprintf(stderr, "ERROR: Sync byte missing at packet %d. Invalid TS stream.\n", packetnum);
                exit(1);
            }

            if (remap) {
                if (b_get_pid(pkt) == pidlist[0]) {
                    /* remove old pid */
                    pkt[1] &= ~0x1f;
                    pkt[2] = 0;
                    /* set new pid */
                    pkt[1] |= (pidlist[1]>>8) & 0x1f;
                    pkt[2] = pidlist[1] & 0xff;
                }
                fwrite(&pkt[offset], 1, packetsize-offset, stdout);
                total_packets++;
                continue;
            }

            if (totalpids) {
                for (i=0;i<totalpids;i++) {
                    if (b_get_pid(pkt) == pidlist[i]) {
                        selected_pid = true;
                        break;
                    }
                }
                if (!selected_pid && !pts_offset) {
#if BCHP_7411_VER == BCHP_VER_C0
                    /* for 7411c even a pid that's filtered out counts against the data_end_packet count */
                    if (btp.in_process) {
                        btp.count++;
                    }
#endif
                    total_packets++;
                    continue;
                }
            }

            payload_start = 4;
            if (pkt[3] & 0x20) {
                /* is adaptation field present? */
                payload_start += pkt[4] + 1; /* then add the adaptation_field_length and 1 for the length field itself */
            }

            /* strip transport header */
            if (btp_strip || strip) {
                offset = payload_start;
            }

            /* add a pts_offset to the PES header */
            if (selected_pid && pts_offset) {
                /* this code assumes the PES header does not span a transport packet */
                unsigned sccount = 0;
                for (i=payload_start; i<packetsize - 16; i++) {
                    if (sccount == 3) {
                        if (b_is_pes_stream_id(pkt[i])) {
                            b_pes_header pes_header;
                            b_get_pes_header(&pkt[i], &pes_header);
                            if (pes_header.pes_type == b_pes_packet_type_pes) {
                                pes_header.pts += pts_offset;
                                pes_header.dts += pts_offset;
                                b_set_pes_header(&pkt[i], &pes_header);
                            }
                            break;
                        }
                        sccount = 0;
                    }
                    sccount = b_check_for_start_code(pkt[i], sccount);
                }
            }

            /* btp_strip process:
            1. Check if it's a PROCESS BTP.
            2. If yes, then reset the btp_strip state. (Warn if previous PROCESS not complete.)
            3. If no, continue with current btp_strip state.
            4. If not in btp_strip state, packet should be discarded. Give warning if not another BTP.
            5. Otherwise strip the ES data as appropriate and advance the btp_strip state.
            */
            if (btp_strip && b_is_btp(pkt)) {
                int mode = b_get_btp_word(pkt, 0);
                if (btp.in_process) {
                    fprintf(stderr, "ERROR: BTP %s found inside PROCESS block at %#x, data_end_packet %d.\n",
                        b_btp_mode_str(mode), total_packets * packetsize, btp.data_end_packet);
                }
                /* The following BTP's have start/end delimiters */
                if (mode == TT_MODE_PROCESS) {
                    btp.in_process = true;
#if BCHP_7411_VER == BCHP_VER_C0
                    btp.data_start_byte = b_get_btp_word(pkt, 4) & 0xFFFF;
                    btp.data_start_byte_hack_on = true;
                    btp.data_end_packet = b_get_btp_word(pkt, 5) & 0xFFFFFF;
                    btp.data_end_byte = b_get_btp_word(pkt, 6) & 0xFFFF;
#elif defined BCHP_7411_VER
                    btp.data_start_byte = b_get_btp_word(pkt, 4);
                    btp.data_end_packet = b_get_btp_word(pkt, 5);
                    btp.data_end_byte = b_get_btp_word(pkt, 6);
                    btp.data_start_byte_hack_on = false;
#else
                    btp.data_start_byte = b_get_btp_word(pkt, 7);
                    btp.data_end_packet = 1; /* only applies to the next packet, no packet counting */
                    btp.data_end_byte = b_get_btp_word(pkt, 8);
                    btp.data_start_byte_hack_on = false;
#endif
                    btp.count = 0;
                }
            }
            else if (btp.in_process) {
                /* if not in a BTP PROCESS context, let every packet pass */
                int start = offset;
                int end = packetsize; /* index of first byte not taken */

                if (btp.count == 0 || btp.data_start_byte_hack_on) {
                    /* first packet  - use data_start_byte */
                    if (btp.data_start_byte > start)
                        start = btp.data_start_byte;
#if BCHP_7411_VER == BCHP_VER_C0
                    if (btp.data_start_byte_hack_on) {
                        /* have to repeat the end logic here */
                        if (btp.count == btp.data_end_packet-1) {
                            if (btp.data_end_byte < end)
                                end = btp.data_end_byte;
                            btp.in_process = false;
                        }
                    }
                    btp.data_start_byte_hack_on = false;
#endif
                }
                else if (btp.count == btp.data_end_packet-1) {
                    /* last packet - use data_end_byte */
                    if (btp.data_end_byte < end)
                        end = btp.data_end_byte;
                    btp.in_process = false;
                }
                btp.count++;

                if (end > start) {
                    fwrite(&pkt[start], 1, end-start, stdout);
                }
            }
            else {
                /* Non-BTP, full packet write */
                if (timestamp_offset && strip_timestamp) {
                    fwrite(pkt, 1, packetsize-timestamp_offset, stdout);
                }
                else {
                    fwrite(pkt-timestamp_offset, 1, packetsize, stdout);
                }
            }
            total_packets++;
        }
    }
    return 0;
}
