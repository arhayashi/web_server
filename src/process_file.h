#ifndef READ_FILE_H

#define READ_FILE_H

#define FILE_SUCC (1)  /* indicates file read and stored successfully */
#define FILE_NEXS (0)  /* indicates file doesn't exist or invalid perms */
#define FILE_ERR (-1)  /* indicates file reading or storing failed */
#define FILE_DIR (-2)  /* indicates file is actually a directory */

typedef struct {
    int size;
    void *content;
} file_cont_t;

void close_file(FILE **);
file_cont_t *create_file_cont_t(int, void *);
void free_file_cont_t(file_cont_t **);
file_cont_t *read_file_cont(char *, int *);

#endif
