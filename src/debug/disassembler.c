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

#include "disassembler.h"

#include <stdio.h>



static int cur_line, prev_line;
static int offset;
static struct lump *lmp;

static inline void print_line();
static void print_code();
static void print_op_constant();
static void print_op_constant_long();

void disassemble(lump *l)
{
	lmp = l;
	cur_line = 0, prev_line = -1;

	for (offset = 0; offset < lump_count(lmp); offset++) {
		printf("%04d\t", offset);
		print_line();
		print_code();
	}
}

static inline void print_line()
{
	if (cur_line == prev_line) {
		printf("|\t");
	} else {
		printf("%04d\t", cur_line);
		prev_line = cur_line;
	}
}

static void print_code()
{
	switch(lump_get_code(lmp, offset)) {
		/* The next byte is the constant's address. */
	case OP_CONSTANT:
		print_op_constant();
		break;

	/* The next two bytes make up the constant's address. */
	case OP_CONSTANT_LONG:
		print_op_constant_long();
		break;

	case OP_RETURN:
		printf("OP_RETURN\n");
		break;

	case OP_LINE_INC:
		printf("OP_LINE_INC\n");
		cur_line++;
		break;

	default:
		printf("Instruction not found...\n");
	}
}

static void print_op_constant()
{
	printf("%-16s %04d %g\n", \
	       "OP_CONSTANT",
	       lump_get_constant_offset(lmp, offset),
	       lump_get_constant(lmp, offset));
}

static void print_op_constant_long()
{
	printf("%-16s %04d %g\n", \
	       "OP_CONSTANT_LONG",
	       lump_get_constant_offset(lmp, offset),
	       lump_get_constant(lmp, offset));
}
