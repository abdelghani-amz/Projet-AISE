#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
#include <fstream>
#include "json.hpp"

#ifndef COMMANDS_H
#define COMMANDS_H
using Db = std::unordered_map<std::string, std::string> ;
void get (Db& database, char* key, int client_fd);
void set(Db& database, char* key, char* value, int client_fd) ;
void setnx(Db& database, char* key, char* value, int client_fd);
void del (Db& database, char* key, int client_fd);
void exists(Db& database, char* key, int client_fd);
void strl(Db& database, char* key, int client_fd);
void append(Db& database, char* key, char* value, int client_fd);
void rename(Db& database, char* key, char* value, int client_fd);
void randkey(Db& database, int client_fd);
void keys(Db& database, int client_fd);
void values(Db& database, int client_fd);
void save(Db& database, char* key,int client_fd);
void load(Db& database, char* key,int client_fd);
#endif