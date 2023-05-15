/***************************************************************
**
** Broadcom Corp. Confidential
** Copyright 1998-2002 Broadcom Corp. All Rights Reserved.
**
** THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED 
** SOFTWARE LICENSE AGREEMENT BETWEEN THE USER AND BROADCOM. 
** YOU HAVE NO RIGHT TO USE OR EXPLOIT THIS MATERIAL EXCEPT 
** SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
**
** File:		venom2_fb.h
** Description: API to access venom2 frame buffer
**
** Created: 12/18/2001 Vladimir N. Silyaev
**
**
****************************************************************/

#ifndef VENOM2_FB_H
#define VENOM2_FB_H

typedef struct venom2_fb *venom2_fb;
typedef struct venom2_fb_zone *venom2_fb_zone;

enum v2fbColorFormat {v2fb420, v2fb422, v2fb444, v2fbRGB};

int Venom2FBCreate(venom2_fb *pfb, unsigned long frameb_addr);
int Venom2FBDestroy(venom2_fb fb);

int Venom2FBZoneCreate(venom2_fb fb, venom2_fb_zone *pzone, unsigned witdh, unsigned height, enum v2fbColorFormat targetCf);
int Venom2FBZoneDestroy(venom2_fb_zone zone);

int Venom2FBZoneCapture(venom2_fb_zone zone, unsigned mbline);
int Venom2FBZoneTransfer(venom2_fb_zone zone, unsigned char *line);

#endif /* VENOM2_FB_H */
