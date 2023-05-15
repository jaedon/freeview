/* (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 U.S. copyright and other applicable intellectual property laws and
 may not be copied without the express permission of Netflix, Inc.,
 which reserves all rights.  Reuse of any of this content for any
 purpose without the permission of Netflix, Inc. is strictly
 prohibited.
*/

// Taken in part from: http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx

#ifndef _RED_BLACK_TREE_H_
#define _RED_BLACK_TREE_H_

#include "../ma_rss_info_generator.h"

typedef unsigned int uint32_t;

enum RBDIR {
    RBDIR_Left = 0,
    RBDIR_Right
};

typedef struct data_struct  {
    void* ptr;
    struct data_struct* next;
} DATA_STRUCT_CONTAINER;

typedef struct {
    int size;
    DATA_STRUCT_CONTAINER* top;
} DATA_STRUCT_LIST;

typedef struct rb_node {
    int red;
    uint32_t data;
    DATA_STRUCT_LIST list;
    struct rb_node *child[2];
} RB_NODE;

typedef struct rb_tree {
    RB_NODE *root;
} RB_TREE;


void rb_tree_init(RB_TREE** tree);
RB_NODE* rb_insert_r ( RB_NODE* node, uint32_t data, void* struct_ptr );
int rb_insert ( RB_TREE* tree, uint32_t data, void* struct_ptr );
int rb_assert ( RB_NODE *node );
RB_NODE* rb_make_node ( uint32_t data, void* data_struct );
RB_NODE* rb_rotate ( RB_NODE* node, int dir );
RB_NODE* rb_rotate_double ( RB_NODE* node, int dir );
RB_NODE* rb_search(RB_TREE* tree, uint32_t key);


void my_malloc_init(uint32_t* top, uint32_t* size);

#endif // _RED_BLACK_TREE_H_

