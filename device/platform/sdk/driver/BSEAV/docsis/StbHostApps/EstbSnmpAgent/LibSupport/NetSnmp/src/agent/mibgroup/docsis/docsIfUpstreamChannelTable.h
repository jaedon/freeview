/*
 * Note: this file originally auto-generated by mib2c using
 *        mib2c.brcm.iterate.conf 2003/6/16 turcotte
 */
#ifndef DOCSIFUPSTREAMCHANNELTABLE_H
#define DOCSIFUPSTREAMCHANNELTABLE_H

/*
 * function declarations 
 */
void            init_docsIfUpstreamChannelTable(void);
void            initialize_table_docsIfUpstreamChannelTable(void);
Netsnmp_Node_Handler docsIfUpstreamChannelTable_handler;


/*
 * column number definitions for table docsIfUpstreamChannelTable 
 */
#define COLUMN_DOCSIFUPCHANNELID		1
#define COLUMN_DOCSIFUPCHANNELFREQUENCY		2
#define COLUMN_DOCSIFUPCHANNELWIDTH		3
#define COLUMN_DOCSIFUPCHANNELMODULATIONPROFILE		4
#define COLUMN_DOCSIFUPCHANNELSLOTSIZE		5
#define COLUMN_DOCSIFUPCHANNELTXTIMINGOFFSET		6
#define COLUMN_DOCSIFUPCHANNELRANGINGBACKOFFSTART		7
#define COLUMN_DOCSIFUPCHANNELRANGINGBACKOFFEND		8
#define COLUMN_DOCSIFUPCHANNELTXBACKOFFSTART		9
#define COLUMN_DOCSIFUPCHANNELTXBACKOFFEND		10
#endif                          /* DOCSIFUPSTREAMCHANNELTABLE_H */