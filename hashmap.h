#ifndef HashMap
#define HashMap

#define KType char*
#define VType char*

struct link {
	KType key;
	VType value;
	struct link *next;
};

typedef struct link link;

struct hashMap {
	link **table; /*array of pointers to links*/
	int size; /*no of buckets*/
	int count; /*no of links*/
};

typedef struct hashMap hashMap;

int hash(char *str);

/*init the hashMap*/
void initMap(struct hashMap *ht, int size);

struct hashMap *createMap(int size);

void insertMap(struct hashMap *ht, KType k, VType v);

/* returns the value stored specified by the key k */
VType inMap(struct hashMap *ht, KType k);

/* key is in the hashtable. 0 is no */
int isKey(struct hashMap *ht, KType k);

/* remove key */
void removeKey(struct hashMap *ht, KType k);

/* returns the no. of links */
int size(const struct hashMap *ht);

/* returns no. of buckets */
int capacity(const struct hashMap *ht);

void freeMap(struct hashMap *ht);

void deleteMap(struct hashMap *ht);

#endif
