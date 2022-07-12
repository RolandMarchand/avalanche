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

#include "token_vector.h"
#include "src/macros.h"

#include <stdlib.h>

static void token_vector_grow(struct token_vector *ta);

struct token_vector *token_vector_init()
{
	struct token_vector *ta = malloc(sizeof(struct token_vector));

	ASSERT(ta != NULL, "Unable to allocate memory for token_vector.");

	ta->count = 0;
	ta->size = TOKEN_VECTOR_BUFFER_COUNT * sizeof(struct token);
	ta->array = malloc(ta->size);

	ASSERT(ta->array != NULL, "Unable to allocate memory for token_vector.");

	return ta;
}

void token_vector_add(struct token_vector *ta, struct token t)
{
	if (ta->count == (ta->size / sizeof(struct token)))
		token_vector_grow(ta);
	ta->array[ta->count] = t;
	ta->count++;
}

static void token_vector_grow(struct token_vector *ta)
{
	ta->size += TOKEN_VECTOR_BUFFER_COUNT * sizeof(struct token);
	ta->array = realloc(ta->array, ta->size);

	ASSERT(ta->array != NULL, "Unable to grow token_vector.");
}

void token_vector_del(struct token_vector *ta)
{
	free(ta->array);
	free(ta);
	ta = NULL;
}
