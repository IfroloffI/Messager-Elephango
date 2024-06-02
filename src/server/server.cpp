//
// Created by Ilya on 02.06.2024.
//

#include "server.h"
#include <iostream>

Server::Server(unsigned short port) : running(true) {
    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error starting server on port " << port << std::endl;
    } else {
        std::cout << "Server started on port " << port << std::endl;
    }
    acceptThread = new std::thread(&Server::start, this);
}

Server::~Server() {
    running = false;
    listener.close();
    acceptThread->join();
    delete acceptThread;
    for (auto client : clients) {
        client->disconnect();
        delete client;
    }
}

void Server::start() {
    while (running) {
        auto* client = new sf::TcpSocket();
        if (listener.accept(client) == sf::Socket::Done) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(client);
            std::thread(&Server::handleClient, this, client).detach();
        } else {
            delete client;
        }
    }
}

void Server::handleClient(sf::TcpSocket* client) {
    sf::Packet packet;
    while (client->receive(packet) == sf::Socket::Done) {
        std::string message;
        packet >> message;
        broadcastMessage(message, client);
        packet.clear();
    }
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    client->disconnect();
    delete client;
}

void Server::broadcastMessage(const std::string& message, sf::TcpSocket* sender) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto client : clients) {
        if (client != sender) {
            sf::Packet packet;
            packet << message;
            client->send(packet);
        }
    }
}
