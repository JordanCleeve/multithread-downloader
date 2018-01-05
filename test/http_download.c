#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "http.h"


int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "usage: ./http_download url filename\n");
        exit(1);
    }

    char *url = argv[1];
    char *filename = argv[2];

    Buffer *response = http_url(url);


    FILE *fp = fopen(filename, "w");

    if (response) {
        if (fp == NULL) {
            fprintf(stderr, "error writing to: %s\n", filename);
            exit(EXIT_FAILURE);
        }

        char *data = http_get_content(response);
        size_t length = response->length - (data - response->data);


        fwrite(data, 1, length, fp);
        
        printf("downloaded %d bytes from %s\n", (int)length, url);
        buffer_free(response);

    }
    else {
        printf("failed to download from %s\n", url);
    }

    fclose(fp);

    return 0;
}

