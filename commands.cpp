#include "commands.h"

using Db = std::unordered_map<std::string, std::string> ;
void get (Db& database, char* key, int client_fd){
    
    if (database.find(key) != database.end()){
        send(client_fd, database[key].c_str(), database[key].size() + 1, 0) ;
        send(client_fd, "\n", 2, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void set(Db& database, char* key, char* value, int client_fd) {
    database.insert(std::make_pair(key, value)) ;
    send(client_fd, "OK\n", 4, 0) ;
}

void setnx(Db& database, char* key, char* value, int client_fd) {
    if (database.find(key) == database.end()){
        database.insert(std::make_pair(key, value)) ;
        send(client_fd, "OK\n", 4, 0) ;
    }
    else
        send(client_fd, "Key already exists\n", 20, 0) ;
}


void getset(Db& database, char* key, char* value, int client_fd) {
    if (database.find(key) != database.end()){
        send(client_fd, database[key].c_str(), database[key].size() + 1, 0) ;
        send(client_fd, "\n", 2, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
    
    database.insert(std::make_pair(key, value)) ;
}

void del (Db& database, char* key, int client_fd){
    if (database.find(key) != database.end()){
        if (database.erase(key) > 0) 
            send(client_fd, "OK\n", 4, 0) ;
        else
            send(client_fd, "Error while deleting\n", 22, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void exists(Db& database, char* key, int client_fd){
    if (database.find(key) != database.end())
        send(client_fd, "OK\n", 4, 0) ;
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void strl(Db& database, char* key, int client_fd){
    char value[30] ;
    if (database.find(key) != database.end()){
        sprintf(value, "%lu", database[key].size()) ;
        send(client_fd, value, strlen(value), 0) ;
        send(client_fd, "\n", 2, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void append(Db& database, char* key, char* value, int client_fd){
    if (database.find(key) != database.end()){
        database[key] += value ;
        send(client_fd, "OK\n", 4, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void rename(Db& database, char* key, char* value, int client_fd){
    if (database.find(key) != database.end()){
        auto temp = database[key] ;
        database.erase(key);
        database[value] = temp ;
        send(client_fd, "OK\n", 4, 0) ;
    }
    else
        send(client_fd, "Key not found in database\n", 27, 0) ;
}

void randkey(Db& database, int client_fd){
    int element = rand() % database.size() ;
    auto iterator = database.begin();
    
    for(auto i = 0 ; i < element ; ++i){
        iterator++ ;
    }

    auto key = iterator->first;
    send(client_fd, key.c_str(), key.size() + 1, 0) ;
}

void keys(Db& database, int client_fd){
    for(auto& [key, _] : database){
        send(client_fd, key.c_str(), key.size() + 1, 0) ;
        send(client_fd, "\n", 1, 0) ;
    }
}

void values(Db& database, int client_fd){
    for(auto& [_, val] : database){
        send(client_fd, val.c_str(), val.size() + 1, 0) ;
        send(client_fd, "\n", 1, 0) ;
    }
}

void save(Db& database, char* key,int client_fd){
    nlohmann::json jsonMap;
    for (const auto& pair : database) {
        jsonMap[pair.first] = pair.second;
    }
    std::ofstream file(key);
    file << jsonMap.dump(4);  

    file.close();

}

void load(Db& database, char* key,int client_fd){
    std::ifstream inputFile(key);

    if (!inputFile.is_open()){
        send(client_fd, "File not found\n", 16, 0) ;
        return;
    }
        
    nlohmann::json jsonData;
    inputFile >> jsonData;
    inputFile.close();

    if (!jsonData.is_object()) {
        send(client_fd, "Invalid file\n", 14, 0) ;
        return;
    }

    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        database[it.key()] = it.value();
    }
}