// Internal dependencies
#include "artichoke.h"

// MACRO that provide a clearer API to iterate through a HTML tree.
#define foreach_tree_node(root_node)                                                               \
    context_t artichoke_context = {.head = -1};                                                    \
    while (artichoke_get_next_node(&root_node, &artichoke_context))

const int TEXT_BLOCK_TAGS_LENGTH = 5;
const long unsigned int TEXT_BLOCK_TAGS[] = {
    MyHTML_TAG__TEXT,
    MyHTML_TAG_ARTICLE,
    MyHTML_TAG_BODY,
    MyHTML_TAG_DIV,
    MyHTML_TAG_P
};

void artichoke_clean(myhtml_tree_t *tree) {
    myhtml_tree_node_t *body = myhtml_tree_get_node_body(tree);
    text_block_collection_t collection = {.length = 0};

    artichoke_get_text_blocks(body, &collection);
    
    printf("Collection length: %i.\n", collection.length);

    for (int i = 0; i < collection.length; i++) {
        printf("Text block %i:\n", i);
        printf("\t- tag: %s.\n", myhtml_tag_name_by_id(tree, collection.items[i].node->tag_id, NULL));
	printf("\t- depth: %i.\n", collection.items[i].depth);
        printf("\t- word count: %i.\n", collection.items[i].word_count);
        printf("\t- hyperlink word count: %i.\n", collection.items[i].hyperlinks_word_count);
    }
}

void artichoke_get_text_blocks(myhtml_tree_node_t *node, text_block_collection_t *collection) {
    // Convenience proxies.
    const char *node_text;
    int word_count;
    text_block_t *current_block;
    
    // During the DFS iteration, we want to keep track of every
    // <a> tag scope. This allow to count words that are inside
    // hyperlinks.
    bool inside_hyperlink = false;
    int hyperlink_depth = -1;
    context_t iter_context = {
        .head = -1,
    };
    
    // Keep track of the depth of the current node during 
    // the iteration process.
    int depth = 0;
    int checkpoint_depths[10];
    int text_block_max_depth = 0;
    
    // 
    while (node) {
        // Text blocks extraction.
	printf("Node: %s.\n", myhtml_tag_name_by_id(node->tree, node->tag_id, NULL));
	if (inside_hyperlink && depth == hyperlink_depth) {
            inside_hyperlink = false;
	}

        if (node->tag_id == MyHTML_TAG_A) {
	    inside_hyperlink = true;
	    hyperlink_depth = depth;
	} else if (artichoke_is_text_block(node->tag_id)) {
	    current_block = &(collection->items[collection->length]);
	    current_block->node = node;
	    current_block->depth = depth;
	    
	    if (node->tag_id == MyHTML_TAG__TEXT) {
                node_text = myhtml_node_text(node, NULL);
		word_count = artichoke_count_words(node_text);
		current_block->word_count = word_count;

		if (inside_hyperlink) {
                    current_block->hyperlinks_word_count = word_count;
	        }
	    } 
            collection->length++;
	    if (depth > text_block_max_depth) {
	        text_block_max_depth = depth;
	    }
        }

	// Depth-First-Search iteration process.
	if (node->child) {
            node = node->child;
	    depth++;
            if (node->next) {
                iter_context.head++;
                iter_context.checkpoints[iter_context.head] = node->next;
		checkpoint_depths[iter_context.head] = depth;
            }
        } else if (iter_context.head >= 0) {
            node = iter_context.checkpoints[iter_context.head];
            depth = checkpoint_depths[iter_context.head];
	    if (node->next) {
                iter_context.checkpoints[iter_context.head] = node->next;
		checkpoint_depths[iter_context.head] = depth;
            } else {
                iter_context.checkpoints[iter_context.head] = NULL;
		checkpoint_depths[iter_context.head] = 0;
                iter_context.head--;
            }
        } else {
            node = NULL;
        }
    }
    /*
    printf("TExt block initial value\n");
    for (int i = 0; i < collection->length; i++) {
        printf("Text block %i:\n", i);
        printf("\t- tag: %s.\n", myhtml_tag_name_by_id(collection->items[i].node->tree, collection->items[i].node->tag_id, NULL));
        printf("\t- word count: %i.\n", collection->items[i].word_count);
        printf("\t- hyperlink word count: %i.\n", collection->items[i].hyperlink_words_count);
    }
    */
    // Aggregate the word count hierarchically for each text block.
    int word_count_by_depth[text_block_max_depth];
    int hyperlinks_word_count_by_depth[text_block_max_depth];
    int previous_depth = collection->items[collection->length -1].depth;
    for (int i = 0; i <= text_block_max_depth; i++) {
	word_count_by_depth[i] = 0;
	hyperlinks_word_count_by_depth[i] = 0;
    }
    
    for (int i = collection->length - 1; i >=0; i--) {
        current_block = &(collection->items[i]);
        
	printf("Text block %i:\n", i);
        printf("\t- tag: %s.\n", myhtml_tag_name_by_id(current_block->node->tree, current_block->node->tag_id, NULL));
        printf("\t- word count: %i.\n", current_block->word_count);
        printf("\t- hyperlink word count: %i.\n", current_block->hyperlinks_word_count);
        printf("\t--->We go up from %i to %i.\n", previous_depth, current_block->depth);
	
	if (current_block->depth < previous_depth) {
            current_block->word_count += word_count_by_depth[previous_depth];
	    current_block->hyperlinks_word_count += hyperlinks_word_count_by_depth[previous_depth];
            word_count_by_depth[current_block->depth] += current_block->word_count;
	    hyperlinks_word_count_by_depth[current_block->depth] += current_block->hyperlinks_word_count;
	    word_count_by_depth[previous_depth] = 0;
            hyperlinks_word_count_by_depth[previous_depth] = 0;
	} else {
            word_count_by_depth[current_block->depth] += current_block->word_count;
	    hyperlinks_word_count_by_depth[current_block->depth] += current_block->hyperlinks_word_count;
	}
        previous_depth = current_block->depth;
    }


    for (int i = 0; i < collection->length; i++) {
	
        if (artichoke_is_boilerplate(&(collection->items[i]))) {
	    myhtml_node_delete_recursive(collection->items[i].node);
	}
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


bool artichoke_is_boilerplate(text_block_t *text_block) {
    
}





