/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Workfile: check_index_lib.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 12/2/10 12:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/record/check_index_lib.c $
 * 
 * Hydra_Software_Devel/2   12/2/10 12:39p jtna
 * SW7125-716: rework overflow recovery unittests
 * 
 * Hydra_Software_Devel/1   12/11/08 6:21p jtna
 * PR48779: initial version
 * 
 ******************************************************************************/
#include "check_index_lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bstd_defs.h"
#include "berr.h"
#include "bchp.h"
#include "bkni.h"

BDBG_MODULE(check_index_lib);

typedef struct NAV_Fields
{
    uint32_t seqHdrStartOffset;
    uint8_t  frameType;
    uint8_t  version;
    uint8_t  refFrameOffset;
    uint16_t seqHdrSize;
    uint32_t frameOffsetHi;
    uint32_t frameOffsetLo;
    uint32_t framePts;
    uint32_t frameSize;
    uint32_t timestamp;
} NAV_Fields;

typedef struct SCT6_Fields
{
    uint8_t  scEntryType;
    uint8_t  sc;
    uint16_t twoBytesAfterSc;
    uint8_t  offsetIntoPacket;
    uint32_t offsetFromStart;
    uint8_t  offsetIntoPacketMSB;
    uint32_t esPayload234;
    uint8_t  flags;
    uint32_t esPayload567;
} SCT6_Fields;

void generate_payload(unsigned char *pMem, unsigned pid, unsigned num_packets, unsigned num_packets_per_rap) 
{
    unsigned i, j;
    
    for (i=0;i<num_packets;i++) {
        pMem[i*188+0] = 0x47;
        pMem[i*188+1] = 0x00;
        pMem[i*188+2] = pid;
        pMem[i*188+3] = 0x10; /* not scrambled, payload only, 0 continuity counter */

        /* fill the payload with the byte number of each byte */
        for (j=4; j<188; j++) {
            pMem[i*188+j] = j;
        }

        /* every so often, insert a "random access point" */
        if (i%num_packets_per_rap==0) {
            pMem[i*188+4] = 0x0;
            pMem[i*188+5] = 0x0;
            pMem[i*188+6] = 0x1;
            pMem[i*188+7] = 0xB3; /* sequence header */

            /* skip 8 bytes for the SQH. garbage here is ok.
               the SQH is needed or bcmindexer.c will discard the picture */

            pMem[i*188+16] = 0x0;
            pMem[i*188+17] = 0x0;
            pMem[i*188+18] = 0x1;
            pMem[i*188+19] = 0x0; /* picture start code */
            pMem[i*188+20] = 0x0;
            pMem[i*188+21] = 0x8; /* frame-type: I */
        }
    }
}

/* look for picture start code (0x00 0x00 0x01 0x00) at given offset in TS packets */
int get_next_picture_startcode_pos(FILE *f, unsigned offset /* in bytes */, bool *eof)
{
    unsigned char packet[188];
    while (fread(packet, sizeof(packet), 1, f) == 1) { /* read TS packets until startcode is found */
        if (packet[0]!=0x47) {
            BDBG_ERR(("Packet out of sync at byte %ld", ftell(f)-188));
            return -1;
        }
        if (packet[offset]==0x0 && packet[offset+1]==0x0 && packet[offset+2]==0x1 && packet[offset+3]==0x0) {
            /* found startcode */
            return (ftell(f)-188+offset);
        }
    }
    
    if (!feof(f)) {
        BDBG_ERR(("Unknown file read error"));
        *eof = false;
    }
    else {
        *eof = true;
    }

    return -1;
}

/* reads a NAV entry and puts the fields in a struct */
int read_next_nav_entry(FILE *f, NAV_Fields *fields)
{
    BNAV_Entry entry;
    if (fread(&entry, sizeof(entry), 1, f) != 1) { /* read a NAV entry */
        if (!feof(f)) {
            BDBG_ERR(("Unknown NAV file read error"));
        }
        return -1;
    }

    fields->seqHdrStartOffset = BNAV_get_seqHdrStartOffset(&entry);
    fields->frameType = BNAV_get_frameType(&entry);
    BSTD_UNUSED(fields->version);
    fields->refFrameOffset = BNAV_get_refFrameOffset(&entry);
    BSTD_UNUSED(fields->seqHdrSize);
    fields->frameOffsetHi = entry.words[2];
    fields->frameOffsetLo = entry.words[3];
    fields->framePts = entry.words[4];
    fields->frameSize = BNAV_get_frameSize(&entry);
    fields->timestamp = entry.words[6];
    return 0;
}

#define SQH_SIZE 12 /* size of sequence header */
#define TS_HEADER_SIZE 4 /* size of TS header */

/* parses a NAV file and check correctness against data file */
int check_nav_index(const char *data, const char *index)
{
    FILE *fdata, *findx;
    NAV_Fields nav_curr, nav_prev;
    bool fail = false, eof = false;
    int pos = 0;
    unsigned num_nav = 0;

    fdata = fopen(data, "rb");
    findx = fopen(index, "rb");
    
    if (!fdata) {
        BDBG_ERR(("Could not open file %s", data));
        return 1;
    }
    if (!findx) {
        BDBG_ERR(("Could not open file %s", index));
        return 1;
    }

    BKNI_Memset(&nav_curr, 0, sizeof(nav_curr));
    BKNI_Memset(&nav_prev, 0, sizeof(nav_prev));

    BDBG_WRN(("Checking '%s' against '%s'", data, index));
    
    while (!feof(findx)) { /* parse the entire NAV file */
        if (read_next_nav_entry(findx, &nav_curr)) {
            break; /* EOF */
        }
        num_nav++;

        /* parse the data file and search for the next startcode */
        pos = get_next_picture_startcode_pos(fdata, TS_HEADER_SIZE+SQH_SIZE, &eof);

        if (eof) {
            break;
        }

        if (pos == -1) {
            BDBG_ERR(("Found NAV entry (%u) in index file but no corresponding startcode in data file", num_nav));
            fail = true;
            break;
        }

        /* check that frameOffset is getting correctly incremented */
        if (nav_prev.frameOffsetLo) {
            if (nav_curr.frameOffsetLo != nav_prev.frameOffsetLo+nav_prev.frameSize+SQH_SIZE)  {
                BDBG_ERR(("frameOffset not incremented correctly at NAV entry %u", num_nav));
                fail = true;
            }
        }
           
        /* check for sync between NAV entry and data file */
        if (nav_curr.frameOffsetLo != (uint32_t)pos) {
            BDBG_ERR(("frameOffset(0x%08x) out of sync with data file(0x%08x) at NAV entry %u ", 
               nav_curr.frameOffsetLo, pos, num_nav));
            fail = true;
        }

        nav_prev = nav_curr;
    }

    BDBG_WRN(("Compared %u entries. Result: %s", num_nav, fail?"FAIL":"PASS"));

    fclose(fdata);
    fclose(findx);
    
    return fail;
}

/* reads a SCT6 entry and puts the fields in a struct */
int read_next_sct6_entry(FILE *f, SCT6_Fields *fields)
{
    BSCT_SixWord_Entry entry;
    if (fread(&entry, sizeof(entry), 1, f) != 1) { /* read a sct6 entry */
        if (!feof(f)) {
            BDBG_ERR(("Unknown SCT6 file read error"));
        }
        return -1;
    }

    fields->scEntryType = entry.word0 >> 24;
    fields->sc = entry.startCodeBytes >> 24;
    fields->twoBytesAfterSc = (entry.startCodeBytes>>8) & 0xFFFF;
    fields->offsetIntoPacket = entry.startCodeBytes&0xFF;
    fields->offsetFromStart = entry.recordByteCount;
    BSTD_UNUSED(fields->offsetIntoPacketMSB);
    BSTD_UNUSED(fields->esPayload234);
    BSTD_UNUSED(fields->flags);
    BSTD_UNUSED(fields->esPayload567);
    return 0;
}

/* parses SCT6 file and check correctness against data file */
int check_sct6_index(const char *data, const char *indx)
{
    FILE *fdata, *findx;
    SCT6_Fields fields;
    bool fail = false, eof = false;
    int pos = 0;
    unsigned num_sct = 0;

    fdata = fopen(data, "rb");
    findx = fopen(indx, "rb");

    if (!fdata) {
        BDBG_ERR(("Could not open file %s", data));
        return 1;
    }
    if (!findx) {
        BDBG_ERR(("Could not open file %s", indx));
        return 1;
    }

    BKNI_Memset(&fields, 0, sizeof(fields));

    BDBG_WRN(("Checking '%s' against '%s'", data, indx));
   
    while (!feof(findx)) { /* parse the entire index file */
        if (read_next_sct6_entry(findx, &fields)) {
            break; /* EOF */
        }
        if (fields.sc == 0xb3) {
            continue;
        }
        num_sct++;

        /* parse the data file and search for the next startcode */
        pos = get_next_picture_startcode_pos(fdata, TS_HEADER_SIZE+SQH_SIZE, &eof);

        if (eof) {
            break;
        }

        if (pos == -1) {
            BDBG_ERR(("Found SCT entry (%u) in index file but no corresponding startcode in data file", num_sct));
            fail = true;
            break;
        }

        /* check for sync between SCT6 entry and data file */
        if (fields.offsetFromStart+fields.offsetIntoPacket != (unsigned)pos) {
            BDBG_ERR(("frameOffset(0x%08x) out of sync with data file(0x%08x) at SCT entry %u", 
               fields.offsetFromStart+fields.offsetIntoPacket, pos, num_sct));
            fail = true;
        }
    }

    BDBG_WRN(("Compared %u entries. Result: %s", num_sct, fail?"FAIL":"PASS"));

    fclose(fdata);
    fclose(findx);
    
    return fail;
}
