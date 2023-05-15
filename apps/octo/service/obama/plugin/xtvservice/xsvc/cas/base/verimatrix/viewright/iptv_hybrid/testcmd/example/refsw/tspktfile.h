
/*** tspsktfile.h - Transport Stream packet file library definitions. ***/

#ifndef     _TSPKTFILE_H
#define     _TSPKTFILE_H

#include    <stdio.h>


/* Send/Recieve Interface Structure. */

typedef struct          tspkt_if_file
        {
            FILE        *f;
        }
        TSPKT_IF_FILE;


/* Send/Receive file functions. */

extern void tspkt_file_init(TSPKT_IF_FILE *pif);

extern void tspkt_file_term(TSPKT_IF_FILE *pif);

extern int tspkt_file_open(TSPKT_IF_FILE *pif, char *fname, char *mode);

extern void tspkt_file_close(TSPKT_IF_FILE *pif);

extern int tspkt_file_rcv(TSPKT_IF_FILE *pif, unsigned char buf[188]);

extern void tspkt_file_send(TSPKT_IF_FILE *pif, unsigned char buf[188]);


#endif      /* _TSPKTFILE_H */

