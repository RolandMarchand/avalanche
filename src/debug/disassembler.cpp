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

#include "disassembler.hpp"

#include <stdio.h>

static void print_code(struct lump *lmp, int *offset, int *line);
static void print_op_constant(struct lump *lmp, int *offset);
static void print_op_constant_long(struct lump *lmp, int *offset);

void disassembler::lump(struct lump *lmp)
{
	int cur_line = 0, prev_line = -1;
	for (int offset = 0; offset < lmp->count; offset++) {
		printf("%04d\t", offset);

		if (cur_line == prev_line) {
			printf("|\t");
		} else {
			printf("%04d\t", cur_line);
			prev_line = cur_line;
		}

		print_code(lmp, &offset, &cur_line);
	}
}

void disassembler::instruction(struct lump *lmp, int offset)
{
	/* TODO, fill up a static table to avoid reading the whole
	 * code array every function call. */
	int line = 0;
	for(int i = 0; i < offset; i++) {
		if (lmp->array[i] == op_code::LINE_INC)
			line++;
	}

	print_code(lmp, &offset, &line);
}

static void print_code(struct lump *lmp, int *offset, int *line)
{
	switch(lmp->array[*offset]) {
		/* The next byte is the constant's address. */
	case op_code::CONSTANT:
		print_op_constant(lmp, offset);
		*offset += 1;
		break;

	/* The next two bytes make up the constant's address. */
	case op_code::CONSTANT_LONG:
		print_op_constant_long(lmp, offset);
		*offset += 2;
		break;

	case op_code::RETURN:
		printf("RETURN\n");
		break;

	case op_code::LINE_INC:
		printf("LINE_INC\n");
		(*line)++;
		break;

	default:
		printf("Instruction not found...\n");
	}
}

static void print_op_constant(struct lump *lmp, int *offset)
{
	int const_offset = lmp->array[*offset + 1];

	printf("%-16s %04d %g\n", \
	       "OP_CONSTANT",
	       const_offset,
	       lmp->constants->array[const_offset]);
}

static void print_op_constant_long(struct lump *lmp, int *offset)
{
	uint8_t byte1 = lmp->array[*offset + 1];
	uint8_t byte2 = lmp->array[*offset + 2];
	int const_offset = byte1 << 8 | byte2;

	printf("%-16s %04d %g\n", \
	       "CONSTANT_LONG",
	       const_offset,
	       lmp->constants->array[const_offset]);
}
