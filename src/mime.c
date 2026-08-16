/*
 * This function determines the given resource's file type and then returns the
 * corresponding formatted mime type.
 */

#include <string.h>

#include "mime.h"

#define DEFAULT_TYPE "application/octet-stream"

char *get_mime_type(char *resource) {
    if (resource == NULL) {
        return DEFAULT_TYPE;
    }

    char *type = strrchr(resource, '.');

    if (type == NULL) {
        return DEFAULT_TYPE;
    }

    type++;  /* increment so comparing the file type without . */

    if ((strcasecmp(type, "html") == 0) || (strcasecmp(type, "htm") == 0)) {
        return "text/html";
    }

    if ((strcasecmp(type, "jpeg") == 0) || (strcasecmp(type, "jpg") == 0)) {
        return "image/jpg";
    }

    if (strcasecmp(type, "css") == 0) {
        return "text/css";
    }

    if (strcasecmp(type, "js") == 0) { 
        return "application/javascript";
    }

    if (strcasecmp(type, "json") == 0) {
        return "application/json";
    }
    
    if (strcasecmp(type, "txt") == 0) {
        return "text/plain";
    }

    if (strcasecmp(type, "gif") == 0) {
        return "image/gif";
    }

    if (strcasecmp(type, "png") == 0) {
        return "image/png";
    }

    if (strcasecmp(type, "ico") == 0) {
        return "image/x-icon";
    }
} /* get_mime_type() */
