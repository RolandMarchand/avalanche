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

#include "macros.hpp"
#include "hashmap.hpp"

#include <crypt.h>
#include <string.h>

#define HASH(string) (crypt(string, "$3$"))
#define GET_BUCKET(hashmap, index) (&hashmap->buckets[(index) % MAX_SIZE])
#define IS_BUCKET_SET(bucket) (b->hash[0])
/* Buckets that require linear probing are marked. */
#define IS_BUCKET_PROBED(bucket) (b->hash[0] == 'p')

static int hash(char *string);
static int hash2int(char *hash);

struct hashmap hashmap::init()
{
	struct hashmap hm;

	for (int i = 0; i < MAX_SIZE; i++)
		hm.buckets[i] = (struct bucket){.hash[0] = 0};

	return hm;
}

enum hashmap::error hashmap::set(struct hashmap *hm, char *key, double value)
{
	char *hash = HASH(key);
	int idx = hash2int(hash);

	for (int i = 0; i < MAX_SIZE; i++) {
		struct bucket *b = GET_BUCKET(hm, idx + i);

		if (IS_BUCKET_SET(b)) {
			if (strcmp(b->hash, hash) != 0) continue;
		} else {
			strcpy(b->hash, hash);
		}

		/* Mark the bucket as requiring probing. */
		if (i != 0) {
			b->hash[0] = 'p';
		}

		b->value = value;
		return OK;
	}

	return FULL;
}

double hashmap::get(struct hashmap *hm, char *key)
{
	char *hash = HASH(key);
	int idx = hash2int(hash);

	for (int i = 0; i < MAX_SIZE; i++) {
		struct bucket *b = GET_BUCKET(hm, idx + i);

		if (!IS_BUCKET_SET(b)) return NAN;
		if (strcmp(b->hash, hash) == 0) return b->value;
	}

	return NAN;
}

void hashmap::remove(struct hashmap *hm, char *key)
{
	char *hash = HASH(key);
	int idx = hash2int(hash);

	for (int i = 0; i < MAX_SIZE; i++) {
		struct bucket *b = GET_BUCKET(hm, idx + i);
		if (!IS_BUCKET_SET(b)) return;

		if (strcmp(b->hash, hash) != 0) continue;

		/* Scroll back all values requiring probing. */
		while (1) {
#define NEXT_BUCKET (GET_BUCKET(hm, idx + i))
			i++;
			if (IS_BUCKET_PROBED(NEXT_BUCKET)) {
				*b = *NEXT_BUCKET;
				b = NEXT_BUCKET;
				continue;
			}
			
			b->hash[0] = 0;
			return;
#undef NEXT_BUCKET
		}
	}
}

static int hash2int(char *hash)
{
	ASSERT(!strncmp(hash, "$3$$", 4)	\
	       && strlen(hash) == 36,
	       "Hash must be computed with NT algorithm.\n");

	/* Skip the NT algorithm's signature. */
	hash += 4;

	int ret = 0;
	for (int i = 0; i < sizeof(int); i++) {
		ret <<= 8;
		ret |= *hash;
		hash++;
	}

	return ret % hashmap::MAX_SIZE;
}
