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

static void print_code(struct lump *lmp, int *offset, int *line);
static void print_op_constant(struct lump *lmp, int *offset);
static void print_op_constant_long(struct lump *lmp, int *offset);

void disassemble(struct lump *lmp)
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

void disassemble_instruction(struct lump *lmp, int offset)
{
	/* TODO, fill up a static table to avoid reading the whole
	 * code array every function call. */
	int line = 0;
	for(int i = 0; i < offset; i++) {
		if (lmp->array[i] == OP_LINE_INC)
			line++;
	}

	print_code(lmp, &offset, &line);
}

static void print_code(struct lump *lmp, int *offset, int *line)
{
	switch (lmp->array[*offset]) {
	case OP_GREATER:
		printf("OP_GREATER\n");
                break;

	case OP_GREATER_EQUAL:
		printf("OP_GREATER_EQUAL\n");
                break;

	case OP_LESS:
		printf("OP_LESS\n");
                break;

	case OP_LESS_EQUAL:
		printf("OP_LESS_EQUAL\n");
                break;

	case OP_EQUAL:
		printf("OP_EQUAL\n");
                break;

	case OP_NOT_EQUAL:
		printf("OP_NOT_EQUAL\n");
                break;

	case OP_ADD:
		printf("OP_ADD\n");
                break;

	case OP_SUBSTRACT:
		printf("OP_SUBSTRACT\n");
                break;

	case OP_MULTIPLY:
		printf("OP_MULTIPLY\n");
                break;

	case OP_MODULO:
		printf("OP_MODULO\n");
                break;

	case OP_DIVIDE:
		printf("OP_DIVIDE\n");
                break;

	case OP_LOGICAL_NOT:
		printf("OP_LOGICAL_NOT\n");
                break;

	case OP_NEGATE:
		printf("OP_NEGATE\n");
                break;

	/* The next byte is the constant's address. */
	case OP_CONSTANT:
		print_op_constant(lmp, offset);
		*offset += 1;
		break;

	/* The next two bytes make up the constant's address. */
	case OP_CONSTANT_LONG:
		print_op_constant_long(lmp, offset);
		*offset += 2;
		break;

	case OP_RETURN:
		printf("OP_RETURN\n");
		break;

	case OP_LINE_INC:
		printf("OP_LINE_INC\n");
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
	       "OP_CONSTANT_LONG",
	       const_offset,
	       lmp->constants->array[const_offset]);
}
