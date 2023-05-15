#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "commands.h"

static int skip_atoi(const char **s)
{
	int i=0;

	while (isdigit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

int btst_scanf(BTST_P_Context *pContext, const char * fmt, va_list args)
{
	int num = 0;
	int qualifier;
	int field_width;

	char fmt_str[128], width_str[32], conv_str[16], qualifier_str[16];
	
	while(*fmt) {
		/* skip any white space in format */
		/* white space in format matchs any amount of
		 * white space, including none, in the input.
		 */
		if (isspace(*fmt)) {
			while (isspace(*fmt))
				++fmt;
		}

		if (*fmt != '%' && *fmt) {
			break;
		}

		if (!*fmt)
			break;

		++fmt;
		
		/* skip this conversion.
		 * advance both strings to next white space
		 */
		if (*fmt == '*') {
			while (!isspace(*fmt) && *fmt)
				fmt++;
			continue;
		}

		/* get field width */
		field_width = -1;
		if (isdigit(*fmt))
		{
			field_width = skip_atoi(&fmt);
			sprintf(width_str, "%d", field_width);
		}
		
		/* get conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' ||
		    *fmt == 'Z' || *fmt == 'z') {
			qualifier = *fmt++;
			sprintf(qualifier_str, "%c", qualifier);
				if (/* qualifier */ *(fmt+1) == 'h') {
					qualifier = 'H';
					sprintf(qualifier_str, "%s", "hh");
					fmt++;
				} else if (/* qualifier*/ *(fmt+1) == 'l') {
					qualifier = 'L';
					sprintf(qualifier_str, "%s", "ll");
					fmt++;
 				}
		}

		if (!*fmt)
			break;

		sprintf(conv_str, "%c", *fmt);
		
		switch(*fmt++) {
		case 'c':
		case 's':
		{
			char *s = (char *) va_arg(args, char*);

			sprintf(fmt_str, "%%");
			if (field_width != -1)
				strcat(fmt_str, width_str);
#if 0
			printf("fmt_str = %s, width_str = %s, conv = %s\n", fmt_str, width_str, conv_str);
#endif
			strcat(fmt_str, conv_str);

			if (pContext->bRepeatCmd)
			{
				
				fscanf(pContext->fp_cmd, fmt_str, s);

			}
			else
			{
				if (*conv_str == 'c') 
				{
					/* skip previous white spaces */ 
					do {
						scanf("%c", s);
					} while (isspace(*s));
					
				}
				else
					scanf(fmt_str, s);
				
				if (pContext->bRecordCmd)
				{
					if (*conv_str == 'c')					
						fprintf(pContext->fp_cmd, fmt_str, *s);
					else /* conv_str == 's' */ 
						fprintf(pContext->fp_cmd, fmt_str, s);
					fprintf(pContext->fp_cmd, "\n");
				}	
			}

#if 0
			printf("verify fmt c & s: \n");
			if (*conv_str == 'c') 	
			{
				printf(" -- character --\n");
				printf(fmt_str, *s);
			}
			else /* conv_str == 's' */ 
				printf(fmt_str, s);
			printf("\n");

#endif
			num++;
		}
			continue;
		case 'n':
			/* return number of characters read so far */
		{
			int *i = (int *)va_arg(args,int*);
			*i = 0; /* FIXME */ 
		}
		continue;
		case 'o':
		case 'u':
		case 'x':
		case 'X':
		{
			switch(qualifier) {
			case 'H': /* hh */ 
			{
				unsigned char *s = (unsigned char *) va_arg(args, unsigned char *);

				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);

				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);

				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

			case 'h':
			{
				unsigned short *s = (unsigned short *) va_arg(args, unsigned short *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
			
			case 'l':
			{
				unsigned long *s = (unsigned long *) va_arg(args, unsigned long *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

#if 0 /* To avoid compiler warning: ISO C90 does not support 'long long'. */ 
			case 'L': /* ll */ 
			{
				unsigned long long *s = (unsigned long long *) va_arg(args, unsigned long long *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
#endif
			case 'z':
			case 'Z':
			{
				size_t *s = (size_t *) va_arg(args, size_t *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

			default:
			{
				unsigned *s = (unsigned  *) va_arg(args, unsigned *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, conv_str);

				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
			}

#if 0
			printf("fmt_str = %s, width_str = %s, qualifier_str = %s, conv = %s\n", fmt_str, width_str, qualifier_str, conv_str);
#endif		

			num++;
		}
			continue;

		case 'i':
		case 'd':
		{
			switch(qualifier) {
			case 'H': /* hh */ 
			{
				char *s = (char *) va_arg(args, char *);

				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);

				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);

				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

			case 'h':
			{
				short *s = (short *) va_arg(args, short *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
			
			case 'l':
			{
				long *s = (long *) va_arg(args, long *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

#if 0 /* To avoid compiler warning: ISO C90 does not support 'long long'. */ 
			case 'L': /* ll */ 
			{
				long long *s = (long long *) va_arg(args, long long *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
#endif

			case 'z':
			case 'Z':
			{
				size_t *s = (size_t *) va_arg(args, size_t *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, qualifier_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;

			default:
			{
				int *s = (int  *) va_arg(args, int *);
			
				sprintf(fmt_str, "%%");
				if (field_width != -1)
					strcat(fmt_str, width_str);
				strcat(fmt_str, conv_str);
			
				if (pContext->bRepeatCmd)
				{
				
					fscanf(pContext->fp_cmd, fmt_str, s);
			
				}
				else
				{
					scanf(fmt_str, s);
					if (pContext->bRecordCmd)
					{
						fprintf(pContext->fp_cmd, fmt_str, *s);
						fprintf(pContext->fp_cmd, "\n");
					}	
				}				
			}
				break;
			}

#if 0
			printf("fmt_str = %s, width_str = %s, qualifier_str = %s, conv = %s\n", fmt_str, width_str, qualifier_str, conv_str);
#endif		

			num++;
		}
			continue;
		case '%':
			continue;
		default:
			/* invalid format; stop here */
			printf("btsc_scanf unsupported formt %c\n", *fmt);
			return num;
		}
	}
	return num;
}


int BTST_P_Scanf(BTST_P_Context *pContext, const char * fmt, ...)
{
	va_list args;
	int i;

	va_start(args,fmt);
	i = btst_scanf(pContext,fmt,args);
	va_end(args);
	return i;
}

