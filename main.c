// Internal dependencies
#include "artichoke.h"


int main(int argc, const char * argv[])
{
    // basic init
    myhtml_t* myhtml = myhtml_create();
    myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
    
    // first tree init
    myhtml_tree_t* tree = myhtml_tree_create();
    myhtml_tree_init(tree, myhtml);

    char *html_buffer = (
        "<html>"
            "<head>"
                "<title>Hello World!</title>"
                "<meta charset=\"utf8\">"
                "<meta name=\"author\" content=\"ducdetronquito\">"
            "</head>"
            "<body>"
                "Hello World"
                "<div>"
                    "<p>"
                        "I am a super paragraph !"
                    "</p>"
                    "<p>"
                        "Wow me too, but with a <span>super span !</span>"
                    "</p>"
                "</div>"
            "</body>"
        "</html>"
    );

    // parse html
    myhtml_parse(tree, MyENCODING_UTF_8, html_buffer, strlen(html_buffer));

    // Clean DOM.
    artichoke_clean(tree);
    
    myhtml_tree_destroy(tree);
    myhtml_destroy(myhtml);

    return 0;
}
