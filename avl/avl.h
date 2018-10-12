/*
 * Copyright (C) 1995       Sam Rushing <rushing@nightmare.com>,
 * Copyright (C) 2012-2018  Philipp "ph3-der-loewe" Schafft <lion@lion.leolix.org>
 */

/* $Id: avl.h,v 1.7 2003/07/07 01:10:14 brendan Exp $ */

#ifndef _LIBIGLOO__AVL_H_
#define _LIBIGLOO__AVL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define AVL_KEY_PRINTER_BUFLEN (256)

#ifndef NO_THREAD
#include "thread/thread.h"
#else
#define thread_rwlock_create(x) do{}while(0)
#define igloo_thread_rwlock_destroy(x) do{}while(0)
#define thread_rwlock_rlock(x) do{}while(0)
#define thread_rwlock_wlock(x) do{}while(0)
#define thread_rwlock_unlock(x) do{}while(0)
#endif

typedef struct avl_node_tag {
  void *        key;
  struct avl_node_tag *    left;
  struct avl_node_tag *    right;  
  struct avl_node_tag *    parent;
  /*
   * The lower 2 bits of <rank_and_balance> specify the balance
   * factor: 00==-1, 01==0, 10==+1.
   * The rest of the bits are used for <rank>
   */
  unsigned int        rank_and_balance;
#if !defined(NO_THREAD) && defined(HAVE_AVL_NODE_LOCK)
  rwlock_t rwlock;
#endif
} avl_node;

#define AVL_GET_BALANCE(n)    ((int)(((n)->rank_and_balance & 3) - 1))

#define AVL_GET_RANK(n)    (((n)->rank_and_balance >> 2))

#define AVL_SET_BALANCE(n,b) \
  ((n)->rank_and_balance) = \
    (((n)->rank_and_balance & (~3)) | ((int)((b) + 1)))

#define AVL_SET_RANK(n,r) \
  ((n)->rank_and_balance) = \
    (((n)->rank_and_balance & 3) | (r << 2))

struct _avl_tree;

typedef int (*avl_key_compare_fun_type)    (void * compare_arg, void * a, void * b);
typedef int (*avl_iter_fun_type)    (void * key, void * iter_arg);
typedef int (*avl_iter_index_fun_type)    (unsigned long index, void * key, void * iter_arg);
typedef int (*avl_free_key_fun_type)    (void * key);
typedef int (*avl_key_printer_fun_type)    (char *, void *);

/*
 * <compare_fun> and <compare_arg> let us associate a particular compare
 * function with each tree, separately.
 */

typedef struct _avl_tree {
  avl_node *            root;
  unsigned int          height;
  unsigned int          length;
  avl_key_compare_fun_type    compare_fun;
  void *             compare_arg;
#ifndef NO_THREAD
  rwlock_t rwlock;
#endif
} avl_tree;

avl_tree * igloo_avl_tree_new (avl_key_compare_fun_type compare_fun, void * compare_arg);
avl_node * igloo_avl_node_new (void * key, avl_node * parent);

void igloo_avl_tree_free (
  avl_tree *        tree,
  avl_free_key_fun_type    free_key_fun
  );

int igloo_avl_insert (
  avl_tree *        ob,
  void *        key
  );

int igloo_avl_delete (
  avl_tree *        tree,
  void *        key,
  avl_free_key_fun_type    free_key_fun
  );

int igloo_avl_get_by_index (
  avl_tree *        tree,
  unsigned long        index,
  void **        value_address
  );

int igloo_avl_get_by_key (
  avl_tree *        tree,
  void *        key,
  void **        value_address
  );

int igloo_avl_iterate_inorder (
  avl_tree *        tree,
  avl_iter_fun_type    iter_fun,
  void *        iter_arg
  );

int igloo_avl_iterate_index_range (
  avl_tree *        tree,
  avl_iter_index_fun_type iter_fun,
  unsigned long        low,
  unsigned long        high,
  void *        iter_arg
  );

int igloo_avl_get_span_by_key (
  avl_tree *        tree,
  void *        key,
  unsigned long *    low,
  unsigned long *    high
  );

int igloo_avl_get_span_by_two_keys (
  avl_tree *        tree,
  void *        key_a,
  void *        key_b,
  unsigned long *    low,
  unsigned long *    high
  );

int igloo_avl_verify (avl_tree * tree);

void igloo_avl_print_tree (
  avl_tree *        tree,
  avl_key_printer_fun_type key_printer
  );

avl_node *igloo_avl_get_first(avl_tree *tree);

avl_node *igloo_avl_get_prev(avl_node * node);

avl_node *igloo_avl_get_next(avl_node * node);

/* These two are from David Ascher <david_ascher@brown.edu> */

int igloo_avl_get_item_by_key_most (
  avl_tree *        tree,
  void *        key,
  void **        value_address
  );

int igloo_avl_get_item_by_key_least (
  avl_tree *        tree,
  void *        key,
  void **        value_address
  );

/* optional locking stuff */
void igloo_avl_tree_rlock(avl_tree *tree);
void igloo_avl_tree_wlock(avl_tree *tree);
void igloo_avl_tree_unlock(avl_tree *tree);
void avl_node_rlock(avl_node *node);
void avl_node_wlock(avl_node *node);
void avl_node_unlock(avl_node *node);

#ifdef __cplusplus
}
#endif

#endif /* __AVL_H */
