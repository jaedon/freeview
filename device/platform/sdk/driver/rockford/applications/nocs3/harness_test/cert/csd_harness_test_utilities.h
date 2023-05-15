
#ifndef __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__
#define __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define CSD_HELP_M_KEY		"-key KL|SW = Inicates if the key is protected through the KL or is a \
Software provided key"
#define CSD_HELP_M_EMI		"-emi EMI = Defines Encryption Method Indicator of the input data"
#define CSD_HELP_M_IVC		"[-ivc Value64|Value128] = IV Value.  Optional Parameter. A 64bit/128 bit \
from the command line (-ivc). If the option is not present and for particular EMI an IV is needed then \
it must be taken from the .csv file provided with option -tv"
#define CSD_HELP_M_TV		"-tv TestVectorFileName = Indicates the path and filename of the .CSV files \
which contains the test vector"
#define CSD_HELP_M_VPID		"-videoPid VPID = Sets video PID to be processed. "
#define CSD_HELP_M_APID		"-audioPid APID = Sets audio PID to be processed. "
#define CSD_HELP_M_PPID		"-pcrPid PPID = Sets PCR PID to be processed. "
#define CSD_HELP_M_IF		"-if InFile|-ife = Define what is the input stream. If -if then the input comes \
from the file (InFile is a path to an MPEG TS file). If -ife then the input streams comes from the STB FrontEnd."
#define CSD_HELP_M_OF		"[-of OutFile] = Output File. Optional Parameter. If this option is present, the \
descrambled output is sent into OutFile as well as to the video output.  If not specified the descrambled stream \
only goes to the video output."

void csd_harness_test_help (void);
void csd_harness_test_version (void);

#ifdef __cplusplus
}
#endif

#endif /* __NOCS3_CSD_HARNESS_TEST_UTILITIES_H__ */