//
// Created by kopan on 25.12.2022.
//

#include "client.h"


int Client::client(int argc, char*  argv[]) {
    if(argc != 3) {
        cout << "Ojoj.. problém, nezadali ste správny počet argumentov, alebo ste nezadali správny formát argumentov... \n";
        cout << "Pomôcka : NICK-IP-PORT \n";
        exit(1);
    }

    int sockfd;
    string ipAddr = argv[1];
    char ip[ipAddr.size() + 1];
    strcpy(ip, ipAddr.c_str());
    unsigned short cisloPortu = atoi(argv[2]);

    // vytvorenie socketu
    sockfd  = socket(AF_INET, SOCK_STREAM, 0);

    // adresa socketu
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(cisloPortu);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    int status = connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if(status == -1){
        cout << ("Ojoj... Bohužiaľ sa spojenie z dákeho neznámeho dôvodu nepodarilo nadviazať...\n");
        close(sockfd);
        exit(1);
    }

    cout << ("============ | OBESENEC | ============ \n");

    cout << ("Vítaj! zadaj svoj nick : ");
    cin >> nick;

    posliSpravu1(sockfd, nick); // odoslanie 1
    spustiProgram1(sockfd, nick);
    close(sockfd);
    return 1;
}

void Client::posliSpravu1(int socket, string str) {
    char buffer[2000];
    bzero(buffer, 2001);
    strcpy(buffer, str.c_str());
    int status = send(socket, buffer, 2000, 0);
    if(status == -1) {
        cout << ("Ojoj... bohužiaľ sa nepodarilo odoslať správu serveru....\n");
        close(socket);
        exit(1);
    }
}

string Client::obdrzSpravu(int socket) {
    char buffer[2000];
    int bytesRem = 2000;
    bzero(buffer, 2001);
    while(bytesRem > 0) {
        int recvBytes = recv(socket, buffer, bytesRem, 0);
        if(recvBytes < 0) {
            cout << ("Ojoj... nepodarilo sa obdržať správu od serveru...\n");
            close(socket);
            exit(1);
        }
        bytesRem -= recvBytes;
    }
    return buffer;
}


void Client::spustiProgram1(int socket, string username) {

    int spravnost;
    int status;

    uint16_t converter;
    //Start
    int sockfd = socket;

    string hlaska = "";
    string slovo;
    string hadane = "";
    string pokus;
    string pocetChyb;
    string progress;

    bool uhadnute = false;

    while(!uhadnute){
        pokus = obdrzSpravu(sockfd);
        pocetChyb = obdrzSpravu(sockfd);
        progress = obdrzSpravu(sockfd);
        if(pocetChyb == "9") {
            cout << "\nPocet chyb: " << pocetChyb << "/9\n";
            cout << progress << "\n";
            cout << "Si prehral! =(";
        }
        slovo = obdrzSpravu(sockfd); // Obdrzanie 3
        cout << "\nPokus č." << pokus << "\n";
        cout << "\nPocet chyb: " << pocetChyb << "/9\n";
        cout << progress << "\n";
        cout << "Hladane slovo: " << slovo << "\n";

        do {
            cout << "Zadaj pismeno: ";
            cin >> hadane;
        } while(hadane.length() != 1 || isalpha(hadane[0]) == 0);


        posliSpravu1(sockfd, hadane); // odoslanie 3

        // See if entered guess is invalid
        status = recv(sockfd, &converter, sizeof(uint16_t), 0); // Obdrzanie 4
        if(status <= 0){
            cout << "Ojoj... Nastala chyba, neobdržali sme žiadne data...";
            close(sockfd);
            exit(1);
        }
        spravnost = ntohs(converter);

        if(spravnost != 1) {
            cout << "Ojoj, už si raz skúsil zadať toto písmenko... \nPokus ti nezapočítame....\n";
        }

        else {
            hlaska = obdrzSpravu(sockfd); // recv 6

            cout << hlaska << "\n";

            status = recv(sockfd, &converter, sizeof(uint16_t), 0); // recv 7
            if(status <= 0){
                cout << "Ojoj, neobdržali sme žiadne dáta....\n";
                close(sockfd);
                exit(1);
            }

            spravnost = ntohs(converter);

            // Koneic hry
            if(spravnost == 1) {
                uhadnute = true;
                hlaska = obdrzSpravu(sockfd); // obdrzanie 8
                cout << hlaska << endl;

                uhadnute = true;
            }
        }
    }
}


