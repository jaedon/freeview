/*
 * output.c
 */

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "utils.h"

static void print_newline(FILE *);
static void print_token(FILE *, char *);

static int indent = 0;
static bool newline = true;

/* keep track of () blocks that should be indented */
#define PAREN_LEVEL_MAX		8192
static int paren_level = 0;
static bool indent_paren[PAREN_LEVEL_MAX];

void
output_init(void)
{
	#if 1 // hmkim : added.
	indent = 0;
	newline = true;
	paren_level = 0;
	#endif

	bzero(indent_paren, sizeof(indent_paren));

	return;
}

void
output_token(FILE *out, char *tok)
{
	/* assert */
	if(tok[0] == '\0')
		fatal("output_token: 0 length token");

	if(tok[0] == '(' && tok[1] == '\0')
	{
		/* assert */
		if(paren_level >= (PAREN_LEVEL_MAX - 1))
			fatal("output_token: max nested parenthesis reached");
		print_token(out, tok);
		paren_level ++;
		if(indent_paren[paren_level])
		{
			print_newline(out);
			indent ++;
		}
	}
	else if(tok[0] == ')' && tok[1] == '\0')
	{
		/* assert */
		if(paren_level <= 0)
			fatal("output_token: unexpected ')'");
		if(indent_paren[paren_level])
		{
			indent --;
			print_newline(out);
		}
		indent_paren[paren_level] = false;
		paren_level --;
		print_token(out, tok);
	}
	else if(tok[0] == '{' && tok[1] == ':')
	{
		print_newline(out);
		print_token(out, tok);
		indent ++;
	}
	else if(tok[0] == '}')
	{
		/* assert */
		if(indent == 0)
			fatal("output_token: unexpected '}'");
		indent --;
		print_newline(out);
		print_token(out, tok);
	}
	else if(strcmp(tok, ":ActionSlots") == 0
	     || strcmp(tok, ":Items") == 0
	     || strcmp(tok, ":LinkEffect") == 0
	     || strcmp(tok, ":MovementTable") == 0
	     || strcmp(tok, ":Multiplex") == 0
	     || strcmp(tok, ":NextScenes") == 0
	     || strcmp(tok, ":NoTokenActionSlots") == 0
	     || strcmp(tok, ":OnCloseDown") == 0
	     || strcmp(tok, ":OnRestart") == 0
	     || strcmp(tok, ":OnSpawnCloseDown") == 0
	     || strcmp(tok, ":OnStartUp") == 0
	     || strcmp(tok, ":Positions") == 0
	     || strcmp(tok, ":TokenGroupItems") == 0)
	{
		print_newline(out);
		print_token(out, tok);
		/* next () block should be indented */
		indent_paren[paren_level + 1] = true;
	}
	else if(tok[0] == ':'
	     && strcmp(tok, ":CCPriority") != 0
	     && strcmp(tok, ":ContentRef") != 0
	     && strcmp(tok, ":IndirectRef") != 0
	     && strcmp(tok, ":GBoolean") != 0
	     && strcmp(tok, ":GContentRef") != 0
	     && strcmp(tok, ":GInteger") != 0
	     && strcmp(tok, ":GObjectRef") != 0
	     && strcmp(tok, ":GOctetString") != 0
	     && strcmp(tok, ":NewCCPriority") != 0
	     && strcmp(tok, ":NewRefContent") != 0)
	{
		print_newline(out);
		print_token(out, tok);
	}
	else
	{
		print_token(out, tok);
	}

	return;
}

static void
print_newline(FILE *out)
{
	if(!newline)
		fprintf(out, "\n");

	newline = true;

	return;
}

static void
print_token(FILE *out, char *tok)
{
	int i = indent;

	if(newline)
	{
		while(i > 0)
		{
			fprintf(out, "\t");
			i --;
		}
		newline = false;
	}
	else
	{
		fprintf(out, " ");
	}

	fprintf(out, "%s", tok);

	return;
}

#endif // hmkim : added.
