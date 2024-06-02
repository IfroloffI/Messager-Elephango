//
// Created by Ilya on 02.06.2024.
//

#ifndef MESSAGER_ELEPHANGO_SERVER_H
#define MESSAGER_ELEPHANGO_SERVER_H

#include <SFML/Network.hpp>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <libpq-fe.h>

class Server {
public:
    Server(unsigned short port);

    ~Server();

    void start();

private:
    void handleClient(sf::TcpSocket *client);

    void broadcastMessage(const std::string &message, sf::TcpSocket *sender);

    void storeMessageInDB(const std::string &message, const std::string &sender);

    bool authenticateUser(const std::string &username, const std::string &password);

    sf::TcpListener listener;
    std::vector<sf::TcpSocket *> clients;
    std::mutex clientsMutex;
    bool running;
    std::thread *acceptThread;

    PGconn *dbConnection;
};

#endif //MESSAGER_ELEPHANGO_SERVER_H
