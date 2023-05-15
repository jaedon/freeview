
/*** tspsktfile.c - Transport Stream packet file library functions. ***/

#include    "tspktfile.h"


/* Send/Receive file functions. */

void tspkt_file_init(TSPKT_IF_FILE *pif)
{
    if (pif)
    {
        pif->f = NULL;
    }
}

void tspkt_file_term(TSPKT_IF_FILE *pif)
{
    /* (Precautionary).*/

    if ((pif) && (pif->f))
    {
        tspkt_file_close(pif);
    }
}

int tspkt_file_open(TSPKT_IF_FILE *pif, char *fname, char *mode)
{
    int     result = -1;

    if ((pif) && (pif->f == NULL) && (fname) && (mode))
    {
        pif->f = fopen(fname, mode);
        if (pif->f)
        {
            result = 0;
        }
    }

    return result;
}

void tspkt_file_close(TSPKT_IF_FILE *pif)
{
    if ((pif) && (pif->f))
    {
        fclose(pif->f);
        pif->f = NULL;
    }
}

int tspkt_file_rcv(TSPKT_IF_FILE *pif, unsigned char buf[188])
{
    int     result = -1;
    size_t  s, r;

    if ((pif) && (buf))
    {
        /* Locate a sync byte in the first 188 bytes*/
        r = 188;
        do
        {
            s = fread(buf, 1, 1, pif->f);
            r -= s;
        }
        while ((s == 1) && (buf[0] != 0x47));
    
        if ((s == 1) && (buf[0] == 0x47))
        {
            /* Read the remainder of the packet. */
            s = fread((buf + 1), 1, 187, pif->f);
            if (s == 187)
            {
                result = 0;
            }
        }
    }

    return result;
}

void tspkt_file_send(TSPKT_IF_FILE *pif, unsigned char buf[188])
{
    if ((pif) && (buf) && (buf[0] == 0x47))
    {
        (void) fwrite(buf, 1, 188, pif->f);
    }
}


