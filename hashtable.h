// HT_SUCCESS indicates successful execution of the function.
extern const int HT_SUCCESS;
// HT_ALREADY_STORED indicates that a key was already stored in the hashtable.
extern const int HT_ALREADY_STORED;
// HT_NOT_STORED indicates that a key was not stored in the hashtable.
extern const int HT_NOT_STORED;

// a generic hashtable
struct hashtable;

// requires: all functions require valid (non-NULL) parameters

// ht_create(key_clone, key_hash, hash_length, key_compare, key_destroy, 
//   key_print) creates a new empty generic hash table with the following
//   parameters:
//   * key_clone   clones a key;
//   * key_hash    calculate the hash value of a key;
//   * hash_length the length of the result of key_hash (in bits).
//   * key_compare compares two keys a, b with the following return values:
//                 ** < 0: a is less than b,
//                 ** = 0: a and b are equal, and
//                 ** > 0: b is less than a;
//   * key_destroy destroys a key;
//   * key_print   prints a key;
// effects: allocates heap memory; client must call ht_destroy
// requires: hash_length must be positive
// time: O(n), where n is the length of the hash table
struct hashtable *ht_create(void *(*key_clone)(const void *),
                            int (*hash_func)(const void *, int), 
                            int hash_length,
                            int (*key_compare)(const void *, const void *),
                            void (*key_destroy)(void *),
                            void (*key_print)(const void *));

// ht_destroy(ht) frees all resources allocated by the hashtable ht.
// effects: invalidates ht
// time: O(n + m), where n is the length of ht and m is the number of
//   items in ht
void ht_destroy(struct hashtable *ht);

// ht_insert(ht, key) inserts the key key into the hash table ht. The
//   function returns
//   * HT_SUCCESS if key has been inserted into ht or
//   * HT_ALREADY_STORED if key is already stored in ht.
// time: O(m + cl + co + hf), where m is the number of items in ht, 
//   cl: complexity of key_clone; co: complexity of key_compare; hf: complexity
//   of key_hash
int ht_insert(struct hashtable *ht, const void *key);

// ht_remove(ht, key) removes the key key from the hash table ht. The
//   function returns
//   * HT_SUCCESS if key has been removed from ht, or
//   * HT_NOT_STORED if key was not stored in ht.
// time: O(m + cl + co + hf), where m is the number of items in ht, 
//   cl: complexity of key_clone; co: complexity of key_compare; hf: complexity
//   of key_hash
int ht_remove(struct hashtable *ht, const void *key);

// ht_print(ht) prints the content of hash table ht to the console.
// effects: creates output
// time: O(n + m * cp), where n: length of ht, m: number of items in ht,
//   cp: complexity of key_print
void ht_print(const struct hashtable *ht);

