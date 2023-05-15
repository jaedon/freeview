/* $Header:   //DTV/////.   1.0   July 28 2008 13:25:52      jhson$ */
/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_exkey.h -> di_usb_hid.h
// Original Author: Moonhoen Lee
// File Description: extra key(USB Keybaord, USB Mouse, USB Keyboard) Header File.
// Module:
// Remarks:
//
//
/*******************************************************************/
/* Copyright (c) 2009 HUMAX Co., Ltd.                              */
/* All rights reserved.                                            */
/*******************************************************************/

#ifndef __DI_USB_HID_H__
#define __DI_USB_HID_H__

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
// Start Including Header Files

/** di error type */
#include "di_err.h"
/** humax type */
#include "htype.h"
/** for struct timeval */
#include <sys/time.h>

// End Including Headers

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
// Start #define

// End #define

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee



// End global variable

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

/** Extra Key data. The structure is originated from linux kernel source. */
typedef struct {
	/* time */
	struct timeval time;
	/** type */
	HUINT16 type;
	/** code */
	HUINT16 code;
	/** value */
	HINT32 value;
} DI_USB_HID_DATA;

typedef enum
{
	/** USB Keyboard */
	DI_USB_HID_DEVICE_USBKEYBOARD = 0x1,
	/** USB Mouse */
	DI_USB_HID_DEVICE_USBMOUSE = 0x2,
	/** USB Joystick */
	DI_USB_HID_DEVICE_USBJOYSTICK = 0x4
} DI_USB_HID_DEVICE;

typedef HINT32 (* pfnDI_USB_HID_Notify)(DI_USB_HID_DEVICE eDevice, DI_USB_HID_DATA tData);

// End typedef

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee


// End static variable


/*******************************************************************/
/*********************** Function Prototype ************************/
/*******************************************************************/
// Start Function Prototype

/**
 *  register a callback fucntion for ex-key event
 *
 * @param[in] eDevices devices which are registed to pfnNotify callback function
 * @param[in] pfnNotify callback function
 */
DI_ERR_CODE DI_USB_HID_RegisterHIDEventCallback (
	const DI_USB_HID_DEVICE eDevices, 
	pfnDI_USB_HID_Notify pfnNotify);

DI_ERR_CODE DI_USB_HID_Init(void);

// End Function Prototype

#endif /* __DI_USB_HID_H__ */


