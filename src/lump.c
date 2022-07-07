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

#include "lump.h"
#include "constant.h"
#include "src/macros.h"

#include <stdlib.h>

struct lump {
	uint8_t *array;
	int size;
	int count;
	struct constant_array *constants;
};

static void lump_grow(lump *lmp);
/* Add a code to a lump that does not take arguments. */
static void lump_add_code_niladic(lump *l, enum op_code code);
/* Add a code to a lump that takes one byte argument. */
static void lump_add_code_monadic(lump *l, enum op_code code, uint8_t val);
/* Add a code to a lump that takes two byte arguments. `val` is stored
 * as a big-endian sequence. */
static void lump_add_code_dyladic(lump *l, enum op_code code, uint16_t val);

lump *lump_init()
{
	lump *lmp = malloc(sizeof(lump));

	ASSERT(lmp != NULL, "Unable to allocate memory for lump.");

	lmp->count = 0;
	lmp->size = LUMP_BUFFER_COUNT * sizeof(char);
	lmp->array = malloc(lmp->size);
	lmp->constants = constant_array_init();

	ASSERT(lmp->array != NULL, "Unable to allocate memory for lump.");

	return lmp;
}

void lump_del(lump *lmp)
{
	constant_array_del(lmp->constants);
	free(lmp->array);
	free(lmp);
	lmp = NULL;
}

int lump_add_code(lump *lmp, enum op_code code)
{
	lump_add_code_niladic(lmp, code);
	return lmp->count - 1;
}

int lump_add_constant(lump *lmp, double d)
{
	int offset = constant_array_add(lmp->constants, d);

	if (offset < 1 << 8)
		lump_add_code_monadic(lmp, OP_CONSTANT, offset);
	else
		lump_add_code_dyladic(lmp, OP_CONSTANT_LONG, offset);

	return offset;
}

static void lump_grow(lump *lmp)
{
	lmp->size += LUMP_BUFFER_COUNT * sizeof(uint8_t);
	lmp->array = realloc(lmp->array, lmp->size);

	ASSERT(lmp->array != NULL, "Unable to grow lump.");
}

uint8_t lump_get_code(lump *lmp, int offset)
{
	return lmp->array[offset];
}

double lump_get_constant(lump *lmp, int offset)
{
	return lmp->constants->array[offset];
}

int lump_count(lump *lmp)
{
	return lmp->count;
}

static void lump_add_code_niladic(lump *lmp, enum op_code code)
{
	if (lmp->count == (lmp->size / sizeof(uint8_t)))
		lump_grow(lmp);
	
	lmp->array[lmp->count] = code;
	lmp->count++;
}

static void lump_add_code_monadic(lump *lmp, enum op_code code, uint8_t val)
{
        if (lmp->count + 1 >= (lmp->size / sizeof(uint8_t)))
                lump_grow(lmp);

        lmp->array[lmp->count] = code;
        lmp->array[lmp->count + 1] = val;
        lmp->count += 2;
}

static void lump_add_code_dyladic(lump *lmp, enum op_code code, uint16_t val)
{
        if (lmp->count + 2 >= (lmp->size / sizeof(uint8_t)))
                lump_grow(lmp);

        lmp->array[lmp->count] = code;
        lmp->array[lmp->count + 1] = val >> 8;
        lmp->array[lmp->count + 2] = val & 0x00FF;
        lmp->count += 3;
}