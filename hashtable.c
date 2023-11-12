// This is the implementation of the generic hash table ADT with binary search 
//   trees.

#include <stdlib.h>
#include "hashtable.h"
#include "cs136-trace.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

// -----------------------------------------------------------------------
// DO NOT CHANGE THESE
// See hashtable.h for documentation
const int HT_SUCCESS        = 0;
const int HT_ALREADY_STORED = 1;
const int HT_NOT_STORED     = 2;
// -----------------------------------------------------------------------

// a generic bstnode
struct bstnode {            
  void *key; 
  int level;           
  struct bstnode *left;
  struct bstnode *right;
};

// a generic bst
struct bst {
  struct bstnode *root;
};


// See hashtable.h for documentation
struct hashtable {
  int hash_length;
  int ht_length;
  int (*hash_func)(const void *, int);
  void *(*key_clone)(const void *);
  int (*key_compare)(const void *, const void *);
  void (*key_destroy)(void *);
  void (*key_print)(const void *);
  struct bst **table; 
};

// bst_create() creates a NULL bst
// effect: allocates memory (caller must call bst_destroy) 
// time: O(1)
static struct bst *bst_create(void) {
  struct bst *d = malloc(sizeof(struct bst));
  d->root = NULL;
  return d;
}

// free_bstnode(node, key_destroy) function that frees a bstnode
// requires: key_destory is a valid pointer
// effect: frees memory
// time: O(n * ds) where n is number of nodes and ds is the time complexity of key_destroy
static void free_bstnode(struct bstnode *node, void (*key_destroy)(void *)) {
  assert(key_destroy);

  if (node) {
    free_bstnode(node->left, key_destroy);
    free_bstnode(node->right, key_destroy);
    key_destroy(node->key);
    free(node);
  }
}

// bst_destroy(d, key_destroy) function that frees a bst
// requires: d and key_destroy are valid pointers
// effects: frees memory
// time: O(n*ds) where n is the numer of items in bst and ds is the time
//  complexity of key_destory
static void bst_destroy(struct bst *d, void (*key_destroy)(void *)) {
  assert(d);
  assert(key_destroy);

  free_bstnode(d->root, key_destroy);
  free(d);
}

// bst_lookup(key, key_compare, d) function that looksup the key provided
//   in the bst and returns it and returns NULL otherwise
// requires: key, key_compare, d are valid pointers
// time: O(n * co) where n is the number of items in bst and co is the
//   time complexity of key_compare
static const void *bst_lookup(const void *key, 
                               int (*key_compare)(const void *, const void *), 
                               const struct bst *d) {
  assert(d);
  assert(key_compare);
  assert(key);
  struct bstnode *node = d->root;
  while (node) {
    if (key_compare(node->key, key) == 0) {
      return node->key;
    }

    if (key_compare(key, node->key) < 0) {
      node = node->left;
    }
    else {
      node = node->right;
    }
  }
  return NULL;
}

// new_leaf(val, key_clone) returns a leaf node with the value provided
// requires: val, key_clone are valid pointers
// effect: allocates memory (caller must call bst_destroy)
// time: O(cl) where cl is the time complexity of key_clone
static struct bstnode *new_leaf(const void *val, 
                                void *(*key_clone)(const void *)) {
  assert(val);
  assert(key_clone);

  struct bstnode *leaf = malloc(sizeof(struct bstnode));
  leaf->key = key_clone(val);
  leaf->left = NULL;
  leaf->right = NULL;
  return leaf;
}

// bst_insert(val, key_compare, key_clone, d) inserts the given value 
//   in bst
// requires: val, d, key_compare, key_clone are valid pointers
// effect: modifies d and allocates memory (must call bst_destroy)
// time: O(n * co + cl) where n is the number of items in bst and s is the time
//   efficiency of key_compare and where cl is the time complexity of key_clone
static void bst_insert(const void *val, 
                        int (*key_compare)(const void *, const void *),
                        void *(*key_clone)(const void *),
                        struct bst *d) {
  assert(val);
  assert(d);
  assert(key_compare);
  assert(key_clone);

  struct bstnode *node = d->root;
  struct bstnode *parent = NULL;
  while (node && key_compare(node->key, val) != 0) {
    parent = node;
    if (key_compare(val, node->key) < 0) {
      node = node->left;
    } else {
      node = node->right;
    }
  }

  if (parent == NULL) { // empty tree
    d->root = new_leaf(val, key_clone);
  } else if (key_compare(val, parent->key) < 0) {
    parent->left = new_leaf(val, key_clone);
  } else {
    parent->right = new_leaf(val, key_clone);
  }
}

// bst_remove(key, key_compare, key_destroy, d) removes key from
//   bst
// requires: d, key, key_compare, key_destroy is a valid pointer
// effect: modifies d
//         frees memory
// time: O(n * co + ds) where n is the number of items in bst and where co
//   is the time complexity of key_compare and where ds is the time complexity of
//   key destroy
static void bst_remove(const void *key, 
                 int (*key_compare)(const void *, const void *),
                 void (*key_destroy)(void *),
                 struct bst *d) {
  assert(d);
  assert(key);
  assert(key_compare);
  assert(key_destroy);

  struct bstnode *target = d->root;
  struct bstnode *target_parent = NULL;
  // find the target (and its parent)
  while (target && key_compare(target->key, key) != 0) {
    target_parent = target;
    if (key_compare(key,target->key) < 0) {
      target = target->left;
    } else {
      target = target->right;
    }
  }
  if (target == NULL) {
    return; // key not found
  }
  // find the node to "replace" the target
  struct bstnode *replacement = NULL;
  if (target->left == NULL) {
    replacement = target->right;
  } else if (target->right == NULL) {
    replacement = target->left;
  } else {
    // neither child is NULL:
    // find the replacement node and its parent
    replacement = target->right;
    struct bstnode *replacement_parent = target;
    while (replacement->left) {
      replacement_parent = replacement;
      replacement = replacement->left;
    }
    // update the child links for the replacement and its parent
    replacement->left = target->left;
    if (replacement_parent != target) {
      replacement_parent->left = replacement->right;
      replacement->right = target->right;
    }
  }
  // free the target, and update the target's parent
  key_destroy(target->key);
  free(target);
  if (target_parent == NULL) {
    d->root = replacement;
  } else if (key_compare(key, target_parent->key) > 0) {
    target_parent->right = replacement;
  } else {
    target_parent->left = replacement;
  }
}

// bstnode_print(d, node, first, key_print) prints a bstnode
// requires: node, first, key_print are valid pointers
// effect: produces output
// time: O(cp) where cp is the time complexity of key print
static void bstnode_print(struct bstnode *node, 
                          bool *first,
                          void (*key_print)(const void *)) {
  assert(node);
  assert(first);
  assert(key_print);

  if (*first) {
    *first = false;
  } else {
    printf(",");
  }

  printf("%d-", node->level);
  key_print(node->key);
}

// bstnodes_print(d, node, first, key_print) prints bstnodes
// requires: first, key_print are valid pointers
// effect: produces output
// time: O(n * cp) where n is the number of nodes in bst and where cp is
//  the time complexity of key_print
static void bstnodes_print(struct bstnode *node, 
                           bool *first,
                           void (*key_print)(const void *),
                           int counter) { 
  assert(first);
  assert(key_print);

  if (node) {
    node->level = counter;
    bstnodes_print(node->left, first, key_print, counter+1);
    bstnode_print(node, first, key_print);
    bstnodes_print(node->right, first, key_print, counter+1);
  }
}

// bst_print(t, key_print) prints bst
// requires: t, key_print are valid pointers
// effect: produces output
// time: O(n * cp) where n is the number of nodes in bst and where cp is
//  the time complexity of key_print
void bst_print (struct bst *t, 
                void (*key_print)(const void *)) {
  assert(t);  
  assert(key_print);
  
  int counter = 0;

  bool first = true;
  bstnodes_print(t->root, &first, key_print, counter);
}

// See hashtable.h for documentation
struct hashtable *ht_create(void *(*key_clone)(const void *),
                            int (*hash_func)(const void *, int), 
                            int hash_length,
                            int (*key_compare)(const void *, const void *),
                            void (*key_destroy)(void *),
                            void (*key_print)(const void *)) {
  
  assert(key_clone);
  assert(hash_func);
  assert(hash_length > 0);
  assert(key_compare);
  assert(key_destroy);
  assert(key_print);

  struct hashtable *ht = malloc(sizeof(struct hashtable));
  ht->hash_length = hash_length;
  ht->hash_func = hash_func;
  ht->key_clone = key_clone;
  ht->key_destroy = key_destroy;
  ht->key_compare = key_compare;
  ht->key_print = key_print;

  ht->ht_length = 2;
  for (int i = 0; i < ht->hash_length-1; i++) {
    ht->ht_length *= 2;
  }

  ht->table = malloc(sizeof(struct bst) * ht->ht_length);

  for(int i = 0; i < ht->ht_length; i++) {
    ht->table[i] = NULL;
  }

  return ht;
}

// See hashtable.h for documentation
void ht_destroy(struct hashtable *ht) {
  assert(ht);

  for (int i = 0; i < ht->ht_length; i++) {
    if (ht->table[i] != NULL) {
      bst_destroy(ht->table[i], ht->key_destroy);
    }
  }
  free(ht->table);
  free(ht);
}

// See hashtable.h for documentation
int ht_insert(struct hashtable *ht, const void *key) {
  assert(ht);
  assert(key);

  const int hash_index = ht->hash_func(key, ht->hash_length);

  if (ht->table[hash_index] == NULL) {
    ht->table[hash_index] = bst_create();
  }

  const void *key_stored = bst_lookup(key, ht->key_compare, 
                                      ht->table[hash_index]);
  if (key_stored != NULL) {
    return HT_ALREADY_STORED;
  }
  else {
    bst_insert(key, ht->key_compare, ht->key_clone, 
                ht->table[hash_index]);
    return HT_SUCCESS;
  }
}

// See hashtable.h for documentation
int ht_remove(struct hashtable *ht, const void *key) {
  assert(ht);
  assert(key);

  const int hash_index = ht->hash_func(key, ht->hash_length);

  if (ht->table[hash_index] == NULL) {
    return HT_NOT_STORED;
  }

  const void *key_stored = bst_lookup(key, ht->key_compare, 
                                      ht->table[hash_index]);
  
  if (key_stored == NULL) {
    return HT_NOT_STORED;
  }
  else {
    bst_remove(key, ht->key_compare, ht->key_destroy, 
                ht->table[hash_index]);
    return HT_SUCCESS;
  }
}

// See hashtable.h for documentation
void ht_print(const struct hashtable *ht) {
  assert(ht);

  for (int i = 0; i < ht->ht_length; i++) {
    printf("%d: ", i);
    printf("[");
    if (ht->table[i] != NULL) {
      bst_print(ht->table[i], ht->key_print);
    }
    printf("]\n");
  }
}
