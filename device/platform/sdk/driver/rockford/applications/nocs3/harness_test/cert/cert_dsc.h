#ifndef __CERT_DSC__
#define __CERT_DSC__

#include "types.h"

/* int cert_dsc(int argc, char *argv[]); */
int cert_dsc_ciph(CERT_DSC *xpDSC, DATA_IO *xpIO, const char *ecm2cert_csv_filename);


#endif

/*** end of file ***/
