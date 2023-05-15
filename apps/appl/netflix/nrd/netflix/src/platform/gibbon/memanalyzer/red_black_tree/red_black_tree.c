/* (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 U.S. copyright and other applicable intellectual property laws and
 may not be copied without the express permission of Netflix, Inc.,
 which reserves all rights.  Reuse of any of this content for any
 purpose without the permission of Netflix, Inc. is strictly
 prohibited.
*/

// Taken in part from: http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_rbtree.aspx

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <ctype.h>
#include <execinfo.h>

#include "red_black_tree.h"

#ifdef ENABLE_RB_TREE
#define MEM_POOL_SIZE (  ( (sizeof(RB_NODE) + sizeof(DATA_STRUCT_CONTAINER)) * MAX_TRACE_ENTRIES + sizeof(uint32_t)-1) / sizeof(uint32_t) )
#define MY_MALLOC my_malloc


static RB_TREE g_rb_tree;
static pthread_mutex_t g_tree_mutex;


static uint32_t g_rb_tree_memory_pool[MEM_POOL_SIZE];
static uint32_t g_mem_pool_ptr;
static uint32_t g_mem_pool_size;

void my_malloc_init(uint32_t* top, uint32_t* size)
{
    g_mem_pool_ptr = (uint32_t) g_rb_tree_memory_pool;
    g_mem_pool_size = 0;

    *top = g_mem_pool_ptr;
    *size = MEM_POOL_SIZE * sizeof(uint32_t);
}

void * my_malloc(size_t size)
{
    void * p = NULL;

    size = (size + 3) & ~3;
    g_mem_pool_size += size;

    assert ( g_mem_pool_size  < ( MEM_POOL_SIZE * 4 ) );
    if ( g_mem_pool_size < ( MEM_POOL_SIZE * 4) ) {
        p = (void*) g_mem_pool_ptr;
        g_mem_pool_ptr += size;
    }

    return p;
}
#if 0
void dump_callstack()
{
    void *trace[64];
    int trace_size;
    int concatenated_callstack_size;
    char** callstack;
    int i;

    trace_size = backtrace(trace, 63);

    callstack = (char**) backtrace_symbols(trace, trace_size);

    printf("printing_call_stack!!!!\n");
    for ( i = 0;i < trace_size; i++){
        printf("%s\n", callstack[i]);
    }
    printf("\n");

    if ( callstack) {
        free ( callstack );
    }
}

void check_ptr(void* ptr)
{
    uint32_t addr = (uint32_t) ptr;
    uint32_t top = (uint32_t) g_rb_tree_memory_pool;
    uint32_t end = (uint32_t) (g_rb_tree_memory_pool + MEM_POOL_SIZE);
    fflush(stdout);
    if ( ptr < top || ptr > end ) {
        printf("ptr is invalid 0x%x (0x%x - 0x%x)\n", ptr, top, end);
        fflush(stdout);
        dump_callstack();
        exit(1);
    }

}
#endif

void rb_tree_init(RB_TREE** tree)
{   *tree = &g_rb_tree;
    (*tree)->root = (RB_NODE*) NULL;
    pthread_mutex_init(&g_tree_mutex, NULL);
}

void my_push(DATA_STRUCT_LIST* list, void* ptr)
{
    DATA_STRUCT_CONTAINER* new_entry = (DATA_STRUCT_CONTAINER*) MY_MALLOC(sizeof(DATA_STRUCT_CONTAINER));

    new_entry->ptr = ptr;
    new_entry->next = NULL;
    if ( list->top == NULL ) {
        list->top = new_entry;
    } else {
        new_entry->next = list->top;
        list->top = new_entry;
    }
    list->size++;

}

RB_NODE* rb_search(RB_TREE* tree, uint32_t key)
{

    RB_NODE* ptr = tree->root;

    pthread_mutex_lock(&g_tree_mutex);

    while ((ptr != 0) && (ptr->data != key)) {
        if (ptr->data > key) {
            ptr = ptr->child[RBDIR_Left];
        } else {
            ptr = ptr->child[RBDIR_Right];
        }
    }
    pthread_mutex_unlock(&g_tree_mutex);

    return ptr;
}


__inline int rb_is_red ( RB_NODE *node )
{
    return node != NULL && node->red == 1;
}

RB_NODE* rb_insert_r ( RB_NODE* node, uint32_t data, void* struct_ptr )
{
    if ( node == NULL ) {
        node = rb_make_node ( data, struct_ptr );
    } else if ( data == node->data ) {
        my_push(&node->list, struct_ptr);
    } else {
        int dir = (int) ((node->data > data)?RBDIR_Left:RBDIR_Right);
        node->child[dir] = rb_insert_r ( node->child[dir], data, struct_ptr );

        // rebalance
        if ( rb_is_red ( node->child[dir] ) ) {
            if ( rb_is_red ( node->child[!dir] ) ) {
                // case 1
                node->red = 1;
                node->child[0]->red = 0;
                node->child[1]->red = 0;
            } else {
                // case 2 & 3
                if ( rb_is_red ( node->child[dir]->child[dir] ) ) {
                    node = rb_rotate ( node, !dir );
                } else if ( rb_is_red ( node->child[dir]->child[!dir] ) ) {
                    node = rb_rotate_double ( node, !dir );
                }
            }
        }
    }

    return node;
}

int rb_insert ( RB_TREE* tree, uint32_t data, void* struct_ptr )
{
    pthread_mutex_lock(&g_tree_mutex);

    tree->root = rb_insert_r ( tree->root, data, struct_ptr );
    tree->root->red = 0;

    //rb_assert(tree->root);

    pthread_mutex_unlock(&g_tree_mutex);

    return 1;
}


RB_NODE* rb_rotate ( RB_NODE* node, int dir )
{

    RB_NODE* tmp = node->child[!dir];

    node->child[!dir] = tmp->child[dir];
    tmp->child[dir] = node;

    node->red = 1;
    tmp->red = 0;

    return tmp;
}

RB_NODE* rb_rotate_double ( RB_NODE* node, int dir )
{
    node->child[!dir] = rb_rotate ( node->child[!dir], !dir );
    return rb_rotate ( node, dir );
}

RB_NODE* rb_make_node ( uint32_t data, void* data_struct )
{
    RB_NODE* node = MY_MALLOC ( sizeof(RB_NODE) );

    assert ( node != NULL );

    if ( node != NULL ) {
        node->data = data;
        node->list.size = 0;
        node->list.top = NULL;
        my_push(&node->list, data_struct);
        node->red = 1; /* 1 is red, 0 is black */
        node->child[RBDIR_Left] = NULL;
        node->child[RBDIR_Right] = NULL;
    }

    return node;
}

#if 0
int rb_assert ( RB_NODE *node )
{
    int lh, rh;

    if ( node == NULL ) {
        return 1;
    } else {
        RB_NODE* ln = node->child[0];
        RB_NODE* rn = node->child[1];

        /* Consecutive red childs */
        if ( rb_is_red ( node ) ) {
            if ( rb_is_red ( ln ) || rb_is_red ( rn ) ) {
                puts ( "Red violation" );
                return 0;
            }
        }

        lh = rb_assert ( ln );
        rh = rb_assert ( rn );

        /* Invalid binary search tree */
        if ( ( ln != NULL && ln->data >= node->data )
          || ( rn != NULL && rn->data <= node->data ) )
        {
            puts ( "Binary tree violation" );
            return 0;
        }

        /* Black height mismatch */
        if ( lh != 0 && rh != 0 && lh != rh ) {
            puts ( "Black violation" );
            return 0;
        }

        /* Only count black childs */
        if ( lh != 0 && rh != 0 ) {
            return rb_is_red ( node ) ? lh : lh + 1;
        } else {
            return 0;
        }
    }
}
#endif

#endif // ENABLE_RB_TREE

