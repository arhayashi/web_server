#ifndef READ_FILE_H

#define READ_FILE_H

typedef struct {
    int size;
    void *content;
} file_cont_t;

void close_file(FILE **);
file_cont_t *create_file_cont_t();
void free_file_cont_t(file_cont_t **);
file_cont_t *read_file_cont(char *);

#endif
