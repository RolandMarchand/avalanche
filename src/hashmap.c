#include "macros.h"
#include "hashmap.h"

#include <crypt.h>
#include <string.h>

/* $3$ means to use the NT hash algorithm. */
#define HASH(string) (crypt(string, "$3$"))
#define GET_BUCKET(hashmap, index) (&hashmap->buckets[(index) % MAX_SIZE])
#define IS_BUCKET_SET(bucket) (b->hash[0])

static int hash(char *string);
static int hash2int(char *hash);

struct hashmap hashmap_init()
{
	struct hashmap hm;

	for (int i = 0; i < MAX_SIZE; i++)
		hm.buckets[i] = (struct bucket){.hash[0] = 0};

	return hm;
}

enum hashmap_error hashmap_set(struct hashmap *hm, char *key, double value)
{
	char *hash = HASH(key);
	int idx = hash2int(hash);

	for (int i = 0; i < MAX_SIZE; i++) {
		struct bucket *b = GET_BUCKET(hm, idx + i);

		if (IS_BUCKET_SET(b)) {
			/* Check for hash collisions, continue if it's a different hash. */
			if (strcmp(b->hash, hash) != 0) continue;
		} else {
			/* We have a new free bucket, so we set the key hash. */
			strcpy(b->hash, hash);
		}

		b->value = value;
		return HASHMAP_OK;
	}

	return HASHMAP_FULL;
}

double hashmap_get(struct hashmap *hm, char *key)
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

void hashmap_del(struct hashmap *hm, char *key)
{
	char *hash = HASH(key);
	int idx = hash2int(hash);

	for (int i = 0; i < MAX_SIZE; i++) {
		struct bucket *b = GET_BUCKET(hm, idx + i);
		if (!IS_BUCKET_SET(b)) return;

		if (strcmp(b->hash, hash) != 0) continue;

		b->hash[0] = 0;
		return;
	}
}

static int hash2int(char *hash)
{
	ASSERT(!strncmp(hash, "$3$$", 4)	\
	       && strlen(hash) == 36,
	       "Hash must be computed with NT algorithm.\n");

	/* Skip the NT algorithm's signature */
	hash += 4;

	int ret = 0;
	for (size_t i = 0; i < sizeof(int); i++) {
		ret <<= 8;
		ret |= *hash;
		hash++;
	}

	return ret % MAX_SIZE;
}
