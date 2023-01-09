//
// Created by kopan on 25.12.2022.
//

#ifndef SEMCPP2_GAME_CLIENT_H
#define SEMCPP2_GAME_CLIENT_H

#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

class Client {
private:
    string nick;

public:
    int client(int argc, char *argv[]);
    void posliSpravu1(int socket, string str);
    string obdrzSpravu(int socket);
    void spustiProgram1(int socket, string username);
};

#endif //SEMCPP2_GAME_CLIENT_H
