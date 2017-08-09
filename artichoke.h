#ifndef ARTICHOKE_H
#define ARTICHOKE_H

// Builtin dependencies
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
    myhtml_tree_node_t *checkpoints[10];
} context_t;


void artichoke_clean(myhtml_tree_t* tree);
bool artichoke_get_next_node(myhtml_tree_node_t *node, context_t *context);

#endif // ARTICHOKE_H
