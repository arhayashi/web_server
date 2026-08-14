/*
 * This file contains code to gather information about a file and put its
 * content in a file_content_t struct.
 */

#include <stdlib.h>

#include "process_file.h"

/*
 * Dynamically allocates a file_content_t struct.
 */

file_cont_t *create_file_cont_t() {
    file_cont_t *file_cont = malloc(sizeof(file_cont_t));

    if (file_cont == NULL) {
        return NULL;
    }

    file_cont->size = 0;
    file_cont->content = NULL;

    return file_cont;
} /* create_file_cont_t() */

int main(void) {

}
