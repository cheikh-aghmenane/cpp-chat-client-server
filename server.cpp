#include "common.h"

int main(int argc, char *argv[])
{
    int port = (argc > 1) ? std::atoi(argv[1]) : 5000;

    if (!net_init()) {
        std::cout << "Erreur : initialisation reseau impossible\n";
        return 1;
    }

    // 1) Creer la socket
    socket_t listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener == BAD_SOCKET) {
        std::cout << "Erreur : creation de la socket\n";
        return 1;
    }

    // Permet de relancer le serveur sans attendre sur le meme port
    int yes = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

    // 2) Associer la socket a un port (bind)
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    for (int i = 0; i < 8; i++) addr.sin_zero[i] = 0;

    if (bind(listener, (sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cout << "Erreur : le port " << port << " est deja utilise\n";
        return 1;
    }

    // 3) Ecouter
    listen(listener, 1);
    std::cout << "[Serveur] En attente de connexion sur le port " << port << "...\n";

    // 4) Accepter un client
    sockaddr_in clientAddr;
#ifdef _WIN32
    int len = sizeof(clientAddr);
#else
    socklen_t len = sizeof(clientAddr);
#endif
    socket_t client = accept(listener, (sockaddr *)&clientAddr, &len);
    if (client == BAD_SOCKET) {
        std::cout << "Erreur : accept\n";
        return 1;
    }
    std::cout << "[Serveur] Client connecte depuis " << inet_ntoa(clientAddr.sin_addr) << "\n";

    // 5) Discuter
    chat(client, "Serveur");

    CLOSE_SOCKET(client);
    CLOSE_SOCKET(listener);
    net_cleanup();
    return 0;
}