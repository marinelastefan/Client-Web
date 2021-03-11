#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // scriu numele metodei, url, tipul protocolului si parametrii
   if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);


    // adaug hostul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaug token
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    
    // adaug cookie
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                           char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // scriu numele metodei, url si tipul protocolului
    memset(line, 0, LINELEN);
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // adaug hostul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaug content type si content length
    memset(line, 0, LINELEN);
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    // adaug tokenul
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // adaug cookie 
    if (cookies != NULL) {
        strcat(message,cookies);
    }

    compute_message(message, "");
    // adaug datele
    compute_message(message, body_data);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char *query_params,
                            char *cookies, char *token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // scriu numele metodei, url, parametrii si tipul protocolului
    if (query_params != NULL) {
        sprintf(line, "DELETE %s/%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // adaug hostul
    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaug tokenul
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    
    // adaug cookie
    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", cookies);
        compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}