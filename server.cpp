//
// Created by kopan on 25.12.2022.
//

#include "server.h"

void* Server::spustiProgram_wrapper(void* object)
{
    reinterpret_cast<Server*>(object)->spustiProgram(object);
    return 0;
}

int Server::server(int argc,char* argv[]) {
    if(argc != 2) {
        cout << "Ojoj.. problém, nezadali ste správny počet argumentov, alebo ste nezadali správny formát argumentov... \n";
        cout << "Pomôcka : IP-PORT \n";
        exit(1);
    }

    unsigned short portNum = atoi(argv[1]);

    // VYtvorenie sockuetu server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //bindnutie socketu na IP a port
    int status = bind(sockfd, (struct sockaddr*) &serverAddr, sizeof(serverAddr));
    if(status == -1) {
        cout << "Ojoj, ako si sa nám nepodarilo bindnúť server...\nSkusíli ste zadať iný port?\n";
        exit(1);
    }

    listen(sockfd, 128);
    cout << ("Server je pripravený, čaká sa až sa naň napoja hráči.... \n");

    //vytvorenie socketu servera
    struct sockaddr_in adressaKlienta;
    pthread_t vlakno;

    while(1) {
        socklen_t dlzkaAdresy = sizeof(adressaKlienta);
        int clientSock = accept(sockfd, (struct sockaddr*)&adressaKlienta, &dlzkaAdresy);
        if(clientSock == -1) {
            cout << ("Ojoj... Nastala chyba... Nepodarilo sa klientovi napojiť....");
            close(clientSock);
            exit(1);
        }
        cout << "Pripojenie úspešné!" << endl;

        // Vlákno
        status = pthread_create (&vlakno, NULL, spustiProgram_wrapper, (void*) &clientSock);
        if(status != 0) {
            cout << ("Ojoj, nastala chyba... nepodarilo sa vytvoriť vlákno! \n");
            close(clientSock);
            exit(1);
        }
    }
}

void Server::odosliSpravu(int socket, string str) {
    char buffer[2000];
    bzero(buffer, 2001);
    strcpy(buffer, str.c_str());
    int status = send(socket, buffer, 2000, 0);
    if(status == -1) {
        cout << "Ojoj, nepodarila sa odoslať správa...\n";
        close(socket);
        exit(1);
    }
}

string Server::hangman(int n){
    switch (n) {
        case 9: return ("  __ |\n |   | |\n |   O |\n |  /|\\|\n_|_ / \\|  "); break;
        case 8: return ("  __ |\n |   | |\n |   O |\n |  /|\\|\n_|_ /  |  "); break;
        case 7: return ("  __ |\n |   | |\n |   O |\n |  /|\\|\n_|_    |  "); break;
        case 6: return ("  __ |\n |   | |\n |   O |\n |  /| |\n_|_    |  "); break;
        case 5: return ("  __ |\n |   | |\n |   O |\n |   | |\n_|_    |  "); break;
        case 4: return ("  __ |\n |   | |\n |   O |\n |     |\n_|_    |  "); break;
        case 3: return ("  __ |\n |     |\n |     |\n |     |\n_|_    |  "); break;
        case 2: return ("       |\n |     |\n |     |\n |     |\n_|_    |  "); break;
        case 1: return ("       |\n       |\n       |\n       |\n___    |  "); break;
        case 0: return ("       |\n       |\n       |\n       |\n       |  "); break;
    }
    return NULL;
}

string Server::dostanSpravu(int socket) {
    char buffer[2000];
    int bytesRem = 2000;
    bzero(buffer, 2001);
    while(bytesRem > 0) {
        int recvBytes = recv(socket, buffer, bytesRem, 0);
        if(recvBytes < 0) {
            cout << ("Ojoj, niekde nastala chyba, nepodarilo sa obdržať dáta !\n");
            close(socket);
            exit(1);
        }
        bytesRem -= recvBytes;
    }
    return buffer;
}

string Server::getniSlovo() {
    srand(time(NULL));
    ifstream MyReadFile("/home/kopanev/words.txt");
    string word = "";
    int index = rand() % 15;
    if (MyReadFile) {
        for (int i = 1; i < index; i++)
            MyReadFile >> word;
    }
    MyReadFile.close();
    return word;
}

void* Server::spustiProgram(void* socket) {

    int clientSock = *(int *) socket;
    bool hotovo = false;
    int pocetChyb = 0;
    int aktualnePismenko = 0;
    string pismena[2000];
    bool uhadnutePismeno;
    bool spravnost;
    string hlaska = "";

    string postupPouzivatela = "";  // výpis _ _ _ _ ako pomôcka
    string postup = "";
    int pocitadloPostupu = 0;

    string nick = dostanSpravu(clientSock); // obdrzanie 1 pre getnutie nicku hráča
    cout << "Hráč: " << nick << " sa úspešne pripojil!\n";

    string slovicko = this->getniSlovo();
    int dlzkaSlova = slovicko.length();

    cout << "Hádané slovíčko je: " << slovicko << " pre používateľa : " << nick << "\n";

    for (int i = 0; i < dlzkaSlova; i++) {
        postupPouzivatela += "-";
    }

    int kolo = 1;
    while (!hotovo) {
        uhadnutePismeno = false;
        postup = hangman(pocetChyb);
        odosliSpravu(clientSock, to_string(kolo)); // odoslanie 2
        odosliSpravu(clientSock, to_string(pocetChyb)); // odoslanie 2.1
        odosliSpravu(clientSock, postup); // odoslanie 2.2
        odosliSpravu(clientSock, postupPouzivatela); // odoslanie 3

        string hadaj = dostanSpravu(clientSock); // obdrzanie 4

        cout << "Hráč: " << nick << " háda, že by to mohlo byť písmenko: " << hadaj << endl;

        if ((hadaj.length() != 1) || (isalpha(hadaj[0]) == 0)) {
            cout << "Ojoj, hráč sa pomýlil...\n";
            pthread_detach(pthread_self());
            close(clientSock);
            exit(1);
        }

        hadaj[0] = toupper(hadaj[0]);

        // kontrola, že či je hádané písmeno uhádnuté
        for (int i = 0; i < aktualnePismenko; i++) {
            if (hadaj == pismena[i]) {
                cout << nick << " uhádol písmenko!!!" << endl;
                uhadnutePismeno = true;
            }
        }

        if (uhadnutePismeno) {
            int platnost = 0;
            int konvertor = htons(platnost);
            int status = send(clientSock, &konvertor, sizeof(uint16_t), 0); // odoslanie 5
            if (status <= 0) {
                cout << "Ojoj, nastala neznáma chyba !\n";
                pthread_detach(pthread_self());
                close(clientSock);
                exit(1);
            }
        } else {
            pismena[aktualnePismenko] = hadaj;
            aktualnePismenko++;
            spravnost = false;
            int platnost = 1;
            int konvertor = htons(platnost);
            int status = send(clientSock, &konvertor, sizeof(uint16_t), 0); // odolsanie 5
            if (status <= 0) {
                cout << "Ojoj, nastala neznáma chyba !\n";
                pthread_detach(pthread_self());
                close(clientSock);
                exit(1);
            }

            //pozerá sa, že či je uhádnutie v skole
            for (int i = 0; i < dlzkaSlova; i++) {
                if (hadaj[0] == slovicko[i]) {
                    postupPouzivatela[i] = hadaj[0];
                    spravnost = true;
                    pocitadloPostupu++;
                }
            }
            if (spravnost) {
                hlaska = "Správne!";
            } else {
                hlaska = "Nesprávne!";
                pocetChyb++;
            }

            odosliSpravu(clientSock, hlaska); // odoslanie 6

            if (pocitadloPostupu == dlzkaSlova || pocetChyb == 9) {

                hotovo = true;
                platnost = 1;
                konvertor = htons(platnost);
                status = send(clientSock, &konvertor, sizeof(uint16_t), 0); // odoslanie 7
                if (status <= 0) {
                    cout << "Ojoj, nastala neznáma chyba !\n";
                    pthread_detach(pthread_self());
                    close(clientSock);
                    exit(1);
                }

                if (pocetChyb == 9) {
                    cout << "Hrac " << nick << " prehral!" << endl;
                    hlaska = "\nPrehral si\n" + hangman(9) + "\n\nŠťastie môžeš skúsiť nabudúce!\nHľadané slovo je " + slovicko + "\n";
                    odosliSpravu(clientSock, hlaska); // odolsanie 8
                } else {

                    hlaska = "\nGRATULUJEME!!! Uhádol si slovo " + slovicko + "!!\n" + "počet pokusov: " +
                             to_string(kolo);
                    odosliSpravu(clientSock, hlaska); // odolsanie 8


                    int score = (float) kolo / (float) dlzkaSlova;
                    score = floor(score * 100.00 + 0.5) / 100.00;

                    stringstream stream;

                    stream << fixed << setprecision(2) << score;
                    string s = stream.str();

                    cout << "Skóre hráča " << nick << " je: " << s << "\n";
                }
            } else {
                platnost = 0;
                konvertor = htons(platnost);
                status = send(clientSock, &konvertor, sizeof(uint16_t), 0); // odoslanie 7
                if (status <= 0) {
                    cout << "Ojoj, nastala neznáma chyba !\n";
                    pthread_detach(pthread_self());
                    close(clientSock);
                    exit(1);
                }
            }
            kolo++;
        }
    }
    // }
    pthread_detach(pthread_self());
    close(clientSock);
    return NULL;
}

