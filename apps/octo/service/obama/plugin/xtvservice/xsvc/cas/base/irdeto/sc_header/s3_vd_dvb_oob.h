/*
 * $Id: s3_vd_dvb_oob.h,v 1.3 2011/04/21 06:08:21 jcao Exp $
 *
 * Copyright 2005-2006 Irdeto Access B.V.
 *
 * This file and the information contained herein are the subject of copyright
 * and intellectual property rights under international convention. All rights
 * reserved. No part of this file may be reproduced, stored in a retrieval
 * system or transmitted in any form by any means, electronic, mechanical or
 * optical, in whole or in part, without the prior written permission of Irdeto
 * Access B.V.
 *
 * This file is part of the SoftCell 3 Integration library.
 */

/*
 * This file contains all the definitions for the 'DVB OOB virtual driver'.
 */

#ifndef S3_VD_DVB_OOB_H
#define S3_VD_DVB_OOB_H

#include "s3_portable_types.h"
#include "s3_vd.h"

/* VD_IO() opcodes. */
enum
{
	/* Read a section from the virtual driver. */
	VD_DVB_OOB_READ_SECTTION = ( VD_DRIVER_DVB_OOB * 10 ),
	/* REturn the status for the last read section to the driver. */
	VD_DVB_OOB_SET_STATUS
};

enum
{
	OOB_STATUS_SC_EMM = 0x01,
	OOB_STATUS_IRD_EMM,
	OOB_STATUS_CAM_EMM,
	OOB_STATUS_Format_error,
	OOB_STATUS_Address_error,
	OOB_STATUS_SoftCell_error,
	OOB_STATUS_No_SC
};

typedef void (* vd_dvb_oob_notify_fn)( ia_word32 wSoftCellVal );

/* Structure given in VD_Open() for this driver type. */
typedef struct
{
	vd_dvb_oob_notify_fn pfnNotify;
	ia_word32            wSoftCellVal;
} vd_dvb_oob_open_st;

/* Structure used for VD_DVB_OOB_SET_STATUS opcode in VD_IO(). */
typedef struct
{
	ia_byte  bOobStatus;
	ia_byte  bScStatus;
} vd_dvb_oob_status_st;

#endif /* S3_VD_DVB_OOB_H */

