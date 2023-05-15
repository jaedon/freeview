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
 * $brcm_Workfile: i2ctest.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 9/6/12 6:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/i2ctest.c $
 * 
 * 2   9/6/12 6:06p erickson
 * SW7435-349: add standard header
 * 
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <malloc.h>
#include "bcm_frontend_ioctl.h"

#define I2C_DEVICE  "/dev/bcmupgi2c"

static const char *usage =
"[options] [displaystring] \n\
    --help           -h  \n\
    --read           -r (read)\n\
    --write          -w (write)\n\
    --i2cChipAddress -a (eg fc in hex for 4500)\n\
    --regAddress     -g (01 in hex optional)\n\
    --data           -d (aabbccddeeff00 in hex no space or commas,\n\
    --size           -s (number if bytes to read from the slave)\n\
    examples: \n\
    i2ctest -r -a fc -g 00 -s 10 -read 10 bytes starting from addr 00 on 4500 chip\n\
    i2ctest -w -a fc -g a0 -d 01 -reset 4500 chip on BCM94538 board \n\
    i2ctest -r -a 10 -s2         -read isl6421 chip registers on  BCM94538 board \n";
    

int main (int argc, char **argv) 
{
    int rc = 0;
    int c=0;
    int error = 0;
    int done = 0;
    int devp;
    int operation =0;
    int i;
    int j;
    int lengthofdatastring = 0;
    brcm_upgi2c_ctrl data = {0,0,NULL,0};
    char tempString[256];
    char tempString2[3]={0,0,'\0'};

    while ((!error) && (!done))
    {
        int option_index = 0;
        int argval;
        static const struct option long_options[] = 
        {
            {"help",      0, 0, 'h'},
            {"read",      0, 0, 'r'},
            {"write",     0, 0, 'w'},            
            {"address",   1, 0, 'a'},
            {"register",  1, 0, 'g'},
            {"data",      1, 0, 'd'},
            {"size",      1, 0, 's'},
            {0, 0, 0, 0}
        };

        c = getopt_long (argc, argv, "hrwa:g:d:s:",long_options, &option_index);
        if (c == -1)
        {
            done = 1;
        }
        else
        {
            switch (c)
            {
            case 'h':
                fprintf(stdout,"usage: %s %s\n",argv[0],usage);
                return 0;
                break;               
            case 'r':
                operation = 2;
                break;
            case 'w':
                operation = 1;
                break;
            case 'a':
                sscanf(optarg,"%x",&argval);
                data.i2cDeviceId = argval; 
                break;
            case 'g':
                sscanf(optarg,"%x",&argval);
                data.i2cDevRegAddr = argval;
                operation +=2; /* 3 reg write,4 reg read */

                break;
            case 'd':
                if (sscanf(optarg,"%s",tempString) < 1)
                {
                    printf("Missing argument to '-d' switch\n");
                    return -1;                   
                }

                lengthofdatastring = strlen(tempString);

                for (i=0; i < lengthofdatastring; i++)
                {
                    if (! isxdigit(tempString[i]))
                    {
                        printf("Data string '-d%s' contains non-hex character(s)\n", tempString);
                        for (j=0; j<(15+i); j++)
                            printf(" ");

                        printf("^\n\n");    
                        return 1;
                    }
                }

                if (lengthofdatastring % 2)
                {
                    printf("length of data must be an integer number of bytes\n");
                    return 1;           
                }
                data.size = lengthofdatastring / 2; 
                break;
            case 's':
                sscanf(optarg,"%d",&argval);
                if (lengthofdatastring && argval != data.size)
                {
                    printf("size mismatch between -s%d and -d%s\n", argval, tempString);
                    return -1;   
                }
                data.size = argval;
                break;
            default:
                fprintf (stderr,"?? getopt returned character code 0%o ??\n", c);
                fprintf(stdout,"usage: %s %s\n",argv[0],usage);
                return -1;
            }
        }
    }

    if (data.size == 0 )
    {
        printf("Data size cannot be zero\n");
        fprintf(stdout,"usage: %s %s\n",argv[0],usage);
        return -1;       
    }
    /* open the device */
    devp = open(I2C_DEVICE,O_RDWR);
    if (devp <0)
    {
        printf("Unable to open the device %s\n",I2C_DEVICE);
        return -1;
    }
    /*allocate memory */
    if ((data.pData = calloc(data.size,sizeof(char)))==NULL)
    {
        printf("Unable to allocate %d bytes memory \n",data.size);
        return -1;
    }
    /*format the data */
    /*printf("you entered %dbytes  %s\n",req.size,tempString);*/

    for (i=0, j=0;tempString[i]!='\0';i++)
    {
        tempString2[j]=tempString[i];
        if (j==1)
        {
            j=0; 
            sscanf(tempString2,"%x",&data.pData[i/2]);
        }
        else
        {
            j++;
        }            
    }

    switch (operation)
    {
    case 1:
    case 3:
        printf("writing %d bytes starting from %02x to the device 0x%02x\n",data.size,data.i2cDevRegAddr,data.i2cDeviceId);
        for (j=0;/*j<i/2*/j<data.size;j++)
        {
            printf("0x%02x,",data.pData[j]);
        }
        printf("\n");
        if(operation == 1)
            error = ioctl(devp, BRCM_IOCTL_UPGI2C_WRITE,&data);
        else
            error = ioctl(devp, BRCM_IOCTL_UPGI2C_REG_WRITE,&data);

        if (error == -1)
            printf("failed to write\n");
        break;
    case 2:
    case 4:
        /* read */
        printf("reading %d bytes starting from %02x from the device 0x%02x\n",data.size,data.i2cDevRegAddr,data.i2cDeviceId);
        if(operation==2)
            error = ioctl(devp, BRCM_IOCTL_UPGI2C_READ,&data);
        else
            error = ioctl(devp, BRCM_IOCTL_UPGI2C_REG_READ,&data);

        if (error == -1)
            printf("failed to read\n");
        else
        {
            for (i=0;i<data.size;i++)
            {
                printf("0x%02x,",data.pData[i]);
            }
            printf("\n");

        }
        break;
    default:
        printf("Operation not supported\n");
        fprintf(stdout,"usage: %s %s\n",argv[0],usage);

    }
    free(data.pData);
    close(devp);
    return(error);   
}
