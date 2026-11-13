#include "common.h"

int main(int argc, char *argv[])
{
    std::string ip = (argc > 1) ? argv[1] : "127.0.0.1";
    int port = (argc > 2) ? std::atoi(argv[2]) : 5000;

    if (!net_init()) {
        std::cout << "Erreur : initialisation reseau impossible\n";
        return 1;
    }

    // 1) Creer la socket
    socket_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == BAD_SOCKET) {
        std::cout << "Erreur : creation de la socket\n";
        return 1;
    }

    // 2) Adresse du serveur
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    for (int i = 0; i < 8; i++) addr.sin_zero[i] = 0;

    // 3) Se connecter
    std::cout << "[Client] Connexion a " << ip << ":" << port << "...\n";
    if (connect(s, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Erreur : impossible de se connecter. Le serveur est-il lance ?\n";
        return 1;
    }
    std::cout << "[Client] Connecte.\n";

    // 4) Discuter
    chat(s, "Client");

    CLOSE_SOCKET(s);
    net_cleanup();
    return 0;
}