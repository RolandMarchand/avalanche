/*
 * Copyright (c) 2022, Roland Marchand <roland.marchand@protonmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "error.h"
#include "parser.h"

#include <stdio.h>

void report(int line, char *message)
{
	parser.panic = 1;

	switch(parser.current_token->type) {
	case TOKEN_END_OF_FILE:
		fprintf(stderr, "[line %d] at end: %s\n", line, message);
		break;
	case TOKEN_STRING:
		fprintf(stderr, "[line %d] at \": %s\n", line, message);
		break;
	case TOKEN_INVALID:
		fprintf(stderr, "[line %d] at invalid token: %s\n", line, message);
		break;
	case TOKEN_NEWLINE:
		fprintf(stderr, "[line %d] at new line: %s\n", line, message);
	default:
		fprintf(stderr, "[line %d] at %s: %s\n",
			line, sbstr2str(&parser.current_token->lexeme), message);
	}	
}
