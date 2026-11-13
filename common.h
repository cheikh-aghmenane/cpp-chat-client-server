#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <cstdlib>

// ---------- Portabilite Windows / Linux ----------
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET socket_t;
    #define CLOSE_SOCKET closesocket
    #define BAD_SOCKET INVALID_SOCKET
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    typedef int socket_t;
    #define CLOSE_SOCKET close
    #define BAD_SOCKET (-1)
#endif

// Initialise la bibliotheque reseau (necessaire sous Windows uniquement).
inline bool net_init()
{
#ifdef _WIN32
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
#else
    return true;
#endif
}

inline void net_cleanup()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

// ---------- Reception des messages (dans un thread separe) ----------
// Le thread attend les messages de l'autre cote et les affiche.
// Pendant ce temps, le programme principal lit le clavier et envoie.
static void receive_loop(socket_t s)
{
    char buffer[1024];
    while (true) {
        int n = recv(s, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            std::cout << "\n[Connexion fermee par l'autre cote]\n";
            CLOSE_SOCKET(s);
            net_cleanup();
            std::exit(0);
        }
        buffer[n] = '\0';
        std::cout << "\r" << buffer << "Vous : " << std::flush;
    }
}

#ifdef _WIN32
static DWORD WINAPI thread_entry(LPVOID arg)
{
    receive_loop(*static_cast<socket_t *>(arg));
    return 0;
}
#else
static void *thread_entry(void *arg)
{
    receive_loop(*static_cast<socket_t *>(arg));
    return NULL;
}
#endif

inline void start_receiver(socket_t &s)
{
#ifdef _WIN32
    CreateThread(NULL, 0, thread_entry, &s, 0, NULL);
#else
    pthread_t t;
    pthread_create(&t, NULL, thread_entry, &s);
    pthread_detach(t);
#endif
}

// ---------- Boucle de discussion (identique pour serveur et client) ----------
inline void chat(socket_t s, const std::string &name)
{
    start_receiver(s);
    std::cout << "Discussion ouverte. Tapez /quit pour quitter.\n";

    std::string line;
    while (true) {
        std::cout << "Vous : " << std::flush;
        if (!std::getline(std::cin, line) || line == "/quit")
            break;
        if (line.empty())
            continue;
        std::string message = name + " : " + line + "\n";
        if (send(s, message.c_str(), (int)message.size(), 0) < 0)
            break;
    }
}

#endif