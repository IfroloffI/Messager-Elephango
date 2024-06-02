#include "server.h"
#include <iostream>

Server::Server(unsigned short port) : running(true) {
    if (listener.listen(port) != sf::Socket::Done) {
        std::cerr << "Error starting server on port " << port << std::endl;
    } else {
        std::cout << "Server started on port " << port << std::endl;
    }
    acceptThread = new std::thread(&Server::start, this);

    // Подключение к базе данных
    dbConnection = PQconnectdb(
            "user=postgres dbname=elephango password=123456 hostaddr=127.0.0.1 port=5432");
    if (PQstatus(dbConnection) != CONNECTION_OK) {
        std::cerr << "Error connecting to database: " << PQerrorMessage(dbConnection) << std::endl;
    }
}

Server::~Server() {
    running = false;
    listener.close();
    acceptThread->join();
    delete acceptThread;

    // Закрытие соединения с базой данных
    PQfinish(dbConnection);

    for (auto client: clients) {
        client->disconnect();
        delete client;
    }
}

void Server::start() {
    while (running) {
        auto *client = new sf::TcpSocket();
        if (listener.accept(*client) == sf::Socket::Done) {
            std::lock_guard<std::mutex> lock(clientsMutex);
            clients.push_back(client);
            std::thread(&Server::handleClient, this, client).detach();
        } else {
            delete client;
        }
    }
}

void Server::handleClient(sf::TcpSocket *client) {
    sf::Packet packet;
    std::string username, password;

    // Аутентификация пользователя
    if (client->receive(packet) == sf::Socket::Done) {
        packet >> username >> password;
        if (!authenticateUser(username, password)) {
            // Отправить сообщение об ошибке аутентификации
            sf::Packet responsePacket;
            responsePacket << "Authentication Failed";
            client->send(responsePacket);
            client->disconnect();
            delete client;
            return;
        }
    }

    while (client->receive(packet) == sf::Socket::Done) {
        std::string message;
        packet >> message;
        broadcastMessage(message, client);
        storeMessageInDB(message, username);
        packet.clear();
    }

    std::lock_guard<std::mutex> lock(clientsMutex);
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    client->disconnect();
    delete client;
}

void Server::broadcastMessage(const std::string &message, sf::TcpSocket *sender) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    for (auto client: clients) {
        if (client != sender) {
            sf::Packet packet;
            packet << message;
            client->send(packet);
        }
    }
}

void Server::storeMessageInDB(const std::string &message, const std::string &sender) {
    std::string query = "INSERT INTO messages (sender, message) VALUES ('" + sender + "', '" + message + "')";
    PGresult *res = PQexec(dbConnection, query.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Error inserting message into database: " << PQerrorMessage(dbConnection) << std::endl;
    }
    PQclear(res);
}

bool Server::authenticateUser(const std::string &username, const std::string &password) {
    std::string query = "SELECT password FROM users WHERE username = '" + username + "'";
    PGresult *res = PQexec(dbConnection, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Error querying database: " << PQerrorMessage(dbConnection) << std::endl;
        PQclear(res);
        return false;
    }

    if (PQntuples(res) == 0) {
        // Пользователь не найден, добавить нового пользователя
        std::string insertQuery =
                "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')";
        PGresult *insertRes = PQexec(dbConnection, insertQuery.c_str());
        if (PQresultStatus(insertRes) != PGRES_COMMAND_OK) {
            std::cerr << "Error inserting new user into database: " << PQerrorMessage(dbConnection) << std::endl;
            PQclear(insertRes);
            PQclear(res);
            return false;
        }
        PQclear(insertRes);
        PQclear(res);
        return true;
    }

    std::string storedPassword = PQgetvalue(res, 0, 0);
    PQclear(res);
    return storedPassword == password;
}
