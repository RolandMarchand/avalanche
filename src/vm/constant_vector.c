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

#include "constant.h"
#include "src/macros.h"

#include <stdlib.h>

static void constant_vector_grow(struct constant_vector *ca);

struct constant_vector *constant_vector_init()
{
	struct constant_vector *ca = malloc(sizeof(struct constant_vector));

	ASSERT(ca != NULL, "Unable to allocate memory for constant_vector.");

	ca->count = 0;
	ca->size = CONSTANT_VECTOR_BUFFER_COUNT * sizeof(double);
	ca->array = malloc(ca->size);

	ASSERT(ca->array != NULL, "Unable to allocate memory for constant_vector.");

	return ca;
}

void constant_vector_free(struct constant_vector *ca)
{
	free(ca->array);
	free(ca);
	ca = NULL;
}


int constant_vector_add(struct constant_vector *ca, double d)
{
	if (ca->count == (ca->size / sizeof(double)))
		constant_vector_grow(ca);
	ca->array[ca->count] = d;
	return ca->count++;
}

static void constant_vector_grow(struct constant_vector *ca)
{
	ca->size += CONSTANT_VECTOR_BUFFER_COUNT * sizeof(double);
	ca->array = realloc(ca->array, ca->size);

	ASSERT(ca->array != NULL, "Unable to grow constant_vector.");
}
