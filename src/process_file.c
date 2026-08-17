/*
 * This file contains code to gather information about a file and put its
 * content in a file_content_t struct.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "process_file.h"

/*
 * Closes a file given by fd and sets the pointer to NULL.
 */

void close_file(FILE **fd) {
    fclose(*fd);
    *fd = NULL;
} /* close_file() */

/*
 * Dynamically allocates a file_content_t struct.
 */

file_cont_t *create_file_cont_t() {
    file_cont_t *file_cont = malloc(sizeof(file_cont_t));

    if (file_cont == NULL) {
        fprintf(stderr, "[ERROR] unable to create file_cont_t struct\n");
        return NULL;
    }

    file_cont->size = 0;
    file_cont->content = NULL;

    return file_cont;
} /* create_file_cont_t() */

/*
 * Frees a file_cont_t struct and sets the pointer to NULL.
 */

void free_file_cont_t(file_cont_t **file_cont) {
    free((*file_cont)->content);
    free(*file_cont);
    *file_cont = NULL;
} /* free_file_cont_t() */

/*
 * Returns a pointer to a dynamically allocated, filled in file_cont_t struct.
 */

file_cont_t *read_file_cont(char *file) {
    printf("%s\n", file);
    int status;

    FILE *fd = fopen(file, "rb");

    if (fd == NULL) {
        fprintf(stderr, "[ERROR] unable to open file for reading\n");
        return NULL;
    }

    struct stat file_stats;
    status = stat(file, &file_stats);

    if (status == -1) {
        fprintf(stderr, "[ERROR] unable to get file stats\n");
        close_file(&fd);
        return NULL;
    }

    if (!(file_stats.st_mode & S_IFREG)) {  /* mode is bitfield */
        fprintf(stderr, "[ERROR] attempting to access non-regular file\n");
        close_file(&fd);
        return NULL;
    }

    int file_size = (int)file_stats.st_size;  /* original file size in bytes */
    void *buff = calloc(1, file_size);        /* store read content */

    if (buff == NULL) {
        fprintf(stderr, "[ERROR] failed malloc to store file size\n");
        close_file(&fd);
        return NULL;
    }

    /* read_size stores num of bytes actually read in to keep track of real */
    /* file size since it could possibly change if written to while reading */

    int read_size = 0;
    void *buf_pos = buff;  /* keeps track of where to put stuff in buff */

    /* second condition ensures that don't write past buff in the case where */
    /* file is written to while reading where the file size would increase */

    while (((status = fread(buf_pos, 1, file_size - read_size, fd)) != 0) &&
           (read_size < file_size)) {
        /* status stores number of bytes read here */

        buf_pos += status;
        read_size += status;
    }

    if (ferror(fd)) {
        fprintf(stderr, "[ERROR] unable to read from file\n");
        close_file(&fd);
        free(buff);
        return NULL;
    }

    file_cont_t *file_cont = create_file_cont_t();

    file_cont->size = read_size;
    file_cont->content = buff;

    return file_cont;
} /* read_file() */

#if 0
int main() {
    /* quick test */

    file_cont_t *file_cont = read_file_cont("net.h");
    printf("size = %d\n", file_cont->size);
    printf("content: \n%s", file_cont->content);
}

#endif
