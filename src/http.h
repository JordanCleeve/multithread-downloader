#ifndef HTTP_H
#define HTTP_H


/*
 * http_query:
 *
 * Perform an HTTP 1.0 query to a given host and page and port number.
 * host is a hostname and page is a path on the remote server.
 *
 * On any error (for example an invalid path or host name)
 * NULL is returned and any resources associated with the query
 * are cleaned up. An error may be printed on stderr.
 *
 * The string returned is the raw http response with content,
 * and the user is responsible for freeing the memory.
 *
 * arguments:
 *    host - hostname e.g. www.canterbury.ac.nz
 *    page - e.g.  /index.html
 *
 */
typedef struct {
    char *data;
    size_t length;

} Buffer;


Buffer* http_query(char *host, char *page, int port);

/*
 * http_content:
 *
 * Separate the content from the header of an http request.
 * NOTE: returned string is an offset into the response, so
 * should not be freed by the user. Do not copy the data.
 *
 * return NULL on failure (i.e. the buffer passed in is not a HTTP response)
 *
 */
char* http_get_content(Buffer *response);

Buffer *http_url(const char *url);

inline static void buffer_free(Buffer *buffer) {
    free(buffer->data);
    free(buffer);
}


#endif