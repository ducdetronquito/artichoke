// Internal dependencies
#include "artichoke.h"

// MACRO that provide a clearer API to iterate through a HTML tree.
#define foreach_tree_node(root_node)                                                               \
    context_t artichoke_context = {.head = -1};                                                    \
    while (artichoke_get_next_node(&root_node, &artichoke_context))

const int TEXT_BLOCK_TAGS_LENGTH = 3;
const long unsigned int TEXT_BLOCK_TAGS[] = {MyHTML_TAG_BODY, MyHTML_TAG_DIV, MyHTML_TAG_P};

void artichoke_clean(myhtml_tree_t *tree) {
    myhtml_tree_node_t *body = myhtml_tree_get_node_body(tree);
    text_block_collection_t collection = {.length = 0};

    artichoke_get_text_blocks(body, &collection);
    printf("Collection length: %i.\n", collection.length);

    for (int i = 0; i < collection.length; i++) {
        printf("Text block %i:\n", i);
        printf("\t- tag: %lx.\n", collection.items[i].node->tag_id);
        printf("\t- word count: %i.\n", collection.items[i].word_count);
        printf("\t- hyperlink word count: %i.\n", collection.items[i].hyperlink_words_count);
    }
}

void artichoke_get_text_blocks(myhtml_tree_node_t *root_node, text_block_collection_t *collection) {
    text_block_t *current_text_block;

    const char *node_text;
    int word_count;
    // int i = 0;
    text_block_t *current_block;
    myhtml_tree_node_t *hyperlink_end_scope = NULL;
    foreach_tree_node(root_node) {
        // printf("Node %i:\n", i);
        // printf("\t- tag: <%s>.\n", myhtml_tag_name_by_id(root_node->tree, root_node->tag_id,
        // NULL));
        // printf("\t- length: %i.\n", collection->length);
        // printf("\t- item adress: %p.\n", &(collection->items[collection->length]));

        if (root_node == hyperlink_end_scope) {
            hyperlink_end_scope = NULL;
        }

        if (root_node->tag_id == MyHTML_TAG_A) {
            hyperlink_end_scope = artichoke_context.checkpoints[artichoke_context.head];
        } else if (root_node->tag_id == MyHTML_TAG__TEXT) {
            node_text = myhtml_node_text(root_node, NULL);
            word_count = artichoke_count_words(node_text);
            current_block = &(collection->items[collection->length - 1]);
            // printf("\t--> Adding text content: %s.\n", node_text);
            // printf("\t\t - previous tag: <%s>.\n",
            // myhtml_tag_name_by_id(current_block->node->tree, current_block->node->tag_id, NULL));
            if (hyperlink_end_scope) {
                current_block->hyperlink_words_count += word_count;
            }
            current_block->word_count += word_count;
        } else if (artichoke_is_text_block(root_node->tag_id)) {
            collection->items[collection->length].node = root_node;
            collection->length++;
        }
        // i++;
    }
}

bool artichoke_get_next_node(myhtml_tree_node_t **node, context_t *context) {
    if ((*node)->child) {
        *node = (*node)->child;
        if ((*node)->next) {
            context->head++;
            context->checkpoints[context->head] = (*node)->next;
        }
        return node;
    } else if (context->head >= 0) {
        *node = context->checkpoints[context->head];
        if ((*node)->next) {
            context->checkpoints[context->head] = (*node)->next;
        } else {
            context->checkpoints[context->head] = NULL;
            context->head--;
        }
        return node;
    } else {
        return NULL;
    }
}

bool artichoke_is_text_block(long int tag_id) {
    for (int i = 0; i < TEXT_BLOCK_TAGS_LENGTH; i++) {
        if (tag_id == TEXT_BLOCK_TAGS[i]) {
            return true;
        }
    }
    return false;
}

int artichoke_count_words(const char *string) {
    bool seeking_word = true;
    int word_count = 0;
    while (*string != '\0') {
        if (seeking_word) {
            if (!isspace(*string)) {
                word_count++;
                seeking_word = !seeking_word;
            }
        } else {
            if (isspace(*string)) {
                seeking_word = !seeking_word;
            }
        }
        string++;
    }
    return word_count;
}
