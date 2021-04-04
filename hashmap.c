#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashmap.h"

int hash(char *str)
{
	int i;
	int r = 0;

	for (i = 0; str[i] != '\0'; i++)
		r += 31 * str[i];
	return r;
}

void _initMap(struct hashMap *ht, int size)
{
	int index;

	if (ht == NULL)
		return;
	ht->table = (link **)malloc(sizeof(link *) * size);
	ht->size = size;
	ht->count = 0;
	for (index = 0; index < size; index++)
		ht->table[index] = 0;
}

hashMap *createMap(int size)
{
	hashMap *ht;

	assert(size > 0);
	ht = malloc(sizeof(hashMap));
	assert(ht != 0);
	_initMap(ht, size);
	return ht;
}

void _freeMap(struct hashMap *ht)
{
	int i = 0;
	struct link *currLink, *nextLink;

	for (i = 0; i < ht->size; i++) {
		if (ht->count == 0)
			break;

		currLink = ht->table[i];
		if (currLink != 0)
			nextLink = currLink->next;
		while (currLink != 0) {
			free(currLink->key);
			free(currLink->value);

			free(currLink);
			ht->count--;

			currLink = nextLink;
			if (currLink != 0)
				nextLink = currLink->next;
		}
	}
	free(ht->table);
}

void deleteMap(hashMap *ht)
{
	assert(ht != 0);
	_freeMap(ht);
	free(ht);
}

void _setTableSize(struct hashMap *ht, int newTableSize)
{
	int i = 0;
	struct hashMap *newHM, *oldHM;
	struct link *currLink;

	newHM = createMap(newTableSize);
	oldHM = ht;

	while (i < ht->size) {
		currLink = ht->table[i];
		while (currLink != 0) {
			insertMap(newHM, currLink->key, currLink->value);
			currLink = currLink->next;
		}
		i++;
	}

	_freeMap(oldHM);
	ht->table = newHM->table;
	ht->size = newHM->size;
	ht->count = newHM->count;
}

void insertMap(struct hashMap *ht, KType k, VType v)
{
	int index;
	struct link *newlink;
	char *newKType = (char *) malloc(strlen(k) + 1);
	char *newVType = (char *) malloc(strlen(v) + 1);
	float loadRatio = 0.0;

	newlink = (struct link *) malloc(sizeof(struct link));

	index = hash(k) % ht->size;

	if (index < 0)
		index += ht->size;

	assert(newlink);

	if (isKey(ht, k))
		removeKey(ht, k);

	strcpy(newKType, k);
	strcpy(newVType, v);

	newlink->key = newKType;
	newlink->value = newVType;
	newlink->next = ht->table[index];
	ht->table[index] = newlink;

	ht->count++;


	loadRatio = (float)ht->count / (float)ht->size;

	if (loadRatio >= 0.75)
		_setTableSize(ht, ht->size * 2);

}

VType inMap(struct hashMap *ht, KType k)
{
	int index;
	struct link *currLink;

	index = hash(k) % ht->size;

	currLink = ht->table[index];
	while (currLink != 0) {
		if (strcmp(k, currLink->key) == 0)
			return currLink->value;
		currLink = currLink->next;
	}

	return 0;
}

int isKey(struct hashMap *ht, KType k)
{
	int index;
	struct link *currLink;

	index = hash(k) % ht->size;

	currLink = ht->table[index];
	while (currLink != 0) {
		if (strcmp(k, currLink->key) == 0)
			return 1;
		currLink = currLink->next;
	}

	return 0;
}

void removeKey(struct hashMap *ht, KType k)
{
	int index;
	struct link *lastLink, *currLink;

	index = hash(k) % ht->size;

	currLink = ht->table[index];
	lastLink = currLink;
	while (currLink != 0) {
		if (strcmp(k, currLink->key) == 0) {
			if (lastLink == currLink) {
				lastLink = currLink->next;
				ht->table[index] = lastLink;
			} else
				lastLink->next = currLink->next;

			free(currLink->value);
			free(currLink->key);

			free(currLink);

			ht->count--;

			break;
		}
		currLink = currLink->next;
	}
}

int size(const struct hashMap *ht)
{
	return ht->count;
}

int capacity(const struct hashMap *ht)
{
	return ht->size;
}
