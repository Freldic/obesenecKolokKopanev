#include "server.h"
#include "client.h"
#include "cstring"

int main(int argc, char * argv[]){
    Server *server = new Server();
    Client *client = new Client();
    char * argument[argc];
    argument[0] = argv[0];
    for(int i = 2; i<=argc; i++){
        argument[i-1] = argv[i];
    }

    if (strcmp(argv[1],"server") == 0){
        cout << "1. argument : " << argv[1] << endl;
        server->server(argc -1, argument);
    }

    if(strcmp(argv[1],"klient") == 0){

        cout << "1. argument : " << argv[1] << endl;
        client->client(argc -1, argument);

    } else {
        cout << "Nastala chyba pri zadávaní argumentu na strane klienta!" << endl;
        exit(1);
    }

    delete client;
    delete server;
}