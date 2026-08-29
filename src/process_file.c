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
    if (fd == NULL) {
        return;
    }

    fclose(*fd);
    *fd = NULL;
} /* close_file() */

/*
 * Dynamically allocates a file_content_t struct.
 */

file_cont_t *create_file_cont_t(int size, void *content) {
    file_cont_t *file_cont = malloc(sizeof(file_cont_t));

    if (file_cont == NULL) {
        fprintf(stderr, "[ERROR] unable to allocate space for file_cont_t"
                "struct\n");
        return NULL;
    }

    file_cont->size = size;
    file_cont->content = content;

    return file_cont;
} /* create_file_cont_t() */

/*
 * Frees a file_cont_t struct and sets the pointer to NULL.
 */

void free_file_cont_t(file_cont_t **file_cont) {
    if ((file_cont == NULL) || (*file_cont == NULL)) {
        return;
    }

    free((*file_cont)->content);
    free(*file_cont);
    *file_cont = NULL;
} /* free_file_cont_t() */

/*
 * Returns a pointer to a dynamically allocated, filled in file_cont_t struct.
 * The indicator is set to FILE_ERR if any issues related to reading the file
 * or storing its contents were encountered. It is set to FILE_NEXS if the
 * file doesn't exist or have correct read permissions. On success, it is set
 * to FILE_SUCC.
 */

file_cont_t *read_file_cont(char *file, int *indicator) {
    if (file == NULL) {
        fprintf(stderr, "[ERROR] file name not given\n");
        *indicator = FILE_ERR;
        return NULL;
    }

    int status;

    FILE *fd = fopen(file, "rb");

    if (fd == NULL) {
        fprintf(stderr, "[WARNING] file unable to be read or doesn't exist\n");
        *indicator = FILE_NEXS;
        return NULL;
    }

    /* get info about the file like its size and type */

    struct stat file_stats;
    status = stat(file, &file_stats);

    if (status == -1) {
        fprintf(stderr, "[WARNING] unable to obtain file stats\n");
        *indicator = FILE_ERR;
        close_file(&fd);
        return NULL;
    }

    /* if directory, will try to access dir/index.html in http.c */

    if (S_ISDIR(file_stats.st_mode)) {
        fprintf(stderr, "[WARNING] attempting to access directory\n");
        *indicator = FILE_DIR;
        close_file(&fd);
        return NULL;
    }

    /* ie. symbolic links */

    if (!S_ISREG(file_stats.st_mode)) {
        fprintf(stderr, "[ERROR] attempting to access non-regular file\n");
        *indicator = FILE_ERR;
        close_file(&fd);
        return NULL;
    }

    int file_size = (int)file_stats.st_size;  /* original file size in bytes */
    void *buff = calloc(1, file_size);        /* store file content */

    if (buff == NULL) {
        fprintf(stderr, "[ERROR] unable to allocate space for file content\n");
        *indicator = FILE_ERR;
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
        /* status stores number of bytes that were read */

        buf_pos += status;
        read_size += status;
    }

    if (ferror(fd)) {
        fprintf(stderr, "[ERROR] encountered error while reading from file\n");
        *indicator = FILE_ERR;
        close_file(&fd);
        free(buff);
        return NULL;
    }

    file_cont_t *file_cont = create_file_cont_t(read_size, buff);

    if (file_cont == NULL) {
        /* error for failed malloc inside function call */

        *indicator = FILE_ERR;
        close_file(&fd);
        free(buff);
        return NULL;
    }

    *indicator = FILE_SUCC;
    
    return file_cont;
} /* read_file() */
