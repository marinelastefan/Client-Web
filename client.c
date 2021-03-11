#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int main(int argc, char *argv[]) {
	int sockfd;
	char *message;
	char *response;
	char command[100];
	char *find;
	char *cookie = NULL;
	char username[100];
	char password[100];
	char *auth_string = NULL;
	char token[1000];
	int wasLogged = 0;
	while(1) {
		// citesc comanda de la tastatura
		memset(command, 0, 100);
		scanf("%s", command);
		// daca e exit inchid programul
		if (strncmp(command, "exit", 4) == 0) {
			break;
		}
		// daca comanda este register
		if (strncmp(command, "register", 8) == 0) {
			// citesc de la tastatura username si parola
       		printf("username:");
       		scanf("%s", username);
       		printf("password:");
       		scanf("%s", password);
       		//creez jsonul de inregistrare
       		JSON_Value *value = json_value_init_object();
       		JSON_Object *object = json_value_get_object(value);
       		json_object_set_string(object, "username", username);
       		json_object_set_string(object, "password", password);
       		// il convertesc in string pentru a-l transmite ca parametru
       		// la compute_post_request
       		auth_string = json_serialize_to_string_pretty(value);
       		message = compute_post_request(
       			"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       			"/api/v1/tema/auth/register","application/json", 
       			auth_string, NULL, NULL);
       		sockfd = open_connection("3.8.116.10", 8080, 
       			AF_INET, SOCK_STREAM, 0);
       		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		close_connection(sockfd);
       		// in caz ca numele exista deja se va intoarce o eroare
       		find = basic_extract_json_response(response);
       		if (find) {
       			printf("%s\n",find );
       		}
       		json_free_serialized_string(auth_string);
       		json_value_free(value);

       	// comanda este login	
       	} else if (strncmp(command, "login", 5) == 0) {
       		printf("username:");
       		scanf("%s", username);
       		printf("password:");
       		scanf("%s", password);
       		// formez jsonul de autentificare
       		JSON_Value *value = json_value_init_object();
       		JSON_Object *object = json_value_get_object(value);
       		json_object_set_string(object, "username", username);
       		json_object_set_string(object, "password", password);
       		auth_string = json_serialize_to_string_pretty(value);
       		message = compute_post_request(
       			"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       			"/api/v1/tema/auth/login","application/json",
       			auth_string, NULL, NULL);
       		sockfd = open_connection("3.8.116.10", 8080, 
       			AF_INET, SOCK_STREAM, 0);
       		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		close_connection(sockfd);
    		// il marchez ca fiind logat
    		wasLogged = 1;
    		// in caz ca username ul sau parola sunt gresite se va intoarce o eroare
       		find = basic_extract_json_response(response);
       		if (find) {
       			printf("%s\n",find);
       		}
       		json_value_free(value);

			} else if (strncmp(command, "enter_library", 13) == 0) {
				//daca nu e logat nu are acces la librarie
				if (wasLogged == 0) {
					printf("You are not logged in\n");
				} else {
					// asigur conexiunea cu serverul
					message = compute_post_request(
						"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
	       				"/api/v1/tema/auth/login","application/json",
	       				auth_string, NULL, NULL);
					sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	       			send_to_server(sockfd, message);
	    			response = receive_from_server(sockfd);
	    			close_connection(sockfd);
	    			// salvez cookie ul
	    			char *aux = strstr(response, "connect.sid=");
	       			if (aux) {
		       			cookie = strtok(aux, ";");
	   
	       			}

				message = compute_get_request(
					"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
	       			"/api/v1/tema/library/access", NULL, cookie, NULL);
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
	       		send_to_server(sockfd, message);
	    		response = receive_from_server(sockfd);
	    		close_connection(sockfd);
	    		// retin tokenul
	    		char *token_json = basic_extract_json_response(response);
	    		char *token_aux = strtok(token_json, ":");
	    		memset(token, 0,sizeof(token));
	    		token_aux = strtok(NULL, ":");
	    		strncpy(token, token_aux + 1, strlen(token_aux) - 3);
	    	}

		} else if (strcmp(command,"get_books") == 0) {
				message = compute_get_request(
					"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       				"/api/v1/tema/library/books", NULL, NULL, token);
       			sockfd = open_connection("3.8.116.10", 8080, 
       				AF_INET, SOCK_STREAM, 0);
       			send_to_server(sockfd, message);
    			response = receive_from_server(sockfd);
    			close_connection(sockfd);
    			// extrag cartile pentru a le afisa
    			char *books = basic_extract_json_response(response);
    			if (books) {
    				printf("%s\n", books);
    			}
		} else if (strcmp(command, "add_book") == 0) {
			JSON_Value *value = json_value_init_object();
       		JSON_Object *object = json_value_get_object(value);
       		char title[100];
       		char author[100];
       		char genre[100];
       		char publisher[100];
       		char page_count[10];
       		char *book_string = NULL;
       		// obtin informatiile necesare adaugarii unei carti
       		printf("title:");
       		scanf("%s", title);
       		printf("author:");
       		scanf("%s", author);
       		printf("genre:");
       		scanf("%s", genre);
       		printf("publisher:");
       		scanf("%s", publisher);
       		printf("page_count:");
       		scanf("%s", page_count);
       		// formez jsonul
       		json_object_set_string(object, "title", title);
       		json_object_set_string(object, "author", author);
       		json_object_set_string(object, "genre", genre);
       		json_object_set_string(object, "publisher", publisher);
       		json_object_set_string(object, "page_count", page_count);
       		// il convertesc in string
       		book_string = json_serialize_to_string_pretty(value);
       		// adaug cartea
       		message = compute_post_request(
       			"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       			"/api/v1/tema/library/books","application/json",
       			book_string, NULL, token);
       		sockfd = open_connection("3.8.116.10", 8080, 
       			AF_INET, SOCK_STREAM, 0);
       		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		close_connection(sockfd);
    		json_value_free(value);
		} else if (strcmp(command, "logout") == 0) {
			// daca nu e logat nu are cum sa dea logout
			if (wasLogged == 0) {
				printf("You are not logged in\n");
			} else {
				message = compute_post_request(
					"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       				"/api/v1/tema/auth/login","application/json",
       				auth_string, NULL, NULL);
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, 
					SOCK_STREAM, 0);
       			send_to_server(sockfd, message);
    			response = receive_from_server(sockfd);
    			close_connection(sockfd);
    			// retin cookie ul
    			char *aux = strstr(response, "connect.sid=");
	       		if (aux) {
		       		cookie = strtok(aux, ";");
	   
	       		}
	       		
				message = compute_get_request(
					"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       				"/api/v1/tema/auth/logout", NULL, cookie, NULL);
				sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
       			send_to_server(sockfd, message);
    			response = receive_from_server(sockfd);
    			close_connection(sockfd);
    		}


        } else if (strcmp(command, "get_book") == 0){
			char id[100];
			char url[1000];
			printf("id:");
			scanf("%s", id);
			memset(url, 0, sizeof(url));
			strcpy(url, "/api/v1/tema/library/books/");
			strcat(url, id);
			message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       			url, NULL, NULL, token);
       		sockfd = open_connection("3.8.116.10", 8080, 
       			AF_INET, SOCK_STREAM, 0);
       		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		close_connection(sockfd);
    		char *books = basic_extract_json_response(response);
    		close_connection(sockfd);
    		printf("%s\n", books);
    		
		} else if (strcmp(command, "delete_book") == 0) {
			// obtin informatii legate de id-ul cartii pe care vreau sa o sterg
			char id[100];
			char url[1000];
			printf("id:");
			scanf("%s", id);
			memset(url, 0, sizeof(url));
			strcpy(url, "/api/v1/tema/library/books/");
			strcat(url, id);

			message = compute_delete_request(
				"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
       			url, NULL, NULL, token);
       		sockfd = open_connection("3.8.116.10", 8080, 
       			AF_INET, SOCK_STREAM, 0);
       		send_to_server(sockfd, message);
    		response = receive_from_server(sockfd);
    		close_connection(sockfd);
    		char *print = basic_extract_json_response(response);
    		// daca cartea nu exista id ul se va afisa un mesaj de eroare
    		if (print) {
    			printf("%s\n", print);
    		}

			
		}  else {
			printf("Wrong_command, try_again\n");
		}
		
    }
	close_connection(sockfd);
	return 0;
}