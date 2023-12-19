#include <regex.h>
#include <iostream>
#include "commands.h"
static int count = 0 ;
#define MAX_BUFFER_SIZE 1024

static std::unordered_map<std::string, std::string> database{} ;
static int server_sock ;
static const char* one_opt = "^([A-Z]+)\\s+([A-Za-z0-9_]+)\\s?$" ;
static const char* two_opts = "^([A-Z]+)\\s+([A-Za-z0-9_]+)\\s+([A-Za-z0-9_]+)\\s?$" ;


struct th_info
{
	int fd;
    char response[MAX_BUFFER_SIZE];
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void * handle_client(void * pctx)
{
	struct th_info * ctx = (struct th_info *) pctx;
	int client_fd = ctx->fd;
    char* response = (ctx->response);

    regex_t regex1, regex2;
    if (regcomp(&regex1, one_opt, REG_EXTENDED) != 0 || regcomp(&regex2, two_opts, REG_EXTENDED) != 0) {
        send(client_fd, "Error compiling regular expression\n", 36, 0) ;
        close(client_fd) ;
        free(ctx) ;
        count-- ;
        pthread_exit(NULL);
    }

    while(true){
        memset(response, '\0', MAX_BUFFER_SIZE) ;
        read(client_fd, response, MAX_BUFFER_SIZE);
        regmatch_t matches[4];

        char action[10] = {'\0'} ;
        char key[30] = {'\0'} ;
        char value[30] =  {'\0'} ;

        pthread_mutex_lock(&mutex);
        if (regexec(&regex2, response, 4, matches, 0) == 0){
            strncpy(action, response + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
            strncpy(key, response + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);
            strncpy(value, response + matches[3].rm_so, matches[3].rm_eo - matches[3].rm_so);

            if(strcmp(action, "SET") == 0){
                set(database, key, value, client_fd);
            }
            else if(strcmp(action, "SETNX") == 0){
                setnx(database, key, value, client_fd);
            }
            else if(strcmp(action, "APPEND") == 0){
                append(database, key, value, client_fd);
            }

            else if(strcmp(action, "RENAME") == 0){
                rename(database, key, value, client_fd) ;
            }
            else
                send(client_fd, "Invalid command format\n", 24, 0) ;
        }
        else if (regexec(&regex1, response, 3, matches, 0) == 0) {
            strncpy(action, response + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
            strncpy(key, response + matches[2].rm_so, matches[2].rm_eo - matches[2].rm_so);

            if(strcmp(action, "GET") == 0){
                get(database, key, client_fd) ;
            }
            else if((strcmp(action, "PING") == 0) || (strcmp(action, "ECHO") == 0)){
                send(client_fd, key, strlen(key), 0) ;
                send(client_fd, "\n", 1, 0) ;
            }
            else if(strcmp(action, "DEL") == 0){
                del(database, key, client_fd) ;
            }
            else if(strcmp(action, "EXISTS") == 0){
                exists(database, key, client_fd) ;
            }
            else if (strcmp(action, "STRLEN") == 0){
                strl(database, key, client_fd) ;
            }
            else if(strcmp(action, "SAVE") == 0){
                save(database, key, client_fd) ;
            }
            else if(strcmp(action, "LOAD") == 0){
                load(database, key, client_fd) ;
            }
            else
                send(client_fd, "Invalid command format\n", 24, 0) ;
        }
        else{
            // If the command doesn't match the regular expression, check for PING and QUIT command
            if (strncmp(response, "PING", 4) == 0) {
                send(client_fd, "PONG\n", 6, 0) ;
            } 

            else if (strncmp(response, "KEYS", 4) == 0){
                keys(database, client_fd) ;
            }
            else if (strncmp(response, "VALUES", 4) == 0){
                values(database, client_fd) ;
            }

            else if (strncmp(response, "RANDOMKEY", 4) == 0){
                randkey(database, client_fd) ;
            }

            else if (strncmp(response, "QUIT", 4) == 0){
                printf("Closing connexion with client %d\n", client_fd) ;
                send(client_fd, "Closing connexion with server\n", 31, 0) ;
                close(client_fd) ;
                free(ctx) ;
                regfree(&regex1);
                regfree(&regex2);
                count-- ;
                pthread_mutex_unlock(&mutex) ;
                pthread_exit(NULL) ;
            }
            else if (strncmp(response, "SAVE", 4) == 0){
            }
            else {
                send(client_fd, "Invalid command format\n", 24, 0) ;
            }

        }

        pthread_mutex_unlock(&mutex) ;

    }
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    struct addrinfo * result = NULL;

    int ret = getaddrinfo(NULL, argv[1], &hints, &result);


    if(ret < 0)
    {
        herror("getaddrinfo");
        return 1;
    }

    server_sock = -1;
    int server_ready = 0;

    struct addrinfo *tmp;
    for(tmp = result; tmp != NULL; tmp=tmp->ai_next)
    {
        server_sock = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);

        if(server_sock < 0)
        {
            perror("socket");
            continue;
        }

        if( bind(server_sock, tmp->ai_addr, tmp->ai_addrlen) < 0)
        {
            perror("bind");
            continue;
        }

        if( listen(server_sock, 5) < 0)
        {
            perror("listen");
            continue;
        }

        server_ready = 1;
        break;
    }

    if(server_ready == 0)
    {
        fprintf(stderr, "PAS SERVER READY :-'(");
        return 1;
    }


    int client_fd = -1;

    struct sockaddr client_addr;
    socklen_t client_addr_len;

    int i = 0;

    while(true)
    {   
        client_fd = accept(server_sock, &client_addr, &client_addr_len);
        count++ ;
        if(client_fd < 0)
        {
            perror("accept");
            break;
        }
	
        struct th_info * ctx = (struct th_info *)malloc(sizeof(struct th_info));
        if(ctx == NULL)
        {
            perror("malloc");
        }
        ctx->fd = client_fd;

        pthread_t th;
        pthread_create(&th, NULL, handle_client, (void*)ctx);   
    }

    close(server_sock);

    return 0;
}
