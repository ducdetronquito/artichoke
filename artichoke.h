#ifndef ARTICHOKE_H
#define ARTICHOKE_H

// Builtin dependencies
#include <ctype.h>   // isspace
#include <stdbool.h> // bool
// External dependecies
#include <myhtml/myhtml.h> // myhtml_node_text, myhtml_tag_name_by_id
#include <myhtml/tag.h>
#include <myhtml/tag_const.h>
#include <myhtml/tree.h> // myhtml_tree_get_node_html, myhtml_tree_node_t, myhtml_tree_t,

// `context_t` is used as a stack that store pointers to `myhtml_tree_node_t`
// I haven't coded a proper stack yet, so this one is limited in size.
// It can allow us to iterate over an HTML tree with at most 10 node of depth.
// It will be improved soon.
typedef struct {
    int head;
    int checkpoint_depths[10];
    myhtml_tree_node_t *checkpoints[10];
} context_t;

typedef struct {
    myhtml_tree_node_t *node;
    int word_count;
    int hyperlinks_word_count;
    int depth;
    
} text_block_t;

typedef struct {
    int length;
    text_block_t items[100];
} text_block_collection_t;

void artichoke_clean(myhtml_tree_t *tree);
bool artichoke_get_next_node(myhtml_tree_node_t **node, context_t *context);
void artichoke_get_text_blocks(myhtml_tree_node_t *root_node, text_block_collection_t *collection);
bool artichoke_is_text_block(long int tag_id);
int artichoke_count_words(const char *string);
#endif // ARTICHOKE_H
