/*
 * output.h
 */

#ifndef __OUTPUT_H__
#define __OUTPUT_H__

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>

void output_init(void);

void output_token(FILE *, char *);

#endif // hmkim : added.

#endif	/* __OUTPUT_H__ */

