#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "http.h"

#define BUF_SIZE 1024

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
        
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

// returns a socket connected to a given host:port 
int client_socket(char *hostname, int _port)
{
	int err;

	char port[20];
	err = snprintf(port, 20, "%d", _port); // Make a string out of the port number
	if (err < 0) handle_error("snprintf: failed");
	if (err >= 20) handle_error("socket creation: malformed port");

	struct addrinfo hints; // set hints for address
	struct addrinfo *result = NULL; // address structure for matched network address 

	int sockfd = socket(AF_INET, SOCK_STREAM, 0); // creates socket and returns the file descriptor
	if (sockfd == -1) handle_error_en(errno, "socket: failed");
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;        // use an internet address
	hints.ai_socktype = SOCK_STREAM;  // use TCP socket
	err = getaddrinfo(hostname, port, &hints, &result); // get address info assuming correct first result
	if (err != 0) {
		for (int i = 0; i < 100; i++) {
			if (getaddrinfo(hostname, port, &hints, &result) == 0) break;
			usleep(100);
			if (i == 99) {
				fprintf(stderr, "getaddrrinfo: %s\n", gai_strerror(err));
				exit(EXIT_FAILURE);
			}
		}
	}

	err = connect(sockfd, result->ai_addr, result->ai_addrlen); // connect to server
	if (err == -1) handle_error_en(errno, "connect: failed");
  
	freeaddrinfo(result);
  
	return sockfd;
}

// returns a char array containing a query built from given host and page information
char *build_get_query(char *host, char *page)
{
	char *request = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: getter\r\n\r\n";
	char *query = malloc(strlen(request) + strlen(host) + strlen(page));
	int err = sprintf(query, request, page, host);
	if (err < 0) handle_error("sprintf: failed");
	return query;
}

// sents given query to the server given from a connected socket
void send_query(int sockfd, char *query) {
	int err;
	if ((err = send(sockfd, query, strlen(query), 0)) < 0) {
		handle_error_en(err, "send: failed");
	}
}   

Buffer *new_buffer() {
	Buffer *buffer = malloc(sizeof(Buffer));
	if (buffer == NULL)
		handle_error("malloc: allocating buffer failed");
	buffer->data = malloc(BUF_SIZE);
	if (buffer->data == NULL)
		handle_error("malloc: allocating buffer->data failed");
	buffer->length = 0;
	return buffer;
}

// returns a buffer fulled with the content recieved from a connected socket 
Buffer *recieve_content(int sockfd) {
	Buffer *buffer = new_buffer();
	int bytes_recieved;
	
	// recieve data from socket into buffer
    while((bytes_recieved = recv(sockfd, buffer->data + buffer->length, BUF_SIZE, 0)) > 0) {
		buffer->length += bytes_recieved;
		buffer->data = realloc(buffer->data, buffer->length + BUF_SIZE); // resizing buffer
		if (bytes_recieved == -1) {
			handle_error_en(bytes_recieved, "recv: failed");
		}
    }
    buffer->data[buffer->length] = '\0';
	return buffer;
}

// returns a buffer fulled with the content from host:port/page
Buffer *http_query(char *host, char *page, int port) {
    int sockfd = client_socket(host, port); // Create connection on socket
	char *query = build_get_query(host, page); // create querry
	send_query(sockfd, query);
	free(query);
	Buffer *content = recieve_content(sockfd);	
	close(sockfd);
	return content;
}

// split http content from the response string
char *http_get_content(Buffer *response) {
    char *header_end = strstr(response->data, "\r\n\r\n");

    if (header_end) {
        return header_end + 4;
    }
    else {
        return response->data;
    }
}


Buffer *http_url(const char *url) {
    char host[BUF_SIZE];
    strncpy(host, url, BUF_SIZE);

    char *page = strstr(host, "/");
    if (page) {
        page[0] = '\0';

        ++page;
        return http_query(host, page, 80);
    }
    else {
        fprintf(stderr, "could not split url into host/page %s\n", url);
        return NULL;
    }
}
