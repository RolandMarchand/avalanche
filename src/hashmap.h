#pragma once

#include <math.h>

#define MAX_SIZE 256

enum hashmap_error {
	HASHMAP_OK,
	HASHMAP_NOT_FOUND,
	HASHMAP_FULL
};

struct bucket {
	char hash[36]; /* Size of NT hash */
	double value;
};

struct hashmap {
	struct bucket buckets[MAX_SIZE];
};

struct hashmap hashmap_init();
enum hashmap_error hashmap_set(struct hashmap *hm, char *key, double value);
double hashmap_get(struct hashmap *hm, char *key);
void hashmap_del(struct hashmap *hm, char *key);
