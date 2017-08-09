#include "artichoke.h"


void artichoke_clean(myhtml_tree_t* tree) {
    myhtml_tree_node_t *html_node = myhtml_tree_get_node_html(tree);
    
    myhtml_tree_node_t *current_node = html_node;

    myhtml_tag_name_by_id(tree, current_node->tag_id, NULL);
    
    context_t context = {
        .head=-1
    };

    while (artichoke_get_next_node(current_node, &context)) {
        printf("%s\n", myhtml_tag_name_by_id(tree, current_node->tag_id, NULL));
    }

}

// `artichoke_get_next_node` is the core of the iteration process
// The idea is to store the minimal amout of data to be able
// iterate through the tree, without having to check for
// "already seen" node.
bool artichoke_get_next_node(myhtml_tree_node_t *node, context_t *context) {
    if (node->child) {
        // If our incoming node has a child, this child will be
        // the start for the next iteration.
        *node = *(node->child);
        if (node->next) {
            // But, if this child has a sibling, we need
            // to store it in our context, and we are going to
            // call him a "checkpoint".
            // It means that, when the iteration stop to a lead node,
            // it will start back to the last checkpoint.
            // Keeping "checkpoints" is a way to avoid marking
            // nodes as "seen".
            context->head++;
            context->checkpoints[context->head] = node->next;
        }
        return true;
    } else if (context->head >= 0) {
        // If our incoming node has no more child, meaning he is a leaf,
        // the next step of the iteration will be the last checkpoint.
        *node = *(context->checkpoints[context->head]);
        if (node->next) {
            context->checkpoints[context->head] = node->next;
        } else {
            context->checkpoints[context->head] = NULL;
            context->head--;
        }
        return true;
    } else {
        return false;
    }
}
