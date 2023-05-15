/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bcmplayer_test.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/3/10 3:54p $
 *
 * Module Description: Transport Stream Index Player Header File
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bcmplayer/tests/bcmplayer_test.c $
 * 
 * 1   6/3/10 3:54p erickson
 * SW7400-2788: add testse
 *
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "bcmplayer.h"

int main(int argc, char **argv)
{
    BNAV_Player_Handle bcmplayer;
    BNAV_Player_Settings settings;
    FILE *indexfile;
    long index;
    int rc;

    if (argc < 2) {
        printf("Usage: bcmplayer_test FILE\n");
        exit(1);
    }

    indexfile = fopen(argv[1], "rb");
    if (!indexfile) {
        return -1;
    }

    BNAV_Player_GetDefaultSettings(&settings);
    settings.videoPid = 0x21;
    settings.readCb = (BP_READ_CB)fread;
    settings.tellCb = (BP_TELL_CB)ftell;
    settings.seekCb = (BP_SEEK_CB)fseek;
    settings.filePointer = indexfile;
    rc = BNAV_Player_Open(&bcmplayer, &settings);
    BDBG_ASSERT(!rc);

    index = 100;
    rc = BNAV_Player_SetCurrentIndex(bcmplayer, index);
    BDBG_ASSERT(!rc);
    printf("at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index, -1);
    printf("back to I: at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index, -1);
    printf("same: at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index-1, -1);
    printf("previous: at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index+1, 1);
    printf("forward to I: at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index, 1);
    printf("same: at index %ld\n", index);

    index = BNAV_Player_FindIFrameFromIndex(bcmplayer, index+1, 1);
    printf("next: at index %ld\n", index);

    BNAV_Player_Close(bcmplayer);

    return 0;
}
