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

#include "lump.hpp"
#include "constant.hpp"
#include "src/macros.hpp"

#include <stdlib.h>

static void grow(struct lump *lmp);
/* Add a code to a lump that does not take arguments. */
static void add_code_niladic(struct lump *l, enum op_code code);
/* Add a code to a lump that takes one byte argument. */
static void add_code_monadic(struct lump *l, enum op_code code, uint8_t val);
/* Add a code to a lump that takes two byte arguments. `val` is stored
 * as a big-endian sequence. */
static void add_code_dyladic(struct lump *l, enum op_code code, uint16_t val);

struct lump *lump::init()
{
	struct lump *lmp = (struct lump *)malloc(sizeof(struct lump));

	ASSERT(lmp != NULL, "Unable to allocate memory for lump.");

	lmp->count = 0;
	lmp->size = BUFFER_COUNT * sizeof(uint8_t);
	lmp->array = (uint8_t *)malloc(lmp->size);
	lmp->constants = constant_vector::init();

	ASSERT(lmp->array != NULL, "Unable to allocate memory for lump.");

	return lmp;
}

void lump::free(struct lump *lmp)
{
	constant_vector::free(lmp->constants);
	::free(lmp->array);
	free(lmp);
	lmp = NULL;
}

int lump::add_code(struct lump *lmp, enum op_code code)
{
	add_code_niladic(lmp, code);
	return lmp->count - 1;
}

int lump::add_constant(struct lump *lmp, double d)
{
	int offset = constant_vector::add(lmp->constants, d);

	if (offset < 1 << 8)
		add_code_monadic(lmp, op_code::CONSTANT, offset);
	else
		add_code_dyladic(lmp, op_code::CONSTANT_LONG, offset);

	return offset;
}

static void grow(struct lump *lmp)
{
	lmp->size += lump::BUFFER_COUNT * sizeof(uint8_t);
	lmp->array = (uint8_t *)realloc(lmp->array, lmp->size);

	ASSERT(lmp->array != NULL, "Unable to grow lump.");
}

static void add_code_niladic(struct lump *lmp, enum op_code code)
{
	if (lmp->count == (lmp->size / sizeof(uint8_t)))
		grow(lmp);

	lmp->array[lmp->count] = code;
	lmp->count++;
}

static void add_code_monadic(struct lump *lmp, enum op_code code, uint8_t val)
{
	if (lmp->count + 1 >= (lmp->size / sizeof(uint8_t)))
		grow(lmp);

	lmp->array[lmp->count] = code;
	lmp->array[lmp->count + 1] = val;
	lmp->count += 2;
}

static void add_code_dyladic(struct lump *lmp, enum op_code code, uint16_t val)
{
	if (lmp->count + 2 >= (lmp->size / sizeof(uint8_t)))
		grow(lmp);

	lmp->array[lmp->count] = code;
	lmp->array[lmp->count + 1] = val >> 8;
	lmp->array[lmp->count + 2] = val & 0x00FF;
	lmp->count += 3;
}
